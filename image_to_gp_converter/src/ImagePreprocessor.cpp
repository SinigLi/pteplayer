#include "ImagePreprocessor.h"
#include "RecognitionException.h"
#include <algorithm>
#include <cmath>

namespace ImageToGP {

ImagePreprocessor::ImagePreprocessor() {
}

ImagePreprocessor::~ImagePreprocessor() {
}

cv::Mat ImagePreprocessor::preprocessImage(const cv::Mat& input) {
    try {
        cv::Mat processed = input.clone();
        
        // 基础预处理流程
        processed = cleanImage(processed);
        processed = binarizeImage(processed);
        processed = correctSkew(processed);
        
        return processed;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "Image preprocessing failed", e.what());
    }
}

std::vector<TrackRegion> ImagePreprocessor::detectTrackRegions(const cv::Mat& image) {
    std::vector<TrackRegion> regions;
    
    try {
        // 首先评估图像质量
        double quality = assessImageQuality(image);
        
        // 如果图像质量太低，先进行自动调整
        cv::Mat processedImage = image;
        if (quality < 30.0) {
            processedImage = autoAdjustImage(image);
        }
        
        // 检测六线谱区域
        auto tabRegions = detectTablatureRegions(processedImage);
        for (size_t i = 0; i < tabRegions.size(); ++i) {
            TrackRegion region(tabRegions[i], ScoreType::Tablature, static_cast<int>(i));
            regions.push_back(region);
        }
        
        // 检测简谱区域
        auto numRegions = detectNumberedRegions(processedImage);
        for (size_t i = 0; i < numRegions.size(); ++i) {
            TrackRegion region(numRegions[i], ScoreType::Numbered, 
                             static_cast<int>(tabRegions.size() + i));
            regions.push_back(region);
        }
        
        // 检测可能的五线谱区域（作为备选）
        auto staffRegions = detectStaffRegions(processedImage);
        for (size_t i = 0; i < staffRegions.size(); ++i) {
            TrackRegion region(staffRegions[i], ScoreType::Staff, 
                             static_cast<int>(tabRegions.size() + numRegions.size() + i));
            regions.push_back(region);
        }
        
        // 去除重叠的区域，优先保留置信度高的区域
        regions = removeOverlappingRegions(regions);
        
        // 按y坐标排序最终的区域列表
        std::sort(regions.begin(), regions.end(), 
                 [](const TrackRegion& a, const TrackRegion& b) {
                     return a.region.y < b.region.y;
                 });
        
        // 重新分配trackIndex
        for (size_t i = 0; i < regions.size(); ++i) {
            regions[i].trackIndex = static_cast<int>(i);
        }
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "Track region detection failed", e.what());
    }
    
    return regions;
}

cv::Mat ImagePreprocessor::extractTrackRegion(const cv::Mat& image, const cv::Rect& region) {
    try {
        // 确保区域在图像范围内
        cv::Rect safeRegion = region & cv::Rect(0, 0, image.cols, image.rows);
        return image(safeRegion).clone();
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "Track region extraction failed", e.what());
    }
}

cv::Mat ImagePreprocessor::cleanImage(const cv::Mat& input) {
    cv::Mat cleaned = input.clone();
    
    try {
        // 1. 去噪处理 - 使用双边滤波保持边缘
        cv::Mat denoised;
        cv::bilateralFilter(cleaned, denoised, 9, 75, 75);
        
        // 2. 形态学操作去除小噪点
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
        cv::Mat opened;
        cv::morphologyEx(denoised, opened, cv::MORPH_OPEN, kernel);
        
        // 3. 对比度增强
        cv::Mat enhanced;
        opened.convertTo(enhanced, -1, 1.2, 10); // alpha=1.2 (对比度), beta=10 (亮度)
        
        // 4. 最后用中值滤波去除剩余噪点
        cv::medianBlur(enhanced, cleaned, 3);
        
    } catch (const cv::Exception& e) {
        // 如果高级处理失败，回退到简单的中值滤波
        cv::medianBlur(input, cleaned, 3);
    }
    
    return cleaned;
}

cv::Mat ImagePreprocessor::binarizeImage(const cv::Mat& input) {
    cv::Mat gray, binary;
    
    try {
        // 转换为灰度图
        if (input.channels() > 1) {
            cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = input.clone();
        }
        
        // 尝试多种二值化方法，选择最佳结果
        cv::Mat binary1, binary2, binary3;
        
        // 方法1: 自适应阈值 (Gaussian)
        cv::adaptiveThreshold(gray, binary1, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                             cv::THRESH_BINARY, 15, 10);
        
        // 方法2: 自适应阈值 (Mean)
        cv::adaptiveThreshold(gray, binary2, 255, cv::ADAPTIVE_THRESH_MEAN_C, 
                             cv::THRESH_BINARY, 15, 10);
        
        // 方法3: Otsu阈值
        cv::threshold(gray, binary3, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        
        // 评估哪种方法效果最好（基于边缘数量）
        std::vector<cv::Vec4i> edges1, edges2, edges3;
        cv::HoughLinesP(binary1, edges1, 1, CV_PI/180, 50, 30, 10);
        cv::HoughLinesP(binary2, edges2, 1, CV_PI/180, 50, 30, 10);
        cv::HoughLinesP(binary3, edges3, 1, CV_PI/180, 50, 30, 10);
        
        // 选择检测到最多线条的方法
        if (edges1.size() >= edges2.size() && edges1.size() >= edges3.size()) {
            binary = binary1;
        } else if (edges2.size() >= edges3.size()) {
            binary = binary2;
        } else {
            binary = binary3;
        }
        
        // 后处理：去除小的连通组件
        cv::Mat labels, stats, centroids;
        int numComponents = cv::connectedComponentsWithStats(binary, labels, stats, centroids);
        
        cv::Mat filtered = cv::Mat::zeros(binary.size(), CV_8UC1);
        for (int i = 1; i < numComponents; i++) {
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            if (area > 50) { // 只保留面积大于50的组件
                cv::Mat mask = (labels == i);
                filtered.setTo(255, mask);
            }
        }
        
        binary = filtered;
        
    } catch (const cv::Exception& e) {
        // 如果高级处理失败，回退到简单的自适应阈值
        if (input.channels() > 1) {
            cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = input.clone();
        }
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                             cv::THRESH_BINARY, 11, 2);
    }
    
    return binary;
}

cv::Mat ImagePreprocessor::correctSkew(const cv::Mat& input) {
    try {
        cv::Mat gray;
        if (input.channels() > 1) {
            cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = input.clone();
        }
        
        // 检测线条来确定倾斜角度
        std::vector<cv::Vec4i> lines = detectLines(gray);
        
        if (lines.empty()) {
            return input.clone(); // 没有检测到线条，不进行校正
        }
        
        // 计算所有水平线的角度
        std::vector<double> angles;
        for (const auto& line : lines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 计算线条长度
            double length = std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            
            // 只考虑较长的线条（可能是谱线）
            if (length > input.cols * 0.3) {
                double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
                
                // 只考虑接近水平的线条（-30度到30度）
                if (std::abs(angle) < 30.0) {
                    angles.push_back(angle);
                }
            }
        }
        
        if (angles.empty()) {
            return input.clone(); // 没有合适的线条，不进行校正
        }
        
        // 计算角度的中位数作为倾斜角度
        std::sort(angles.begin(), angles.end());
        double skewAngle = angles[angles.size() / 2];
        
        // 如果倾斜角度很小，不需要校正
        if (std::abs(skewAngle) < 0.5) {
            return input.clone();
        }
        
        // 执行旋转校正
        cv::Point2f center(input.cols / 2.0, input.rows / 2.0);
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, -skewAngle, 1.0);
        
        cv::Mat corrected;
        cv::warpAffine(input, corrected, rotationMatrix, input.size(), 
                      cv::INTER_CUBIC, cv::BORDER_REPLICATE);
        
        return corrected;
        
    } catch (const cv::Exception& e) {
        // 如果校正失败，返回原图
        return input.clone();
    }
}

std::vector<cv::Rect> ImagePreprocessor::detectTablatureRegions(const cv::Mat& image) {
    std::vector<cv::Rect> regions;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 检测水平线条
        std::vector<cv::Vec4i> lines = detectLines(gray);
        
        if (lines.size() < 3) {
            return regions; // 至少需要3条线才可能是谱表
        }
        
        // 按y坐标对线条进行分组，寻找六线谱模式
        std::vector<std::vector<cv::Vec4i>> lineGroups;
        std::vector<bool> used(lines.size(), false);
        
        for (size_t i = 0; i < lines.size(); i++) {
            if (used[i]) continue;
            
            std::vector<cv::Vec4i> group;
            group.push_back(lines[i]);
            used[i] = true;
            
            int baseY = (lines[i][1] + lines[i][3]) / 2;
            
            // 寻找与当前线条y坐标接近的其他线条
            for (size_t j = i + 1; j < lines.size(); j++) {
                if (used[j]) continue;
                
                int currentY = (lines[j][1] + lines[j][3]) / 2;
                
                // 如果在合理的六线谱间距范围内（通常10-30像素）
                if (std::abs(currentY - baseY) < 150 && group.size() < 6) {
                    group.push_back(lines[j]);
                    used[j] = true;
                }
            }
            
            // 如果找到了4-6条线，可能是六线谱
            if (group.size() >= 4 && group.size() <= 6) {
                lineGroups.push_back(group);
            }
        }
        
        // 为每个线条组创建区域
        for (const auto& group : lineGroups) {
            if (group.empty()) continue;
            
            // 计算边界框
            int minX = image.cols, maxX = 0;
            int minY = image.rows, maxY = 0;
            
            for (const auto& line : group) {
                minX = std::min(minX, std::min(line[0], line[2]));
                maxX = std::max(maxX, std::max(line[0], line[2]));
                minY = std::min(minY, std::min(line[1], line[3]));
                maxY = std::max(maxY, std::max(line[1], line[3]));
            }
            
            // 扩展区域以包含音符
            int padding = 30;
            minX = std::max(0, minX - padding);
            maxX = std::min(image.cols, maxX + padding);
            minY = std::max(0, minY - padding);
            maxY = std::min(image.rows, maxY + padding);
            
            cv::Rect region(minX, minY, maxX - minX, maxY - minY);
            
            // 验证区域的合理性
            if (region.width > image.cols * 0.3 && region.height > 50 && region.height < 200) {
                regions.push_back(region);
            }
        }
        
        // 按y坐标排序区域
        std::sort(regions.begin(), regions.end(), 
                 [](const cv::Rect& a, const cv::Rect& b) {
                     return a.y < b.y;
                 });
        
    } catch (const cv::Exception& e) {
        // 如果检测失败，返回空列表
        regions.clear();
    }
    
    return regions;
}

std::vector<cv::Rect> ImagePreprocessor::detectNumberedRegions(const cv::Mat& image) {
    std::vector<cv::Rect> regions;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 二值化图像以便检测数字
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                             cv::THRESH_BINARY, 15, 10);
        
        // 使用形态学操作连接相近的数字
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 5));
        cv::Mat dilated;
        cv::dilate(binary, dilated, kernel);
        
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(dilated, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        // 分析轮廓以找到可能的简谱区域
        for (const auto& contour : contours) {
            cv::Rect bbox = cv::boundingRect(contour);
            
            // 简谱区域的特征：
            // 1. 宽度相对较大（包含多个数字）
            // 2. 高度适中（数字高度）
            // 3. 宽高比合理
            double aspectRatio = static_cast<double>(bbox.width) / bbox.height;
            
            if (bbox.width > image.cols * 0.2 &&    // 至少占图像宽度的20%
                bbox.height > 20 && bbox.height < 100 && // 高度在合理范围内
                aspectRatio > 3.0) {                 // 宽高比大于3:1
                
                // 进一步验证：检查区域内是否包含数字特征
                cv::Mat roi = gray(bbox);
                
                // 统计区域内的连通组件（可能的数字）
                cv::Mat roiBinary;
                cv::adaptiveThreshold(roi, roiBinary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                                     cv::THRESH_BINARY, 11, 2);
                
                std::vector<std::vector<cv::Point>> roiContours;
                cv::findContours(roiBinary, roiContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
                
                // 统计可能是数字的小轮廓
                int digitCount = 0;
                for (const auto& roiContour : roiContours) {
                    cv::Rect digitBbox = cv::boundingRect(roiContour);
                    double digitAspectRatio = static_cast<double>(digitBbox.width) / digitBbox.height;
                    
                    // 数字的特征：合理的宽高比和大小
                    if (digitBbox.area() > 50 && digitBbox.area() < 2000 &&
                        digitAspectRatio > 0.3 && digitAspectRatio < 2.0) {
                        digitCount++;
                    }
                }
                
                // 如果找到足够多的数字特征，认为是简谱区域
                if (digitCount >= 3) {
                    // 扩展区域边界
                    int padding = 10;
                    bbox.x = std::max(0, bbox.x - padding);
                    bbox.y = std::max(0, bbox.y - padding);
                    bbox.width = std::min(image.cols - bbox.x, bbox.width + 2 * padding);
                    bbox.height = std::min(image.rows - bbox.y, bbox.height + 2 * padding);
                    
                    regions.push_back(bbox);
                }
            }
        }
        
        // 合并重叠的区域
        std::vector<cv::Rect> mergedRegions;
        std::vector<bool> merged(regions.size(), false);
        
        for (size_t i = 0; i < regions.size(); i++) {
            if (merged[i]) continue;
            
            cv::Rect currentRegion = regions[i];
            merged[i] = true;
            
            // 查找与当前区域重叠的其他区域
            for (size_t j = i + 1; j < regions.size(); j++) {
                if (merged[j]) continue;
                
                cv::Rect intersection = currentRegion & regions[j];
                if (intersection.area() > 0) {
                    // 合并区域
                    currentRegion = currentRegion | regions[j];
                    merged[j] = true;
                }
            }
            
            mergedRegions.push_back(currentRegion);
        }
        
        regions = mergedRegions;
        
        // 按y坐标排序区域
        std::sort(regions.begin(), regions.end(), 
                 [](const cv::Rect& a, const cv::Rect& b) {
                     return a.y < b.y;
                 });
        
    } catch (const cv::Exception& e) {
        // 如果检测失败，返回空列表
        regions.clear();
    }
    
    return regions;
}

std::vector<cv::Vec4i> ImagePreprocessor::detectLines(const cv::Mat& image) {
    std::vector<cv::Vec4i> lines;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 使用多种参数进行边缘检测，适应不同的图像质量
        cv::Mat edges1, edges2, edges3;
        cv::Canny(gray, edges1, 30, 100);   // 低阈值，检测更多边缘
        cv::Canny(gray, edges2, 50, 150);   // 中等阈值
        cv::Canny(gray, edges3, 80, 200);   // 高阈值，只检测强边缘
        
        // 合并边缘图像
        cv::Mat combinedEdges;
        cv::bitwise_or(edges1, edges2, combinedEdges);
        cv::bitwise_or(combinedEdges, edges3, combinedEdges);
        
        // 使用霍夫变换检测线条
        std::vector<cv::Vec4i> allLines;
        
        // 检测水平线（谱线）
        cv::HoughLinesP(combinedEdges, allLines, 1, CV_PI/180, 
                       std::max(30, image.cols/10), // 阈值基于图像宽度
                       image.cols/4,                // 最小线长
                       5);                          // 最大间隙
        
        // 过滤和分类线条
        for (const auto& line : allLines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 计算线条长度和角度
            double length = std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
            
            // 过滤条件：
            // 1. 线条长度至少是图像宽度的1/4
            // 2. 角度接近水平（±15度）
            if (length > image.cols * 0.25 && std::abs(angle) < 15.0) {
                lines.push_back(line);
            }
        }
        
        // 按y坐标排序线条
        std::sort(lines.begin(), lines.end(), 
                 [](const cv::Vec4i& a, const cv::Vec4i& b) {
                     return (a[1] + a[3]) < (b[1] + b[3]);
                 });
        
        // 去除重复的线条
        std::vector<cv::Vec4i> uniqueLines;
        for (const auto& line : lines) {
            bool isDuplicate = false;
            int currentY = (line[1] + line[3]) / 2;
            
            for (const auto& existing : uniqueLines) {
                int existingY = (existing[1] + existing[3]) / 2;
                if (std::abs(currentY - existingY) < 10) { // 10像素容差
                    isDuplicate = true;
                    break;
                }
            }
            
            if (!isDuplicate) {
                uniqueLines.push_back(line);
            }
        }
        
        lines = uniqueLines;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "Line detection failed", e.what());
    }
    
    return lines;
}

double ImagePreprocessor::assessImageQuality(const cv::Mat& image) {
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 计算图像的清晰度（基于拉普拉斯算子）
        cv::Mat laplacian;
        cv::Laplacian(gray, laplacian, CV_64F);
        
        cv::Scalar mean, stddev;
        cv::meanStdDev(laplacian, mean, stddev);
        
        double sharpness = stddev.val[0] * stddev.val[0];
        
        // 计算对比度
        cv::meanStdDev(gray, mean, stddev);
        double contrast = stddev.val[0];
        
        // 计算亮度分布
        cv::Mat hist;
        int histSize = 256;
        float range[] = {0, 256};
        const float* histRange = {range};
        int channels[] = {0};
        cv::calcHist(&gray, 1, channels, cv::Mat(), hist, 1, &histSize, &histRange);
        
        // 计算直方图的熵（信息量）
        double entropy = 0.0;
        for (int i = 0; i < histSize; i++) {
            float p = hist.at<float>(i) / (gray.rows * gray.cols);
            if (p > 0) {
                entropy -= p * std::log2(p);
            }
        }
        
        // 综合质量评分（0-100）
        double quality = (sharpness / 1000.0 + contrast / 100.0 + entropy / 10.0) * 10.0;
        return std::min(100.0, std::max(0.0, quality));
        
    } catch (const cv::Exception&) {
        return 50.0; // 默认中等质量
    }
}

cv::Mat ImagePreprocessor::autoAdjustImage(const cv::Mat& image) {
    try {
        cv::Mat adjusted = image.clone();
        
        // 自动白平衡
        if (image.channels() == 3) {
            cv::Mat lab;
            cv::cvtColor(adjusted, lab, cv::COLOR_BGR2Lab);
            
            std::vector<cv::Mat> labChannels;
            cv::split(lab, labChannels);
            
            // 对L通道进行直方图均衡化
            cv::equalizeHist(labChannels[0], labChannels[0]);
            
            cv::merge(labChannels, lab);
            cv::cvtColor(lab, adjusted, cv::COLOR_Lab2BGR);
        }
        
        // 自动对比度调整
        cv::Mat gray;
        if (adjusted.channels() > 1) {
            cv::cvtColor(adjusted, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = adjusted.clone();
        }
        
        // 计算最佳对比度参数
        cv::Scalar mean, stddev;
        cv::meanStdDev(gray, mean, stddev);
        
        double alpha = 1.0; // 对比度
        double beta = 0.0;  // 亮度
        
        if (stddev.val[0] < 30) { // 对比度太低
            alpha = 1.5;
        } else if (stddev.val[0] > 80) { // 对比度太高
            alpha = 0.8;
        }
        
        if (mean.val[0] < 80) { // 太暗
            beta = 20;
        } else if (mean.val[0] > 180) { // 太亮
            beta = -20;
        }
        
        adjusted.convertTo(adjusted, -1, alpha, beta);
        
        return adjusted;
        
    } catch (const cv::Exception&) {
        return image.clone();
    }
}

std::vector<cv::Rect> ImagePreprocessor::detectTextRegions(const cv::Mat& image) {
    std::vector<cv::Rect> textRegions;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 使用MSER检测文本区域
        cv::Ptr<cv::MSER> mser = cv::MSER::create();
        std::vector<std::vector<cv::Point>> regions;
        std::vector<cv::Rect> bboxes;
        
        mser->detectRegions(gray, regions, bboxes);
        
        // 过滤文本区域
        for (const auto& bbox : bboxes) {
            // 过滤条件：合理的宽高比和大小
            double aspectRatio = static_cast<double>(bbox.width) / bbox.height;
            int area = bbox.width * bbox.height;
            
            if (aspectRatio > 0.1 && aspectRatio < 10.0 && 
                area > 100 && area < image.rows * image.cols * 0.1) {
                textRegions.push_back(bbox);
            }
        }
        
    } catch (const cv::Exception&) {
        // 如果MSER失败，使用简单的轮廓检测
        try {
            cv::Mat grayFallback;
            if (image.channels() > 1) {
                cv::cvtColor(image, grayFallback, cv::COLOR_BGR2GRAY);
            } else {
                grayFallback = image.clone();
            }
            
            cv::Mat binary;
            cv::threshold(grayFallback, binary, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
            
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            for (const auto& contour : contours) {
                cv::Rect bbox = cv::boundingRect(contour);
                double aspectRatio = static_cast<double>(bbox.width) / bbox.height;
                
                if (aspectRatio > 0.5 && aspectRatio < 5.0 && 
                    bbox.area() > 200 && bbox.area() < image.rows * image.cols * 0.05) {
                    textRegions.push_back(bbox);
                }
            }
        } catch (const cv::Exception&) {
            // 如果都失败了，返回空列表
        }
    }
    
    return textRegions;
}

cv::Mat ImagePreprocessor::separateForeground(const cv::Mat& image) {
    try {
        cv::Mat result;
        
        if (image.channels() == 1) {
            // 对于灰度图，使用Otsu阈值分离前景
            cv::threshold(image, result, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        } else {
            // 对于彩色图，使用GrabCut算法
            cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
            cv::Mat bgdModel, fgdModel;
            
            // 设置一个矩形作为前景的大致区域
            cv::Rect rect(image.cols * 0.1, image.rows * 0.1, 
                         image.cols * 0.8, image.rows * 0.8);
            
            cv::grabCut(image, mask, rect, bgdModel, fgdModel, 5, cv::GC_INIT_WITH_RECT);
            
            // 创建前景掩码
            cv::Mat foregroundMask = (mask == cv::GC_FGD) | (mask == cv::GC_PR_FGD);
            
            // 应用掩码
            result = cv::Mat::zeros(image.size(), image.type());
            image.copyTo(result, foregroundMask);
        }
        
        return result;
        
    } catch (const cv::Exception&) {
        return image.clone();
    }
}

std::vector<cv::Rect> ImagePreprocessor::detectStaffRegions(const cv::Mat& image) {
    std::vector<cv::Rect> regions;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 检测水平线条
        std::vector<cv::Vec4i> lines = detectLines(gray);
        
        if (lines.size() < 5) {
            return regions; // 五线谱至少需要5条线
        }
        
        // 寻找五线谱模式（5条等间距的平行线）
        std::vector<std::vector<cv::Vec4i>> staffGroups;
        std::vector<bool> used(lines.size(), false);
        
        for (size_t i = 0; i < lines.size(); i++) {
            if (used[i]) continue;
            
            std::vector<cv::Vec4i> group;
            group.push_back(lines[i]);
            used[i] = true;
            
            int baseY = (lines[i][1] + lines[i][3]) / 2;
            
            // 寻找可能组成五线谱的其他线条
            for (size_t j = i + 1; j < lines.size() && group.size() < 5; j++) {
                if (used[j]) continue;
                
                int currentY = (lines[j][1] + lines[j][3]) / 2;
                
                // 五线谱的线间距通常比六线谱更小且更规律
                if (std::abs(currentY - baseY) < 100) {
                    group.push_back(lines[j]);
                    used[j] = true;
                }
            }
            
            // 如果找到了5条线，检查间距是否相等
            if (group.size() == 5) {
                // 按y坐标排序
                std::sort(group.begin(), group.end(), 
                         [](const cv::Vec4i& a, const cv::Vec4i& b) {
                             return (a[1] + a[3]) < (b[1] + b[3]);
                         });
                
                // 检查间距是否大致相等
                std::vector<int> gaps;
                for (size_t k = 1; k < group.size(); k++) {
                    int y1 = (group[k-1][1] + group[k-1][3]) / 2;
                    int y2 = (group[k][1] + group[k][3]) / 2;
                    gaps.push_back(y2 - y1);
                }
                
                // 计算间距的标准差
                double meanGap = 0;
                for (int gap : gaps) meanGap += gap;
                meanGap /= gaps.size();
                
                double variance = 0;
                for (int gap : gaps) {
                    variance += (gap - meanGap) * (gap - meanGap);
                }
                variance /= gaps.size();
                double stddev = std::sqrt(variance);
                
                // 如果间距相对均匀，认为是五线谱
                if (stddev < meanGap * 0.3) {
                    staffGroups.push_back(group);
                }
            }
        }
        
        // 为每个五线谱组创建区域
        for (const auto& group : staffGroups) {
            if (group.empty()) continue;
            
            int minX = image.cols, maxX = 0;
            int minY = image.rows, maxY = 0;
            
            for (const auto& line : group) {
                minX = std::min(minX, std::min(line[0], line[2]));
                maxX = std::max(maxX, std::max(line[0], line[2]));
                minY = std::min(minY, std::min(line[1], line[3]));
                maxY = std::max(maxY, std::max(line[1], line[3]));
            }
            
            // 扩展区域
            int padding = 40;
            minX = std::max(0, minX - padding);
            maxX = std::min(image.cols, maxX + padding);
            minY = std::max(0, minY - padding);
            maxY = std::min(image.rows, maxY + padding);
            
            cv::Rect region(minX, minY, maxX - minX, maxY - minY);
            
            if (region.width > image.cols * 0.3 && region.height > 80 && region.height < 300) {
                regions.push_back(region);
            }
        }
        
    } catch (const cv::Exception&) {
        // 如果检测失败，返回空列表
    }
    
    return regions;
}

std::vector<TrackRegion> ImagePreprocessor::removeOverlappingRegions(const std::vector<TrackRegion>& regions) {
    std::vector<TrackRegion> result;
    std::vector<bool> removed(regions.size(), false);
    
    for (size_t i = 0; i < regions.size(); i++) {
        if (removed[i]) continue;
        
        TrackRegion bestRegion = regions[i];
        double bestConfidence = calculateRegionConfidence(cv::Mat(), regions[i].region, regions[i].type);
        
        // 检查与其他区域的重叠
        for (size_t j = i + 1; j < regions.size(); j++) {
            if (removed[j]) continue;
            
            cv::Rect intersection = regions[i].region & regions[j].region;
            double overlapRatio = static_cast<double>(intersection.area()) / 
                                std::min(regions[i].region.area(), regions[j].region.area());
            
            if (overlapRatio > 0.3) { // 如果重叠超过30%
                double confidence = calculateRegionConfidence(cv::Mat(), regions[j].region, regions[j].type);
                
                if (confidence > bestConfidence) {
                    bestRegion = regions[j];
                    bestConfidence = confidence;
                }
                removed[j] = true;
            }
        }
        
        result.push_back(bestRegion);
        removed[i] = true;
    }
    
    return result;
}

double ImagePreprocessor::calculateRegionConfidence(const cv::Mat& image, const cv::Rect& region, ScoreType type) {
    double confidence = 0.0;
    
    // 基于区域类型和特征计算置信度
    switch (type) {
        case ScoreType::Tablature:
            // 六线谱的置信度基于线条数量和规律性
            confidence = 0.8; // 基础分数
            if (region.height > 100 && region.height < 200) confidence += 0.1;
            if (region.width > region.height * 3) confidence += 0.1;
            break;
            
        case ScoreType::Numbered:
            // 简谱的置信度基于数字特征
            confidence = 0.7; // 基础分数
            if (region.height > 30 && region.height < 80) confidence += 0.1;
            if (region.width > region.height * 5) confidence += 0.2;
            break;
            
        case ScoreType::Staff:
            // 五线谱的置信度基于线条间距的规律性
            confidence = 0.6; // 基础分数
            if (region.height > 80 && region.height < 150) confidence += 0.2;
            if (region.width > region.height * 4) confidence += 0.2;
            break;
            
        default:
            confidence = 0.5;
            break;
    }
    
    return std::min(1.0, confidence);
}

} // namespace ImageToGP