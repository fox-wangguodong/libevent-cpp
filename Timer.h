//
// Created by 王国栋 on 2022/4/6.
//

#ifndef LIBEVENT_CPP_TIMER_H
#define LIBEVENT_CPP_TIMER_H

#include "EventBase.h"

class Timer
{
public:
    Timer(EventBase &eventBase);
    Timer(const Timer &t) = delete;
    Timer(Timer &&t) = delete;
    ~Timer();
    Timer &operator=(const Timer &t) = delete;
    Timer &operator=(Timer &&t) = delete;
    explicit operator event *()
    {
        return m_Event;
    }

    /**
   * @brief 添加定时器到事件循环
   * @param callback 定时器回调函数
   * @param ms 定时器间隔
   */
    void addTimer(std::function<void()> callback, std::chrono::milliseconds ms);

    /**
   * @brief 从事件循环中取消定时器
   */
    void cancelTimer();

private:
    event *m_Event;
    std::function<void()> m_Callback;
    static void timer_callback(evutil_socket_t fd, short flags, void *args);
};

#endif// LIBEVENT_CPP_TIMER_H
