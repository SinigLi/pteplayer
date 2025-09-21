#ifndef LYRICSRECOGNIZER_H
#define LYRICSRECOGNIZER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>
#include <memory>

#include <tesseract/baseapi.h>

namespace ImageToGP {

// 文本区域结构
struct TextRegion {
    cv::Rect region;        // 文本区域
    std::string text;       // 识别的文字
    float confidence;       // 识别置信度
    
    TextRegion() : confidence(0.0f) {}
    TextRegion(const cv::Rect& r, const std::string& t, float c = 0.0f)
        : region(r), text(t), confidence(c) {}
};

class LyricsRecognizer {
public:
    LyricsRecognizer();
    ~LyricsRecognizer();
    
    // 识别歌词
    LyricsResult recognize(const cv::Mat& image);
    
    // 带音符位置的歌词识别
    LyricsResult recognizeWithNotePositions(const cv::Mat& image, const std::vector<cv::Point>& notePositions);
    
private:
    // OCR文字识别
    std::vector<TextRegion> extractTextRegions(const cv::Mat& image);
    
    // 建立歌词与音符的位置关系
    std::vector<LyricMapping> mapLyricsToNotes(
        const std::vector<TextRegion>& lyrics,
        const std::vector<int>& notePositions);
    
    // 辅助方法
    cv::Mat preprocessForOCR(const cv::Mat& image);
    std::vector<cv::Rect> findTextAreas(const cv::Mat& image);
    std::string recognizeTextInRegion(const cv::Mat& region);
    std::string cleanLyricText(const std::string& rawText);
    std::vector<std::string> splitIntoLines(const std::string& text);
    int findNearestNote(int textPosition, const std::vector<int>& notePositions);
    
    // 高级歌词映射方法
    std::vector<std::vector<TextRegion>> separateLyricVerses(const std::vector<TextRegion>& textRegions);
    std::vector<LyricMapping> createAdvancedMapping(const std::vector<TextRegion>& textRegions, 
                                                   const std::vector<cv::Point>& notePositions);
    bool isChineseText(const std::string& text);
    std::vector<std::string> segmentChineseText(const std::string& text);
    std::vector<std::string> segmentEnglishText(const std::string& text);
    float calculateMappingConfidence(const TextRegion& textRegion, const cv::Point& notePos);
    
    tesseract::TessBaseAPI* m_tesseract_chi;
    tesseract::TessBaseAPI* m_tesseract_eng;
};

} // namespace ImageToGP

#endif // LYRICSRECOGNIZER_H