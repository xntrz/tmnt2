#pragma once

#include "System/Common/Process/Process.hpp"


class CTimeoutProcess : public CProcess
{
private:
    static const float TIMEOUT_SEC_NONINPUT;
    static const float TIMEOUT_SEC_TOTAL;
    
    union PARAM
    {
        bool m_bEnable;
        bool m_bInteractive;
    };

    struct MESSAGE
    {
        enum TYPE
        {
            TYPE_START = 0,
            TYPE_RESET,
            TYPE_ENABLE,
            TYPE_INTERACTIVE,
        };

        TYPE m_type;
        PARAM m_param;
    };
    
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_WATCHTIMER,
        STATE_TIMEISUP,
    };

    class CTimerObj;
    class CNonInputTimer;

public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);
    static void Terminate(CProcess* pSender);
    static void Start(CProcess* pSender);
    static void Reset(CProcess* pSender);
    static void Enable(CProcess* pSender, bool bEnable);
    static void SetInteractive(CProcess* pSender, bool bInteractive);
    
    CTimeoutProcess(void);
    virtual ~CTimeoutProcess(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;

private:
    static bool postPrivateMessage(CProcess* pSender, MESSAGE* pMessage);
    void execTimeout(void);
    void messageProc(void);
    void resetObject(void);
    void changeState(STATE eState);

private:
    STATE m_eState;
    int32 m_iRootSeqLabel;
    CTimerObj* m_apTimer[2];
    int32 m_nTimeoutTimerNo;
    bool m_bEnable;
    bool m_bInteractive;
};