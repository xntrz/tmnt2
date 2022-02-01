#pragma once

#include "System/Common/Process/Sequence.hpp"


class CEndingSequence final : public CSequence
{
private:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_MOVIE,
        STEP_STAFFROLL,
        STEP_RESULT,
        STEP_UNLOCK,
        STEP_SAVE,
        STEP_EOL,
    };
    
public:
    static CProcess* Instance(void);

    CEndingSequence(void);
    virtual ~CEndingSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
    void PostSequence(void);
    void Branch(void);

private:
    int32 m_iType;
    STEP m_step;
};