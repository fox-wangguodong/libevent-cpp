//
// Created by 王国栋 on 2022/4/8.
//

#include "TCPClient.h"
TCPClient::TCPClient(EventBase &eventBase) : m_EventBase(eventBase)
{
    m_BufferEvent = bufferevent_socket_new((event_base *) m_EventBase, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(m_BufferEvent,
                      TCPClient::read_data_callback,
                      TCPClient::write_data_callback,
                      TCPClient::event_callback,
                      (void *) this);
    bufferevent_enable(m_BufferEvent, EV_READ);
    m_EvBuffer = new EvBuffer();
}
TCPClient::~TCPClient()
{
    if (m_EvBuffer != nullptr)
    {
        delete m_EvBuffer;
    }
    if (m_BufferEvent != nullptr)
    {
        bufferevent_free(m_BufferEvent);
    }
}

void TCPClient::setConnectCallback(std::function<void()> callback)
{
    m_ConnectedCallback = callback;
}

void TCPClient::setErrorCallback(std::function<void()> callback)
{
    m_ErrorCallback = callback;
}

void TCPClient::setTimeoutCallback(std::function<void()> callback)
{
    m_TimeoutCallback = callback;
}

void TCPClient::setDisconnectedCallback(std::function<void()> callback)
{
    m_DisconnectedCallback = callback;
}

void TCPClient::setWriteCallback(std::function<void()> callback)
{
    m_WriteCallback = callback;
}

void TCPClient::setReadCallback(std::function<void(EvBuffer *)> callback)
{
    m_ReadCallback = callback;
}

void TCPClient::write_data_callback(struct bufferevent *bev, void *ctx)
{
    TCPClient *client = static_cast<TCPClient *>(ctx);
    if (client == nullptr)
    {
        return;
    }

    client->m_WriteCallback();
}

void TCPClient::read_data_callback(struct bufferevent *bev, void *ctx)
{
    TCPClient *client = static_cast<TCPClient *>(ctx);
    if (client == nullptr)
    {
        return;
    }
    int ret = bufferevent_read_buffer(client->m_BufferEvent, (evbuffer *) client->m_EvBuffer);
    if (ret == 0)
    {
        client->m_ReadCallback(client->m_EvBuffer);
    }
}

void TCPClient::event_callback(struct bufferevent *bev, short what, void *ctx)
{
    TCPClient *client = static_cast<TCPClient *>(ctx);
    if (client == nullptr)
    {
        return;
    }

    if (what & BEV_EVENT_CONNECTED)
    {
        //已经连接
        client->m_ConnectedCallback();
    }
    else if (what & BEV_EVENT_EOF)
    {
        //断开了连接
        client->m_DisconnectedCallback();
    }
    else if (what & BEV_EVENT_TIMEOUT)
    {
        //超时后需要重新设置EV_READ
        bufferevent_enable(client->m_BufferEvent, EV_READ);
        client->m_TimeoutCallback();
    }
    else if (what & BEV_EVENT_ERROR)
    {
        //出现错误
        client->m_ErrorCallback();
    }
}

bool TCPClient::connect(std::string ip_address, short port)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    int ret = bufferevent_socket_connect(m_BufferEvent, (struct sockaddr *) &address, sizeof(address));
    return ret == 0;
}

bool TCPClient::send(const char *data, size_t len)
{
    int ret = bufferevent_write(m_BufferEvent, data, len);
    return ret == 0;
}
