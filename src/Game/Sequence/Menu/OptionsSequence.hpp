#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"


class COptionsSequence final : public CAnim2DSequence
{
public:
    static CProcess* Instance(void);

    COptionsSequence(void);
    virtual ~COptionsSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual void BeginFadein(void) override;
};