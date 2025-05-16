#include "BaseKaraiChr.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/084KaraiAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CBaseKaraiChr::CAttackDStatusObserver::CAttackDStatusObserver(void)
: CAttackDStatusObserver(BASEKARAI::MOTIONNAMES::ATTACK_D1)
{
    ;
};


CBaseKaraiChr::CAttackDStatusObserver::CAttackDStatusObserver(const char* pszMotion)
: m_step(STEP_DASH_BEGIN)
, m_vecDashPos(Math::VECTOR3_ZERO)
, m_attackPlayerNo(0)
, m_fTimer(0.0f)
, m_fDashTime(0.0f)
, m_bDashEnd(false)
, m_pszMotion(pszMotion)
, m_bEffectActive(false)
, m_hEffectWSplash(0)
{
    ASSERT(m_pszMotion != nullptr);
};


/*virtual*/ void CBaseKaraiChr::CAttackDStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(m_pszMotion, bForce);

    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_WAITING);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    m_step = STEP_DASH_BEGIN;
    m_bDashEnd = false;
    m_attackPlayerNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;

    InitWaterSplashEffect();
};


/*virtual*/ CBaseKaraiChr::CAttackDStatusObserver::RESULT
CBaseKaraiChr::CAttackDStatusObserver::Observing(void) /*override*/
{
    ProcWaterSplashEffect();

    switch (m_step)
    {
    case STEP_DASH_BEGIN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                SetDashStart();
                m_step = STEP_DASH;
            };
        }
        break;

    case STEP_DASH:
        {
            bool bDashEnd = false;

            RwV3d vecFootPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetFootPosition(&vecFootPos);

            if (CEnemyUtils6045::GetXZDistance(&vecFootPos, &m_vecDashPos) < 0.3f)
                bDashEnd = true;

            if (m_fTimer < m_fDashTime)
                m_fTimer += CGameProperty::GetElapsedTime();
            else
                bDashEnd = true;

            const CCharacterCompositor::COLLISIONWALLINFO* pWallInfo = EnemyChr().Compositor().GetCollisionWall();
            ASSERT(pWallInfo != nullptr);

            if ((pWallInfo->m_bHit && (std::sin(MATH_DEG2RAD(45.0f)) > pWallInfo->m_vNormal.y)) || bDashEnd)
            {
                SetDashEnd();
                m_bDashEnd = true;
                m_step = STEP_DASH_END;
            };
        }
        break;

    case STEP_DASH_END:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;            
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CAttackDStatusObserver::OnEnd(void) /*override*/
{
    if (m_hEffectWSplash)
    {
        CEffectManager::Finish(m_hEffectWSplash);
        m_hEffectWSplash = 0;
    };

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ void CBaseKaraiChr::CAttackDStatusObserver::SetDashStart(void)
{
    SetDashParameter(m_attackPlayerNo, 17.0f, 4.0f);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(BASEKARAI::MOTIONNAMES::ATTACK_D2, bForce);
    EnemyChr().Compositor().StopMotion();
    
    CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_ALL_DASH1,
                                      &EnemyChr().Compositor(),
                                      1.0f,
                                      CEnemyUtils6045::TRACE_FOOT,
                                      true);

    RwV3d vecOffset = { 0.0f, 0.0f, -0.75f };
    CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_ALL_DASH2,
                                      &EnemyChr().Compositor(),
                                      1.0f,
                                      CEnemyUtils6045::TRACE_FOOT,
                                      true,
                                      &vecOffset);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4122));
};


/*virtual*/ void CBaseKaraiChr::CAttackDStatusObserver::SetDashEnd(void)
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().PlayMotion();
};


void CBaseKaraiChr::CAttackDStatusObserver::InitWaterSplashEffect(void)
{
    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);

    m_hEffectWSplash = CEffectManager::Play(EFFECTID::ID_ALL_W_SPLASH, &vecPos);
    ASSERT(m_hEffectWSplash);
    
    m_bEffectActive = false;

    if (m_hEffectWSplash)
    {
        float fShadowRadius = EnemyChr().CharacterParameter().m_fShadowRadius;
        CEffectManager::SetScale(m_hEffectWSplash, (fShadowRadius / 1.5f));
        CEffectManager::SetActive(m_hEffectWSplash, m_bEffectActive);
    };
};


void CBaseKaraiChr::CAttackDStatusObserver::ProcWaterSplashEffect(void)
{
    bool bEffectActive = false;

    if (CEnemyUtils::CheckInfoWater(&EnemyChr().Compositor()) && (m_step == STEP_DASH))
        bEffectActive = true;

    if (m_bEffectActive != bEffectActive)
    {
        m_bEffectActive = bEffectActive;
        CEffectManager::SetActive(m_hEffectWSplash, m_bEffectActive);
    };

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    CWorldMap::GetWaterSurfacePosition(&vecFootPos);

    CEffectManager::SetPosition(m_hEffectWSplash, &vecFootPos);
};


void CBaseKaraiChr::CAttackDStatusObserver::SetDashParameter(int32 attackPlayerNo,
                                                             float fDashSpeed,
                                                             float fDashDistance)
{
    CBaseKaraiChr::GetDashMarkPosition(&m_vecDashPos,
                                       &EnemyChr().Compositor(),
                                       attackPlayerNo,
                                       fDashDistance);

    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    float fDir = 0.0f;
    if (CEnemyUtils6045::GetDirection(&fDir, &vecFootPos, &m_vecDashPos))
        EnemyChr().Compositor().SetDirection(fDir);

    CEnemyUtils6045::ReplaceVelocityToNowDirection(&EnemyChr().Compositor(), fDashSpeed);
    float fDistance = CEnemyUtils6045::GetXZDistance(&vecFootPos, &m_vecDashPos);

    m_fTimer = 0.0f;
    m_fDashTime = (fDistance / fDashSpeed);
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAttackSpecialStatusObserver::CAttackSpecialStatusObserver(CBaseKaraiChr* pBaseKaraiChr)
: CAttackDStatusObserver(BASEKARAI::MOTIONNAMES::ATTACK_SPECIAL1)
, m_pBaseKaraiChr(pBaseKaraiChr)
, m_fChargeTime(BASEKARAI::ATTACK_SPECIAL_CHARGETIME[CGameProperty::GetDifficulty()])
, m_fChargeStepTimer(0.0f)
, m_stepCharge(STEPCHARGE_ONE)
, m_hEffectCharge(0)
, m_ahEffectElectric()
{
    for (int32 i = 0; i < COUNT_OF(m_ahEffectElectric); ++i)
        m_ahEffectElectric[i] = 0;
};


/*virtual*/ void CBaseKaraiChr::CAttackSpecialStatusObserver::OnStart(void) /*override*/
{
    if (m_pBaseKaraiChr->IsKnockBackCtrlActive())
        m_pBaseKaraiChr->SetKnockBackCtrlEnable(false);

    CAttackDStatusObserver::OnStart();

    float endT = EnemyChr().Compositor().GetMotionEndTime();
    float fMotionSpeed = (endT / m_fChargeTime);

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetMotionSpeed(fMotionSpeed);

    InitChargeEffect();
    InitElectricEffect();

    CMessageManager::Request(SEGROUPID::VALUE(134));
};


/*virtual*/ CBaseKaraiChr::CAttackSpecialStatusObserver::RESULT CBaseKaraiChr::CAttackSpecialStatusObserver::Observing(void) /*override*/
{
    if (m_step == STEP_DASH_BEGIN)
    {
        CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(),
                                    m_attackPlayerNo,
                                    MATH_DEG2RAD(45.0f),
                                    MATH_DEG2RAD(3.0f));
        ProcChargeEffect();
    };

    return CAttackDStatusObserver::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CAttackSpecialStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

    for (int32 i = 0; i < COUNT_OF(m_ahEffectElectric); ++i)
    {
        if (m_ahEffectElectric[i])
        {
            CEffectManager::Finish(m_ahEffectElectric[i]);
            m_ahEffectElectric[i] = 0;
        };
    };

    if (m_hEffectCharge)
    {
        CEffectManager::Finish(m_hEffectCharge);
        m_hEffectCharge = 0;
    };

    if (m_pBaseKaraiChr->IsKnockBackCtrlActive())
        m_pBaseKaraiChr->SetKnockBackCtrlEnable(true);

    if (m_bDashEnd)
        m_pBaseKaraiChr->AIModerator().DecisionUnitCommonParameter().SetAttackWaitTime(1.5f);

    return CAttackDStatusObserver::OnEnd();
};


/*virtual*/ void CBaseKaraiChr::CAttackSpecialStatusObserver::SetDashStart(void) /*override*/
{
    float fDashDistance = 0.0f;
    float fDashSpeed = 24.0f;

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_attackPlayerNo);
    if (pPlayerChr)
    {
        RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecFootPosEnemy);

        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        float fDist = CEnemyUtils6045::GetXZDistance(&vecFootPosEnemy, &vecFootPosPlayer);
        if (fDist >= 1.0f)
            fDashDistance = fDist + 4.0f;
        else
            fDashDistance = 5.0f;
    }
    else
    {
        fDashDistance = 5.0f;
    };

    SetDashParameter(m_attackPlayerNo, fDashSpeed, fDashDistance);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(BASEKARAI::MOTIONNAMES::ATTACK_SPECIAL2, bForce);
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);

    if (m_hEffectCharge)
    {
        CEffectManager::Finish(m_hEffectCharge);
        m_hEffectCharge = 0;
    };

    CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_ALL_DASH1, &EnemyChr().Compositor(), 1.75f, CEnemyUtils6045::TRACE_FOOT, true);
    CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_ALL_DASH2, &EnemyChr().Compositor(), 2.0f, CEnemyUtils6045::TRACE_FOOT, true);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4411));
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8387));

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ void CBaseKaraiChr::CAttackSpecialStatusObserver::SetDashEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    for (int32 i = 0; i < COUNT_OF(m_ahEffectElectric); ++i)
    {
        if (m_ahEffectElectric[i])
        {
            CEffectManager::Finish(m_ahEffectElectric[i]);
            m_ahEffectElectric[i] = 0;
        };
    };

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    bool bForce = true;
    EnemyChr().Compositor().ChangeMotion(BASEKARAI::MOTIONNAMES::ATTACK_SPECIAL3, bForce);
};


void CBaseKaraiChr::CAttackSpecialStatusObserver::InitChargeEffect(void)
{
    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

    m_hEffectCharge = CEnemyUtils6045::EntrySimpleEffect(EFFECTID::ID_ALL_TAME1, &vecBodyPos, 1.25f);
    m_stepCharge = STEPCHARGE_ONE;
    m_fChargeStepTimer = 0.0f;

    CEnemyUtils6045::EntrySimpleEffect(EFFECTID::ID_ALL_TAME_FLASH, &vecBodyPos, 1.25f);
};


void CBaseKaraiChr::CAttackSpecialStatusObserver::ProcChargeEffect(void)
{
    float fKeyTime = -1.0f;
    EFFECTID::VALUE effectId = EFFECTID::ID_UNKNOWN;

    switch (m_stepCharge)
    {
    case STEPCHARGE_ONE:
        fKeyTime = m_fChargeTime * ((1.0f / static_cast<float>(STEPCHARGENUM)) * 1.0f);
        effectId = EFFECTID::ID_ALL_TAME2;
        break;

    case STEPCHARGE_TWO:
        fKeyTime = m_fChargeTime * ((1.0f / static_cast<float>(STEPCHARGENUM)) * 2.0f);
        effectId = EFFECTID::ID_ALL_TAME3;
        break;

    case STEPCHARGE_THREE:
        break;

    default:
        break;
    };

    if ((fKeyTime <= m_fChargeStepTimer) && (fKeyTime > 0.0f))
    {
        RwV3d vecBodyPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

        if (m_hEffectCharge)
        {
            CEffectManager::Finish(m_hEffectCharge);
            m_hEffectCharge = 0;
        };

        m_hEffectCharge = CEnemyUtils6045::EntrySimpleEffect(effectId, &vecBodyPos, 1.25f);
        CEnemyUtils6045::EntrySimpleEffect(EFFECTID::ID_ALL_TAME_FLASH, &vecBodyPos, 1.25f);

        ++m_stepCharge;
    };

    if (m_fChargeStepTimer < m_fChargeTime)
        m_fChargeStepTimer += CGameProperty::GetElapsedTime();
};


void CBaseKaraiChr::CAttackSpecialStatusObserver::InitElectricEffect(void)
{
    float afScale[] = { 1.5f, 2.25f };

    static_assert(COUNT_OF(afScale) == COUNT_OF(m_ahEffectElectric), "table incorrect");

    for (int32 i = 0; i < COUNT_OF(m_ahEffectElectric); ++i)
    {
        m_ahEffectElectric[i] = CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_SHRD_ELEC,
                                                                  &EnemyChr().Compositor(),
                                                                  afScale[i],
                                                                  CEnemyUtils6045::TRACE_BODY,
                                                                  false);
    };
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAttackEStatusObserver::CAttackEStatusObserver(void)
{
    SetParameter(BASEKARAI::MOTIONNAMES::ATTACK_E1,
                 BASEKARAI::MOTIONNAMES::ATTACK_E2,
                 BASEKARAI::MOTIONNAMES::ATTACK_E3,
                 6.0f);
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAttackFStatusObserver::CAttackFStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttack(BASEKARAI::MOTIONNAMES::ATTACK_F)
{
    ;
};


/*virtual*/ void CBaseKaraiChr::CAttackFStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttack::OnStart();

    int32 playerTargetNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(playerTargetNo);
    if (pPlayerChr)
    {
        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        float fDir = 0.0f;
        if (CEnemyUtils6045::GetDirection(&fDir, &EnemyChr().Compositor(), &vecFootPosPlayer))
            EnemyChr().Compositor().SetDirection(fDir);
    };
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseKaraiChr::CAttackCounterStatusObserver::OnStart(void) /*override*/
{
    CAttackFStatusObserver::OnStart();
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CAttackCounterStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    return CAttackFStatusObserver::OnEnd();
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAttackCStatusObserver::CAttackCStatusObserver(void)
: m_attackPlayerNo(0)
, m_shotCount(0)
, m_shotContinualAimNo(0)
, m_fShotDirBase(0.0f)
, m_fTimer(0.0f)
, m_bShotEnableFlag(false)
, m_bShotContinualFlag(false)
, m_shotNum(0)
, m_fShotRadSize(0.0f)
, m_fShotContinualTime(0.0f)
, m_fShotContinualInterval(0.0f)
{
    SetParameter(BASEKARAI::MOTIONNAMES::ATTACK_C1,
                 BASEKARAI::MOTIONNAMES::ATTACK_C2,
                 BASEKARAI::MOTIONNAMES::ATTACK_C3,
                 9.0f);
};


/*virtual*/ void CBaseKaraiChr::CAttackCStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::CJumpActionStatusObserver::OnStart();

    m_attackPlayerNo = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_shotCount = 0;
    m_bShotEnableFlag = false;

    if (Math::RandFloat() >= 0.5f)
        SetShotSametimeSetting(12, MATH_PI2);
    else
        SetShotContinualSetting(12, MATH_PI2, 0.25f, (Math::Rand() % 12));

    //SetShotSametimeSetting(12, MATH_PI2);
    //SetShotContinualSetting(12, MATH_PI2, 0.25f, (Math::Rand() % 12));
};


/*virtual*/ CBaseKaraiChr::CAttackCStatusObserver::RESULT
CBaseKaraiChr::CAttackCStatusObserver::Observing(void) /*override*/
{
    if (m_step == STEP_JUMP)
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        fDir += (CGameProperty::GetElapsedTime() * (MATH_PI * 5.0f));
        EnemyChr().Compositor().SetDirection(fDir);

        if (m_bShotEnableFlag)
        {
            if (m_bShotContinualFlag)
                ShotKunaiContinual();
            else
                ShotKunaiSameTime();
        }
        else if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
        {
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8393));
            m_bShotEnableFlag = true;
        };
    };

    return CBaseChr6045::CJumpActionStatusObserver::Observing();
};


/*virtual*/ void CBaseKaraiChr::CAttackCStatusObserver::SetJumpSetting(void) /*override*/
{
    CBaseChr6045::CJumpActionStatusObserver::SetJumpSetting();
    EnemyChr().Compositor().StopMotion();
};


void CBaseKaraiChr::CAttackCStatusObserver::SetShotContinualSetting(int32 iShotNum,
                                                                    float fAreaRadius,
                                                                    float fShotTime,
                                                                    int32 iAimNo)
{
    m_bShotContinualFlag     = true;
    m_shotNum                = iShotNum;
    m_fShotRadSize           = (fAreaRadius / static_cast<float>(iShotNum));
    m_fShotContinualInterval = (fShotTime / static_cast<float>(iShotNum));
    m_fShotContinualTime     = fShotTime;
    m_shotContinualAimNo     = iAimNo;
};


void CBaseKaraiChr::CAttackCStatusObserver::SetShotSametimeSetting(int32 iShotNum, float fAreaRadius)
{
    m_bShotContinualFlag = false;
    m_shotNum            = iShotNum;
    m_fShotRadSize       = (fAreaRadius / static_cast<float>(iShotNum));
};


void CBaseKaraiChr::CAttackCStatusObserver::ShotKunaiContinual(void)
{
    if (m_shotCount >= m_shotNum)
        return;

    if (!m_shotCount)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_attackPlayerNo);
        if (pPlayerChr)
        {
            RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetBodyPosition(&vecBodyPosEnemy);
                
            RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
            pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

            if (!CEnemyUtils6045::GetDirection(&m_fShotDirBase, &vecBodyPosEnemy, &vecBodyPosPlayer))
                m_fShotDirBase = EnemyChr().Compositor().GetDirection();
        }
        else
        {
            m_fShotDirBase = EnemyChr().Compositor().GetDirection();
        };

        m_fTimer = 0.0f;
    };

    for (int32 i = 0; i < m_shotNum; ++i)
    {
        float fShotTime = (static_cast<float>(m_shotCount) * m_fShotContinualInterval);
        if (fShotTime > m_fTimer)
            break;

        RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBodyPosition(&vecBodyPosEnemy);

        float fYaw = (static_cast<float>(m_shotCount) * m_fShotRadSize) -
                     (static_cast<float>(m_shotContinualAimNo) * m_fShotRadSize);
        fYaw += m_fShotDirBase;
        fYaw = CEnemyUtils::RadianCorrect(fYaw);

        float fPitch = 0.0f;
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_attackPlayerNo);
        if (pPlayerChr)
        {
            RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
            pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

            RwV3d vecBuf = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecBuf, &vecBodyPosPlayer, &vecBodyPosEnemy);
            Math::Vec3_Normalize(&vecBuf, &vecBuf);

            fPitch = std::asin(-vecBuf.y);
        };

        RwMatrix matRot;
        RwMatrixSetIdentityMacro(&matRot);
        Math::Matrix_RotationYawPitchRoll(&matRot, fYaw, fPitch, 0.0f);

        RwV3d vecDir = Math::VECTOR3_AXIS_Z;
        RwV3dTransformVector(&vecDir, &vecDir, &matRot);

        CEnemyUtils6045::EntryShotWeapon(SHOTID::ID_KUNAI_ENEMY,
                                         &EnemyChr().Compositor(),
                                         &vecBodyPosEnemy,
                                         &vecDir,
                                         !(m_shotCount % 3));

        ++m_shotCount;
    };

    m_fTimer += CGameProperty::GetElapsedTime();
};


void CBaseKaraiChr::CAttackCStatusObserver::ShotKunaiSameTime(void)
{
    if (m_shotCount >= m_shotNum)
        return;

    RwV3d vecDir = Math::VECTOR3_ZERO;

    RwV3d vecBodyPosEnemy = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetBodyPosition(&vecBodyPosEnemy);

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(m_attackPlayerNo);
    if (pPlayerChr)
    {
        RwV3d vecBodyPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetBodyPosition(&vecBodyPosPlayer);

        Math::Vec3_Sub(&vecDir, &vecBodyPosPlayer, &vecBodyPosEnemy);
        Math::Vec3_Normalize(&vecDir, &vecDir);
    }
    else
    {
        vecDir = Math::VECTOR3_AXIS_Z;
        EnemyChr().Compositor().RotateVectorByDirection(&vecDir, &vecDir);
    };

    float fAngle = static_cast<float>(m_shotNum / 2) * m_fShotRadSize;

    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    Math::Matrix_RotateY(&matRot, -fAngle);

    RwV3dTransformVector(&vecDir, &vecDir, &matRot);

    Math::Matrix_RotateY(&matRot, m_fShotRadSize);

    for (int32 i = 0; i < m_shotNum; ++i)
    {
        RwV3dTransformVector(&vecDir, &vecDir, &matRot);

        CEnemyUtils6045::EntryShotWeapon(SHOTID::ID_KUNAI_ENEMY,
                                         &EnemyChr().Compositor(),
                                         &vecBodyPosEnemy,
                                         &vecDir,
                                         true);

        ++m_shotCount;
    };
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseKaraiChr::CAttackC2StatusObserver::OnStart(void) /*override*/
{
    CAttackCStatusObserver::OnStart();

    if (Math::RandFloat() >= 0.5f)
        SetShotSametimeSetting(6, (MATH_PI * 0.9f));
    else
        SetShotContinualSetting(6, (MATH_PI * 0.9f), 0.25f, 3);
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAttackBStatusObserver::CAttackBStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_B)
, m_step(STEP_BEGIN)
, m_orderType(-1)
{
    ;
};


/*virtual*/ void CBaseKaraiChr::CAttackBStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();

    m_orderType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_step = STEP_BEGIN;
};


/*virtual*/ CBaseKaraiChr::CAttackBStatusObserver::RESULT
CBaseKaraiChr::CAttackBStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_BEGIN:
        {
            if (m_orderType != KARAI_AI::ORDERTYPE_ATTACK_BB)
            {
                m_step = STEP_END;
                break;
            };

            float fRotRate = EnemyChr().Feature().m_fRotateRate;
            CEnemyUtils::RotateToPlayer(&EnemyChr().Compositor(), m_orderTargetNo, fRotRate, MATH_DEG2RAD(3.0f));

            float connectT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
            float nowT = EnemyChr().Compositor().GetMotionTime();

            if (nowT >= connectT)
            {
                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(BASEKARAI::MOTIONNAMES::ATTACK_BB, bForce);
                EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
                m_step = STEP_BB;
            };
        }
        break;

    case STEP_BB:
        {
            if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
            {
                RwV3d vecFootPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetFootPosition(&vecFootPos);

                uint32 hMagic = CEnemyUtils6045::EntrySimpleMagic(MAGICID::ID_WINDPRESS, &vecFootPos);
                ASSERT(hMagic);

                if (hMagic)
                {
                    CMagicManager::SetChangeStatus(hMagic, CHitAttackData::STATUS_KNOCK);
                    CMagicManager::SetStatusTime(hMagic, 0.0f);
                };                
            };
        }
        break;

    case STEP_END:
        break;

    default:
        ASSERT(false);
        break;
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CAttackBStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    return CBaseChr6045::COneShotMotionForAttackPlayerAim::OnEnd();
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAttackAStatusObserver::CAttackAStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttackPlayerAim(ENEMYTYPES::MOTIONNAMES::ATTACK_A)
, m_step(STEP_BEGIN)
, m_orderType(-1)
{
    ;
};


/*virtual*/ void CBaseKaraiChr::CAttackAStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttackPlayerAim::OnStart();

    m_orderType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
    m_step = STEP_BEGIN;
};


/*virtual*/ CBaseKaraiChr::CAttackAStatusObserver::RESULT
CBaseKaraiChr::CAttackAStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_BEGIN:
        {
            if (m_orderType == KARAI_AI::ORDERTYPE_ATTACK_A)
            {
                m_step = STEP_END;
                break;
            };

            float connectT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
            float nowT = EnemyChr().Compositor().GetMotionTime();

            if (nowT >= connectT)
            {
                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(BASEKARAI::MOTIONNAMES::ATTACK_AA, bForce);
                CGameSound::PlayAttackSE(&EnemyChr().Compositor());

                m_step = STEP_AA;
            };
        }
        break;

    case STEP_AA:
        {
            if (m_orderType == KARAI_AI::ORDERTYPE_ATTACK_AA)
            {
                m_step = STEP_END;
                break;
            };

            float connectT = EnemyChr().Compositor().GetNextChainMotionConnectTime();
            float nowT = EnemyChr().Compositor().GetMotionTime();

            if (nowT >= connectT)
            {
                const char* pszMotionName = nullptr;
                switch (m_orderType)
                {
                case KARAI_AI::ORDERTYPE_ATTACK_AAA: pszMotionName = BASEKARAI::MOTIONNAMES::ATTACK_AAA; break;
                case KARAI_AI::ORDERTYPE_ATTACK_AAB: pszMotionName = BASEKARAI::MOTIONNAMES::ATTACK_AAB; break;
                case KARAI_AI::ORDERTYPE_ATTACK_AAC: pszMotionName = BASEKARAI::MOTIONNAMES::ATTACK_AAC; break;
                default: ASSERT(false); break;
                };

                bool bForce = true;
                EnemyChr().Compositor().ChangeMotion(pszMotionName, bForce);
                CGameSound::PlayAttackSE(&EnemyChr().Compositor());

                m_step = STEP_AAA;
            };
        }
        break;

    case STEP_AAA:
        m_step = STEP_END;
        break;

    case STEP_END:
        break;

    default:
        ASSERT(false);
        break;
    };

    return CBaseChr6045::COneShotMotionForAttackPlayerAim::Observing();
};


//
// *********************************************************************************
//


CBaseKaraiChr::CGuardStatusObserver::CGuardStatusObserver(CBaseKaraiChr* pBaseKaraiChr)
: m_pBaseKaraiChr(pBaseKaraiChr)
{
    ASSERT(m_pBaseKaraiChr != nullptr);
    
    SetParameter(0.5f, BASEKARAI::GUARD_COUNTERACTION_TIME[CGameProperty::GetDifficulty()]);
};


/*virtual*/ CBaseKaraiChr::CGuardStatusObserver::RESULT
CBaseKaraiChr::CGuardStatusObserver::PassageGuardTotalTime(void) /*override*/
{
    if (CAIUtils::MakeNearerPlayerData(&EnemyChr().Compositor()))
    {
        const CAIUtils::NEARER_PLAYERDATA* pNearerPlayerData = CAIUtils::GetNearerPlayerData(0);
        if (pNearerPlayerData)
        {
            float fDistanceOfSuitable = EnemyChr().AICharacteristic().m_fDistanceOfSuitable;
            if (pNearerPlayerData->distance < fDistanceOfSuitable)
            {
                m_pBaseKaraiChr->AIModerator().SetCatchTriggerAction(KARAI_AI::CATCHTRIGGER_COUNTER);
                m_bCounterActionFlag = true;
                return RESULT_END;
            };
        };
    };

    m_fTotalTimer = 0.0f;
    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


CBaseKaraiChr::CMoveBackRoundStatusObserver::CMoveBackRoundStatusObserver(void)
: CBaseChr6045::COneShotMotionForAttack(BASEKARAI::MOTIONNAMES::MOVE_BACKROUND)
{
    ;
};


/*virtual*/ void CBaseKaraiChr::CMoveBackRoundStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::COneShotMotionForAttack::OnStart();

    EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetMotionSpeed(1.125f);

    float fDirection = 0.0f;
    RwV3d vecOrderPos = EnemyChr().AIThinkOrder().OrderMove().m_vAt;
    if (CEnemyUtils6045::GetDirection(&fDirection, &EnemyChr().Compositor(), &vecOrderPos))
    {
        fDirection += MATH_PI;
        fDirection = CEnemyUtils::RadianCorrect(fDirection);
        EnemyChr().Compositor().SetDirection(fDirection);
    };

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    RwV3d vecVelocity = { 0.0f, 0.0f, -3.0f };
    EnemyChr().Compositor().RotateVectorByDirection(&vecVelocity, &vecVelocity);
    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    RwV3d vecAccel = vecVelocity;
    float endT = EnemyChr().Compositor().GetMotionEndTime();
    Math::Vec3_Scale(&vecAccel, &vecAccel, -(1.125f / endT));

    EnemyChr().Compositor().SetAcceleration(&vecAccel);
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CMoveBackRoundStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    return CBaseChr6045::COneShotMotionForAttack::OnEnd();
};


//
// *********************************************************************************
//


CBaseKaraiChr::CMoveJumpEscapeStatusObserver::CMoveJumpEscapeStatusObserver(CBaseKaraiChr* pBaseKaraiChr)
: m_pBaseKaraiChr(pBaseKaraiChr)
, m_vecOrderPos(Math::VECTOR3_ZERO)
, m_bDoubleJump(false)
{
    ASSERT(m_pBaseKaraiChr != nullptr);

    SetParameter(BASEKARAI::MOTIONNAMES::MOVE_JUMPESCAPE1,
                 BASEKARAI::MOTIONNAMES::MOVE_JUMPESCAPE2,
                 BASEKARAI::MOTIONNAMES::MOVE_JUMPESCAPE4,
                 11.0f);
};


/*virtual*/ void CBaseKaraiChr::CMoveJumpEscapeStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::CJumpActionStatusObserver::OnStart();

    m_vecOrderPos = EnemyChr().AIThinkOrder().OrderMove().m_vAt;

    float fDirection = 0.0f;
    if (CEnemyUtils6045::GetDirection(&fDirection, &EnemyChr().Compositor(), &m_vecOrderPos))
        EnemyChr().Compositor().SetDirection(fDirection);
    
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
};


/*virtual*/ CBaseKaraiChr::CMoveJumpEscapeStatusObserver::RESULT
CBaseKaraiChr::CMoveJumpEscapeStatusObserver::Observing(void) /*override*/
{
    if ((m_step == STEP_JUMP) && !m_bDoubleJump)
    {
        RwV3d vecVelocity = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetVelocity(&vecVelocity);

        if (vecVelocity.y < 0.0f)
        {
            bool bForce = true;
            EnemyChr().Compositor().ChangeMotion(BASEKARAI::MOTIONNAMES::MOVE_JUMPESCAPE3, bForce);
            EnemyChr().Compositor().StopMotion();

            vecVelocity.y = m_fJumpInitSpeed;
            EnemyChr().Compositor().SetVelocity(&vecVelocity);

            EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8388));

            m_bDoubleJump = true;
        };
    };

    return CBaseChr6045::CJumpActionStatusObserver::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CMoveJumpEscapeStatusObserver::OnEnd(void) /*override*/
{
    CBaseChr6045::CJumpActionStatusObserver::OnEnd();

    CEnemyUtils6045::ClearGravitySpecialSetting(&EnemyChr().Compositor());

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);

    m_pBaseKaraiChr->AIModerator().DecisionUnitCommonParameter().SetMoveWaitTime(0.75f);

    return ENEMYTYPES::STATUS_THINKING;
};


/*virtual*/ void CBaseKaraiChr::CMoveJumpEscapeStatusObserver::SetJumpSetting(void) /*override*/
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetFootPosition(&vecFootPos);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecVelocity, &m_vecOrderPos, &vecFootPos);

    float fGravity = (CGameProperty::GetGravity() * 1.5f);
    float fJumpSpeedXZ = (m_fJumpInitSpeed * 2.0f) / fGravity;
    fJumpSpeedXZ = std::fabs(fJumpSpeedXZ);
    fJumpSpeedXZ = 1.0f / (fJumpSpeedXZ * 1.75f);

    vecVelocity.x *= fJumpSpeedXZ;
    vecVelocity.y = m_fJumpInitSpeed;
    vecVelocity.z *= fJumpSpeedXZ;

    EnemyChr().Compositor().SetVelocity(&vecVelocity);

    CEnemyUtils6045::SetGravitySpecialSetting(&EnemyChr().Compositor(), fGravity);

    m_bDoubleJump = false;
};


//
// *********************************************************************************
//


CBaseKaraiChr::CHideStatusObserver::CHideStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_APPEAR)
{
    ;
};


//
// *********************************************************************************
//


CBaseKaraiChr::CAppearStatusObserver::CAppearStatusObserver(void)
: CBaseChr6045::CNothingStatusObserver(ENEMYTYPES::MOTIONNAMES::IDLE, ENEMYTYPES::STATUS_THINKING)
{
    ;
};


//
// *********************************************************************************
//


CBaseKaraiChr::CDeathStatusObserver::CDeathStatusObserver(CBaseKaraiChr* pBaseKaraiChr)
: m_pBaseKaraiChr(pBaseKaraiChr)
{
    ASSERT(m_pBaseKaraiChr != nullptr);

    if (CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
        CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X))
        SetParameter(DEATHKIND_NEXUS, nullptr, 1.0f);
    else
        SetParameter(DEATHKIND_MOTION, nullptr, 1.0f);
};


/*virtual*/ void CBaseKaraiChr::CDeathStatusObserver::OnStart(void) /*override*/
{
    CBaseChr6045::CDeathStatusObserver::OnStart();
    m_pBaseKaraiChr->ClearModeEffects();
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseKaraiChr::CThinkingStatusObserver::OnStart(void) /*override*/
{
    m_nextStatus = ENEMYTYPES::STATUS_IDLE;
};


/*virtual*/ CBaseKaraiChr::CThinkingStatusObserver::RESULT
CBaseKaraiChr::CThinkingStatusObserver::Observing(void) /*override*/
{
    RESULT result = RESULT_END;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_WAIT:
        {
            m_nextStatus = ENEMYTYPES::STATUS_IDLE;            
        }
        break;

    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;

            if ((moveType >= BASEAI6045::ORDERTYPE_MOVE_WALK_POS) &&
                (moveType <= BASEAI6045::ORDERTYPE_MOVE_WALK_NO))
            {
                m_nextStatus = ENEMYTYPES::STATUS_WALK;
            }
            else if ((moveType >= BASEAI6045::ORDERTYPE_MOVE_RUN_POS) &&
                     (moveType <= BASEAI6045::ORDERTYPE_MOVE_RUN_NO))
            {
                m_nextStatus = ENEMYTYPES::STATUS_RUN;
            }
            else if (moveType == BASEAI6045::ORDERTYPE_MOVE_JUMP)
            {
                m_nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
            }
            else if (moveType == KARAI_AI::ORDERTYPE_MOVE_BACKROUND)
            {
                m_nextStatus = BASEKARAI::STATUS_MOVE_BACKROUND;
            }
            else if (moveType == KARAI_AI::ORDERTYPE_MOVE_JUMP_ESCAPE)
            {
                m_nextStatus = BASEKARAI::STATUS_MOVE_JUMPESCAPE;
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case BASEAI6045::ORDERTYPE_ATTACK_TO_AIR:
                if (Math::RandFloat() < 0.5f)
                    m_nextStatus = BASEKARAI::STATUS_ATTACK_E;
                else
                    m_nextStatus = BASEKARAI::STATUS_ATTACK_F;
                break;

            case BASEAI6045::ORDERTYPE_ATTACK_COUNTER:
                m_nextStatus = BASEKARAI::STATUS_ATTACK_COUNTER;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_A:
            case KARAI_AI::ORDERTYPE_ATTACK_AA:
            case KARAI_AI::ORDERTYPE_ATTACK_AAA:
            case KARAI_AI::ORDERTYPE_ATTACK_AAB:
            case KARAI_AI::ORDERTYPE_ATTACK_AAC:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_A;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_B:
            case KARAI_AI::ORDERTYPE_ATTACK_BB:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_B;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_C1:
                m_nextStatus = ENEMYTYPES::STATUS_ATTACK_C;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_C2:
                m_nextStatus = BASEKARAI::STATUS_ATTACK_C;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_D:
                m_nextStatus = BASEKARAI::STATUS_ATTACK_D;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_E:
                m_nextStatus = BASEKARAI::STATUS_ATTACK_E;
                break;

            case KARAI_AI::ORDERTYPE_ATTACK_F:
                m_nextStatus = BASEKARAI::STATUS_ATTACK_F;
                break;
                
            case KARAI_AI::ORDERTYPE_ATTACK_SPECIAL:
                m_nextStatus = BASEKARAI::STATUS_ATTACK_SPECIAL;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        {
            m_nextStatus = ENEMYTYPES::STATUS_GUARD;            
        }
        break;

    default:
        {
            EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            result = RESULT_CONTINUE;
        }
        break;
    };

    return result;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseKaraiChr::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return static_cast<ENEMYTYPES::STATUS>(m_nextStatus);
};


//
// *********************************************************************************
//


/*static*/ void CBaseKaraiChr::GetDashMarkPosition(RwV3d* pvecPos,
                                                   const CCharacterCompositor* pChrCompositor,
                                                   int32 iPlayerNo,
                                                   float fDistance)
{
    ASSERT(pvecPos != nullptr);
    ASSERT(pChrCompositor != nullptr);

    RwV3d vecDir = Math::VECTOR3_ZERO;

    RwV3d vecFootPosEnemy = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&vecFootPosEnemy);

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerNo);
    if (pPlayerChr)
    {
        RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecFootPosPlayer);

        Math::Vec3_Sub(&vecDir, &vecFootPosPlayer, &vecFootPosEnemy);
        Math::Vec3_Normalize(&vecDir, &vecDir);
        vecDir.y = 0.0f;
    }
    else
    {
        vecDir = Math::VECTOR3_AXIS_Z;
        pChrCompositor->RotateVectorByDirection(&vecDir, &vecDir);
    };

    RwV3d vecDashPoint = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecDashPoint, &vecDir, fDistance);
    Math::Vec3_Add(&vecDashPoint, &vecDashPoint, &vecFootPosEnemy);
        
    RwV3d vecStart = vecFootPosEnemy;
    vecStart.y += 0.1f;

    RwV3d vecEnd = vecDashPoint;
    vecEnd.y += 0.1f;

    float fObstacleHeight = 0.0f;
    RwV3d vecCrossPt = Math::VECTOR3_ZERO;
    if (CAIUtils::CheckStep(&vecStart, &vecEnd, &fObstacleHeight, &vecCrossPt))
    {
        Math::Vec3_Scale(&vecDashPoint, &vecDir, -1.0f);
        Math::Vec3_Add(&vecDashPoint, &vecDashPoint, &vecCrossPt);
    };

    *pvecPos = vecDashPoint;
};


CBaseKaraiChr::CBaseKaraiChr(ENEMYID::VALUE enemyId)
: CBaseChr6045(enemyId)
, m_bModeWarning(false)
, m_bModeFinal(false)
, m_hEffectWarning(0)
, m_hEffectFinal(0)
{
    /* init CEnemyCharacter */
    SetFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE);

    /* init CBaseChr6045 */
    m_bKnockBackCtrlActive = true;
    m_knockBackCtrl.SetKnockBackControlEnable(true);

    /* CBaseChr6045 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,            new CBaseChr6045::CKnockForGuardPriorityStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,             new CBaseChr6045::CKnockForGuardPriorityStatusObserver);
#ifdef _DEBUG
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,             new CBaseChr6045::CJumpReadyStatusObserver);
#endif /* _DEBUG */    

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                   new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                   new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                    new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                 new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,              new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,          new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,           new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,    new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,     new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                  new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                 new CCommonEnemyObserver::CFreezeStatusObserver);

    /* CBaseKaraiChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                   new CBaseKaraiChr::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                  new CBaseKaraiChr::CDeathStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_APPEAR,                 new CBaseKaraiChr::CAppearStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,               new CBaseKaraiChr::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                  new CBaseKaraiChr::CGuardStatusObserver(this));
    AttachStatusObserver(BASEKARAI::STATUS_MOVE_BACKROUND,          new CBaseKaraiChr::CMoveBackRoundStatusObserver);
    AttachStatusObserver(BASEKARAI::STATUS_MOVE_JUMPESCAPE,         new CBaseKaraiChr::CMoveJumpEscapeStatusObserver(this));
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,               new CBaseKaraiChr::CAttackAStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,               new CBaseKaraiChr::CAttackBStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,               new CBaseKaraiChr::CAttackCStatusObserver);
    AttachStatusObserver(BASEKARAI::STATUS_ATTACK_C,                new CBaseKaraiChr::CAttackC2StatusObserver);
    AttachStatusObserver(BASEKARAI::STATUS_ATTACK_D,                new CBaseKaraiChr::CAttackDStatusObserver);
    AttachStatusObserver(BASEKARAI::STATUS_ATTACK_E,                new CBaseKaraiChr::CAttackEStatusObserver);
    AttachStatusObserver(BASEKARAI::STATUS_ATTACK_F,                new CBaseKaraiChr::CAttackFStatusObserver);
    AttachStatusObserver(BASEKARAI::STATUS_ATTACK_SPECIAL,          new CBaseKaraiChr::CAttackSpecialStatusObserver(this));
    AttachStatusObserver(BASEKARAI::STATUS_ATTACK_COUNTER,          new CBaseKaraiChr::CAttackCounterStatusObserver);
};


/*virtual*/ CBaseKaraiChr::~CBaseKaraiChr(void)
{
    ClearModeEffects();
};


/*virtual*/ void CBaseKaraiChr::Run(void) /*override*/
{
    if (!m_bModeWarning)
    {
        float fHpRatio = CEnemyUtils::GetEnemyHPRate(this);
        if (fHpRatio <= 0.7f)
        {
            SetWarningModeSetting();
            m_bModeWarning = true;
        };
    };

    if (!m_bModeFinal)
    {
        float fHpRatio = CEnemyUtils::GetEnemyHPRate(this);
        if (fHpRatio <= 0.35f)
        {
            SetFinalModeSetting();
            m_bModeFinal = true;
        };
    };

    CBaseChr6045::Run();
};


/*virtual*/ bool CBaseKaraiChr::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    ENEMYTYPES::STATUS status = GetStatus();

    if ((status == ENEMYTYPES::STATUS_ATTACK_C)         ||
        (status == BASEKARAI::STATUS_MOVE_JUMPESCAPE)  ||
        (status == BASEKARAI::STATUS_ATTACK_C)         ||
        (status == BASEKARAI::STATUS_ATTACK_E)         ||
        (status == BASEKARAI::STATUS_ATTACK_SPECIAL))
        return false;

    return CBaseChr6045::OnMessageCanChangingAerial(fHeight);
};


void CBaseKaraiChr::SetWarningModeSetting(void)
{
    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    Compositor().GetBodyPosition(&vecBodyPos);

    CEnemyUtils6045::EntrySimpleEffect(EFFECTID::ID_ALL_TAME_FLASH, &vecBodyPos, 2.0f);

    RwV3d vecOffset = { 0.0f, 0.3f, 0.0f };
    m_hEffectWarning = CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_I_CRYSHINE_R,
                                                         &Compositor(),
                                                         1.125f,
                                                         CEnemyUtils6045::TRACE_BODY,
                                                         false,
                                                         &vecOffset);

    ASSERT(m_hEffectWarning);

    if (m_pAIModerator)
        AIModerator().SetCatchTriggerAction(KARAI_AI::CATCHTRIGGER_MODE_WARNING);
};


void CBaseKaraiChr::SetFinalModeSetting(void)
{
    RwV3d vecBodyPos = Math::VECTOR3_ZERO;
    Compositor().GetBodyPosition(&vecBodyPos);

    CEnemyUtils6045::EntrySimpleEffect(EFFECTID::ID_ALL_TAME_FLASH, &vecBodyPos, 2.0f);

    if (m_hEffectWarning)
    {
        CEffectManager::Finish(m_hEffectWarning);
        m_hEffectWarning = 0;
    };

    RwV3d vecOffset = { 0.0f, -0.9f, 0.0f };
    m_hEffectFinal = CEnemyUtils6045::EntryTraceEffect(EFFECTID::ID_SETUP_ATTACK,
                                                       &Compositor(),
                                                       1.0f,
                                                       CEnemyUtils6045::TRACE_BODY,
                                                       false,
                                                       &vecOffset);

    ASSERT(m_hEffectFinal);

    if (m_pAIModerator)
        AIModerator().SetCatchTriggerAction(KARAI_AI::CATCHTRIGGER_MODE_FINAL);
};


bool CBaseKaraiChr::IsModeWarning(void) const
{
    return m_bModeWarning;
};


bool CBaseKaraiChr::IsModeFinal(void) const
{
    return m_bModeFinal;
};


void CBaseKaraiChr::ClearModeEffects(void)
{
    if (m_hEffectWarning)
    {
        CEffectManager::Finish(m_hEffectWarning);
        m_hEffectWarning = 0;
    };

    if (m_hEffectFinal)
    {
        CEffectManager::Finish(m_hEffectFinal);
        m_hEffectFinal = 0;
    };
};