#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/Effect/EffectID.hpp"


class CEffectCommonLoadInfo final : public CGameObjLoadInfo
{
public:
    CEffectCommonLoadInfo(void) {};
    virtual ~CEffectCommonLoadInfo(void) {};
    virtual bool MakeFileName(char* pszFilenameBuff) const override;
};


class CEffectLoadInfo final : public CGameObjLoadInfo
{
public:
    CEffectLoadInfo(EFFECTID::VALUE effectID);
    virtual ~CEffectLoadInfo(void) {};
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    EFFECTID::VALUE m_effectID;
    char m_szEffectName[32];
};