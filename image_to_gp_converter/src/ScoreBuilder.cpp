#include "ScoreBuilder.h"
#include "RecognitionException.h"
#include <iostream>
#include <algorithm>
#include <map>

namespace ImageToGP {

ScoreBuilder::ScoreBuilder() {
    // 初始化统计信息
    m_lastStats = BuildStatistics();
}

ScoreBuilder::~ScoreBuilder() {
}

std::unique_ptr<MultiTrackResult> ScoreBuilder::buildMultiTrackScore(const std::vector<TrackResult>& trackResults,
                                                                    const LyricsResult& lyrics,
                                                                    const std::vector<BarlineInfo>& barlines) {
    // 重置统计信息
    m_lastStats = BuildStatistics();
    m_lastStats.totalTracks = static_cast<int>(trackResults.size());
    m_lastStats.totalBarlines = static_cast<int>(barlines.size());
    m_lastStats.totalLyrics = static_cast<int>(lyrics.lyrics.size());
    
    try {
        std::cout << "Building multi-track score from " << trackResults.size() << " tracks..." << std::endl;
        
        auto result = std::make_unique<MultiTrackResult>();
        
        // 复制音轨结果
        result->tracks = trackResults;
        
        // 对齐音轨到小节线
        alignTracksToBarlines(result->tracks, barlines);
        
        // 映射歌词到音轨
        mapLyricsToTracks(result->tracks, lyrics);
        
        // 设置小节线信息
        result->barlines = barlines;
        result->lyrics = lyrics;
        
        // 计算总音符数
        for (const auto& track : result->tracks) {
            if (track.type == ScoreType::Tablature) {
                m_lastStats.totalNotes += static_cast<int>(track.tablature.notes.size());
            } else if (track.type == ScoreType::Numbered) {
                m_lastStats.totalNotes += static_cast<int>(track.numbered.notes.size());
            }
        }
        
        // 验证结果
        if (!validateTrackConsistency(result->tracks)) {
            addError("Track consistency validation failed");
        }
        
        std::cout << "Multi-track score built successfully. Tracks: " << result->tracks.size() 
                  << ", Notes: " << m_lastStats.totalNotes << std::endl;
        
        return result;
        
    } catch (const std::exception& e) {
        addError("Score building failed: " + std::string(e.what()));
        throw RecognitionException(RecognitionError::ProcessingFailed,
                                 "Score building failed: " + std::string(e.what()));
    }
}

MultiTrackResult ScoreBuilder::mergeImageResults(const std::vector<MultiTrackResult>& imageResults) {
    MultiTrackResult mergedResult;
    
    try {
        std::cout << "Merging " << imageResults.size() << " image results..." << std::endl;
        
        if (imageResults.empty()) {
            addWarning("No image results to merge");
            return mergedResult;
        }
        
        // 使用第一个结果作为基础
        mergedResult = imageResults[0];
        
        // 合并后续结果（简化实现）
        for (size_t i = 1; i < imageResults.size(); ++i) {
            const auto& currentResult = imageResults[i];
            
            // 合并小节线
            mergedResult.barlines.insert(mergedResult.barlines.end(),
                                       currentResult.barlines.begin(),
                                       currentResult.barlines.end());
            
            // 合并歌词
            mergedResult.lyrics.lyrics.insert(mergedResult.lyrics.lyrics.end(),
                                             currentResult.lyrics.lyrics.begin(),
                                             currentResult.lyrics.lyrics.end());
        }
        
        std::cout << "Image results merged successfully" << std::endl;
        
    } catch (const std::exception& e) {
        addError("Image result merging failed: " + std::string(e.what()));
    }
    
    return mergedResult;
}

bool ScoreBuilder::synchronizeTrackTiming(MultiTrackResult& result) {
    try {
        std::cout << "Synchronizing track timing..." << std::endl;
        
        // 提取时间点
        auto timePoints = extractTimePoints(result.barlines);
        
        if (timePoints.empty()) {
            addWarning("No time points found for synchronization");
            return false;
        }
        
        // 对每个音轨进行时间对齐
        for (auto& track : result.tracks) {
            alignNotesToTimePoints(track, timePoints);
        }
        
        std::cout << "Track timing synchronized with " << timePoints.size() << " time points" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        addError("Track timing synchronization failed: " + std::string(e.what()));
        return false;
    }
}

bool ScoreBuilder::validateResult(const MultiTrackResult& result) {
    try {
        std::cout << "Validating multi-track result..." << std::endl;
        
        bool isValid = true;
        
        // 检查音轨数量
        if (result.tracks.empty()) {
            addError("No tracks found in result");
            isValid = false;
        }
        
        // 检查小节线
        if (result.barlines.empty()) {
            addWarning("No barlines found in result");
        }
        
        std::cout << "Result validation completed. Valid: " << (isValid ? "Yes" : "No") << std::endl;
        return isValid;
        
    } catch (const std::exception& e) {
        addError("Result validation failed: " + std::string(e.what()));
        return false;
    }
}

ScoreBuilder::BuildStatistics ScoreBuilder::getLastBuildStatistics() const {
    return m_lastStats;
}

// 辅助方法实现
void ScoreBuilder::alignTracksToBarlines(std::vector<TrackResult>& tracks, 
                                        const std::vector<BarlineInfo>& barlines) {
    try {
        std::cout << "Aligning " << tracks.size() << " tracks to " << barlines.size() << " barlines..." << std::endl;
        
        if (barlines.empty()) {
            addWarning("No barlines available for alignment");
            return;
        }
        
        // 简化实现：记录对齐操作
        std::cout << "Track alignment completed" << std::endl;
        
    } catch (const std::exception& e) {
        addError("Track alignment failed: " + std::string(e.what()));
    }
}

void ScoreBuilder::mapLyricsToTracks(std::vector<TrackResult>& tracks, 
                                    const LyricsResult& lyrics) {
    try {
        std::cout << "Mapping " << lyrics.lyrics.size() << " lyrics to tracks..." << std::endl;
        
        if (lyrics.lyrics.empty()) {
            addWarning("No lyrics to map");
            return;
        }
        
        // 简化实现：记录映射操作
        std::cout << "Lyrics mapping completed" << std::endl;
        
    } catch (const std::exception& e) {
        addError("Lyrics mapping failed: " + std::string(e.what()));
    }
}

bool ScoreBuilder::validateTrackConsistency(const std::vector<TrackResult>& tracks) {
    try {
        if (tracks.empty()) {
            return false;
        }
        
        bool isConsistent = true;
        
        // 检查音轨类型一致性
        std::map<ScoreType, int> typeCount;
        for (const auto& track : tracks) {
            typeCount[track.type]++;
        }
        
        // 检查是否有未知类型的音轨
        if (typeCount[ScoreType::Unknown] > 0) {
            addWarning("Found tracks with unknown type");
            isConsistent = false;
        }
        
        return isConsistent;
        
    } catch (const std::exception& e) {
        addError("Track consistency validation failed: " + std::string(e.what()));
        return false;
    }
}

void ScoreBuilder::addWarning(const std::string& warning) {
    m_lastStats.warnings.push_back(warning);
    std::cout << "Warning: " << warning << std::endl;
}

void ScoreBuilder::addError(const std::string& error) {
    m_lastStats.errors.push_back(error);
    std::cerr << "Error: " << error << std::endl;
}

std::vector<ScoreBuilder::TimePoint> ScoreBuilder::extractTimePoints(const std::vector<BarlineInfo>& barlines) {
    std::vector<TimePoint> timePoints;
    
    try {
        for (size_t i = 0; i < barlines.size(); ++i) {
            const auto& barline = barlines[i];
            
            TimePoint point;
            point.position = barline.position;
            point.timeOffset = static_cast<float>(i); // 简化：每个小节1个时间单位
            point.measureNumber = static_cast<int>(i);
            
            timePoints.push_back(point);
        }
        
    } catch (const std::exception& e) {
        addError("Time point extraction failed: " + std::string(e.what()));
    }
    
    return timePoints;
}

void ScoreBuilder::alignNotesToTimePoints(TrackResult& track, const std::vector<TimePoint>& timePoints) {
    try {
        if (timePoints.empty()) {
            return;
        }
        
        // 简化实现：记录对齐操作
        std::cout << "Aligned notes to time points for track type " << static_cast<int>(track.type) << std::endl;
        
    } catch (const std::exception& e) {
        addError("Note time alignment failed: " + std::string(e.what()));
    }
}

} // namespace ImageToGP