#include "080UltimateNinja.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/080UltimateNinjaAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


namespace MOTIONNAMES
{
    static const char* ATTACK_AA       = "AA";
    static const char* ATTACK_BBBB     = "BBBB";
    static const char* ATTACK_WEIGHT   = "Fundou";
    static const char* ATTACK_SMOKE    = "Enmaku";
    static const char* WAPR_1          = "Warp1";
    static const char* WAPR_2          = "Warp2";
    static const char* ATTACK_JUMP     = "JShuriken";
    static const char* ATTACK_DRAGON_1 = "Ryu1";
    static const char* ATTACK_DRAGON_2 = "Ryu2";
    static const char* ATTACK_DRAGON_3 = "Ryu3";
    static const char* ATTACK_DRAGON_4 = "Ryu4";
};


class C080UltimateNinja::CScatterKnifeMgr
{
public:
    static const int32 ROLL_RATE;
    static const float ROLL_DISTANCE;
    static const int32 SUB_KNIFE_NUM;
    static const float KNIFE_DISTANCE;

public:
    CScatterKnifeMgr(CCharacterCompositor* pChrCompositor);
    ~CScatterKnifeMgr(void) {};

    void MakeTargetData(RwV3d* pvecTargetPos,
                        RwV3d* pvecTargetDir,
                        const RwV3d* pvecCurrentPos,
                        const RwV3d* pvecOrderPos,
                        float fRollDistance = ROLL_DISTANCE,
                        int32 iRollRate = ROLL_RATE);

    void ShotKnifes(const RwV3d* pvecCurrentPos,
                    const RwV3d* pvecTargetPos,
                    const RwV3d* pvecTargetDir,
                    float fKnifeDistance = KNIFE_DISTANCE,
                    int32 iSubKnifeNum = SUB_KNIFE_NUM);

    void ShotKnife(const RwV3d* pvecCurrentPos,
                   const RwV3d* pvecTargetPos,
                   const RwV3d* pvecTargetDir,
                   float fKnifeDistance);

protected:
    CCharacterCompositor* m_pChrCompositor;
};


class C080UltimateNinja::CThinkingObserver : public Utils6034::CBaseThinkingObserver 
{
public:
    virtual ~CThinkingObserver(void) {};
    virtual int32 OnGetOriginalMoveStatus(int32 iMoveType) override;
    virtual int32 OnGetOriginalAttackStatus(int32 iAttackType) override;
};


class C080UltimateNinja::CAttackAAObserver : public Utils6034::CBaseOneShotTurnAttack
{
public:
    CAttackAAObserver(void);
    virtual ~CAttackAAObserver(void) {};
    virtual void OnStart(void) override;
};


class C080UltimateNinja::CAttackBBBBObserver : public Utils6034::CBaseOneShotAttack
{
public:
    CAttackBBBBObserver(void);
    virtual ~CAttackBBBBObserver(void) {};
    virtual void OnStart(void) override;
};


class C080UltimateNinja::CAttackJumpReadyObserver : public Utils6034::CBaseJumpReadyObserver
{
public:
    CAttackJumpReadyObserver(Utils6034::ICharacterController* pChrController);
    virtual ~CAttackJumpReadyObserver(void) {};
    virtual void OnStart(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::ICharacterController* m_pChrController;
};


class C080UltimateNinja::CAttackJumpAerialObserver : public CEnemyCharacter::CStatusObserver 
{
public:
    CAttackJumpAerialObserver(CCharacterCompositor* pChrCompositor,
                              Utils6034::ICharacterController* pChrController);
    virtual ~CAttackJumpAerialObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    int32                            m_step;
    Utils6034::CFixTimer             m_timer;
    int32                            m_loopCount;
    CScatterKnifeMgr                 m_scatterKnifeMgr;
    Utils6034::ICharacterController* m_pChrController;
    bool                             m_bFlagInvincibilityRestored;
};


class C080UltimateNinja::CAttackDragonObserver : public CEnemyCharacter::CStatusObserver 
{
public:
    CAttackDragonObserver(UltimateNinjaUtils::CDragon* pDragon);
    virtual ~CAttackDragonObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
    void UpdateFlying(void);

protected:
    int32                        m_step;
    UltimateNinjaUtils::CDragon* m_pDragon;
    RwV3d                        m_vecStartPos;
    Utils6034::CFixTimer         m_timer;
    float                        m_fFrameVelY;
    float                        m_fMaxAltitude;
};


class C080UltimateNinja::CAttackDragonAerialObserver : public CEnemyCharacter::CStatusObserver 
{
public:
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
};


class C080UltimateNinja::CAttackDragonTouchdownObserver : public CCommonEnemyObserver::CTouchdownStatus 
{
public:
    virtual void OnStart(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
};


class C080UltimateNinja::CAttackSmokeObserver : public CEnemyCharacter::CStatusObserver 
{
public:
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;
};


class C080UltimateNinja::CAttackThrowWeightObserver : public CEnemyCharacter::CStatusObserver 
{
public:
    CAttackThrowWeightObserver(Utils6034::CFixTimer* pTimerSpinningWeightSE);
    virtual ~CAttackThrowWeightObserver(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::CFixTimer* m_pTimerSpinningWeightSE;
};


class C080UltimateNinja::CWarpObserer : public CEnemyCharacter::CStatusObserver 
{
public:
    CWarpObserer(void);
    virtual ~CWarpObserer(void) {};
    virtual void OnStart(void) override;
    virtual RESULT Observing(void) override;
    virtual ENEMYTYPES::STATUS OnEnd(void) override;

protected:
    Utils6034::CFixTimer m_timerWarpOut;
    RwV3d                m_vecOrderPos;
    int32                m_step;
};


class C080UltimateNinja::CWarpEndObserver : public C080UltimateNinja::CWarpObserer 
{
public:
    virtual ~CWarpEndObserver(void) {};
    virtual RESULT Observing(void) override;
};


//
// *********************************************************************************
//


/*static*/ const int32 C080UltimateNinja::CScatterKnifeMgr::ROLL_RATE = 40;
/*static*/ const float C080UltimateNinja::CScatterKnifeMgr::ROLL_DISTANCE = 3.5f;
/*static*/ const int32 C080UltimateNinja::CScatterKnifeMgr::SUB_KNIFE_NUM = 2;
/*static*/ const float C080UltimateNinja::CScatterKnifeMgr::KNIFE_DISTANCE = 1.8f;


C080UltimateNinja::CScatterKnifeMgr::CScatterKnifeMgr(CCharacterCompositor* pChrCompositor)
: m_pChrCompositor(pChrCompositor)
{
    ;
};


void
C080UltimateNinja::CScatterKnifeMgr::MakeTargetData(RwV3d* pvecTargetPos,
                                                    RwV3d* pvecTargetDir,
                                                    const RwV3d* pvecOrderPos,
                                                    const RwV3d* pvecCurrentPos,
                                                    float fRollDistance /*= ROLL_DISTANCE*/,
                                                    int32 iRollRate /*= ROLL_RATE*/)
{
    RwV3d vecFront = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecFront, pvecOrderPos, pvecCurrentPos);
    vecFront.y = 0.0f;

    RwV3d vecLeft = Math::VECTOR3_ZERO;
    Utils6034::MakeXByDirection(&vecLeft, &vecFront, &Math::VECTOR3_AXIS_Y);

    RwMatrix matRotY;
    RwMatrixSetIdentity(&matRotY);
    Math::Matrix_RotateY(&matRotY, MATH_DEG2RAD(iRollRate));

    RwV3dTransformVector(pvecTargetDir, &vecLeft, &matRotY);

    RwV3d vecOrderPos = *pvecOrderPos;    
    RwV3d vecCurrentPos = *pvecCurrentPos;
    vecCurrentPos.y = vecOrderPos.y;

    if (fRollDistance < 0.0f)
        Utils6034::CalcRotationPoint(pvecTargetPos, &vecCurrentPos, &vecOrderPos, -fRollDistance, 0.0f, false);
    else
        Utils6034::CalcRotationPoint(pvecTargetPos, &vecCurrentPos, &vecOrderPos, fRollDistance, 0.0f, true);

    pvecTargetPos->y = CWorldMap::GetMapHeight(pvecTargetPos);
};


void
C080UltimateNinja::CScatterKnifeMgr::ShotKnifes(const RwV3d* pvecCurrentPos,
                                                const RwV3d* pvecTargetPos,
                                                const RwV3d* pvecTargetDir,
                                                float fKnifeDistance /*= KNIFE_DISTANCE*/,
                                                int32 iSubKnifeNum /*= SUB_KNIFE_NUM*/)
{
    ShotKnife(pvecCurrentPos, pvecTargetPos, pvecTargetDir, 0.0f);

    for (int32 i = 0; i < iSubKnifeNum; ++i)
    {
        float fDist = static_cast<float>(i + 1) * fKnifeDistance;

        ShotKnife(pvecCurrentPos, pvecTargetPos, pvecTargetDir,  fDist);
        ShotKnife(pvecCurrentPos, pvecTargetPos, pvecTargetDir, -fDist);
    };
};


void
C080UltimateNinja::CScatterKnifeMgr::ShotKnife(const RwV3d* pvecCurrentPos,
                                               const RwV3d* pvecTargetPos,
                                               const RwV3d* pvecTargetDir,
                                               float fKnifeDistance)
{
    RwV3d vecShotDir = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecShotDir, pvecTargetDir, fKnifeDistance);
    Math::Vec3_Add(&vecShotDir, &vecShotDir, pvecTargetPos);
    Math::Vec3_Sub(&vecShotDir, &vecShotDir, pvecCurrentPos);

    CShotManager::Shot(SHOTID::ID_SCATTER_KNIFE, pvecCurrentPos, &vecShotDir, m_pChrCompositor);
};


//
// *********************************************************************************
//


/*virtual*/ int32
C080UltimateNinja::CThinkingObserver::OnGetOriginalMoveStatus(int32 iMoveType) /*override*/
{
    switch (iMoveType)
    {
    case C080UltimateNinjaAI::ORDERTYPE_MOVE_WARP:
        return C080UltimateNinja::STATUS_WARP_START;

    case C080UltimateNinjaAI::ORDERTYPE_MOVE_WARP_END:
        return C080UltimateNinja::STATUS_WARP_END;

    default:
        ASSERT(false);
        break;
    };

    return Utils6034::CBaseThinkingObserver::OnGetOriginalMoveStatus(iMoveType);
};


/*virtual*/ int32
C080UltimateNinja::CThinkingObserver::OnGetOriginalAttackStatus(int32 iAttackType) /*override*/
{
    switch (iAttackType)
    {
    case C080UltimateNinjaAI::ORDERTYPE_ATTACK_AA:
        return ENEMYTYPES::STATUS_ATTACK_AA;
        
    case C080UltimateNinjaAI::ORDERTYPE_ATTACK_BBBB:
        return ENEMYTYPES::STATUS_ATTACK_B;

    case C080UltimateNinjaAI::ORDERTYPE_ATTACK_ENMAKU:
        return ENEMYTYPES::STATUS_ATTACK_C;

    case C080UltimateNinjaAI::ORDERTYPE_ATTACK_FUNDOU:
        return ENEMYTYPES::STATUS_FIRE_A;

    case C080UltimateNinjaAI::ORDERTYPE_ATTACK_DRAGON:
        return ENEMYTYPES::STATUS_FIRE_B;

    case C080UltimateNinjaAI::ORDERTYPE_ATTACK_JUMP:
        return C080UltimateNinja::STATUS_ATTACK_JUMP_READY;

    default:
        ASSERT(false);
        break;
    };

    return Utils6034::CBaseThinkingObserver::OnGetOriginalAttackStatus(iAttackType);
};


//
// *********************************************************************************
//


C080UltimateNinja::CAttackAAObserver::CAttackAAObserver(void)
: Utils6034::CBaseOneShotTurnAttack(MOTIONNAMES::ATTACK_AA)
{
    ;
};


/*virtual*/
void C080UltimateNinja::CAttackAAObserver::OnStart(void) /*override*/
{
    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
    Utils6034::CBaseOneShotTurnAttack::OnStart();
};


//
// *********************************************************************************
//


C080UltimateNinja::CAttackBBBBObserver::CAttackBBBBObserver(void)
: Utils6034::CBaseOneShotAttack(MOTIONNAMES::ATTACK_BBBB)
{
    ;
};


/*virtual*/
void C080UltimateNinja::CAttackBBBBObserver::OnStart(void) /*override*/
{
    CGameSound::PlayAttackSE(&EnemyChr().Compositor());
    Utils6034::CBaseOneShotAttack::OnStart();
};



//
// *********************************************************************************
//


C080UltimateNinja::CAttackJumpReadyObserver::CAttackJumpReadyObserver(Utils6034::ICharacterController* pChrController)
: m_pChrController(pChrController)
{
    ;
};


/*virtual*/ void
C080UltimateNinja::CAttackJumpReadyObserver::OnStart(void) /*override*/
{
    m_pChrController->FlagInvincibility().IncCounter();
    Utils6034::CBaseJumpReadyObserver::OnStart();
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackJumpReadyObserver::OnEnd(void) /*override*/
{
    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

    RwV3d vecAwayDir = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecAwayDir, &vecMyPos, &vecOrderPos);
    Math::Vec3_Normalize(&vecAwayDir, &vecAwayDir);

    RwV3d vecAwayVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecAwayVelocity, &vecAwayDir, EnemyChr().Feature().m_fAerialMoveSpeed);
    vecAwayVelocity.y = EnemyChr().Feature().m_fJumpInitializeSpeed;

    EnemyChr().Compositor().SetVelocity(&vecAwayVelocity);

    m_pChrController->FlagInvincibility().DecCounter();

    return static_cast<ENEMYTYPES::STATUS>(C080UltimateNinja::STATUS_ATTACK_JUMP_AERIAL);
};


//
// *********************************************************************************
//


C080UltimateNinja::CAttackJumpAerialObserver::CAttackJumpAerialObserver(CCharacterCompositor* pChrCompositor,
                                                                        Utils6034::ICharacterController* pChrController)
: m_step(-1)
, m_timer(0.2f)
, m_loopCount(-1)
, m_scatterKnifeMgr(pChrCompositor)
, m_pChrController(pChrController)
, m_bFlagInvincibilityRestored(false)
{
    ;
};


/*virtual*/ void
C080UltimateNinja::CAttackJumpAerialObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_bFlagInvincibilityRestored = false;
    m_pChrController->FlagInvincibility().IncCounter();
    m_loopCount = 10;

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JUMP, true);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);    
};


/*virtual*/ C080UltimateNinja::CAttackJumpAerialObserver::RESULT
C080UltimateNinja::CAttackJumpAerialObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(),
                                          EnemyChr().Feature().m_fJumpInitializeSpeed);

            if (EnemyChr().Compositor().GetVelocity().y < 0.0f)
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_JUMP, true);

                RwV3d vecBodyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

                RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

                RwV3d vecTargetPos = Math::VECTOR3_ZERO;
                RwV3d vecTargetDir = Math::VECTOR3_ZERO;
                m_scatterKnifeMgr.MakeTargetData(&vecTargetPos,
                                                 &vecTargetDir,
                                                 &vecBodyPos,
                                                 &vecOrderPos,
                                                 CScatterKnifeMgr::ROLL_DISTANCE,
                                                 CScatterKnifeMgr::ROLL_RATE);

                m_scatterKnifeMgr.ShotKnifes(&vecBodyPos, &vecTargetPos, &vecTargetDir);

                m_timer.Start();
                m_loopCount = 0;
                m_pChrController->FlagInvincibility().DecCounter();
                m_bFlagInvincibilityRestored = true;
                ++m_step;

                CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8453));
            };
        }
        break;

    case 1:
        {
            m_timer.Update();
            if (m_timer.IsAlarm())
            {
                RwV3d vecBodyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

                RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

                RwV3d vecTargetPos = Math::VECTOR3_ZERO;
                RwV3d vecTargetDir = Math::VECTOR3_ZERO;
                m_scatterKnifeMgr.MakeTargetData(&vecTargetPos,
                                                 &vecTargetDir,
                                                 &vecBodyPos,
                                                 &vecOrderPos,
                                                 -CScatterKnifeMgr::ROLL_DISTANCE,
                                                 -CScatterKnifeMgr::ROLL_RATE);

                m_scatterKnifeMgr.ShotKnifes(&vecBodyPos, &vecTargetPos, &vecTargetDir);

                m_loopCount = 0;
                ++m_step;
            };
        }
        break;

    default:
        break;
    };

    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

    CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(),
                                  &vecOrderPos,
                                  EnemyChr().Feature().m_fRotateRate,
                                  MATH_DEG2RAD(3.0f));

    if (++m_loopCount < 3)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4128));

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackJumpAerialObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    if (!m_bFlagInvincibilityRestored)
        m_pChrController->FlagInvincibility().DecCounter();
    
    return ENEMYTYPES::STATUS_AERIAL;
};


//
// *********************************************************************************
//


C080UltimateNinja::CAttackDragonObserver::CAttackDragonObserver(UltimateNinjaUtils::CDragon* pDragon)
: m_step(-1)
, m_pDragon(pDragon)
, m_vecStartPos(Math::VECTOR3_ZERO)
, m_timer(1.0f)
, m_fFrameVelY(CGameProperty::GetElapsedTime())
, m_fMaxAltitude(5.0f)
{
    ;
};


/*virtual*/ void
C080UltimateNinja::CAttackDragonObserver::OnStart(void) /*override*/
{
    m_step = 0;

    int32 iPathID = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    m_pDragon->SetPathID(iPathID);

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_DRAGON_1, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().GetPosition(&m_vecStartPos);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
	EnemyChr().Compositor().SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);

    bool bIsNexusMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M60X) ||
                       CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M61X);

    bool bIsRandProc = (Math::RandRange(0, 100) < 14);

    if (bIsNexusMap || bIsRandProc)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8457));
    else
        CMessageManager::Request(SEGROUPID::VALUE(112));
};


/*virtual*/ C080UltimateNinja::CAttackDragonObserver::RESULT
C080UltimateNinja::CAttackDragonObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_DRAGON_2, true);

            m_timer.Start();
            ++m_step;
        }
        break;

    case 1:
        {
            m_timer.Update();
            if (m_timer.IsAlarm())
            {
                m_pDragon->Start();
                ++m_step;
            };

            UpdateFlying();
        }
        break;

    case 2:
        {
            if (!m_pDragon->IsActive())
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_DRAGON_3, true);
                EnemyChr().Compositor().PlayMotion();
                EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

                return RESULT_END;
            };

            UpdateFlying();
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackDragonObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    static_cast<Utils6034::IBaseAIModerator&>(EnemyChr().AIModerator()).NotifyGuard();

    return static_cast<ENEMYTYPES::STATUS>(C080UltimateNinja::STATUS_ATTACK_DRAGON_AERIAL);
};


void C080UltimateNinja::CAttackDragonObserver::UpdateFlying(void)
{
    RwV3d vecPosNow = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPosNow);

    vecPosNow.y += m_fFrameVelY;

    if ((vecPosNow.y - m_vecStartPos.y) > m_fMaxAltitude)
        vecPosNow.y = (m_fMaxAltitude + m_vecStartPos.y);

    EnemyChr().Compositor().SetPosition(&vecPosNow);
};


//
// *********************************************************************************
//


/*virtual*/ void
C080UltimateNinja::CAttackDragonAerialObserver::OnStart(void) /*override*/
{
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
};


/*virtual*/ C080UltimateNinja::CAttackDragonAerialObserver::RESULT
C080UltimateNinja::CAttackDragonAerialObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackDragonAerialObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void
C080UltimateNinja::CAttackDragonTouchdownObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_DRAGON_4);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    CEnemyUtils::EntryTouchDownEffectAndSE(&EnemyChr());
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackDragonTouchdownObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C080UltimateNinja::CAttackSmokeObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_SMOKE, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecMyPos);

    RwV3d vecToOrderPosDir = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecToOrderPosDir, &vecOrderPos, &vecMyPos);
    Math::Vec3_Normalize(&vecToOrderPosDir, &vecToOrderPosDir);

    RwV3d vecAwayVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecAwayVelocity, &vecToOrderPosDir, -5.0f);

    EnemyChr().Compositor().SetVelocity(&vecAwayVelocity);
};


/*virtual*/ C080UltimateNinja::CAttackSmokeObserver::RESULT
C080UltimateNinja::CAttackSmokeObserver::Observing(void) /*override*/
{
    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
    CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vecOrderPos, MATH_DEG2RAD(360.0f));

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecBodyPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetBodyPosition(&vecBodyPos);

        RwV3d vecDir = Math::VECTOR3_ZERO;
        Utils6034::GetCharacterDirectionVector(&EnemyChr().Compositor(), &vecDir);

        Math::Vec3_Scale(&vecDir, &vecDir, 1.5f);
        Math::Vec3_Add(&vecBodyPos, &vecBodyPos, &vecDir);

        CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_ULNIN_ENMAKU), &vecBodyPos);

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4391));

        EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    };

    if (EnemyChr().Compositor().IsMotionEnd())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackSmokeObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


C080UltimateNinja::CAttackThrowWeightObserver::CAttackThrowWeightObserver(Utils6034::CFixTimer* pTimerSpinningWeightSE)
: m_pTimerSpinningWeightSE(pTimerSpinningWeightSE)
{
    ;
};


/*virtual*/ void C080UltimateNinja::CAttackThrowWeightObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ATTACK_WEIGHT, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);

    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
    float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecOrderPos);
    EnemyChr().Compositor().SetDirection(fDir);
};


/*virtual*/ C080UltimateNinja::CAttackThrowWeightObserver::RESULT
C080UltimateNinja::CAttackThrowWeightObserver::Observing(void) /*override*/
{
    RwV3d vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
    CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(), &vecOrderPos, 1.2f);

    if (EnemyChr().Compositor().TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING))
    {
        RwV3d vecDirection = Math::VECTOR3_ZERO;
        Utils6034::GetCharacterDirectionVector(&EnemyChr().Compositor(), &vecDirection);

        RwV3d vecHandPos = Math::VECTOR3_ZERO;
        Utils6034::GetWorldBonePosition(&vecHandPos, &EnemyChr().Compositor(), 9);

        CShotManager::Shot(SHOTID::ID_SPINNING_WEIGHT,
                           &vecHandPos,
                           &vecDirection,
                           &EnemyChr().Compositor());

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8458));
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4392));

        m_pTimerSpinningWeightSE->Start();
    };

    if (EnemyChr().Compositor().IsMotionEnd())
    {
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CAttackThrowWeightObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


C080UltimateNinja::CWarpObserer::CWarpObserer(void)
: m_timerWarpOut(0.35f)
, m_vecOrderPos(Math::VECTOR3_ZERO)
, m_step(-1)
{
    ;
};


/*virtual*/ void C080UltimateNinja::CWarpObserer::OnStart(void) /*override*/
{
    m_step = 0;
    m_vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
    m_timerWarpOut.Start();

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WAPR_1, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetEnableBodyHit(false);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetPosition(&vecPos);
    CEffectManager::Play(EFFECTID::ID_WINDWARP_START, &vecPos);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4393));
};


/*virtual*/ C080UltimateNinja::CWarpObserer::RESULT
C080UltimateNinja::CWarpObserer::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::WAPR_2, true);
            EnemyChr().Compositor().SetPosition(&m_vecOrderPos);

            ++m_step;
        }
        break;

    case 1:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
                return RESULT_END;
            };

            m_vecOrderPos = Utils6034::GetPositionByThinkOrder(&EnemyChr().AIThinkOrder());
            
            CEnemyUtils::RotateToPosition(&EnemyChr().Compositor(),
                                          &m_vecOrderPos,
                                          EnemyChr().Feature().m_fRotateRate);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    m_timerWarpOut.Update();

    if (m_timerWarpOut.IsAlarm())
    {
        m_timerWarpOut.Pause();

        CEffectManager::Play(EFFECTID::ID_WINDWARP_OUT, &m_vecOrderPos);        
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4394));
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS
C080UltimateNinja::CWarpObserer::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHit(true);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ C080UltimateNinja::CWarpEndObserver::RESULT
C080UltimateNinja::CWarpEndObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
    {
        EnemyChr().Compositor().SetPosition(&m_vecOrderPos);
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

        return RESULT_END;
    };

    m_timerWarpOut.Update();

    if (m_timerWarpOut.IsAlarm())
    {
        m_timerWarpOut.Pause();

        CEffectManager::Play(EFFECTID::ID_WINDWARP_OUT, &m_vecOrderPos);
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4394));
    };

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


C080UltimateNinja::C080UltimateNinja(void)
: CEnemyCharacter(ENEMYID::ID_ULTIMATE_NINJA)
, m_chrController()
, m_timerSpinningWeightSE(2.0f)
, m_dragon(&Compositor())
{
    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                         new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                      new CCommonEnemyObserver::CTouchdownStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                           new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                            new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                    new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                     new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                         new CCommonEnemyObserver::CCharacterEffectStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,                  new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,                   new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,            new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,             new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                          new CCommonEnemyObserver::CGetupStatus);
    
    /* Utils6034 status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                           new Utils6034::CBaseHideObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                          new Utils6034::CBaseDeathObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                          new Utils6034::CGuardObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                     new Utils6034::CBaseJumpReadyObserver);

    /* C080UltimateNinja status */
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                       new C080UltimateNinja::CThinkingObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_AA,                      new C080UltimateNinja::CAttackAAObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_B,                       new C080UltimateNinja::CAttackBBBBObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_C,                       new C080UltimateNinja::CAttackSmokeObserver);
	AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_A,                         new C080UltimateNinja::CAttackThrowWeightObserver(&m_timerSpinningWeightSE));
    AttachStatusObserver(C080UltimateNinja::STATUS_WARP_START,              new C080UltimateNinja::CWarpObserer);
    AttachStatusObserver(C080UltimateNinja::STATUS_WARP_END,                new C080UltimateNinja::CWarpEndObserver);
    AttachStatusObserver(C080UltimateNinja::STATUS_ATTACK_JUMP_READY,       new C080UltimateNinja::CAttackJumpReadyObserver(&m_chrController));
    AttachStatusObserver(C080UltimateNinja::STATUS_ATTACK_JUMP_AERIAL,      new C080UltimateNinja::CAttackJumpAerialObserver(&Compositor(), &m_chrController));
    AttachStatusObserver(ENEMYTYPES::STATUS_FIRE_B,                         new C080UltimateNinja::CAttackDragonObserver(&m_dragon));
    AttachStatusObserver(C080UltimateNinja::STATUS_ATTACK_DRAGON_AERIAL,    new C080UltimateNinja::CAttackDragonAerialObserver);
    AttachStatusObserver(C080UltimateNinja::STATUS_ATTACK_DRAGON_TOUCHDOWN, new C080UltimateNinja::CAttackDragonTouchdownObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C080UltimateNinjaAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 1.6f;

    parameter.m_feature.m_fWalkMoveSpeed        = 2.55f;
    parameter.m_feature.m_fRunMoveSpeed         = 6.1f;
    parameter.m_feature.m_fDashMoveSpeed        = 0.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.0f;
    parameter.m_feature.m_fRotateRate           = 2.3f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 8.5f;
    parameter.m_feature.m_fAerialMoveSpeed      = 3.2f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 500;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 0.3f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 0.6f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 0.5f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 0.5f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 15.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 2.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    Initialize(&parameter, false);

    /* init AI moderator */
    Utils6034::IBaseAIModerator& rAIModerator = static_cast<Utils6034::IBaseAIModerator&>(AIModerator());
    rAIModerator.Initialize(&m_chrController);

    /* init chr controller */
    m_chrController.Initialize(this, &AIThinkOrder(), &rAIModerator);
    m_chrController.SetOverflowData(100, GetNoReactionDamageAmount(), 2.0f);
};


/*virtual*/ C080UltimateNinja::~C080UltimateNinja(void)
{
    ;
};


/*virtual*/ void C080UltimateNinja::Run(void) /*override*/
{
    CEnemyCharacter::Run();
	
	m_dragon.Update();
	m_timerSpinningWeightSE.Update();

	if (!CShotManager::IsExist(SHOTID::ID_SPINNING_WEIGHT) ||
		m_timerSpinningWeightSE.IsAlarm())
	{
		CGameSound::StopSE(SDCODE_SE(4392));
	};
};


/*virtual*/ void C080UltimateNinja::Draw(void) /*override*/
{
    CEnemyCharacter::Draw();
	m_dragon.Draw();
};


/*virtual*/ void C080UltimateNinja::OnStart(void) /*override*/
{
    CEnemyCharacter::OnStart();

    bool bIsBridgeMap = CEnemyUtils::CheckNowMapForMapID(MAPID::ID_M44NB);
    if (bIsBridgeMap)
    {
        if (Math::Rand() & 1)
            CMessageManager::Request(SEGROUPID::VALUE(110));
        else
            CMessageManager::Request(SEGROUPID::VALUE(111));
    };
};


/*virtual*/ bool C080UltimateNinja::OnMessageCanChangingAerial(float fHeight) /*override*/
{
    int32 statusNow = static_cast<int32>(GetStatus());

    if ((statusNow == C080UltimateNinja::STATUS_ATTACK_JUMP_READY) ||
        (statusNow == C080UltimateNinja::STATUS_ATTACK_JUMP_AERIAL) ||
        (statusNow == C080UltimateNinja::STATUS_ATTACK_DRAGON_AERIAL) ||
        (statusNow == ENEMYTYPES::STATUS_FIRE_B))
        return false;

    return CEnemyCharacter::OnMessageCanChangingAerial(fHeight);
};


/*virtual*/ void C080UltimateNinja::OnMessageTouchdown(float fHeight) /*override*/
{
    if (!TestFlag(ENEMYTYPES::FLAG_DEATH_STATUS))
    {
        int32 statusNow = static_cast<int32>(GetStatus());

        if (statusNow == C080UltimateNinja::STATUS_ATTACK_JUMP_AERIAL)
        {
            bool bResult = SetStatus(ENEMYTYPES::STATUS_TOUCHDOWN);
            ASSERT(bResult);
            return;
        };

        if (statusNow == C080UltimateNinja::STATUS_ATTACK_DRAGON_AERIAL)
        {
            bool bResult = SetStatus(C080UltimateNinja::STATUS_ATTACK_DRAGON_TOUCHDOWN);
            ASSERT(bResult);
            return;
        };
    };

    CEnemyCharacter::OnMessageTouchdown(fHeight);
};


/*virtual*/ CHARACTERTYPES::ATTACKRESULTTYPE
C080UltimateNinja::OnDamage(CCharacterAttackCalculator& rCalc) /*override*/
{
    m_chrController.StoreEnemyFlag();
    m_chrController.SetEnemyFlag();

    CHARACTERTYPES::ATTACKRESULTTYPE attackResultType = CEnemyCharacter::OnDamage(rCalc);

    m_chrController.RestoreEnemyFlag();
    m_chrController.SendAIMessage(attackResultType, rCalc.GetAttack().GetPower());

    if (m_chrController.IsResultKnockDamaged(attackResultType) &&
        m_chrController.DamageCounter().IsOverflowDamage())
    {
        m_chrController.FlagNoReaction().AddTime(1.0f);
    };

    return attackResultType;
};


int32 C080UltimateNinja::GetNoReactionDamageAmount(void) const
{
    int32 aiOverflowQuantity[] = { 90, 60, 45 };
    static_assert(COUNT_OF(aiOverflowQuantity) == GAMETYPES::DIFFICULTY_NUM, "update me");

    GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
    return aiOverflowQuantity[difficulty];
};