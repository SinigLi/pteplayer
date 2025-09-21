# 图片转GP文件设计文档

## 概述

本设计文档描述了将文件夹中一组带有下标的乐谱图片转换为GP文件的技术实现方案。系统采用传统图像处理技术，支持单张图片包含多个音轨（六线谱+简谱），优先支持六线谱、简谱和歌词识别，最终输出标准的Score数据结构并转换为GP文件格式。

## 架构

### 整体架构图

```
输入文件夹 → 图片扫描 → 图片预处理 → 多音轨识别 → 专用识别引擎 → Score构建器 → GP文件输出
     ↓         ↓         ↓          ↓          ↓            ↓           ↓
  文件夹遍历  下标排序   图像清理    音轨分离    六线谱/简谱   多音轨合并   现有转换接口
             文件验证   二值化      区域检测    歌词识别      数据结构
```

### 模块划分

1. **图片管理模块** (ImageManager)
2. **图像预处理模块** (ImagePreprocessor) 
3. **乐谱识别引擎** (ScoreRecognizer)
4. **Score构建模块** (ScoreBuilder)
5. **GP转换模块** (GPConverter)

## 组件和接口

### 1. 图片管理模块 (ImageManager)

**职责：** 管理输入图片的排序、验证和批量处理

```cpp
class ImageManager {
public:
    // 从文件夹加载图片组
    bool loadFromFolder(const std::string& folderPath);
    
    // 按下标排序图片
    std::vector<cv::Mat> getSortedImages() const;
    
    // 验证图片完整性
    bool validateImageSequence() const;
    
    // 获取图片元信息
    ImageMetadata getImageInfo(int index) const;
    
    // 扫描文件夹中的图片文件
    std::vector<std::string> scanImageFiles(const std::string& folderPath);
    
    // 解析文件名获取曲名、标识和序号
    bool parseFileName(const std::string& fileName, std::string& songName, 
                      std::string& identifier, int& sequence);
    
    // 查找对应的GP文件
    std::string findReferenceGPFile(const std::string& folderPath, 
                                   const std::string& songName, 
                                   const std::string& key);
    
private:
    std::map<int, cv::Mat> m_imageMap;  // 下标到图片的映射
    std::vector<ImageMetadata> m_metadata;
    std::string m_folderPath;           // 输入文件夹路径
};

struct ImageMetadata {
    int index;                          // 图片下标
    std::string path;                   // 文件路径
    cv::Size size;                      // 图片尺寸
    std::vector<TrackRegion> tracks;    // 图片中的音轨区域
};

struct TrackRegion {
    cv::Rect region;     // 音轨在图片中的区域
    ScoreType type;      // 音轨类型（六线谱/简谱）
    int trackIndex;      // 音轨编号
};
```

### 2. 图像预处理模块 (ImagePreprocessor)

**职责：** 对图片进行标准化预处理，为识别做准备

```cpp
class ImagePreprocessor {
public:
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
};

enum class ScoreType {
    Tablature,    // 六线谱
    Numbered,     // 简谱
    Staff,        // 五线谱
    Mixed,        // 混合类型（同时包含多种）
    Unknown       // 未知类型
};
```

### 3. 乐谱识别引擎 (ScoreRecognizer)

**职责：** 根据乐谱类型调用相应的识别算法，支持多音轨识别

```cpp
class ScoreRecognizer {
public:
    // 识别单张图片的多个音轨
    MultiTrackResult recognizeImage(const cv::Mat& image, 
                                   const std::vector<TrackRegion>& tracks);
    
    // 识别单个音轨区域
    TrackResult recognizeTrack(const cv::Mat& trackImage, ScoreType type);
    
private:
    std::unique_ptr<TablatureRecognizer> m_tabRecognizer;
    std::unique_ptr<NumberedRecognizer> m_numRecognizer;
    std::unique_ptr<LyricsRecognizer> m_lyricsRecognizer;
};

struct MultiTrackResult {
    std::vector<TrackResult> tracks;    // 多个音轨的识别结果
    LyricsResult lyrics;                // 歌词（通常跨音轨）
    std::vector<BarlineInfo> barlines;  // 小节线（通常跨音轨）
    TimeSignature timeSignature;
    KeySignature keySignature;
};

struct TrackResult {
    int trackIndex;                     // 音轨编号
    ScoreType type;                     // 音轨类型
    TablatureResult tablature;          // 六线谱结果
    NumberedResult numbered;            // 简谱结果
    std::vector<ChordInfo> chords;      // 和弦标记
};

// 六线谱识别器
class TablatureRecognizer {
public:
    TablatureResult recognize(const cv::Mat& image);
    
private:
    // 识别六条线
    std::vector<cv::Vec4i> detectTabLines(const cv::Mat& image);
    
    // 识别品位数字
    std::vector<FretNote> recognizeFretNumbers(const cv::Mat& image);
    
    // 识别时值标记
    std::vector<Duration> recognizeDurations(const cv::Mat& image);
};

// 简谱识别器
class NumberedRecognizer {
public:
    NumberedResult recognize(const cv::Mat& image);
    
private:
    // 识别数字音符
    std::vector<NumberedNote> recognizeNumbers(const cv::Mat& image);
    
    // 识别高低音点
    std::vector<OctaveMarker> recognizeOctaveMarkers(const cv::Mat& image);
    
    // 识别调号标记
    KeySignature recognizeKeySignature(const cv::Mat& image);
};

// 歌词识别器
class LyricsRecognizer {
public:
    LyricsResult recognize(const cv::Mat& image);
    
private:
    // OCR文字识别
    std::vector<TextRegion> extractTextRegions(const cv::Mat& image);
    
    // 建立歌词与音符的位置关系
    std::vector<LyricMapping> mapLyricsToNotes(
        const std::vector<TextRegion>& lyrics,
        const std::vector<NotePosition>& notes);
};
```

### 4. Score构建模块 (ScoreBuilder)

**职责：** 将多音轨识别结果转换为Score数据结构

```cpp
class ScoreBuilder {
public:
    // 构建完整的Score对象（支持多音轨）
    Score buildScore(const std::vector<MultiTrackResult>& results);
    
private:
    // 创建System结构（包含多个Staff）
    System createSystem(const MultiTrackResult& result);
    
    // 为每个音轨创建Staff
    Staff createStaffForTrack(const TrackResult& trackResult);
    
    // 创建六线谱Staff
    Staff createTablatureStaff(const TablatureResult& tabResult);
    
    // 创建简谱Staff（转换为标准记谱法）
    Staff createNumberedStaff(const NumberedResult& numResult);
    
    // 创建Position和Note
    std::vector<Position> createPositions(const std::vector<FretNote>& notes);
    
    // 合并多张图片的内容
    void mergeResults(Score& score, const std::vector<MultiTrackResult>& results);
    
    // 处理小节线和重复记号（跨音轨）
    void processBarlines(System& system, const std::vector<BarlineInfo>& barlines);
    
    // 处理和弦标记
    void processChords(System& system, const std::vector<ChordInfo>& chords);
    
    // 处理歌词（跨音轨）
    void processLyrics(System& system, const LyricsResult& lyrics);
    
    // 同步多音轨的时间对齐
    void synchronizeTracks(std::vector<Staff>& staves);
};
```

### 5. GP转换模块 (GPConverter)

**职责：** 调用现有的GP格式转换接口

```cpp
class GPConverter {
public:
    // 转换Score为GP文件
    bool convertToGP(const Score& score, const std::string& outputPath);
    
private:
    // 调用现有的GP7导出功能
    bool exportToGP7(const Score& score, const std::string& path);
    
    // 验证生成的GP文件
    bool validateGPFile(const std::string& path);
};
```

### 6. 测试对比模块 (TestComparator)

**职责：** 用于集成测试时对比识别结果与标准GP文件

```cpp
class TestComparator {
public:
    // 对比两个Score对象
    ComparisonResult compareScores(const Score& recognized, const Score& reference);
    
    // 从GP文件加载Score对象
    Score loadScoreFromGP(const std::string& gpFilePath);
    
    // 生成详细的对比报告
    std::string generateReport(const ComparisonResult& result);
    
private:
    // 对比音符准确率
    float compareNotes(const Score& recognized, const Score& reference);
    
    // 对比时值准确率
    float compareDurations(const Score& recognized, const Score& reference);
    
    // 对比调性和拍号
    float compareSignatures(const Score& recognized, const Score& reference);
    
    // 对比歌词
    float compareLyrics(const Score& recognized, const Score& reference);
};

struct ComparisonResult {
    float overallAccuracy;      // 总体准确率
    float noteAccuracy;         // 音符准确率
    float durationAccuracy;     // 时值准确率
    float signatureAccuracy;    // 调号拍号准确率
    float lyricsAccuracy;       // 歌词准确率
    std::vector<std::string> differences;  // 具体差异列表
};
```

## 数据模型

### 识别结果数据结构

```cpp
// 已在上面定义了MultiTrackResult和TrackResult

struct TablatureResult {
    std::vector<FretNote> notes;        // 品位音符
    std::vector<Duration> durations;    // 时值信息
    int stringCount;                    // 弦数（通常为6）
};

struct NumberedResult {
    std::vector<NumberedNote> notes;    // 数字音符
    KeySignature keySignature;          // 调号（如1=C）
    std::vector<Duration> durations;    // 时值信息
};

struct FretNote {
    int string;        // 弦号 (0-5)
    int fret;          // 品位 (0-24)
    int position;      // 在图片中的x坐标
    Duration duration; // 时值
};

struct NumberedNote {
    int number;        // 音符数字 (1-7)
    int octave;        // 八度 (-2 到 +2)
    int position;      // 位置
    Duration duration; // 时值
};

struct LyricMapping {
    std::string text;  // 歌词文字
    int notePosition;  // 对应音符位置
    int lineNumber;    // 歌词行号
};
```

## 错误处理

### 错误类型定义

```cpp
enum class RecognitionError {
    UnsupportedFormat,     // 不支持的图片格式
    InvalidSequence,       // 图片下标不完整
    NoValidContent,        // 无有效乐谱内容
    ProcessingFailed,      // 处理过程异常
    ConversionFailed       // GP转换失败
};

class RecognitionException : public std::exception {
public:
    RecognitionException(RecognitionError error, const std::string& message);
    const char* what() const noexcept override;
    RecognitionError getErrorType() const;
    
private:
    RecognitionError m_error;
    std::string m_message;
};
```

### 错误处理策略

1. **输入验证**：检查图片格式、下标完整性
2. **识别容错**：部分识别失败时继续处理其他部分
3. **用户反馈**：提供具体的错误信息和建议
4. **日志记录**：记录详细的处理过程用于调试

## 测试策略

### 单元测试

- **图片预处理测试**：验证二值化、去噪、校正效果
- **识别算法测试**：使用标准测试图片验证识别准确率
- **数据转换测试**：验证Score对象构建的正确性

### 集成测试

- **端到端测试**：完整的图片到GP文件转换流程
- **多图片测试**：验证图片序列的正确处理
- **格式兼容测试**：确保生成的GP文件能在Guitar Pro中正常打开
- **对比测试**：使用提供的测试数据集进行准确性验证

#### 测试数据集结构
```
测试数据集/
├── 谱子1/
│   ├── 彩虹-C.gp           # 标准GP文件
│   ├── 彩虹-C#01.png       # 第1页图片
│   ├── 彩虹-C#02.png       # 第2页图片
│   ├── 彩虹-C#03.png       # 第3页图片
│   └── ...
├── 谱子2/
│   ├── 歌曲名-调性.gp
│   ├── 歌曲名-调性#01.png
│   ├── 歌曲名-调性#02.png
│   └── ...
└── ...
```

#### 文件命名规则
- **GP文件**：`曲名-标识.gp`（如：彩虹-C.gp）
- **图片文件**：`曲名-标识#序号.png`（如：彩虹-C#01.png, 彩虹-C#02.png）
- **序号格式**：两位数字，从01开始递增
- **注意**：文件名中的"标识"部分仅用于文件配对，实际调性信息以GP文件内容为准

#### 对比测试流程
1. **加载标准答案**：读取reference.gp文件内容作为标准
2. **执行识别**：对png图片组进行识别转换
3. **结果对比**：比较识别结果与标准GP文件的差异
4. **准确率统计**：计算音符、时值、调性等各项识别准确率

### 性能测试

- **处理速度**：单张图片的识别时间应控制在5秒内
- **内存使用**：处理大图片时的内存占用优化
- **批量处理**：多张图片的并行处理能力

## 技术选型

### 图像处理库
- **OpenCV 4.x**：主要的图像处理和计算机视觉库
- **Tesseract OCR**：用于歌词文字识别

### 开发工具
- **C++17**：符合项目现有技术栈
- **CMake**：构建系统集成
- **Qt6**：如需GUI界面支持

### 第三方依赖
- **opencv**：图像处理核心库
- **tesseract**：OCR文字识别
- **boost**：现有项目依赖，用于文件系统操作
## 多音轨支
持特性

### 音轨检测策略
1. **六线谱检测**：识别6条平行水平线的区域
2. **简谱检测**：识别数字音符密集的区域  
3. **区域分离**：将不同音轨区域分别提取处理
4. **时间对齐**：确保多音轨在时间轴上的同步

### 文件夹输入处理
1. **文件扫描**：遍历指定文件夹中的所有图片文件
2. **下标解析**：从文件名中提取数字下标进行排序
3. **格式验证**：支持jpg、png、bmp等常见图片格式
4. **批量处理**：按顺序处理每张图片的多个音轨

### 数据合并策略
1. **音轨映射**：将识别的音轨映射到Score的不同Staff
2. **时间同步**：确保多音轨的节拍对齐
3. **小节对应**：保持跨音轨的小节线一致性
4. **歌词关联**：将歌词正确关联到对应的音轨和位置