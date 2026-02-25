#include "BaseFlyingEnemyChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyTracer.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/BaseGeneralEnemyAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Module/CharacterEffectModule.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ const float CBaseFlyingEnemyChr::BASE_ALTITUDE = 20.0f;


CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::CBaseFlyingStatusObserver(void)
: CBaseFlyingStatusObserver(BASE_ALTITUDE)
{
    ;
};


CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::CBaseFlyingStatusObserver(float fAltitude)
: m_fAltitude(fAltitude)
{
    ;
};


/*virtual*/ void CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
};


/*virtual*/ float CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::GetAltitude(void) const
{
    return static_cast<const CBaseFlyingEnemyChr&>(EnemyChr()).GetAltitude();
};


/*virtual*/ float CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::GetAttackAltitude(void) const
{
    return static_cast<const CBaseFlyingEnemyChr&>(EnemyChr()).GetAttackAltitude();
};


/*virtual*/ void CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::DoStoppingVelocity(void)
{
    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVelocity);

    float fMoveSpeed = Math::Vec3_Length(&vecVelocity) * 0.8f;
    if (fMoveSpeed < 0.01f)
        fMoveSpeed = 0.0f;

    vecVelocity = { 0.0f, 0.0f, fMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


/*virtual*/ float CBaseFlyingEnemyChr::CBaseFlyingStatusObserver::CalcAltitude(float fHeightAt,
                                                                               float fHeight,
                                                                               float fSpeed) const
{
    float dt = CGameProperty::GetElapsedTime();
    float fSpeedPerFrame = (dt * fSpeed);

    if ((fHeightAt <= CWorldMap::INVALID_MAP_HEIGHT) ||
        (CWorldMap::GetCharacterDeathHeight() >= fHeightAt))
        return 0.0f;

    float fAltitude = (fHeight - fHeightAt);
    float fAltitudeAbs = std::fabs(fAltitude);

    if (fAltitudeAbs >= m_fAltitude)
        return 0.0f;

    if ((fHeight + fSpeedPerFrame) < fHeightAt)
        return fSpeed;

    if ((fHeight - fSpeedPerFrame) <= fHeightAt)
        return 0.0f;

    if (fAltitude >= 4.0f)
        return -fSpeed;

    return -(std::sin(fAltitude * 0.25f) * fSpeed);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFlyingEnemyChr::CAppearStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseFlyingEnemyChr::CAppearStatusObserver::RESULT
CBaseFlyingEnemyChr::CAppearStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFlyingEnemyChr::CAppearStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecPos);

    float fPatrolRadius = EnemyChr().CharacterParameter().m_feature.m_fPatrolRadius;
    EnemyChr().SetPatrolArea(&vecPos, fPatrolRadius);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseFlyingEnemyChr::CAppearFlyStatusObserver::CAppearFlyStatusObserver(float fAppearOffsetY)
: m_fTimer(0.0f)
, m_fMoveSpeed(0.0f)
, m_fRotRate(0.0f)
, m_fDistanceOfSuitable(0.0f)
, m_fAppearOffsetY(fAppearOffsetY)
, m_vecStartPos(Math::VECTOR3_ZERO)
{
    ;
};


/*virtual*/ void CBaseFlyingEnemyChr::CAppearFlyStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingStatusObserver::OnStart();

    m_fTimer              = 0.0f;
    m_fMoveSpeed          = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;
    m_fRotRate            = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
    m_fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;

    EnemyChr().Compositor().GetPosition(&m_vecStartPos);

    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    EnemyChr().Compositor().SetEnableCatchHit(false);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);
    vecPos.y += m_fAppearOffsetY;
    EnemyChr().Compositor().SetPosition(&vecPos);
};


/*virtual*/ CBaseFlyingEnemyChr::CAppearFlyStatusObserver::RESULT
CBaseFlyingEnemyChr::CAppearFlyStatusObserver::Observing(void) /*override*/
{
    RwV3d vecStartPos = m_vecStartPos;
    vecStartPos.y = 0.0f;

    RwV3d vecNowPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecNowPos);
    vecNowPos.y = 0.0f;

	float fDir = EnemyChr().Compositor().GetDirection();

    if (!Math::Vec3_FEqual(&vecNowPos, &vecStartPos))
        fDir = CEnemyUtils::GetDirection(&vecNowPos, &vecStartPos);

    EnemyChr().Compositor().RotateDirection(fDir, m_fRotRate);

    RwV3d vecVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    float dt = CGameProperty::GetElapsedTime();
    float fSpeed = (dt * m_fMoveSpeed);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    float fNoise = m_vecStartPos.y + AltitudeNoise();

    if ((vecPos.y + fSpeed) >= fNoise)
    {
        if ((vecPos.y - fSpeed) > fNoise)
            vecVelocity.y = -m_fMoveSpeed;
    }
    else
    {
        vecVelocity.y = m_fMoveSpeed;
    };

    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    if (Math::FEqual(vecVelocity.y, 0.0f))
        return RESULT_END;

    m_fTimer += dt;
    
    if (m_fTimer > 20.0f)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFlyingEnemyChr::CAppearFlyStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecPos);

    float fPatrolRadius = EnemyChr().CharacterParameter().m_feature.m_fPatrolRadius;
    EnemyChr().SetPatrolArea(&vecPos, fPatrolRadius);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ float CBaseFlyingEnemyChr::CAppearFlyStatusObserver::AltitudeNoise(void)
{
    return 0.0f;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFlyingEnemyChr::CIdleStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseFlyingEnemyChr::CIdleStatusObserver::RESULT
CBaseFlyingEnemyChr::CIdleStatusObserver::Observing(void) /*override*/
{
    DoStoppingVelocity();

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order == CAIThinkOrder::ORDER_WAIT)
        return RESULT_CONTINUE;
    
    if (order == CAIThinkOrder::ORDER_NOTHING)
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFlyingEnemyChr::CIdleStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFlyingEnemyChr::CMoveStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_fMoveSpeed          = EnemyChr().CharacterParameter().m_feature.m_fWalkMoveSpeed;
    m_fRotRate            = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;
    m_fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
    m_order               = EnemyChr().AIThinkOrder().GetOrder();
};


/*virtual*/ CBaseFlyingEnemyChr::CMoveStatusObserver::RESULT
CBaseFlyingEnemyChr::CMoveStatusObserver::Observing(void) /*override*/
{
    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    if (order != m_order)
        return RESULT_END;

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    RwV3d vecAt = Math::VECTOR3_ZERO;
    CBaseGeneralEnemyChr::GetMoveAt(EnemyChr().AIThinkOrder(), &vecAt);

    Direction(&vecPos, &vecAt);

    int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);
    float fAltitude = 0.0f;

    if (moveTarget)
    {
        fAltitude = GetAttackAltitude();
        fAltitude += vecAt.y;
    }
    else
    {
        fAltitude = GetAltitude();
    };

    m_fAltitude = fAltitude;
    vecAt.y = m_fAltitude;

    Velocity(&vecPos, &vecAt);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFlyingEnemyChr::CMoveStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ float CBaseFlyingEnemyChr::CMoveStatusObserver::AltitudeNoise(void)
{
    return 0.0f;
};


void CBaseFlyingEnemyChr::CMoveStatusObserver::Direction(const RwV3d* vecPos, const RwV3d* vecAt)
{
    RwV3d vecPosXZ = *vecPos;
    vecPosXZ.y = 0.0f;

    RwV3d vecAtXZ = *vecAt;
    vecAtXZ.y = 0.0f;

    float fDir = EnemyChr().Compositor().GetDirection();
    if (!Math::Vec3_Equal(&vecPosXZ, &vecAtXZ))
        fDir = CEnemyUtils::GetDirection(vecPos, vecAt);

    EnemyChr().Compositor().RotateDirection(fDir, m_fRotRate);

    m_fDirection = EnemyChr().Compositor().GetDirection();
};


void CBaseFlyingEnemyChr::CMoveStatusObserver::Velocity(const RwV3d* vecPos, const RwV3d* vecAt)
{
    RwMatrix matRot;
    RwMatrixSetIdentity(&matRot);
    Math::Matrix_RotationYawPitchRoll(&matRot, m_fDirection, 0.0f, 0.0f);

    RwV3d vecVelocity = { 0.0f, 0.0f, m_fMoveSpeed };
    RwV3dTransformVector(&vecVelocity, &vecVelocity, &matRot);

    RwV3d vecHeightPos = *vecPos;
    RwV3d vecHeightAt = *vecAt;

    int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);
    
    if (moveTarget)
    {
        int32 targetNo = EnemyChr().AIThinkOrder().OrderMove().m_iPlayerNo;
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(targetNo);

        if (pPlayerChr)
        {
            PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
            CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);
            if (playerStateFlag & CAIUtils::PLAYER_STATE_AERIAL)
                vecHeightAt.y = vecHeightPos.y;
        };
    }
    else
    {
        float fPatrolHeight = EnemyChr().CharacterParameter().m_feature.m_vPatrolOrigin.y;
        
        if (vecHeightAt.y >= fPatrolHeight)
            fPatrolHeight = vecHeightAt.y;
        
        vecHeightAt.y = fPatrolHeight;
    };

    float fRadius = EnemyChr().Compositor().GetCollisionParameter().m_fRadius;

    RwV3d vecStep = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vecStep, &vecVelocity);
    Math::Vec3_Scale(&vecStep, &vecStep, (fRadius * 2.0f));

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);
    Math::Vec3_Add(&vecMyPos, &vecMyPos, &vecStep);

    if (CWorldMap::CheckCollisionLine(&vecHeightPos, &vecHeightAt))
        vecMyPos.y += 10.0f;

    float fHeight = CWorldMap::GetMapHeight(&vecMyPos, fRadius);
    fHeight += GetAttackAltitude();

    if (vecHeightAt.y >= fHeight)
        fHeight = vecHeightAt.y;

    vecHeightAt.y = fHeight;

    vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    vecVelocity.y = CalcAltitude(vecHeightAt.y, vecMyPos.y, m_fMoveSpeed);
    vecVelocity.y += AltitudeNoise();

    EnemyChr().Compositor().SetVelocity(&vecVelocity);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFlyingEnemyChr::CAttackAStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::ATTACK_A, true);
    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
};


/*virtual*/ CBaseFlyingEnemyChr::CAttackAStatusObserver::RESULT
CBaseFlyingEnemyChr::CAttackAStatusObserver::Observing(void) /*override*/
{
    DoStoppingVelocity();

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFlyingEnemyChr::CAttackAStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


CBaseFlyingEnemyChr::CDeathStatusObserver::CDeathStatusObserver(EFFECT_GENERIC::DEATHTYPE deathType,
                                                                float fEffectScale)
: m_fTimer(0.0f)
, m_fEffectScale(fEffectScale)
, m_deathType(deathType)
, m_hEffect(0)
{
    ;
};


/*virtual*/ void CBaseFlyingEnemyChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingStatusObserver::OnStart();

    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::KNOCK, bForce);

    RwV3d vecAttackDir = EnemyChr().Compositor().AttackParameter().m_vDirection;
    EnemyChr().Compositor().SetDirectionFromVector(&vecAttackDir);

    RwV3d vecAttackVel = EnemyChr().Compositor().AttackParameter().m_vVelocity;
    EnemyChr().Compositor().SetVelocity(&vecAttackVel);

    float endT = EnemyChr().Compositor().GetMotionEndTime();
    RwV3d vecAccel = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecAccel, &vecAttackVel, -(1.0f / endT));

    EnemyChr().Compositor().SetAcceleration(&vecAccel);

    m_fTimer = 0.0f;

    if (m_deathType == EFFECT_GENERIC::DEATHTYPE_MONSTER)
    {
        RwV3d vecOffset = Math::VECTOR3_ZERO; // unused (default)
        float fDir = EnemyChr().Compositor().GetDirection();

        uint32 hEffect = CEffectManager::PlayTrace(EFFECTID::ID_BLACKSMOKE, new CEnemyTracer(&EnemyChr()), &vecOffset, fDir);
        ASSERT(hEffect);

        if (hEffect)
            CEffectManager::SetScale(hEffect, (m_fEffectScale * 2.0f));

        m_hEffect = hEffect;
    };
};


/*virtual*/ CBaseFlyingEnemyChr::CDeathStatusObserver::RESULT
CBaseFlyingEnemyChr::CDeathStatusObserver::Observing(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    if (EnemyChr().Compositor().IsMotionEnd())
        EnemyChr().Compositor().StopMotion();

    const CCharacterCompositor::COLLISIONGROUNDINFO* pGroundInfo = EnemyChr().Compositor().GetCollisionGround();
    ASSERT(pGroundInfo != nullptr);

    bool bGroundHit = pGroundInfo->m_bHit;

    if (bGroundHit)
        return RESULT_END;

    m_fTimer += CGameProperty::GetElapsedTime();
    
    if (m_fTimer > 1.0f)
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFlyingEnemyChr::CDeathStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableCatchHit(true);

    if (m_hEffect)
    {
        if (!CEffectManager::IsEnd(m_hEffect))
        {
            CEffectManager::End(m_hEffect);
            m_hEffect = 0;
        };
    };

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);
    EFFECT_GENERIC::CallDeathEffect(m_deathType, &vecPos, m_fEffectScale);

    return ENEMYTYPES::STATUS_QUIT;
};

//
// *********************************************************************************
//


/*virtual*/ void CBaseFlyingEnemyChr::CBindStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_bBodyHitFlag = EnemyChr().Compositor().IsEnableBodyHitSelfToOther();
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    m_fTargetTime = EnemyChr().Compositor().AttackParameter().m_fTroubleTime;
    m_fTime = 0.0f;

    ENEMYTYPES::STATUS currentStatus = Status();
    ASSERT(currentStatus == ENEMYTYPES::STATUS_BIND);

    bool bReset = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DINDLE, bReset);

    IModule* pModule = CharacterEffectModule::CreateBindEffectModule(&EnemyChr().Compositor());
    EnemyChr().Compositor().IncludeModule(pModule);
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFlyingEnemyChr::CStunStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_bBodyHitFlag = EnemyChr().Compositor().IsEnableBodyHitSelfToOther();
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    m_fTargetTime = EnemyChr().Compositor().AttackParameter().m_fTroubleTime;
    m_fTime = 0.0f;

    ENEMYTYPES::STATUS currentStatus = Status();
    ASSERT(currentStatus == ENEMYTYPES::STATUS_STUN);

    bool bReset = true;
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::DINDLE, bReset);

    IModule* pModule = CharacterEffectModule::CreateBindEffectModule(&EnemyChr().Compositor());
    EnemyChr().Compositor().IncludeModule(pModule);
};


//
// *********************************************************************************
//


CBaseFlyingEnemyChr::CBaseFlyingEnemyChr(ENEMYID::VALUE enemyId)
: CBaseGeneralEnemyChr(enemyId)
, m_fAttackAltitude(0.0f)
{
    Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,       new CBaseGeneralEnemyChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,   new CBaseGeneralEnemyChr::CThinkingStatusObserver);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK,      new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,     new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_BIND,       new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* CBaseFlyingEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,     new CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,       new CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new CMoveStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,      new CDeathStatusObserver(EFFECT_GENERIC::DEATHTYPE_MONSTER, 0.4f));
};


/*virtual*/ CBaseFlyingEnemyChr::~CBaseFlyingEnemyChr(void)
{
    ;
};


/*virtual*/ bool CBaseFlyingEnemyChr::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    return false;
};


/*virtual*/ void CBaseFlyingEnemyChr::OnMessageDoChangingAerial(void) /*override*/
{
    ;
};


/*virtual*/ float CBaseFlyingEnemyChr::GetAltitude(void) const
{
    return CharacterParameter().m_feature.m_vPatrolOrigin.y;
};


/*virtual*/ float CBaseFlyingEnemyChr::GetAttackAltitude(void) const
{
    return m_fAttackAltitude;
};


/*virtual*/ void CBaseFlyingEnemyChr::SetAttackAltitude(float fAltitude)
{
    m_fAttackAltitude = fAltitude;

    RwV3d vecPos = Math::VECTOR3_ZERO;
    Compositor().GetPosition(&vecPos);
    vecPos.y += m_fAttackAltitude;

    float fMapHeight = CWorldMap::GetMapHeight(&vecPos);
    if (fMapHeight > CWorldMap::INVALID_MAP_HEIGHT)
    {
        Compositor().GetFootPosition(&vecPos);

        if ((fMapHeight + m_fAttackAltitude) > vecPos.y)
        {
            float fRand = Math::RandFloat();            
            vecPos.y += (fRand + m_fAttackAltitude + 3.0f);

            Compositor().SetPosition(&vecPos);

            float fPatrolRadius = CharacterParameter().m_feature.m_fPatrolRadius;
            SetPatrolArea(&vecPos, fPatrolRadius);
        };
    };
};