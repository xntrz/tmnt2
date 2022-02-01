#pragma once


template<class T>
class CQueue final
{
public:
    inline CQueue(uint32 uSize)
    : m_pQueue(new T[uSize])
    , m_uTop(0)
    , m_uEnd(0)
    , m_uSize(uSize)
    {
        ASSERT(m_pQueue);
    };

    
    inline virtual ~CQueue(void)
    {
        if (m_pQueue)
        {
            delete[] m_pQueue;
            m_pQueue = nullptr;
        };
    };

    
    inline bool empty(void) const
    {
        return (m_uTop == m_uEnd);
    };
    

    inline void push(const T* pT)
    {
        ASSERT(m_pQueue);

        m_pQueue[m_uEnd++] = *pT;
        if (m_uEnd >= m_uSize)
            m_uEnd = 0;

        ASSERT(!empty());
    };

    
    inline void pop(void)
    {
        ++m_uTop;
        if (m_uTop >= m_uSize)
            m_uTop = 0;
    };

    
    inline T* peek(void) const
    {
        return &m_pQueue[m_uTop];
    };

    
private:
    T* m_pQueue;
    uint32 m_uTop;
    uint32 m_uEnd;
    uint32 m_uSize;
};