#pragma once

#include "System/Common/Process/Sequence.hpp"
#include "Game/Component/GameMain/SecretID.hpp"


class CUnlockMessage;


class CUnlockSequence final : public CSequence
{
private:
    enum PHASE
    {
        PHASE_LOAD = 0,
        PHASE_FADEIN,
        PHASE_FADEIN_WAIT,
        PHASE_NOTIFY,
        PHASE_FADEOUT,
        PHASE_FADEOUT_WAIT,
        PHASE_EOL,
    };

    enum NOTIFYPHASE
    {
        NOTIFYPHASE_PEEK = 0,
        NOTIFYPHASE_OPEN,
        NOTIFYPHASE_SHOW,
        NOTIFYPHASE_CLOSE,
        NOTIFYPHASE_EOL,        
    };

public:
    static CProcess* Instance(void);

    CUnlockSequence(void);
    virtual ~CUnlockSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    bool Notify(void);
    
private:
    PHASE m_phase;
    NOTIFYPHASE m_notifyphase;
    SECRETID::VALUE m_UnnnotifiedSecretID;
    CUnlockMessage* m_pUnlockMessage;
};