cmake_minimum_required(VERSION 3.10)

project(VideoRecorder
    VERSION 1.0
    DESCRIPTION "智能车视频录制工具"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找OpenCV依赖包
find_package(OpenCV REQUIRED)

# 创建可执行文件
add_executable(collection_video collection_video.cpp)

# 设置包含目录
target_include_directories(collection_video PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

# 链接OpenCV库
target_link_libraries(collection_video
    ${OpenCV_LIBS}
)

# 安装规则
install(TARGETS collection_video DESTINATION bin)
