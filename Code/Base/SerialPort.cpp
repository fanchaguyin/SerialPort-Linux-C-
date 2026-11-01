#include "SerialPort.h"

SerialPort::SerialPort()
{
}

SerialPort::~SerialPort()
{
    if (fd >= 0)
    {
        close(fd);
    }
}

/**
 * @brief 串口设置初始化
 * @param portName
 * @param baudRate
 * @param dataBit
 * @param parity
 * @param stopBit
 * @return
 */
int SerialPort::initSerial(std::string portName, int baudRate, int dataBit, char parity, int stopBit)
{
    const char *devName = portName.c_str();

    // 打开串口设备
    int fd = open(devName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        printf("[串口] 打开 %s 失败\n", devName);
        return -1;
    }

    // 设置串口阻塞， 0：阻塞， FNDELAY：非阻塞
    if (fcntl(fd, F_SETFL, 0) < 0) // 阻塞，即使前面在open串口设备时设置的是非阻塞的
    {
        printf("[串口] 串口阻塞!\n");
    }

    // 测试是否是终端设备
    if (isatty(fd) == 0)
    {
        printf("[串口] standard input is not a terminal device\n");
        close(fd);
        return -1;
    }

    // 设置串口参数
    int result = setParameters(fd, baudRate, dataBit, parity, stopBit);
    if (result == -1) // 设置8位数据位、1位停止位、无校验
    {
        fprintf(stderr, "[串口] Set opt Error\n");
        close(fd);
        return result;
    }
    printf("[串口] 设置串口参数 success\n");

    // 设置串口描述符
    setFdSerial(fd);

    tcflush(fd, TCIOFLUSH); // 清掉串口缓存
    fcntl(fd, F_SETFL, 0);  // 串口阻塞
    usleep(30000);          // 休眠30ms

    return 0;
}

/**
 * @brief 设置串口参数
 * @param fd
 * @param baudRate
 * @param dataBit
 * @param nParity
 * @param nStop
 * @return
 */
int SerialPort::setParameters(int fdSerial, int baudRate, int dataBit, char nParity, int nStop)
{
    // 检查文件描述符合法性
    if (fdSerial < 0)
    {
        fprintf(stderr, "Invalid file descriptor\n");
        return -1;
    }

    struct termios newtio, oldtio;

    // 保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息
    if (tcgetattr(fdSerial, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));   // 新termios参数清零
    newtio.c_cflag |= CLOCAL | CREAD; // CLOCAL--忽略 modem 控制线,本地连线, 不具数据机控制功能, CREAD--使能接收标志

    // 设置数据位数
    newtio.c_cflag &= ~CSIZE; // 清数据位标志
    switch (dataBit)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size\n");
        return -1;
    }

    // 设置奇偶校验位
    switch (nParity)
    {
    case 'o':
    case 'O': // 奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E': // 偶校验
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N': // 无校验
        newtio.c_cflag &= ~PARENB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return -1;
    }

    // 设置停止位
    switch (nStop)
    {
    case 1:
        newtio.c_cflag &= ~CSTOPB;
        break;
    case 2:
        newtio.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stop bits\n");
        printf("Unsupported stop bits.");
        return -1;
    }

    // 设置波特率 2400/4800/9600/19200/38400/57600/115200/230400
    switch (baudRate)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 19200:
        cfsetispeed(&newtio, B19200);
        cfsetospeed(&newtio, B19200);
        break;
    case 38400:
        cfsetispeed(&newtio, B38400);
        cfsetospeed(&newtio, B38400);
        break;
    case 57600:
        cfsetispeed(&newtio, B57600);
        cfsetospeed(&newtio, B57600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 230400:
        cfsetispeed(&newtio, B230400);
        cfsetospeed(&newtio, B230400);
        break;
    default:
        printf("\tSorry, Unsupported baud rate, set default 9600!\n\n");
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    // 设置read读取最小字节数和超时时间
    newtio.c_cc[VTIME] = 1; // 读取一个字符等待1*(1/10)s
    newtio.c_cc[VMIN] = 1;  // 读取字符的最少个数为1

    tcflush(fdSerial, TCIFLUSH);                    // 清空缓冲区
    if (tcsetattr(fdSerial, TCSANOW, &newtio) != 0) // 激活新设置
    {
        perror("[串口] SetupSerial 3");
        return -1;
    }
    printf("[串口] Serial set done!\n");
    return 0;
}

/**
 * @brief 串口发送数据
 * @param buf 发送的数据
 * @param len 发送数据的长度
 * @return
 */
int SerialPort::send(char *buf, int len)
{
    int fdSerial = getFdSerial();
    ssize_t ret = 0;

    ret = write(fdSerial, buf, len);
    if (ret == len)
    {
        // printf("[串口] SerialPort send data\n");
        return ret;
    }
    else
    {
        printf("[串口]write device error\n");
        tcflush(fdSerial, TCOFLUSH);
        return -1;
    }
}

/**
 * @brief 串口接收
 * @return
 */
int SerialPort::receive()
{
    int len, fsSel;
    fd_set fsRead;
    int fdSerial = getFdSerial();

    FD_ZERO(&fsRead);          // 每次循环都要清空集合，否则不能检测描述符变化
    FD_SET(fdSerial, &fsRead); // 添加描述符

    fsSel = select(fdSerial + 1, &fsRead, NULL, NULL, NULL); //&timeVal

    if (fsSel < 0)
    {
        perror("[串口] select error");
        return -1;
    }
    if (fsSel > 0)
    {
        usleep(50000); // 读取串口时需要一点延时，防止串口数据没读完（拆包）

        uint8_t rxBuf[1024];
        len = read(fdSerial, rxBuf, sizeof(rxBuf) - 1); // 预留1字节给字符串结束符
        if (len <= 0)
        {
            return -1;
        }
        rxBuf[len] = '\0';
        setRecvData(rxBuf, len);
        return len;
    }
    else
    {
        printf("receive data is unsuccess!\n");
        return -1;
    }
}

/**
 * @brief 串口接收线程
 */
void SerialPort::receiveThread()
{
    printf("开始接收串口数据...\n");
    while (true)
    {
        receive();
        usleep(5000);
    }
}

/**
 * @brief 开启串口接收线程
 */
void SerialPort::startReceiveThread()
{
    std::thread myThread([this]()
                         { this->receiveThread(); });
    myThread.detach(); // 分离线程，不等待其完成
}

/**
 * @brief 设置串口文件描述符
 * @param fd
 */
void SerialPort::setFdSerial(int fdSerial)
{
    std::lock_guard<std::mutex> lock(fdMutex);
    fd = fdSerial;
}

/**
 * @brief 获取串口文件描述符
 * @return
 */
int SerialPort::getFdSerial()
{
    std::lock_guard<std::mutex> lock(fdMutex);
    return fd;
}

/**
 * @brief 设置串口接收数据
 * @param rxBuf
 */
void SerialPort::setRecvData(const uint8_t *rxBuf, int len)
{
    std::lock_guard<std::mutex> lock(recvDataMutex);
    recvDataLen = len;
    std::memcpy(recvData, rxBuf, len);
}

/**
 * @brief 获取串口接收数据
 * @return
 */
RecvData SerialPort::getRecvData()
{
    std::lock_guard<std::mutex> lock(recvDataMutex);
    receiveData.len = recvDataLen;
    receiveData.rxBuf = recvData;
    return receiveData;
}