# Natural_Selection 智能车项目构建指南

## 📋 概述

本项目已经过优化，支持在任何环境下进行跨平台编译，包括自动处理依赖包下载和环境检测。

## 🚀 快速开始

### 方法一：使用构建脚本（推荐）

#### Linux/macOS 用户
```bash
# 给脚本执行权限
chmod +x build.sh

# 标准编译
./build.sh

# 清理后重新编译
./build.sh --clean

# 编译并运行测试
./build.sh --test

# 编译并安装到系统
./build.sh --install
```

#### Windows 用户
```cmd
# 标准编译
build.bat

# 清理后重新编译
build.bat --clean

# 编译并运行测试
build.bat --test

# 编译并安装到系统
build.bat --install
```

### 方法二：手动使用CMake

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译项目
make -j$(nproc)  # Linux/macOS
# 或
cmake --build . --parallel  # Windows
```

## 🔧 环境要求

### 必需工具
- **CMake** (版本 >= 3.16)
- **C++编译器** (支持C++17)
  - Linux/macOS: GCC >= 7 或 Clang >= 5
  - Windows: Visual Studio 2019+ 或 MinGW-w64
- **Git** (用于下载依赖包)

### 可选工具
- **pkg-config** (Linux/macOS)
- **Visual Studio** (Windows)

## 📦 自动依赖管理

项目会自动处理以下依赖包：

### OpenCV (计算机视觉库)
- **版本**: 4.8.0
- **功能**: 图像处理、视频捕获、特征检测
- **自动下载**: 如果系统未安装，会自动从GitHub下载并编译

### nlohmann/json (JSON处理库)
- **版本**: 3.11.2
- **功能**: JSON数据解析和生成
- **自动下载**: 如果系统未安装，会自动从GitHub下载

### libserial (串口通信库)
- **版本**: 0.6.0
- **功能**: 串口设备通信
- **自动下载**: 如果系统未安装，会自动从GitHub下载并编译

## 🖥️ 平台支持

### Linux
- **支持发行版**: Ubuntu, Debian, CentOS, RHEL, Fedora
- **自动依赖安装**: 支持apt、yum、dnf包管理器
- **编译器**: GCC, Clang
- **构建系统**: Make

### macOS
- **支持版本**: macOS 10.14+
- **包管理器**: Homebrew
- **编译器**: Clang, GCC
- **构建系统**: Make

### Windows
- **支持版本**: Windows 10+
- **编译器**: Visual Studio 2019+, MinGW-w64
- **构建系统**: Visual Studio, Make, Ninja
- **包管理器**: Chocolatey, Scoop

## 🔍 构建脚本功能

### 自动检测功能
- ✅ 操作系统检测
- ✅ 编译器检测 (GCC, Clang, MSVC)
- ✅ CMake版本检测
- ✅ Git检测
- ✅ 依赖包检测

### 自动安装功能
- ✅ Linux系统依赖包自动安装
- ✅ 缺失依赖包自动下载
- ✅ 编译环境自动配置

### 构建选项
- ✅ 多核心并行编译
- ✅ Debug/Release模式切换
- ✅ 清理构建目录
- ✅ 运行测试套件
- ✅ 系统安装

## 📁 项目结构

```
Natural_Selection/
├── CMakeLists.txt          # 主CMake配置文件
├── build.sh               # Linux/macOS构建脚本
├── build.bat              # Windows构建脚本
├── BUILD_GUIDE.md         # 本构建指南
├── include/               # 头文件目录
├── src/                   # 源文件目录
├── config/                # 配置文件目录
├── res/                   # 资源文件目录
└── build/                 # 构建输出目录（自动生成）
    ├── bin/               # 可执行文件
    ├── lib/               # 库文件
    └── CMakeCache.txt     # CMake缓存
```

## 🛠️ 故障排除

### 常见问题

#### 1. CMake版本过低
```bash
# 错误信息
CMake 3.16 or higher is required.  You are running version 3.10.2

# 解决方案
# Ubuntu/Debian
sudo apt-get update && sudo apt-get install cmake

# CentOS/RHEL
sudo yum install cmake

# macOS
brew install cmake

# Windows
# 从 https://cmake.org/download/ 下载最新版本
```

#### 2. 编译器不支持C++17
```bash
# 检查编译器版本
g++ --version
clang++ --version

# 升级编译器
# Ubuntu/Debian
sudo apt-get install g++-8

# CentOS/RHEL
sudo yum install devtoolset-8-gcc-c++
```

#### 3. 依赖包下载失败
```bash
# 检查网络连接
ping github.com

# 使用代理（如果需要）
export https_proxy=http://proxy:port
export http_proxy=http://proxy:port
```

#### 4. 权限问题
```bash
# Linux/macOS权限问题
sudo chmod +x build.sh

# Windows权限问题
# 以管理员身份运行命令提示符
```

### 调试模式

启用详细输出以诊断问题：

```bash
# Linux/macOS
./build.sh --verbose

# Windows
build.bat --verbose
```

## 🔧 高级配置

### 自定义CMake选项

```bash
# 创建自定义构建目录
mkdir custom_build && cd custom_build

# 配置自定义选项
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local

# 编译
make -j$(nproc)
```

### 交叉编译

```bash
# 设置交叉编译工具链
export CC=arm-linux-gnueabihf-gcc
export CXX=arm-linux-gnueabihf-g++

# 配置
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
```

## 📊 性能优化

### 编译优化
- 使用多核心并行编译
- 启用编译器优化选项
- 使用ccache加速重复编译

### 运行时优化
- 使用Release模式编译
- 启用链接时优化 (LTO)
- 使用静态链接减少依赖

## 🤝 贡献指南

1. Fork项目
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建Pull Request

## 📞 技术支持

如果遇到构建问题，请：

1. 查看本指南的故障排除部分
2. 检查项目的Issues页面
3. 提供详细的错误信息和环境信息
4. 联系项目维护者

## 📄 许可证

本项目采用MIT许可证，详见LICENSE文件。

---

**注意**: 首次编译可能需要较长时间，因为需要下载和编译依赖包。后续编译会更快。 