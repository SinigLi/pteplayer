#include "BarlineRecognizer.h"
#include "RecognitionException.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>
#include <iostream>

namespace ImageToGP {

BarlineRecognizer::BarlineRecognizer() 
    : m_minLineLength(30)
    , m_maxLineGap(5)
    , m_lineThreshold(50.0)
    , m_doubleLineDistance(8) {
}

BarlineRecognizer::~BarlineRecognizer() {
}

BarlineResult BarlineRecognizer::recognize(const cv::Mat& image) {
    BarlineResult result;
    
    try {
        std::cout << "Starting barline recognition..." << std::endl;
        
        // 检测垂直线
        auto candidates = detectVerticalLines(image);
        std::cout << "Found " << candidates.size() << " vertical line candidates" << std::endl;
        
        // 分类小节线类型
        auto barlines = classifyBarlines(candidates, image);
        std::cout << "Classified " << barlines.size() << " barlines" << std::endl;
        
        // 检测重复段落编号
        auto repeatNumbers = detectRepeatNumbers(image, barlines);
        
        // 将重复编号分配给相应的小节线
        for (size_t i = 0; i < barlines.size() && i < repeatNumbers.size(); ++i) {
            if (repeatNumbers[i] > 0) {
                barlines[i].repeatNumber = repeatNumbers[i];
            }
        }
        
        // 转换为标准格式
        result.barlines = barlines;
        result.barlineInfo = convertToBarlineInfo(barlines);
        result.totalMeasures = static_cast<int>(barlines.size()) + 1; // 小节数 = 小节线数 + 1
        
        std::cout << "Barline recognition completed. Found " << result.totalMeasures << " measures" << std::endl;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::ProcessingFailed,
                                 "Barline recognition failed: " + std::string(e.what()));
    }
    
    return result;
}

BarlineResult BarlineRecognizer::recognizeInRegion(const cv::Mat& image, const cv::Rect& region) {
    try {
        // 确保区域在图像范围内
        cv::Rect safeRegion = region & cv::Rect(0, 0, image.cols, image.rows);
        cv::Mat regionImage = image(safeRegion);
        
        // 在区域内识别
        BarlineResult result = recognize(regionImage);
        
        // 调整坐标到原图像坐标系
        for (auto& barline : result.barlines) {
            barline.region.x += safeRegion.x;
            barline.region.y += safeRegion.y;
            barline.position += safeRegion.x;
        }
        
        for (auto& info : result.barlineInfo) {
            info.position += safeRegion.x;
        }
        
        return result;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::ProcessingFailed,
                                 "Region barline recognition failed: " + std::string(e.what()));
    }
}

// 检测垂直线
std::vector<BarlineDetection> BarlineRecognizer::detectVerticalLines(const cv::Mat& image) {
    std::vector<BarlineDetection> detections;
    
    try {
        // 预处理图像
        cv::Mat processed = preprocessForBarlines(image);
        
        // 使用霍夫变换检测直线
        std::vector<cv::Vec4i> lines = findVerticalLines(processed);
        
        // 过滤和合并相近的线
        for (const auto& line : lines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 计算线的长度和角度
            int length = static_cast<int>(cv::norm(cv::Point(x2 - x1, y2 - y1)));
            double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
            
            // 过滤：只保留接近垂直的长线
            if (length >= m_minLineLength && std::abs(angle - 90.0) < 15.0) {
                // 创建小节线区域
                int x = std::min(x1, x2);
                int y = std::min(y1, y2);
                int width = std::max(3, std::abs(x2 - x1) + 1);
                int height = length;
                
                cv::Rect region(x, y, width, height);
                int position = x + width / 2;
                
                BarlineDetection detection(region, BarlineType::Single, position);
                detections.push_back(detection);
            }
        }
        
        // 按x坐标排序
        std::sort(detections.begin(), detections.end(),
                 [](const BarlineDetection& a, const BarlineDetection& b) {
                     return a.position < b.position;
                 });
        
        // 合并相近的检测结果
        std::vector<BarlineDetection> merged;
        for (const auto& detection : detections) {
            bool shouldMerge = false;
            
            for (auto& existing : merged) {
                if (std::abs(detection.position - existing.position) < 10) {
                    // 合并：保留更长的线
                    if (detection.region.height > existing.region.height) {
                        existing = detection;
                    }
                    shouldMerge = true;
                    break;
                }
            }
            
            if (!shouldMerge) {
                merged.push_back(detection);
            }
        }
        
        detections = merged;
        
    } catch (const cv::Exception& e) {
        std::cerr << "Vertical line detection error: " << e.what() << std::endl;
    }
    
    return detections;
}

// 分类小节线类型
std::vector<BarlineDetection> BarlineRecognizer::classifyBarlines(const std::vector<BarlineDetection>& candidates, const cv::Mat& image) {
    std::vector<BarlineDetection> classified;
    
    try {
        for (auto detection : candidates) {
            // 分类小节线类型
            detection.type = classifyBarlineType(detection.region, image);
            
            // 计算置信度
            detection.confidence = 0.8f; // 基础置信度
            
            // 根据类型调整置信度
            if (detection.type == BarlineType::RepeatStart || detection.type == BarlineType::RepeatEnd) {
                detection.confidence = 0.9f; // 重复记号通常更明显
            }
            
            classified.push_back(detection);
            
            std::cout << "Classified barline at x=" << detection.position 
                      << " as type " << static_cast<int>(detection.type) << std::endl;
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Barline classification error: " << e.what() << std::endl;
    }
    
    return classified;
}

// 检测重复段落编号
std::vector<int> BarlineRecognizer::detectRepeatNumbers(const cv::Mat& image, const std::vector<BarlineDetection>& barlines) {
    std::vector<int> numbers(barlines.size(), 0);
    
    try {
        for (size_t i = 0; i < barlines.size(); ++i) {
            const auto& barline = barlines[i];
            
            // 只在重复记号附近查找编号
            if (barline.type == BarlineType::RepeatEnd || barline.type == BarlineType::RepeatBoth) {
                // 在小节线上方区域查找数字
                cv::Rect searchRegion(
                    std::max(0, barline.region.x - 20),
                    std::max(0, barline.region.y - 30),
                    40,
                    30
                );
                
                // 确保搜索区域在图像范围内
                searchRegion &= cv::Rect(0, 0, image.cols, image.rows);
                
                if (searchRegion.area() > 0) {
                    int number = detectRepeatNumber(searchRegion, image);
                    numbers[i] = number;
                    
                    if (number > 0) {
                        std::cout << "Found repeat number " << number << " at barline " << i << std::endl;
                    }
                }
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Repeat number detection error: " << e.what() << std::endl;
    }
    
    return numbers;
}

// 预处理图像用于小节线检测
cv::Mat BarlineRecognizer::preprocessForBarlines(const cv::Mat& image) {
    cv::Mat processed;
    
    try {
        // 转换为灰度图
        if (image.channels() == 3) {
            cv::cvtColor(image, processed, cv::COLOR_BGR2GRAY);
        } else {
            processed = image.clone();
        }
        
        // 高斯模糊去噪
        cv::GaussianBlur(processed, processed, cv::Size(3, 3), 0);
        
        // 二值化
        cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        
        // 反转图像（使线条为白色）
        cv::bitwise_not(processed, processed);
        
        // 形态学操作，连接断开的线条
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 5));
        cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);
        
    } catch (const cv::Exception& e) {
        std::cerr << "Barline preprocessing error: " << e.what() << std::endl;
        processed = image.clone();
    }
    
    return processed;
}

// 查找垂直线
std::vector<cv::Vec4i> BarlineRecognizer::findVerticalLines(const cv::Mat& image) {
    std::vector<cv::Vec4i> lines;
    
    try {
        // 使用霍夫变换检测直线
        cv::HoughLinesP(image, lines, 1, CV_PI/180, 
                       static_cast<int>(m_lineThreshold), 
                       m_minLineLength, m_maxLineGap);
        
        // 过滤垂直线
        std::vector<cv::Vec4i> verticalLines;
        for (const auto& line : lines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 计算角度
            double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
            angle = std::abs(angle);
            
            // 保留接近垂直的线（80-100度）
            if (angle > 80 && angle < 100) {
                verticalLines.push_back(line);
            }
        }
        
        lines = verticalLines;
        
    } catch (const cv::Exception& e) {
        std::cerr << "Vertical line finding error: " << e.what() << std::endl;
    }
    
    return lines;
}

// 分类小节线类型
BarlineType BarlineRecognizer::classifyBarlineType(const cv::Rect& region, const cv::Mat& image) {
    try {
        // 检查是否有重复点
        bool hasLeftDots = hasRepeatDots(region, image, true);
        bool hasRightDots = hasRepeatDots(region, image, false);
        
        if (hasLeftDots && hasRightDots) {
            return BarlineType::RepeatBoth;
        } else if (hasLeftDots) {
            return BarlineType::RepeatEnd;
        } else if (hasRightDots) {
            return BarlineType::RepeatStart;
        }
        
        return BarlineType::Single;
        
    } catch (const cv::Exception& e) {
        std::cerr << "Barline type classification error: " << e.what() << std::endl;
        return BarlineType::Single;
    }
}

// 检查重复点
bool BarlineRecognizer::hasRepeatDots(const cv::Rect& region, const cv::Mat& image, bool checkLeft) {
    try {
        // 定义搜索区域
        int searchWidth = 20;
        int searchX = checkLeft ? 
            std::max(0, region.x - searchWidth) : 
            std::min(image.cols - searchWidth, region.x + region.width);
        
        cv::Rect searchRegion(searchX, region.y, searchWidth, region.height);
        searchRegion &= cv::Rect(0, 0, image.cols, image.rows);
        
        if (searchRegion.area() == 0) {
            return false;
        }
        
        cv::Mat searchArea = image(searchRegion);
        
        // 查找圆形区域（重复点）
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(searchArea, circles, cv::HOUGH_GRADIENT, 1, 10, 50, 20, 2, 8);
        
        // 如果找到至少2个圆，认为有重复点
        return circles.size() >= 2;
        
    } catch (const cv::Exception& e) {
        std::cerr << "Repeat dots detection error: " << e.what() << std::endl;
        return false;
    }
}

// 检测重复编号
int BarlineRecognizer::detectRepeatNumber(const cv::Rect& region, const cv::Mat& image) {
    try {
        cv::Mat regionImage = image(region);
        
        // 简单的数字识别：查找小的连通区域
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(regionImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            
            // 过滤：合理的数字大小
            if (boundingRect.width >= 5 && boundingRect.width <= 15 &&
                boundingRect.height >= 8 && boundingRect.height <= 20) {
                
                // 简单的形状分析来识别数字1和2
                double area = cv::contourArea(contour);
                double perimeter = cv::arcLength(contour, true);
                
                if (perimeter > 0) {
                    double compactness = (perimeter * perimeter) / area;
                    
                    // 数字1通常比较细长
                    if (boundingRect.height > boundingRect.width * 2 && compactness > 20) {
                        return 1;
                    }
                    // 数字2通常有更复杂的形状
                    else if (compactness > 15 && compactness < 25) {
                        return 2;
                    }
                }
            }
        }
        
        return 0; // 未识别到数字
        
    } catch (const cv::Exception& e) {
        std::cerr << "Repeat number detection error: " << e.what() << std::endl;
        return 0;
    }
}

// 转换为BarlineInfo格式
std::vector<BarlineInfo> BarlineRecognizer::convertToBarlineInfo(const std::vector<BarlineDetection>& detections) {
    std::vector<BarlineInfo> infos;
    
    try {
        for (const auto& detection : detections) {
            BarlineInfo info;
            info.position = detection.position;
            
            // 转换类型
            switch (detection.type) {
                case BarlineType::RepeatStart:
                    info.isRepeatStart = true;
                    info.isRepeatEnd = false;
                    break;
                case BarlineType::RepeatEnd:
                    info.isRepeatStart = false;
                    info.isRepeatEnd = true;
                    break;
                case BarlineType::RepeatBoth:
                    info.isRepeatStart = true;
                    info.isRepeatEnd = true;
                    break;
                default:
                    info.isRepeatStart = false;
                    info.isRepeatEnd = false;
                    break;
            }
            
            info.repeatCount = detection.repeatNumber;
            
            infos.push_back(info);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "BarlineInfo conversion error: " << e.what() << std::endl;
    }
    
    return infos;
}

} // namespace ImageToGP