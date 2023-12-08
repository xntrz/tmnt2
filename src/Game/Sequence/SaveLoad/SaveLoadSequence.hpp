#pragma once

#include "System/Common/Process/Sequence.hpp"

#include "Game/System/SaveLoad/SaveLoad.hpp"


class CSaveLoadSequence : public CSequence
{
public:
    enum PHASE
    {
        PHASE_FADEIN = 0,
        PHASE_FADEIN_WAIT,
        PHASE_RUN,
        PHASE_FADEOUT,
        PHASE_FADEOUT_WAIT,
        PHASE_EOL,
    };
    
public:
    CSaveLoadSequence(CSaveLoad::TYPE type);
    virtual ~CSaveLoadSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

protected:
    PHASE m_phase;
    CSaveLoad::TYPE m_type;
};


class CSaveLoadAutoSequence final
{
public:
    static CProcess* Instance(void) { return new CSaveLoadSequence(CSaveLoad::TYPE_AUTOSAVE); };
};


class CSaveLoadCheckSequence final
{
public:
    static CProcess* Instance(void) { return new CSaveLoadSequence(CSaveLoad::TYPE_CHECK); };
};


class CSaveLoadMenuSaveSequence final
{
public:
    static CProcess* Instance(void) { return new CSaveLoadSequence(CSaveLoad::TYPE_SAVE); };
};


class CSaveLoadMenuLoadSequence final
{
public:
    static CProcess* Instance(void) { return new CSaveLoadSequence(CSaveLoad::TYPE_LOAD); };
};