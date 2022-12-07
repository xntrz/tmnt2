#pragma once

#include "System/Common/Process/Sequence.hpp"


class CDatabaseSequence final : public CSequence
{
public:
    static int32 GetDBViewerNo(void);
    static void SetDBViewerNo(int32 iDbViewerNo);
    static CProcess* Instance(void);

    CDatabaseSequence(void);
    virtual ~CDatabaseSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;

private:
    int32 DatabaseBranch(int32 iLabel);
    
private:
    int32 m_iLabelCurrent;
};