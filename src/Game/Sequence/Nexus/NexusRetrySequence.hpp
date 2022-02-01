#pragma once

#include "System/Common/Process/Sequence.hpp"


class CDialog;


class CNexusRetrySequence final : public CSequence
{
private:
    enum STEP
    {
        STEP_START = 0,
        STEP_FADEOUT,
        STEP_RUN,
        STEP_FADEIN,
        STEP_EOL,
    };
    
public:
    static CProcess* Instance(void);

    CNexusRetrySequence(void);
    virtual ~CNexusRetrySequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
    
private:
    STEP m_step;
    CDialog* m_pDlg;
    bool m_bRetryFlag;
};