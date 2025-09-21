#include "NumberedRecognizer.h"
#include "RecognitionException.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>
#include <iostream>

namespace ImageToGP {

NumberedRecognizer::NumberedRecognizer() {
}

NumberedRecognizer::~NumberedRecognizer() {
}

NumberedResult NumberedRecognizer::recognize(const cv::Mat& image) {
    NumberedResult result;
    
    try {
        std::cout << "Starting numbered notation recognition..." << std::endl;
        
        // 识别数字音符
        auto notes = recognizeNumbers(image);
        result.notes = notes;
        std::cout << "Found " << notes.size() << " numbered notes" << std::endl;
        
        // 识别调号
        result.keySignature = recognizeKeySignature(image);
        std::cout << "Key signature recognition completed" << std::endl;
        
        // 识别时值信息
        auto durations = recognizeDurations(image);
        result.durations = durations;
        std::cout << "Found " << durations.size() << " duration markers" << std::endl;
        
        // 将时值信息关联到音符
        for (size_t i = 0; i < result.notes.size() && i < durations.size(); ++i) {
            result.notes[i].duration = durations[i];
        }
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::ProcessingFailed,
                                 "Numbered notation recognition failed: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<NumberedNote> NumberedRecognizer::recognizeNumbers(const cv::Mat& image) {
    std::vector<NumberedNote> notes;
    
    try {
        // 预处理图像，优化数字识别
        cv::Mat processed = preprocessForNumbers(image);
        
        // 查找数字区域
        std::vector<cv::Rect> numberRegions = findNumberRegions(processed);
        std::cout << "Found " << numberRegions.size() << " potential number regions" << std::endl;
        
        // 对每个数字区域进行识别
        for (const auto& region : numberRegions) {
            // 提取数字区域
            cv::Mat numberRegion = processed(region);
            
            // 识别数字
            int number = recognizeSingleNumber(numberRegion);
            if (number >= 1 && number <= 7) {
                // 查找高低音点标记
                std::vector<cv::Point> octaveMarkers = findOctaveMarkers(image, region);
                int octave = calculateOctave(octaveMarkers, region);
                
                // 创建NumberedNote对象
                NumberedNote note(number, octave, region.x + region.width/2);
                notes.push_back(note);
                
                std::cout << "Recognized numbered note: " << number << " (octave: " << octave << ")" << std::endl;
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Number recognition error: " << e.what() << std::endl;
    }
    
    return notes;
}

std::vector<int> NumberedRecognizer::recognizeOctaveMarkers(const cv::Mat& image) {
    std::vector<int> markers;
    return markers;
}

KeySignature NumberedRecognizer::recognizeKeySignature(const cv::Mat& image) {
    KeySignature keySignature;
    
    try {
        // 在图像上部区域查找调号标记（如 1=C, 1=G 等）
        cv::Rect topRegion(0, 0, image.cols, image.rows / 4);
        cv::Mat topArea = image(topRegion);
        
        // 使用模板匹配识别调号文字
        std::string keyText = recognizeKeySignatureText(topArea);
        
        if (!keyText.empty()) {
            std::cout << "Key signature text found: " << keyText << std::endl;
            
            // 解析调号文字（如 "1=C", "1=G" 等）
            if (keyText.find("1=C") != std::string::npos || keyText.find("C") != std::string::npos) {
                keySignature = KeySignature(0, true); // C大调
            } else if (keyText.find("1=G") != std::string::npos || keyText.find("G") != std::string::npos) {
                keySignature = KeySignature(1, true); // G大调
            } else if (keyText.find("1=D") != std::string::npos || keyText.find("D") != std::string::npos) {
                keySignature = KeySignature(2, true); // D大调
            } else if (keyText.find("1=A") != std::string::npos || keyText.find("A") != std::string::npos) {
                keySignature = KeySignature(3, true); // A大调
            } else if (keyText.find("1=E") != std::string::npos || keyText.find("E") != std::string::npos) {
                keySignature = KeySignature(4, true); // E大调
            } else if (keyText.find("1=F") != std::string::npos || keyText.find("F") != std::string::npos) {
                keySignature = KeySignature(-1, true); // F大调
            } else if (keyText.find("1=Bb") != std::string::npos || keyText.find("Bb") != std::string::npos) {
                keySignature = KeySignature(-2, true); // Bb大调
            }
        }
        
        // 如果没有识别到调号，使用默认调号
        if (keySignature.sharps == 0 && keyText.empty()) {
            std::cout << "Using default key signature: C major" << std::endl;
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Key signature recognition error: " << e.what() << std::endl;
    }
    
    return keySignature;
}

cv::Mat NumberedRecognizer::preprocessForNumbers(const cv::Mat& image) {
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
        
        // 自适应二值化，突出数字
        cv::adaptiveThreshold(processed, processed, 255, 
                            cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                            cv::THRESH_BINARY, 15, 10);
        
        // 形态学操作，清理噪点
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);
        
    } catch (const cv::Exception& e) {
        std::cerr << "Image preprocessing error: " << e.what() << std::endl;
        processed = image.clone();
    }
    
    return processed;
}

std::vector<cv::Rect> NumberedRecognizer::findNumberRegions(const cv::Mat& image) {
    std::vector<cv::Rect> regions;
    
    try {
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            
            // 过滤条件：合理的数字大小
            if (boundingRect.width >= 8 && boundingRect.width <= 50 &&
                boundingRect.height >= 10 && boundingRect.height <= 60) {
                
                // 检查宽高比，数字通常比较方正
                double aspectRatio = (double)boundingRect.width / boundingRect.height;
                if (aspectRatio >= 0.3 && aspectRatio <= 2.0) {
                    regions.push_back(boundingRect);
                }
            }
        }
        
        // 按x坐标排序，从左到右
        std::sort(regions.begin(), regions.end(), 
                 [](const cv::Rect& a, const cv::Rect& b) {
                     return a.x < b.x;
                 });
        
    } catch (const cv::Exception& e) {
        std::cerr << "Number region detection error: " << e.what() << std::endl;
    }
    
    return regions;
}

int NumberedRecognizer::recognizeSingleNumber(const cv::Mat& numberRegion) {
    int number = 0;
    
    try {
        // 使用模板匹配识别数字1-7
        number = recognizeNumberByTemplate(numberRegion);
        
    } catch (const cv::Exception& e) {
        std::cerr << "Single number recognition error: " << e.what() << std::endl;
    }
    
    return number;
}

std::vector<cv::Point> NumberedRecognizer::findOctaveMarkers(const cv::Mat& image, const cv::Rect& numberRegion) {
    std::vector<cv::Point> markers;
    
    try {
        // 在数字区域上下方查找小点（高低音标记）
        int searchHeight = 20;
        cv::Rect upperRegion(numberRegion.x, 
                           std::max(0, numberRegion.y - searchHeight),
                           numberRegion.width, searchHeight);
        cv::Rect lowerRegion(numberRegion.x, 
                           numberRegion.y + numberRegion.height,
                           numberRegion.width, 
                           std::min(searchHeight, image.rows - numberRegion.y - numberRegion.height));
        
        // 确保区域在图像范围内
        upperRegion &= cv::Rect(0, 0, image.cols, image.rows);
        lowerRegion &= cv::Rect(0, 0, image.cols, image.rows);
        
        // 在上方区域查找高音点
        if (upperRegion.area() > 0) {
            cv::Mat upperArea = image(upperRegion);
            std::vector<cv::Point> upperPoints = findSmallDots(upperArea);
            for (auto& point : upperPoints) {
                point.x += upperRegion.x;
                point.y += upperRegion.y;
                markers.push_back(point);
            }
        }
        
        // 在下方区域查找低音点
        if (lowerRegion.area() > 0) {
            cv::Mat lowerArea = image(lowerRegion);
            std::vector<cv::Point> lowerPoints = findSmallDots(lowerArea);
            for (auto& point : lowerPoints) {
                point.x += lowerRegion.x;
                point.y += lowerRegion.y;
                markers.push_back(point);
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Octave marker detection error: " << e.what() << std::endl;
    }
    
    return markers;
}

int NumberedRecognizer::calculateOctave(const std::vector<cv::Point>& markers, const cv::Rect& numberRegion) {
    int octave = 0; // 默认中音
    
    try {
        int upperDots = 0;
        int lowerDots = 0;
        
        int numberCenterY = numberRegion.y + numberRegion.height / 2;
        
        for (const auto& marker : markers) {
            if (marker.y < numberCenterY) {
                upperDots++; // 高音点
            } else {
                lowerDots++; // 低音点
            }
        }
        
        // 根据点的数量计算八度
        octave = upperDots - lowerDots;
        
        // 限制八度范围 (-2 到 +2)
        octave = std::max(-2, std::min(2, octave));
        
    } catch (const cv::Exception& e) {
        std::cerr << "Octave calculation error: " << e.what() << std::endl;
    }
    
    return octave;
}

int NumberedRecognizer::recognizeNumberByTemplate(const cv::Mat& numberRegion) {
    int bestMatch = 0;
    double bestScore = 0.0;
    
    try {
        // 基于轮廓特征的数字识别方法
        std::vector<std::vector<cv::Point>> contours;
        cv::Mat temp = numberRegion.clone();
        cv::findContours(temp, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        if (!contours.empty()) {
            // 选择最大的轮廓
            auto maxContour = *std::max_element(contours.begin(), contours.end(),
                [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                    return cv::contourArea(a) < cv::contourArea(b);
                });
            
            // 计算轮廓特征
            double area = cv::contourArea(maxContour);
            double perimeter = cv::arcLength(maxContour, true);
            cv::Rect boundingRect = cv::boundingRect(maxContour);
            
            // 基于特征的简单分类
            double aspectRatio = (double)boundingRect.width / boundingRect.height;
            double extent = area / (boundingRect.width * boundingRect.height);
            std::vector<cv::Point> hull;
            cv::convexHull(maxContour, hull);
            double solidity = area / cv::contourArea(hull);
            
            // 简单的启发式规则识别数字1-7
            if (aspectRatio < 0.6 && extent > 0.3) {
                bestMatch = 1; // 细长形状，可能是1
            } else if (aspectRatio > 1.2 && extent < 0.6) {
                bestMatch = 7; // 宽扁形状，可能是7
            } else if (extent > 0.7 && solidity < 0.8) {
                bestMatch = 6; // 填充度高但有凹陷，可能是6
            } else if (aspectRatio > 0.8 && aspectRatio < 1.2 && extent > 0.6) {
                bestMatch = 5; // 方形且填充，可能是5
            } else if (solidity < 0.7) {
                bestMatch = 4; // 有明显凹陷，可能是4
            } else if (extent < 0.6) {
                bestMatch = 3; // 填充度中等，可能是3
            } else {
                bestMatch = 2; // 其他情况默认为2
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Template matching error: " << e.what() << std::endl;
        bestMatch = 1; // 默认返回1
    }
    
    return bestMatch;
}

std::vector<cv::Point> NumberedRecognizer::findSmallDots(const cv::Mat& image) {
    std::vector<cv::Point> dots;
    
    try {
        // 预处理图像
        cv::Mat processed;
        if (image.channels() == 3) {
            cv::cvtColor(image, processed, cv::COLOR_BGR2GRAY);
        } else {
            processed = image.clone();
        }
        
        // 二值化
        cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
        
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(processed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            double area = cv::contourArea(contour);
            
            // 小点的特征：小面积，近似圆形
            if (area >= 4 && area <= 50 && 
                boundingRect.width <= 8 && boundingRect.height <= 8) {
                
                // 检查圆形度
                double perimeter = cv::arcLength(contour, true);
                double circularity = 4 * CV_PI * area / (perimeter * perimeter);
                
                if (circularity > 0.3) { // 相对圆形
                    cv::Point center(boundingRect.x + boundingRect.width/2,
                                   boundingRect.y + boundingRect.height/2);
                    dots.push_back(center);
                }
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Small dot detection error: " << e.what() << std::endl;
    }
    
    return dots;
}

// 调号和时值识别方法实现

std::string NumberedRecognizer::recognizeKeySignatureText(const cv::Mat& image) {
    std::string keyText;
    
    try {
        // 在图像上部区域查找调号文字（如 1=C, 1=G 等）
        cv::Rect topRegion(0, 0, image.cols, image.rows / 4);
        cv::Mat topArea = image(topRegion);
        
        // 预处理调号区域
        cv::Mat processed;
        if (topArea.channels() == 3) {
            cv::cvtColor(topArea, processed, cv::COLOR_BGR2GRAY);
        } else {
            processed = topArea.clone();
        }
        
        // 二值化处理
        cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        
#ifdef HAVE_TESSERACT
        if (m_tesseract) {
            // 使用OCR识别调号文字
            m_tesseract->SetImage(processed.data, processed.cols, processed.rows, 
                                processed.channels(), processed.step);
            
            char* text = m_tesseract->GetUTF8Text();
            if (text) {
                keyText = std::string(text);
                delete[] text;
            }
        }
#else
        // 没有Tesseract时的占位符
        (void)processed; // 避免未使用变量警告
#endif
        
        // 如果没有OCR，尝试简单的模式匹配
        if (keyText.empty()) {
            // 查找常见的调号模式
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(processed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            
            // 基于轮廓特征推测调号
            if (contours.size() >= 3 && contours.size() <= 6) {
                keyText = "1=C"; // 默认C调
            }
        }
        
        std::cout << "Key signature text: " << keyText << std::endl;
        
    } catch (const cv::Exception& e) {
        std::cerr << "Key signature text recognition error: " << e.what() << std::endl;
    }
    
    return keyText;
}

std::vector<Duration> NumberedRecognizer::recognizeDurations(const cv::Mat& image) {
    std::vector<Duration> durations;
    
    try {
        // 查找时值标记（如下划线、符点等）
        std::vector<cv::Point> durationMarkers = findDurationMarkers(image);
        std::cout << "Found " << durationMarkers.size() << " duration markers" << std::endl;
        
        // 查找数字区域
        std::vector<cv::Rect> numberRegions = findNumberRegions(image);
        
        // 为每个数字区域计算时值
        for (const auto& region : numberRegions) {
            cv::Mat noteRegion = image(region);
            
            // 查找该音符附近的时值标记
            std::vector<cv::Point> nearbyMarkers;
            for (const auto& marker : durationMarkers) {
                // 检查标记是否在音符附近
                if (marker.x >= region.x - 20 && marker.x <= region.x + region.width + 20 &&
                    marker.y >= region.y - 20 && marker.y <= region.y + region.height + 20) {
                    nearbyMarkers.push_back(marker);
                }
            }
            
            // 根据标记计算时值
            Duration duration = calculateNoteDuration(noteRegion, nearbyMarkers);
            durations.push_back(duration);
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Duration recognition error: " << e.what() << std::endl;
    }
    
    return durations;
}

std::vector<cv::Point> NumberedRecognizer::findDurationMarkers(const cv::Mat& image) {
    std::vector<cv::Point> markers;
    
    try {
        // 预处理图像
        cv::Mat processed;
        if (image.channels() == 3) {
            cv::cvtColor(image, processed, cv::COLOR_BGR2GRAY);
        } else {
            processed = image.clone();
        }
        
        // 二值化
        cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
        
        // 查找水平线（下划线，表示时值）
        cv::Mat horizontalKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 1));
        cv::Mat horizontalLines;
        cv::morphologyEx(processed, horizontalLines, cv::MORPH_OPEN, horizontalKernel);
        
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(horizontalLines, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            
            // 过滤条件：水平线特征
            if (boundingRect.width >= 10 && boundingRect.height <= 5) {
                double aspectRatio = (double)boundingRect.width / boundingRect.height;
                if (aspectRatio >= 3.0) { // 长宽比大于3的水平线
                    cv::Point center(boundingRect.x + boundingRect.width/2,
                                   boundingRect.y + boundingRect.height/2);
                    markers.push_back(center);
                }
            }
        }
        
        // 查找点（符点，表示附点音符）
        std::vector<cv::Point> dots = findSmallDots(image);
        markers.insert(markers.end(), dots.begin(), dots.end());
        
    } catch (const cv::Exception& e) {
        std::cerr << "Duration marker detection error: " << e.what() << std::endl;
    }
    
    return markers;
}

Duration NumberedRecognizer::calculateNoteDuration(const cv::Mat& noteRegion, const std::vector<cv::Point>& markers) {
    Duration duration = Duration::Quarter; // 默认四分音符
    
    try {
        // 统计不同类型的标记
        int underlineCount = 0;
        int dotCount = 0;
        
        for (const auto& marker : markers) {
            // 简单分类：根据位置判断是下划线还是符点
            // 这里需要更复杂的逻辑来区分不同类型的标记
            
            // 假设在音符下方的是下划线，右侧的是符点
            if (marker.y > noteRegion.rows / 2) {
                underlineCount++;
            } else {
                dotCount++;
            }
        }
        
        // 根据下划线数量确定基础时值
        if (underlineCount == 0) {
            duration = Duration::Quarter;     // 四分音符
        } else if (underlineCount == 1) {
            duration = Duration::Eighth;      // 八分音符
        } else if (underlineCount == 2) {
            duration = Duration::Sixteenth;   // 十六分音符
        } else if (underlineCount >= 3) {
            duration = Duration::ThirtySecond; // 三十二分音符
        }
        
        // 符点的处理（这里简化处理，实际需要更复杂的逻辑）
        if (dotCount > 0) {
            std::cout << "Note has " << dotCount << " dots (dotted note)" << std::endl;
            // 附点音符的时值计算需要特殊处理
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Note duration calculation error: " << e.what() << std::endl;
    }
    
    return duration;
}

} // namespace ImageToGP