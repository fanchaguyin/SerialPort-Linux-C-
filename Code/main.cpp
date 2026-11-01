#include "SerialPort.h"
#include <cstdio>
#include <unistd.h>

int main()
{
    SerialPort serialPort;
    std::string portName = "/dev/ttyACM0";
    int baudRate = 9600;
    int dataBit = 8;
    char parity = 'N';
    int stopBit = 1;

    if (serialPort.initSerial(portName, baudRate, dataBit, parity, stopBit) != 0)
    {
        return -1;
    }
    serialPort.startReceiveThread();
    while (1)
    {
        RecvData recvData = serialPort.getRecvData();
        for (int i = 0; i < recvData.len; ++i)
        {
            printf("%02X ", recvData.rxBuf[i]); // 打印两位十六进制数
        }
        printf("\n");
        usleep(100000);
    }
    return 0;
}
