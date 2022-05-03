//
// Created by 王国栋 on 2022/5/2.
//
#include "../EventBase.h"
#include "../TCPServer.h"
#include "../Timer.h"
#include <iostream>

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