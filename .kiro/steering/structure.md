# 项目结构

## 顶层目录结构
```
pteplayer/
├── actions/          # 用户操作和命令处理
├── app/             # 应用程序核心逻辑
├── audio/           # 音频处理和MIDI播放
├── dialogs/         # 用户界面对话框
├── exec/            # 主程序入口和QML界面
├── formats/         # 文件格式解析器
├── midi/            # MIDI文件处理
├── painters/        # 图形渲染和绘制
├── score/           # 乐谱数据模型
├── widgets/         # UI组件
├── util/            # 工具函数
├── third_part/      # 第三方库
├── fonts/           # 字体文件
├── images/          # 图标和图片资源
└── icons/           # 应用图标
```

## 核心模块说明

### `/app` - 应用程序核心
- `powertabeditor.h/cpp` - 主应用程序类
- `PtePlayer.h/cpp` - 播放器核心组件
- `documentmanager.h/cpp` - 文档管理
- `settings.h/cpp` - 应用设置

### `/score` - 乐谱数据模型
- `score.h/cpp` - 乐谱主数据结构
- `note.h/cpp` - 音符定义
- `position.h/cpp` - 位置信息
- `system.h/cpp` - 乐谱系统
- `staff.h/cpp` - 谱表定义

### `/formats` - 文件格式支持
- `gp7/` - Guitar Pro 7格式
- `gpx/` - Guitar Pro X格式
- `guitar_pro/` - 通用Guitar Pro格式
- `powertab/` - PowerTab格式
- `midi/` - MIDI格式

### `/exec` - 程序入口和界面
- `main.cpp` - 程序主入口
- `gallery.qml` - QML主界面
- `pages/` - QML页面组件
- `+Material/` - Material Design主题

### `/actions` - 操作命令
- `add*.h/cpp` - 添加操作（音符、小节等）
- `edit*.h/cpp` - 编辑操作
- `remove*.h/cpp` - 删除操作
- `undomanager.h/cpp` - 撤销/重做管理

### `/painters` - 图形渲染
- `systemrenderer.h/cpp` - 系统渲染器
- `staffpainter.h/cpp` - 谱表绘制
- `musicfont.h/cpp` - 音乐字体处理

## 文件命名约定
- 头文件使用 `.h` 扩展名
- 实现文件使用 `.cpp` 扩展名
- QML文件使用 `.qml` 扩展名
- 资源文件使用 `.qrc` 扩展名
- UI文件使用 `.ui` 扩展名

## 构建配置
- 每个模块目录都包含独立的 `CMakeLists.txt`
- 主CMakeLists.txt位于根目录
- 第三方库在 `third_part/` 目录下独立管理

## 资源文件组织
- `/fonts` - 音乐符号字体和文本字体
- `/images` - 工具栏图标和音乐符号图片
- `/icons` - 应用程序图标
- `/music` - 示例音频文件（节拍器等）