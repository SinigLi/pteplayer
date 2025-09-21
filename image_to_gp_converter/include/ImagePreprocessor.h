#ifndef IMAGEPREPROCESSOR_H
#define IMAGEPREPROCESSOR_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>

namespace ImageToGP {

class ImagePreprocessor {
public:
    ImagePreprocessor();
    ~ImagePreprocessor();
    
    // 预处理单张图片
    cv::Mat preprocessImage(const cv::Mat& input);
    
    // 检测图片中的多个音轨区域
    std::vector<TrackRegion> detectTrackRegions(const cv::Mat& image);
    
    // 分离单个音轨区域
    cv::Mat extractTrackRegion(const cv::Mat& image, const cv::Rect& region);
    
private:
    // 图像清理
    cv::Mat cleanImage(const cv::Mat& input);
    
    // 二值化处理
    cv::Mat binarizeImage(const cv::Mat& input);
    
    // 倾斜校正
    cv::Mat correctSkew(const cv::Mat& input);
    
    // 检测六线谱区域（6条平行线）
    std::vector<cv::Rect> detectTablatureRegions(const cv::Mat& image);
    
    // 检测简谱区域（数字音符密集区域）
    std::vector<cv::Rect> detectNumberedRegions(const cv::Mat& image);
    
    // 线条检测
    std::vector<cv::Vec4i> detectLines(const cv::Mat& image);
    
    // 图像质量评估
    double assessImageQuality(const cv::Mat& image);
    
    // 自动调整图像参数
    cv::Mat autoAdjustImage(const cv::Mat& image);
    
    // 检测图像中的文本区域
    std::vector<cv::Rect> detectTextRegions(const cv::Mat& image);
    
    // 分离前景和背景
    cv::Mat separateForeground(const cv::Mat& image);
    
private:
    // 检测五线谱区域
    std::vector<cv::Rect> detectStaffRegions(const cv::Mat& image);
    
    // 去除重叠的区域
    std::vector<TrackRegion> removeOverlappingRegions(const std::vector<TrackRegion>& regions);
    
    // 计算区域的置信度分数
    double calculateRegionConfidence(const cv::Mat& image, const cv::Rect& region, ScoreType type);
};

} // namespace ImageToGP

#endif // IMAGEPREPROCESSOR_H