#pragma once

#include "System/Common/Process/Process.hpp"


class CLoadingDisplay : public CProcess
{
public:
    enum MODE
    {
        MODE_NORMAL = 0,        
    };

private:    
    enum STATE
    {
        STATE_NONE = 0,
        STATE_DISPLAY,
    };

    struct MESSAGE
    {
        enum TYPE
        {
            TYPE_START = 0,
            TYPE_STOP,
        };

        TYPE m_type;
        MODE m_mode;
    };

public:
    static CProcess* Instance(void);
    static bool Initialize(CProcess* pCurrent);
    static void Terminate(CProcess* pCurrent);
    static bool Start(CProcess* pSender, MODE mode = MODE_NORMAL);
    static void Stop(CProcess* pSender);

    CLoadingDisplay(void);
    virtual ~CLoadingDisplay(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;

private:
    static bool postPrivateMessage(CProcess* pSender, MESSAGE* pMessage);
    void messageProc(void);

private:
    MODE m_mode;
    STATE m_state;
    float m_fPhase;
};