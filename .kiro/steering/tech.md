# 技术栈

## 构建系统
- **CMake** (最低版本 3.16)
- 模块化构建，每个功能模块都有独立的CMakeLists.txt

## 核心技术栈
- **C++17** - 主要编程语言
- **Qt 6.6** - 跨平台应用框架
- **QML** - 用户界面设计语言，专为移动端重新设计
- **Boost** - C++扩展库（需要配置BOOST_INCLUDE_PATH）

## 第三方依赖库
- **pugixml** - XML解析库
- **minizip-tools** - ZIP文件处理
- **rtmidi** - MIDI输入输出处理
- **nlohmann/json** - JSON处理库

## 音频处理
- **QMediaPlayer** - Qt音频播放
- **QAudioOutput** - 音频输出设备管理
- **MIDI软件合成器** - 内置MIDI播放功能

## 文件格式支持
- **GP7/GPX** - Guitar Pro 7格式
- **PowerTab** - 传统PowerTab格式
- **MIDI** - 标准MIDI文件

## 常用构建命令

### 配置项目
```bash
cmake -DBOOST_INCLUDE_PATH=/path/to/boost/include .
```

### 编译项目
```bash
cmake --build .
```

### Windows特定
- 使用Visual Studio或MinGW编译器
- 需要配置Qt6环境变量
- 支持Windows MIDI API

## 开发环境要求
- Windows 10/11 (win32平台)
- Qt 6.6 SDK
- CMake 3.16+
- Boost库
- C++17兼容编译器