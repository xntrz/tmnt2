#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Sequence.hpp"

#include "DebugUtils.hpp"


class CDebugMenuSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CDebugMenuSequence(void);
    virtual ~CDebugMenuSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    CDebugMenuCtrl m_menu;
};


#endif /* _DEBUG */