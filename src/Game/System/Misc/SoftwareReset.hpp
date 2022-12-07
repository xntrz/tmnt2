#pragma once

#include "System/Common/Process/Process.hpp"


class CSoftwareReset : public CProcess
{
private:
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
    static bool Initialize(CProcess* pCurrent);
    static void Terminate(CProcess* pCurrent);

    CSoftwareReset(void);
    virtual ~CSoftwareReset(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;

private:    
    void execReset(void);
    void messageProc(void);
    void clear(void);
    
private:
    static bool m_bEnable;
    float m_fKeyTimer;
    int32 m_iRootSeqLabel;
    MODE m_mode;
};