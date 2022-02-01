#include "MagicParameter.hpp"


CMagicParameter::CMagicParameter(void)
: m_pszBaseEffectName(nullptr)
, m_feature(MAGICTYPES::FEATURE_NONE)
, m_collisionBody({ 0 })
, m_collisionAttack({ 0 })
, m_movement({ 0 })
, m_locusinfo({ 0 })
, m_nReflectNumMax(0)
, m_fLivetime(0.0f)
, m_fChangeSize(0.0f)
, m_fScale(1.0f)
, m_fHitTimingStart(0.0f)
, m_fHitTimingEnd(0.0f)
, m_pszAppearEffectName(nullptr)
, m_pszVanishEffectName(nullptr)
, m_pszReflectEffectName(nullptr)
, m_pszVanishMagicName(nullptr)
{
    ;
};


CMagicParameter::~CMagicParameter(void)
{
    ;
};


void CMagicParameter::SetBaseEffectName(const char* pszName)
{
    ASSERT(pszName);
    m_pszBaseEffectName = pszName;
};


void CMagicParameter::SetFeature(MAGICTYPES::FEATURE feature)
{
    m_feature = feature;
};


void CMagicParameter::AddFeature(MAGICTYPES::FEATURE feature)
{
    uint32 uFlagField = m_feature;
    FLAG_CHANGE(uFlagField, feature, true);
    m_feature = MAGICTYPES::FEATURE(uFlagField);
};


void CMagicParameter::SetCollisionBody(const RwV3d* pvPosition, float fRadius)
{
    if (pvPosition)
        m_collisionBody.m_vPosition = *pvPosition;

    m_collisionBody.m_fRadius = fRadius;    
};


void CMagicParameter::SetCollisionAttack(
    const RwV3d* pvPosition,
    float fRadius,
    int32 nPower,
    int32 nAntiguard,
    int32 nStatus,
    float fTime,
    float fFlyX,
    float fFlyY
)
{
    if (pvPosition)
        m_collisionAttack.m_vPosition = *pvPosition;

    m_collisionAttack.m_fRadius = fRadius;
    m_collisionAttack.m_nDamage = nPower;
    m_collisionAttack.m_nAntiguard = nAntiguard;
    m_collisionAttack.m_nStatus = nStatus;
    m_collisionAttack.m_fStatusTime = fTime;
    m_collisionAttack.m_fFlyawayX = fFlyX;
    m_collisionAttack.m_fFlyawayY = fFlyY;
};


void CMagicParameter::SetConfusionFlag(bool bSet)
{
    m_collisionAttack.m_bConfusion = bSet;
};


void CMagicParameter::SetMovement(const RwV3d* pvVelocity, const RwV3d* pvAcceleration)
{
    if (pvVelocity)
        m_movement.m_vVelocity = *pvVelocity;

    if (pvAcceleration)
        m_movement.m_vAcceleration = *pvAcceleration;
};


void CMagicParameter::SetLocusInfo(MAGICTYPES::LOCUSINFO& rLocusInfo)
{
    m_locusinfo = rLocusInfo;
};


void CMagicParameter::SetReflectNum(int32 nReflectNum)
{
    m_nReflectNumMax = nReflectNum;
};


void CMagicParameter::SetLive(float fLivetime)
{
    m_fLivetime = fLivetime;
    m_fHitTimingStart = 0.0f;
    m_fHitTimingEnd = m_fLivetime;
};


void CMagicParameter::SetChangeSize(float fChangeSize)
{
    m_fChangeSize = fChangeSize;
};


void CMagicParameter::SetScale(float fScale)
{
    m_fScale = fScale;
};


void CMagicParameter::SetHitTiming(float fStart, float fEnd)
{
    m_fHitTimingStart = fStart;
    m_fHitTimingEnd = fEnd;
};


void CMagicParameter::SetAppearEffectName(const char* pszName)
{
    m_pszAppearEffectName = pszName;
};


void CMagicParameter::SetVanishEffectName(const char* pszName)
{
    m_pszVanishEffectName = pszName;
};


void CMagicParameter::SetReflectEffectName(const char* pszName)
{
    m_pszReflectEffectName = pszName;
};


void CMagicParameter::SetVanishMagicName(const char* pszName)
{
    m_pszVanishMagicName = pszName;
};