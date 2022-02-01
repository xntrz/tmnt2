#pragma once

#include "MagicTypes.hpp"


class CMagic;


class CMagicParameter
{
public:
    friend CMagic;

public:
    CMagicParameter(void);
    ~CMagicParameter(void);
    void SetBaseEffectName(const char* pszName);
    void SetFeature(MAGICTYPES::FEATURE feature);
    void AddFeature(MAGICTYPES::FEATURE feature);
    void SetCollisionBody(const RwV3d* pvPosition, float fRadius);
    void SetCollisionAttack(
        const RwV3d* pvPosition,
        float fRadius,
        int32 nPower,
        int32 nAntiguard,
        int32 nStatus,
        float fTime = 0.0f,
        float fFlyX = 0.0f,
        float fFlyY = 0.0f
    );
    void SetConfusionFlag(bool bSet);
    void SetMovement(const RwV3d* pvVelocity, const RwV3d* pvAcceleration);
    void SetLocusInfo(MAGICTYPES::LOCUSINFO& rLocusInfo);
    void SetReflectNum(int32 nReflectNum);
    void SetLive(float fLivetime);
    void SetChangeSize(float fChangeSize);
    void SetScale(float fScale);
    void SetHitTiming(float fStart, float fEnd);
    void SetAppearEffectName(const char* pszName);
    void SetVanishEffectName(const char* pszName);
    void SetReflectEffectName(const char* pszName);
    void SetVanishMagicName(const char* pszName);

private:
    const char* m_pszBaseEffectName;
    MAGICTYPES::FEATURE m_feature;
    MAGICTYPES::COLLISIONBODY m_collisionBody;
    MAGICTYPES::COLLISIONATTACK m_collisionAttack;
    MAGICTYPES::MOVEMENT m_movement;
    MAGICTYPES::LOCUSINFO m_locusinfo;    
    int32 m_nReflectNumMax;
    float m_fLivetime;
    float m_fChangeSize;
    float m_fScale;
    float m_fHitTimingStart;
    float m_fHitTimingEnd;
    const char* m_pszAppearEffectName;
    const char* m_pszVanishEffectName;
    const char* m_pszReflectEffectName;
    const char* m_pszVanishMagicName;
};