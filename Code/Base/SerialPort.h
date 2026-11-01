#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <string>
#include <termios.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <mutex>
#include <thread>

// 串口接收数据，rxBuf 指向 SerialPort 内部缓冲区，仅在下一次接收前有效
struct RecvData
{
    uint8_t *rxBuf;
    int len;
};

class SerialPort
{
public:
    // 构造函数
    SerialPort();
    // 析构函数
    ~SerialPort();

    // 初始化串口
    int initSerial(std::string portName, int baudRate, int dataBit, char parity, int stopBit);
    // 串口发送
    int send(char *buf, int len);
    // 开启串口接收线程
    void startReceiveThread();

    // 获取串口接收数据
    RecvData getRecvData();

private:
    // 设置串口参数
    int setParameters(int fd, int baudRate, int dataBit, char nParity, int nStop);
    // 设置串口文件描述符
    void setFdSerial(int fd);
    // 获取串口文件描述符
    int getFdSerial();
    // 串口接收
    int receive();
    // 串口接收线程
    void receiveThread();
    // 设置串口接收数据
    void setRecvData(const uint8_t *rxBuf, int len);

private:
    int fd = -1;        //  串口文件描述符
    std::mutex fdMutex; //  串口文件描述符互斥锁

    RecvData receiveData;     //  接收数据结构体
    uint8_t recvData[1024];   // 接收到的数据
    int recvDataLen = 0;      // 接收到的数据长度
    std::mutex recvDataMutex; // 接收数据锁
};

#endif // SERIALPORT_H