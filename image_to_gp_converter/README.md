# 图片转GP文件转换器

这是一个独立的命令行工具，用于将乐谱图片转换为Guitar Pro (GP) 文件格式。

## 功能特性

- 支持六线谱识别（品位数字识别）
- 支持简谱识别（数字音符识别）
- 支持歌词识别（中英文OCR）
- 支持多音轨处理（单张图片包含多个音轨）
- 支持批量处理（文件夹中的图片序列）
- 提供测试模式（与标准GP文件对比）

## 环境要求

### 必需依赖
- C++17 兼容编译器
- CMake 3.16+
- OpenCV 4.x
- 设置环境变量 `OPENCV_PATH` 指向OpenCV安装目录

### 可选依赖
- Tesseract OCR（用于歌词识别）

## 构建方法

### Windows

1. 设置OpenCV环境变量：
```cmd
set OPENCV_PATH=C:\path\to\opencv
```

2. 构建项目：
```cmd
cd image_to_gp_converter
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux/macOS

1. 设置OpenCV环境变量：
```bash
export OPENCV_PATH=/path/to/opencv
```

2. 构建项目：
```bash
cd image_to_gp_converter
mkdir build
cd build
cmake ..
make
```

## 使用方法

### 基本用法

```bash
# 转换单个文件夹中的图片
./ImageToGPConverter /path/to/images output.gp

# 显示详细处理信息
./ImageToGPConverter -v /path/to/images output.gp
```

### 测试模式

```bash
# 使用默认测试数据集
./ImageToGPConverter --test

# 指定测试数据集目录
./ImageToGPConverter --test-dir "G:\learn\吉他\gpproj\png"
```

### 命令行选项

- `-h, --help`: 显示帮助信息
- `-v, --verbose`: 显示详细处理信息
- `-t, --test`: 启用测试模式
- `--test-dir <path>`: 指定测试数据集目录

## 文件格式要求

### 输入文件命名格式

- **GP文件**: `曲名-标识.gp`（如：彩虹-C.gp）
- **图片文件**: `曲名-标识#序号.png`（如：彩虹-C#01.png, 彩虹-C#02.png）

### 支持的图片格式

- PNG (.png)
- JPEG (.jpg, .jpeg)
- BMP (.bmp)

### 目录结构示例

```
测试数据/
├── 歌曲1/
│   ├── 彩虹-C.gp
│   ├── 彩虹-C#01.png
│   ├── 彩虹-C#02.png
│   └── 彩虹-C#03.png
├── 歌曲2/
│   ├── 童年-G.gp
│   ├── 童年-G#01.png
│   └── 童年-G#02.png
└── ...
```

## 开发状态

当前版本为基础框架，各个识别模块的具体实现正在开发中：

- [x] 项目结构和命令行框架
- [x] 图片管理模块
- [ ] 图像预处理模块
- [ ] 六线谱识别引擎
- [ ] 简谱识别引擎
- [ ] 歌词识别引擎
- [ ] Score构建模块
- [ ] GP转换模块
- [ ] 测试对比模块

## 故障排除

### OpenCV相关错误

1. 确保设置了 `OPENCV_PATH` 环境变量
2. 检查OpenCV库文件是否在 `$(OPENCV_PATH)/lib` 目录下
3. 确认OpenCV版本为4.x

### 图片识别错误

1. 确保图片格式为支持的类型
2. 检查图片文件命名是否符合格式要求
3. 确认图片质量清晰，对比度良好

### 文件路径错误

1. 使用绝对路径避免相对路径问题
2. 确保路径中没有特殊字符
3. 在Windows下注意路径分隔符的使用

## 许可证

本项目基于现有的Power Tab Editor项目开发，遵循相同的开源许可证。