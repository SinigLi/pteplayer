#include "GPConverter.h"
#include "RecognitionException.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace ImageToGP {

GPConverter::GPConverter() {
    // 初始化统计信息
    m_lastStats = ConversionStatistics();
}

GPConverter::~GPConverter() {
}

bool GPConverter::convertToGP(const MultiTrackResult& result, const std::string& outputPath) {
    // 重置统计信息
    m_lastStats = ConversionStatistics();
    m_lastStats.outputPath = outputPath;
    m_lastStats.totalTracks = static_cast<int>(result.tracks.size());
    m_lastStats.totalMeasures = static_cast<int>(result.barlines.size());
    
    try {
        std::cout << "Converting multi-track result to GP file: " << outputPath << std::endl;
        
        // 计算总音符数
        for (const auto& track : result.tracks) {
            if (track.type == ScoreType::Tablature) {
                m_lastStats.totalNotes += static_cast<int>(track.tablature.notes.size());
            } else if (track.type == ScoreType::Numbered) {
                m_lastStats.totalNotes += static_cast<int>(track.numbered.notes.size());
            }
        }
        
        // 根据文件扩展名选择格式
        std::string extension = std::filesystem::path(outputPath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        bool success = false;
        if (extension == ".gp7") {
            m_lastStats.outputFormat = "GP7";
            success = convertToGP7(result, outputPath);
        } else if (extension == ".gpx") {
            m_lastStats.outputFormat = "GPX";
            success = convertToGPX(result, outputPath);
        } else {
            // 默认使用GP7格式
            m_lastStats.outputFormat = "GP7";
            std::string gp7Path = outputPath;
            if (extension.empty()) {
                gp7Path += ".gp7";
            }
            success = convertToGP7(result, gp7Path);
            m_lastStats.outputPath = gp7Path;
        }
        
        if (success) {
            // 验证生成的文件
            success = validateGPFile(m_lastStats.outputPath);
            if (!success) {
                addError("Generated GP file validation failed");
            }
        }
        
        m_lastStats.success = success;
        
        std::cout << "GP conversion " << (success ? "completed successfully" : "failed") 
                  << ". Format: " << m_lastStats.outputFormat 
                  << ", Notes: " << m_lastStats.totalNotes << std::endl;
        
        return success;
        
    } catch (const std::exception& e) {
        addError("GP conversion failed: " + std::string(e.what()));
        m_lastStats.success = false;
        return false;
    }
}

bool GPConverter::convertToGP7(const MultiTrackResult& result, const std::string& outputPath) {
    try {
        std::cout << "Converting to GP7 format..." << std::endl;
        
        // 写入文件
        return writeGP7File(result, outputPath);
        
    } catch (const std::exception& e) {
        addError("GP7 conversion failed: " + std::string(e.what()));
        return false;
    }
}

bool GPConverter::convertToGPX(const MultiTrackResult& result, const std::string& outputPath) {
    try {
        std::cout << "Converting to GPX format..." << std::endl;
        
        // 写入文件
        return writeGPXFile(result, outputPath);
        
    } catch (const std::exception& e) {
        addError("GPX conversion failed: " + std::string(e.what()));
        return false;
    }
}

GPConverter::ConversionStatistics GPConverter::getLastConversionStats() const {
    return m_lastStats;
}

bool GPConverter::validateGPFile(const std::string& path) {
    try {
        // 基础的文件存在性检查
        if (!std::filesystem::exists(path)) {
            addError("Output file does not exist: " + path);
            return false;
        }
        
        auto fileSize = std::filesystem::file_size(path);
        if (fileSize == 0) {
            addError("Output file is empty: " + path);
            return false;
        }
        
        if (fileSize < 100) {
            addWarning("Output file seems too small: " + std::to_string(fileSize) + " bytes");
        }
        
        std::cout << "GP file validation passed. Size: " << fileSize << " bytes" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        addError("File validation failed: " + std::string(e.what()));
        return false;
    }
}

// 内部转换方法实现
bool GPConverter::writeGP7File(const MultiTrackResult& result, const std::string& path) {
    try {
        // 简化实现：创建一个基础的GP7文件结构
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            addError("Cannot create output file: " + path);
            return false;
        }
        
        // GP7文件头（简化版本）
        std::string header = "FICHIER GUITAR PRO v7.00";
        file.write(header.c_str(), header.length());
        
        // 写入基础信息
        std::string title = "Converted from Image";
        std::string artist = "Image Recognition";
        
        // 简化的GP7格式数据
        size_t trackCount = result.tracks.size();
        file.write(reinterpret_cast<const char*>(&trackCount), sizeof(size_t));
        
        for (const auto& track : result.tracks) {
            // 写入音轨信息
            int trackType = static_cast<int>(track.type);
            file.write(reinterpret_cast<const char*>(&trackType), sizeof(int));
            
            if (track.type == ScoreType::Tablature) {
                size_t noteCount = track.tablature.notes.size();
                file.write(reinterpret_cast<const char*>(&noteCount), sizeof(size_t));
                
                for (const auto& note : track.tablature.notes) {
                    file.write(reinterpret_cast<const char*>(&note.string), sizeof(int));
                    file.write(reinterpret_cast<const char*>(&note.fret), sizeof(int));
                    file.write(reinterpret_cast<const char*>(&note.position), sizeof(int));
                }
            }
        }
        
        file.close();
        
        std::cout << "GP7 file written successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        addError("GP7 file writing failed: " + std::string(e.what()));
        return false;
    }
}

bool GPConverter::writeGPXFile(const MultiTrackResult& result, const std::string& path) {
    try {
        // GPX是基于XML的格式，创建简化的XML结构
        std::ofstream file(path);
        if (!file.is_open()) {
            addError("Cannot create output file: " + path);
            return false;
        }
        
        // XML头
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<gpx version=\"7.0\">\n";
        file << "  <score>\n";
        file << "    <title>Converted from Image</title>\n";
        file << "    <artist>Image Recognition</artist>\n";
        
        // 写入音轨信息
        file << "    <tracks>\n";
        for (size_t i = 0; i < result.tracks.size(); ++i) {
            const auto& track = result.tracks[i];
            file << "      <track id=\"" << i << "\" type=\"" << static_cast<int>(track.type) << "\">\n";
            
            if (track.type == ScoreType::Tablature) {
                file << "        <notes>\n";
                for (const auto& note : track.tablature.notes) {
                    file << "          <note string=\"" << note.string 
                         << "\" fret=\"" << note.fret 
                         << "\" position=\"" << note.position << "\"/>\n";
                }
                file << "        </notes>\n";
            }
            
            file << "      </track>\n";
        }
        file << "    </tracks>\n";
        
        // 写入小节线信息
        if (!result.barlines.empty()) {
            file << "    <barlines>\n";
            for (const auto& barline : result.barlines) {
                file << "      <barline position=\"" << barline.position << "\"/>\n";
            }
            file << "    </barlines>\n";
        }
        
        file << "  </score>\n";
        file << "</gpx>\n";
        
        file.close();
        
        std::cout << "GPX file written successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        addError("GPX file writing failed: " + std::string(e.what()));
        return false;
    }
}

std::string GPConverter::generateGP7Content(const MultiTrackResult& result) {
    try {
        std::ostringstream content;
        
        // 生成GP7格式的内容描述
        content << "GP7 Content Summary:\n";
        content << "Tracks: " << result.tracks.size() << "\n";
        content << "Barlines: " << result.barlines.size() << "\n";
        content << "Lyrics: " << result.lyrics.lyrics.size() << "\n";
        
        return content.str();
        
    } catch (const std::exception& e) {
        addError("GP7 content generation failed: " + std::string(e.what()));
        return "";
    }
}

std::string GPConverter::generateGPXContent(const MultiTrackResult& result) {
    try {
        std::ostringstream content;
        
        // 生成GPX格式的内容描述
        content << "GPX Content Summary:\n";
        content << "Tracks: " << result.tracks.size() << "\n";
        content << "Barlines: " << result.barlines.size() << "\n";
        content << "Lyrics: " << result.lyrics.lyrics.size() << "\n";
        
        return content.str();
        
    } catch (const std::exception& e) {
        addError("GPX content generation failed: " + std::string(e.what()));
        return "";
    }
}

void GPConverter::addWarning(const std::string& warning) {
    m_lastStats.warnings.push_back(warning);
    std::cout << "Warning: " << warning << std::endl;
}

void GPConverter::addError(const std::string& error) {
    m_lastStats.errors.push_back(error);
    std::cerr << "Error: " << error << std::endl;
}

} // namespace ImageToGP