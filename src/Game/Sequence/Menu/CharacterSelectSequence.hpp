#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"


class CPlayerSelectWorkPool;


class CCharacterSelectSequence final : public CAnim2DSequence
{
public:
    static CProcess* Instance(void);

    CCharacterSelectSequence(void);
    virtual ~CCharacterSelectSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual void BeginFadeIn(void) override;

private:
    CPlayerSelectWorkPool* m_pWorkPool;
};