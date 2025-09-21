#ifndef SCORERECOGNIZER_H
#define SCORERECOGNIZER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>
#include <memory>

namespace ImageToGP {

// 前向声明
class TablatureRecognizer;
class NumberedRecognizer;
class LyricsRecognizer;
class ImagePreprocessor;

class ScoreRecognizer {
public:
    ScoreRecognizer();
    ~ScoreRecognizer();
    
    // 识别单张图片的多个音轨
    MultiTrackResult recognizeImage(const cv::Mat& image, 
                                   const std::vector<TrackRegion>& tracks);
    
    // 识别单个音轨区域
    TrackResult recognizeTrack(const cv::Mat& trackImage, ScoreType type);
    
private:
    std::unique_ptr<TablatureRecognizer> m_tabRecognizer;
    std::unique_ptr<NumberedRecognizer> m_numRecognizer;
    std::unique_ptr<LyricsRecognizer> m_lyricsRecognizer;
    std::unique_ptr<ImagePreprocessor> m_preprocessor;
};

} // namespace ImageToGP

#endif // SCORERECOGNIZER_H