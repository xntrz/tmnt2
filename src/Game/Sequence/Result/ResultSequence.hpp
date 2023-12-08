#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CResultWorkPool;


class CResultSequence final : public CAnim2DSequence
{
public:
    static CProcess* Instance(void);

    CResultSequence(void);
    virtual ~CResultSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual void BeginFadein(void) override;
    void ResultItemProc(void);

private:
    CResultWorkPool* m_pResultWorkPool;
    GAMETYPES::RESULTTYPE m_resulttype;
    const char* m_pszLvlUpCrystal;
};