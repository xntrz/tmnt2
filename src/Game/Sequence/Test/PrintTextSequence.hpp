#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Sequence.hpp"


class CPrintTextSequence final : public CSequence
{
public:
    static CProcess* Instance(void) { return new CPrintTextSequence; };
    
    CPrintTextSequence(void) {};
    virtual ~CPrintTextSequence(void) {};
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override {};
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override {};

private:
    int32 m_type;
};


#endif /* _DEBUG */