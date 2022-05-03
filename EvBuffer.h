//
// Created by 王国栋 on 2022/5/3.
//

#ifndef LIBEVENT_CPP__EVBUFFER_H
#define LIBEVENT_CPP__EVBUFFER_H

#include <event.h>
#include <event2/event.h>

class EvBuffer
{
public:
    EvBuffer();
    EvBuffer(const EvBuffer &evBuffer) = delete;
    EvBuffer(EvBuffer &&evBuffer);
    virtual ~EvBuffer();
    EvBuffer &operator=(const EvBuffer &evBuffer) = delete;
    EvBuffer &operator=(EvBuffer &&evBuffer);
    explicit operator evbuffer *()
    {
        return m_EvBuffer;
    }

    /**
     * @brief 数据缓冲区加锁
     */
    void lock();

    /**
     * @brief 数据缓冲区释放锁
     */
    void unlock();

    /**
     * @brief 获取数据缓冲区的字节数
     * @return 返回 evbuffer 存储的字节数
     */
    size_t get_length();

    /**
     * @brief 向缓冲区添加数据
     * @param data 数据存储区首地址
     * @param len 数据内容长度
     * @return 成功返回0; 失败返回-1
     */
    bool write(char *data, size_t len);

    /**
     * @brief 从缓冲区读取数据
     * @param data 存储区首地址,用来存储读取的数据
     * @param len 读取的数据字节数
     */
    int read(char *data, size_t len);

    /**
     * @brief 扩容EvBuffer
     * @param size 容量最小字节数
     */
    void extend(size_t size);

private:
    evbuffer *m_EvBuffer;
};


#endif//LIBEVENT_CPP__EVBUFFER_H
