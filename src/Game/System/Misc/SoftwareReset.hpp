#pragma once

#include "System/Common/Process/Process.hpp"


class CSoftwareResetProcess : public CProcess
{
public:
    enum MODE
    {
        MODE_NORMAL = 0,
        MODE_DEBUGMENU,
    };
    
    struct MESSAGE
    {
        enum TYPE
        {
            TYPE_MODE = 0,
            TYPE_ENABLE,
            TYPE_ROOTSEQ,
        };

        union PARAM
        {
            bool m_bEnable;
            int32 m_iRootSeqLabel;
            MODE m_mode;
        };

        TYPE m_type;
        PARAM m_param;
    };
    
public:
    static CProcess* Instance(void);
    static bool Initialize(CProcess* pSender, MODE mode);
    static void Terminate(CProcess* pSender);
    static void SetEnable(CProcess* pSender, bool bState);

    CSoftwareResetProcess(void);
    virtual ~CSoftwareResetProcess(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;

private:
    static bool postPrivateMessage(CProcess* pSender, MESSAGE* pMsg);
    void execReset(void);
    void messageProc(void);
    void clear(MODE mode);
    
private:
    static bool m_bEnable;
    static float m_fResetResponseSec;
    float m_fKeyTimer;
    int32 m_iRootSeqLabel;
    MODE m_mode;
};