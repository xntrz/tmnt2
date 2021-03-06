#pragma once

#include "System/Common/Process/Process.hpp"


class CLoadingDisplay : public CProcess
{
public:
    enum MODE
    {
        MODE_NORMAL = 0,        
    };

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
    static void Initialize(void);
    static void Terminate(void);
    static void Start(MODE mode = MODE_NORMAL);
    static void Stop(void);

    CLoadingDisplay(void);
    virtual ~CLoadingDisplay(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;

private:    
    void messageProc(void);

private:
    static MESSAGE m_message;
    MODE m_mode;
    STATE m_state;
    float m_fPhase;
};