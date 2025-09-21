#ifndef RECOGNITIONEXCEPTION_H
#define RECOGNITIONEXCEPTION_H

#include <exception>
#include <string>

namespace ImageToGP {

// 识别错误类型枚举
enum class RecognitionError {
    UnsupportedFormat,     // 不支持的图片格式
    InvalidSequence,       // 图片下标不完整
    NoValidContent,        // 无有效乐谱内容
    ProcessingFailed,      // 处理过程异常
    ConversionFailed,      // GP转换失败
    FileNotFound,          // 文件未找到
    OpenCVError,           // OpenCV处理错误
    OCRError,              // OCR识别错误
    InvalidInput,          // 无效输入
    InvalidFormat,         // 无效格式
    InitializationFailed   // 初始化失败
};

// 识别异常类
class RecognitionException : public std::exception {
public:
    RecognitionException(RecognitionError error, const std::string& message);
    RecognitionException(RecognitionError error, const std::string& message, 
                        const std::string& details);
    
    const char* what() const noexcept override;
    RecognitionError getErrorType() const;
    const std::string& getMessage() const;
    const std::string& getDetails() const;
    
    // 获取用户友好的错误信息
    std::string getUserFriendlyMessage() const;
    
private:
    RecognitionError m_error;
    std::string m_message;
    std::string m_details;
    mutable std::string m_whatMessage;  // 缓存what()返回的消息
    
    void buildWhatMessage() const;
};

// 错误类型到字符串的转换
std::string errorTypeToString(RecognitionError error);

// 用户友好的错误信息映射
std::string getErrorSuggestion(RecognitionError error);

} // namespace ImageToGP

#endif // RECOGNITIONEXCEPTION_H