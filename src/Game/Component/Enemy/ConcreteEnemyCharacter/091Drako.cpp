#include "091Drako.hpp"

#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Enemy/AIUtils.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/ConcreteAIModerator/091DrakoAI.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


static const RwV3d WAND_OFFSET = { 0.0f, 1.22f, 0.0f };
static const float QUAKE_JUMP_SPEED = 15.0f;


namespace MOTIONNAMES
{
    static const char* ENERGYBALL   = "AA";
    static const char* TAIL         = "B";
    static const char* SOMERSAULT   = "Backaway";
    static const char* ANTI_AIR     = "E";

    static const char* RUSH_START   = "D1";
    static const char* RUSH_END     = "D3";

    static const char* BEAM_START   = "F1";
    static const char* BEAM_LOOP    = "F2";
    static const char* BEAM_END     = "F3";
    
    static const char* QUAKE_START  = "JAttack1";
    static const char* QUAKE_AIR    = "JAttack2";
    static const char* QUAKE_END    = "JAttack3";
}; /* namespace MOTIONNAMES */


//
// *********************************************************************************
//


C091Drako::CRodTracer::CRodTracer(CEnemyCharacter* pEnmChr)
: m_hObj(pEnmChr->Compositor().GetHandle())
{
    ;
};


/*virtual*/ float C091Drako::CRodTracer::GetDirection(void) /*override*/
{
    return 0.0f;
};


/*virtual*/ void C091Drako::CRodTracer::GetPosition(RwV3d* pvPosition) /*override*/
{
    CGameObject* pGameObj = CGameObjectManager::GetObject(m_hObj);
    if (pGameObj == nullptr)
    {
        *pvPosition = Math::VECTOR3_ZERO;
        return;
    };

    CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pGameObj);

    RwV3d vecMyPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetPosition(&vecMyPos);

    RwV3d vecWandPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetBonePosition(&vecWandPos, 7, &WAND_OFFSET);
    Math::Vec3_Add(&vecWandPos, &vecWandPos, &vecMyPos);

    *pvPosition = vecWandPos;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CHideStatusObserver::OnStart(void) /*override*/
{
    ;
};


/*virtual*/ C091Drako::CHideStatusObserver::RESULT
C091Drako::CHideStatusObserver::Observing(void) /*override*/
{
    return RESULT_END;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CHideStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_IDLE;
};


//
// *********************************************************************************
//


C091Drako::CThinkingStatusObserver::CThinkingStatusObserver()
: m_nextStatus(ENEMYTYPES::STATUS_IDLE)
{
    ;
};


/*virtual*/ void C091Drako::CThinkingStatusObserver::OnStart(void) /*override*/
{
    ENEMYTYPES::STATUS nextStatus = m_nextStatus;

    CAIThinkOrder::ORDER order = EnemyChr().AIThinkOrder().GetOrder();
    switch (order)
    {
    case CAIThinkOrder::ORDER_MOVE:
        {
            int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
            switch (moveType)
            {
            case C091DrakoAI::ORDERTYPE::MOVE_WALK_POS:
            case C091DrakoAI::ORDERTYPE::MOVE_WALK_NO:
                nextStatus = ENEMYTYPES::STATUS_WALK;
                break;

            case C091DrakoAI::ORDERTYPE::MOVE_RUN_POS:
            case C091DrakoAI::ORDERTYPE::MOVE_RUN_NO:
                nextStatus = ENEMYTYPES::STATUS_RUN;
                break;

            case C091DrakoAI::ORDERTYPE::MOVE_JUMP:
                nextStatus = ENEMYTYPES::STATUS_JUMP_READY;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_ATTACK:
        {
            int32 attackType = EnemyChr().AIThinkOrder().OrderAttack().m_iAttackType;
            switch (attackType)
            {
            case C091DrakoAI::ORDERTYPE::ATTACK_ENERGY_BALL:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_ENERGY_BALL);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_TAIL:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_TAIL);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_SOMERSAULT:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_SOMERSAULT);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_RUSH:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_RUSH);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_ANTI_AIR:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_ANTI_AIR);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_BEAM:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_BEAM);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_QUAKE:
                nextStatus = static_cast<ENEMYTYPES::STATUS>(C091Drako::STATUS_ATTACK_QUAKE);
                break;

            case C091DrakoAI::ORDERTYPE::ATTACK_THROW:
                nextStatus = ENEMYTYPES::STATUS_THROW;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case CAIThinkOrder::ORDER_DEFENCE:
        nextStatus = ENEMYTYPES::STATUS_GUARD;
        break;

    default:
        nextStatus = ENEMYTYPES::STATUS_IDLE;
        break;
    };

    EnemyChr().SetStatus(nextStatus);

    switch (nextStatus)
    {
    case ENEMYTYPES::STATUS_IDLE:
    case ENEMYTYPES::STATUS_WALK:
    case ENEMYTYPES::STATUS_RUN:
    case ENEMYTYPES::STATUS_GUARD:
        break;

    default:
        EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        break;
    };
};


/*virtual*/ C091Drako::CThinkingStatusObserver::RESULT
C091Drako::CThinkingStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CThinkingStatusObserver::OnEnd(void) /*override*/
{
    return m_nextStatus;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CDeathStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::FLYAWAY_IDLING, true);
    EnemyChr().Compositor().SetEnableCatchHit(false);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);
};


/*virtual*/ C091Drako::CDeathStatusObserver::RESULT
C091Drako::CDeathStatusObserver::Observing(void) /*override*/
{
    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CDeathStatusObserver::OnEnd(void) /*override*/
{
    return ENEMYTYPES::STATUS_QUIT;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CTouchdownStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CTouchdownStatus::OnStart();

    if (EnemyChr().StatusSubject().StatusPrev() == ENEMYTYPES::STATUS_JUMP_ATTACK)
    {
        m_bQuake = true;

        EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::QUAKE_END, true);

        RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
        EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);
        vecMyFootPos.y = 0.0f;

        uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vecMyFootPos);
        CEffectManager::SetScale(hEffect, 4.0f);

        CGameProperty::SetCameraVibration(0.5f, 1.0f, 5);

        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4417));
    }
    else
    {
        m_bQuake = false;
    };
};


/*virtual*/ C091Drako::CTouchdownStatusObserver::RESULT
C091Drako::CTouchdownStatusObserver::Observing(void) /*override*/
{
    if (m_bQuake)
    {
        int32 numPlayers = CAIUtils::GetPlayerNum();
        for (int32 i = 0; i < numPlayers; ++i)
        {
            CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(i);
            if (!pPlrChr)
                continue;

            RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
            pPlrChr->GetFootPosition(&vecPlrFootPos);

            RwV3d vecDir = Math::VECTOR3_ZERO;
            Math::Vec3_Normalize(&vecDir, &vecPlrFootPos);
            Math::Vec3_Scale(&vecDir, &vecDir, 0.5f);
			vecDir.y = 0.0f;

			Math::Vec3_Sub(&vecPlrFootPos, &vecPlrFootPos, &vecDir);

            float fGroundHeight = CWorldMap::GetMapHeight(&vecPlrFootPos);
            if (vecPlrFootPos.y < (fGroundHeight + 0.3f))
            {
                RwV3d vecPlrBodyPos = Math::VECTOR3_ZERO;
                pPlrChr->GetBodyPosition(&vecPlrBodyPos);

                RwSphere hitSphere;
                hitSphere.radius = 0.1f;
                hitSphere.center = vecPlrBodyPos;

                CHitAttackData attackShake;
                attackShake.SetObject(EnemyChr().Compositor().GetHandle());
                attackShake.SetPower(15);
                attackShake.SetAttackNo(i);
                attackShake.SetTarget(CHitAttackData::TARGET_PLAYER);
                attackShake.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
                attackShake.SetStatus(CHitAttackData::STATUS_FLYAWAY);
                attackShake.SetFlyawayParameter(0.0f, 4.0f);
                attackShake.SetShape(CHitAttackData::SHAPE_SPHERE);
                attackShake.SetSphere(&hitSphere);

                vecPlrBodyPos.y -= 1.0f;
                attackShake.SetObjectPos(&vecPlrBodyPos);

                CHitAttackManager::RegistAttack(&attackShake);
            };
        };
    };

    if (EnemyChr().Compositor().IsMotionEnd())
    {
        if (m_bQuake)
            CMessageManager::Request(SEGROUPID::VALUE(155));
        
        return RESULT_END;
    };

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CGuardStatusObserver::OnStart(void) /*override*/
{
    CCommonEnemyObserver::CGuardStatus::OnStart();
    m_step = 0;
};


/*virtual*/ C091Drako::CGuardStatusObserver::RESULT
C091Drako::CGuardStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case STEP_START:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_1, true);
            ++m_step;
        }
        break;

    case STEP_START_RUN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                ++m_step;
        }
        break;

    case STEP_HOLD:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_2, true);
            ++m_step;
        }
        break;

    case STEP_HOLD_RUN:
        {
            if (EnemyChr().AIThinkOrder().GetOrder() != CAIThinkOrder::ORDER_DEFENCE)
                ++m_step;
        }
        break;

    case STEP_END:
        {
            EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::GUARD_3, true);
            ++m_step;
        }
        break;

    case STEP_END_RUN:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CEnergyBallStatusObserver::OnStart(void) /*override*/
{
    m_hEffect = 0;
    m_hMagic = 0;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ENERGYBALL, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    int32 orderTarget = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTarget);
    ASSERT(pPlrChr != nullptr);

    RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecPlrFootPos);

    float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecPlrFootPos);
    float fDirMe = EnemyChr().Compositor().GetDirection();
    float fDirDiff = (fDir - fDirMe);
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
    fDirDiff = Clamp(fDirDiff, -MATH_DEG2RAD(60.0f), MATH_DEG2RAD(60.0f));

    EnemyChr().Compositor().SetDirection(fDirMe + fDirDiff);
};


/*virtual*/ C091Drako::CEnergyBallStatusObserver::RESULT
C091Drako::CEnergyBallStatusObserver::Observing(void) /*override*/
{
    if (m_hEffect)
    {
        float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
        if (fNowMotionT > 0.726f)
        {
            CEffectManager::Finish(m_hEffect);
            m_hEffect = 0;

            RwV3d vecMyPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetPosition(&vecMyPos);

            RwV3d vecWandPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetBonePosition(&vecWandPos, 7, &WAND_OFFSET);
            Math::Vec3_Add(&vecWandPos, &vecWandPos, &vecMyPos);

            CMagicManager::CParameter param;
            param.SetPositon(&vecWandPos);

            m_hMagic = CMagicManager::Play(MAGICID::ID_DRKBALL, &param);
        };
    }
    else if (!m_hMagic)
    {
        float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
        if (fNowMotionT > 0.231f)
        {
            m_hEffect = CEffectManager::PlayTrace(EFFECTID::ID_DRKBALL_START,
                                                  new C091Drako::CRodTracer(&EnemyChr()),
                                                  &Math::VECTOR3_ZERO);
        };
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CEnergyBallStatusObserver::OnEnd(void) /*override*/
{
    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };

    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CTailStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::TAIL, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ C091Drako::CTailStatusObserver::RESULT
C091Drako::CTailStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CTailStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CSomersaultStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::SOMERSAULT, true);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    float fDir = EnemyChr().Compositor().GetDirection();    
    RwV3d vecVel = { (std::sin(fDir) * -8.0f), 0.0f, (std::cos(fDir) * -8.0f) };
    EnemyChr().Compositor().SetVelocity(&vecVel);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ C091Drako::CSomersaultStatusObserver::RESULT
C091Drako::CSomersaultStatusObserver::Observing(void) /*override*/
{
    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);

    float fMoveSpeed = Math::Vec3_Length(&vecVel);
    fMoveSpeed -= ((8.0f * CGameProperty::GetElapsedTime()) / 0.5f);
    if (fMoveSpeed < 0.0f)
        fMoveSpeed = 0.0f;

    Math::Vec3_Normalize(&vecVel, &vecVel);
    Math::Vec3_Scale(&vecVel, &vecVel, fMoveSpeed);

    EnemyChr().Compositor().SetVelocity(&vecVel);

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CSomersaultStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CRushStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::RUSH_START, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);

    int32 orderTarget = EnemyChr().AIThinkOrder().OrderAttack().m_iPlayerNo;
    CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(orderTarget);
    ASSERT(pPlrChr != nullptr);

    RwV3d vecPlrFootPos = Math::VECTOR3_ZERO;
    pPlrChr->GetFootPosition(&vecPlrFootPos);

    float fDir = CEnemyUtils::GetDirection(&EnemyChr().Compositor(), &vecPlrFootPos);
    float fDirMe = EnemyChr().Compositor().GetDirection();
    float fDirDiff = (fDir - fDirMe);
    fDirDiff = CEnemyUtils::RadianCorrect(fDirDiff);
    fDirDiff = Clamp(fDirDiff, -MATH_DEG2RAD(60.0f), MATH_DEG2RAD(60.0f));

    EnemyChr().Compositor().SetDirection(fDirMe + fDirDiff);
};


/*virtual*/ C091Drako::CRushStatusObserver::RESULT
C091Drako::CRushStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
            {
                EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::RUSH_END, true);
                ++m_step;
            };
        }
        break;

    case 1:
        {
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CRushStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CAntiAirStatusObserver::OnStart(void) /*override*/
{
    m_hMagic = 0;
    m_stopCnt = 0;
    m_fStopTime = 0.0f;
    
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::ANTI_AIR, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(false);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_NOREACTION);
};


/*virtual*/ C091Drako::CAntiAirStatusObserver::RESULT
C091Drako::CAntiAirStatusObserver::Observing(void) /*override*/
{
    if (m_hMagic)
    {        
        if (++m_stopCnt < 15) // hold anim for 15 frames
            EnemyChr().Compositor().SetMotionTime(m_fStopTime);
    }
    else
    {
        float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
        if (fNowMotionT > 0.561f)
        {
            m_fStopTime = EnemyChr().Compositor().GetMotionTime();

            float fMyDir = EnemyChr().Compositor().GetDirection();
            RwV3d vecMyFootPos = Math::VECTOR3_ZERO;
            EnemyChr().Compositor().GetFootPosition(&vecMyFootPos);

            RwV3d vecMagicPos = vecMyFootPos;
            vecMagicPos.x += (std::sin(fMyDir) * 0.5f);
            vecMagicPos.z += (std::cos(fMyDir) * 0.5f);

            CMagicManager::CParameter param;
            param.SetPositon(&vecMagicPos);

            m_hMagic = CMagicManager::Play(MAGICID::ID_DRK_PILLAR, &param);

            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4416));
        };
    };

    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CAntiAirStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().Compositor().SetEnableBodyHitSelfToOther(true);
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CBeamStatusObserver::OnStart(void) /*override*/
{
    m_step = 0;
    m_loopCnt = 0;
    m_playerNo = 0;
    m_hEffect = 0;

    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::BEAM_START, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    m_hEffect = CEffectManager::PlayTrace(EFFECTID::ID_ROD_FLASH,
                                          new CRodTracer(&EnemyChr()),
                                          &Math::VECTOR3_ZERO);;
    CEffectManager::SetScale(m_hEffect, 0.5f);
    
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4418));
    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8476));
};


/*virtual*/ C091Drako::CBeamStatusObserver::RESULT
C091Drako::CBeamStatusObserver::Observing(void) /*override*/
{
    switch (m_step)
    {
    case 0:
        {
            if (!EnemyChr().Compositor().IsMotionEnd())
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::BEAM_LOOP, true);
            CEffectManager::SetScale(m_hEffect, 2.0f);
            ++m_step;
        }
        break;

    case 1:
        {
            int32 numPlayers = CAIUtils::GetPlayerNum();
            float fEndMotionT = EnemyChr().Compositor().GetMotionEndTime();
            float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
            float fStartMotionTForPlayerNo = ((static_cast<float>(m_playerNo) * fEndMotionT) / static_cast<float>(numPlayers));
            if (fStartMotionTForPlayerNo < fNowMotionT)
            {
                RwV3d vecMyPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetPosition(&vecMyPos);

                RwV3d vecWandPos = Math::VECTOR3_ZERO;
                EnemyChr().Compositor().GetBonePosition(&vecWandPos, 7, &WAND_OFFSET);
                Math::Vec3_Add(&vecWandPos, &vecWandPos, &vecMyPos);

                RwV3d vecPlrPos = Math::VECTOR3_ZERO;
                CPlayerCharacter* pPlrChr = CAIUtils::GetActivePlayer(m_playerNo);
                if (pPlrChr)
                    pPlrChr->GetFootPosition(&vecPlrPos); /* TODO some player chr can avoid beam hit by just staying (slashuur for example)
                                                                  because catch sphere too high from the foot and  beam move vector just miss the hit catch sphere.
                                                                  This how its works in retail game but i guess its incorrect (better get pos of the body not foot) */

                RwV3d vecDir = Math::VECTOR3_ZERO;
                Math::Vec3_Sub(&vecDir, &vecPlrPos, &vecWandPos);
        
                CMagicManager::CParameter param;
                param.SetPositon(&vecWandPos);
                param.SetDirection(&vecDir);

                CMagicManager::Play(MAGICID::ID_ROD_BEAM, &param);

                CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4415));

                ++m_playerNo;
            };

            if (!IsLoopMotionEnd())
                break;

            m_playerNo = 0;
            if (++m_loopCnt < 3)
                break;

            EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::BEAM_END, true);
            ++m_step;
        }
        break;

    case 2:
        {
            EndEffect();
            
            if (EnemyChr().Compositor().IsMotionEnd())
                return RESULT_END;
        }
        break;

    default:
        break;
    };

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CBeamStatusObserver::OnEnd(void) /*override*/
{
    EndEffect();
    CMessageManager::Request(SEGROUPID::VALUE(154));
    return ENEMYTYPES::STATUS_THINKING;
};


bool C091Drako::CBeamStatusObserver::IsLoopMotionEnd(void) const
{
    float fEndMotionT = EnemyChr().Compositor().GetMotionEndTime();
    float fNowMotionT = EnemyChr().Compositor().GetMotionTime();
    float dt = CGameProperty::GetElapsedTime();

    return ((fNowMotionT + dt) > fEndMotionT);
};


void C091Drako::CBeamStatusObserver::EndEffect(void)
{
    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;

        CGameSound::FadeOutSE(SDCODE_SE(4418), CGameSound::FADESPEED_NORMAL);
    };
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CQuakeStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::QUAKE_START, true);
    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    CMessageManager::Request(SEGROUPID::VALUE(156));

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8473));
};


/*virtual*/ C091Drako::CQuakeStatusObserver::RESULT
C091Drako::CQuakeStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CQuakeStatusObserver::OnEnd(void) /*override*/
{
    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);
    vecVel.y += QUAKE_JUMP_SPEED;
    EnemyChr().Compositor().SetVelocity(&vecVel);

    return ENEMYTYPES::STATUS_JUMP_ATTACK;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CJumpAttackStatusObserver::OnStart(void) /*override*/
{
    m_bSoundPlay = false;

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    EnemyChr().Compositor().ChangeMotion(MOTIONNAMES::QUAKE_AIR, true);

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4419));
};


/*virtual*/ C091Drako::CJumpAttackStatusObserver::RESULT
C091Drako::CJumpAttackStatusObserver::Observing(void) /*override*/
{
    RwV3d vecVel = Math::VECTOR3_ZERO;
    EnemyChr().Compositor().GetVelocity(&vecVel);

    if (vecVel.y < 0.0f)
    {
        vecVel.y -= (CGameProperty::GetElapsedTime() * 29.4f);
        EnemyChr().Compositor().SetVelocity(&vecVel);

        if (!m_bSoundPlay)
        {
            m_bSoundPlay = true;
            CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8475));
        };
    };

    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), QUAKE_JUMP_SPEED);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CJumpAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


/*virtual*/ void C091Drako::CThrowStatusObserver::OnStart(void) /*override*/
{
    m_bSoundPlay = false;
    CCommonEnemyObserver::CBaseThrow::OnStart();
};


/*virtual*/ C091Drako::CThrowStatusObserver::RESULT
C091Drako::CThrowStatusObserver::Observing(void) /*override*/
{
    if (IsThrowHit() && !m_bSoundPlay)
    {
        m_bSoundPlay = true;
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(8474));    
    };

    return CCommonEnemyObserver::CBaseThrow::Observing();
};


/*virtual*/ ENEMYTYPES::STATUS C091Drako::CThrowStatusObserver::OnEnd(void) /*override*/
{
    if (IsThrowSuccess())
    {
        float fDir = EnemyChr().Compositor().GetDirection();
        EnemyChr().Compositor().SetDirection(fDir - MATH_PI);
    };

    return CCommonEnemyObserver::CBaseThrow::OnEnd();
};


//
// *********************************************************************************
//


C091Drako::C091Drako(void)
: CEnemyCharacter(ENEMYID::ID_DORAKO)
, m_pThrow(nullptr)
{
    SetFlag(ENEMYTYPES::FLAG_GUARD_ALLRANGE);

    /* C091Drako status */
    AttachStatusObserver(ENEMYTYPES::STATUS_HIDE,                       new C091Drako::CHideStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_THINKING,                   new C091Drako::CThinkingStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_DEATH,                      new C091Drako::CDeathStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_TOUCHDOWN,                  new C091Drako::CTouchdownStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_GUARD,                      new C091Drako::CGuardStatusObserver);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_ATTACK,                new C091Drako::CJumpAttackStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_ENERGY_BALL,          new C091Drako::CEnergyBallStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_TAIL,                 new C091Drako::CTailStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_SOMERSAULT,           new C091Drako::CSomersaultStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_RUSH,                 new C091Drako::CRushStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_ANTI_AIR,             new C091Drako::CAntiAirStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_BEAM,                 new C091Drako::CBeamStatusObserver);
    AttachStatusObserver(C091Drako::STATUS_ATTACK_QUAKE,                new C091Drako::CQuakeStatusObserver);
    AttachThrowStatusObserver();

    /* CCommonEnemyObserver status */
    AttachStatusObserver(ENEMYTYPES::STATUS_IDLE,                       new CCommonEnemyObserver::CIdleStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,                       new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_RUN,                        new CCommonEnemyObserver::CMoveStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_JUMP_READY,                 new CCommonEnemyObserver::CJumpReadyStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_AERIAL,                     new CCommonEnemyObserver::CAerialStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_FRONT,                new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_KNOCK_BACK,                 new CCommonEnemyObserver::CKnockStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_FRONT,              new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BACK,               new CCommonEnemyObserver::CFlyawayStatus(false));
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT,        new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK,         new CCommonEnemyObserver::CFlyawayBoundStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_GETUP,                      new CCommonEnemyObserver::CGetupStatus);
    AttachStatusObserver(ENEMYTYPES::STATUS_FREEZE,                     new CCommonEnemyObserver::CCharacterEffectStatusObserver);

    /* init charater parameters */
    PARAMETER parameter;
    std::memset(&parameter, 0, sizeof(parameter));
    
    parameter.m_pfnAIInstance   = C091DrakoAI::Instance;
    parameter.m_bToon           = true;
    parameter.m_fShadowRadius   = 3.0f;

    parameter.m_feature.m_fWalkMoveSpeed        = 3.4f;
    parameter.m_feature.m_fRunMoveSpeed         = 8.0f;
    parameter.m_feature.m_fDashMoveSpeed        = 16.0f;
    parameter.m_feature.m_fDashMoveTime         = 0.2f;
    parameter.m_feature.m_fRotateRate           = 1.5f;
    parameter.m_feature.m_fJumpInitializeSpeed  = 10.606602f; // TODO
    parameter.m_feature.m_fAerialMoveSpeed      = 8.0f;
    parameter.m_feature.m_fAerialAcceleration   = 12.0f;
    parameter.m_feature.m_iHPMax                = 1000;
    parameter.m_feature.m_iHP                   = 0;
    parameter.m_feature.m_vPatrolOrigin         = Math::VECTOR3_ZERO;
    parameter.m_feature.m_fPatrolRadius         = 0.0f;
    parameter.m_feature.m_iPattern              = 0;

    parameter.m_AICharacteristic.m_fThinkingFrequency   = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfActivity     = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfMove         = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfFrontView    = 1.0f;
    parameter.m_AICharacteristic.m_fRatioOfRearView     = 1.0f;
    parameter.m_AICharacteristic.m_fRadiusOfAction      = 10.0f;
    parameter.m_AICharacteristic.m_fDistanceOfSuitable  = 20.0f;
    parameter.m_AICharacteristic.m_fRatioOfImpulsiveness= 0.0f;

    uint8 aFreqTable[][GAMETYPES::DIFFICULTY_NUM] =
    {
        /* FREQUENCY_THINKING   0 */  {  10,   5,   0 },
    };
    
    parameter.m_iFrequencyMax    = COUNT_OF(aFreqTable);
    parameter.m_puFrequencyParam = &aFreqTable[0][0];

	bool bReplace = false;
	Initialize(&parameter, bReplace);
};


/*virtual*/ C091Drako::~C091Drako(void)
{
    ;
};


/*virtual*/ void C091Drako::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    CEnemyCharacter::OnMessageAttackResult(pCatch);
    m_pThrow->SetAttackParam(pCatch);
};


void C091Drako::AttachThrowStatusObserver(void)
{
    m_pThrow = new C091Drako::CThrowStatusObserver;

    RwV3d vecBone = { 0.2f, -0.3f, 0.1f };
    m_pThrow->SetTraceBoneData(4, &vecBone);

    RwV3d vecThrow = { 0.0f, 10.0f, -10.0f };
    m_pThrow->SetThrowVector(&vecThrow);

    AttachStatusObserver(ENEMYTYPES::STATUS_THROW, m_pThrow);
};