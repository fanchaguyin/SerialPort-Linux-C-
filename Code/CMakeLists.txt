cmake_minimum_required(VERSION 3.10)
project(SerialPort)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加源文件
set(SOURCES
  main.cpp
  Base/SerialPort.cpp
)

# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/Base)

# 创建可执行文件
add_executable(SerialPort ${SOURCES})

# 添加编译警告选项
target_compile_options(SerialPort PRIVATE -Wall)

# 如果需要链接第三方库，可以添加以下内容（示例）
# target_link_libraries(SerialPort PRIVATE pthread rt)
find_package(Threads REQUIRED)
target_link_libraries(SerialPort PRIVATE Threads::Threads)