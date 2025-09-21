#include "RecognitionException.h"
#include <sstream>

namespace ImageToGP {

RecognitionException::RecognitionException(RecognitionError error, const std::string& message)
    : m_error(error), m_message(message) {
    buildWhatMessage();
}

RecognitionException::RecognitionException(RecognitionError error, const std::string& message, 
                                         const std::string& details)
    : m_error(error), m_message(message), m_details(details) {
    buildWhatMessage();
}

const char* RecognitionException::what() const noexcept {
    return m_whatMessage.c_str();
}

RecognitionError RecognitionException::getErrorType() const {
    return m_error;
}

const std::string& RecognitionException::getMessage() const {
    return m_message;
}

const std::string& RecognitionException::getDetails() const {
    return m_details;
}

std::string RecognitionException::getUserFriendlyMessage() const {
    std::ostringstream oss;
    oss << m_message;
    
    std::string suggestion = getErrorSuggestion(m_error);
    if (!suggestion.empty()) {
        oss << "\nSuggestion: " << suggestion;
    }
    
    if (!m_details.empty()) {
        oss << "\nDetails: " << m_details;
    }
    
    return oss.str();
}

void RecognitionException::buildWhatMessage() const {
    std::ostringstream oss;
    oss << "[" << errorTypeToString(m_error) << "] " << m_message;
    if (!m_details.empty()) {
        oss << " (" << m_details << ")";
    }
    m_whatMessage = oss.str();
}

std::string errorTypeToString(RecognitionError error) {
    switch (error) {
        case RecognitionError::UnsupportedFormat:
            return "Unsupported Format";
        case RecognitionError::InvalidSequence:
            return "Invalid Sequence";
        case RecognitionError::NoValidContent:
            return "No Valid Content";
        case RecognitionError::ProcessingFailed:
            return "Processing Failed";
        case RecognitionError::ConversionFailed:
            return "Conversion Failed";
        case RecognitionError::FileNotFound:
            return "File Not Found";
        case RecognitionError::OpenCVError:
            return "OpenCV Error";
        case RecognitionError::OCRError:
            return "OCR Error";
        default:
            return "Unknown Error";
    }
}

std::string getErrorSuggestion(RecognitionError error) {
    switch (error) {
        case RecognitionError::UnsupportedFormat:
            return "Please use supported image formats (jpg, png, bmp)";
        case RecognitionError::InvalidSequence:
            return "Please ensure image files are named as 'songname-id#number.png' with consecutive numbers starting from 01";
        case RecognitionError::NoValidContent:
            return "Please ensure images contain clear musical notation content, recommend using high resolution images";
        case RecognitionError::ProcessingFailed:
            return "Please check image quality, ensure clear and good contrast";
        case RecognitionError::ConversionFailed:
            return "Please check if recognition results contain valid musical data";
        case RecognitionError::FileNotFound:
            return "Please check if file path is correct and file exists";
        case RecognitionError::OpenCVError:
            return "Please ensure OpenCV library is correctly installed, check OPENCV_PATH environment variable";
        case RecognitionError::OCRError:
            return "Please ensure Tesseract OCR is correctly installed, or check if text in image is clear";
        default:
            return "";
    }
}

} // namespace ImageToGP