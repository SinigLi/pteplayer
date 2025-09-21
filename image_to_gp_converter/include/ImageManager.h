#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "DataStructures.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <map>
#include <ctime>

namespace ImageToGP {

class ImageManager {
public:
    ImageManager();
    ~ImageManager();
    
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
                                   const std::string& identifier);
    
    // 获取图片总数
    size_t getImageCount() const;
    
    // 获取缺失的序号列表
    std::vector<int> getMissingSequences() const;
    
    // 获取重复的序号列表
    std::vector<int> getDuplicateSequences() const;
    
    // 验证图片质量
    bool validateImageQuality(const cv::Mat& image) const;
    
    // 获取文件夹路径
    const std::string& getFolderPath() const;
    
    // 获取所有元数据
    const std::vector<ImageMetadata>& getAllMetadata() const;
    
    // 根据序号获取图片
    cv::Mat getImageByIndex(int index) const;
    
    // 获取图片的文件信息
    struct FileInfo {
        std::string songName;
        std::string identifier;
        std::string extension;
        size_t fileSize;
        std::time_t lastModified;
    };
    FileInfo getFileInfo(int index) const;
    
    // 更新图片的音轨区域信息
    bool updateTrackRegions(int index, const std::vector<TrackRegion>& regions);
    
    // 获取统计信息
    struct Statistics {
        size_t totalImages;
        size_t totalFileSize;
        cv::Size averageImageSize;
        std::vector<std::string> supportedFormats;
        std::map<std::string, int> formatCounts;
    };
    Statistics getStatistics() const;
    
    // 标记图片处理状态
    bool markAsProcessed(int index, bool success = true, const std::string& error = "");
    
    // 获取未处理的图片列表
    std::vector<int> getUnprocessedImages() const;
    
    // 获取处理失败的图片列表
    std::vector<int> getFailedImages() const;
    
    // 重置所有处理状态
    void resetProcessingStatus();
    
    // 按歌曲名称分组图片
    std::map<std::string, std::vector<int>> groupImagesBySong() const;
    
    // 按标识符分组图片
    std::map<std::string, std::vector<int>> groupImagesByIdentifier() const;
    
    // 获取指定歌曲的所有图片序号
    std::vector<int> getImagesBySong(const std::string& songName) const;
    
    // 获取指定标识符的所有图片序号
    std::vector<int> getImagesByIdentifier(const std::string& identifier) const;
    
    // 查找对应的GP文件（支持多种查找策略）
    struct GPFileSearchResult {
        std::string filePath;
        bool exactMatch;        // 是否精确匹配
        std::string matchType;  // 匹配类型描述
    };
    GPFileSearchResult findGPFileAdvanced(const std::string& songName, 
                                         const std::string& identifier) const;
    
    // 导出元数据到JSON格式
    std::string exportMetadataToJSON() const;
    
    // 从JSON导入元数据
    bool importMetadataFromJSON(const std::string& jsonData);
    
private:
    std::map<int, cv::Mat> m_imageMap;      // 下标到图片的映射
    std::vector<ImageMetadata> m_metadata;  // 图片元数据
    std::string m_folderPath;               // 输入文件夹路径
    
    // 检查图片格式是否支持
    bool isSupportedFormat(const std::string& filename);
    
    // 加载单张图片
    cv::Mat loadImage(const std::string& imagePath);
    
    // 从JSON字符串解析单个图片对象
    void parseImageObjectFromJSON(const std::string& objStr);
};

} // namespace ImageToGP

#endif // IMAGEMANAGER_H