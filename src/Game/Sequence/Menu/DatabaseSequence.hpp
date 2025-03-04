#pragma once

#include "System/Common/Process/Sequence.hpp"


class CDatabaseSequence final : public CSequence
{
public:
    static void SetDBViewerNo(int32 iDBViewerNo);
    static int32 GetDBViewerNo(void);
    static CProcess* Instance(void);

    CDatabaseSequence(void);
    virtual ~CDatabaseSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    int32 DatabaseBranch(int32 iLabel);
    
private:
    int32        m_iLabelCurrent;
};