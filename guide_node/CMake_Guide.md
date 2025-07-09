# CMake 完整指导文档

## 目录
1. [CMake 简介](#cmake-简介)
2. [C++ 工程结构规范](#c-工程结构规范)
3. [CMake 基础语法](#cmake-基础语法)
4. [项目配置与构建](#项目配置与构建)
5. [依赖管理](#依赖管理)
6. [高级特性](#高级特性)
7. [最佳实践](#最佳实践)
8. [常见问题与解决方案](#常见问题与解决方案)
9. [实战示例](#实战示例)

---

## CMake 简介

### 什么是 CMake？
CMake 是一个**跨平台的自动化构建工具**，它的主要作用是：
- 让你只需写一份配置文件（CMakeLists.txt），就能在不同操作系统（如 Windows、Linux、macOS）上编译你的 C++ 项目。
- CMake 本身不会直接编译代码，而是**生成本地平台的构建文件**（比如 Makefile、Visual Studio 工程文件等），然后你用这些文件去编译项目。

#### 为什么要用 CMake？
- **跨平台**：不用为每个平台单独写构建脚本。
- **自动化**：自动查找头文件、库文件、依赖关系。
- **易维护**：项目大了以后，手写 makefile 或 IDE 工程文件会很痛苦，CMake 让一切变得简单。
- **社区支持好**：几乎所有主流 C++ 项目都支持 CMake。

#### CMake 的基本工作流程
1. **写 CMakeLists.txt**：在项目根目录写一个配置文件，告诉 CMake 你的项目结构、源文件、依赖等。
2. **生成构建文件**：用 CMake 命令生成本地平台的构建文件。
3. **编译项目**：用生成的构建文件（比如 make、ninja、Visual Studio）去编译项目。

#### 举例说明
比如你在 Windows 上用 Visual Studio，CMake 会帮你生成 .sln 工程文件；在 Linux 上用 make，CMake 会帮你生成 Makefile。

---

## C++ 工程结构规范

### 为什么要有规范的工程结构？
- 让项目更清晰，方便团队协作和后期维护。
- 让 CMake 能自动找到你的源文件、头文件、依赖。
- 方便集成第三方库、测试、文档等。

### 标准项目结构（详细注释）
```
项目根目录/
├── CMakeLists.txt              # CMake 的主配置文件，所有 CMake 配置都从这里开始
├── README.md                   # 项目说明文档，介绍项目用途、用法等
├── LICENSE                     # 许可证文件，开源项目必备
├── .gitignore                  # Git 忽略文件，告诉 git 哪些文件不用提交
├── build/                      # 构建输出目录，CMake 会在这里生成中间文件和可执行文件
├── docs/                       # 文档目录，存放 API 文档、用户手册等
│   ├── api/                    # API 文档
│   └── user_guide/             # 用户指南
├── include/                    # 头文件目录，放对外公开的 .h/.hpp 文件
│   └── 项目名/                 # 建议用项目名做一级子目录，防止头文件冲突
│       ├── public_headers.h    # 公共头文件
│       └── internal_headers.h  # 内部头文件
├── src/                        # 源代码目录，放 .cpp/.c 文件
│   ├── main.cpp               # 主程序入口
│   ├── core/                  # 核心功能模块
│   │   ├── core.cpp
│   │   └── core.h
│   ├── utils/                 # 工具模块
│   │   ├── utils.cpp
│   │   └── utils.h
│   └── tests/                 # 测试代码
│       ├── test_core.cpp
│       └── test_utils.cpp
├── libs/                       # 第三方库目录，存放外部依赖的源码或二进制
│   └── external_lib/          # 外部库
├── scripts/                    # 构建脚本，方便一键编译或部署
│   ├── build.sh               # Linux/macOS 构建脚本
│   └── build.bat              # Windows 构建脚本
└── cmake/                      # CMake 模块和工具，存放自定义的 CMake 脚本
    ├── FindPackage.cmake       # 自定义查找模块
    └── CompilerFlags.cmake     # 编译器标志设置
```

#### 结构说明
- **CMakeLists.txt**：CMake 的入口文件，所有 CMake 配置都从这里开始。
- **include/**：放头文件，建议用项目名做子目录，防止和别的库冲突。
- **src/**：放源文件，按功能模块分子目录。
- **build/**：CMake 生成的中间文件和可执行文件都放这里，通常不加入版本控制。
- **libs/**、**third_party/**：放第三方库源码或二进制。
- **tests/**：放测试代码，建议和主代码分开。
- **cmake/**：放自定义 CMake 脚本，比如查找第三方库的脚本。
- **scripts/**：放一键构建、部署等脚本。

### 棋子分拣机器人项目推荐结构（详细注释）
```
棋子分拣机器人/
├── CMakeLists.txt              # 主配置文件，整个项目的 CMake 入口
├── README.md                   # 项目说明
├── .gitignore                  # Git 忽略文件
├── build/                      # 构建目录，CMake 生成的文件都放这里
├── include/                    # 头文件
│   └── chess_sorter/          # 用项目英文名做一级目录，防止头文件冲突
│       ├── vision/            # 视觉模块头文件
│       │   ├── camera.h       # 摄像头相关接口
│       │   └── image_processor.h # 图像处理相关接口
│       ├── control/           # 控制模块头文件
│       │   ├── robot_controller.h # 机器人控制接口
│       │   └── motion_planner.h   # 运动规划接口
│       ├── algorithm/         # 算法模块头文件
│       │   ├── chess_detector.h   # 棋子识别算法接口
│       │   └── path_optimizer.h   # 路径优化算法接口
│       └── utils/             # 工具头文件
│           ├── logger.h       # 日志工具
│           └── config.h       # 配置管理
├── src/                        # 源代码
│   ├── main.cpp               # 主程序
│   ├── vision/                # 视觉模块实现
│   │   ├── camera.cpp
│   │   └── image_processor.cpp
│   ├── control/               # 控制模块实现
│   │   ├── robot_controller.cpp
│   │   └── motion_planner.cpp
│   ├── algorithm/             # 算法模块实现
│   │   ├── chess_detector.cpp
│   │   └── path_optimizer.cpp
│   └── utils/                 # 工具实现
│       ├── logger.cpp
│       └── config.cpp
├── tests/                      # 测试代码
│   ├── test_vision.cpp        # 视觉模块测试
│   ├── test_control.cpp       # 控制模块测试
│   └── test_algorithm.cpp     # 算法模块测试
├── third_party/               # 第三方库
│   ├── opencv/                # OpenCV 库
│   └── eigen/                 # Eigen 数学库
├── config/                    # 配置文件
│   ├── camera_config.yaml     # 摄像头配置
│   └── robot_config.yaml      # 机器人配置
├── scripts/                   # 构建脚本
│   ├── build.sh               # Linux/macOS 构建脚本
│   ├── build.bat              # Windows 构建脚本
│   └── install_deps.sh        # 安装依赖脚本
└── cmake/                     # CMake 模块
    ├── FindOpenCV.cmake       # 查找 OpenCV 的脚本
    ├── FindEigen.cmake        # 查找 Eigen 的脚本
    └── CompilerFlags.cmake    # 编译器参数设置
```

#### 结构说明
- **vision/**、**control/**、**algorithm/**、**utils/**：按功能模块分目录，便于管理和扩展。
- **tests/**：每个模块都可以有自己的测试代码，方便单元测试。
- **third_party/**：存放第三方库源码或头文件。
- **config/**：存放项目运行需要的配置文件。
- **scripts/**：一键构建、安装依赖等脚本。
- **cmake/**：自定义 CMake 查找脚本和编译参数。

> **建议**：实际项目可以根据需要调整结构，但一定要保证头文件、源文件、第三方库、测试、配置等分门别类，便于 CMake 配置和团队协作。

---

## CMake 基础语法

本节将详细讲解 CMake 最常用的命令和语法，每一行都配有注释，帮助你理解其作用和用法。

### 1. 项目定义
```cmake
# 定义项目名称、版本和支持的语言
project(ChessSorter VERSION 1.0.0 LANGUAGES CXX)
# project() 是 CMake 的核心命令，必须写在 CMakeLists.txt 的前面
# LANGUAGES CXX 表示本项目是 C++ 项目

# 设置 C++ 标准为 17，并强制要求
set(CMAKE_CXX_STANDARD 17)           # 指定 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 必须使用指定标准，否则报错
```

### 2. 变量设置
```cmake
# 用 set() 定义变量，方便后续引用
set(SOURCES
    src/main.cpp                    # 主程序入口
    src/vision/camera.cpp           # 视觉模块实现
    src/control/robot_controller.cpp# 控制模块实现
)

set(HEADERS
    include/chess_sorter/vision/camera.h
    include/chess_sorter/control/robot_controller.h
)
# 变量名建议大写，便于区分
```

### 3. 包含目录
```cmake
# 添加头文件搜索路径
include_directories(include)  # 传统写法，所有 target 都能用
include_directories(${CMAKE_SOURCE_DIR}/include) # 绝对路径写法

# 推荐现代写法：只给指定 target 添加头文件路径
# chess_sorter 是可执行文件的名字
# PUBLIC 表示编译和链接时都要用到
# PRIVATE 只在编译时用，INTERFACE 只在链接时用
# 这样可以更好地管理依赖
# target_include_directories(目标名 作用域 路径)
target_include_directories(chess_sorter PUBLIC include)
```

### 4. 添加可执行文件
```cmake
# 创建可执行文件 chess_sorter，包含所有源文件和头文件
add_executable(chess_sorter ${SOURCES} ${HEADERS})
# add_executable(名字 源文件列表)
# 头文件不是必须写，但写上可以让 IDE 识别

# 设置可执行文件的输出目录
set_target_properties(chess_sorter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin # 可执行文件输出到 build/bin/
)
```

### 5. 添加库
```cmake
# 创建静态库（.a/.lib）
add_library(chess_sorter_lib STATIC ${SOURCES} ${HEADERS})
# 创建动态库（.so/.dll）
add_library(chess_sorter_lib SHARED ${SOURCES} ${HEADERS})
# 一般推荐用 STATIC，除非有特殊需求
```

### 6. 链接库
```cmake
# 给目标程序链接库文件
# 例如链接 OpenCV、Eigen、自己写的库等
# target_link_libraries(目标名 库名1 库名2 ...)
target_link_libraries(chess_sorter
    chess_sorter_lib   # 链接自己写的静态库
    ${OpenCV_LIBS}     # 链接 OpenCV
    Eigen3::Eigen      # 链接 Eigen
)
```

### 7. 条件判断与选项
```cmake
# 判断当前平台
if(WIN32)
    message(STATUS "当前平台是 Windows")
elseif(UNIX)
    message(STATUS "当前平台是 Linux/Unix")
endif()

# 定义开关选项
option(ENABLE_TESTS "是否编译测试代码" ON)
# 使用 if(ENABLE_TESTS) 判断是否开启
```

### 8. 添加子目录
```cmake
# 如果你的项目有多个模块，可以用 add_subdirectory() 添加子目录
add_subdirectory(src)    # 添加源码目录
add_subdirectory(tests)  # 添加测试目录
# 每个子目录下都可以有自己的 CMakeLists.txt
```

---

## 项目配置与构建

本节将详细讲解如何配置和编译一个 C++ 项目，适合初学者。

### 1. CMakeLists.txt 结构详解

```cmake
# 1. 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)
# 这句必须写在最前面，防止用太老的 CMake 版本导致命令不兼容

# 2. 定义项目名称、版本、描述和语言
project(ChessSorter 
    VERSION 1.0.0 
    DESCRIPTION "棋子分拣机器人控制系统"
    LANGUAGES CXX
)

# 3. 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 设置编译选项（警告、优化等）
if(MSVC)
    add_compile_options(/W4) # Windows 下用 MSVC 编译器，开启最高警告级别
else()
    add_compile_options(-Wall -Wextra -Wpedantic) # Linux/Mac 下 GCC/Clang 编译器，开启常用警告
endif()

# 5. 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # 可执行文件输出到 build/bin/
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 动态库输出到 build/lib/
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 静态库输出到 build/lib/

# 6. 查找依赖包（如 OpenCV、Eigen）
find_package(OpenCV REQUIRED)   # 查找 OpenCV，没找到会报错
find_package(Eigen3 REQUIRED)   # 查找 Eigen3，没找到会报错

# 7. 添加子目录（源码、测试）
add_subdirectory(src)           # 添加源码目录
add_subdirectory(tests)         # 添加测试目录

# 8. 创建主可执行文件
add_executable(chess_sorter src/main.cpp) # 这里只写主入口，其他源文件建议在 src/CMakeLists.txt 里 add_library

# 9. 链接库
# chess_sorter_lib 是你自己写的库，OpenCV_LIBS/Eigen3::Eigen 是第三方库
# 这样可以让主程序用到所有功能
# target_link_libraries(目标名 库名1 库名2 ...)
target_link_libraries(chess_sorter 
    chess_sorter_lib
    ${OpenCV_LIBS}
    Eigen3::Eigen
)

# 10. 设置包含目录
# 让编译器能找到头文件
# PRIVATE 只对 chess_sorter 有效
# PUBLIC/INTERFACE 见前文
# target_include_directories(目标名 作用域 路径)
target_include_directories(chess_sorter PRIVATE
    include
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)
```

### 2. 构建流程详解

#### 步骤一：创建构建目录
```bash
mkdir build      # 新建 build 目录，防止源码目录被污染
cd build         # 进入 build 目录
```

#### 步骤二：用 CMake 生成构建文件
```bash
cmake ..         # 在 build 目录下执行，.. 表示上一级源码目录
# CMake 会自动检测你的编译器和依赖，生成 Makefile 或 Visual Studio 工程文件

# Windows 下可以指定生成器
cmake .. -G "Visual Studio 16 2019"
# Linux 下可以指定 Makefile
cmake .. -G "Unix Makefiles"
# 指定编译类型（Release/Debug）
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### 步骤三：编译项目
```bash
make                 # Linux/macOS 下用 make 编译
cmake --build .      # 跨平台通用命令
cmake --build . --config Release  # 指定 Release 模式
cmake --build . --parallel 4      # 并行编译，加快速度
```

#### 步骤四：运行可执行文件
```bash
# 编译完成后，二进制文件在 build/bin/ 目录下
./bin/chess_sorter   # Linux/macOS
bin\chess_sorter.exe # Windows
```

#### 常见问题
- **CMake 报错找不到依赖**：检查 find_package() 是否拼写正确，依赖是否已安装。
- **编译报错找不到头文件/库文件**：检查 include_directories()、target_link_libraries() 是否配置正确。
- **生成的可执行文件找不到**：检查 set_target_properties() 或 set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ...) 是否设置了输出目录。

---

## 依赖管理

依赖管理是 CMake 项目中非常重要的一部分，它决定了你的项目能否正确找到和使用第三方库。本节将详细讲解如何管理项目依赖。

### 1. 查找系统包（find_package）

`find_package()` 是 CMake 查找第三方库的核心命令，它会自动搜索系统中的库文件、头文件等。

#### 基本用法
```cmake
# 查找 OpenCV 库，REQUIRED 表示必须找到，否则报错
find_package(OpenCV REQUIRED)
# 如果找到了，CMake 会设置 OpenCV_INCLUDE_DIRS（头文件路径）和 OpenCV_LIBS（库文件）

# 查找 Eigen 数学库
find_package(Eigen3 REQUIRED)
# 如果找到了，会设置 EIGEN3_INCLUDE_DIR 和 Eigen3::Eigen 目标

# 查找 Boost 库，指定需要的组件
find_package(Boost REQUIRED COMPONENTS filesystem system)
# COMPONENTS 指定需要 Boost 的哪些子库

# 条件查找，找不到也不会报错
find_package(CUDA QUIET)
if(CUDA_FOUND)
    message(STATUS "找到 CUDA: ${CUDA_VERSION}")
    # 如果找到了 CUDA，可以启用 GPU 加速功能
else()
    message(STATUS "未找到 CUDA，将使用 CPU 版本")
endif()
```

#### 常见问题解决
```cmake
# 如果 find_package 找不到库，可以手动指定路径
# 方法1：设置 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/path/to/opencv")

# 方法2：直接设置变量
set(OpenCV_DIR "/path/to/opencv/cmake")
set(EIGEN3_INCLUDE_DIR "/path/to/eigen")

# 方法3：使用 pkg-config（Linux 下常用）
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENCV REQUIRED opencv4)
```

### 2. 自定义查找模块

当 CMake 没有内置某个库的查找模块时，你需要自己写一个。

#### 创建查找模块
```cmake
# 文件名：cmake/FindCustomLib.cmake
# 这个文件告诉 CMake 如何找到你的自定义库

# 设置库的头文件路径
set(CUSTOMLIB_INCLUDE_DIRS "/usr/local/include/customlib")
# 设置库文件路径
set(CUSTOMLIB_LIBRARIES "/usr/local/lib/libcustomlib.a")

# 包含标准查找模块处理函数
include(FindPackageHandleStandardArgs)
# 检查是否找到了必要的组件
find_package_handle_standard_args(CustomLib DEFAULT_MSG
    CUSTOMLIB_INCLUDE_DIRS    # 检查头文件路径
    CUSTOMLIB_LIBRARIES       # 检查库文件路径
)

# 标记这些变量为高级选项，在 cmake-gui 中不会显示
mark_as_advanced(CUSTOMLIB_INCLUDE_DIRS CUSTOMLIB_LIBRARIES)
```

#### 使用自定义查找模块
```cmake
# 在主 CMakeLists.txt 中使用
# 告诉 CMake 在哪里找自定义模块
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# 查找自定义库
find_package(CustomLib REQUIRED)

# 使用找到的库
target_include_directories(my_target PRIVATE ${CUSTOMLIB_INCLUDE_DIRS})
target_link_libraries(my_target ${CUSTOMLIB_LIBRARIES})
```

### 3. 使用 FetchContent（CMake 3.11+）

FetchContent 是 CMake 3.11 引入的新功能，可以直接从网络下载和配置第三方库，非常适合管理依赖。

#### 基本用法
```cmake
# 包含 FetchContent 模块
include(FetchContent)

# 声明要下载的库
FetchContent_Declare(
    googletest                    # 库的名字
    GIT_REPOSITORY https://github.com/google/googletest.git  # Git 仓库地址
    GIT_TAG release-1.12.1       # 指定版本标签
)

# 下载并配置库
FetchContent_MakeAvailable(googletest)

# 使用下载的库
add_executable(test_chess_sorter tests/test_main.cpp)
target_link_libraries(test_chess_sorter
    gtest_main    # Google Test 的主库
    gmock_main    # Google Mock 的主库
)
```

#### 高级用法
```cmake
# 下载多个库
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)

# 批量配置
FetchContent_MakeAvailable(json spdlog)

# 使用
target_link_libraries(chess_sorter
    nlohmann_json::nlohmann_json  # JSON 库
    spdlog::spdlog                # 日志库
)
```

#### 本地缓存
```cmake
# 避免重复下载，使用本地缓存
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()
```

### 4. 依赖管理最佳实践

#### 版本管理
```cmake
# 在项目开始就定义所有依赖的版本
set(OPENCV_VERSION "4.8.0")
set(EIGEN_VERSION "3.4.0")
set(GTEST_VERSION "1.12.1")

# 查找时指定版本
find_package(OpenCV ${OPENCV_VERSION} REQUIRED)
```

#### 依赖检查
```cmake
# 检查依赖是否满足要求
if(OpenCV_VERSION VERSION_LESS "4.0.0")
    message(FATAL_ERROR "OpenCV 版本太低，需要 4.0.0 或更高版本")
endif()

# 打印依赖信息
message(STATUS "OpenCV 版本: ${OpenCV_VERSION}")
message(STATUS "Eigen 版本: ${EIGEN3_VERSION}")
```

---

## 高级特性

本节介绍 CMake 的一些高级功能，这些功能可以让你的项目更加灵活和强大。

### 1. 条件编译

条件编译允许你根据不同的平台、编译器或配置选项来编译不同的代码。

#### 平台检测
```cmake
# 检测操作系统
if(WIN32)
    message(STATUS "当前平台: Windows")
    set(PLATFORM_SPECIFIC_SOURCES src/windows_specific.cpp)
    add_definitions(-DWINDOWS_PLATFORM)  # 定义宏，在代码中可以用 #ifdef WINDOWS_PLATFORM
elseif(UNIX AND NOT APPLE)
    message(STATUS "当前平台: Linux")
    set(PLATFORM_SPECIFIC_SOURCES src/linux_specific.cpp)
    add_definitions(-DLINUX_PLATFORM)
elseif(APPLE)
    message(STATUS "当前平台: macOS")
    set(PLATFORM_SPECIFIC_SOURCES src/macos_specific.cpp)
    add_definitions(-DMACOS_PLATFORM)
endif()

# 检测编译器
if(MSVC)
    message(STATUS "使用 MSVC 编译器")
    add_compile_options(/utf-8)  # 支持 UTF-8 编码
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "使用 GCC 编译器")
    add_compile_options(-fPIC)   # 生成位置无关代码
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "使用 Clang 编译器")
    add_compile_options(-fPIC)
endif()
```

#### 功能开关
```cmake
# 定义功能开关，用户可以通过命令行控制
option(ENABLE_DEBUG "启用调试功能" OFF)           # 默认关闭
option(ENABLE_TESTS "编译测试代码" ON)            # 默认开启
option(ENABLE_CUDA "启用 CUDA 加速" OFF)          # 默认关闭
option(BUILD_SHARED_LIBS "构建动态库" OFF)        # 默认构建静态库

# 根据开关设置编译选项
if(ENABLE_DEBUG)
    add_definitions(-DDEBUG_MODE)                 # 定义调试宏
    set(CMAKE_BUILD_TYPE Debug)                   # 设置为调试模式
    message(STATUS "调试模式已启用")
endif()

if(ENABLE_TESTS)
    enable_testing()                              # 启用测试功能
    find_package(GTest QUIET)
    if(GTest_FOUND)
        message(STATUS "Google Test 已找到，将编译测试")
    else()
        message(WARNING "Google Test 未找到，跳过测试编译")
    endif()
endif()

if(ENABLE_CUDA)
    find_package(CUDA QUIET)
    if(CUDA_FOUND)
        message(STATUS "CUDA 已找到，启用 GPU 加速")
        add_definitions(-DUSE_CUDA)
    else()
        message(WARNING "CUDA 未找到，禁用 GPU 加速")
        set(ENABLE_CUDA OFF)
    endif()
endif()
```

### 2. 自定义目标

自定义目标允许你定义一些特殊的构建任务，比如清理、测试、文档生成等。

#### 清理目标
```cmake
# 自定义清理目标，删除整个构建目录
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "正在删除构建目录..."
)

# 清理特定文件类型
add_custom_target(clean-cache
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMENT "正在清理 CMake 缓存..."
)
```

#### 测试目标
```cmake
# 自定义测试运行目标
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure
    DEPENDS chess_sorter  # 依赖主程序，确保先编译
    COMMENT "正在运行测试..."
)

# 运行特定测试
add_custom_target(test-vision
    COMMAND ${CMAKE_CTEST_COMMAND} -R "test_vision" --verbose
    DEPENDS test_chess_sorter
    COMMENT "正在运行视觉模块测试..."
)
```

#### 文档生成目标
```cmake
# 查找 Doxygen（文档生成工具）
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    # 配置 Doxygen
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    # 生成配置文件
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    # 创建文档生成目标
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "正在生成 API 文档..."
        VERBATIM
    )
endif()
```

### 3. 安装配置

安装配置定义了如何将你的程序安装到系统中，包括可执行文件、库文件、头文件、配置文件等。

#### 基本安装
```cmake
# 安装可执行文件到 bin 目录
install(TARGETS chess_sorter
    RUNTIME DESTINATION bin        # 可执行文件安装到 /usr/local/bin/
    LIBRARY DESTINATION lib        # 动态库安装到 /usr/local/lib/
    ARCHIVE DESTINATION lib        # 静态库安装到 /usr/local/lib/
)

# 安装头文件到 include 目录
install(DIRECTORY include/chess_sorter
    DESTINATION include            # 安装到 /usr/local/include/
    FILES_MATCHING PATTERN "*.h"   # 只安装 .h 文件
    PATTERN "*.hpp"                # 也安装 .hpp 文件
)

# 安装配置文件
install(FILES config/camera_config.yaml
    DESTINATION etc/chess_sorter   # 安装到 /usr/local/etc/chess_sorter/
)
```

#### 高级安装
```cmake
# 安装时生成配置文件
configure_file(
    ${CMAKE_SOURCE_DIR}/config/config.h.in        # 模板文件
    ${CMAKE_BINARY_DIR}/config/config.h           # 生成的文件
)

# 安装生成的文件
install(FILES ${CMAKE_BINARY_DIR}/config/config.h
    DESTINATION include/chess_sorter
)

# 安装整个目录
install(DIRECTORY scripts/
    DESTINATION bin/chess_sorter
    FILES_MATCHING PATTERN "*.sh"
    PATTERN "*.bat"
    PATTERN "*.py"
)

# 安装时设置文件权限
install(FILES config/chess_sorter.conf
    DESTINATION etc/chess_sorter
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
```

#### 打包配置
```cmake
# 启用 CPack 打包功能
include(CPack)

# 设置包信息
set(CPACK_PACKAGE_NAME "ChessSorter")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "智能棋子分拣机器人")
set(CPACK_PACKAGE_VENDOR "Your Company")

# 设置生成器
set(CPACK_GENERATOR "TGZ")        # 生成 tar.gz 包
# set(CPACK_GENERATOR "ZIP")      # 生成 zip 包
# set(CPACK_GENERATOR "DEB")      # 生成 deb 包（Ubuntu/Debian）
# set(CPACK_GENERATOR "RPM")      # 生成 rpm 包（CentOS/RHEL）

# 生成安装包
# 编译完成后运行：cpack
```

### 4. 生成器表达式

生成器表达式是 CMake 的高级功能，允许你在配置时根据目标属性动态设置值。

#### 基本用法
```cmake
# 根据构建类型设置不同的编译选项
target_compile_definitions(chess_sorter PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE>           # Debug 模式下定义 DEBUG_MODE
    $<$<CONFIG:Release>:NDEBUG>             # Release 模式下定义 NDEBUG
)

# 根据目标类型设置不同的链接选项
target_link_libraries(chess_sorter
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:exec_libs>    # 可执行文件链接 exec_libs
    $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:shared_libs>  # 动态库链接 shared_libs
)
```

#### 高级用法
```cmake
# 条件链接库
target_link_libraries(chess_sorter
    $<$<BOOL:${ENABLE_CUDA}>:cuda_libs>     # 只有启用 CUDA 时才链接 cuda_libs
    $<$<BOOL:${ENABLE_OPENCL}>:opencl_libs> # 只有启用 OpenCL 时才链接 opencl_libs
)

# 根据平台设置不同的源文件
set_target_properties(chess_sorter PROPERTIES
    SOURCES
        src/main.cpp
        $<$<BOOL:${WIN32}>:src/windows_specific.cpp>
        $<$<BOOL:${UNIX}>:src/unix_specific.cpp>
)
```

---

## 最佳实践

本节总结了 CMake 项目开发中的最佳实践，遵循这些原则可以让你的项目更加规范、易维护。

### 1. 项目组织

#### 清晰的目录结构
```cmake
# 推荐的项目结构
project_root/
├── CMakeLists.txt          # 主配置文件
├── include/                # 头文件目录
│   └── project_name/       # 用项目名做一级目录，避免冲突
├── src/                    # 源代码目录
│   ├── main.cpp           # 主程序入口
│   ├── module1/           # 按功能模块分目录
│   └── module2/
├── tests/                  # 测试代码目录
├── docs/                   # 文档目录
├── scripts/                # 构建脚本
└── cmake/                  # CMake 模块
```

#### 模块化设计
```cmake
# 主 CMakeLists.txt 只负责项目配置和依赖
cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0.0)

# 设置全局选项
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)

# 查找依赖
find_package(OpenCV REQUIRED)

# 添加子目录
add_subdirectory(src)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

# 主程序
add_executable(my_app src/main.cpp)
target_link_libraries(my_app my_lib ${OpenCV_LIBS})
```

```cmake
# src/CMakeLists.txt 负责源码模块
add_library(my_lib STATIC
    module1/class1.cpp
    module2/class2.cpp
)

target_include_directories(my_lib PUBLIC include)
target_compile_features(my_lib PUBLIC cxx_std_17)
```

### 2. 构建配置

#### 设置合适的 C++ 标准
```cmake
# 明确指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展

# 或者使用更现代的方式
target_compile_features(my_target PUBLIC cxx_std_17)
```

#### 配置编译警告
```cmake
# 根据编译器设置警告选项
if(MSVC)
    # Windows MSVC 编译器
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Linux GCC 编译器
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang 编译器
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()
```

#### 设置输出目录
```cmake
# 统一设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 或者为特定目标设置
set_target_properties(my_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
```

### 3. 依赖管理

#### 版本控制
```cmake
# 在项目开始就定义所有依赖版本
set(REQUIRED_OPENCV_VERSION "4.5.0")
set(REQUIRED_EIGEN_VERSION "3.4.0")

# 查找依赖时检查版本
find_package(OpenCV ${REQUIRED_OPENCV_VERSION} REQUIRED)
if(OpenCV_VERSION VERSION_LESS REQUIRED_OPENCV_VERSION)
    message(FATAL_ERROR "OpenCV version ${OpenCV_VERSION} is too old. Need at least ${REQUIRED_OPENCV_VERSION}")
endif()
```

#### 依赖检查
```cmake
# 检查依赖是否满足要求
function(check_dependency dep_name found_var version_var)
    if(NOT ${found_var})
        message(FATAL_ERROR "${dep_name} not found. Please install it first.")
    endif()
    message(STATUS "Found ${dep_name}: ${${version_var}}")
endfunction()

# 使用
find_package(OpenCV REQUIRED)
check_dependency("OpenCV" OpenCV_FOUND OpenCV_VERSION)
```

### 4. 测试集成

#### 配置测试框架
```cmake
# 启用测试
enable_testing()

# 查找测试框架
find_package(GTest QUIET)
if(GTest_FOUND)
    message(STATUS "Google Test found, tests will be built")
else()
    message(WARNING "Google Test not found, tests will be skipped")
endif()

# 添加测试
if(GTest_FOUND)
    add_executable(test_my_lib tests/test_main.cpp)
    target_link_libraries(test_my_lib my_lib GTest::gtest GTest::gtest_main)
    add_test(NAME MyLibTests COMMAND test_my_lib)
endif()
```

#### 测试最佳实践
```cmake
# 为每个模块创建测试
add_executable(test_module1 tests/test_module1.cpp)
target_link_libraries(test_module1 module1_lib GTest::gtest GTest::gtest_main)
add_test(NAME Module1Tests COMMAND test_module1)

# 设置测试属性
set_tests_properties(Module1Tests PROPERTIES
    TIMEOUT 30                    # 30秒超时
    ENVIRONMENT "TEST_DATA_DIR=${CMAKE_SOURCE_DIR}/test_data"
)
```

### 5. 文档和配置

#### 生成 API 文档
```cmake
# 查找 Doxygen
find_package(Doxygen QUIET)

if(DOXYGEN_FOUND)
    # 配置 Doxygen
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    # 设置 Doxygen 变量
    set(DOXYGEN_PROJECT_NAME "${PROJECT_NAME}")
    set(DOXYGEN_PROJECT_VERSION "${PROJECT_VERSION}")
    set(DOXYGEN_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
    
    # 生成配置文件
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    # 创建文档目标
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()
```

#### 配置文件管理
```cmake
# 生成配置文件
configure_file(
    ${CMAKE_SOURCE_DIR}/config/config.h.in
    ${CMAKE_BINARY_DIR}/config/config.h
)

# 安装配置文件
install(FILES ${CMAKE_BINARY_DIR}/config/config.h
    DESTINATION include/${PROJECT_NAME}
)

install(FILES config/app.conf
    DESTINATION etc/${PROJECT_NAME}
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)
```

---

## 常见问题与解决方案

本节总结了 CMake 使用中的常见问题和解决方法，帮助你快速解决遇到的问题。

### 1. 找不到头文件

#### 问题描述
编译时报告找不到头文件，如：
```
fatal error: 'opencv2/opencv.hpp' file not found
```

#### 解决方案
```cmake
# 方法1：使用 target_include_directories（推荐）
target_include_directories(my_target PRIVATE
    include                    # 项目头文件目录
    ${OpenCV_INCLUDE_DIRS}     # OpenCV 头文件目录
    ${EIGEN3_INCLUDE_DIR}      # Eigen 头文件目录
)

# 方法2：使用 include_directories（传统方法）
include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

# 方法3：手动指定路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/path/to/include")
```

#### 调试技巧
```cmake
# 打印包含目录，检查路径是否正确
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Eigen include dir: ${EIGEN3_INCLUDE_DIR}")

# 检查头文件是否存在
find_file(OPENCV_HEADER opencv2/opencv.hpp
    PATHS ${OpenCV_INCLUDE_DIRS}
    NO_DEFAULT_PATH
)
if(NOT OPENCV_HEADER)
    message(FATAL_ERROR "OpenCV header not found in ${OpenCV_INCLUDE_DIRS}")
endif()
```

### 2. 链接错误

#### 问题描述
链接时找不到库文件，如：
```
undefined reference to `cv::imread'
```

#### 解决方案
```cmake
# 方法1：使用 target_link_libraries（推荐）
target_link_libraries(my_target
    ${OpenCV_LIBS}        # OpenCV 库
    Eigen3::Eigen         # Eigen 库
    pthread               # 系统库
)

# 方法2：使用 link_directories（传统方法）
link_directories(/path/to/lib)
target_link_libraries(my_target opencv_core opencv_imgproc)

# 方法3：手动指定库路径
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/path/to/lib")
```

#### 调试技巧
```cmake
# 打印库文件路径
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV lib dir: ${OpenCV_LIB_DIR}")

# 检查库文件是否存在
foreach(lib ${OpenCV_LIBS})
    find_library(LIB_${lib} ${lib}
        PATHS ${OpenCV_LIB_DIR}
        NO_DEFAULT_PATH
    )
    if(NOT LIB_${lib})
        message(WARNING "Library ${lib} not found")
    endif()
endforeach()
```

### 3. 版本兼容性

#### 问题描述
不同 CMake 版本的行为差异，如某些命令在新版本中已废弃。

#### 解决方案
```cmake
# 设置最低版本要求
cmake_minimum_required(VERSION 3.16)

# 检查功能可用性
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
    # 使用新特性
    target_compile_features(my_target PUBLIC cxx_std_17)
else()
    # 使用旧方法
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# 检查编译器支持
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    message(FATAL_ERROR "Compiler does not support C++17")
endif()
```

### 4. 跨平台编译

#### 问题描述
在不同平台上编译失败，如 Windows 和 Linux 的差异。

#### 解决方案
```cmake
# 平台检测和条件编译
if(WIN32)
    # Windows 特定设置
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    if(MSVC)
        add_compile_options(/utf-8)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    endif()
elseif(UNIX AND NOT APPLE)
    # Linux 特定设置
    add_compile_options(-fPIC)
    find_package(Threads REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
elseif(APPLE)
    # macOS 特定设置
    add_compile_options(-fPIC)
    find_package(Threads REQUIRED)
endif()

# 条件源文件
set(PLATFORM_SOURCES)
if(WIN32)
    list(APPEND PLATFORM_SOURCES src/platform/windows.cpp)
elseif(UNIX)
    list(APPEND PLATFORM_SOURCES src/platform/unix.cpp)
endif()
```

### 5. 性能优化

#### 问题描述
大型项目编译速度慢，依赖关系复杂。

#### 解决方案
```cmake
# 启用并行编译
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    cmake_host_system_information(RESULT N_PROCESSORS QUERY NUMBER_OF_PHYSICAL_CORES)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N_PROCESSORS})
endif()

# 使用 Ninja 生成器（比 Make 更快）
# cmake .. -G Ninja

# 预编译头文件
target_precompile_headers(my_target PRIVATE
    <vector>
    <string>
    <memory>
    "common.h"
)

# 分离编译单元
set_target_properties(my_target PROPERTIES
    COMPILE_FLAGS "-fPIC"
    POSITION_INDEPENDENT_CODE ON
)
```

### 6. 调试技巧

#### 打印调试信息
```cmake
# 打印变量值
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# 打印目标属性
get_target_property(TARGET_TYPE my_target TYPE)
message(STATUS "Target type: ${TARGET_TYPE}")

# 打印源文件列表
get_target_property(SOURCES my_target SOURCES)
message(STATUS "Sources: ${SOURCES}")
```

#### 使用 CMake GUI 调试
```bash
# 启动 CMake GUI
cmake-gui

# 或者在命令行中交互式配置
ccmake ..
```

### 7. 常见错误修复

#### 错误：CMake Error: The source directory does not appear to contain CMakeLists.txt
**原因**：当前目录没有 CMakeLists.txt 文件
**解决**：确保在项目根目录（包含 CMakeLists.txt 的目录）运行 cmake 命令

#### 错误：CMake Error: Could not find package
**原因**：找不到指定的包
**解决**：
```cmake
# 检查包是否安装
# 手动指定包路径
set(OpenCV_DIR "/path/to/opencv/cmake")
# 或者使用 pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENCV REQUIRED opencv4)
```

#### 错误：undefined reference
**原因**：链接时找不到库文件
**解决**：
```cmake
# 确保正确链接库
target_link_libraries(my_target ${OpenCV_LIBS})
# 检查库文件是否存在
find_library(OPENCV_LIB opencv_core)
if(NOT OPENCV_LIB)
    message(FATAL_ERROR "OpenCV library not found")
endif()
```

---

## 实战示例

本节将提供一个完整的棋子分拣机器人项目示例，展示如何从零开始构建一个 CMake 项目。

### 1. 完整的项目结构

首先，让我们创建一个完整的项目结构：

```bash
# 创建项目目录结构
mkdir -p chess_sorter/{include/chess_sorter/{vision,control,algorithm,utils},src/{vision,control,algorithm,utils},tests,config,scripts,cmake,third_party}
cd chess_sorter
```

### 2. 主 CMakeLists.txt 文件

这是项目的核心配置文件，包含了所有必要的设置：

```cmake
# 文件名：CMakeLists.txt（项目根目录）
# 这是整个项目的 CMake 入口文件，所有配置都从这里开始

# 1. 指定 CMake 最低版本要求（必须写在最前面）
cmake_minimum_required(VERSION 3.16)

# 2. 定义项目信息
project(ChessSorter 
    VERSION 1.0.0 
    DESCRIPTION "智能棋子分拣机器人控制系统"
    HOMEPAGE_URL "https://github.com/your-username/chess-sorter"
    LANGUAGES CXX
)

# 3. 设置 C++ 标准（推荐使用 C++17 或更高版本）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器扩展，确保标准兼容性

# 4. 设置编译选项（根据编译器类型设置不同的警告级别）
if(MSVC)
    # Windows 下使用 MSVC 编译器
    add_compile_options(/W4 /utf-8)  # 最高警告级别，支持 UTF-8
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)  # 禁用安全警告
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Linux 下使用 GCC 编译器
    add_compile_options(-Wall -Wextra -Wpedantic -O2)  # 开启所有警告，优化级别2
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # 使用 Clang 编译器
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# 5. 设置输出目录（让生成的文件更有组织）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # 可执行文件输出到 build/bin/
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # 动态库输出到 build/lib/
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)  # 静态库输出到 build/lib/

# 6. 定义项目选项（用户可以通过命令行控制）
option(ENABLE_TESTS "编译测试代码" ON)           # 默认开启测试
option(ENABLE_DOCS "生成文档" OFF)               # 默认关闭文档
option(ENABLE_CUDA "启用 CUDA 加速" OFF)         # 默认关闭 CUDA
option(BUILD_SHARED_LIBS "构建动态库" OFF)       # 默认构建静态库

# 7. 设置 CMake 模块路径（告诉 CMake 在哪里找自定义模块）
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# 8. 查找依赖包（项目需要的第三方库）
# OpenCV：计算机视觉库，用于图像处理
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "找到 OpenCV: ${OpenCV_VERSION}")
    message(STATUS "OpenCV 包含目录: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV 库文件: ${OpenCV_LIBS}")
endif()

# Eigen：数学库，用于矩阵运算
find_package(Eigen3 REQUIRED)
if(Eigen3_FOUND)
    message(STATUS "找到 Eigen3: ${EIGEN3_VERSION}")
endif()

# 9. 添加子目录（每个子目录都有自己的 CMakeLists.txt）
add_subdirectory(src)  # 添加源码目录

# 10. 条件添加测试目录
if(ENABLE_TESTS)
    enable_testing()  # 启用 CMake 测试功能
    add_subdirectory(tests)
endif()

# 11. 条件添加文档目录
if(ENABLE_DOCS)
    add_subdirectory(docs)
endif()

# 12. 创建主可执行文件
add_executable(chess_sorter src/main.cpp)

# 13. 设置主程序的包含目录
target_include_directories(chess_sorter PRIVATE
    include                    # 项目头文件目录
    ${OpenCV_INCLUDE_DIRS}     # OpenCV 头文件目录
    ${EIGEN3_INCLUDE_DIR}      # Eigen 头文件目录
)

# 14. 链接库文件
target_link_libraries(chess_sorter 
    chess_sorter_lib          # 链接自己写的静态库
    ${OpenCV_LIBS}            # 链接 OpenCV 库
    Eigen3::Eigen             # 链接 Eigen 库
)

# 15. 设置编译定义（在代码中可以用 #ifdef 判断）
target_compile_definitions(chess_sorter PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    $<$<BOOL:${ENABLE_CUDA}>:USE_CUDA>  # 如果启用 CUDA，定义 USE_CUDA 宏
)

# 16. 安装配置（定义如何安装程序）
install(TARGETS chess_sorter
    RUNTIME DESTINATION bin        # 可执行文件安装到 /usr/local/bin/
    LIBRARY DESTINATION lib        # 动态库安装到 /usr/local/lib/
    ARCHIVE DESTINATION lib        # 静态库安装到 /usr/local/lib/
)

# 安装头文件
install(DIRECTORY include/chess_sorter
    DESTINATION include            # 安装到 /usr/local/include/
    FILES_MATCHING PATTERN "*.h"   # 只安装 .h 文件
    PATTERN "*.hpp"                # 也安装 .hpp 文件
)

# 安装配置文件
install(FILES config/camera_config.yaml
    DESTINATION etc/chess_sorter   # 安装到 /usr/local/etc/chess_sorter/
)

# 17. 打印配置信息
message(STATUS "=== 棋子分拣机器人项目配置信息 ===")
message(STATUS "项目名称: ${PROJECT_NAME}")
message(STATUS "项目版本: ${PROJECT_VERSION}")
message(STATUS "C++ 标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "启用测试: ${ENABLE_TESTS}")
message(STATUS "启用文档: ${ENABLE_DOCS}")
message(STATUS "启用 CUDA: ${ENABLE_CUDA}")
message(STATUS "构建动态库: ${BUILD_SHARED_LIBS}")
message(STATUS "输出目录: ${CMAKE_BINARY_DIR}")
message(STATUS "=====================================")
```

### 3. 源码目录的 CMakeLists.txt

```cmake
# 文件名：src/CMakeLists.txt
# 这个文件负责编译所有的源代码文件

# 1. 收集所有源文件
set(VISION_SOURCES
    vision/camera.cpp              # 摄像头控制
    vision/image_processor.cpp     # 图像处理
)

set(CONTROL_SOURCES
    control/robot_controller.cpp   # 机器人控制
    control/motion_planner.cpp     # 运动规划
)

set(ALGORITHM_SOURCES
    algorithm/chess_detector.cpp   # 棋子检测算法
    algorithm/path_optimizer.cpp   # 路径优化算法
)

set(UTILS_SOURCES
    utils/logger.cpp               # 日志工具
    utils/config.cpp               # 配置管理
)

# 2. 创建静态库（包含所有模块的实现）
add_library(chess_sorter_lib STATIC
    ${VISION_SOURCES}
    ${CONTROL_SOURCES}
    ${ALGORITHM_SOURCES}
    ${UTILS_SOURCES}
)

# 3. 设置库的包含目录
target_include_directories(chess_sorter_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include    # 项目头文件目录
    ${OpenCV_INCLUDE_DIRS}         # OpenCV 头文件
    ${EIGEN3_INCLUDE_DIR}          # Eigen 头文件
)

# 4. 设置库的编译特性
target_compile_features(chess_sorter_lib PUBLIC cxx_std_17)

# 5. 设置库的编译定义
target_compile_definitions(chess_sorter_lib PRIVATE
    $<$<BOOL:${ENABLE_CUDA}>:USE_CUDA>
)

# 6. 链接依赖库
target_link_libraries(chess_sorter_lib
    ${OpenCV_LIBS}    # OpenCV 库
    Eigen3::Eigen     # Eigen 库
)

# 7. 设置库的属性
set_target_properties(chess_sorter_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/chess_sorter"
)
```

### 4. 测试目录的 CMakeLists.txt

```cmake
# 文件名：tests/CMakeLists.txt
# 这个文件负责编译测试代码

# 1. 查找测试框架
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(WARNING "Google Test 未找到，跳过测试编译")
    return()
endif()

message(STATUS "找到 Google Test，将编译测试代码")

# 2. 创建测试可执行文件
add_executable(test_chess_sorter
    test_main.cpp              # 测试主入口
    test_vision.cpp            # 视觉模块测试
    test_control.cpp           # 控制模块测试
    test_algorithm.cpp         # 算法模块测试
    test_utils.cpp             # 工具模块测试
)

# 3. 设置测试程序的包含目录
target_include_directories(test_chess_sorter PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

# 4. 链接库
target_link_libraries(test_chess_sorter
    chess_sorter_lib           # 链接项目库
    GTest::gtest               # Google Test 库
    GTest::gtest_main          # Google Test 主库
    GTest::gmock               # Google Mock 库
    GTest::gmock_main          # Google Mock 主库
)

# 5. 添加测试
add_test(NAME ChessSorterTests COMMAND test_chess_sorter)

# 6. 设置测试属性
set_tests_properties(ChessSorterTests PROPERTIES
    TIMEOUT 60                     # 60秒超时
    ENVIRONMENT "TEST_DATA_DIR=${CMAKE_SOURCE_DIR}/test_data"
)

# 7. 创建自定义测试目标
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --output-on-failure
    DEPENDS test_chess_sorter
    COMMENT "正在运行所有测试..."
)

# 8. 为每个模块创建单独的测试
add_executable(test_vision tests/test_vision.cpp)
target_link_libraries(test_vision chess_sorter_lib GTest::gtest GTest::gtest_main)
add_test(NAME VisionTests COMMAND test_vision)

add_executable(test_control tests/test_control.cpp)
target_link_libraries(test_control chess_sorter_lib GTest::gtest GTest::gtest_main)
add_test(NAME ControlTests COMMAND test_control)
```

### 5. 构建脚本示例

#### Linux/macOS 构建脚本 (build.sh)
```bash
#!/bin/bash
# 文件名：scripts/build.sh
# Linux 和 macOS 下的构建脚本

set -e  # 遇到错误立即退出

echo "=== 棋子分拣机器人构建脚本 ==="

# 检查是否在项目根目录
if [ ! -f "CMakeLists.txt" ]; then
    echo "错误：请在项目根目录运行此脚本"
    exit 1
fi

# 创建构建目录
echo "创建构建目录..."
mkdir -p build
cd build

# 配置项目
echo "配置项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "编译项目..."
make -j$(nproc)  # 使用所有 CPU 核心并行编译

# 运行测试（如果启用了）
if [ -f "Makefile" ] && grep -q "test" Makefile; then
    echo "运行测试..."
    make test
fi

echo "构建完成！"
echo "可执行文件位置: $(pwd)/bin/chess_sorter"
echo "库文件位置: $(pwd)/lib/"
```

#### Windows 构建脚本 (build.bat)
```batch
@echo off
REM 文件名：scripts/build.bat
REM Windows 下的构建脚本

echo === 棋子分拣机器人构建脚本 ===

REM 检查是否在项目根目录
if not exist "CMakeLists.txt" (
    echo 错误：请在项目根目录运行此脚本
    pause
    exit /b 1
)

REM 创建构建目录
echo 创建构建目录...
if not exist build mkdir build
cd build

REM 配置项目
echo 配置项目...
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release

REM 编译项目
echo 编译项目...
cmake --build . --config Release --parallel

REM 运行测试（如果启用了）
echo 运行测试...
ctest --verbose --output-on-failure

echo 构建完成！
echo 可执行文件位置: %cd%\bin\Release\chess_sorter.exe
echo 库文件位置: %cd%\lib\Release\

pause
```

### 6. 自定义 CMake 模块示例

#### OpenCV 查找模块 (cmake/FindOpenCV.cmake)
```cmake
# 文件名：cmake/FindOpenCV.cmake
# 自定义 OpenCV 查找模块（如果系统没有提供）

# 设置搜索路径
set(OpenCV_SEARCH_PATHS
    /usr/local
    /usr
    /opt/opencv
    "C:/opencv"
    "C:/Program Files/opencv"
)

# 查找头文件
find_path(OpenCV_INCLUDE_DIRS
    NAMES opencv2/opencv.hpp
    PATHS ${OpenCV_SEARCH_PATHS}
    PATH_SUFFIXES include include/opencv4 include/opencv
)

# 查找库文件
find_library(OpenCV_CORE_LIB
    NAMES opencv_core
    PATHS ${OpenCV_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

find_library(OpenCV_IMGPROC_LIB
    NAMES opencv_imgproc
    PATHS ${OpenCV_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

# 设置库列表
set(OpenCV_LIBS ${OpenCV_CORE_LIB} ${OpenCV_IMGPROC_LIB})

# 检查是否找到
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenCV DEFAULT_MSG
    OpenCV_INCLUDE_DIRS
    OpenCV_LIBS
)

# 标记为高级选项
mark_as_advanced(OpenCV_INCLUDE_DIRS OpenCV_LIBS)
```

### 7. 配置文件示例

#### 相机配置文件 (config/camera_config.yaml)
```yaml
# 文件名：config/camera_config.yaml
# 相机配置文件

camera:
  device_id: 0                    # 相机设备ID
  width: 1920                     # 图像宽度
  height: 1080                    # 图像高度
  fps: 30                         # 帧率
  exposure: 100                   # 曝光时间
  gain: 1.0                       # 增益

image_processing:
  blur_kernel_size: 5             # 模糊核大小
  threshold_value: 127            # 二值化阈值
  min_contour_area: 100           # 最小轮廓面积

chess_detection:
  red_lower: [0, 100, 100]        # 红色棋子HSV下限
  red_upper: [10, 255, 255]       # 红色棋子HSV上限
  black_lower: [0, 0, 0]          # 黑色棋子HSV下限
  black_upper: [180, 255, 30]     # 黑色棋子HSV上限
```

### 8. 使用说明

#### 基本构建流程
```bash
# 1. 克隆项目
git clone https://github.com/your-username/chess-sorter.git
cd chess-sorter

# 2. 安装依赖（Ubuntu/Debian）
sudo apt update
sudo apt install build-essential cmake libopencv-dev libeigen3-dev libgtest-dev

# 3. 构建项目
./scripts/build.sh

# 4. 运行程序
./build/bin/chess_sorter
```

#### 高级构建选项
```bash
# 启用调试模式
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 启用 CUDA 加速
cmake .. -DENABLE_CUDA=ON

# 禁用测试
cmake .. -DENABLE_TESTS=OFF

# 生成文档
cmake .. -DENABLE_DOCS=ON
make docs

# 安装程序
sudo make install
```

### 9. 项目特点总结

这个实战示例展示了：

1. **完整的项目结构**：清晰的目录组织和模块化设计
2. **详细的注释**：每个配置都有详细的中文注释
3. **跨平台支持**：Windows 和 Linux 的构建脚本
4. **测试集成**：完整的测试框架配置
5. **依赖管理**：第三方库的查找和链接
6. **配置管理**：运行时配置文件的处理
7. **安装部署**：程序的安装和打包

通过这个示例，你可以学习到如何构建一个完整的、可维护的 CMake 项目，并将其应用到你的棋子分拣机器人项目中。

---

## 总结

本指导文档涵盖了 CMake 的完整使用流程，从基础语法到高级特性，为您的棋子分拣机器人项目提供了完整的构建解决方案。通过遵循这些最佳实践，您可以：

1. **建立标准化的项目结构**：清晰的目录组织和模块化设计
2. **配置高效的构建系统**：自动化编译、测试和部署
3. **管理复杂的依赖关系**：第三方库的查找、版本控制和链接
4. **实现跨平台编译**：Windows、Linux、macOS 的统一构建
5. **集成测试和文档**：完整的质量保证体系

建议您根据项目的具体需求，逐步实施这些配置，并在开发过程中不断完善和优化构建系统。记住，CMake 是一个强大的工具，掌握它可以让您的 C++ 项目开发更加高效和规范。

### 学习建议

1. **从简单开始**：先创建一个简单的 CMakeLists.txt，逐步添加功能
2. **实践为主**：多动手尝试，遇到问题及时查阅文档
3. **版本控制**：将 CMake 配置文件纳入版本控制，记录配置变更
4. **持续改进**：随着项目发展，不断优化构建配置
5. **社区支持**：遇到问题时，可以查阅 CMake 官方文档和社区资源

祝您在 CMake 学习和项目开发中取得成功！
