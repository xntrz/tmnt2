#pragma once

#include "System/Common/Process/Process.hpp"


class CMessageWindow;

class CPadConnectCheckProcess : public CProcess
{
private:
    struct MESSAGE
    {
        enum TYPE
        {
            TYPE_RESET = 0,            
        };

        TYPE m_type;
    };
    
public:
    static CProcess* Instance(void);
    static void Initialize(CProcess* pSender);    
    static void Terminate(CProcess* pSender);
    static void Reset(CProcess* pSender);
    
    CPadConnectCheckProcess(void);
    virtual ~CPadConnectCheckProcess(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
    void MessageProc(void);    
    void DrawFilter(void) const;
    bool SeqSleep(int32 iLabel);
    bool SeqWakeup(int32 iLabel);
    void WarningMsgSet(int32 iControllerPort);
    void WarningMsgClear(void);
    void WarningBegin(int32 iController);
    void WarningEnd(void);
    bool WarningIsShowing(void) const;
    
private:
    bool m_bSleepFlag;
    int32 m_iSeqLabel;
    CMessageWindow* m_pWindow;
    int32 m_iWarningMsgTextID;
};