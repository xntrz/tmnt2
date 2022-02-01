#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"


class CMapEffectLightGimmick : public CGimmick
{
public:
    CMapEffectLightGimmick(const char* pszName, void* pParam);
    virtual ~CMapEffectLightGimmick(void);
    virtual void Draw(void) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    void setLightEnable(bool bEnable);

protected:
    RwV3d m_vPosition;
    RwRGBA m_Color;
    uint32 m_hLight;
};


class CMapEffectGimmick : public CGimmick
{
protected:
    struct EFFECTINFO
    {
        const char* m_pszName;
        bool m_bMagic;
        bool m_bLoop;
        float m_fIntervalTime;
        float m_fScale;
        float m_fRandAreaRadius;
    };

public:
    CMapEffectGimmick(const char* pszName, void* pParam);
    virtual ~CMapEffectGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    void setEffectInfo(int32 id, int32 subid);
    void removeEffect(void);
    bool playEffect(void);

protected:
    static const EFFECTINFO m_aSmokeEffectKindInfo[];
    static const EFFECTINFO m_aFlameEffectKindInfo[];
    static const EFFECTINFO m_aSteamEffectKindInfo[];
    static const EFFECTINFO m_aSparkEffectKindInfo[];
    static const EFFECTINFO m_aExploEffectKindInfo[];
    static const EFFECTINFO m_aBubbleEffectKindInfo[];
    static const EFFECTINFO m_aWatterFalleMistEffectKindInfo[];
    static const EFFECTINFO m_aWatterFalleSprashEffectKindInfo[];
    RwV3d m_vPosition;
    RwV3d m_vDirection;
    float m_fTime;
    uint32 m_hMagic;
    uint32 m_hEffect;
    const EFFECTINFO* m_pEffectInfo;
    bool m_bEnable;
    bool m_bLoopActive;
};