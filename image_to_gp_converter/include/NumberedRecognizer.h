#ifndef NUMBEREDRECOGNIZER_H
#define NUMBEREDRECOGNIZER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>
#include <memory>

// Tesseract forward declaration (conditional)
#ifdef HAVE_TESSERACT
#include <tesseract/baseapi.h>
#endif

namespace ImageToGP {

class NumberedRecognizer {
public:
    NumberedRecognizer();
    ~NumberedRecognizer();
    
    // 识别简谱
    NumberedResult recognize(const cv::Mat& image);
    
private:
    // 识别数字音符
    std::vector<NumberedNote> recognizeNumbers(const cv::Mat& image);
    
    // 识别高低音点
    std::vector<int> recognizeOctaveMarkers(const cv::Mat& image);
    
    // 识别调号标记
    KeySignature recognizeKeySignature(const cv::Mat& image);
    
    // 辅助方法
    cv::Mat preprocessForNumbers(const cv::Mat& image);
    std::vector<cv::Rect> findNumberRegions(const cv::Mat& image);
    int recognizeSingleNumber(const cv::Mat& numberRegion);
    std::vector<cv::Point> findOctaveMarkers(const cv::Mat& image, const cv::Rect& numberRegion);
    int calculateOctave(const std::vector<cv::Point>& markers, const cv::Rect& numberRegion);
    
    // 模板匹配和小点检测
    int recognizeNumberByTemplate(const cv::Mat& numberRegion);
    std::vector<cv::Point> findSmallDots(const cv::Mat& image);
    
    // 调号和时值识别
    std::string recognizeKeySignatureText(const cv::Mat& image);
    std::vector<Duration> recognizeDurations(const cv::Mat& image);
    std::vector<cv::Point> findDurationMarkers(const cv::Mat& image);
    Duration calculateNoteDuration(const cv::Mat& noteRegion, const std::vector<cv::Point>& markers);
    
#ifdef HAVE_TESSERACT
    tesseract::TessBaseAPI* m_tesseract;
#endif
};

} // namespace ImageToGP

#endif // NUMBEREDRECOGNIZER_H