//
// Created by 王国栋 on 2022/4/6.
//

#ifndef LIBEVENT_CPP_TCPSERVER_H
#define LIBEVENT_CPP_TCPSERVER_H

#include "EvBuffer.h"
#include "EventBase.h"
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>

class TCPServer
{
public:
    class TCPConnect
    {
    public:
        TCPConnect(TCPServer* server, int fd, std::string ip, short port);
        ~TCPConnect();

        int getFd();
        EvBuffer& getBuffer();

    private:
        int m_Fd;
        std::string m_Ip;
        short m_Port;
        EvBuffer m_EvBuffer;
        TCPServer* m_Server;
        bufferevent* m_BufferEvent;
        static void read_callback(struct bufferevent* bev, void* ctx);
        static void write_callback(struct bufferevent* bev, void* ctx);
        static void event_callback(struct bufferevent* bev, short what, void* ctx);
    };

public:
    TCPServer(EventBase& eventBase, short port);
    TCPServer(const TCPServer& t) = delete;
    TCPServer(TCPServer&& t) = delete;
    ~TCPServer();
    TCPServer& operator=(const TCPServer& t) = delete;
    TCPServer& operator=(TCPServer&& t) = delete;

    /**
     * @brief 新客户端连接到来时的回调函数
     * @param callback 回调函数,参数TCPConnect为新连接的客户端
     */
    void setNewClientCallback(std::function<void(TCPConnect*)> callback);

    /**
     * @brief 客户端断开链接时的回调函数
     * @param callback 回调函数,参数TCPConnect为断开的客户端
     */
    void setDisconnectedCallback(std::function<void(TCPConnect*)> callback);

    /**
     * @brief 客户端可读时的回调函数
     * @param callback 回调函数,参数TCPConnect为当前读取的客户端
     */
    void setReadClientCallback(std::function<void(TCPConnect*)> callback);

    /**
     * @brief 客户端写入时的回调函数
     * @param callback 回调函数,参数TCPConnect为当前写入的客户端
     */
    void setWriteClientCallback(std::function<void(TCPConnect*)> callback);

    /**
     * @brief 启动TCPServer服务,即开始接收新的客户端链接
     * @return
     */
    void start();

    /**
     * @brief 暂停TCPServer服务,即不在接收新的客户端链接
     */
    void stop();

private:
    EventBase& m_EventBase;
    evconnlistener* m_Listener;
    static void listener_cb(evconnlistener* listener, evutil_socket_t fd, struct sockaddr* addr, int len, void* ptr);

    std::unordered_map<int, std::shared_ptr<TCPConnect>> m_Clients;
    std::function<void(TCPConnect*)> m_NewConnectCallback;
    std::function<void(TCPConnect*)> m_DisconnectCallback;
    std::function<void(TCPConnect*)> m_ReadClientCallback;
    std::function<void(TCPConnect*)> m_WriteClientCallback;
};

#endif// LIBEVENT_CPP_TCPSERVER_H
