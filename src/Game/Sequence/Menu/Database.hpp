#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"


class CDatabase final : public CAnim2DSequence
{
public:
    static CProcess* Instance(void);

    CDatabase(void);
    virtual ~CDatabase(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;
    virtual void BeginFadeOut(void) override;
    virtual bool OnRet(void) override;

private:
    int32 m_iSelWindowType;
    bool m_bGotoViewerFlag;
};