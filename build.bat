@echo off
setlocal enabledelayedexpansion

REM =============================================================================
REM Natural_Selection 智能车项目构建脚本 (Windows版本)
REM 支持Windows环境下的跨平台编译，自动处理依赖和环境检测
REM =============================================================================

REM 设置错误处理
set "ERRORLEVEL=0"

REM 颜色定义 (Windows 10+)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM 打印带颜色的消息
:print_info
echo %BLUE%[INFO]%NC% %~1
goto :eof

:print_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:print_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:print_error
echo %RED%[ERROR]%NC% %~1
goto :eof

REM 检测Visual Studio
:detect_visual_studio
call :print_info "检测Visual Studio环境..."
for /f "tokens=*" %%i in ('where cl 2^>nul') do (
    set "VS_FOUND=true"
    goto :vs_found
)
set "VS_FOUND=false"
call :print_warning "未找到Visual Studio编译器"
goto :eof

:vs_found
call :print_success "找到Visual Studio编译器"
goto :eof

REM 检测MinGW
:detect_mingw
call :print_info "检测MinGW环境..."
for /f "tokens=*" %%i in ('where g++ 2^>nul') do (
    set "MINGW_FOUND=true"
    goto :mingw_found
)
set "MINGW_FOUND=false"
call :print_warning "未找到MinGW编译器"
goto :eof

:mingw_found
call :print_success "找到MinGW编译器"
goto :eof

REM 检测CMake
:check_cmake
call :print_info "检测CMake..."
for /f "tokens=*" %%i in ('where cmake 2^>nul') do (
    set "CMAKE_FOUND=true"
    for /f "tokens=*" %%j in ('cmake --version 2^>nul') do (
        call :print_success "找到CMake: %%j"
        goto :eof
    )
)
set "CMAKE_FOUND=false"
call :print_error "未找到CMake，请先安装CMake"
call :print_info "安装方法:"
echo   1. 从 https://cmake.org/download/ 下载安装
echo   2. 或使用 Chocolatey: choco install cmake
echo   3. 或使用 Scoop: scoop install cmake
exit /b 1

REM 检测Git
:check_git
call :print_info "检测Git..."
for /f "tokens=*" %%i in ('where git 2^>nul') do (
    set "GIT_FOUND=true"
    call :print_success "Git已安装"
    goto :eof
)
set "GIT_FOUND=false"
call :print_error "未找到Git，请先安装Git"
call :print_info "安装方法:"
echo   1. 从 https://git-scm.com/download/win 下载安装
echo   2. 或使用 Chocolatey: choco install git
echo   3. 或使用 Scoop: scoop install git
exit /b 1

REM 创建构建目录
:create_build_dir
set "BUILD_DIR=build"
if exist "%BUILD_DIR%" (
    call :print_info "清理现有构建目录..."
    rmdir /s /q "%BUILD_DIR%"
)
mkdir "%BUILD_DIR%"
call :print_info "创建构建目录: %BUILD_DIR%"
goto :eof

REM 配置CMake
:configure_cmake
call :print_info "配置CMake项目..."
cd "%BUILD_DIR%"

REM 根据找到的编译器选择生成器
if "%VS_FOUND%"=="true" (
    set "CMAKE_GENERATOR=Visual Studio 16 2019"
    set "CMAKE_ARCH=x64"
) else if "%MINGW_FOUND%"=="true" (
    set "CMAKE_GENERATOR=MinGW Makefiles"
    set "CMAKE_ARCH="
) else (
    set "CMAKE_GENERATOR=Unix Makefiles"
    set "CMAKE_ARCH="
)

REM 执行CMake配置
if defined CMAKE_ARCH (
    cmake .. -G "%CMAKE_GENERATOR%" -A %CMAKE_ARCH% -DCMAKE_BUILD_TYPE=Release
) else (
    cmake .. -G "%CMAKE_GENERATOR%" -DCMAKE_BUILD_TYPE=Release
)

if %ERRORLEVEL% equ 0 (
    call :print_success "CMake配置成功"
) else (
    call :print_error "CMake配置失败"
    exit /b 1
)
goto :eof

REM 编译项目
:build_project
call :print_info "开始编译项目..."

REM 获取CPU核心数
for /f "tokens=*" %%i in ('wmic cpu get NumberOfCores /value ^| find "="') do (
    for /f "tokens=2 delims==" %%j in ("%%i") do set "CORES=%%j"
)
if not defined CORES set "CORES=4"

call :print_info "使用 %CORES% 个CPU核心进行编译"

REM 执行编译
if "%VS_FOUND%"=="true" (
    cmake --build . --config Release --parallel %CORES%
) else (
    cmake --build . --parallel %CORES%
)

if %ERRORLEVEL% equ 0 (
    call :print_success "编译成功！"
) else (
    call :print_error "编译失败"
    exit /b 1
)
goto :eof

REM 运行测试（可选）
:run_tests
if "%1"=="--test" (
    call :print_info "运行测试..."
    cmake --build . --target test
    if %ERRORLEVEL% equ 0 (
        call :print_success "测试通过"
    ) else (
        call :print_error "测试失败"
        exit /b 1
    )
)
goto :eof

REM 安装项目（可选）
:install_project
if "%1"=="--install" (
    call :print_info "安装项目..."
    cmake --build . --target install
    if %ERRORLEVEL% equ 0 (
        call :print_success "安装成功"
    ) else (
        call :print_error "安装失败"
        exit /b 1
    )
)
goto :eof

REM 显示帮助信息
:show_help
echo Natural_Selection 智能车项目构建脚本 (Windows版本)
echo.
echo 用法: %~nx0 [选项]
echo.
echo 选项:
echo   --help, -h     显示此帮助信息
echo   --clean, -c    清理构建目录后重新编译
echo   --test, -t     编译后运行测试
echo   --install, -i  编译后安装到系统
echo   --debug        使用Debug模式编译
echo   --verbose, -v  显示详细输出
echo.
echo 示例:
echo   %~nx0              # 标准编译
echo   %~nx0 --clean      # 清理后重新编译
echo   %~nx0 --test       # 编译并运行测试
echo   %~nx0 --install    # 编译并安装
goto :eof

REM 主函数
:main
call :print_info "=== Natural_Selection 智能车项目构建脚本 (Windows) ==="

REM 解析命令行参数
set "CLEAN_BUILD=false"
set "RUN_TESTS=false"
set "INSTALL_PROJECT=false"
set "DEBUG_MODE=false"
set "VERBOSE=false"

:parse_args
if "%1"=="" goto :args_done
if "%1"=="--help" goto :show_help
if "%1"=="-h" goto :show_help
if "%1"=="--clean" (
    set "CLEAN_BUILD=true"
    shift
    goto :parse_args
)
if "%1"=="-c" (
    set "CLEAN_BUILD=true"
    shift
    goto :parse_args
)
if "%1"=="--test" (
    set "RUN_TESTS=true"
    shift
    goto :parse_args
)
if "%1"=="-t" (
    set "RUN_TESTS=true"
    shift
    goto :parse_args
)
if "%1"=="--install" (
    set "INSTALL_PROJECT=true"
    shift
    goto :parse_args
)
if "%1"=="-i" (
    set "INSTALL_PROJECT=true"
    shift
    goto :parse_args
)
if "%1"=="--debug" (
    set "DEBUG_MODE=true"
    shift
    goto :parse_args
)
if "%1"=="--verbose" (
    set "VERBOSE=true"
    shift
    goto :parse_args
)
if "%1"=="-v" (
    set "VERBOSE=true"
    shift
    goto :parse_args
)
call :print_error "未知选项: %1"
call :show_help
exit /b 1

:args_done

REM 环境检测
call :detect_visual_studio
call :detect_mingw
call :check_cmake
call :check_git

REM 创建构建目录
if "%CLEAN_BUILD%"=="true" (
    call :print_info "清理构建目录..."
    if exist "build" rmdir /s /q "build"
)
call :create_build_dir

REM 配置CMake
call :configure_cmake

REM 编译项目
call :build_project

REM 运行测试
if "%RUN_TESTS%"=="true" (
    call :run_tests --test
)

REM 安装项目
if "%INSTALL_PROJECT%"=="true" (
    call :install_project --install
)

call :print_success "=== 构建完成 ==="
call :print_info "可执行文件位置: build\bin\Natural_Selection.exe"
call :print_info "运行命令: build\bin\Natural_Selection.exe"

exit /b 0 