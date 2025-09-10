#include "BaseGunnerEnemyChr.hpp"
#include "BaseGrapplerEnemyChr.hpp"

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ void CBaseGunnerEnemyChr::CHideStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::IDLE);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
};


/*virtual*/ CBaseGunnerEnemyChr::CHideStatusObserver::RESULT
CBaseGunnerEnemyChr::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGunnerEnemyChr::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_APPEAR;
};


//
// *********************************************************************************
//


CBaseGunnerEnemyChr::CFireStatusObserver::CFireStatusObserver(const PARAM* pParam)
: m_param()
, m_fireNo(0)
, m_vecAt(Math::VECTOR3_ZERO)
, m_playerNo(-1)
, m_bContinuationFire(false)
{
    ASSERT(pParam != nullptr, "param required");
    m_param = *pParam;
};


/*virtual*/ void CBaseGunnerEnemyChr::CFireStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_fireNo    = 0;
    m_vecAt     = EnemyChr().AIThinkOrder().OrderAttack().m_vAt;
    m_playerNo  = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;

    ENEMYTYPES::STATUS currentStatus = Status();
    m_bContinuationFire = ((currentStatus >= ENEMYTYPES::STATUS_FIRE_AA) &&
                           (currentStatus <= ENEMYTYPES::STATUS_FIRE_CC));
    
    switch (currentStatus)
    {
    case ENEMYTYPES::STATUS_FIRE_A:
    case ENEMYTYPES::STATUS_FIRE_AA:
        MotionRegist(MOTIONID_FIRE_START, ENEMYTYPES::MOTIONNAMES::FIRE_A1);
        MotionRegist(MOTIONID_FIRE,       ENEMYTYPES::MOTIONNAMES::FIRE_A2);
        MotionRegist(MOTIONID_FIRE_END,   ENEMYTYPES::MOTIONNAMES::FIRE_A3);
        break;

    case ENEMYTYPES::STATUS_FIRE_B:
    case ENEMYTYPES::STATUS_FIRE_BB:
        MotionRegist(MOTIONID_FIRE_START, ENEMYTYPES::MOTIONNAMES::FIRE_B1);
        MotionRegist(MOTIONID_FIRE,       ENEMYTYPES::MOTIONNAMES::FIRE_B2);
        MotionRegist(MOTIONID_FIRE_END,   ENEMYTYPES::MOTIONNAMES::FIRE_B3);
        break;

    case ENEMYTYPES::STATUS_FIRE_C:
    case ENEMYTYPES::STATUS_FIRE_CC:
        MotionRegist(MOTIONID_FIRE_START, ENEMYTYPES::MOTIONNAMES::FIRE_C1);
        MotionRegist(MOTIONID_FIRE,       ENEMYTYPES::MOTIONNAMES::FIRE_C2);
        MotionRegist(MOTIONID_FIRE_END,   ENEMYTYPES::MOTIONNAMES::FIRE_C3);
        break;

    default:
        ASSERT(false);
        break;
    };

    MotionPlay(MOTIONID_FIRE_START, true);
};


/*virtual*/ CBaseGunnerEnemyChr::CFireStatusObserver::RESULT
CBaseGunnerEnemyChr::CFireStatusObserver::Observing(void) /*override*/
{
    if (m_param.bRepeatChase && (MotionCurrent() == MOTIONID_FIRE))
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_playerNo);
        if (pPlayerChr)
        {
            RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
            pPlayerChr->GetFootPosition(&vecFootPosPlayer);

            RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetFootPosition(&vecFootPosEnemy);

            float fDir = CEnemyUtils::GetDirection(&vecFootPosEnemy, &vecFootPosPlayer);
            float fRotRate = EnemyChr().CharacterParameter().m_feature.m_fRotateRate;

            EnemyChr().Compositor().RotateDirection(fDir, fRotRate);
        };
    };

    if (IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseGunnerEnemyChr::CFireStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ void CBaseGunnerEnemyChr::CFireStatusObserver::Update(void) /*override*/
{
    CMotionManagedStatusObserver::Update();

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        Fire();

        CGameSound::PlayAttackSE(&EnemyChr().Compositor());
    };
};


/*virtual*/ bool CBaseGunnerEnemyChr::CFireStatusObserver::OnMotionChange(int32 idOld, int32 idNew)
{
    if (idOld == MOTIONID_FIRE)
    {
        if (m_bContinuationFire)
        {
            if (++m_fireNo < m_param.repeatCount)
                idNew = MOTIONID_FIRE;
        };
    };

    MotionPlay(idNew, true);

    return false;
};


/*virtual*/ void CBaseGunnerEnemyChr::CFireStatusObserver::Fire(void)
{
    RwV3d vecBonePos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetOffsetPosition(&vecBonePos, m_param.boneId);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    Math::Vec3_Add(&vecPos, &vecPos, &vecBonePos);

    float fFireConvergence = static_cast<float>(EnemyChr().FrequencyParameter(CEnemyParameter::FREQUENCY_FIRE_CONVERGENCE));
    float fRange = 1.0f - (fFireConvergence * 0.01f);

    float fLow  = -fRange;
    float fHigh =  fRange;

    float fConvergence = (Math::RandFloatRange(fLow, fHigh) * MATH_DEG2RAD(1.8f));

    float fDir = (EnemyChr().Compositor().GetDirection() + fConvergence);

    float fYaw = fDir;
    float fPitch = 0.0f;
    RwV3d vecDir = Math::VECTOR3_AXIS_Z;
    
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_playerNo);
    if (pPlayerChr)
    {
        RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

        RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBodyPosition(&vecBodyPosEnemy);

        RwV3d vec = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vec, &vecBodyPosPlayer, &vecBodyPosEnemy);

        fDir = std::atan2(vec.x, vec.z);
        fYaw = fDir;

        RwMatrix matRotY;
        RwMatrixSetIdentityMacro(&matRotY);
        Math::Matrix_RotateY(&matRotY, -fDir);
        RwV3dTransformVector(&vec, &vec, &matRotY);

        fPitch = -std::atan2(vec.y, vec.z);
    };

    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    Math::Matrix_RotationYawPitchRoll(&matRot, fYaw, fPitch, 0.0f);
    RwV3dTransformVector(&vecDir, &vecDir, &matRot);

    switch (m_param.type)
    {
    case PARAM::TYPE_EFFECT:
        {
            uint32 hEffect = CEffectManager::Play(m_param.effectId, &vecPos, fYaw);
            RwMatrixTranslate(&matRot, &vecDir, rwCOMBINEREPLACE);
            CEffectManager::SetDirection(hEffect, &matRot);
        }
        break;

    case PARAM::TYPE_MAGIC:
        {
            CGameObject* pParent = &EnemyChr().Compositor();
            CShotManager::Shot(m_param.magicId, &vecPos, &vecDir, pParent, MATH_DEG2RAD(15.0f));
        }
        break;

    case PARAM::TYPE_SHOT:
        {
            CGameObject* pParent = &EnemyChr().Compositor();            
            CShotManager::Shot(m_param.shotId, &vecPos, &vecDir, pParent, MATH_DEG2RAD(15.0f), 5.0f);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


//
// *********************************************************************************
//


CBaseGunnerEnemyChr::CBaseGunnerEnemyChr(ENEMYID::VALUE idEnemy)
: CBaseGeneralEnemyChr(idEnemy)
{
    /* CBaseGeneralEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CBaseGeneralEnemyChr::CIdleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_TURN,                       new CBaseGeneralEnemyChr::CDummyTurnStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new CBaseGeneralEnemyChr::CGuardStatusObserver(3.0f));
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new CBaseGeneralEnemyChr::CThinkingStatusObserver);

    /* CBaseGrapplerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CBaseGrapplerEnemyChr::CWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                        new CBaseGrapplerEnemyChr::CRunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new CBaseGrapplerEnemyChr::CDeathStatusObserver);
    
    /* CBaseGunnerEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new CBaseGunnerEnemyChr::CHideStatusObserver);

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                     new CCommonEnemyObserver::CNoneAppearStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CCommonEnemyObserver::CKnockFrontStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CCommonEnemyObserver::CKnockBackStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CCommonEnemyObserver::CFlyawayStatus(true));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FRONT,            new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_BACK,             new CCommonEnemyObserver::CCrashWallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_FRONT,       new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_FALL_BACK,        new CCommonEnemyObserver::CCrashWallFallStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_FRONT,  new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK,   new CCommonEnemyObserver::CCrashWallTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_DINDLE,                     new CCommonEnemyObserver::CDindleStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_STUN,                       new CCommonEnemyObserver::CStunStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_SLEEP,                      new CCommonEnemyObserver::CSleepStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CFreezeStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_BIND,                       new CCommonEnemyObserver::CBindStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_COUNTERACT,                 new CCommonEnemyObserver::CCounteractStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_CAUGHT,                     new CCommonEnemyObserver::CCaughtStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED,                     new CCommonEnemyObserver::CLiftedStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_LIFTED_WALK,                new CCommonEnemyObserver::CLiftedWalkStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_FRONT,               new CCommonEnemyObserver::CThrownFrontStatusObserver(true, 1.0f));
    AttachStatusObserver(ENEMYTYPES::STATUS_THROWN_BACK,                new CCommonEnemyObserver::CThrownBackStatusObserver);
};


/*virtual*/ CBaseGunnerEnemyChr::~CBaseGunnerEnemyChr(void)
{
    ;
};