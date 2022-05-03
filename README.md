# libevent-cpp

仿照muduo库封装libevent库,提供现代c++接口


# 封装定时器类

设置好定时任务后,通过定时器可以实现定时触发事件回调

```
int main()
{
    EventBase eventBase;
    Timer timer(eventBase);
    timer.addTimer(
            []()
            {
                std::cout << "tick" << std::endl;
            },
            std::chrono::milliseconds(1000));
    eventBase.loop();
    return 0;
}
```

# TCPClient

TCP客户端,提供更简洁的回调接口,方便使用

```
int main()
{
    EventBase eventBase;
    TCPClient tcpClient(eventBase);
    tcpClient.setConnectCallback(
            []()
            {
                std::cout << "client connected !" << std::endl;
            });
    tcpClient.setTimeoutCallback(
            []()
            {
                std::cout << "client write timeout !" << std::endl;
            });
    tcpClient.setDisconnectedCallback(
            []()
            {
                std::cout << "client disconnected !" << std::endl;
            });
    tcpClient.setErrorCallback(
            []()
            {
                std::cout << "client error !" << std::endl;
            });
    tcpClient.setWriteCallback(
            []()
            {
                std::cout << "client writed !" << std::endl;
            });
    tcpClient.setReadCallback(
            [](EvBuffer* evBuffer)
            {
                char buffer[1024];
                size_t len = evBuffer->read(buffer, 1024);
                std::cout << "client read " << len << " bytes !" << std::endl;
            });
    tcpClient.connect("127.0.0.1", 8080);
    Timer timer(eventBase);
    timer.addTimer(
            [&]()
            {
                std::string data = "hello";
                tcpClient.send(data.c_str(), data.size());
                std::cout << data << std::endl;
            },
            std::chrono::milliseconds(1000));
    eventBase.loop();
    return 0;
}
```

# TCPServer

TCP服务端使用方式

```
int main()
{
    EventBase eventBase;

    TCPServer tcpServer(eventBase, 8080);
    tcpServer.setNewClientCallback(
            [](TCPServer::TCPConnect *tcpConnect)
            {
                std::cout << "client connected " << tcpConnect->getFd() << std::endl;
            });
    tcpServer.setDisconnectedCallback(
            [](TCPServer::TCPConnect *tcpConnect)
            {
                std::cout << "client disconnected " << tcpConnect->getFd() << std::endl;
            });
    tcpServer.setReadClientCallback(
            [](TCPServer::TCPConnect *tcpConnect)
            {
                auto len = tcpConnect->getBuffer().get_length();
                char buffer[len];
                auto dataLength = tcpConnect->getBuffer().read(buffer, len);
                std::cout << "client " << tcpConnect->getFd() << " read data:" << buffer << std::endl;
            });
    tcpServer.setWriteClientCallback(
            [](TCPServer::TCPConnect *tcpConnect)
            {
                std::cout << "write client " << tcpConnect->getFd() << std::endl;
            });
    tcpServer.start();
    eventBase.loop();
    return 0;
}
```


