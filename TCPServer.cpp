//
// Created by 王国栋 on 2022/4/6.
//

#include "TCPServer.h"

TCPServer::TCPServer(EventBase &eventBase, short port) : m_EventBase(eventBase)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    m_Listener = evconnlistener_new_bind((event_base *) m_EventBase,
                                         TCPServer::listener_cb,
                                         this,
                                         LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_DISABLED,
                                         36,
                                         (struct sockaddr *) &server_address,
                                         sizeof(server_address));
}

TCPServer::~TCPServer()
{
    if (m_Listener != nullptr)
    {
        evconnlistener_free(m_Listener);
        m_Listener = nullptr;
    }
}

void TCPServer::listener_cb(struct evconnlistener *listener, int fd, struct sockaddr *addr, int len, void *ptr)
{
    TCPServer *tcpServer = static_cast<TCPServer *>(ptr);
    if (tcpServer == nullptr)
    {
        return;
    }

    sockaddr_in *clientAddress = (sockaddr_in *) addr;
    std::shared_ptr<TCPConnect> tcpConnect = std::make_shared<TCPConnect>(tcpServer,
                                                                          fd,
                                                                          inet_ntoa(clientAddress->sin_addr),
                                                                          ntohs(clientAddress->sin_port));
    tcpServer->m_Clients.emplace(fd, tcpConnect);
    tcpServer->m_NewConnectCallback(tcpConnect.get());
}

void TCPServer::setNewClientCallback(std::function<void(TCPConnect *)> callback)
{
    m_NewConnectCallback = callback;
}
void TCPServer::setDisconnectedCallback(std::function<void(TCPConnect *)> callback)
{
    m_DisconnectCallback = callback;
}
void TCPServer::setReadClientCallback(std::function<void(TCPConnect *)> callback)
{
    m_ReadClientCallback = callback;
}
void TCPServer::setWriteClientCallback(std::function<void(TCPConnect *)> callback)
{
    m_WriteClientCallback = callback;
}

void TCPServer::start()
{
    evconnlistener_enable(m_Listener);
}

void TCPServer::stop()
{
    evconnlistener_disable(m_Listener);
}

TCPServer::TCPConnect::TCPConnect(TCPServer *server, int fd, std::string ip, short port)
: m_Server(server), m_Fd(fd), m_Ip(ip), m_Port(port)
{
    m_BufferEvent = bufferevent_socket_new((event_base *) m_Server->m_EventBase, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(m_BufferEvent,
                      TCPConnect::read_callback,
                      TCPConnect::write_callback,
                      TCPConnect::event_callback,
                      this);
    bufferevent_enable(m_BufferEvent, EV_READ);
}

TCPServer::TCPConnect::~TCPConnect()
{
    if (m_BufferEvent != nullptr)
    {
        bufferevent_free(m_BufferEvent);
        m_BufferEvent = nullptr;
    }
}

void TCPServer::TCPConnect::read_callback(struct bufferevent *bev, void *ctx)
{
    TCPConnect *tcpConnect = static_cast<TCPConnect *>(ctx);
    if (tcpConnect != nullptr)
    {
        bufferevent_read_buffer(tcpConnect->m_BufferEvent, (evbuffer *) tcpConnect->m_EvBuffer);
        tcpConnect->m_Server->m_ReadClientCallback(tcpConnect);
    }
}
void TCPServer::TCPConnect::write_callback(struct bufferevent *bev, void *ctx)
{
    TCPConnect *tcpConnect = static_cast<TCPConnect *>(ctx);
    if (tcpConnect != nullptr)
    {
        tcpConnect->m_Server->m_WriteClientCallback(tcpConnect);
    }
}
void TCPServer::TCPConnect::event_callback(struct bufferevent *bev, short what, void *ctx)
{
    TCPConnect *tcpConnect = static_cast<TCPConnect *>(ctx);
    if (tcpConnect == nullptr)
    {
        return;
    }
    if (what & BEV_EVENT_CONNECTED)
    {
        //已经连接
        tcpConnect->m_Server->m_NewConnectCallback(tcpConnect);
    }
    else if (what & BEV_EVENT_EOF)
    {
        //断开了连接
        tcpConnect->m_Server->m_DisconnectCallback(tcpConnect);
    }
    else if (what & BEV_EVENT_TIMEOUT)
    {
        //超时后需要重新设置EV_READ
        bufferevent_enable(tcpConnect->m_BufferEvent, EV_READ);
    }
    else if (what & BEV_EVENT_ERROR)
    {
        //出现错误,则删除该tcpConnect
        tcpConnect->m_Server->m_DisconnectCallback(tcpConnect);
        auto iter = tcpConnect->m_Server->m_Clients.find(tcpConnect->m_Fd);
        if (iter != tcpConnect->m_Server->m_Clients.end())
        {
            tcpConnect->m_Server->m_Clients.erase(iter);
        }
    }
}

int TCPServer::TCPConnect::getFd()
{
    return m_Fd;
}

EvBuffer &TCPServer::TCPConnect::getBuffer()
{
    return m_EvBuffer;
}
