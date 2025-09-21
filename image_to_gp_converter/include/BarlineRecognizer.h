#ifndef BARLINERECOGNIZER_H
#define BARLINERECOGNIZER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>
#include <vector>

namespace ImageToGP {

// 小节线类型枚举
enum class BarlineType {
    Single,         // 单小节线
    Double,         // 双小节线
    RepeatStart,    // 重复开始
    RepeatEnd,      // 重复结束
    RepeatBoth,     // 双向重复
    Final           // 终止线
};

// 小节线检测结果
struct BarlineDetection {
    cv::Rect region;        // 小节线区域
    BarlineType type;       // 小节线类型
    int position;           // x坐标位置
    float confidence;       // 检测置信度
    int repeatNumber;       // 重复段落编号（1.、2.等）
    
    BarlineDetection() : type(BarlineType::Single), position(0), confidence(0.0f), repeatNumber(0) {}
    BarlineDetection(const cv::Rect& r, BarlineType t, int pos, float conf = 1.0f)
        : region(r), type(t), position(pos), confidence(conf), repeatNumber(0) {}
};

// 小节线识别结果
struct BarlineResult {
    std::vector<BarlineDetection> barlines;    // 检测到的小节线
    std::vector<BarlineInfo> barlineInfo;      // 转换后的小节线信息
    int totalMeasures;                         // 总小节数
    
    BarlineResult() : totalMeasures(0) {}
};

class BarlineRecognizer {
public:
    BarlineRecognizer();
    ~BarlineRecognizer();
    
    // 识别小节线和重复记号
    BarlineResult recognize(const cv::Mat& image);
    
    // 在指定区域内识别小节线
    BarlineResult recognizeInRegion(const cv::Mat& image, const cv::Rect& region);
    
private:
    // 核心检测方法
    std::vector<BarlineDetection> detectVerticalLines(const cv::Mat& image);
    std::vector<BarlineDetection> classifyBarlines(const std::vector<BarlineDetection>& candidates, const cv::Mat& image);
    std::vector<int> detectRepeatNumbers(const cv::Mat& image, const std::vector<BarlineDetection>& barlines);
    
    // 辅助方法
    cv::Mat preprocessForBarlines(const cv::Mat& image);
    std::vector<cv::Vec4i> findVerticalLines(const cv::Mat& image);
    BarlineType classifyBarlineType(const cv::Rect& region, const cv::Mat& image);
    bool hasRepeatDots(const cv::Rect& region, const cv::Mat& image, bool checkLeft = true);
    int detectRepeatNumber(const cv::Rect& region, const cv::Mat& image);
    std::vector<BarlineInfo> convertToBarlineInfo(const std::vector<BarlineDetection>& detections);
    
    // 参数配置
    int m_minLineLength;        // 最小线长度
    int m_maxLineGap;          // 最大线间隙
    double m_lineThreshold;     // 线检测阈值
    int m_doubleLineDistance;   // 双线间距
};

} // namespace ImageToGP

#endif // BARLINERECOGNIZER_H