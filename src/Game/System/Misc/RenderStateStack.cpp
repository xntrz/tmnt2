#include "RenderStateStack.hpp"


CRenderStateStack::CRenderStateStack(void)
: m_flag(0)
{
    ;
};


CRenderStateStack::~CRenderStateStack(void)
{
    for (int32 i = 0; i < RSVALUE_NUM; ++i)
    {
        uint32 rsBit = (1 << i);

        if (m_flag & rsBit)
        {
            RwRenderStateSet(static_cast<RwRenderState>(i), m_aValue[i].m_pVoid);
            m_flag &= (~rsBit);
        };
    };
};


void CRenderStateStack::Push(RwRenderState rs)
{
    if (rs == rwRENDERSTATETEXTURERASTER)
        return;

    ASSERT(rs >= 0);
    ASSERT(rs < RSVALUE_NUM);

    uint32 rsBit = (1 << static_cast<int32>(rs));

    if (!(m_flag & rsBit))
    {
        RwRenderStateGet(rs, reinterpret_cast<void*>(&m_aValue[rs]));
        m_flag |= rsBit;
    };
};


void CRenderStateStack::set(RwRenderState rs, void* pValue)
{
    RwRenderStateSet(rs, pValue);
};