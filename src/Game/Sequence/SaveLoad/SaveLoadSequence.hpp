#pragma once

#include "System/Common/Process/Sequence.hpp"

#include "Game/System/SaveLoad/SaveLoad.hpp"


class CSaveLoadSequenceBase : public CSequence
{
public:
    enum PHASE
    {
        PHASE_FADEOUT = 0,
        PHASE_FADEOUT_WAIT,
        PHASE_RUN,
        PHASE_FADEIN,
        PHASE_FADEIN_WAIT,
        PHASE_EOL,
    };
    
public:
    static CProcess* Instance(void);

    CSaveLoadSequenceBase(CSaveLoad::TYPE type);
    virtual ~CSaveLoadSequenceBase(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;

protected:
    PHASE m_phase;
    CSaveLoad::TYPE m_type;
};