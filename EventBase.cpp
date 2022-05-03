//
// Created by 王国栋 on 2022/3/30.
//

#include "EventBase.h"

void EventBase::callback(int fd, short flags, void *args)
{
    EventBase *eventBase = dynamic_cast<EventBase *>((EventBase *) args);//将传入的参数转为Event指针后调用回调函数
    if (eventBase != nullptr && eventBase->m_Callback_list.size() > 0)
    {
        auto &functor = eventBase->m_Callback_list.front();
        functor();
        eventBase->m_Callback_list.pop();
    }
}

EventBase::EventBase()
{
    m_EventBase = event_base_new();// 创建eventBase
    assert(m_EventBase != nullptr);
}

EventBase::~EventBase()
{
    if (m_EventBase != nullptr)
    {
        event_base_free(m_EventBase);// 释放eventBase
        m_EventBase = nullptr;
    }
}

std::chrono::milliseconds EventBase::getTimeStamp()
{
    timeval time_val;
    bool ret = event_base_gettimeofday_cached(m_EventBase, &time_val);
    if (ret == 0)
    {
        return std::chrono::milliseconds(time_val.tv_sec * 1000 + time_val.tv_usec / 1000);
    }
    else
    {
        auto time_point = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        return std::chrono::milliseconds(time_point.time_since_epoch().count());
    }
}
int EventBase::loop_break()
{
    return event_base_loopbreak(m_EventBase);
}

int EventBase::loop_exit(std::chrono::milliseconds ms)
{
    if (ms.count() == 0)
    {
        return event_base_loopexit(m_EventBase, NULL);
    }
    else
    {
        timeval time_val;
        time_val.tv_sec = ms.count() / 1000;
        time_val.tv_usec = (ms.count() % 1000) * 1000;
        return event_base_loopexit(m_EventBase, &time_val);
    }
}

int EventBase::dispatch()
{
    return event_base_dispatch(m_EventBase);//等同于 event_base_loop(m_EventBase,0);
}

void EventBase::loop()
{
    event_base_loop(m_EventBase, EVLOOP_NO_EXIT_ON_EMPTY);
}

void EventBase::runInLoop(std::function<void()> callback)
{
    m_Callback_list.push(callback);
    event_base_once(m_EventBase, -1, EV_TIMEOUT, &EventBase::callback, this, nullptr);
}
