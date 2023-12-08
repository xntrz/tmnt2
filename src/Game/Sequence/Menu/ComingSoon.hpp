#pragma once

#include "System/Common/Process/Sequence.hpp"


class CComingSoonSequence final : public CSequence
{
private:
    enum STEP
    {
        STEP_READFILE = 0,
        STEP_FADEIN,
        STEP_DRAW,
        STEP_FADEOUT,
        STEP_END,        
    };

public:
    static CProcess* Instance(void);

    CComingSoonSequence(void);
    virtual ~CComingSoonSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void DrawScreen(float t) const;
    
private:
    STEP m_eStep;
    float m_fTimer;
    RwTexture* m_pTexture;
};