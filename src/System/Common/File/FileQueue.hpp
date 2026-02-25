#pragma once

template<class T>
class CFileQueue
{
public:
    CFileQueue(uint32 uSize);
    ~CFileQueue(void);
    void push(T data);
    void pop(void);
    T front(void);
    bool is_empty(void) const;
    bool is_full(void) const;

private:
    T*      m_pQueue;
    uint32  m_front;
    uint32  m_rear;
    uint32  m_size;
};


template<class T>
CFileQueue<T>::CFileQueue(uint32 size)
: m_pQueue(new T[size])
, m_front(0)
, m_rear(0)
, m_size(size)
{
    ;
};


template<class T>
CFileQueue<T>::~CFileQueue(void)
{
    if (m_pQueue)
    {
        delete[] m_pQueue;
        m_pQueue = nullptr;

        m_size = 0;
    };
};


template<class T>
void CFileQueue<T>::push(T data)
{
    m_pQueue[m_rear] = data;
    if (++m_rear >= m_size)
        m_rear = 0;
};


template<class T>
void CFileQueue<T>::pop(void)
{
    if (++m_front >= m_size)
        m_front = 0;
};


template<class T>
T CFileQueue<T>::front(void)
{
    return m_pQueue[m_front];
};


template<class T>
bool CFileQueue<T>::is_empty(void) const
{
    return (m_front == m_rear);
};


template<class T>
bool CFileQueue<T>::is_full(void) const
{
    return (((m_rear + 1) % m_size) == m_front);
};