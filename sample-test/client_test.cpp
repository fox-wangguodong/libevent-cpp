//
// Created by 王国栋 on 2022/5/2.
//

#include "../EventBase.h"
#include "../TCPClient.h"
#include "../Timer.h"
#include <iostream>

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