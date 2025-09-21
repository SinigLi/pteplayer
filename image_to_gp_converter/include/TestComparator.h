#ifndef TESTCOMPARATOR_H
#define TESTCOMPARATOR_H

#include "DataStructures.h"
#include <string>
#include <vector>
#include <memory>

// 包含Score类定义
#include <score/score.h>

namespace ImageToGP {

// 比较结果统计
struct ComparisonStatistics {
    int totalNotes;                    // 总音符数
    int matchedNotes;                  // 匹配的音符数
    int missedNotes;                   // 遗漏的音符数
    int extraNotes;                    // 多余的音符数
    
    int totalChords;                   // 总和弦数
    int matchedChords;                 // 匹配的和弦数
    int missedChords;                  // 遗漏的和弦数
    int extraChords;                   // 多余的和弦数
    
    int totalLyrics;                   // 总歌词数
    int matchedLyrics;                 // 匹配的歌词数
    int missedLyrics;                  // 遗漏的歌词数
    int extraLyrics;                   // 多余的歌词数
    
    float noteAccuracy;                // 音符准确率
    float chordAccuracy;               // 和弦准确率
    float lyricsAccuracy;              // 歌词准确率
    float overallAccuracy;             // 总体准确率
    
    std::vector<std::string> errors;   // 错误信息
    std::vector<std::string> warnings; // 警告信息
    
    ComparisonStatistics() 
        : totalNotes(0), matchedNotes(0), missedNotes(0), extraNotes(0)
        , totalChords(0), matchedChords(0), missedChords(0), extraChords(0)
        , totalLyrics(0), matchedLyrics(0), missedLyrics(0), extraLyrics(0)
        , noteAccuracy(0.0f), chordAccuracy(0.0f), lyricsAccuracy(0.0f), overallAccuracy(0.0f) {}
};

// 差异报告项
struct DifferenceItem {
    enum Type {
        NoteMismatch,      // 音符不匹配
        ChordMismatch,     // 和弦不匹配
        LyricsMismatch,    // 歌词不匹配
        MissingNote,       // 缺失音符
        ExtraNote,         // 多余音符
        MissingChord,      // 缺失和弦
        ExtraChord,        // 多余和弦
        MissingLyrics,     // 缺失歌词
        ExtraLyrics        // 多余歌词
    };
    
    Type type;                         // 差异类型
    std::string description;           // 差异描述
    int position;                      // 位置信息
    std::string expected;              // 期望值
    std::string actual;                // 实际值
    
    DifferenceItem(Type t, const std::string& desc, int pos = 0)
        : type(t), description(desc), position(pos) {}
};

// 差异报告
struct DifferenceReport {
    std::vector<DifferenceItem> differences;  // 差异列表
    ComparisonStatistics statistics;          // 统计信息
    std::string summaryText;                  // 摘要文本
    
    DifferenceReport() {}
};

class TestComparator {
public:
    TestComparator();
    ~TestComparator();
    
    // 主要比较方法
    DifferenceReport compareResults(const MultiTrackResult& recognized, 
                                   const MultiTrackResult& reference);
    
    // 从GP文件加载参考数据
    std::unique_ptr<MultiTrackResult> loadGPFile(const std::string& gpFilePath);
    
    // 使用FileFormatManager加载GP文件为Score对象
    std::unique_ptr<Score> loadScoreFromGP(const std::string& gpFilePath);
    
    // 分析GP文件的基本信息（不需要完整解析）
    struct GPFileInfo {
        int trackCount;
        int systemCount;
        int staffCount;
        size_t fileSize;
        std::string format;
        std::string title;
        bool isValid;
        
        GPFileInfo() : trackCount(0), systemCount(0), staffCount(0), fileSize(0), isValid(false) {}
    };
    GPFileInfo analyzeGPFile(const std::string& gpFilePath);
    
    // 生成详细报告
    std::string generateDetailedReport(const DifferenceReport& report);
    
    // 保存报告到文件
    bool saveReportToFile(const DifferenceReport& report, const std::string& outputPath);
    
    // 获取最后的比较统计
    ComparisonStatistics getLastComparisonStats() const;
    
private:
    // 比较具体元素
    void compareNotes(const std::vector<TrackResult>& recognized,
                     const std::vector<TrackResult>& reference,
                     DifferenceReport& report);
    
    void compareChords(const std::vector<ChordInfo>& recognized,
                      const std::vector<ChordInfo>& reference,
                      DifferenceReport& report);
    
    void compareLyrics(const LyricsResult& recognized,
                      const LyricsResult& reference,
                      DifferenceReport& report);
    
    // 辅助方法
    bool isNoteMatch(const FretNote& note1, const FretNote& note2);
    bool isChordMatch(const ChordInfo& chord1, const ChordInfo& chord2);
    bool isLyricsMatch(const LyricMapping& lyrics1, const LyricMapping& lyrics2);
    
    void calculateAccuracy(ComparisonStatistics& stats);
    std::string formatStatistics(const ComparisonStatistics& stats);
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 成员变量
    ComparisonStatistics m_lastStats;
    
    // 比较参数
    int m_positionTolerance;           // 位置容差
    float m_confidenceThreshold;       // 置信度阈值
};

} // namespace ImageToGP

#endif // TESTCOMPARATOR_H