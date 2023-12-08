#pragma once

#include "System/Common/Process/Sequence.hpp"


class CDatabaseViewer final : public CSequence
{
private:
    enum PHASE
    {
        PHASE_LOAD = 0,
        PHASE_RUN,
        PHASE_END,
    };
    
public:
    static CProcess* Instance(void);
    
    CDatabaseViewer(void);
    virtual ~CDatabaseViewer(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    PHASE m_ePhase;
};