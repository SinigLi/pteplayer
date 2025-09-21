#ifndef SIGNATURERECOGNIZER_H
#define SIGNATURERECOGNIZER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

#include <tesseract/baseapi.h>

namespace ImageToGP {

// 和弦检测结果
struct ChordDetection {
    cv::Rect region;        // 和弦标记区域
    std::string name;       // 和弦名称（如Am、C7等）
    int position;           // x坐标位置
    float confidence;       // 识别置信度
    
    ChordDetection() : position(0), confidence(0.0f) {}
    ChordDetection(const cv::Rect& r, const std::string& n, int pos, float conf = 1.0f)
        : region(r), name(n), position(pos), confidence(conf) {}
};

// 调号检测结果
struct KeySignatureDetection {
    cv::Rect region;        // 调号区域
    int sharps;            // 升号数量（负数表示降号）
    bool isMajor;          // 是否为大调
    float confidence;       // 识别置信度
    
    KeySignatureDetection() : sharps(0), isMajor(true), confidence(0.0f) {}
    KeySignatureDetection(const cv::Rect& r, int s, bool major, float conf = 1.0f)
        : region(r), sharps(s), isMajor(major), confidence(conf) {}
};

// 拍号检测结果
struct TimeSignatureDetection {
    cv::Rect region;        // 拍号区域
    int numerator;         // 分子
    int denominator;       // 分母
    float confidence;       // 识别置信度
    
    TimeSignatureDetection() : numerator(4), denominator(4), confidence(0.0f) {}
    TimeSignatureDetection(const cv::Rect& r, int num, int den, float conf = 1.0f)
        : region(r), numerator(num), denominator(den), confidence(conf) {}
};

// 综合识别结果
struct SignatureResult {
    std::vector<ChordDetection> chords;         // 和弦标记
    KeySignatureDetection keySignature;        // 调号
    TimeSignatureDetection timeSignature;      // 拍号
    std::vector<ChordInfo> chordInfo;          // 转换后的和弦信息
    
    SignatureResult() {}
};

class SignatureRecognizer {
public:
    SignatureRecognizer();
    ~SignatureRecognizer();
    
    // 综合识别
    SignatureResult recognize(const cv::Mat& image);
    
    // 单独识别方法
    std::vector<ChordDetection> recognizeChords(const cv::Mat& image);
    KeySignatureDetection recognizeKeySignature(const cv::Mat& image);
    TimeSignatureDetection recognizeTimeSignature(const cv::Mat& image);
    
private:
    // 和弦识别相关
    std::vector<cv::Rect> findChordRegions(const cv::Mat& image);
    std::string recognizeChordText(const cv::Mat& region);
    bool isValidChordName(const std::string& text);
    std::string normalizeChordName(const std::string& text);
    
    // 调号识别相关
    std::vector<cv::Rect> findKeySignatureRegions(const cv::Mat& image);
    int countSharpsFlats(const cv::Mat& region);
    bool isSharpSymbol(const cv::Mat& symbol);
    bool isFlatSymbol(const cv::Mat& symbol);
    
    // 拍号识别相关
    std::vector<cv::Rect> findTimeSignatureRegions(const cv::Mat& image);
    std::pair<int, int> parseTimeSignature(const cv::Mat& region);
    int recognizeDigit(const cv::Mat& digitRegion);
    
    // 辅助方法
    cv::Mat preprocessForText(const cv::Mat& image);
    cv::Mat preprocessForSymbols(const cv::Mat& image);
    std::vector<ChordInfo> convertToChordInfo(const std::vector<ChordDetection>& detections);
    
    // OCR引擎
    tesseract::TessBaseAPI* m_tesseract_eng;
    
    // 参数配置
    int m_minChordWidth;        // 最小和弦标记宽度
    int m_maxChordWidth;        // 最大和弦标记宽度
    int m_minChordHeight;       // 最小和弦标记高度
    int m_maxChordHeight;       // 最大和弦标记高度
};

} // namespace ImageToGP

#endif // SIGNATURERECOGNIZER_H