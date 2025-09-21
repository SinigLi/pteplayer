cmake_minimum_required(VERSION 3.16)
project(ImageToGPConverter)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent Core5Compat Gui Multimedia Network PrintSupport Quick QuickControls2 Sql Svg Widgets Xml)

qt_standard_project_setup()

# 使用环境变量OPENCV_PATH配置OpenCV
if(DEFINED ENV{OPENCV_PATH})
    set(OpenCV_DIR "$ENV{OPENCV_PATH}")
    include_directories("$ENV{OPENCV_PATH}/include")
    link_directories("$ENV{OPENCV_PATH}/lib")
    message(STATUS "Using OpenCV from: $ENV{OPENCV_PATH}")
else()
    message(FATAL_ERROR "OPENCV_PATH environment variable not set")
endif()

# 查找OpenCV包
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

# 配置Tesseract OCR（使用WIN_3RD环境变量）
if(DEFINED ENV{WIN_3RD})
    set(WIN_3RD_PATH "$ENV{WIN_3RD}")
    message(STATUS "Using WIN_3RD path: ${WIN_3RD_PATH}")
    
    # 设置Tesseract包含目录和库目录
    include_directories("${WIN_3RD_PATH}/include")
    link_directories("${WIN_3RD_PATH}/lib")
    
    # 查找Tesseract头文件
    find_path(TESSERACT_INCLUDE_DIR
        NAMES tesseract/baseapi.h
        PATHS "${WIN_3RD_PATH}/include"
        NO_DEFAULT_PATH
    )
    
    # 查找Tesseract静态库
    find_library(TESSERACT_LIBRARY
        NAMES tesseract55 tesseract libtesseract tesseract53
        PATHS "${WIN_3RD_PATH}/lib"
        NO_DEFAULT_PATH
    )
    
    # 查找Leptonica静态库
    find_library(LEPTONICA_LIBRARY
        NAMES leptonica-1.85.0 leptonica libleptonica lept-5
        PATHS "${WIN_3RD_PATH}/lib"
        NO_DEFAULT_PATH
    )
    
    # 查找libarchive静态库（Tesseract依赖）
    find_library(LIBARCHIVE_LIBRARY
        NAMES archive libarchive
        PATHS "${WIN_3RD_PATH}/lib"
        NO_DEFAULT_PATH
    )
    
    # 查找LZ4库（libarchive依赖）
    find_library(LZ4_LIBRARY
        NAMES lz4 liblz4
        PATHS "${WIN_3RD_PATH}/lib"
        NO_DEFAULT_PATH
    )
    
    if(TESSERACT_INCLUDE_DIR AND TESSERACT_LIBRARY AND LEPTONICA_LIBRARY AND LIBARCHIVE_LIBRARY)
        set(TESSERACT_FOUND TRUE)
        message(STATUS "Found Tesseract OCR (static libraries):")
        message(STATUS "  Include: ${TESSERACT_INCLUDE_DIR}")
        message(STATUS "  Tesseract Library: ${TESSERACT_LIBRARY}")
        message(STATUS "  Leptonica Library: ${LEPTONICA_LIBRARY}")
        message(STATUS "  LibArchive Library: ${LIBARCHIVE_LIBRARY}")
    else()
        set(TESSERACT_FOUND FALSE)
        message(WARNING "Tesseract OCR libraries not found in WIN_3RD path")
        if(NOT TESSERACT_INCLUDE_DIR)
            message(STATUS "  Missing: tesseract/baseapi.h")
        endif()
        if(NOT TESSERACT_LIBRARY)
            message(STATUS "  Missing: tesseract library")
        endif()
        if(NOT LEPTONICA_LIBRARY)
            message(STATUS "  Missing: leptonica library")
        endif()
        if(NOT LIBARCHIVE_LIBRARY)
            message(STATUS "  Missing: libarchive library")
        endif()
    endif()
else()
    set(TESSERACT_FOUND FALSE)
    message(WARNING "WIN_3RD environment variable not set. Tesseract OCR功能将被禁用.")
endif()

# 源文件
set(SOURCES
    src/main.cpp
    src/ImageManager.cpp
    src/ImagePreprocessor.cpp
    src/ScoreRecognizer.cpp
    src/TablatureRecognizer.cpp
    src/NumberedRecognizer.cpp
    src/LyricsRecognizer.cpp
    src/BarlineRecognizer.cpp
    src/SignatureRecognizer.cpp
    src/ScoreBuilder.cpp
    src/GPConverter.cpp
    src/TestComparator.cpp
    src/RecognitionException.cpp
)

# 头文件
set(HEADERS
    include/ImageManager.h
    include/ImagePreprocessor.h
    include/ScoreRecognizer.h
    include/TablatureRecognizer.h
    include/NumberedRecognizer.h
    include/LyricsRecognizer.h
    include/BarlineRecognizer.h
    include/SignatureRecognizer.h
    include/ScoreBuilder.h
    include/GPConverter.h
    include/TestComparator.h
    include/RecognitionException.h
    include/DataStructures.h
)

# 包含头文件目录
include_directories(include)

# 创建可执行文件
qt_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 参考exec工程的方式包含现有项目模块
target_include_directories(${PROJECT_NAME} PRIVATE
    ..
    ../third_part
    ../third_part/minizip-tools
    ../third_part/pugi-xml
    ${BOOST_INCLUDE_PATH}
)

# 参考exec工程的方式链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
    Qt::Concurrent
    Qt::Core
    Qt::Core5Compat
    Qt::Gui
    Qt::Multimedia
    Qt::Network
    Qt::PrintSupport
    Qt::Quick
    Qt::QuickControls2
    Qt::Sql
    Qt::Svg
    Qt::Widgets
    Qt::Xml
	ptactions
	pteapp
	ptaudio
	ptedialogs
	pteformats
	ptemidi
	ptepainters
	ptescore
	pteutil
	ptewidgets
    minizip
    pugixml-static
    rtmidi
    ${boost_container_lib}
    ${boost_date_time_lib}
    ${boost_exception_lib}
    ${boost_filesystem_lib}
    ${boost_iostreams_lib}
    ${boost_random_lib}
    ${boost_regex_lib}
    ${boost_system_lib}
)
find_library(boost_container_lib boost_container HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_date_time_lib boost_date_time HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_exception_lib boost_exception HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_filesystem_lib boost_filesystem HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_iostreams_lib boost_iostreams HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_random_lib boost_random HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_regex_lib boost_regex HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
find_library(boost_system_lib boost_system HINTS ../boost-lib/arm64-v8a/libboost_atomic.a)
# 链接Tesseract库（如果找到）
if(TESSERACT_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${TESSERACT_LIBRARY} 
        ${LEPTONICA_LIBRARY} 
        ${LIBARCHIVE_LIBRARY}
    )
    
    # 链接libarchive的依赖库
    if(LZ4_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${LZ4_LIBRARY})
    endif()
    
    # 静态链接时需要的系统库
    if(WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            ws2_32      # Windows Socket库
            user32      # Windows用户界面库
            gdi32       # Windows图形设备接口
            comdlg32    # Windows通用对话框库
            advapi32    # Windows高级API库
            xmllite     # Windows XML Lite库（libarchive依赖）
        )
    endif()
    
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_TESSERACT)
    message(STATUS "Tesseract OCR support enabled (static linking)")
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE NO_TESSERACT)
    message(STATUS "Tesseract OCR support disabled")
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(WIN32)
INCLUDE(FindZLIB)

IF(NOT ZLIB_FOUND)
        message(FATAL_ERROR "pteplayer library is required for libminizip!")
ENDIF(NOT ZLIB_FOUND)

include_directories(${ZLIB_INCLUDE_DIR})
link_directories(${ZLIB_LIBRARIES})
endif(WIN32)

# 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()
