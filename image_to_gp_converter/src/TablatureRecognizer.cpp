#include "TablatureRecognizer.h"
#include "RecognitionException.h"
#include <algorithm>
#include <cmath>
#include <regex>

namespace ImageToGP {

TablatureRecognizer::TablatureRecognizer() 
    : m_stringCount(6), m_confidenceThreshold(0.7)
#ifdef HAVE_TESSERACT
    , m_tessAPI(nullptr)
#endif
{
#ifdef HAVE_TESSERACT
    // 初始化Tesseract OCR引擎
    m_tessAPI = new tesseract::TessBaseAPI();
    
    // 初始化OCR，使用英文和中文语言包，只识别数字和字母
    if (m_tessAPI->Init(nullptr, "eng+chi_sim", tesseract::OEM_LSTM_ONLY) != 0) {
        delete m_tessAPI;
        m_tessAPI = nullptr;
        throw RecognitionException(RecognitionError::InitializationFailed,
                                 "Failed to initialize Tesseract OCR");
    }
    
    // 设置OCR参数，识别数字、字母和常见符号
    m_tessAPI->SetVariable("tesseract_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#bmaj");
    m_tessAPI->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
#endif
}

TablatureRecognizer::~TablatureRecognizer() {
#ifdef HAVE_TESSERACT
    if (m_tessAPI) {
        m_tessAPI->End();
        delete m_tessAPI;
    }
#endif
}

TablatureResult TablatureRecognizer::recognize(const cv::Mat& image) {
    TablatureResult result;
    
    try {
        if (image.empty()) {
            throw RecognitionException(RecognitionError::InvalidInput, "Input image is empty");
        }
        
        // 1. 检测六线谱线条
        auto lines = detectTabLines(image);
        if (lines.empty()) {
            throw RecognitionException(RecognitionError::NoValidContent, 
                                     "No tablature lines detected");
        }
        
        // 2. 验证线条是否为有效的六线谱
        if (!validateTablatureLines(lines)) {
            throw RecognitionException(RecognitionError::InvalidFormat,
                                     "Invalid tablature line pattern");
        }
        
        // 3. 计算弦的位置
        auto stringPositions = calculateStringPositions(lines);
        
        // 4. 识别品位数字
        auto notes = recognizeFretNumbers(image, stringPositions);
        
        // 5. 过滤低置信度的结果
        notes = filterLowConfidenceNotes(notes);
        
        // 6. 合并和弦音符
        notes = mergeChordNotes(notes);
        
        // 7. 按位置排序
        sortNotesByPosition(notes);
        
        // 8. 识别时值
        auto durations = recognizeDurations(image, notes);
        
        // 9. 设置结果
        result.notes = notes;
        result.durations = durations;
        result.stringCount = m_stringCount;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "Tablature recognition failed", e.what());
    }
    
    return result;
}

std::vector<cv::Vec4i> TablatureRecognizer::detectTabLines(const cv::Mat& image) {
    std::vector<cv::Vec4i> lines;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 二值化处理
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                             cv::THRESH_BINARY, 15, 10);
        
        // 使用形态学操作增强水平线条
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 1));
        cv::Mat enhanced;
        cv::morphologyEx(binary, enhanced, cv::MORPH_CLOSE, kernel);
        
        // 边缘检测
        cv::Mat edges;
        cv::Canny(enhanced, edges, 50, 150);
        
        // 霍夫变换检测直线
        std::vector<cv::Vec4i> allLines;
        cv::HoughLinesP(edges, allLines, 1, CV_PI/180, 
                       std::max(50, image.cols/8),  // 阈值
                       image.cols/3,                // 最小线长
                       10);                         // 最大间隙
        
        // 过滤水平线条
        for (const auto& line : allLines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 计算线条角度
            double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
            double length = std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            
            // 只保留接近水平的长线条
            if (std::abs(angle) < 10.0 && length > image.cols * 0.3) {
                lines.push_back(line);
            }
        }
        
        // 按y坐标排序
        std::sort(lines.begin(), lines.end(), 
                 [](const cv::Vec4i& a, const cv::Vec4i& b) {
                     return (a[1] + a[3]) < (b[1] + b[3]);
                 });
        
        // 去除重复线条
        std::vector<cv::Vec4i> uniqueLines;
        for (const auto& line : lines) {
            bool isDuplicate = false;
            int currentY = (line[1] + line[3]) / 2;
            
            for (const auto& existing : uniqueLines) {
                int existingY = (existing[1] + existing[3]) / 2;
                if (std::abs(currentY - existingY) < 5) {
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

bool TablatureRecognizer::validateTablatureLines(const std::vector<cv::Vec4i>& lines) {
    if (lines.size() < 4 || lines.size() > 8) {
        return false; // 六线谱通常有4-6条线
    }
    
    // 检查线条间距是否合理
    std::vector<int> spacings;
    for (size_t i = 1; i < lines.size(); i++) {
        int y1 = (lines[i-1][1] + lines[i-1][3]) / 2;
        int y2 = (lines[i][1] + lines[i][3]) / 2;
        int spacing = y2 - y1;
        spacings.push_back(spacing);
    }
    
    // 检查间距是否在合理范围内
    for (int spacing : spacings) {
        if (spacing < MIN_STRING_SPACING || spacing > MAX_STRING_SPACING) {
            return false;
        }
    }
    
    // 检查间距的一致性
    if (!spacings.empty()) {
        double meanSpacing = 0;
        for (int spacing : spacings) meanSpacing += spacing;
        meanSpacing /= spacings.size();
        
        double variance = 0;
        for (int spacing : spacings) {
            variance += (spacing - meanSpacing) * (spacing - meanSpacing);
        }
        variance /= spacings.size();
        double stddev = std::sqrt(variance);
        
        // 标准差不应该太大
        if (stddev > meanSpacing * 0.4) {
            return false;
        }
    }
    
    return true;
}

std::vector<int> TablatureRecognizer::calculateStringPositions(const std::vector<cv::Vec4i>& lines) {
    std::vector<int> positions;
    
    for (const auto& line : lines) {
        int yPos = (line[1] + line[3]) / 2;
        positions.push_back(yPos);
    }
    
    // 确保位置按y坐标排序
    std::sort(positions.begin(), positions.end());
    
    return positions;
}

std::vector<FretNote> TablatureRecognizer::recognizeFretNumbers(const cv::Mat& image, const std::vector<int>& stringPositions) {
    std::vector<FretNote> notes;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 二值化处理
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                             cv::THRESH_BINARY, 11, 2);
        
        // 查找轮廓来定位可能的数字
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect bbox = cv::boundingRect(contour);
            
            // 过滤条件：合理的数字大小
            if (bbox.width < 5 || bbox.width > 30 || 
                bbox.height < 8 || bbox.height > 25) {
                continue;
            }
            
            // 检查宽高比
            double aspectRatio = static_cast<double>(bbox.width) / bbox.height;
            if (aspectRatio < 0.3 || aspectRatio > 2.0) {
                continue;
            }
            
            // 确定这个数字在哪根弦上
            int stringIndex = mapYPositionToString(bbox.y + bbox.height/2, stringPositions);
            if (stringIndex < 0) {
                continue; // 不在任何弦上
            }
            
            // 提取数字区域并进行OCR识别
            cv::Mat digitRegion = preprocessForOCR(gray(bbox));
            std::string digitText = recognizeDigitWithOCR(digitRegion);
            
            int fretValue;
            if (isValidFretNumber(digitText, fretValue)) {
                FretNote note;
                note.string = stringIndex;
                note.fret = fretValue;
                note.position = bbox.x + bbox.width/2;
                note.duration = Duration::Quarter; // 默认四分音符
                
                notes.push_back(note);
            }
        }
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "Fret number recognition failed", e.what());
    }
    
    return notes;
}

cv::Mat TablatureRecognizer::preprocessForOCR(const cv::Mat& region) {
    cv::Mat processed = region.clone();
    
    try {
        // 调整大小以提高OCR准确性
        if (processed.cols < 20 || processed.rows < 20) {
            cv::resize(processed, processed, cv::Size(20, 20), 0, 0, cv::INTER_CUBIC);
        }
        
        // 高斯模糊去噪
        cv::GaussianBlur(processed, processed, cv::Size(3, 3), 0);
        
        // 二值化
        cv::threshold(processed, processed, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        
        // 形态学操作清理
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
        cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);
        
    } catch (const cv::Exception&) {
        // 如果预处理失败，返回原图
        return region.clone();
    }
    
    return processed;
}

std::string TablatureRecognizer::recognizeDigitWithOCR(const cv::Mat& digitRegion) {
#ifdef HAVE_TESSERACT
    if (!m_tessAPI) {
        return "";
    }
    
    try {
        // 设置图像数据
        m_tessAPI->SetImage(digitRegion.data, digitRegion.cols, digitRegion.rows, 
                           digitRegion.channels(), digitRegion.step);
        
        // 获取识别结果
        char* text = m_tessAPI->GetUTF8Text();
        std::string result(text);
        delete[] text;
        
        // 清理结果（去除空格和换行）
        result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
        
        return result;
        
    } catch (const std::exception&) {
        return "";
    }
#else
    // 简化的OCR实现，基于模板匹配（当没有Tesseract时的备用方案）
    try {
        // 计算区域的一些特征来推测数字
        cv::Mat binary;
        cv::threshold(digitRegion, binary, 128, 255, cv::THRESH_BINARY);
        
        // 计算黑色像素的分布
        int blackPixels = cv::countNonZero(255 - binary);
        double density = static_cast<double>(blackPixels) / (digitRegion.rows * digitRegion.cols);
        
        // 基于密度和形状特征的简单数字识别
        if (density < 0.1) return "1";
        else if (density < 0.2) return "7";
        else if (density < 0.3) return "4";
        else if (density < 0.4) return "2";
        else if (density < 0.5) return "3";
        else if (density < 0.6) return "5";
        else if (density < 0.7) return "6";
        else if (density < 0.8) return "9";
        else return "8";
        
    } catch (const std::exception&) {
        return "0"; // 默认返回0
    }
#endif
}

bool TablatureRecognizer::isValidFretNumber(const std::string& text, int& fretValue) {
    if (text.empty()) {
        return false;
    }
    
    try {
        fretValue = std::stoi(text);
        return fretValue >= 0 && fretValue <= 24; // 有效品位范围
    } catch (const std::exception&) {
        return false;
    }
}

int TablatureRecognizer::mapYPositionToString(int yPos, const std::vector<int>& stringPositions) {
    if (stringPositions.empty()) {
        return -1;
    }
    
    // 找到最接近的弦位置
    int closestString = -1;
    int minDistance = INT_MAX;
    
    for (size_t i = 0; i < stringPositions.size(); i++) {
        int distance = std::abs(yPos - stringPositions[i]);
        if (distance < minDistance && distance <= MAX_STRING_SPACING/2) {
            minDistance = distance;
            closestString = static_cast<int>(i);
        }
    }
    
    return closestString;
}

std::vector<Duration> TablatureRecognizer::recognizeDurations(const cv::Mat& image, const std::vector<FretNote>& notes) {
    std::vector<Duration> durations;
    
    try {
        // 为每个音符检测时值符号
        for (const auto& note : notes) {
            cv::Point noteCenter(note.position, 0);
            
            // 在音符周围查找时值符号
            auto symbols = detectDurationSymbols(image, noteCenter, 40);
            
            Duration noteDuration = Duration::Quarter; // 默认四分音符
            
            if (!symbols.empty()) {
                // 分析最接近的符号
                cv::Mat symbolRegion = image(symbols[0]);
                noteDuration = analyzeDurationSymbol(symbolRegion);
            }
            
            durations.push_back(noteDuration);
        }
        
    } catch (const cv::Exception& e) {
        // 如果时值识别失败，返回默认时值
        durations.assign(notes.size(), Duration::Quarter);
    }
    
    return durations;
}

std::vector<cv::Rect> TablatureRecognizer::detectDurationSymbols(const cv::Mat& image, const cv::Point& center, int radius) {
    std::vector<cv::Rect> symbols;
    
    try {
        // 定义搜索区域
        cv::Rect searchArea(
            std::max(0, center.x - radius),
            std::max(0, center.y - radius),
            std::min(image.cols - std::max(0, center.x - radius), 2 * radius),
            std::min(image.rows - std::max(0, center.y - radius), 2 * radius)
        );
        
        cv::Mat roi = image(searchArea);
        cv::Mat gray;
        
        if (roi.channels() > 1) {
            cv::cvtColor(roi, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = roi.clone();
        }
        
        // 检测垂直线条（符干）
        cv::Mat edges;
        cv::Canny(gray, edges, 50, 150);
        
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(edges, lines, 1, CV_PI/180, 20, 15, 5);
        
        for (const auto& line : lines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 检查是否为垂直线（符干）
            double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
            if (std::abs(angle - 90) < 20 || std::abs(angle + 90) < 20) {
                cv::Rect stemRect(
                    searchArea.x + std::min(x1, x2) - 5,
                    searchArea.y + std::min(y1, y2) - 5,
                    std::abs(x2 - x1) + 10,
                    std::abs(y2 - y1) + 10
                );
                symbols.push_back(stemRect);
            }
        }
        
    } catch (const cv::Exception&) {
        // 如果检测失败，返回空列表
    }
    
    return symbols;
}

Duration TablatureRecognizer::analyzeDurationSymbol(const cv::Mat& symbolRegion) {
    // 简化的时值分析，基于符号的几何特征
    
    cv::Mat gray;
    if (symbolRegion.channels() > 1) {
        cv::cvtColor(symbolRegion, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = symbolRegion.clone();
    }
    
    // 计算符号的密度和复杂度
    cv::Mat binary;
    cv::threshold(gray, binary, 128, 255, cv::THRESH_BINARY);
    
    int blackPixels = cv::countNonZero(255 - binary);
    double density = static_cast<double>(blackPixels) / (symbolRegion.rows * symbolRegion.cols);
    
    // 基于密度判断时值类型
    if (density > 0.3) {
        return Duration::Eighth;     // 八分音符（有符尾）
    } else if (density > 0.15) {
        return Duration::Quarter;    // 四分音符（有符干）
    } else {
        return Duration::Half;       // 二分音符（空心）
    }
}

std::vector<FretNote> TablatureRecognizer::mergeChordNotes(const std::vector<FretNote>& notes) {
    std::vector<FretNote> mergedNotes = notes;
    
    // 按位置分组，相近位置的音符可能是和弦
    const int CHORD_TOLERANCE = 20; // 像素容差
    
    std::sort(mergedNotes.begin(), mergedNotes.end(),
             [](const FretNote& a, const FretNote& b) {
                 return a.position < b.position;
             });
    
    // 这里可以添加更复杂的和弦合并逻辑
    // 目前保持简单，直接返回排序后的结果
    
    return mergedNotes;
}

void TablatureRecognizer::sortNotesByPosition(std::vector<FretNote>& notes) {
    std::sort(notes.begin(), notes.end(),
             [](const FretNote& a, const FretNote& b) {
                 return a.position < b.position;
             });
}

std::vector<FretNote> TablatureRecognizer::filterLowConfidenceNotes(const std::vector<FretNote>& notes) {
    std::vector<FretNote> filteredNotes;
    
    // 简单的过滤逻辑，可以根据需要扩展
    for (const auto& note : notes) {
        // 检查品位数字是否合理
        if (note.fret >= 0 && note.fret <= 24 && 
            note.string >= 0 && note.string < m_stringCount) {
            filteredNotes.push_back(note);
        }
    }
    
    return filteredNotes;
}

std::vector<cv::Rect> TablatureRecognizer::detectStems(const cv::Mat& image) {
    std::vector<cv::Rect> stems;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 边缘检测
        cv::Mat edges;
        cv::Canny(gray, edges, 50, 150);
        
        // 检测垂直线条（符干）
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(edges, lines, 1, CV_PI/180, 15, 20, 5);
        
        for (const auto& line : lines) {
            int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
            
            // 计算线条角度
            double angle = std::atan2(y2 - y1, x2 - x1) * 180.0 / CV_PI;
            double length = std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            
            // 检查是否为垂直线条（符干）
            if ((std::abs(angle - 90) < 15 || std::abs(angle + 90) < 15) && length > 15) {
                cv::Rect stemRect(
                    std::min(x1, x2) - 2,
                    std::min(y1, y2) - 2,
                    std::abs(x2 - x1) + 4,
                    std::abs(y2 - y1) + 4
                );
                
                // 确保矩形在图像范围内
                stemRect &= cv::Rect(0, 0, image.cols, image.rows);
                
                if (stemRect.area() > 0) {
                    stems.push_back(stemRect);
                }
            }
        }
        
    } catch (const cv::Exception& e) {
        // 如果检测失败，返回空列表
    }
    
    return stems;
}

std::vector<cv::Rect> TablatureRecognizer::detectBeamsAndTies(const cv::Mat& image) {
    std::vector<cv::Rect> beamsAndTies;
    
    try {
        cv::Mat gray;
        if (image.channels() > 1) {
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = image.clone();
        }
        
        // 二值化
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                             cv::THRESH_BINARY, 15, 10);
        
        // 使用形态学操作检测水平结构（符尾、连音线）
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 3));
        cv::Mat morphed;
        cv::morphologyEx(binary, morphed, cv::MORPH_CLOSE, kernel);
        
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(morphed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect bbox = cv::boundingRect(contour);
            
            // 过滤条件：符尾和连音线的特征
            double aspectRatio = static_cast<double>(bbox.width) / bbox.height;
            
            if (bbox.width > 10 && bbox.height > 2 && 
                aspectRatio > 2.0 && aspectRatio < 10.0) {
                beamsAndTies.push_back(bbox);
            }
        }
        
    } catch (const cv::Exception& e) {
        // 如果检测失败，返回空列表
    }
    
    return beamsAndTies;
}

Duration TablatureRecognizer::determineDurationFromSymbols(const cv::Mat& region, const cv::Point& notePosition) {
    try {
        // 检测符干
        auto stems = detectStems(region);
        
        // 检测符尾和连音线
        auto beams = detectBeamsAndTies(region);
        
        // 根据符号组合判断时值
        if (!stems.empty() && !beams.empty()) {
            // 有符干和符尾 -> 八分音符或更短
            if (beams.size() >= 2) {
                return Duration::Sixteenth;  // 十六分音符
            } else {
                return Duration::Eighth;     // 八分音符
            }
        } else if (!stems.empty()) {
            // 只有符干 -> 四分音符
            return Duration::Quarter;
        } else {
            // 没有符干 -> 二分音符或全音符
            cv::Mat gray;
            if (region.channels() > 1) {
                cv::cvtColor(region, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = region.clone();
            }
            
            // 检查音符头是否为空心
            cv::Mat binary;
            cv::threshold(gray, binary, 128, 255, cv::THRESH_BINARY);
            
            int blackPixels = cv::countNonZero(255 - binary);
            double fillRatio = static_cast<double>(blackPixels) / (region.rows * region.cols);
            
            if (fillRatio < 0.1) {
                return Duration::Whole;      // 全音符（空心且大）
            } else {
                return Duration::Half;       // 二分音符（空心）
            }
        }
        
    } catch (const cv::Exception&) {
        return Duration::Quarter; // 默认四分音符
    }
}

} // namespace ImageToGP