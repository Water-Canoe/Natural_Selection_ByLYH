# =============================================================================
# Natural_Selection 智能车项目 CMake 配置文件
# 支持跨平台编译，自动处理依赖包，适用于任何环境
# =============================================================================

# 设置最低CMake版本要求
cmake_minimum_required(VERSION 3.16)

# 项目基本信息
project(Natural_Selection
    VERSION 1.0.0
    DESCRIPTION "智能车控制系统 - 支持跨平台编译"
    HOMEPAGE_URL "https://github.com/Water-Canoe/Natural_Selection_ByLYH"
    LANGUAGES CXX
)

# =============================================================================
# 全局编译设置
# =============================================================================

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 跨平台编译选项
if(MSVC)
    # Windows MSVC编译器设置
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Linux/macOS GCC/Clang编译器设置
    add_compile_options(-Wall -Wextra -Wpedantic -fPIC)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wno-deprecated-declarations)
    endif()
endif()

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# =============================================================================
# 依赖包管理
# =============================================================================

# 包含FetchContent用于自动下载依赖
include(FetchContent)

# 设置依赖包下载目录
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

# =============================================================================
# OpenCV 依赖处理
# =============================================================================

# 尝试查找系统安装的OpenCV
find_package(OpenCV QUIET)

if(NOT OpenCV_FOUND)
    message(STATUS "系统未找到OpenCV，将自动下载...")
    
    # 自动下载OpenCV
    FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.8.0
        GIT_SHALLOW TRUE
    )
    
    # 设置OpenCV编译选项
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_java OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_python OFF CACHE BOOL "" FORCE)
    set(BUILD_opencv_js OFF CACHE BOOL "" FORCE)
    set(WITH_1394 OFF CACHE BOOL "" FORCE)
    set(WITH_ADE OFF CACHE BOOL "" FORCE)
    set(WITH_FFMPEG ON CACHE BOOL "" FORCE)
    set(WITH_GSTREAMER OFF CACHE BOOL "" FORCE)
    set(WITH_GTK ON CACHE BOOL "" FORCE)
    set(WITH_IPP OFF CACHE BOOL "" FORCE)
    set(WITH_ITT OFF CACHE BOOL "" FORCE)
    set(WITH_OPENCL OFF CACHE BOOL "" FORCE)
    set(WITH_OPENCLAMDBLAS OFF CACHE BOOL "" FORCE)
    set(WITH_OPENCLAMDFFT OFF CACHE BOOL "" FORCE)
    set(WITH_OPENCL_SVM OFF CACHE BOOL "" FORCE)
    set(WITH_OPENEXR OFF CACHE BOOL "" FORCE)
    set(WITH_OPENGL OFF CACHE BOOL "" FORCE)
    set(WITH_OPENJPEG OFF CACHE BOOL "" FORCE)
    set(WITH_OPENMP OFF CACHE BOOL "" FORCE)
    set(WITH_OPENNI OFF CACHE BOOL "" FORCE)
    set(WITH_OPENVX OFF CACHE BOOL "" FORCE)
    set(WITH_PNG ON CACHE BOOL "" FORCE)
    set(WITH_PROTOBUF OFF CACHE BOOL "" FORCE)
    set(WITH_PTHREADS_PF ON CACHE BOOL "" FORCE)
    set(WITH_PVAPI OFF CACHE BOOL "" FORCE)
    set(WITH_QT OFF CACHE BOOL "" FORCE)
    set(WITH_QUICKTIME OFF CACHE BOOL "" FORCE)
    set(WITH_TBB OFF CACHE BOOL "" FORCE)
    set(WITH_TIFF ON CACHE BOOL "" FORCE)
    set(WITH_UNICAP OFF CACHE BOOL "" FORCE)
    set(WITH_V4L ON CACHE BOOL "" FORCE)
    set(WITH_VA OFF CACHE BOOL "" FORCE)
    set(WITH_VA_INTEL OFF CACHE BOOL "" FORCE)
    set(WITH_VDPAU OFF CACHE BOOL "" FORCE)
    set(WITH_VTK OFF CACHE BOOL "" FORCE)
    set(WITH_WEBP ON CACHE BOOL "" FORCE)
    set(WITH_XIMEA OFF CACHE BOOL "" FORCE)
    set(WITH_XINE OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(opencv)
    set(OpenCV_FOUND TRUE)
    set(OpenCV_INCLUDE_DIRS ${opencv_SOURCE_DIR}/include)
    set(OpenCV_LIBS opencv_core opencv_imgproc opencv_imgcodecs opencv_videoio opencv_highgui)
endif()

# =============================================================================
# nlohmann_json 依赖处理
# =============================================================================

# 尝试查找系统安装的nlohmann_json
find_package(nlohmann_json QUIET)

if(NOT nlohmann_json_FOUND)
    message(STATUS "系统未找到nlohmann_json，将自动下载...")
    
    # 自动下载nlohmann_json
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
        GIT_SHALLOW TRUE
    )
    
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# =============================================================================
# libserial 依赖处理
# =============================================================================

# 尝试查找系统安装的libserial
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBSERIAL QUIET libserial)
endif()

if(NOT LIBSERIAL_FOUND)
    message(STATUS "系统未找到libserial，将自动下载...")
    
    # 自动下载libserial
    FetchContent_Declare(
        libserial
        GIT_REPOSITORY https://github.com/crayzeewulf/libserial.git
        GIT_TAG v0.6.0
        GIT_SHALLOW TRUE
    )
    
    # 设置libserial编译选项
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(libserial)
    set(LIBSERIAL_FOUND TRUE)
    set(LIBSERIAL_INCLUDE_DIRS ${libserial_SOURCE_DIR}/src)
    set(LIBSERIAL_LIBRARIES Serial)
endif()

# =============================================================================
# 源文件收集
# =============================================================================

# 收集所有源文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.c"
)

# 收集所有头文件
file(GLOB_RECURSE HEADERS 
    "include/*.hpp"
    "include/*.h"
    "src/*.hpp"
    "src/*.h"
)

# 打印找到的源文件数量
message(STATUS "找到 ${SOURCES} 个源文件")
message(STATUS "找到 ${HEADERS} 个头文件")

# =============================================================================
# 可执行文件创建
# =============================================================================

# 创建主可执行文件
add_executable(Natural_Selection ${SOURCES} ${HEADERS})

# 设置包含目录
target_include_directories(Natural_Selection PRIVATE
    include
    src
    ${OpenCV_INCLUDE_DIRS}
    ${LIBSERIAL_INCLUDE_DIRS}
)

# 设置编译定义
target_compile_definitions(Natural_Selection PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<BOOL:${OpenCV_FOUND}>:HAVE_OPENCV>
    $<$<BOOL:${LIBSERIAL_FOUND}>:HAVE_LIBSERIAL>
)

# 链接依赖库
target_link_libraries(Natural_Selection PRIVATE
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
    ${LIBSERIAL_LIBRARIES}
)

# =============================================================================
# 平台特定设置
# =============================================================================

if(WIN32)
    # Windows特定设置
    target_link_libraries(Natural_Selection PRIVATE ws2_32)
elseif(UNIX AND NOT APPLE)
    # Linux特定设置
    target_link_libraries(Natural_Selection PRIVATE pthread)
    find_package(Threads REQUIRED)
    target_link_libraries(Natural_Selection PRIVATE Threads::Threads)
elseif(APPLE)
    # macOS特定设置
    find_package(Threads REQUIRED)
    target_link_libraries(Natural_Selection PRIVATE Threads::Threads)
endif()

# =============================================================================
# 安装配置
# =============================================================================

# 安装可执行文件
install(TARGETS Natural_Selection
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安装配置文件
install(FILES config/config.json
    DESTINATION etc/Natural_Selection
)

# 安装资源文件
install(DIRECTORY res/
    DESTINATION share/Natural_Selection
    FILES_MATCHING PATTERN "*.jpg" PATTERN "*.png" PATTERN "*.json"
)

# =============================================================================
# 测试配置（可选）
# =============================================================================

option(BUILD_TESTS "构建测试" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# =============================================================================
# 调试信息输出
# =============================================================================

message(STATUS "=== Natural_Selection 构建配置 ===")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "OpenCV: ${OpenCV_FOUND}")
message(STATUS "nlohmann_json: ${nlohmann_json_FOUND}")
message(STATUS "libserial: ${LIBSERIAL_FOUND}")
message(STATUS "源文件数量: ${SOURCES}")
message(STATUS "头文件数量: ${HEADERS}")
message(STATUS "输出目录: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "================================")
