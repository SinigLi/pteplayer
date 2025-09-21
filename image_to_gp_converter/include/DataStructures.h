#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <optional>
#include <ctime>

namespace ImageToGP {

// 乐谱类型枚举
enum class ScoreType {
    Tablature,    // 六线谱
    Numbered,     // 简谱
    Staff,        // 五线谱
    Mixed,        // 混合类型（同时包含多种）
    Unknown       // 未知类型
};

// 时值类型枚举
enum class Duration {
    Whole = 1,        // 全音符
    Half = 2,         // 二分音符
    Quarter = 4,      // 四分音符
    Eighth = 8,       // 八分音符
    Sixteenth = 16,   // 十六分音符
    ThirtySecond = 32 // 三十二分音符
};

// 音轨区域结构
struct TrackRegion {
    cv::Rect region;     // 音轨在图片中的区域
    ScoreType type;      // 音轨类型
    int trackIndex;      // 音轨编号
    
    TrackRegion() : trackIndex(0), type(ScoreType::Unknown) {}
    TrackRegion(const cv::Rect& r, ScoreType t, int idx) 
        : region(r), type(t), trackIndex(idx) {}
};

// 图片元数据结构
struct ImageMetadata {
    int index;                          // 图片下标
    std::string path;                   // 文件路径
    cv::Size size;                      // 图片尺寸
    std::vector<TrackRegion> tracks;    // 图片中的音轨区域
    
    // 扩展信息
    std::string songName;               // 歌曲名称
    std::string identifier;             // 标识符
    size_t fileSize;                    // 文件大小（字节）
    std::time_t lastModified;           // 最后修改时间
    bool isProcessed;                   // 是否已处理
    std::string processingError;        // 处理错误信息
    
    ImageMetadata() : index(0), fileSize(0), lastModified(0), isProcessed(false) {}
};

// 品位音符结构（六线谱）
struct FretNote {
    int string;        // 弦号 (0-5)
    int fret;          // 品位 (0-24)
    int position;      // 在图片中的x坐标
    Duration duration; // 时值
    
    FretNote() : string(0), fret(0), position(0), duration(Duration::Quarter) {}
    FretNote(int s, int f, int pos, Duration d = Duration::Quarter)
        : string(s), fret(f), position(pos), duration(d) {}
};

// 数字音符结构（简谱）
struct NumberedNote {
    int number;        // 音符数字 (1-7)
    int octave;        // 八度 (-2 到 +2)
    int position;      // 位置
    Duration duration; // 时值
    
    NumberedNote() : number(1), octave(0), position(0), duration(Duration::Quarter) {}
    NumberedNote(int n, int oct, int pos, Duration d = Duration::Quarter)
        : number(n), octave(oct), position(pos), duration(d) {}
};

// 歌词映射结构
struct LyricMapping {
    std::string text;  // 歌词文字
    int notePosition;  // 对应音符位置
    int lineNumber;    // 歌词行号
    
    LyricMapping() : notePosition(0), lineNumber(0) {}
    LyricMapping(const std::string& t, int pos, int line)
        : text(t), notePosition(pos), lineNumber(line) {}
};

// 小节线信息
struct BarlineInfo {
    int position;      // 位置
    bool isRepeatStart; // 是否为重复开始
    bool isRepeatEnd;   // 是否为重复结束
    int repeatCount;    // 重复次数
    
    BarlineInfo() : position(0), isRepeatStart(false), isRepeatEnd(false), repeatCount(0) {}
};

// 和弦信息
struct ChordInfo {
    std::string name;  // 和弦名称 (如 "Am", "C7")
    int position;      // 位置
    
    ChordInfo() : position(0) {}
    ChordInfo(const std::string& n, int pos) : name(n), position(pos) {}
};

// 调号结构
struct KeySignature {
    int sharps;        // 升号数量（负数表示降号）
    bool isMajor;      // 是否为大调
    
    KeySignature() : sharps(0), isMajor(true) {}
    KeySignature(int s, bool major) : sharps(s), isMajor(major) {}
};

// 拍号结构
struct TimeSignature {
    int numerator;     // 分子
    int denominator;   // 分母
    
    TimeSignature() : numerator(4), denominator(4) {}
    TimeSignature(int num, int den) : numerator(num), denominator(den) {}
};

// 六线谱识别结果
struct TablatureResult {
    std::vector<FretNote> notes;        // 品位音符
    std::vector<Duration> durations;    // 时值信息
    int stringCount;                    // 弦数（通常为6）
    
    TablatureResult() : stringCount(6) {}
};

// 简谱识别结果
struct NumberedResult {
    std::vector<NumberedNote> notes;    // 数字音符
    KeySignature keySignature;          // 调号（如1=C）
    std::vector<Duration> durations;    // 时值信息
    
    NumberedResult() {}
};

// 歌词识别结果
struct LyricsResult {
    std::vector<LyricMapping> lyrics;   // 歌词映射
    int totalLines;                     // 总行数
    
    LyricsResult() : totalLines(0) {}
};

// 单个音轨识别结果
struct TrackResult {
    int trackIndex;                     // 音轨编号
    ScoreType type;                     // 音轨类型
    TablatureResult tablature;          // 六线谱结果
    NumberedResult numbered;            // 简谱结果
    std::vector<ChordInfo> chords;      // 和弦标记
    
    TrackResult() : trackIndex(0), type(ScoreType::Unknown) {}
};

// 多音轨识别结果
struct MultiTrackResult {
    std::vector<TrackResult> tracks;    // 多个音轨的识别结果
    LyricsResult lyrics;                // 歌词（通常跨音轨）
    std::vector<BarlineInfo> barlines;  // 小节线（通常跨音轨）
    TimeSignature timeSignature;
    KeySignature keySignature;
    
    MultiTrackResult() {}
};

// 对比测试结果
struct ComparisonResult {
    float overallAccuracy;      // 总体准确率
    float noteAccuracy;         // 音符准确率
    float durationAccuracy;     // 时值准确率
    float signatureAccuracy;    // 调号拍号准确率
    float lyricsAccuracy;       // 歌词准确率
    std::vector<std::string> differences;  // 具体差异列表
    
    ComparisonResult() : overallAccuracy(0.0f), noteAccuracy(0.0f), 
                        durationAccuracy(0.0f), signatureAccuracy(0.0f), 
                        lyricsAccuracy(0.0f) {}
};

} // namespace ImageToGP

#endif // DATASTRUCTURES_H