#include "MagicParameter.hpp"


CMagicParameter::CMagicParameter(void)
: m_pszBaseEffectName(nullptr)
, m_feature(MAGICTYPES::FEATURE_NONE)
, m_collisionBody({})
, m_collisionAttack({})
, m_movement({})
, m_locusinfo({})
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
    m_feature |= feature;
};


void CMagicParameter::SetCollisionBody(const RwV3d* pvPosition, float fRadius)
{
    //ASSERT(m_feature & MAGICTYPES::FEATURE_BODY, "setting data of unused feature");

    if (pvPosition)
        m_collisionBody.m_vPosition = *pvPosition;

    m_collisionBody.m_fRadius = fRadius;    
};


void CMagicParameter::SetCollisionBody(float fRadius)
{
    SetCollisionBody(nullptr, fRadius);
};


void CMagicParameter::SetCollisionAttack(const RwV3d* pvPosition,
                                         float fRadius,
                                         int32 nPower,
                                         int32 nAntiguard,
                                         int32 nStatus,
                                         float fTime /*= 0.0f*/,
                                         float fFlyX /*= 0.0f*/,
                                         float fFlyY /*= 0.0f*/)
{
    //ASSERT(m_feature & MAGICTYPES::FEATURE_ATTACK, "setting data of unused feature");

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


void CMagicParameter::SetCollisionAttack(float fRadius,
                                         int32 nPower,
                                         int32 nAntiguard,
                                         int32 nStatus,
                                         float fTime /*= 0.0f*/,
                                         float fFlyX /*= 0.0f*/,
                                         float fFlyY /*= 0.0f*/)
{
    SetCollisionAttack(nullptr, fRadius, nPower, nAntiguard, nStatus, fTime, fFlyX, fFlyY);
};


void CMagicParameter::SetConfusionFlag(bool bSet)
{
    m_collisionAttack.m_bConfusion = bSet;
};


void CMagicParameter::SetMovement(const RwV3d* pvVelocity,
                                  const RwV3d* pvAcceleration /*= nullptr*/)
{
    //ASSERT(m_feature & (MAGICTYPES::FEATURE_STRAIGHT   |
    //                    MAGICTYPES::FEATURE_ACCELERATE |
    //                    MAGICTYPES::FEATURE_HOMING), "setting data of unused feature");

    if (pvVelocity)
    {
        //ASSERT(m_feature & MAGICTYPES::FEATURE_STRAIGHT, "setting data of unused feature");
        m_movement.m_vVelocity = *pvVelocity;
    };

    if (pvAcceleration)
    {
        //ASSERT(m_feature & MAGICTYPES::FEATURE_ACCELERATE, "setting data of unused feature");
        m_movement.m_vAcceleration = *pvAcceleration;
    };
};


void CMagicParameter::SetLocusInfo(MAGICTYPES::LOCUSINFO& rLocusInfo)
{
    //ASSERT(m_feature & (MAGICTYPES::FEATURE_LOCUS_VER | MAGICTYPES::FEATURE_LOCUS_HOR), "setting data of unused feature");

    m_locusinfo = rLocusInfo;
};


void CMagicParameter::SetReflectNum(int32 nReflectNum)
{
    //ASSERT(m_feature & MAGICTYPES::FEATURE_COLLISION_REFLECTION, "setting data of unused feature");

    m_nReflectNumMax = nReflectNum;
};


void CMagicParameter::SetLive(float fLivetime)
{
    //ASSERT(m_feature & MAGICTYPES::FEATURE_LIVETIME, "setting data of unused feature");

    m_fLivetime = fLivetime;
    m_fHitTimingStart = 0.0f;
    m_fHitTimingEnd = m_fLivetime;
};


void CMagicParameter::SetChangeSize(float fChangeSize)
{
    //ASSERT(m_feature & MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE, "setting data of unused feature");
    
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