# 制定cmake最小版本
cmake_minimum_required(VERSION 3.5.1)
project(SerialPort)
#用于开关编译时的详细信息打印
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置编译可执行文件的产出目录为项目根目录下
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/libs)

if (${CMAKE_SYSTEM_NAME} MATCHES Linux)
  add_subdirectory(Code)
endif()
