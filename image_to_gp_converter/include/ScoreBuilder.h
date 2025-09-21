#ifndef SCOREBUILDER_H
#define SCOREBUILDER_H

#include "DataStructures.h"
#include <memory>
#include <vector>
#include <map>

// 前向声明，避免循环依赖
class Score;
class System;
class Staff;
class Voice;
class Position;
class Note;
namespace ImageToGP {


class ScoreBuilder {
public:
    ScoreBuilder();
    ~ScoreBuilder();
    
    // 从多音轨识别结果构建Score对象
    std::unique_ptr<MultiTrackResult> buildMultiTrackScore(const std::vector<TrackResult>& trackResults,
                                                          const LyricsResult& lyrics,
                                                          const std::vector<BarlineInfo>& barlines);
    
    // 合并多个图片的识别结果
    MultiTrackResult mergeImageResults(const std::vector<MultiTrackResult>& imageResults);
    
    // 同步多音轨的时间对齐
    bool synchronizeTrackTiming(MultiTrackResult& result);
    
    // 验证识别结果的完整性
    bool validateResult(const MultiTrackResult& result);
    
    // 获取构建统计信息
    struct BuildStatistics {
        int totalTracks;
        int totalNotes;
        int totalBarlines;
        int totalLyrics;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };
    BuildStatistics getLastBuildStatistics() const;
    
private:
    BuildStatistics m_lastStats;
    
    // 内部辅助方法
    void alignTracksToBarlines(std::vector<TrackResult>& tracks, 
                              const std::vector<BarlineInfo>& barlines);
    
    void mapLyricsToTracks(std::vector<TrackResult>& tracks, 
                          const LyricsResult& lyrics);
    
    bool validateTrackConsistency(const std::vector<TrackResult>& tracks);
    
    void addWarning(const std::string& warning);
    void addError(const std::string& error);
    
    // 时间同步相关
    struct TimePoint {
        int position;       // 在图片中的x坐标
        float timeOffset;   // 相对时间偏移
        int measureNumber;  // 小节号
    };
    
    std::vector<TimePoint> extractTimePoints(const std::vector<BarlineInfo>& barlines);
    void alignNotesToTimePoints(TrackResult& track, const std::vector<TimePoint>& timePoints);
};

} // namespace ImageToGP

#endif // SCOREBUILDER_H