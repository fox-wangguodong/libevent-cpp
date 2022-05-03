//
// Created by 王国栋 on 2022/4/6.
//

#include "Timer.h"

Timer::Timer(EventBase &eventBase)
{
    m_Event = event_new(static_cast<event_base *>(eventBase), -1, EV_PERSIST, &Timer::timer_callback, this);
    assert(m_Event != nullptr);
}
Timer::~Timer()
{
    if (m_Event != nullptr)
    {
        event_free(m_Event);//释放event
    }
}
void Timer::addTimer(std::function<void()> callback, std::chrono::milliseconds ms)
{
    m_Callback = callback;
    timeval time_val;
    time_val.tv_sec = ms.count() / 1000;
    time_val.tv_usec = 1000 * (ms.count() % 1000);
    event_add(m_Event, &time_val);// 将event注册到eventBase
}
void Timer::cancelTimer()
{
    event_del(m_Event);// 将event设置为：非pending、非active状态
}
void Timer::timer_callback(int fd, short flags, void *args)
{
    Timer *timer = static_cast<Timer *>(args);
    if (timer != nullptr)
    {
        timer->m_Callback();
    }
}
