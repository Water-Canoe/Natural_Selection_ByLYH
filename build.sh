#!/bin/bash

# =============================================================================
# Natural_Selection 智能车项目构建脚本
# 支持跨平台编译，自动处理依赖和环境检测
# =============================================================================

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="Linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macOS"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        OS="Windows"
    else
        OS="Unknown"
    fi
    print_info "检测到操作系统: $OS"
}

# 检测编译器
detect_compiler() {
    if command -v g++ &> /dev/null; then
        COMPILER="g++"
        COMPILER_VERSION=$(g++ --version | head -n1)
    elif command -v clang++ &> /dev/null; then
        COMPILER="clang++"
        COMPILER_VERSION=$(clang++ --version | head -n1)
    else
        print_error "未找到支持的C++编译器 (g++ 或 clang++)"
        exit 1
    fi
    print_info "检测到编译器: $COMPILER_VERSION"
}

# 检测CMake
check_cmake() {
    if ! command -v cmake &> /dev/null; then
        print_error "未找到CMake，请先安装CMake"
        print_info "安装方法:"
        if [[ "$OS" == "Linux" ]]; then
            echo "  Ubuntu/Debian: sudo apt-get install cmake"
            echo "  CentOS/RHEL: sudo yum install cmake"
        elif [[ "$OS" == "macOS" ]]; then
            echo "  Homebrew: brew install cmake"
        elif [[ "$OS" == "Windows" ]]; then
            echo "  请从 https://cmake.org/download/ 下载安装"
        fi
        exit 1
    fi
    
    CMAKE_VERSION=$(cmake --version | head -n1)
    print_info "检测到CMake: $CMAKE_VERSION"
}

# 检测Git
check_git() {
    if ! command -v git &> /dev/null; then
        print_error "未找到Git，请先安装Git"
        print_info "安装方法:"
        if [[ "$OS" == "Linux" ]]; then
            echo "  Ubuntu/Debian: sudo apt-get install git"
            echo "  CentOS/RHEL: sudo yum install git"
        elif [[ "$OS" == "macOS" ]]; then
            echo "  Homebrew: brew install git"
        elif [[ "$OS" == "Windows" ]]; then
            echo "  请从 https://git-scm.com/download/win 下载安装"
        fi
        exit 1
    fi
    print_info "Git已安装"
}

# 安装系统依赖（Linux）
install_system_dependencies() {
    if [[ "$OS" == "Linux" ]]; then
        print_info "检查并安装系统依赖..."
        
        # 检测包管理器
        if command -v apt-get &> /dev/null; then
            # Ubuntu/Debian
            sudo apt-get update
            sudo apt-get install -y build-essential pkg-config libgtk-3-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev libatlas-base-dev gfortran
        elif command -v yum &> /dev/null; then
            # CentOS/RHEL
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y pkgconfig gtk3-devel ffmpeg-devel libjpeg-turbo-devel libpng-devel libtiff-devel atlas-devel gcc-gfortran
        elif command -v dnf &> /dev/null; then
            # Fedora
            sudo dnf groupinstall -y "Development Tools"
            sudo dnf install -y pkgconfig gtk3-devel ffmpeg-devel libjpeg-turbo-devel libpng-devel libtiff-devel atlas-devel gcc-gfortran
        else
            print_warning "无法自动安装系统依赖，请手动安装必要的开发包"
        fi
    fi
}

# 创建构建目录
create_build_dir() {
    BUILD_DIR="build"
    if [[ -d "$BUILD_DIR" ]]; then
        print_info "清理现有构建目录..."
        rm -rf "$BUILD_DIR"
    fi
    mkdir -p "$BUILD_DIR"
    print_info "创建构建目录: $BUILD_DIR"
}

# 配置CMake
configure_cmake() {
    print_info "配置CMake项目..."
    
    cd "$BUILD_DIR"
    
    # 根据操作系统设置不同的CMake选项
    CMAKE_OPTIONS="-DCMAKE_BUILD_TYPE=Release"
    
    if [[ "$OS" == "Windows" ]]; then
        # Windows特定选项
        CMAKE_OPTIONS="$CMAKE_OPTIONS -G \"MinGW Makefiles\""
    fi
    
    # 执行CMake配置
    cmake .. $CMAKE_OPTIONS
    
    if [[ $? -eq 0 ]]; then
        print_success "CMake配置成功"
    else
        print_error "CMake配置失败"
        exit 1
    fi
}

# 编译项目
build_project() {
    print_info "开始编译项目..."
    
    # 获取CPU核心数
    if [[ "$OS" == "Linux" ]] || [[ "$OS" == "macOS" ]]; then
        CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    else
        CORES=4
    fi
    
    print_info "使用 $CORES 个CPU核心进行编译"
    
    # 执行编译
    make -j$CORES
    
    if [[ $? -eq 0 ]]; then
        print_success "编译成功！"
    else
        print_error "编译失败"
        exit 1
    fi
}

# 运行测试（可选）
run_tests() {
    if [[ "$1" == "--test" ]]; then
        print_info "运行测试..."
        make test
        if [[ $? -eq 0 ]]; then
            print_success "测试通过"
        else
            print_error "测试失败"
            exit 1
        fi
    fi
}

# 安装项目（可选）
install_project() {
    if [[ "$1" == "--install" ]]; then
        print_info "安装项目..."
        sudo make install
        if [[ $? -eq 0 ]]; then
            print_success "安装成功"
        else
            print_error "安装失败"
            exit 1
        fi
    fi
}

# 显示帮助信息
show_help() {
    echo "Natural_Selection 智能车项目构建脚本"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  --help, -h     显示此帮助信息"
    echo "  --clean, -c    清理构建目录后重新编译"
    echo "  --test, -t     编译后运行测试"
    echo "  --install, -i  编译后安装到系统"
    echo "  --debug        使用Debug模式编译"
    echo "  --verbose, -v  显示详细输出"
    echo ""
    echo "示例:"
    echo "  $0              # 标准编译"
    echo "  $0 --clean      # 清理后重新编译"
    echo "  $0 --test       # 编译并运行测试"
    echo "  $0 --install    # 编译并安装"
}

# 主函数
main() {
    print_info "=== Natural_Selection 智能车项目构建脚本 ==="
    
    # 解析命令行参数
    CLEAN_BUILD=false
    RUN_TESTS=false
    INSTALL_PROJECT=false
    DEBUG_MODE=false
    VERBOSE=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help|-h)
                show_help
                exit 0
                ;;
            --clean|-c)
                CLEAN_BUILD=true
                shift
                ;;
            --test|-t)
                RUN_TESTS=true
                shift
                ;;
            --install|-i)
                INSTALL_PROJECT=true
                shift
                ;;
            --debug)
                DEBUG_MODE=true
                shift
                ;;
            --verbose|-v)
                VERBOSE=true
                shift
                ;;
            *)
                print_error "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # 环境检测
    detect_os
    detect_compiler
    check_cmake
    check_git
    
    # 安装系统依赖
    install_system_dependencies
    
    # 创建构建目录
    if [[ "$CLEAN_BUILD" == true ]]; then
        print_info "清理构建目录..."
        rm -rf build
    fi
    create_build_dir
    
    # 配置CMake
    configure_cmake
    
    # 编译项目
    build_project
    
    # 运行测试
    if [[ "$RUN_TESTS" == true ]]; then
        run_tests
    fi
    
    # 安装项目
    if [[ "$INSTALL_PROJECT" == true ]]; then
        install_project
    fi
    
    print_success "=== 构建完成 ==="
    print_info "可执行文件位置: build/bin/Natural_Selection"
    
    if [[ "$OS" == "Linux" ]] || [[ "$OS" == "macOS" ]]; then
        print_info "运行命令: ./build/bin/Natural_Selection"
    elif [[ "$OS" == "Windows" ]]; then
        print_info "运行命令: build\\bin\\Natural_Selection.exe"
    fi
}

# 执行主函数
main "$@" 