cmake_minimum_required(VERSION 3.10)

project(Natural_Selection
    VERSION 1.0
    DESCRIPTION "智能车"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找依赖包
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

# 收集所有源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建可执行文件
add_executable(Natural_Selection ${SOURCES})

# 设置包含目录
target_include_directories(Natural_Selection PRIVATE
    include
    src
    ${OpenCV_INCLUDE_DIRS}
)

# 链接依赖库
target_link_libraries(Natural_Selection
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
)
