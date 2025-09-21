#ifndef TABLATURERECOGNIZER_H
#define TABLATURERECOGNIZER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>

#ifdef HAVE_TESSERACT
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#endif

namespace ImageToGP {

class TablatureRecognizer {
public:
    TablatureRecognizer();
    ~TablatureRecognizer();
    
    // 识别六线谱
    TablatureResult recognize(const cv::Mat& image);
    
    // 设置识别参数
    void setStringCount(int count) { m_stringCount = count; }
    void setConfidenceThreshold(double threshold) { m_confidenceThreshold = threshold; }
    
private:
    // 六线谱线条精确定位
    std::vector<cv::Vec4i> detectTabLines(const cv::Mat& image);
    
    // 验证线条是否为六线谱
    bool validateTablatureLines(const std::vector<cv::Vec4i>& lines);
    
    // 计算弦的y坐标位置
    std::vector<int> calculateStringPositions(const std::vector<cv::Vec4i>& lines);
    
    // 识别品位数字（0-24数字OCR）
    std::vector<FretNote> recognizeFretNumbers(const cv::Mat& image, const std::vector<int>& stringPositions);
    
    // 预处理数字识别区域
    cv::Mat preprocessForOCR(const cv::Mat& region);
    
    // 使用Tesseract OCR识别数字
    std::string recognizeDigitWithOCR(const cv::Mat& digitRegion);
    
    // 验证识别的数字是否为有效品位（0-24）
    bool isValidFretNumber(const std::string& text, int& fretValue);
    
    // 弦位映射和音符位置计算
    int mapYPositionToString(int yPos, const std::vector<int>& stringPositions);
    
    // 识别时值标记
    std::vector<Duration> recognizeDurations(const cv::Mat& image, const std::vector<FretNote>& notes);
    
    // 检测符干
    std::vector<cv::Rect> detectStems(const cv::Mat& image);
    
    // 检测符尾和连音线
    std::vector<cv::Rect> detectBeamsAndTies(const cv::Mat& image);
    
    // 根据符干和符尾判断时值类型
    Duration determineDurationFromSymbols(const cv::Mat& region, const cv::Point& notePosition);
    
    // 检测数字周围的时值符号
    std::vector<cv::Rect> detectDurationSymbols(const cv::Mat& image, const cv::Point& center, int radius = 30);
    
    // 分析符号类型
    Duration analyzeDurationSymbol(const cv::Mat& symbolRegion);
    
    // 合并相邻的音符为和弦
    std::vector<FretNote> mergeChordNotes(const std::vector<FretNote>& notes);
    
    // 按位置排序音符
    void sortNotesByPosition(std::vector<FretNote>& notes);
    
    // 过滤低置信度的识别结果
    std::vector<FretNote> filterLowConfidenceNotes(const std::vector<FretNote>& notes);
    
private:
    int m_stringCount;                    // 弦数，默认6
    double m_confidenceThreshold;         // 置信度阈值
    
#ifdef HAVE_TESSERACT
    tesseract::TessBaseAPI* m_tessAPI;    // Tesseract OCR引擎
#endif
    
    // 六线谱的标准参数
    static const int MIN_STRING_SPACING = 8;   // 最小弦间距
    static const int MAX_STRING_SPACING = 25;  // 最大弦间距
    static const int LINE_THICKNESS_TOLERANCE = 3; // 线条粗细容差
};

} // namespace ImageToGP

#endif // TABLATURERECOGNIZER_H