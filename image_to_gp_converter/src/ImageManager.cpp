#include "ImageManager.h"
#include "RecognitionException.h"
#include <filesystem>
#include <algorithm>
#include <regex>
#include <set>
#include <cctype>
#include <chrono>

namespace ImageToGP {
namespace fs = std::filesystem;

ImageManager::ImageManager() {
}

ImageManager::~ImageManager() {
}

bool ImageManager::loadFromFolder(const std::string& folderPath) {
    try {
        // 清理之前的数据
        m_imageMap.clear();
        m_metadata.clear();
        m_folderPath = folderPath;
        
        // 验证文件夹是否存在
        if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
            throw RecognitionException(RecognitionError::FileNotFound,
                                     "Folder does not exist: " + folderPath);
        }
        
        // 扫描图片文件
        auto imageFiles = scanImageFiles(folderPath);
        if (imageFiles.empty()) {
            throw RecognitionException(RecognitionError::NoValidContent,
                                     "No valid image files found in folder: " + folderPath);
        }
        
        int successCount = 0;
        int failCount = 0;
        std::vector<std::string> failedFiles;
        
        // 解析文件名并加载图片
        for (const auto& filePath : imageFiles) {
            std::string fileName = fs::path(filePath).filename().string();
            std::string songName, identifier;
            int sequence;
            
            if (parseFileName(fileName, songName, identifier, sequence)) {
                try {
                    cv::Mat image = loadImage(filePath);
                    if (!image.empty()) {
                        // 检查是否有重复序号
                        if (m_imageMap.find(sequence) != m_imageMap.end()) {
                            throw RecognitionException(RecognitionError::InvalidSequence,
                                                     "Duplicate sequence number: " + std::to_string(sequence));
                        }
                        
                        m_imageMap[sequence] = image;
                        
                        ImageMetadata metadata;
                        metadata.index = sequence;
                        metadata.path = filePath;
                        metadata.size = image.size();
                        metadata.songName = songName;
                        metadata.identifier = identifier;
                        
                        // 获取文件信息
                        try {
                            if (fs::exists(filePath)) {
                                metadata.fileSize = fs::file_size(filePath);
                                
                                auto ftime = fs::last_write_time(filePath);
                                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
                                metadata.lastModified = std::chrono::system_clock::to_time_t(sctp);
                            }
                        } catch (const fs::filesystem_error&) {
                            metadata.fileSize = 0;
                            metadata.lastModified = 0;
                        }
                        
                        metadata.isProcessed = false;
                        m_metadata.push_back(metadata);
                        
                        successCount++;
                    }
                } catch (const RecognitionException& e) {
                    failCount++;
                    failedFiles.push_back(fileName + ": " + e.getMessage());
                }
            } else {
                failCount++;
                failedFiles.push_back(fileName + ": Invalid filename format");
            }
        }
        
        // 如果没有成功加载任何图片
        if (m_imageMap.empty()) {
            std::string errorMsg = "No images loaded successfully.";
            if (!failedFiles.empty()) {
                errorMsg += " Failed files:\n";
                for (const auto& fail : failedFiles) {
                    errorMsg += "  - " + fail + "\n";
                }
            }
            throw RecognitionException(RecognitionError::ProcessingFailed, errorMsg);
        }
        
        // 验证序号完整性
        if (!validateImageSequence()) {
            auto missing = getMissingSequences();
            if (!missing.empty()) {
                std::string missingStr = "Missing sequences: ";
                for (size_t i = 0; i < missing.size(); ++i) {
                    if (i > 0) missingStr += ", ";
                    missingStr += std::to_string(missing[i]);
                }
                throw RecognitionException(RecognitionError::InvalidSequence, missingStr);
            }
        }
        
        return true;
        
    } catch (const RecognitionException&) {
        // 重新抛出RecognitionException
        throw;
    } catch (const std::exception& e) {
        throw RecognitionException(RecognitionError::ProcessingFailed, 
                                 "Load folder failed", e.what());
    }
}

std::vector<cv::Mat> ImageManager::getSortedImages() const {
    std::vector<cv::Mat> sortedImages;
    
    for (const auto& pair : m_imageMap) {
        sortedImages.push_back(pair.second);
    }
    
    return sortedImages;
}

bool ImageManager::validateImageSequence() const {
    if (m_imageMap.empty()) {
        return false;
    }
    
    // 获取所有序号并排序
    std::vector<int> indices;
    for (const auto& pair : m_imageMap) {
        indices.push_back(pair.first);
    }
    std::sort(indices.begin(), indices.end());
    
    // 检查序号是否从1开始连续
    for (size_t i = 0; i < indices.size(); ++i) {
        int expectedIndex = static_cast<int>(i + 1);
        if (indices[i] != expectedIndex) {
            return false;
        }
    }
    
    return true;
}

ImageMetadata ImageManager::getImageInfo(int index) const {
    for (const auto& metadata : m_metadata) {
        if (metadata.index == index) {
            return metadata;
        }
    }
    return ImageMetadata();
}

std::vector<std::string> ImageManager::scanImageFiles(const std::string& folderPath) {
    std::vector<std::string> imageFiles;
    
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (isSupportedFormat(filename)) {
                    imageFiles.push_back(entry.path().string());
                }
            }
        }
        
        // 按文件名排序
        std::sort(imageFiles.begin(), imageFiles.end());
        
    } catch (const fs::filesystem_error& e) {
        throw RecognitionException(RecognitionError::FileNotFound,
                                 "Cannot access folder", e.what());
    }
    
    return imageFiles;
}

bool ImageManager::parseFileName(const std::string& fileName, std::string& songName, 
                                std::string& identifier, int& sequence) {
    try {
        // 解析格式: 曲名-标识#序号.png
        // 支持更灵活的格式，允许曲名和标识中包含中文字符
        std::regex pattern(R"((.+)-(.+)#(\d{2,3})\.(png|jpg|jpeg|bmp))", std::regex_constants::icase);
        std::smatch matches;
        
        if (std::regex_match(fileName, matches, pattern)) {
            songName = matches[1].str();
            identifier = matches[2].str();
            
            // 解析序号，确保是有效的数字
            std::string sequenceStr = matches[3].str();
            sequence = std::stoi(sequenceStr);
            
            // 验证序号范围（1-999）
            if (sequence < 1 || sequence > 999) {
                return false;
            }
            
            // 去除songName和identifier中的空格
            songName.erase(std::remove_if(songName.begin(), songName.end(), ::isspace), songName.end());
            identifier.erase(std::remove_if(identifier.begin(), identifier.end(), ::isspace), identifier.end());
            
            return !songName.empty() && !identifier.empty();
        }
        
        return false;
        
    } catch (const std::exception& e) {
        // 如果解析过程中出现异常，返回false
        return false;
    }
}

std::string ImageManager::findReferenceGPFile(const std::string& folderPath, 
                                             const std::string& songName, 
                                             const std::string& identifier) {
    // 尝试多种可能的GP文件扩展名
    std::vector<std::string> extensions = {".gp", ".gp7", ".gpx", ".gp5", ".gp4", ".gp3"};
    
    for (const auto& ext : extensions) {
        std::string gpFileName = songName + "-" + identifier + ext;
        std::string gpFilePath = (fs::path(folderPath) / gpFileName).string();
        
        if (fs::exists(gpFilePath)) {
            return gpFilePath;
        }
    }
    
    // 如果没有找到完全匹配的，尝试只用songName匹配
    for (const auto& ext : extensions) {
        std::string gpFileName = songName + ext;
        std::string gpFilePath = (fs::path(folderPath) / gpFileName).string();
        
        if (fs::exists(gpFilePath)) {
            return gpFilePath;
        }
    }
    
    return "";
}

bool ImageManager::isSupportedFormat(const std::string& filename) {
    std::string ext = fs::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // 支持更多图片格式
    static const std::set<std::string> supportedFormats = {
        ".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif", ".webp"
    };
    
    return supportedFormats.find(ext) != supportedFormats.end();
}

cv::Mat ImageManager::loadImage(const std::string& imagePath) {
    try {
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        if (image.empty()) {
            throw RecognitionException(RecognitionError::ProcessingFailed,
                                     "Cannot load image: " + imagePath);
        }
        
        // 验证图片质量
        if (!validateImageQuality(image)) {
            throw RecognitionException(RecognitionError::ProcessingFailed,
                                     "Image quality validation failed: " + imagePath);
        }
        
        return image;
    } catch (const cv::Exception& e) {
        throw RecognitionException(RecognitionError::OpenCVError,
                                 "OpenCV load image failed", e.what());
    }
}

size_t ImageManager::getImageCount() const {
    return m_imageMap.size();
}

std::vector<int> ImageManager::getMissingSequences() const {
    std::vector<int> missing;
    
    if (m_imageMap.empty()) {
        return missing;
    }
    
    // 找到最大序号
    int maxSequence = 0;
    for (const auto& pair : m_imageMap) {
        maxSequence = std::max(maxSequence, pair.first);
    }
    
    // 检查从1到maxSequence的所有序号
    for (int i = 1; i <= maxSequence; ++i) {
        if (m_imageMap.find(i) == m_imageMap.end()) {
            missing.push_back(i);
        }
    }
    
    return missing;
}

std::vector<int> ImageManager::getDuplicateSequences() const {
    std::vector<int> duplicates;
    std::map<int, int> sequenceCounts;
    
    // 统计每个序号的出现次数
    for (const auto& metadata : m_metadata) {
        sequenceCounts[metadata.index]++;
    }
    
    // 找出出现次数大于1的序号
    for (const auto& pair : sequenceCounts) {
        if (pair.second > 1) {
            duplicates.push_back(pair.first);
        }
    }
    
    return duplicates;
}

bool ImageManager::validateImageQuality(const cv::Mat& image) const {
    if (image.empty()) {
        return false;
    }
    
    // 检查图片尺寸（最小尺寸要求）
    const int MIN_WIDTH = 100;
    const int MIN_HEIGHT = 100;
    if (image.cols < MIN_WIDTH || image.rows < MIN_HEIGHT) {
        return false;
    }
    
    // 检查图片是否过大（避免内存问题）
    const int MAX_WIDTH = 10000;
    const int MAX_HEIGHT = 10000;
    if (image.cols > MAX_WIDTH || image.rows > MAX_HEIGHT) {
        return false;
    }
    
    // 检查图片是否为空白（所有像素值相同）
    cv::Scalar mean, stddev;
    cv::meanStdDev(image, mean, stddev);
    
    // 如果标准差太小，说明图片可能是空白的
    double totalStdDev = stddev[0] + stddev[1] + stddev[2];
    if (totalStdDev < 1.0) {
        return false;
    }
    
    return true;
}

const std::string& ImageManager::getFolderPath() const {
    return m_folderPath;
}

const std::vector<ImageMetadata>& ImageManager::getAllMetadata() const {
    return m_metadata;
}

cv::Mat ImageManager::getImageByIndex(int index) const {
    auto it = m_imageMap.find(index);
    if (it != m_imageMap.end()) {
        return it->second;
    }
    return cv::Mat(); // 返回空图片
}

ImageManager::FileInfo ImageManager::getFileInfo(int index) const {
    FileInfo info;
    
    // 查找对应的元数据
    for (const auto& metadata : m_metadata) {
        if (metadata.index == index) {
            fs::path filePath(metadata.path);
            
            // 解析文件名
            std::string fileName = filePath.filename().string();
            std::string songName, identifier;
            int sequence;
            
            if (const_cast<ImageManager*>(this)->parseFileName(fileName, songName, identifier, sequence)) {
                info.songName = songName;
                info.identifier = identifier;
            }
            
            info.extension = filePath.extension().string();
            
            // 获取文件大小
            try {
                if (fs::exists(filePath)) {
                    info.fileSize = fs::file_size(filePath);
                    
                    // 获取最后修改时间
                    auto ftime = fs::last_write_time(filePath);
                    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
                    info.lastModified = std::chrono::system_clock::to_time_t(sctp);
                }
            } catch (const fs::filesystem_error&) {
                info.fileSize = 0;
                info.lastModified = 0;
            }
            
            break;
        }
    }
    
    return info;
}

bool ImageManager::updateTrackRegions(int index, const std::vector<TrackRegion>& regions) {
    // 查找并更新对应的元数据
    for (auto& metadata : m_metadata) {
        if (metadata.index == index) {
            metadata.tracks = regions;
            return true;
        }
    }
    return false;
}

ImageManager::Statistics ImageManager::getStatistics() const {
    Statistics stats;
    stats.totalImages = m_imageMap.size();
    stats.totalFileSize = 0;
    
    int totalWidth = 0, totalHeight = 0;
    
    for (const auto& metadata : m_metadata) {
        // 累计文件大小
        try {
            if (fs::exists(metadata.path)) {
                stats.totalFileSize += fs::file_size(metadata.path);
            }
        } catch (const fs::filesystem_error&) {
            // 忽略文件系统错误
        }
        
        // 累计图片尺寸
        totalWidth += metadata.size.width;
        totalHeight += metadata.size.height;
        
        // 统计格式
        std::string ext = fs::path(metadata.path).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        stats.formatCounts[ext]++;
    }
    
    // 计算平均尺寸
    if (stats.totalImages > 0) {
        stats.averageImageSize.width = totalWidth / static_cast<int>(stats.totalImages);
        stats.averageImageSize.height = totalHeight / static_cast<int>(stats.totalImages);
    }
    
    // 设置支持的格式列表
    stats.supportedFormats = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".tif", ".webp"};
    
    return stats;
}

bool ImageManager::markAsProcessed(int index, bool success, const std::string& error) {
    for (auto& metadata : m_metadata) {
        if (metadata.index == index) {
            metadata.isProcessed = success;
            metadata.processingError = success ? "" : error;
            return true;
        }
    }
    return false;
}

std::vector<int> ImageManager::getUnprocessedImages() const {
    std::vector<int> unprocessed;
    for (const auto& metadata : m_metadata) {
        if (!metadata.isProcessed && metadata.processingError.empty()) {
            unprocessed.push_back(metadata.index);
        }
    }
    return unprocessed;
}

std::vector<int> ImageManager::getFailedImages() const {
    std::vector<int> failed;
    for (const auto& metadata : m_metadata) {
        if (!metadata.processingError.empty()) {
            failed.push_back(metadata.index);
        }
    }
    return failed;
}

void ImageManager::resetProcessingStatus() {
    for (auto& metadata : m_metadata) {
        metadata.isProcessed = false;
        metadata.processingError.clear();
    }
}

std::map<std::string, std::vector<int>> ImageManager::groupImagesBySong() const {
    std::map<std::string, std::vector<int>> groups;
    
    for (const auto& metadata : m_metadata) {
        groups[metadata.songName].push_back(metadata.index);
    }
    
    // 对每个组内的序号进行排序
    for (auto& pair : groups) {
        std::sort(pair.second.begin(), pair.second.end());
    }
    
    return groups;
}

std::map<std::string, std::vector<int>> ImageManager::groupImagesByIdentifier() const {
    std::map<std::string, std::vector<int>> groups;
    
    for (const auto& metadata : m_metadata) {
        groups[metadata.identifier].push_back(metadata.index);
    }
    
    // 对每个组内的序号进行排序
    for (auto& pair : groups) {
        std::sort(pair.second.begin(), pair.second.end());
    }
    
    return groups;
}

std::vector<int> ImageManager::getImagesBySong(const std::string& songName) const {
    std::vector<int> indices;
    
    for (const auto& metadata : m_metadata) {
        if (metadata.songName == songName) {
            indices.push_back(metadata.index);
        }
    }
    
    std::sort(indices.begin(), indices.end());
    return indices;
}

std::vector<int> ImageManager::getImagesByIdentifier(const std::string& identifier) const {
    std::vector<int> indices;
    
    for (const auto& metadata : m_metadata) {
        if (metadata.identifier == identifier) {
            indices.push_back(metadata.index);
        }
    }
    
    std::sort(indices.begin(), indices.end());
    return indices;
}

ImageManager::GPFileSearchResult ImageManager::findGPFileAdvanced(const std::string& songName, 
                                                                 const std::string& identifier) const {
    GPFileSearchResult result;
    result.exactMatch = false;
    
    if (m_folderPath.empty()) {
        result.matchType = "No folder path set";
        return result;
    }
    
    // 尝试多种可能的GP文件扩展名
    std::vector<std::string> extensions = {".gp", ".gp7", ".gpx", ".gp5", ".gp4", ".gp3"};
    
    // 策略1: 精确匹配 "songName-identifier.ext"
    for (const auto& ext : extensions) {
        std::string gpFileName = songName + "-" + identifier + ext;
        std::string gpFilePath = (fs::path(m_folderPath) / gpFileName).string();
        
        if (fs::exists(gpFilePath)) {
            result.filePath = gpFilePath;
            result.exactMatch = true;
            result.matchType = "Exact match (song-identifier)";
            return result;
        }
    }
    
    // 策略2: 只用songName匹配 "songName.ext"
    for (const auto& ext : extensions) {
        std::string gpFileName = songName + ext;
        std::string gpFilePath = (fs::path(m_folderPath) / gpFileName).string();
        
        if (fs::exists(gpFilePath)) {
            result.filePath = gpFilePath;
            result.exactMatch = false;
            result.matchType = "Song name match only";
            return result;
        }
    }
    
    // 策略3: 模糊匹配，查找包含songName的文件
    try {
        for (const auto& entry : fs::directory_iterator(m_folderPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                
                // 检查是否为GP文件
                bool isGPFile = false;
                for (const auto& gpExt : extensions) {
                    if (ext == gpExt) {
                        isGPFile = true;
                        break;
                    }
                }
                
                if (isGPFile) {
                    // 检查文件名是否包含songName
                    std::string lowerFilename = filename;
                    std::string lowerSongName = songName;
                    std::transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
                    std::transform(lowerSongName.begin(), lowerSongName.end(), lowerSongName.begin(), ::tolower);
                    
                    if (lowerFilename.find(lowerSongName) != std::string::npos) {
                        result.filePath = entry.path().string();
                        result.exactMatch = false;
                        result.matchType = "Fuzzy match (contains song name)";
                        return result;
                    }
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        result.matchType = "Filesystem error during search";
        return result;
    }
    
    result.matchType = "No matching GP file found";
    return result;
}

std::string ImageManager::exportMetadataToJSON() const {
    std::string json = "{\n";
    json += "  \"folderPath\": \"" + m_folderPath + "\",\n";
    json += "  \"totalImages\": " + std::to_string(m_metadata.size()) + ",\n";
    json += "  \"images\": [\n";
    
    for (size_t i = 0; i < m_metadata.size(); ++i) {
        const auto& metadata = m_metadata[i];
        
        json += "    {\n";
        json += "      \"index\": " + std::to_string(metadata.index) + ",\n";
        json += "      \"path\": \"" + metadata.path + "\",\n";
        json += "      \"songName\": \"" + metadata.songName + "\",\n";
        json += "      \"identifier\": \"" + metadata.identifier + "\",\n";
        json += "      \"width\": " + std::to_string(metadata.size.width) + ",\n";
        json += "      \"height\": " + std::to_string(metadata.size.height) + ",\n";
        json += "      \"fileSize\": " + std::to_string(metadata.fileSize) + ",\n";
        json += "      \"lastModified\": " + std::to_string(metadata.lastModified) + ",\n";
        json += "      \"isProcessed\": " + std::string(metadata.isProcessed ? "true" : "false") + ",\n";
        json += "      \"processingError\": \"" + metadata.processingError + "\",\n";
        json += "      \"trackCount\": " + std::to_string(metadata.tracks.size()) + "\n";
        json += "    }";
        
        if (i < m_metadata.size() - 1) {
            json += ",";
        }
        json += "\n";
    }
    
    json += "  ]\n";
    json += "}";
    
    return json;
}

bool ImageManager::importMetadataFromJSON(const std::string& jsonData) {
    // 简化的JSON解析实现
    // 注意：这是一个基础版本，实际项目中建议使用nlohmann/json库
    
    try {
        // 清理现有数据
        m_metadata.clear();
        m_imageMap.clear();
        
        // 解析folderPath
        size_t folderPathPos = jsonData.find("\"folderPath\":");
        if (folderPathPos != std::string::npos) {
            size_t startQuote = jsonData.find("\"", folderPathPos + 13);
            size_t endQuote = jsonData.find("\"", startQuote + 1);
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                m_folderPath = jsonData.substr(startQuote + 1, endQuote - startQuote - 1);
            }
        }
        
        // 查找images数组
        size_t imagesPos = jsonData.find("\"images\":");
        if (imagesPos == std::string::npos) {
            return false;
        }
        
        size_t arrayStart = jsonData.find("[", imagesPos);
        size_t arrayEnd = jsonData.rfind("]");
        
        if (arrayStart == std::string::npos || arrayEnd == std::string::npos) {
            return false;
        }
        
        // 简单解析每个图片对象
        std::string imagesArray = jsonData.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
        
        // 按对象分割（查找 { 和 } 配对）
        size_t pos = 0;
        int braceCount = 0;
        size_t objStart = 0;
        
        while (pos < imagesArray.length()) {
            if (imagesArray[pos] == '{') {
                if (braceCount == 0) {
                    objStart = pos;
                }
                braceCount++;
            } else if (imagesArray[pos] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    // 解析单个图片对象
                    std::string objStr = imagesArray.substr(objStart, pos - objStart + 1);
                    parseImageObjectFromJSON(objStr);
                }
            }
            pos++;
        }
        
        return !m_metadata.empty();
        
    } catch (const std::exception& e) {
        return false;
    }
}

// 添加辅助函数来解析单个图片对象
void ImageManager::parseImageObjectFromJSON(const std::string& objStr) {
    ImageMetadata metadata;
    
    // 解析各个字段
    auto extractStringValue = [&](const std::string& key) -> std::string {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = objStr.find(searchKey);
        if (pos != std::string::npos) {
            size_t startQuote = objStr.find("\"", pos + searchKey.length());
            size_t endQuote = objStr.find("\"", startQuote + 1);
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                return objStr.substr(startQuote + 1, endQuote - startQuote - 1);
            }
        }
        return "";
    };
    
    auto extractIntValue = [&](const std::string& key) -> int {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = objStr.find(searchKey);
        if (pos != std::string::npos) {
            size_t numStart = pos + searchKey.length();
            while (numStart < objStr.length() && std::isspace(objStr[numStart])) numStart++;
            size_t numEnd = numStart;
            while (numEnd < objStr.length() && std::isdigit(objStr[numEnd])) numEnd++;
            if (numEnd > numStart) {
                return std::stoi(objStr.substr(numStart, numEnd - numStart));
            }
        }
        return 0;
    };
    
    auto extractBoolValue = [&](const std::string& key) -> bool {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = objStr.find(searchKey);
        if (pos != std::string::npos) {
            size_t valueStart = pos + searchKey.length();
            while (valueStart < objStr.length() && std::isspace(objStr[valueStart])) valueStart++;
            return objStr.substr(valueStart, 4) == "true";
        }
        return false;
    };
    
    // 提取字段值
    metadata.index = extractIntValue("index");
    metadata.path = extractStringValue("path");
    metadata.songName = extractStringValue("songName");
    metadata.identifier = extractStringValue("identifier");
    metadata.size.width = extractIntValue("width");
    metadata.size.height = extractIntValue("height");
    metadata.fileSize = static_cast<size_t>(extractIntValue("fileSize"));
    metadata.lastModified = static_cast<std::time_t>(extractIntValue("lastModified"));
    metadata.isProcessed = extractBoolValue("isProcessed");
    metadata.processingError = extractStringValue("processingError");
    
    if (metadata.index > 0) {
        m_metadata.push_back(metadata);
    }
}

} // namespace ImageToGP