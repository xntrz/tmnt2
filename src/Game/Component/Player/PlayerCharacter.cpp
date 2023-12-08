#include "PlayerCharacter.hpp"
#include "PlayerStateMachine.hpp"
#include "PlayerTracer.hpp"
#include "Player.hpp"
#include "PlayerUtil.hpp"

#include "ConcretePlayerCharacter/Leonardo.hpp"
#include "ConcretePlayerCharacter/Raphael.hpp"
#include "ConcretePlayerCharacter/Michelangero.hpp"
#include "ConcretePlayerCharacter/Donatello.hpp"
#include "ConcretePlayerCharacter/Slashuur.hpp"
#include "ConcretePlayerCharacter/Casey.hpp"
#include "ConcretePlayerCharacter/Karai.hpp"
#include "ConcretePlayerCharacter/Splinter.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Tracer.hpp"
#include "Game/Component/Shot/ShotManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/ToGimmickMessageModule.hpp"
#include "Game/Component/Module/FootprintsModule.hpp"
#include "Game/Component/Module/AccumulateModule.hpp"
#include "Game/Component/Module/FootfallModule.hpp"
#include "Game/Component/Module/BlinkCharacterModule.hpp"
#include "Game/Component/Module/WallJumpSignModule.hpp"
#include "Game/Component/Module/PlayerChangeVoiceModule.hpp"
#include "Game/Component/Module/DamageFloorModule.hpp"
#include "Game/Component/Module/PlayerAttributeControlModule.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Module/BandanaModule.hpp"
#include "Game/Component/Module/LocusModule.hpp"
#include "Game/Component/Module/RippleEffectModule.hpp"
#include "Game/Component/Module/CharacterEffectModule.hpp"
#include "Game/Component/Module/AmbientLightModule.hpp"
#include "Game/System/Character/CharacterAttackCalculator.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Controller.hpp"


/*static*/ CPlayerCharacter* CPlayerCharacter::New(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume)
{
    switch (idChr)
    {
    case PLAYERID::ID_LEO:
        return new CLeonardo(costume);

    case PLAYERID::ID_RAP:
        return new CRaphael(costume);
	
	case PLAYERID::ID_MIC:
		return new CMichelangero(costume);

    case PLAYERID::ID_DON:
        return new CDonatello(costume);

    case PLAYERID::ID_SLA:
        return new CSlashuur(costume);

    case PLAYERID::ID_CAS:
        return new CCasey(costume);

    case PLAYERID::ID_KAR:
        return new CKarai(costume);

    case PLAYERID::ID_SPL:
        return new CSplinter(costume);

    default:
        ASSERT(false);
        return nullptr;
    };
};


/*static*/ void CPlayerCharacter::Delete(CPlayerCharacter* pPlayerChr)
{
    ASSERT(pPlayerChr);
    delete pPlayerChr;
};


CPlayerCharacter::CPlayerCharacter(const char* pszName, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
: CCharacter(pszName, TYPE_PLAYER)
, m_idPlayer(idPlayer)
, m_costume(costume)
, m_nPlayerIndex(-1)
, m_nPlayerNo(-1)
, m_nPadID(-1)
, m_nNumWallJump(0)
, m_attackparameter({ 0 })
, m_attribute(0)
, m_playerflag(0)
, m_pStateMachine(nullptr)
, m_chargephase(PLAYERTYPES::CHARGEPHASE_ZERO)
, m_feature({ 0 })
, m_vReplacepoint(Math::VECTOR3_ZERO)
, m_bActive(false)
, m_bClassicInput(false)
{
    m_bClassicInput = CGameData::Option().Play().IsClassicInput();
};


CPlayerCharacter::~CPlayerCharacter(void)
{
    if (m_pStateMachine)
    {
        delete m_pStateMachine;
        m_pStateMachine = nullptr;
    };
};


void CPlayerCharacter::Run(void)
{
    ASSERT(m_pStateMachine);
    
    m_pStateMachine->Run();
    CCharacter::Run();
};


void CPlayerCharacter::Draw(void) const
{
    CCharacter::Draw();
};


void CPlayerCharacter::IncludeBasicModule(void)
{
    CCharacter::IncludeBasicModule();

    m_pModuleMan->Include(new CPlayerAttributeControlModule(
        MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP,
        PLAYERTYPES::ATTRIBUTE_ATTACK_POWER_UP,
        this
    ));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(
        MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP,
        PLAYERTYPES::ATTRIBUTE_DEFENCE_POWER_UP,
        this
    ));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(
        MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT,
        PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT,
        this
    ));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(
        MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE,
        PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE,
        this
    ));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(
        MODULETYPE::PLAYERATTRCTRL_CONFUSION,
        PLAYERTYPES::ATTRIBUTE_CONFUSION,
        this
    ));

	m_pModuleMan->Include(new CPlayerAttributeControlModule(
		MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY,
		PLAYERTYPES::ATTRIBUTE_INVINCIBILITY,
		this
	));

    m_pModuleMan->Include(new CWallJumpSignModule(this));
    m_pModuleMan->Include(new CPlayerFootfallModule(this));
    m_pModuleMan->Include(new CBlinkCharacterModule(this));
    m_pModuleMan->Include(new CDamageFloorModule(this));
    m_pModuleMan->Include(new CPlayerChangeVoiceModule(this));
    m_pModuleMan->Include(new CRippleEffectModule(this, 1.5f));
    m_pModuleMan->Include(CToGimmickMessageModule::New(this));
    m_pModuleMan->Include(CFootprintsModule::New(this));
    m_pModuleMan->Include(CAccumulateModule::New(this));
};


void CPlayerCharacter::CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame)
{
    CCharacter::CalcVelocityPerFrame(rvVelocityPerFrame);

    RwV3d vBeforeBodyPos = Math::VECTOR3_ZERO;
    RwV3d vAfterBodyPos = Math::VECTOR3_ZERO;
    
    GetFootPosition(&vBeforeBodyPos);
    Math::Vec3_Add(&vAfterBodyPos, &vBeforeBodyPos, &rvVelocityPerFrame);

    if (CGameProperty::AdjustWithinMoveBoundary(&vAfterBodyPos, &vAfterBodyPos))
    {
        rvVelocityPerFrame.x = vAfterBodyPos.x - vBeforeBodyPos.x;
        rvVelocityPerFrame.z = vAfterBodyPos.z - vBeforeBodyPos.z;
    };
};


bool CPlayerCharacter::OnMessageCanChangingAerial(float fFieldHeight)
{
    ASSERT(m_pStateMachine);

    bool bResult = false;
    
    switch (m_pStateMachine->CurrentStatus())
    {
    case PLAYERTYPES::STATUS_JUMP:
    case PLAYERTYPES::STATUS_JUMP_2ND:
    case PLAYERTYPES::STATUS_JUMP_WALL:
    case PLAYERTYPES::STATUS_JUMP_WALL_FAILURE:
    case PLAYERTYPES::STATUS_AERIAL:
    case PLAYERTYPES::STATUS_AERIAL_MOVE:
    case PLAYERTYPES::STATUS_ATTACK_JUMP:
    case PLAYERTYPES::STATUS_CAUGHT:
    case PLAYERTYPES::STATUS_LIFTED:
    case PLAYERTYPES::STATUS_LIFTED_WALK:
    case PLAYERTYPES::STATUS_THROWN:
    case PLAYERTYPES::STATUS_THROWN_BACK:
    case PLAYERTYPES::STATUS_THROWN_COMBINATION:
    case PLAYERTYPES::STATUS_FLYAWAY_BACK:
    case PLAYERTYPES::STATUS_FLYAWAY_FRONT:
        bResult = false;
        break;

    case PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT:
        ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_FRONT);
        bResult = false;
        break;

    case PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK:
        ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_BACK);
        bResult = false;
        break;

    default:
        bResult = !IsPlayerFlagSet(PLAYERTYPES::FLAG_AERIAL_STATUS);
        break;
    };

    return bResult;
};


void CPlayerCharacter::OnMessageDoChangingAerial(void)
{
    ChangeStatus(PLAYERTYPES::STATUS_AERIAL);
};


void CPlayerCharacter::OnMessageTouchdown(float fFieldHeight)
{
    CCharacter::OnMessageTouchdown(fFieldHeight);

    switch (m_pStateMachine->CurrentStatus())
    {
    case PLAYERTYPES::STATUS_JUMP:
    case PLAYERTYPES::STATUS_JUMP_2ND:
    case PLAYERTYPES::STATUS_JUMP_WALL:
    case PLAYERTYPES::STATUS_JUMP_WALL_FAILURE:
    case PLAYERTYPES::STATUS_AERIAL:
    case PLAYERTYPES::STATUS_AERIAL_MOVE:
    case PLAYERTYPES::STATUS_ATTACK_JUMP:
    case PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP:   
        ChangeStatus(PLAYERTYPES::STATUS_TOUCHDOWN);
        break;

    case PLAYERTYPES::STATUS_THROWN:
    case PLAYERTYPES::STATUS_FLYAWAY_BACK:
        ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK);
        break;

    case PLAYERTYPES::STATUS_FLYAWAY_FRONT:
        ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT);
        break;

    case PLAYERTYPES::STATUS_CRASH_WALL_FALL_FRONT:
        ChangeStatus(PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_FRONT);
        break;

    case PLAYERTYPES::STATUS_CRASH_WALL_FALL_BACK:
        ChangeStatus(PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK);
        break;
    };

    if (GetStatus() != PLAYERTYPES::STATUS_CAUGHT)
    {
        if (m_iDamageRequest > 0)
        {
            CGameObjectManager::SendMessage(this, CHARACTERTYPES::MESSAGEID_RECVDMG, (void*)m_iDamageRequest);
            m_iDamageRequest = 0;
        };
    };
};


void CPlayerCharacter::OnMessageAttackResult(CHitCatchData* pCatch)
{
    if (pCatch->GetResult() == CHitCatchData::RESULT_THROWBACK ||
        pCatch->GetResult() == CHitCatchData::RESULT_THROWFRONT)
    {
        if (GetStatus() == PLAYERTYPES::STATUS_LIFT_CHALLENGE ||
            (m_idPlayer == PLAYERID::ID_RAP && GetStatus() == PLAYERTYPES::STATUS_ATTACK_AAC))
        {
            CGameObject* pDefender = pCatch->GetObject();
            ASSERT(pDefender);
            
            m_liftinfo.m_bLiftSuccess = true;
            std::strcpy(m_liftinfo.m_szLiftObjectName, pDefender->GetName());
            m_liftinfo.m_iStatus = pCatch->GetResult();

            if (pCatch->GetResult() == CHitCatchData::RESULT_THROWFRONT &&
                pDefender->GetType() == GAMEOBJECTTYPE::CHARACTER)
            {
                CCharacter* pDefenderChr = (CCharacter*)pDefender;
                if (pDefenderChr->GetAttackCharacterType() == CCharacter::TYPE_ENEMY)
                    pDefenderChr->RequestDamage(pCatch->GetAttack()->GetPower());
            };
        };
    };

    if (pCatch->GetResult() == CHitCatchData::RESULT_HIT)
    {
        switch (GetStatus())
        {
        case PLAYERTYPES::STATUS_DASH_CANCEL:
        case PLAYERTYPES::STATUS_DASH:
        case PLAYERTYPES::STATUS_DASH_FINISH:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_DASH);
            break;

        case PLAYERTYPES::STATUS_ATTACK_A:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_A);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AA:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AA);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AAB:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AAB);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AAC:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AAC);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AABB:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AABB);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AABC:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AABC);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AABBB:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AABBB);
            break;

        case PLAYERTYPES::STATUS_ATTACK_AABBC:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_AABBC);
            break;

        case PLAYERTYPES::STATUS_ATTACK_B:
            {
                switch (m_chargephase)
                {
                case PLAYERTYPES::CHARGEPHASE_ZERO:
                    CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_B_PHASENONE);
                    break;

                case PLAYERTYPES::CHARGEPHASE_1ST:
                    CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_B_PHASE1);
                    break;

                case PLAYERTYPES::CHARGEPHASE_2ND:
                    CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_B_PHASE2);
                    break;

                case PLAYERTYPES::CHARGEPHASE_3RD:
                    CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_B_PHASE3);
                    break;
                };
            }
            break;

        case PLAYERTYPES::STATUS_ATTACK_RUN:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_RUN);
            break;

        case PLAYERTYPES::STATUS_ATTACK_JUMP:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_JUMP);
            break;

        case PLAYERTYPES::STATUS_ATTACK_KNIFE:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_KNIFE);
            break;

        case PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_KNIFE_JUMP);
            break;

        case PLAYERTYPES::STATUS_THROW_BACK:
            CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_THROW_BACK);
            break;
        };
    };

    if (pCatch->GetResult() == CHitCatchData::RESULT_HIT)
    {
        if (GetStatus() >= PLAYERTYPES::STATUS_ATTACK_AABBB &&
            GetStatus() <= PLAYERTYPES::STATUS_ATTACK_AABBC)
        {
            CPlayerChangeVoiceModule* pModule = (CPlayerChangeVoiceModule*)GetModule(MODULETYPE::PLAYER_CHNG_VOICE);
            if (pModule)
                pModule->SetComboWall();                
        };
    };
};


void CPlayerCharacter::OnMessageReceivedDamage(int32 nDamage)
{
	CGameProperty::Player(m_nPlayerNo)->AddHP(-nDamage);
};


void CPlayerCharacter::CalcAttackPower(CHitAttackData& rAttack)
{
    int32 iPower = rAttack.GetPower();

    if (m_chargephase == PLAYERTYPES::CHARGEPHASE_3RD)
        iPower += iPower;

    if (IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_ATTACK_POWER_UP))
        iPower += iPower;

    if (CGameData::Record().Secret().GetAttackLevel() >= 3)
        iPower += iPower;

    iPower = int32(CGameData::Record().Secret().GetAttackEnchanceValue() * float(iPower));
    
    rAttack.SetPower(iPower);
};


CHARACTERTYPES::ATTACKRESULTTYPE CPlayerCharacter::OnDamage(CCharacterAttackCalculator& calculator)
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG DefenceFlag = CheckDefenceStatusFlag(calculator.GetAttack());
    CHARACTERTYPES::ATTACKRESULTTYPE AttackResult = calculator.CalcAtackParameter(m_attackparameter, DefenceFlag);

    if (IsCharacterFlagSet(CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING) ||
        IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_INVINCIBILITY))
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

    switch (AttackResult)
    {
    case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK:
        ChangeKnockStatus();
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY:
        ChangeFlyawayStatus();
        break;
    
    case CHARACTERTYPES::ATTACKRESULTTYPE_THROW:
        ChangeThrowStatus(calculator);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
        ChangeStatus(PLAYERTYPES::STATUS_DINDLE);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
        ChangeStatus(PLAYERTYPES::STATUS_STUN);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
        ChangeStatus(PLAYERTYPES::STATUS_SLEEP);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
        ChangeStatus(PLAYERTYPES::STATUS_FREEZE);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_BIND:
        ChangeStatus(PLAYERTYPES::STATUS_BIND);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_PARTY_ATTACK:
        CheckPartyAttack(calculator);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD:
        ChangeStatus(PLAYERTYPES::STATUS_GUARD_BACK);
        calculator.SetCatchResult(CHitCatchData::RESULT_GUARD);
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT:
        ChangeStatus(PLAYERTYPES::STATUS_GUARD_IMPACT);
        calculator.SetCatchResult(CHitCatchData::RESULT_GUARDIMPACT);
        CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_GUARD_IMPACT);
        PlayerUtil::CallVoiceOfGuardImpact(this);
        break;
    };


    if (!CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_HEALTH) &&
        calculator.IsConfusionAttack() &&
        AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD &&
        AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT)
    {
        SetConfusionEnable(true, 4.0f);                
        IGamepad::StartVibration(GetPadID(), IGamepad::VIBRATIONTYPE_NORMAL, 0.2f);
    };

    if (AttackResult &&
        AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_THROW &&
        AttackResult != CHARACTERTYPES::ATTACKRESULTTYPE_PARTY_ATTACK)
    {
        CGameEvent::SetPlayerDamaged(m_nPlayerNo);
    };

    return AttackResult;
};


void CPlayerCharacter::OnMessageCatch(void* pParam)
{
    ChangeStatus(PLAYERTYPES::STATUS_CAUGHT);
};


void CPlayerCharacter::OnMessageLift(void* pParam)
{
    MSG_LIFT_INFO* pMsgLiftInfo = (MSG_LIFT_INFO*)pParam;

    SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, true);
    SetDirection(pMsgLiftInfo->m_fDirection + Math::PI);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    vVelocity.x = (pMsgLiftInfo->m_vPosition.x - vPosition.x) * (1.0f / CGameProperty::GetElapsedTime());
    vVelocity.y = (pMsgLiftInfo->m_vPosition.y - vPosition.y) * (1.0f / CGameProperty::GetElapsedTime());
    vVelocity.z = (pMsgLiftInfo->m_vPosition.z - vPosition.z) * (1.0f / CGameProperty::GetElapsedTime());

    SetVelocity(&vVelocity);
};


void CPlayerCharacter::OnMessageThrow(void* pParam)
{
    RwV3d* pvVelocity = (RwV3d*)pParam;
    ASSERT(pvVelocity);

    SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, false);
    SetVelocity(pvVelocity);
    ChangeStatus(PLAYERTYPES::STATUS_THROWN);
};


void CPlayerCharacter::OnMessageMissThrow(void* pParam)
{
    RwV3d* pvVelocity = (RwV3d*)pParam;
    ASSERT(pvVelocity);

    SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY, false);
    SetVelocity(pvVelocity);
	RequestDamage(0);
	ChangeStatus(PLAYERTYPES::STATUS_THROWN_MISS);
};


void CPlayerCharacter::OnMessageCombination(void* pParam)
{
	ChangeStatus(PLAYERTYPES::STATUS_THROWN_COMBINATION);
};


void CPlayerCharacter::OnSteppedDeathFloor(void)
{
	IGamepad::StartVibration(GetPadID(), IGamepad::VIBRATIONTYPE_HARD, 0.2f);

	if (FLAG_TEST(m_collisionGroundInfo.m_attribute, MAPTYPES::ATTRIBUTE_DOBON))
		CEffectManager::Play(EFFECTID::ID_ALL_W_DOBON, &m_vPosition);

	CGameEvent::SetPlayerFallen(m_nPlayerNo);
};


int32 CPlayerCharacter::GetPlayerIndex(void) const
{
	int32 nPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < nPlayerNum; ++i)
    {
		IGamePlayer* pGamePlayer = CGameProperty::Player(i);
		CPlayer* pPlayer = pGamePlayer->GetPlayer();

		if (pPlayer->IsIncludedCharacter(m_idPlayer))
			return i;
    };

    return -1;
};


void CPlayerCharacter::ShootingKnife(void)
{
    RwV3d vPosition = *m_pModel->GetBonePositionFromID(m_feature.m_nKnifeAttachBoneID);
    RwV3d vLocalPos = Math::VECTOR3_ZERO;
    
    Math::Vec3_Sub(&vLocalPos, &vPosition, &m_vPosition);
    float fBuffY = vLocalPos.y;
    vLocalPos.y = 0.0f;
    
    float fLen = Math::Vec3_Length(&vLocalPos);
    if(fLen > 0.6f)
    {
        vLocalPos.x *= 0.5f / fLen;
        vLocalPos.z *= 0.5f / fLen;
        vLocalPos.y = fBuffY;

        Math::Vec3_Add(&vPosition, &vLocalPos, &m_vPosition);
    };

    CShotManager::Shot(SHOTID::ID_KNIFE_PLAYER, &vPosition, m_fDirection, this, 0.26f, 5.0f);

    if (!IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE))
        CGameProperty::Player(m_nPlayerNo)->AddShurikenNum(-1);
};


PLAYERTYPES::STATUS CPlayerCharacter::RequesStatusMorphing(PLAYERTYPES::STATUS status)
{
    switch (status)
    {
    case PLAYERTYPES::STATUS_JUMP:
        if (GetStatus() == PLAYERTYPES::STATUS_IDLE ||
            GetStatus() == PLAYERTYPES::STATUS_WALK ||
            GetStatus() == PLAYERTYPES::STATUS_RUN)
        {
            return PLAYERTYPES::STATUS_JUMP_READY;
        };
        break;

    case PLAYERTYPES::STATUS_GUARD:
        if (GetStatus() == PLAYERTYPES::STATUS_IDLE ||
            GetStatus() == PLAYERTYPES::STATUS_WALK ||
            GetStatus() == PLAYERTYPES::STATUS_RUN)
        {
            return PLAYERTYPES::STATUS_GUARD_READY;
        };
        break;

    case PLAYERTYPES::STATUS_LIFT:
        if (GetStatus() == PLAYERTYPES::STATUS_GUARD ||
            GetStatus() == PLAYERTYPES::STATUS_GUARD_READY)
        {
            return PLAYERTYPES::STATUS_LIFT_CHALLENGE;
        };
        break;
    };

    if (GetStatus() == PLAYERTYPES::STATUS_GUARD && status == PLAYERTYPES::STATUS_IDLE)
        return PLAYERTYPES::STATUS_GUARD_FINISH;

    if (status == PLAYERTYPES::STATUS_ATTACK_A ||
        status == PLAYERTYPES::STATUS_ATTACK_B)
    {
        if (status == PLAYERTYPES::STATUS_ATTACK_A &&
			GetStatus() == PLAYERTYPES::STATUS_RUN &&
            m_pStateMachine->GetStatusDuration() >= 1.0f)
        {
            return PLAYERTYPES::STATUS_ATTACK_RUN;
        };

		if (GetStatus() == PLAYERTYPES::STATUS_ATTACK_B_CHARGE &&
			status == PLAYERTYPES::STATUS_ATTACK_B)
		{
			return PLAYERTYPES::STATUS_ATTACK_B;
		}
		else if (status == PLAYERTYPES::STATUS_ATTACK_B)
		{
			return PLAYERTYPES::STATUS_ATTACK_B_CHARGE;
		};
    };

    return status;
};


CHARACTERTYPES::DEFENCERSTATUSFLAG CPlayerCharacter::CheckDefenceStatusFlag(const CHitAttackData& rAttack)
{
    uint32 uResult = CHARACTERTYPES::DEFENCERSTATUSFLAG_NONE;

    if (IsPlayerFlagSet(PLAYERTYPES::FLAG_AERIAL_STATUS))
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_AERIAL);

    if (CGameData::Record().Secret().GetDefenceLevel() >= 3)
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_GROWUP);

    switch (GetStatus())
    {
    case PLAYERTYPES::STATUS_GUARD_READY:
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD);
        
        if (CGameData::Record().Secret().GetDefenceLevel() >= 1)
            FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_STATUS_SHIFT);        
        break;

    case PLAYERTYPES::STATUS_GUARD:
    case PLAYERTYPES::STATUS_GUARD_BACK:
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD);
        break;

    case PLAYERTYPES::STATUS_DINDLE:
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_DINDLE);
        break;

    case PLAYERTYPES::STATUS_STUN:
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_STUN);
        break;

    case PLAYERTYPES::STATUS_SLEEP:
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_SLEEP);
        break;

    case PLAYERTYPES::STATUS_FREEZE:
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_FREEZE);
        break;        
    };

    if (!IsThrowable())
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW);

    if (IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_DEFENCE_POWER_UP))
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_ITEMUP);

    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_HEALTH))
        FLAG_SET(uResult, CHARACTERTYPES::DEFENCERSTATUSFLAG_CHEATCODE_HEALTH);

    return CHARACTERTYPES::DEFENCERSTATUSFLAG(uResult);
};


void CPlayerCharacter::Initialize(PARAMETER* pParameter)
{
    ASSERT(pParameter);

    CCharacter::Initialize(&pParameter->m_chrparameter);

    std::memcpy(&m_feature, &pParameter->m_feature, sizeof(m_feature));

	m_pStateMachine = pParameter->m_pStateMachine;
	ASSERT(m_pStateMachine);

    SetDefaultFlags();    
    m_attribute = PLAYERTYPES::ATTRIBUTE_NONE;
    SetEnableBodyHit(false);

    CCircleShadowModule* pModule = (CCircleShadowModule*)GetModule(MODULETYPE::CIRCLE_SHADOW);
    if (pModule)
        pModule->SetEnable(false);
    
    m_bActive = false;
    ChangeStatus(PLAYERTYPES::STATUS_IDLE);
};


void CPlayerCharacter::OnAttach(CPlayerCharacter* pBeforeCharacter, bool bChangeEffectEnable)
{
    ASSERT(pBeforeCharacter);

    pBeforeCharacter->GetPosition(&m_vPosition);
	SetDirection(pBeforeCharacter->GetDirection());

    CToGimmickMessageModule* pToGimmickMod = (CToGimmickMessageModule*)GetModule(MODULETYPE::TO_GIMMICK_MSG);
    if (pToGimmickMod)
    {
        const COLLISIONGROUNDINFO* pGroundInfo = pBeforeCharacter->GetCollisionGround();
        ASSERT(pGroundInfo);

        if (pGroundInfo->m_bHit &&
            pGroundInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK)
        {
            pToGimmickMod->SetPrevGimmickObjName(pGroundInfo->m_gimmickinfo.m_szGimmickObjName);
        }
        else
        {
            pToGimmickMod->SetPrevGimmickObjName();
        };
    };

    PLAYERTYPES::ATTRIBUTETIME attributetime;
    pBeforeCharacter->GetAttributeTime(&attributetime);
    SetAttributeTime(&attributetime);
    SetEnableBodyHit(true);

    CPlayerAttributeControlModule* pConfusionCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_CONFUSION);
    if (pConfusionCtrlMod)
        pConfusionCtrlMod->Disable();

    SetEffectEnable(true);
    
    if (bChangeEffectEnable)
    {
        RwV3d vOffset = Math::VECTOR3_ZERO;
        CEffectManager::PlayTrace(EFFECTID::ID_BARRIER_START, new CPlayerTracer(this), &vOffset);
    };

    CBandanaModule* pBandanaMod = (CBandanaModule*)GetModule(MODULETYPE::BANDANA);
    if (pBandanaMod)
    {
        UpdateModel();
        RwV3d vPosition = *m_pModel->GetBonePositionFromID(CHARACTERTYPES::BONEID_HEAD);
        pBandanaMod->Reset(&vPosition);
    };

    CCircleShadowModule* pCircleShadowMod = (CCircleShadowModule*)GetModule(MODULETYPE::CIRCLE_SHADOW);
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(true);

    CPlayerChangeVoiceModule* pPlayerChngVoiceMod = (CPlayerChangeVoiceModule*)GetModule(MODULETYPE::PLAYER_CHNG_VOICE);
    if (pPlayerChngVoiceMod)
        pPlayerChngVoiceMod->Start();

    m_nPlayerNo = GetPlayerIndex();
    m_bActive = true;
};


void CPlayerCharacter::OnDetach(CPlayerCharacter* pAfterCharacter)
{
    ASSERT(pAfterCharacter);

    SetEnableBodyHit(false);
    
    pAfterCharacter->SetPosition(&m_vPosition);
    pAfterCharacter->SetDirection(m_fDirection);
    
    SetEffectEnable(false);

    CCircleShadowModule* pCircleShadowMod = (CCircleShadowModule*)GetModule(MODULETYPE::CIRCLE_SHADOW);
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(false);

    CLocusModule* pLocusMod = (CLocusModule*)GetModule(MODULETYPE::LOCUS);
    if (pLocusMod)
        pLocusMod->Reset();

    m_nPlayerIndex = -1;
    m_bActive = false;
};


void CPlayerCharacter::RotateDirection(float fDirection)
{
    float fRotateDirectionMax = CGameProperty::GetElapsedTime() * Math::PI05;
    float fDirectionDiff = fDirection - m_fDirection;

    fDirectionDiff = Math::RadianCorrect(fDirectionDiff);    
    ASSERT(Math::FAbs(fDirectionDiff) <= Math::PI);
    fDirectionDiff = Clamp(fDirectionDiff, -fRotateDirectionMax, fRotateDirectionMax);

    SetDirection(m_fDirection + fDirectionDiff);
};


void CPlayerCharacter::Relocation(const RwV3d* pvPosition, float fDirection, bool bProtect)
{
    ASSERT(pvPosition);
    
    SetConfusionEnable(false, 0.0f);

    ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    ClearSatusParameter();
    SetPosition(pvPosition);
    SetDirection(fDirection);
    
    SetBlinkEnable(true, 3.0f);
    SetInvincibilityEnable(true, 3.0f);    
};


void CPlayerCharacter::GenerateItemEffect(ITEMID::VALUE idItem)
{
    RwV3d vOffset = Math::VECTOR3_ZERO;
    
    switch (idItem)
    {
    case ITEMID::ID_HEAL_SMALL:
        CEffectManager::PlayTrace(EFFECTID::ID_I_RECOVER_S, new CPlayerTracer(this), &vOffset);
        CMessageManager::Request(SEGROUPID::VALUE(35), m_idPlayer);
        break;
        
    case ITEMID::ID_HEAL_FULL:
        CEffectManager::PlayTrace(EFFECTID::ID_I_RECOVER_B, new CPlayerTracer(this), &vOffset);
        CMessageManager::Request(SEGROUPID::VALUE(36), m_idPlayer);
        break;
        
    case ITEMID::ID_MISSILE:
        CMessageManager::Request(SEGROUPID::VALUE(37), m_idPlayer);
        break;
        
    case ITEMID::ID_CRY_RED:
        CMessageManager::Request(SEGROUPID::VALUE(38), m_idPlayer);
        SetAttackPowerupEnable(true, 15.0f);
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_ATTACK, 15.0f)
        );                
        break;
        
    case ITEMID::ID_CRY_GREEN:
        CMessageManager::Request(SEGROUPID::VALUE(39), m_idPlayer);
        SetDefencePowerupEnable(true, 15.0f);
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_DEFENCE, 15.0f)
        );        
        break;
        
    case ITEMID::ID_CRY_WHITE:
        CMessageManager::Request(SEGROUPID::VALUE(40), m_idPlayer);
        SetKnifePowerupEnable(true, 15.0f);
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_KNIFE, 15.0f)
        );
        break;
        
    case ITEMID::ID_CRY_ORANGE:
        CMessageManager::Request(SEGROUPID::VALUE(40), m_idPlayer);
        SetChargePowerupEnable(true, 15.0f);
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_CHARGE, 15.0f)
        );
        CMessageManager::Request(SEGROUPID::VALUE(41), m_idPlayer);
        break;
        
    case ITEMID::ID_ANTIQUE:
        CMessageManager::Request(SEGROUPID::VALUE(42), m_idPlayer);
        break;
        
    case ITEMID::ID_INVINCIBLE:
        SetInvincibilityEnable(true, 15.0f);
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_METUKEI, 15.0f)
        );
        CMessageManager::Request(SEGROUPID::VALUE(43), m_idPlayer);
        break;
        
    case ITEMID::ID_COMEBACK:
        SetBlinkEnable(true, 3.0f);
        SetInvincibilityEnable(true, 3.0f);
        break;
    };
};


void CPlayerCharacter::SetPlayerNo(int32 nPlayerNo)
{
    ASSERT(nPlayerNo >= 0 && nPlayerNo < GAMETYPES::PLAYERS_MAX);
    
    m_nPlayerNo = nPlayerNo;
};


void CPlayerCharacter::SetPadID(int32 nPadID)
{
    m_nPadID = nPadID;
};


void CPlayerCharacter::NotifyWallJumpSuccess(void)
{
    ++m_nNumWallJump;
};


void CPlayerCharacter::SetBlinkEnable(bool bEnable, float fDuration)
{
    CBlinkCharacterModule* pBlinkCtrlMod = (CBlinkCharacterModule*)GetModule(MODULETYPE::BLINK_CHARACTER);
    if (pBlinkCtrlMod)
    {
        if (bEnable)
            pBlinkCtrlMod->Start(fDuration);
        else
            pBlinkCtrlMod->End();
    };
};


void CPlayerCharacter::SetInvincibilityEnable(bool bEnable, float fDuration)
{
    CPlayerAttributeControlModule* pInvinCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY);
    if (pInvinCtrlMod)
    {
        if (bEnable)
            pInvinCtrlMod->Enable(fDuration);
        else
            pInvinCtrlMod->Disable();
    };
};


void CPlayerCharacter::SetAttackPowerupEnable(bool bEnable, float fDuration)
{
    CPlayerAttributeControlModule* pAttackCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP);
    if (pAttackCtrlMod)
    {
        if (bEnable)
            pAttackCtrlMod->Enable(fDuration);
        else
            pAttackCtrlMod->Disable();
    };
};


void CPlayerCharacter::SetDefencePowerupEnable(bool bEnable, float fDuration)
{
    CPlayerAttributeControlModule* pDefenceCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP);
    if (pDefenceCtrlMod)
    {
        if (bEnable)
            pDefenceCtrlMod->Enable(fDuration);
        else
            pDefenceCtrlMod->Disable();
    };
};


void CPlayerCharacter::SetChargePowerupEnable(bool bEnable, float fDuration)
{
    CPlayerAttributeControlModule* pChargeCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT);
    if (pChargeCtrlMod)
    {
        if (bEnable)
            pChargeCtrlMod->Enable(fDuration);
        else
            pChargeCtrlMod->Disable();
    };
};


void CPlayerCharacter::SetKnifePowerupEnable(bool bEnable, float fDuration)
{
    CPlayerAttributeControlModule* pKnifeCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE);
    if (pKnifeCtrlMod)
    {
        if (bEnable)
            pKnifeCtrlMod->Enable(fDuration);
        else
            pKnifeCtrlMod->Disable();
    };
};


void CPlayerCharacter::SetConfusionEnable(bool bEnable, float fDuration)
{
    CPlayerAttributeControlModule* pConfusionCtrlMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_CONFUSION);
    if (pConfusionCtrlMod)
    {
        if (bEnable)
            pConfusionCtrlMod->Enable(fDuration);
        else
            pConfusionCtrlMod->Disable();            
    };
};


void CPlayerCharacter::SetAmbientLightEnable(bool bEnable)
{
    CAmbientLightModule* pAmbientLightMod = (CAmbientLightModule*)GetModule(MODULETYPE::AMBIENT_LIGHT);
    if (pAmbientLightMod)
        pAmbientLightMod->SetEnable(bEnable);
};


void CPlayerCharacter::SetEffectEnable(bool bEnable)
{
    CRippleEffectModule* pRippleEffMod = (CRippleEffectModule*)GetModule(MODULETYPE::RIPPLE);
    if (pRippleEffMod)
        pRippleEffMod->SetEnable(bEnable);

    MODULETYPE::VALUE aItemEffModule[4] =
    {
        MODULETYPE::ITEM_EFFECT_ATTACK,
        MODULETYPE::ITEM_EFFECT_CHARGE,
        MODULETYPE::ITEM_EFFECT_DEFENCE,
        MODULETYPE::ITEM_EFFECT_DART,
    };

    for (int32 i = 0; i < COUNT_OF(aItemEffModule); ++i)
    {
        CharacterItemEffectModule::SetItemEffectModuleEnable(
            this,
            aItemEffModule[i],
            bEnable
        );
    };
};


void CPlayerCharacter::SetLastGroundingStatus(PLAYERTYPES::STATUS status)
{
    m_lastGrondingStatus = status;
};


PLAYERTYPES::STATUS CPlayerCharacter::GetLastGroundingStatus(void) const
{
    return m_lastGrondingStatus;
};


PLAYERTYPES::STATUS CPlayerCharacter::ChangeStatus(PLAYERTYPES::STATUS status)
{
    ASSERT(m_pStateMachine);
    
    status = RequesStatusMorphing(status);
    m_pStateMachine->ChangeStatus(status);
    
    return m_pStateMachine->CurrentStatus();
};


void CPlayerCharacter::ChangeKnockStatus(void)
{
    ChangeStatus(
        m_attackparameter.m_direction == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT ?
        PLAYERTYPES::STATUS_KNOCK_FRONT : PLAYERTYPES::STATUS_KNOCK_BACK
    );
};


void CPlayerCharacter::ChangeFlyawayStatus(void)
{
    ChangeStatus(
        PLAYERTYPES::STATUS((m_attackparameter.m_direction != CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT) + PLAYERTYPES::STATUS_FLYAWAY_FRONT)
    );
};


void CPlayerCharacter::ChangeThrowStatus(CCharacterAttackCalculator& calculator)
{
    CGameObject* pAttacker = calculator.GetAttacker();
    ASSERT(pAttacker);
    ASSERT(pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER);

    CCharacter* pAttackerChr = (CCharacter*)pAttacker;

    float fDiff = m_fDirection - pAttackerChr->GetDirection();

    if (fDiff <= Math::PI05 || fDiff >= Math::PI05)
        calculator.SetCatchResult(CHitCatchData::RESULT_THROWFRONT);
    else
        calculator.SetCatchResult(CHitCatchData::RESULT_THROWBACK);

    std::strcpy(m_liftinfo.m_szLiftObjectName, pAttackerChr->GetName());
};


void CPlayerCharacter::ApplyStatusEffect(void)
{
    switch (GetStatus())
    {
    case PLAYERTYPES::STATUS_FREEZE:
        m_pModuleMan->Include(CharacterEffectModule::CreateFreezeEffectModule(this));
        break;

    case PLAYERTYPES::STATUS_DINDLE:
        m_pModuleMan->Include(CharacterEffectModule::CreateDindleEffectModule(this));
        break;

    case PLAYERTYPES::STATUS_BIND:
        m_pModuleMan->Include(CharacterEffectModule::CreateBindEffectModule(this));
        break;

    case PLAYERTYPES::STATUS_SLEEP:
        m_pModuleMan->Include(CharacterEffectModule::CreateSleepEffectModule(this));
        break;

    case PLAYERTYPES::STATUS_STUN:
        m_pModuleMan->Include(CharacterEffectModule::CreateStunEffectModule(this));
        break;

    default:
        ASSERT(false);
    };
};


void CPlayerCharacter::CheckPartyAttack(CCharacterAttackCalculator& calculator)
{
    if (GetStatus() >= PLAYERTYPES::STATUS_STUN &&
        GetStatus() <= PLAYERTYPES::STATUS_SLEEP)
    {
        ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    }
    else if (GetStatus() == PLAYERTYPES::STATUS_BIND)
    {
        if (calculator.IsSliceAttack())
        {
            ChangeStatus(PLAYERTYPES::STATUS_IDLE);
        };
    };
};


void CPlayerCharacter::RequestAttackA(void)
{
    if(!IsPlayerFlagSet(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK))
    {
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, true);
    };
};


void CPlayerCharacter::RequestAttackB(void)
{
    if (!IsPlayerFlagSet(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
    {
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
        
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, true);
    };
};


void CPlayerCharacter::RequestAttackC(void)
{
    if (!IsPlayerFlagSet(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
    {
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);

        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, true);
    };
};


void CPlayerCharacter::RequestAttackConnect(void)
{
    SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A, false);
    SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B, false);
    SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C, false);
    SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, true);
};


bool CPlayerCharacter::CheckAttackConnect(PLAYERTYPES::FLAG playerflag)
{
    ASSERT(m_pStateMachine);
    
    bool bResult = false;
    
    if (!IsPlayerFlagSet(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT))
        return bResult;

    if (m_bClassicInput)
    {
        //
        //  A -> A
        //  B -> A
        //  C -> B
        //
        
        if (playerflag == PLAYERTYPES::FLAG_REQUEST_ATTACK_C)
        {
            playerflag = PLAYERTYPES::FLAG_REQUEST_ATTACK_B;
        }
        else if (playerflag == PLAYERTYPES::FLAG_REQUEST_ATTACK_B)
        {
            playerflag = PLAYERTYPES::FLAG_REQUEST_ATTACK_A;
        };
    };

    if (m_pStateMachine->GetStatusDuration() > GetNextChainMotionConnectTime())
    {
        bResult = IsPlayerFlagSet(playerflag);
        if (bResult)
            SetPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT, false);
    };

    return bResult;
};


bool CPlayerCharacter::CheckChargeTime(float fTime, PLAYERTYPES::CHARGEPHASE& chargephase)
{
    bool bResult = false;
    
	int32 l = CGameData::Record().Secret().GetChargeLevel();
    if (fTime >= 3.0f ||
        IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT) &&
		CGameData::Record().Secret().GetChargeLevel() >= 2)
    {
        m_chargephase = PLAYERTYPES::CHARGEPHASE_3RD;
        bResult = true;
    }
    else if ((fTime >= 2.0f ||
        IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT)) &&
        CGameData::Record().Secret().GetChargeLevel() >= 1)
    {
        m_chargephase = PLAYERTYPES::CHARGEPHASE_2ND;
		bResult = (CGameData::Record().Secret().GetChargeLevel() < 2);
    }
    else if (fTime >= 1.0f ||
        IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT))
    {
        m_chargephase = PLAYERTYPES::CHARGEPHASE_1ST;
		bResult = (CGameData::Record().Secret().GetChargeLevel() < 1);
    };

    chargephase = m_chargephase;
    
    return bResult;
};


void CPlayerCharacter::ClearCharge(void)
{
    m_chargephase = PLAYERTYPES::CHARGEPHASE_ZERO;
};


void CPlayerCharacter::ClearSatusParameter(void)
{
    SetDefaultFlags();
    m_vVelocity = Math::VECTOR3_ZERO;
    m_vAcceleration = Math::VECTOR3_ZERO;
    m_nNumWallJump = 0;
};


void CPlayerCharacter::SetDefaultFlags(void)
{
    m_playerflag = PLAYERTYPES::FLAG_NONE;

    FLAG_CLEAR(m_characterflag, CHARACTERTYPES::FLAG_FIXED_DIRECTION);
    FLAG_CLEAR(m_characterflag, CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION);
    FLAG_CLEAR(m_characterflag, CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ);
    FLAG_CLEAR(m_characterflag, CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
};


void CPlayerCharacter::ReplayMotion(void)
{
    SetMotionTime(0.0f);
    PlayMotion();
};


void CPlayerCharacter::SetAerialMotionTime(float fTime)
{
    float fT = Clamp(m_vVelocity.y / fTime, -1.0f, 1.0f);
    
    SetMotionTime(
        GetMotionEndTime() * (fT * -0.5f + 0.5f)
    );
};


void CPlayerCharacter::SetAttributeTime(PLAYERTYPES::ATTRIBUTETIME* pAttributeTime)
{
    ASSERT(pAttributeTime);

    CPlayerAttributeControlModule* pCtrlAttackMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP);
    if (pCtrlAttackMod)
        pCtrlAttackMod->SetRemainTime(pAttributeTime->m_fAttackPowerup);

    CPlayerAttributeControlModule* pCtrlDefenceMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP);
    if (pCtrlDefenceMod)
        pCtrlDefenceMod->SetRemainTime(pAttributeTime->m_fDefencePowerup);

    CPlayerAttributeControlModule* pCtrlChargeMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT);
    if (pCtrlChargeMod)
        pCtrlChargeMod->SetRemainTime(pAttributeTime->m_fChargePowerup);

    CPlayerAttributeControlModule* pCtrlKnifeMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE);
    if (pCtrlKnifeMod)
        pCtrlKnifeMod->SetRemainTime(pAttributeTime->m_fKnifePowerup);

    CPlayerAttributeControlModule* pCtrlInvinMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY);
    if (pCtrlInvinMod)
        pCtrlInvinMod->SetRemainTime(pAttributeTime->m_fInvincibility);

    CBlinkCharacterModule* pCtrlBlinkMod = (CBlinkCharacterModule*)GetModule(MODULETYPE::BLINK_CHARACTER);
    if (pCtrlAttackMod)
        pCtrlBlinkMod->SetRemainTime(pAttributeTime->m_fBlink);


    if (pAttributeTime->m_fAttackPowerup > 0.0f)
    {
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_ATTACK, pAttributeTime->m_fAttackPowerup)
        );
    };

    if (pAttributeTime->m_fDefencePowerup > 0.0f)
    {
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_DEFENCE, pAttributeTime->m_fDefencePowerup)
        );
    };

    if (pAttributeTime->m_fChargePowerup > 0.0f)
    {
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_CHARGE, pAttributeTime->m_fChargePowerup)
        );
    };

    if (pAttributeTime->m_fKnifePowerup > 0.0f)
    {
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_DEFENCE, pAttributeTime->m_fKnifePowerup)
        );
    };

    if (pAttributeTime->m_fInvincibility > 0.0f && pAttributeTime->m_bItemEffectExists)
    {
        m_pModuleMan->Include(
            CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_METUKEI, pAttributeTime->m_fInvincibility)
        );
    };
};


void CPlayerCharacter::GetAttributeTime(PLAYERTYPES::ATTRIBUTETIME* pAttributeTime)
{
    ASSERT(pAttributeTime);

    CPlayerAttributeControlModule* pCtrlAttackMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP);
    if (pCtrlAttackMod)
        pAttributeTime->m_fAttackPowerup = pCtrlAttackMod->GetRemainTime();

    CPlayerAttributeControlModule* pCtrlDefenceMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP);
    if (pCtrlDefenceMod)
        pAttributeTime->m_fDefencePowerup = pCtrlDefenceMod->GetRemainTime();

    CPlayerAttributeControlModule* pCtrlChargeMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT);
    if (pCtrlChargeMod)
        pAttributeTime->m_fChargePowerup = pCtrlChargeMod->GetRemainTime();

    CPlayerAttributeControlModule* pCtrlKnifeMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE);
    if (pCtrlKnifeMod)
        pAttributeTime->m_fKnifePowerup = pCtrlKnifeMod->GetRemainTime();

    CPlayerAttributeControlModule* pCtrlInvinMod = (CPlayerAttributeControlModule*)GetModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY);
    if (pCtrlInvinMod)
        pAttributeTime->m_fInvincibility = pCtrlInvinMod->GetRemainTime();

    CBlinkCharacterModule* pCtrlBlinkMod = (CBlinkCharacterModule*)GetModule(MODULETYPE::BLINK_CHARACTER);
    if (pCtrlAttackMod)
        pAttributeTime->m_fBlink = pCtrlAttackMod->GetRemainTime();

    if (IsModuleIncluded(MODULETYPE::ITEM_EFFECT_INVINCIBLE))
    {
        pAttributeTime->m_bItemEffectExists = true;
        CharacterItemEffectModule::SetItemEffectModuleEnable(
            this,
            MODULETYPE::ITEM_EFFECT_INVINCIBLE,
            false
        );
    }
    else
    {
        pAttributeTime->m_bItemEffectExists = false;
    };
};


PLAYERTYPES::STATUS CPlayerCharacter::GetStatus(void) const
{
    ASSERT(m_pStateMachine);
    return m_pStateMachine->CurrentStatus();
};


PLAYERTYPES::STATUS CPlayerCharacter::GetStatusPrev(void) const
{
    ASSERT(m_pStateMachine);
    return m_pStateMachine->PrevStatus();
};


PLAYERID::VALUE CPlayerCharacter::GetID(void) const
{
    return m_idPlayer;
};


int32 CPlayerCharacter::GetNumWallJump(void) const
{
    return m_nNumWallJump;
};


int32 CPlayerCharacter::GetPadID(void) const
{
    return m_nPadID;
};


int32 CPlayerCharacter::GetPlayerNo(void) const
{
    return m_nPlayerNo;
};


void CPlayerCharacter::GetReplacePosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);

    *pvPosition = m_vReplacepoint;
};


void CPlayerCharacter::SetReplacePosition(RwV3d* pvPosition)
{
    ASSERT(pvPosition);

    m_vReplacepoint = *pvPosition;
};


bool CPlayerCharacter::IsEnableCharacterChanging(void) const
{
    return (GetStatus() == PLAYERTYPES::STATUS_IDLE);        
};


bool CPlayerCharacter::IsEnableJump2nd(void) const
{
    return IsPlayerFlagSet(PLAYERTYPES::FLAG_JUMP_2ND);
};


bool CPlayerCharacter::IsEnableJumpWall(void) const
{
    if (m_collisionWallInfo.m_bJumpWallHit)
    {
        if (m_collisionWallInfo.m_attribute == MAPTYPES::ATTRIBUTE_JUMP ||
            m_collisionWallInfo.m_attribute == MAPTYPES::ATTRIBUTE_MISSJUMP)
            return true;
    };

    return false;
};


bool CPlayerCharacter::IsEnableAttackJump(void) const
{
    return IsPlayerFlagSet(PLAYERTYPES::FLAG_AERIAL_STATUS);
};


bool CPlayerCharacter::IsEnableAttackKnife(void) const
{
    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_SHURIKEN))
        return true;
    
    if (CGameProperty::Player(m_nPlayerNo)->GetShurikenNum() <= 0)
        return false;

    return !IsPlayerFlagSet(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
};


bool CPlayerCharacter::IsEnableAttackKnifeJump(void) const
{
    ASSERT(m_pStateMachine);

    bool bIsTechLevelOK         = (CGameData::Record().Secret().GetAerialLevel() >= 1);
    bool bIsEnableAttackKnife   = IsEnableAttackKnife();
    bool bIsInAerial            = IsPlayerFlagSet(PLAYERTYPES::FLAG_AERIAL_STATUS);
    bool bIsNotSameStatus       = (GetStatus() != PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP);
    bool bIsNotOnCooldown       = (m_pStateMachine->GetStatusDuration() > 0.15f);

    return (bIsTechLevelOK && bIsEnableAttackKnife && bIsInAerial && (bIsNotSameStatus || bIsNotOnCooldown));
};


void CPlayerCharacter::SetPlayerFlag(PLAYERTYPES::FLAG flag, bool bSet)
{
    if (bSet)
        FLAG_SET(m_playerflag, flag);
    else
        FLAG_CLEAR(m_playerflag, flag);
};


void CPlayerCharacter::SetAttributeFlag(PLAYERTYPES::ATTRIBUTE flag, bool bSet)
{
    if (bSet)
        FLAG_SET(m_attribute, flag);
    else
        FLAG_CLEAR(m_attribute, flag);
};


bool CPlayerCharacter::IsPlayerFlagSet(PLAYERTYPES::FLAG flag) const
{
    return FLAG_TEST(m_playerflag, flag);
};


bool CPlayerCharacter::IsAttributeFlagSet(PLAYERTYPES::ATTRIBUTE flag) const
{
    return FLAG_TEST(m_attribute, flag);
};


PLAYERTYPES::FLAG CPlayerCharacter::GetPlayerFlag(void) const
{
    return PLAYERTYPES::FLAG(m_playerflag);
};


PLAYERTYPES::ATTRIBUTE CPlayerCharacter::GetAttributeFlag(void) const
{
    return PLAYERTYPES::ATTRIBUTE(m_attribute);
};


bool CPlayerCharacter::IsThrowable(void) const
{
    switch (GetStatus())
    {
    case PLAYERTYPES::STATUS_IDLE:
    case PLAYERTYPES::STATUS_WALK:
    case PLAYERTYPES::STATUS_RUN:
    case PLAYERTYPES::STATUS_GUARD_READY:
    case PLAYERTYPES::STATUS_GUARD:
    case PLAYERTYPES::STATUS_GUARD_FINISH:
    case PLAYERTYPES::STATUS_ATTACK_A:
    case PLAYERTYPES::STATUS_ATTACK_AA:
    case PLAYERTYPES::STATUS_ATTACK_AAB:
    case PLAYERTYPES::STATUS_ATTACK_AAC:
    case PLAYERTYPES::STATUS_ATTACK_AABB:
    case PLAYERTYPES::STATUS_ATTACK_AABC:
    case PLAYERTYPES::STATUS_ATTACK_AABBB:
    case PLAYERTYPES::STATUS_ATTACK_AABBC:
    case PLAYERTYPES::STATUS_ATTACK_B:
    case PLAYERTYPES::STATUS_ATTACK_B_CHARGE:
    case PLAYERTYPES::STATUS_ATTACK_RUN:
    case PLAYERTYPES::STATUS_ATTACK_JUMP:
    case PLAYERTYPES::STATUS_ATTACK_KNIFE:
    case PLAYERTYPES::STATUS_KNOCK_FRONT:
    case PLAYERTYPES::STATUS_KNOCK_BACK:
    case PLAYERTYPES::STATUS_DINDLE:
    case PLAYERTYPES::STATUS_STUN:
    case PLAYERTYPES::STATUS_SLEEP:
    case PLAYERTYPES::STATUS_PUSH:
        return true;

    default:
        return false;
    };
};


const PLAYERTYPES::FEATURE& CPlayerCharacter::Feature(void) const
{
    return m_feature;
};


CHARACTERTYPES::ATTACKPARAMETER& CPlayerCharacter::AttackParameter(void)
{
    return m_attackparameter;
};


PLAYERTYPES::CHARGEPHASE CPlayerCharacter::GetChargePhase(void) const
{
    return m_chargephase;
};


bool CPlayerCharacter::IsEnableChangeStatus(PLAYERTYPES::STATUS status) const
{
    ASSERT(m_pStateMachine);
    return m_pStateMachine->IsEnableChangeStatus(status);
};