//
// Created by 王国栋 on 2022/4/8.
//

#ifndef LIBEVENT_CPP_TCPCLIENT_H
#define LIBEVENT_CPP_TCPCLIENT_H

#include "EvBuffer.h"
#include "EventBase.h"
#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>

class TCPClient
{
public:
    TCPClient(EventBase &eventBase);
    TCPClient(const TCPClient &t) = delete;
    TCPClient(TCPClient &&t) = delete;
    ~TCPClient();
    TCPClient &operator=(const TCPClient &t) = delete;
    TCPClient &operator=(TCPClient &&t) = delete;

    /**
     * @brief 设置连接成功回调
     * @param callback 回调函数
     */
    void setConnectCallback(std::function<void()> callback);

    /**
     * @brief 设置超时回调
     * @param callback 回调函数
     */
    void setTimeoutCallback(std::function<void()> callback);

    /**
     * @brief 设置断开链接回调
     * @param callback 回调函数
     */
    void setDisconnectedCallback(std::function<void()> callback);

    /**
     * @brief 设置异常回调
     * @param callback 回调函数
     */
    void setErrorCallback(std::function<void()> callback);

    /**
     * @brief 设置写入回调
     * @param callback 回调函数
     */
    void setWriteCallback(std::function<void()> callback);

    /**
     * @brief 设置读取回调
     * @param callback 回调函数
     */
    void setReadCallback(std::function<void(EvBuffer *)> callback);

    /**
     * @brief 连接服务器
     * @param ip_address 服务器IP
     * @param port 服务器端口
     * @return 成功返回true;失败返回false
     */
    bool connect(std::string ip_address, short port);

    /**
     * @brief 发送数据
     * @param data 数据内容首地址
     * @param len 数据内容长度
     * @return 成功返回true;失败返回false
     */
    bool send(const char *data, size_t len);

private:
    EventBase &m_EventBase;
    EvBuffer *m_EvBuffer;
    struct bufferevent *m_BufferEvent;
    static void read_data_callback(struct bufferevent *bev, void *ctx);
    static void write_data_callback(struct bufferevent *bev, void *ctx);
    static void event_callback(struct bufferevent *bev, short what, void *ctx);

    std::function<void()> m_ConnectedCallback;
    std::function<void()> m_TimeoutCallback;
    std::function<void()> m_DisconnectedCallback;
    std::function<void()> m_ErrorCallback;
    std::function<void(EvBuffer *)> m_ReadCallback;
    std::function<void()> m_WriteCallback;
};

#endif// LIBEVENT_CPP_TCPCLIENT_H
