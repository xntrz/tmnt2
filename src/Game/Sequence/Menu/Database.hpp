#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"


class CDatabase final : public CAnim2DSequence
{
public:
    static CProcess* Instance(void);

    CDatabase(void);
    virtual ~CDatabase(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual void BeginFadeIn(void) override;
    void MessageProc(void);

private:
    int32 m_iSelWindowType;
    bool  m_bGotoViewerFlag;
};