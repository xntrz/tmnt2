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
        if (IS_FLAG_SET(m_flag, (1 << i)))
        {
            RwRenderStateSet(RwRenderState(i), (void*)m_aValue[i].m_pVoid);
            FLAG_CLEAR(m_flag, (1 << i));
        };
    };
};


void CRenderStateStack::set(RwRenderState rs, void* pValue)
{
    RwRenderStateSet(rs, pValue);
};


void CRenderStateStack::push(RwRenderState rs, void* pValue)
{
    if (rs == rwRENDERSTATETEXTURERASTER)
        return;

    ASSERT(rs >= 0 && rs < RSVALUE_NUM);
    if (!IS_FLAG_SET(m_flag, (1 << rs)))
    {
        RwRenderStateGet(rs, (void*)&m_aValue[ rs ]);
        FLAG_CLEAR(m_flag, (1 << rs));
    };
};
