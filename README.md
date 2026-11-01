# SerialPort

基于 Linux termios 的轻量级串口通信 C++ 库，封装了串口打开、参数设置、数据收发与后台接收线程，可作为嵌入式 / 上位机项目中串口通信模块的参考实现。

## 功能特性

- 支持常见波特率：2400 ~ 230400（不支持时回退 9600）
- 支持 7/8 位数据位、无/奇/偶校验、1/2 位停止位
- 基于 `select` 的阻塞式接收，独立接收线程 + 互斥锁保护共享数据
- 仅依赖 C++11 标准库与 POSIX 接口，无第三方库

## 目录结构

```
├── CMakeLists.txt      # 顶层 CMake 配置
├── build.sh            # 一键构建脚本
├── Code/
│   ├── main.cpp        # 示例程序：初始化串口并循环打印接收数据
│   └── Base/
│       ├── SerialPort.h    # 串口类接口
│       └── SerialPort.cpp  # 串口类实现
└── Docs/
    └── 软件架构.md      # 架构说明
```

## 编译与运行

依赖：Linux、CMake >= 3.10、支持 C++11 的编译器。

```bash
# 构建（产物在 Build/bin/）
./build.sh

# 运行（默认打开 /dev/ttyACM0，请按实际设备修改 main.cpp 中的 portName）
./Build/bin/SerialPort
```

## 使用示例

```cpp
SerialPort serialPort;
// 初始化：设备名、波特率、数据位、校验位、停止位
serialPort.initSerial("/dev/ttyACM0", 9600, 8, 'N', 1);
serialPort.startReceiveThread();   // 启动后台接收线程

RecvData data = serialPort.getRecvData();  // 获取最近一帧数据
```

## 架构说明

见 [Docs/软件架构.md](Docs/软件架构.md)。

## License

[MIT](LICENSE)
