#include "RenderState.hpp"


/*static*/ CRenderState::_Value CRenderState::m_apValue[BITSOF(RwRenderState)];


/*static*/ void CRenderState::Initialize(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apValue); i++)
		RwRenderStateGet(RwRenderState(i), (void*)&m_apValue[i]);		
};