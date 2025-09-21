#ifndef GPCONVERTER_H
#define GPCONVERTER_H

#include <string>
#include "DataStructures.h"
#include <memory>

namespace ImageToGP {

class GPConverter {
public:
    GPConverter();
    ~GPConverter();
    
    // 转换MultiTrackResult为GP文件
    bool convertToGP(const MultiTrackResult& result, const std::string& outputPath);
    
    // 转换为GP7格式
    bool convertToGP7(const MultiTrackResult& result, const std::string& outputPath);
    
    // 转换为GPX格式
    bool convertToGPX(const MultiTrackResult& result, const std::string& outputPath);
    
    // 获取转换统计信息
    struct ConversionStatistics {
        int totalTracks;
        int totalNotes;
        int totalMeasures;
        std::string outputFormat;
        std::string outputPath;
        bool success;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };
    ConversionStatistics getLastConversionStats() const;
    
private:
    // 内部转换方法
    bool writeGP7File(const MultiTrackResult& result, const std::string& path);
    bool writeGPXFile(const MultiTrackResult& result, const std::string& path);
    
    // 验证生成的GP文件
    bool validateGPFile(const std::string& path);
    
    // 辅助方法
    std::string generateGP7Content(const MultiTrackResult& result);
    std::string generateGPXContent(const MultiTrackResult& result);
    void addWarning(const std::string& warning);
    void addError(const std::string& error);
    
    // 统计信息
    ConversionStatistics m_lastStats;
};

} // namespace ImageToGP

#endif // GPCONVERTER_H