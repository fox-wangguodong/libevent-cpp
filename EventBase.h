//
// Created by 王国栋 on 2022/3/30.
//

#ifndef LIBEVENT_CPP_EVENTBASE_H
#define LIBEVENT_CPP_EVENTBASE_H

#include <atomic>
#include <chrono>
#include <event2/event.h>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>


class EventBase
{
public:
    EventBase();
    EventBase(const EventBase &eventBase) = delete;
    EventBase(EventBase &&eventBase) = delete;
    virtual ~EventBase();
    EventBase &operator=(const EventBase &eventBase) = delete;
    EventBase &operator=(EventBase &&eventBase) = delete;
    explicit operator event_base *()
    {
        return m_EventBase;
    }

    /**
     * @brief 添加事件到事件循环中执行
     * @param callback
     */
    void runInLoop(std::function<void()> callback);

    /**
     * @brief 开始事件循环,即使没有事件激活也不退出循环
     */
    void loop();

    /**
     * @brief 分发事件,当base中没有激活的event时将返回
     * @return 成功返回0; 失败返回-1
     */
    int dispatch();

    /**
     * @brief 在指定时间内停止事件循环
     * @param ms 延迟时间,若为0则表示无限延时的停止事件循环,直到处理完所有激活事件后退出事件循环
     * @return 成功返回0; 失败返回-1
     */
    int loop_exit(std::chrono::milliseconds ms = std::chrono::milliseconds(0));

    /**
     * @brief 立即停止事件循环,执行完当前正在处理的激活事件后立即退出事件循环
     * @return 成功返回0; 失败返回-1
     */
    int loop_break();

    /**
     * @brief 获取当前系统时间戳
     * @return 返回距离1970.1.1时刻的毫秒数
     */
    std::chrono::milliseconds getTimeStamp();

private:
    event_base *m_EventBase;
    std::queue<std::function<void()>> m_Callback_list;
    static void callback(evutil_socket_t fd, short flags, void *args);
};

#endif// LIBEVENT_CPP_EVENTBASE_H
