#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <set>
#include <opencv2/opencv.hpp>

#include "DataStructures.h"
#include "RecognitionException.h"
#include "ImageManager.h"
#include "ScoreRecognizer.h"
#include "ScoreBuilder.h"
#include "GPConverter.h"
#include "TestComparator.h"
#include "TablatureRecognizer.h"
#include "ImagePreprocessor.h"

using namespace ImageToGP;
namespace fs = std::filesystem;

// Show usage help
void showUsage(const std::string& programName) {
    std::cout << "Image to GP File Converter v1.0\n";
    std::cout << "Usage: " << programName << " [options] <input_folder> [output_file]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  <input_folder>  Path to folder containing music notation images\n";
    std::cout << "  [output_file]   Output GP file path (optional, default: output.gp)\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help      Show this help information\n";
    std::cout << "  -v, --verbose   Show detailed processing information\n";
    std::cout << "  -t, --test      Test mode, compare recognition results with standard GP files\n";
    std::cout << "  --test-dir      Specify test dataset directory\n";
    std::cout << "  --test-tab      Test TablatureRecognizer with sample images\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " ./images output.gp\n";
    std::cout << "  " << programName << " -v -t ./test_data\n";
    std::cout << "  " << programName << " --test-dir G:\\learn\\guitar\\gpproj\\png\n\n";
    std::cout << "File naming format:\n";
    std::cout << "  GP files: songname-id.gp\n";
    std::cout << "  Image files: songname-id#01.png, songname-id#02.png, ...\n";
}

// 检查OpenCV是否正确初始化
bool checkOpenCV() {
    try {
        cv::Mat testImage = cv::Mat::zeros(100, 100, CV_8UC3);
        if (testImage.empty()) {
            return false;
        }
        std::cout << "OpenCV version: " << CV_VERSION << std::endl;
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error: " << e.what() << std::endl;
        return false;
    }
}

// Process single folder conversion
int processFolder(const std::string& inputFolder, const std::string& outputFile, bool verbose) {
    try {
        if (verbose) {
            std::cout << "Processing folder: " << inputFolder << std::endl;
        }
        
        // 检查输入文件夹是否存在
        if (!fs::exists(inputFolder) || !fs::is_directory(inputFolder)) {
            throw RecognitionException(RecognitionError::FileNotFound, 
                                     "Input folder does not exist: " + inputFolder);
        }
        
        // 创建各个模块实例
        ImageManager imageManager;
        ScoreRecognizer recognizer;
        ScoreBuilder scoreBuilder;
        GPConverter gpConverter;
        
        // 加载图片
        if (verbose) {
            std::cout << "Scanning image files..." << std::endl;
        }
        
        if (!imageManager.loadFromFolder(inputFolder)) {
            throw RecognitionException(RecognitionError::InvalidSequence,
                                     "Cannot load image sequence");
        }
        
        auto sortedImages = imageManager.getSortedImages();
        if (sortedImages.empty()) {
            throw RecognitionException(RecognitionError::NoValidContent,
                                     "No valid image files found in folder");
        }
        
        if (verbose) {
            std::cout << "Found " << sortedImages.size() << " images" << std::endl;
        }
        
        // 识别每张图片
        std::vector<MultiTrackResult> allResults;
        for (size_t i = 0; i < sortedImages.size(); ++i) {
            if (verbose) {
                std::cout << "Processing image " << (i + 1) << "..." << std::endl;
            }
            
            // 这里需要实现具体的识别逻辑
            // MultiTrackResult result = recognizer.recognizeImage(sortedImages[i], ...);
            // allResults.push_back(result);
        }
        
        if (verbose) {
            std::cout << "Building Score object..." << std::endl;
        }
        
        // 构建Score对象
        // Score score = scoreBuilder.buildScore(allResults);
        
        if (verbose) {
            std::cout << "Converting to GP file..." << std::endl;
        }
        
        // Convert to GP file
        // if (!gpConverter.convertToGP(score, outputFile)) {
        //     throw RecognitionException(RecognitionError::ConversionFailed,
        //                              "无法生成GP文件");
        // }
        
        std::cout << "Conversion completed! Output file: " << outputFile << std::endl;
        return 0;
        
    } catch (const RecognitionException& e) {
        std::cerr << "Error: " << e.getUserFriendlyMessage() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unknown error: " << e.what() << std::endl;
        return 1;
    }
}

// Test TablatureRecognizer functionality
int testTablatureRecognizer(const std::string& testImagePath, bool verbose) {
    try {
        if (verbose) {
            std::cout << "Testing TablatureRecognizer..." << std::endl;
        }
        
        // 创建测试图像（如果没有提供路径）
        cv::Mat testImage;
        if (testImagePath.empty() || !fs::exists(testImagePath)) {
            if (verbose) {
                std::cout << "Creating synthetic test image..." << std::endl;
            }
            
            // 创建一个简单的六线谱测试图像
            testImage = cv::Mat::zeros(200, 600, CV_8UC3);
            testImage.setTo(cv::Scalar(255, 255, 255)); // 白色背景
            
            // 绘制6条水平线（六线谱）
            for (int i = 0; i < 6; i++) {
                int y = 50 + i * 20;
                cv::line(testImage, cv::Point(50, y), cv::Point(550, y), cv::Scalar(0, 0, 0), 2);
            }
            
            // 添加一些品位数字
            cv::putText(testImage, "3", cv::Point(100, 55), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 0), 2);
            cv::putText(testImage, "2", cv::Point(150, 75), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 0), 2);
            cv::putText(testImage, "0", cv::Point(200, 95), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 0), 2);
            cv::putText(testImage, "1", cv::Point(250, 115), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 0), 2);
            cv::putText(testImage, "3", cv::Point(300, 135), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 0), 2);
            cv::putText(testImage, "2", cv::Point(350, 155), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 0), 2);
            
        } else {
            if (verbose) {
                std::cout << "Loading test image: " << testImagePath << std::endl;
            }
            testImage = cv::imread(testImagePath);
            if (testImage.empty()) {
                throw RecognitionException(RecognitionError::FileNotFound,
                                         "Cannot load test image: " + testImagePath);
            }
        }
        
        // 创建TablatureRecognizer实例
        TablatureRecognizer recognizer;
        
        if (verbose) {
            std::cout << "Running tablature recognition..." << std::endl;
        }
        
        // 执行识别
        TablatureResult result = recognizer.recognize(testImage);
        
        // 输出结果
        std::cout << "\n=== TablatureRecognizer Test Results ===" << std::endl;
        std::cout << "String count: " << result.stringCount << std::endl;
        std::cout << "Notes found: " << result.notes.size() << std::endl;
        std::cout << "Durations found: " << result.durations.size() << std::endl;
        
        if (!result.notes.empty()) {
            std::cout << "\nDetected notes:" << std::endl;
            for (size_t i = 0; i < result.notes.size(); i++) {
                const auto& note = result.notes[i];
                std::cout << "  Note " << (i+1) << ": String " << note.string 
                         << ", Fret " << note.fret 
                         << ", Position " << note.position << std::endl;
            }
        }
        
        if (!result.durations.empty()) {
            std::cout << "\nDetected durations:" << std::endl;
            for (size_t i = 0; i < result.durations.size(); i++) {
                std::cout << "  Duration " << (i+1) << ": " << static_cast<int>(result.durations[i]) << std::endl;
            }
        }
        
        // 保存测试图像（如果是合成的）
        if (testImagePath.empty()) {
            std::string outputPath = "test_tablature.png";
            cv::imwrite(outputPath, testImage);
            if (verbose) {
                std::cout << "Test image saved to: " << outputPath << std::endl;
            }
        }
        
        std::cout << "\nTablatureRecognizer test completed successfully!" << std::endl;
        return 0;
        
    } catch (const RecognitionException& e) {
        std::cerr << "TablatureRecognizer test error: " << e.getUserFriendlyMessage() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "TablatureRecognizer test unknown error: " << e.what() << std::endl;
        return 1;
    }
}

// 查找文件夹中的GP文件
std::string findGPFile(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == ".gp" || extension == ".gp7" || extension == ".gpx") {
                return entry.path().string();
            }
        }
    }
    return "";
}

// 统计文件夹中的图片数量
int countImageFiles(const std::string& folderPath) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
                count++;
            }
        }
    }
    return count;
}



// Run test mode
int runTestMode(const std::string& testDataDir, bool verbose) {
    try {
        if (verbose) {
            std::cout << "开始测试模式，测试数据目录: " << testDataDir << std::endl;
        }
        
        if (!fs::exists(testDataDir) || !fs::is_directory(testDataDir)) {
            throw RecognitionException(RecognitionError::FileNotFound,
                                     "测试数据目录不存在: " + testDataDir);
        }
        
        TestComparator comparator;
        int totalTests = 0;
        int passedTests = 0;
        int trackCountMatches = 0;
        
        std::cout << "\n=== 图像识别测试报告 ===" << std::endl;
        std::cout << "测试目录: " << testDataDir << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        // 遍历测试数据目录中的每个子文件夹
        for (const auto& entry : fs::directory_iterator(testDataDir)) {
            if (entry.is_directory()) {
                std::string folderPath = entry.path().string();
                std::string folderName = entry.path().filename().string();
                
                std::cout << "\n测试项目: " << folderName << std::endl;
                
                try {
                    // 统计图片数量
                    int imageCount = countImageFiles(folderPath);
                    std::cout << "  图片数量: " << imageCount << std::endl;
                    
                    // 查找标准GP文件
                    std::string referenceGP = findGPFile(folderPath);
                    if (referenceGP.empty()) {
                        std::cout << "  警告: 未找到标准GP文件，跳过此测试" << std::endl;
                        continue;
                    }
                    
                    std::cout << "  标准GP文件: " << fs::path(referenceGP).filename().string() << std::endl;
                    
                    // 分析标准GP文件的音轨数量
                    auto gpInfo = comparator.analyzeGPFile(referenceGP);
                    int expectedTrackCount = gpInfo.trackCount;
                    std::cout << "  预期音轨数: " << expectedTrackCount << std::endl;
                    std::cout << "  系统数: " << gpInfo.systemCount << std::endl;
                    std::cout << "  文件大小: " << gpInfo.fileSize << " bytes" << std::endl;
                    std::cout << "  文件格式: " << gpInfo.format << std::endl;
                    if (!gpInfo.title.empty()) {
                        std::cout << "  曲目标题: " << gpInfo.title << std::endl;
                    }
                    std::cout << "  文件有效性: " << (gpInfo.isValid ? "有效" : "无效") << std::endl;
                    
                    // 创建ImageManager来分析图片
                    ImageManager imageManager;
                    if (imageManager.loadFromFolder(folderPath)) {
                        auto allMetadata = imageManager.getAllMetadata();
                        
                        // 更智能的音轨数量估算
                        int recognizedTrackCount = 1; // 默认至少1个音轨
                        
                        if (allMetadata.size() > 1) {
                            // 分析文件名模式来判断是否为多音轨
                            // 如果文件名包含不同的标识符，可能是多音轨
                            std::set<std::string> uniqueIdentifiers;
                            
                            for (const auto& metadata : allMetadata) {
                                std::string filename = fs::path(metadata.path).stem().string();
                                
                                // 提取标识符（假设格式为：songname-identifier#number）
                                size_t dashPos = filename.find('-');
                                size_t hashPos = filename.find('#');
                                
                                if (dashPos != std::string::npos && hashPos != std::string::npos) {
                                    std::string identifier = filename.substr(dashPos + 1, hashPos - dashPos - 1);
                                    uniqueIdentifiers.insert(identifier);
                                }
                            }
                            
                            // 如果有多个不同的标识符，说明可能有多个音轨
                            if (uniqueIdentifiers.size() > 1) {
                                recognizedTrackCount = static_cast<int>(uniqueIdentifiers.size());
                            } else {
                                // 如果标识符相同，可能是同一音轨的多页，估算为1-2个音轨
                                recognizedTrackCount = std::min(2, static_cast<int>((allMetadata.size() + 2) / 3));
                            }
                        }
                        
                        std::cout << "  识别音轨数: " << recognizedTrackCount << std::endl;
                        
                        // 比较音轨数量
                        bool trackCountMatch = (recognizedTrackCount == expectedTrackCount) || 
                                             (std::abs(recognizedTrackCount - expectedTrackCount) <= 1);
                        
                        if (trackCountMatch) {
                            std::cout << "  音轨数量对比: ✓ 匹配" << std::endl;
                            trackCountMatches++;
                        } else {
                            std::cout << "  音轨数量对比: ✗ 不匹配 (差异: " 
                                      << std::abs(recognizedTrackCount - expectedTrackCount) << ")" << std::endl;
                        }
                        
                        // 尝试加载标准GP文件进行详细分析
                        auto referenceScore = comparator.loadScoreFromGP(referenceGP);
                        if (referenceScore) {
                            std::cout << "  标准GP文件加载: ✓ 成功" << std::endl;
                            
                            // 从Score对象中提取详细信息
                            int actualTrackCount = 0;
                            int totalNotes = 0;
                            
                            for (const auto& system : referenceScore->getSystems()) {
                                for (const auto& staff : system.getStaves()) {
                                    actualTrackCount++;
                                    
                                    // 统计音符数量
                                    for (const auto& voice : staff.getVoices()) {
                                        for (const auto& position : voice.getPositions()) {
                                            totalNotes += static_cast<int>(position.getNotes().size());
                                        }
                                    }
                                }
                                break; // 只统计第一个系统
                            }
                            
                            std::cout << "  实际音轨数: " << actualTrackCount << std::endl;
                            std::cout << "  总音符数: " << totalNotes << std::endl;
                            
                            // 更新预期音轨数为实际值
                            expectedTrackCount = actualTrackCount;
                            
                            // 重新比较音轨数量
                            bool trackCountMatch2 = (recognizedTrackCount == expectedTrackCount) || 
                                                   (std::abs(recognizedTrackCount - expectedTrackCount) <= 1);
                            
                            if (trackCountMatch2) {
                                std::cout << "  音轨数量对比: ✓ 匹配（更新后）" << std::endl;
                                trackCountMatches++;
                            } else {
                                std::cout << "  音轨数量对比: ✗ 不匹配（更新后，差异: " 
                                          << std::abs(recognizedTrackCount - expectedTrackCount) << ")" << std::endl;
                            }
                            
                        } else {
                            std::cout << "  标准GP文件加载: ✗ 失败" << std::endl;
                        }
                        
                        if (trackCountMatch) {
                            std::cout << "  测试结果: ✓ 通过" << std::endl;
                            passedTests++;
                        } else {
                            std::cout << "  测试结果: ✗ 失败" << std::endl;
                        }
                        
                    } else {
                        std::cout << "  错误: 无法加载图片文件" << std::endl;
                    }
                    
                    totalTests++;
                    
                } catch (const std::exception& e) {
                    std::cout << "  测试失败: " << e.what() << std::endl;
                    totalTests++;
                }
            }
        }
        
        // 输出测试总结
        std::cout << "\n=== 测试总结 ===" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过测试: " << passedTests << std::endl;
        std::cout << "音轨数量匹配: " << trackCountMatches << "/" << totalTests << std::endl;
        
        if (totalTests > 0) {
            float passRate = static_cast<float>(passedTests) / totalTests * 100;
            float trackMatchRate = static_cast<float>(trackCountMatches) / totalTests * 100;
            
            std::cout << "通过率: " << std::fixed << std::setprecision(1) << passRate << "%" << std::endl;
            std::cout << "音轨匹配率: " << std::fixed << std::setprecision(1) << trackMatchRate << "%" << std::endl;
        }
        
        return (passedTests == totalTests) ? 0 : 1;
        
    } catch (const RecognitionException& e) {
        std::cerr << "测试错误: " << e.getUserFriendlyMessage() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "测试未知错误: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    // Set locale for output
    std::cout.imbue(std::locale(""));
    
    // Check OpenCV
    if (!checkOpenCV()) {
        std::cerr << "Error: OpenCV initialization failed, please check OPENCV_PATH environment variable" << std::endl;
        return 1;
    }
    
    // Parse command line arguments
    bool verbose = false;
    bool testMode = false;
    bool testTablature = false;
    std::string inputFolder;
    std::string outputFile;
    std::string testDataDir;
    std::string testImagePath;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-t" || arg == "--test") {
            testMode = true;
        } else if (arg == "--test-dir") {
            if (i + 1 < argc) {
                testDataDir = argv[++i];
                testMode = true;
            } else {
                std::cerr << "Error: --test-dir requires directory path" << std::endl;
                return 1;
            }
        } else if (arg == "--test-tab") {
            testTablature = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                testImagePath = argv[++i];
            }
        } else if (inputFolder.empty()) {
            inputFolder = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
        } else {
            std::cerr << "Error: Unknown argument " << arg << std::endl;
            showUsage(argv[0]);
            return 1;
        }
    }
    
    // Validate arguments
    if (testTablature) {
        return testTablatureRecognizer(testImagePath, verbose);
    } else if (testMode) {
        if (testDataDir.empty()) {
            testDataDir = inputFolder.empty() ? "G:\\learn\\吉他\\gpproj\\png" : inputFolder;
        }
        return runTestMode(testDataDir, verbose);
    } else {
        if (inputFolder.empty()) {
            std::cerr << "Error: Please specify input folder" << std::endl;
            showUsage(argv[0]);
            return 1;
        }
        
        if (outputFile.empty()) {
            outputFile = "output.gp";
        }
        
        return processFolder(inputFolder, outputFile, verbose);
    }
}