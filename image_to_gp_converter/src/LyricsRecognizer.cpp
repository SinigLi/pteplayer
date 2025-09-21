#include "LyricsRecognizer.h"
#include "RecognitionException.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <regex>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

namespace ImageToGP {

LyricsRecognizer::LyricsRecognizer() {
    // 初始化中文OCR
    m_tesseract_chi = new tesseract::TessBaseAPI();
    if (m_tesseract_chi->Init(nullptr, "chi_sim", tesseract::OEM_LSTM_ONLY) != 0) {
        std::cerr << "Warning: Failed to initialize Chinese OCR" << std::endl;
        delete m_tesseract_chi;
        m_tesseract_chi = nullptr;
    } else {
        m_tesseract_chi->SetPageSegMode(tesseract::PSM_AUTO);
    }
    
    // 初始化英文OCR
    m_tesseract_eng = new tesseract::TessBaseAPI();
    if (m_tesseract_eng->Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY) != 0) {
        std::cerr << "Warning: Failed to initialize English OCR" << std::endl;
        delete m_tesseract_eng;
        m_tesseract_eng = nullptr;
    } else {
        m_tesseract_eng->SetPageSegMode(tesseract::PSM_AUTO);
    }
}

LyricsRecognizer::~LyricsRecognizer() {
    if (m_tesseract_chi) {
        m_tesseract_chi->End();
        delete m_tesseract_chi;
    }
    if (m_tesseract_eng) {
        m_tesseract_eng->End();
        delete m_tesseract_eng;
    }
}

LyricsResult LyricsRecognizer::recognize(const cv::Mat& image) {
    LyricsResult result;
    
    try {
        std::cout << "Starting lyrics recognition..." << std::endl;
        
        // 提取文字区域
        auto textRegions = extractTextRegions(image);
        std::cout << "Found " << textRegions.size() << " text regions" << std::endl;
        
        // 建立歌词与音符的位置关系
        std::vector<int> notePositions;  // 这里需要从音符识别结果中获取
        // TODO: 从其他识别器获取音符位置信息
        
        auto mappings = mapLyricsToNotes(textRegions, notePositions);
        
        result.lyrics = mappings;
        result.totalLines = static_cast<int>(textRegions.size());
        
        std::cout << "Lyrics recognition completed. Found " << mappings.size() << " lyric mappings" << std::endl;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::ProcessingFailed,
                                 "Lyrics recognition failed: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<TextRegion> LyricsRecognizer::extractTextRegions(const cv::Mat& image) {
    std::vector<TextRegion> regions;
    
    try {
        // 预处理图像，优化OCR识别
        cv::Mat processed = preprocessForOCR(image);
        
        // 查找文字区域
        std::vector<cv::Rect> textAreas = findTextAreas(processed);
        std::cout << "Found " << textAreas.size() << " potential text areas" << std::endl;
        
        // 对每个文字区域进行OCR识别
        for (const auto& area : textAreas) {
            cv::Mat textRegion = processed(area);
            
            // 识别文字
            std::string recognizedText = recognizeTextInRegion(textRegion);
            
            if (!recognizedText.empty()) {
                // 清理和格式化文字
                std::string cleanText = cleanLyricText(recognizedText);
                
                if (!cleanText.empty()) {
                    TextRegion region(area, cleanText, 1.0f);
                    regions.push_back(region);
                    
                    std::cout << "Recognized text: \"" << cleanText << "\"" << std::endl;
                }
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Text region extraction error: " << e.what() << std::endl;
    }
    
    return regions;
}

std::vector<LyricMapping> LyricsRecognizer::mapLyricsToNotes(
    const std::vector<TextRegion>& lyrics,
    const std::vector<int>& notePositions) {
    
    std::vector<LyricMapping> mappings;
    
    try {
        int lineNumber = 0;
        
        for (const auto& textRegion : lyrics) {
            // 将文字按行分割
            std::vector<std::string> lines = splitIntoLines(textRegion.text);
            
            for (const auto& line : lines) {
                // 将每行歌词按空格或标点分割成单词
                std::istringstream iss(line);
                std::string word;
                int wordPosition = textRegion.region.x;
                
                while (iss >> word) {
                    if (!word.empty()) {
                        // 查找最近的音符位置
                        int nearestNotePos = findNearestNote(wordPosition, notePositions);
                        
                        // 创建歌词映射
                        LyricMapping mapping(word, nearestNotePos, lineNumber);
                        mappings.push_back(mapping);
                        
                        // 估算下一个单词的位置（简单估算）
                        wordPosition += static_cast<int>(word.length() * 10); // 假设每个字符10像素宽
                    }
                }
                
                lineNumber++;
            }
        }
        
        std::cout << "Created " << mappings.size() << " lyric-to-note mappings" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Lyric mapping error: " << e.what() << std::endl;
    }
    
    return mappings;
}

// 辅助方法实现
cv::Mat LyricsRecognizer::preprocessForOCR(const cv::Mat& image) {
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
        
        // 自适应二值化，突出文字
        cv::adaptiveThreshold(processed, processed, 255, 
                            cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                            cv::THRESH_BINARY, 11, 2);
        
        // 形态学操作，连接断开的文字
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 1));
        cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);
        
    } catch (const cv::Exception& e) {
        std::cerr << "OCR preprocessing error: " << e.what() << std::endl;
        processed = image.clone();
    }
    
    return processed;
}

std::vector<cv::Rect> LyricsRecognizer::findTextAreas(const cv::Mat& image) {
    std::vector<cv::Rect> textAreas;
    
    try {
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        for (const auto& contour : contours) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            
            // 过滤条件：合理的文字区域大小
            if (boundingRect.width >= 20 && boundingRect.width <= 300 &&
                boundingRect.height >= 8 && boundingRect.height <= 50) {
                
                // 检查宽高比，文字通常比较宽
                double aspectRatio = (double)boundingRect.width / boundingRect.height;
                if (aspectRatio >= 1.5 && aspectRatio <= 15.0) {
                    textAreas.push_back(boundingRect);
                }
            }
        }
        
        // 按y坐标排序，从上到下
        std::sort(textAreas.begin(), textAreas.end(), 
                 [](const cv::Rect& a, const cv::Rect& b) {
                     return a.y < b.y;
                 });
        
        // 合并相近的文字区域
        std::vector<cv::Rect> mergedAreas;
        for (const auto& area : textAreas) {
            bool merged = false;
            
            for (auto& existing : mergedAreas) {
                // 如果两个区域在垂直方向上重叠或相近
                if (abs(area.y - existing.y) < 20 && 
                    abs(area.x - (existing.x + existing.width)) < 50) {
                    // 合并区域
                    int left = std::min(existing.x, area.x);
                    int right = std::max(existing.x + existing.width, area.x + area.width);
                    int top = std::min(existing.y, area.y);
                    int bottom = std::max(existing.y + existing.height, area.y + area.height);
                    
                    existing = cv::Rect(left, top, right - left, bottom - top);
                    merged = true;
                    break;
                }
            }
            
            if (!merged) {
                mergedAreas.push_back(area);
            }
        }
        
        textAreas = mergedAreas;
        
    } catch (const cv::Exception& e) {
        std::cerr << "Text area detection error: " << e.what() << std::endl;
    }
    
    return textAreas;
}

std::string LyricsRecognizer::recognizeTextInRegion(const cv::Mat& region) {
    std::string recognizedText;
    
    try {
        // 放大文字区域以提高识别精度
        cv::Mat enlarged;
        cv::resize(region, enlarged, cv::Size(), 2.0, 2.0, cv::INTER_CUBIC);
        
        // 先尝试中文识别
        if (m_tesseract_chi) {
            m_tesseract_chi->SetImage(enlarged.data, enlarged.cols, enlarged.rows, 
                                    enlarged.channels(), enlarged.step);
            
            char* chiText = m_tesseract_chi->GetUTF8Text();
            if (chiText && strlen(chiText) > 0) {
                recognizedText = std::string(chiText);
                delete[] chiText;
            }
        }
        
        // 如果中文识别失败，尝试英文识别
        if (recognizedText.empty() && m_tesseract_eng) {
            m_tesseract_eng->SetImage(enlarged.data, enlarged.cols, enlarged.rows, 
                                    enlarged.channels(), enlarged.step);
            
            char* engText = m_tesseract_eng->GetUTF8Text();
            if (engText && strlen(engText) > 0) {
                recognizedText = std::string(engText);
                delete[] engText;
            }
        }
        
        // 清理识别结果
        if (!recognizedText.empty()) {
            // 移除换行符和多余空格
            recognizedText.erase(std::remove(recognizedText.begin(), recognizedText.end(), '\n'), recognizedText.end());
            recognizedText.erase(std::remove(recognizedText.begin(), recognizedText.end(), '\r'), recognizedText.end());
            
            // 去除首尾空格
            size_t start = recognizedText.find_first_not_of(" \t");
            if (start != std::string::npos) {
                size_t end = recognizedText.find_last_not_of(" \t");
                recognizedText = recognizedText.substr(start, end - start + 1);
            } else {
                recognizedText.clear();
            }
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "Text recognition error: " << e.what() << std::endl;
    }
    
    return recognizedText;
}

std::string LyricsRecognizer::cleanLyricText(const std::string& rawText) {
    std::string cleanText = rawText;
    
    try {
        // 移除换行符和多余的空格
        std::regex newlineRegex("\\n+");
        cleanText = std::regex_replace(cleanText, newlineRegex, " ");
        
        std::regex spaceRegex("\\s+");
        cleanText = std::regex_replace(cleanText, spaceRegex, " ");
        
        // 移除首尾空格
        cleanText.erase(0, cleanText.find_first_not_of(" \t\r\n"));
        cleanText.erase(cleanText.find_last_not_of(" \t\r\n") + 1);
        
        // 移除特殊字符（保留中文、英文、数字、基本标点）
        std::regex invalidChars("[^\\u4e00-\\u9fa5a-zA-Z0-9\\s\\.,!?;:()\\-'\"]+");
        cleanText = std::regex_replace(cleanText, invalidChars, "");
        
    } catch (const std::exception& e) {
        std::cerr << "Text cleaning error: " << e.what() << std::endl;
    }
    
    return cleanText;
}

std::vector<std::string> LyricsRecognizer::splitIntoLines(const std::string& text) {
    std::vector<std::string> lines;
    
    try {
        std::istringstream stream(text);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
        
        // 如果没有换行符，将整个文本作为一行
        if (lines.empty() && !text.empty()) {
            lines.push_back(text);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Line splitting error: " << e.what() << std::endl;
    }
    
    return lines;
}

int LyricsRecognizer::findNearestNote(int textPosition, const std::vector<int>& notePositions) {
    if (notePositions.empty()) {
        return textPosition; // 如果没有音符位置，返回文字位置
    }
    
    int nearestPos = notePositions[0];
    int minDistance = abs(textPosition - nearestPos);
    
    for (int notePos : notePositions) {
        int distance = abs(textPosition - notePos);
        if (distance < minDistance) {
            minDistance = distance;
            nearestPos = notePos;
        }
    }
    
    return nearestPos;
}

// 带音符位置的歌词识别
LyricsResult LyricsRecognizer::recognizeWithNotePositions(const cv::Mat& image, const std::vector<cv::Point>& notePositions) {
    LyricsResult result;
    
    try {
        std::cout << "Starting advanced lyrics recognition with " << notePositions.size() << " note positions..." << std::endl;
        
        // 提取文字区域
        auto textRegions = extractTextRegions(image);
        std::cout << "Found " << textRegions.size() << " text regions" << std::endl;
        
        // 分离多段歌词
        auto verses = separateLyricVerses(textRegions);
        std::cout << "Separated into " << verses.size() << " verses" << std::endl;
        
        // 创建高级映射
        auto mappings = createAdvancedMapping(textRegions, notePositions);
        
        result.lyrics = mappings;
        result.totalLines = static_cast<int>(textRegions.size());
        
        std::cout << "Advanced lyrics recognition completed. Found " << mappings.size() << " lyric mappings" << std::endl;
        
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::ProcessingFailed,
                                 "Advanced lyrics recognition failed: " + std::string(e.what()));
    }
    
    return result;
}

// 分离多段歌词
std::vector<std::vector<TextRegion>> LyricsRecognizer::separateLyricVerses(const std::vector<TextRegion>& textRegions) {
    std::vector<std::vector<TextRegion>> verses;
    
    try {
        if (textRegions.empty()) {
            return verses;
        }
        
        // 按y坐标排序
        std::vector<TextRegion> sortedRegions = textRegions;
        std::sort(sortedRegions.begin(), sortedRegions.end(), 
                 [](const TextRegion& a, const TextRegion& b) {
                     return a.region.y < b.region.y;
                 });
        
        // 根据垂直间距分组
        std::vector<TextRegion> currentVerse;
        int lastY = -1;
        const int VERSE_SEPARATION_THRESHOLD = 40; // 段落间距阈值
        
        for (const auto& region : sortedRegions) {
            if (lastY >= 0 && (region.region.y - lastY) > VERSE_SEPARATION_THRESHOLD) {
                // 开始新段落
                if (!currentVerse.empty()) {
                    verses.push_back(currentVerse);
                    currentVerse.clear();
                }
            }
            
            currentVerse.push_back(region);
            lastY = region.region.y + region.region.height;
        }
        
        // 添加最后一段
        if (!currentVerse.empty()) {
            verses.push_back(currentVerse);
        }
        
        std::cout << "Separated lyrics into " << verses.size() << " verses" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Verse separation error: " << e.what() << std::endl;
    }
    
    return verses;
}

// 创建高级映射
std::vector<LyricMapping> LyricsRecognizer::createAdvancedMapping(const std::vector<TextRegion>& textRegions, 
                                                                 const std::vector<cv::Point>& notePositions) {
    std::vector<LyricMapping> mappings;
    
    try {
        int lineNumber = 0;
        
        for (const auto& textRegion : textRegions) {
            // 判断是中文还是英文
            bool isChinese = isChineseText(textRegion.text);
            std::vector<std::string> segments;
            
            if (isChinese) {
                segments = segmentChineseText(textRegion.text);
            } else {
                segments = segmentEnglishText(textRegion.text);
            }
            
            // 为每个文字段创建映射
            int segmentIndex = 0;
            for (const auto& segment : segments) {
                if (!segment.empty()) {
                    // 估算段落在文本区域中的位置
                    int segmentX = textRegion.region.x + (segmentIndex * textRegion.region.width / segments.size());
                    cv::Point segmentPos(segmentX, textRegion.region.y + textRegion.region.height / 2);
                    
                    // 找到最近的音符
                    cv::Point nearestNote = segmentPos; // 默认值
                    float minDistance = std::numeric_limits<float>::max();
                    
                    for (const auto& notePos : notePositions) {
                        float distance = cv::norm(segmentPos - notePos);
                        if (distance < minDistance) {
                            minDistance = distance;
                            nearestNote = notePos;
                        }
                    }
                    
                    // 计算映射置信度
                    float confidence = calculateMappingConfidence(textRegion, nearestNote);
                    
                    // 创建映射
                    LyricMapping mapping(segment, nearestNote.x, lineNumber);
                    mappings.push_back(mapping);
                    
                    std::cout << "Mapped \"" << segment << "\" to note at (" << nearestNote.x << ", " << nearestNote.y 
                              << ") with confidence " << confidence << std::endl;
                }
                segmentIndex++;
            }
            
            lineNumber++;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Advanced mapping error: " << e.what() << std::endl;
    }
    
    return mappings;
}

// 判断是否为中文文本
bool LyricsRecognizer::isChineseText(const std::string& text) {
    // 检查是否包含中文字符（Unicode范围：U+4E00-U+9FFF）
    for (size_t i = 0; i < text.length(); ) {
        unsigned char c = text[i];
        
        if (c >= 0x80) { // 多字节字符
            // 简单的UTF-8中文检测
            if (i + 2 < text.length()) {
                unsigned char c1 = text[i];
                unsigned char c2 = text[i + 1];
                unsigned char c3 = text[i + 2];
                
                // 检查是否在中文Unicode范围内
                if (c1 >= 0xE4 && c1 <= 0xE9) {
                    return true;
                }
            }
            i += 3; // 假设UTF-8中文字符为3字节
        } else {
            i++;
        }
    }
    
    return false;
}

// 分割中文文本
std::vector<std::string> LyricsRecognizer::segmentChineseText(const std::string& text) {
    std::vector<std::string> segments;
    
    try {
        // 中文按字符分割
        for (size_t i = 0; i < text.length(); ) {
            unsigned char c = text[i];
            
            if (c >= 0x80) { // 多字节字符（中文）
                if (i + 2 < text.length()) {
                    std::string character = text.substr(i, 3);
                    segments.push_back(character);
                    i += 3;
                } else {
                    i++;
                }
            } else {
                // 单字节字符
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                    segments.push_back(std::string(1, c));
                }
                i++;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Chinese text segmentation error: " << e.what() << std::endl;
    }
    
    return segments;
}

// 分割英文文本
std::vector<std::string> LyricsRecognizer::segmentEnglishText(const std::string& text) {
    std::vector<std::string> segments;
    
    try {
        // 英文按单词分割
        std::istringstream iss(text);
        std::string word;
        
        while (iss >> word) {
            // 移除标点符号
            std::string cleanWord;
            for (char c : word) {
                if (std::isalnum(c) || c == '\'' || c == '-') {
                    cleanWord += c;
                }
            }
            
            if (!cleanWord.empty()) {
                segments.push_back(cleanWord);
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "English text segmentation error: " << e.what() << std::endl;
    }
    
    return segments;
}

// 计算映射置信度
float LyricsRecognizer::calculateMappingConfidence(const TextRegion& textRegion, const cv::Point& notePos) {
    try {
        // 基于距离计算置信度
        cv::Point textCenter(textRegion.region.x + textRegion.region.width / 2,
                           textRegion.region.y + textRegion.region.height / 2);
        
        float distance = cv::norm(textCenter - notePos);
        
        // 距离越近，置信度越高
        float maxDistance = 100.0f; // 最大有效距离
        float confidence = std::max(0.0f, 1.0f - (distance / maxDistance));
        
        // 考虑文本区域的大小和清晰度
        float sizeBonus = std::min(1.0f, textRegion.region.area() / 1000.0f);
        confidence = (confidence + textRegion.confidence + sizeBonus) / 3.0f;
        
        return std::min(1.0f, confidence);
        
    } catch (const std::exception& e) {
        std::cerr << "Confidence calculation error: " << e.what() << std::endl;
        return 0.5f; // 默认置信度
    }
}

} // namespace ImageToGP