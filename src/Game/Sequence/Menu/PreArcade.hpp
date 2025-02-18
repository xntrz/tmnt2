#pragma once

#include "System/Common/Process/Sequence.hpp"


#ifdef TMNT2_BUILD_EU

class CPreArcadeSequence : public CSequence
{
private:
    enum STEP
    {
        STEP_RET = 0,
        STEP_FADE_IN,
        STEP_DISP_WARNING,
        STEP_FADE_OUT,
        STEP_EXIT,
    };

public:
    static CProcess* Instance(void);

    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    int32 m_step;
    float m_fTimer;
};

#endif /* TMNT2_BUILD_EU */