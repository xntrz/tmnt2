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


#ifdef _DEBUG

static inline bool IsDuplicateCharacterExist(const char* pszName)
{
    CGameObject* pObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::CHARACTER);
    while (pObj)
    {
        const char* pszObjName = pObj->GetName();
        if (!std::strcmp(pszObjName, pszName))
            return true;

        pObj = CGameObjectManager::GetNext(GAMEOBJECTTYPE::CHARACTER, pObj);
    };

    return false;
};


static const char* CorrectNameFromDuplicate(const char* pszName)
{
    /*
     *  There is able to create multiple characters of same type but it should have different name
     *  to avoid broke some mechanics that relate on object find by name (e.g. party throw)
     */

    static char s_szNameCorrectionBuff[GAMEOBJECTTYPES::NAME_MAX];
    s_szNameCorrectionBuff[0] = '\0';

    std::strcpy(s_szNameCorrectionBuff, pszName);

    int32 number = 0;
    while (true)
    {
        if (!IsDuplicateCharacterExist(s_szNameCorrectionBuff))
            break;
        
        std::sprintf(s_szNameCorrectionBuff, "%s_%" PRIi32, s_szNameCorrectionBuff, ++number);
    };

    return s_szNameCorrectionBuff;
};

#else /* _DEBUG */

static const char* CorrectNameFromDuplicate(const char* pszName)
{
    return pszName;
};

#endif /* _DEBUG */


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
: CCharacter(CorrectNameFromDuplicate(pszName), TYPE_PLAYER)
, m_idPlayer(idPlayer)
, m_costume(costume)
, m_nPlayerIndex(-1)
, m_nPlayerNo(-1)
, m_nPadID(-1)
, m_nNumWallJump(0)
, m_attackparameter({ 0 })
, m_attribute(PLAYERTYPES::ATTRIBUTE_NONE)
, m_pflag(PLAYERTYPES::FLAG_NONE)
, m_pStateMachine(nullptr)
, m_chargephase(PLAYERTYPES::CHARGEPHASE_ZERO)
, m_feature({ 0 })
, m_vReplacepoint(Math::VECTOR3_ZERO)
, m_bActive(false)
{
	;
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

	m_pModuleMan->Include(CAccumulateModule::New(this));
	m_pModuleMan->Include(CToGimmickMessageModule::New(this));
    m_pModuleMan->Include(new CBlinkCharacterModule(this));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY,
                                                            PLAYERTYPES::ATTRIBUTE_INVINCIBILITY,
                                                            this));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP,
                                                            PLAYERTYPES::ATTRIBUTE_ATTACK_POWER_UP,
                                                            this));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP,
                                                            PLAYERTYPES::ATTRIBUTE_DEFENCE_POWER_UP,
                                                            this));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT,
                                                            PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT,
                                                            this));

    m_pModuleMan->Include(new CPlayerAttributeControlModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE,
                                                            PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE,
                                                            this));

    m_pModuleMan->Include(new CConfusionAttributeControlModule(MODULETYPE::PLAYERATTRCTRL_CONFUSION, this));

    m_pModuleMan->Include(CFootprintsModule::New(this));
    m_pModuleMan->Include(new CRippleEffectModule(this, 1.5f));
    m_pModuleMan->Include(new CWallJumpSignModule(this));
    m_pModuleMan->Include(new CDamageFloorModule(this));
    m_pModuleMan->Include(new CPlayerFootfallModule(this));
    m_pModuleMan->Include(new CPlayerChangeVoiceModule(this));
};


void CPlayerCharacter::CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame)
{
    CCharacter::CalcVelocityPerFrame(rvVelocityPerFrame);
    
    RwV3d vecBeforePos = Math::VECTOR3_ZERO;
    GetFootPosition(&vecBeforePos);

    RwV3d vecAfterPos = Math::VECTOR3_ZERO;    
    Math::Vec3_Add(&vecAfterPos, &vecBeforePos, &rvVelocityPerFrame);

    if (CGameProperty::AdjustWithinMoveBoundary(&vecAfterPos, &vecAfterPos))
    {
        rvVelocityPerFrame.x = (vecAfterPos.x - vecBeforePos.x);
        rvVelocityPerFrame.z = (vecAfterPos.z - vecBeforePos.z);
    };
};


bool CPlayerCharacter::OnMessageCanChangingAerial(float fFieldHeight)
{
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
        return false;

    case PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT:
        ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_FRONT);
        return false;

    case PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK:
        ChangeStatus(PLAYERTYPES::STATUS_FLYAWAY_BACK);
        return false;

    default:
        return (!TestPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS));
    };
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
		m_nNumWallJump = 0;
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

    default:
        break;
    };

    if (GetStatus() != PLAYERTYPES::STATUS_CAUGHT)
    {
        if (m_iDamageRequest > 0)
        {
            CGameObjectManager::SendMessage(this,
                                            CHARACTERTYPES::MESSAGEID_RECVDMG,
                                            reinterpret_cast<void*>(m_iDamageRequest));
            m_iDamageRequest = 0;
        };
    };
};


void CPlayerCharacter::OnMessageAttackResult(CHitCatchData* pCatch)
{
    CHitCatchData::RESULT result = pCatch->GetResult();

    if ((result == CHitCatchData::RESULT_THROWBACK) ||
        (result == CHitCatchData::RESULT_THROWFRONT))
    {
        if ((GetStatus() == PLAYERTYPES::STATUS_LIFT_CHALLENGE) ||
            ((m_idPlayer == PLAYERID::ID_RAP) && (GetStatus() == PLAYERTYPES::STATUS_ATTACK_AAC)))
        {
            CGameObject* pDefender = pCatch->GetObject();
            ASSERT(pDefender);
            
            m_liftinfo.m_bLiftSuccess = true;
            std::strcpy(m_liftinfo.m_szLiftObjectName, pDefender->GetName());
            m_liftinfo.m_iStatus = static_cast<int32>(result);

            if ((result == CHitCatchData::RESULT_THROWFRONT) &&
                (pDefender->GetType() == GAMEOBJECTTYPE::CHARACTER))
            {
                CCharacter* pDefenderChr = static_cast<CCharacter*>(pDefender);

                if (pDefenderChr->GetAttackCharacterType() == CCharacter::TYPE_ENEMY)
                    pDefenderChr->RequestDamage(pCatch->GetAttack()->GetPower());
            };
        };
    };

    if (result == CHitCatchData::RESULT_HIT)
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

                default:
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

        default:
            break;
        };
    };

    if (result == CHitCatchData::RESULT_HIT)
    {
        if ((GetStatus() >= PLAYERTYPES::STATUS_ATTACK_AABBB) &&
            (GetStatus() <= PLAYERTYPES::STATUS_ATTACK_AABBC))
        {
            CPlayerChangeVoiceModule* pModule = static_cast<CPlayerChangeVoiceModule*>(GetModule(MODULETYPE::PLAYER_CHNG_VOICE));
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

    if (TestAttribute(PLAYERTYPES::ATTRIBUTE_ATTACK_POWER_UP))
        iPower += iPower;

    if (CGameData::Record().Secret().GetAttackLevel() >= 3)
        iPower += iPower;

    float fAttackEnhance = CGameData::Record().Secret().GetAttackEnhanceValue();
    iPower = static_cast<int32>(fAttackEnhance * static_cast<float>(iPower));
    
    rAttack.SetPower(iPower);
};


CHARACTERTYPES::ATTACKRESULTTYPE CPlayerCharacter::OnDamage(CCharacterAttackCalculator& calculator)
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceFlag = CheckDefenceStatusFlag(calculator.GetAttack());
    CHARACTERTYPES::ATTACKRESULTTYPE attackResult = calculator.CalcAtackParameter(m_attackparameter, defenceFlag);

    if (TestCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING) ||
        TestAttribute(PLAYERTYPES::ATTRIBUTE_INVINCIBILITY))
        return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;

    switch (attackResult)
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
        break;

    default:
        break;
    };

    if (!CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_HEALTH) &&
        calculator.IsConfusionAttack() &&
        (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD) &&
        (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT))
    {
        SetConfusionEnable(true, 4.0f);
        IGamepad::StartVibration(GetPadID(), IGamepad::VIBRATIONTYPE_NORMAL, 0.2f);
    };

    if (attackResult &&
        (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_THROW) &&
        (attackResult != CHARACTERTYPES::ATTACKRESULTTYPE_PARTY_ATTACK))
    {
        CGameEvent::SetPlayerDamaged(m_nPlayerNo);
    };

    if (attackResult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT)
    {
        CGameEvent::SetPlayerTechnicalAction(m_nPlayerNo, GAMETYPES::TECACT_GUARD_IMPACT);
        PlayerUtil::CallVoiceOfGuardImpact(this);
    };

    return attackResult;
};


void CPlayerCharacter::OnMessageCatch(void* pParam)
{
    ChangeStatus(PLAYERTYPES::STATUS_CAUGHT);
};


void CPlayerCharacter::OnMessageLift(void* pParam)
{
    MSG_LIFT_INFO* pMsgLiftInfo = static_cast<MSG_LIFT_INFO*>(pParam);
    ASSERT(pMsgLiftInfo);

    SetCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    SetDirection(pMsgLiftInfo->m_fDirection + MATH_PI);

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
    RwV3d* pvVelocity = static_cast<RwV3d*>(pParam);
    ASSERT(pvVelocity);

    ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
    SetVelocity(pvVelocity);
    ChangeStatus(PLAYERTYPES::STATUS_THROWN);
};


void CPlayerCharacter::OnMessageMissThrow(void* pParam)
{
    RwV3d* pvVelocity = static_cast<RwV3d*>(pParam);
    ASSERT(pvVelocity);

    ClearCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY);
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

    if (m_collisionGroundInfo.m_attribute == MAPTYPES::ATTRIBUTE_DOBON)
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

    CShotManager::Shot(SHOTID::ID_KNIFE_PLAYER, &vPosition, m_fDirection, this, MATH_DEG2RAD(15.0f), 5.0f);

    if (!TestAttribute(PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE))
        CGameProperty::Player(m_nPlayerNo)->AddShurikenNum(-1);
};


PLAYERTYPES::STATUS CPlayerCharacter::RequestStatusMorphing(PLAYERTYPES::STATUS statusNext)
{
    PLAYERTYPES::STATUS statusCurr = GetStatus();

    switch (statusNext)
    {
	case PLAYERTYPES::STATUS_JUMP_WALL:
		++m_nNumWallJump;
		break;

    case PLAYERTYPES::STATUS_JUMP:
        if ((statusCurr == PLAYERTYPES::STATUS_IDLE) ||
            (statusCurr == PLAYERTYPES::STATUS_WALK) ||
            (statusCurr == PLAYERTYPES::STATUS_RUN))
        {
            return PLAYERTYPES::STATUS_JUMP_READY;
        };
        break;

    case PLAYERTYPES::STATUS_GUARD:
        if ((statusCurr == PLAYERTYPES::STATUS_IDLE) ||
            (statusCurr == PLAYERTYPES::STATUS_WALK) ||
            (statusCurr == PLAYERTYPES::STATUS_RUN))
        {
            return PLAYERTYPES::STATUS_GUARD_READY;
        };
        break;

    case PLAYERTYPES::STATUS_LIFT:
        if ((statusCurr == PLAYERTYPES::STATUS_GUARD) ||
            (statusCurr == PLAYERTYPES::STATUS_GUARD_READY))
        {
            return PLAYERTYPES::STATUS_LIFT_CHALLENGE;
        };
        break;

    default:
        break;
    };

    if ((statusCurr == PLAYERTYPES::STATUS_GUARD) && (statusNext == PLAYERTYPES::STATUS_IDLE))
        return PLAYERTYPES::STATUS_GUARD_FINISH;

    if ((statusNext == PLAYERTYPES::STATUS_ATTACK_A) ||
        (statusNext == PLAYERTYPES::STATUS_ATTACK_B))
    {
        if ((statusCurr == PLAYERTYPES::STATUS_RUN) &&
            (m_pStateMachine->GetStatusDuration() >= 1.0f))
        {
            return PLAYERTYPES::STATUS_ATTACK_RUN;
        };

        if (statusNext == PLAYERTYPES::STATUS_ATTACK_B)
        {
            if (statusCurr != PLAYERTYPES::STATUS_ATTACK_B_CHARGE)
                return PLAYERTYPES::STATUS_ATTACK_B_CHARGE;
        };
    };

    return statusNext;
};


CHARACTERTYPES::DEFENCERSTATUSFLAG CPlayerCharacter::CheckDefenceStatusFlag(const CHitAttackData& rAttack)
{
    CHARACTERTYPES::DEFENCERSTATUSFLAG defenceFlag = CHARACTERTYPES::DEFENCERSTATUSFLAG_NONE;

    if (TestPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS))
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_AERIAL;

    if (CGameData::Record().Secret().GetDefenceLevel() >= 3)
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_GROWUP;

    switch (GetStatus())
    {
    case PLAYERTYPES::STATUS_GUARD_READY:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD;
        
        if (CGameData::Record().Secret().GetDefenceLevel() >= 1)
            defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_STATUS_SHIFT;
        break;

    case PLAYERTYPES::STATUS_GUARD:
    case PLAYERTYPES::STATUS_GUARD_BACK:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD;
        break;

    case PLAYERTYPES::STATUS_DINDLE:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DINDLE;
        break;

    case PLAYERTYPES::STATUS_STUN:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_STUN;
        break;

    case PLAYERTYPES::STATUS_SLEEP:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_SLEEP;
        break;

    case PLAYERTYPES::STATUS_FREEZE:
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_FREEZE;
        break;

    default:
        break;
    };

    if (!IsThrowable())
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW;

    if (TestAttribute(PLAYERTYPES::ATTRIBUTE_DEFENCE_POWER_UP))
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_ITEMUP;

    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_HEALTH))
        defenceFlag |= CHARACTERTYPES::DEFENCERSTATUSFLAG_CHEATCODE_HEALTH;

    return defenceFlag;
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

    CCircleShadowModule* pModule = static_cast<CCircleShadowModule*>(GetModule(MODULETYPE::CIRCLE_SHADOW));
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

    CToGimmickMessageModule* pToGimmickMod = static_cast<CToGimmickMessageModule*>(GetModule(MODULETYPE::TO_GIMMICK_MSG));
    if (pToGimmickMod)
    {
        const COLLISIONGROUNDINFO* pGroundInfo = pBeforeCharacter->GetCollisionGround();
        ASSERT(pGroundInfo);

        if (pGroundInfo->m_bHit && (pGroundInfo->m_hittype == MAPTYPES::HITTYPE_GIMMICK))
            pToGimmickMod->SetPrevGimmickObjName(pGroundInfo->m_gimmickinfo.m_szGimmickObjName);
        else
            pToGimmickMod->SetPrevGimmickObjName("");
    };

    PLAYERTYPES::ATTRIBUTETIME attributetime;
    pBeforeCharacter->GetAttributeTime(&attributetime);
    SetAttributeTime(&attributetime);

    SetEnableBodyHit(true);

    CPlayerAttributeControlModule* pConfusionCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_CONFUSION));
    if (pConfusionCtrlMod)
        pConfusionCtrlMod->Disable();

    SetEffectEnable(true);

    if (bChangeEffectEnable)
    {
        RwV3d vOffset = Math::VECTOR3_ZERO;
        CEffectManager::PlayTrace(EFFECTID::ID_BARRIER_START, new CPlayerTracer(this), &vOffset);
    };

    CBandanaModule* pBandanaMod = static_cast<CBandanaModule*>(GetModule(MODULETYPE::BANDANA));
    if (pBandanaMod)
    {
        UpdateModel();

        RwV3d vPosition = *m_pModel->GetBonePositionFromID(10);
        pBandanaMod->Reset(&vPosition);
    };

    CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(GetModule(MODULETYPE::CIRCLE_SHADOW));
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(true);

    CPlayerChangeVoiceModule* pPlayerChngVoiceMod = static_cast<CPlayerChangeVoiceModule*>(GetModule(MODULETYPE::PLAYER_CHNG_VOICE));
    if (pPlayerChngVoiceMod)
        pPlayerChngVoiceMod->Start();

    m_nPlayerNo = GetPlayerIndex();
    m_bActive = true;
};


void CPlayerCharacter::OnDetach(CPlayerCharacter* pAfterCharacter)
{
    SetEnableBodyHit(false);

    if (pAfterCharacter)
    {
        pAfterCharacter->SetPosition(&m_vPosition);
        pAfterCharacter->SetDirection(m_fDirection);
    };

    SetEffectEnable(false);

    CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(GetModule(MODULETYPE::CIRCLE_SHADOW));
    if (pCircleShadowMod)
        pCircleShadowMod->SetEnable(false);

    CLocusModule* pLocusMod = static_cast<CLocusModule*>(GetModule(MODULETYPE::LOCUS));
    if (pLocusMod)
        pLocusMod->Reset();

    m_nPlayerIndex = -1;
    m_bActive = false;
};


void CPlayerCharacter::RotateDirection(float fDirection)
{
    float fRotateDirectionMax = CGameProperty::GetElapsedTime() * Math::PI05;
    float fDirectionDiff = fDirection - m_fDirection;

    fDirectionDiff = Math::RadianNormalize(fDirectionDiff);
    
    ASSERT(std::fabs(fDirectionDiff) <= Math::PI);
    fDirectionDiff = Clamp(fDirectionDiff, -fRotateDirectionMax, fRotateDirectionMax);

    SetDirection(m_fDirection + fDirectionDiff);
};


void CPlayerCharacter::Relocation(const RwV3d* pvPosition, float fDirection, bool bBlink)
{
    ASSERT(pvPosition);
    
    SetConfusionEnable(false, 0.0f);

    ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    ClearStatusParameter();
    SetPosition(pvPosition);
    SetDirection(fDirection);
    
    SetBlinkEnable((bBlink ? true : false), 3.0f);
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
        SetAttackPowerupEnable(true, 15.0f);
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_ATTACK, 15.0f));
        CMessageManager::Request(SEGROUPID::VALUE(38), m_idPlayer);
        break;
        
    case ITEMID::ID_CRY_GREEN:
        SetDefencePowerupEnable(true, 15.0f);
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_DEFENCE, 15.0f));        
        CMessageManager::Request(SEGROUPID::VALUE(39), m_idPlayer);
        break;
        
    case ITEMID::ID_CRY_WHITE:
        SetKnifePowerupEnable(true, 15.0f);
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_KNIFE, 15.0f));
        CMessageManager::Request(SEGROUPID::VALUE(40), m_idPlayer);
        break;
        
    case ITEMID::ID_CRY_ORANGE:
        SetChargePowerupEnable(true, 15.0f);
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_CHARGE, 15.0f));
        CMessageManager::Request(SEGROUPID::VALUE(41), m_idPlayer);
        break;
        
    case ITEMID::ID_ANTIQUE:
        CMessageManager::Request(SEGROUPID::VALUE(42), m_idPlayer);
        break;
        
    case ITEMID::ID_INVINCIBLE:
        SetInvincibilityEnable(true, 15.0f);
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this, EFFECTID::ID_SETUP_METUKEI, 15.0f));
        CMessageManager::Request(SEGROUPID::VALUE(43), m_idPlayer);
        break;
        
    case ITEMID::ID_COMEBACK:
        SetBlinkEnable(true, 3.0f);
        SetInvincibilityEnable(true, 3.0f);
#ifdef _DEBUG        
        CMessageManager::Request(SEGROUPID::VALUE(44), m_idPlayer);
#endif /* _DEBUG */        
        break;

    default:
        break;
    };
};


void CPlayerCharacter::SetPlayerNo(int32 nPlayerNo)
{
    ASSERT(nPlayerNo >= 0);
    ASSERT(nPlayerNo < GAMETYPES::PLAYERS_MAX);

    m_nPlayerNo = nPlayerNo;
};


void CPlayerCharacter::SetPadID(int32 nPadID)
{
    m_nPadID = nPadID;
};


void CPlayerCharacter::SetBlinkEnable(bool bEnable, float fDuration)
{
    CBlinkCharacterModule* pBlinkCtrlMod = static_cast<CBlinkCharacterModule*>(GetModule(MODULETYPE::BLINK_CHARACTER));
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
    CPlayerAttributeControlModule* pInvinCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY));
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
    CPlayerAttributeControlModule* pAttackCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP));
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
    CPlayerAttributeControlModule* pDefenceCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP));
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
    CPlayerAttributeControlModule* pChargeCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT));
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
    CPlayerAttributeControlModule* pKnifeCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE));
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
    CPlayerAttributeControlModule* pConfusionCtrlMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_CONFUSION));
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
    CAmbientLightModule* pAmbientLightMod = static_cast<CAmbientLightModule*>(GetModule(MODULETYPE::AMBIENT_LIGHT));
    if (pAmbientLightMod)
        pAmbientLightMod->SetEnable(bEnable);
};


void CPlayerCharacter::SetEffectEnable(bool bEnable)
{
    CRippleEffectModule* pRippleEffMod = static_cast<CRippleEffectModule*>(GetModule(MODULETYPE::RIPPLE));
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
        CharacterItemEffectModule::SetItemEffectModuleEnable(this, aItemEffModule[i], bEnable);
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
    
    status = RequestStatusMorphing(status);
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
        m_attackparameter.m_direction == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT ?
        PLAYERTYPES::STATUS_FLYAWAY_FRONT : PLAYERTYPES::STATUS_FLYAWAY_BACK
    );
};


void CPlayerCharacter::ChangeThrowStatus(CCharacterAttackCalculator& calculator)
{
    CGameObject* pAttacker = calculator.GetAttacker();
    ASSERT(pAttacker);
    ASSERT(pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER);

    CCharacter* pAttackerChr = static_cast<CCharacter*>(pAttacker);

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
        break;
    };
};


void CPlayerCharacter::CheckPartyAttack(CCharacterAttackCalculator& calculator)
{
    if ((GetStatus() >= PLAYERTYPES::STATUS_STUN) &&
        (GetStatus() <= PLAYERTYPES::STATUS_SLEEP))
    {
        ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    }
    else if (GetStatus() == PLAYERTYPES::STATUS_BIND)
    {
        if (calculator.IsSliceAttack())
            ChangeStatus(PLAYERTYPES::STATUS_IDLE);
    };
};


void CPlayerCharacter::RequestAttackA(void)
{
    if (!CheckPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK))
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A);
};


void CPlayerCharacter::RequestAttackB(void)
{
    if (!TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
    {
        ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK);
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B);
    };
};


void CPlayerCharacter::RequestAttackC(void)
{
    if (!TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
    {
        ClearPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_MASK);
        SetPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C);
    };
};


PLAYERTYPES::FLAG CPlayerCharacter::CheckAttackConnect(PLAYERTYPES::FLAG mask)
{
    PLAYERTYPES::FLAG result = PLAYERTYPES::FLAG_NONE;

    if (TestPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT))
    {
        float fStatusDuration = m_pStateMachine->GetStatusDuration();
        float fChainMotionConnectTime = GetNextChainMotionConnectTime();

        if (fStatusDuration >= fChainMotionConnectTime)
        {
            ClearPlayerFlag(PLAYERTYPES::FLAG_ENABLE_ATTACK_CONNECT);
            
            if (TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_C))
            {
                result = PLAYERTYPES::FLAG_REQUEST_ATTACK_C;
            }
            else if (TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_B))
            {
                result = PLAYERTYPES::FLAG_REQUEST_ATTACK_B;
            }
            else if (TestPlayerFlag(PLAYERTYPES::FLAG_REQUEST_ATTACK_A))
            {
                result = PLAYERTYPES::FLAG_REQUEST_ATTACK_A;
            };

            result &= mask;
        };
    };

    return result;
};


bool CPlayerCharacter::CheckChargeTime(float fTime, PLAYERTYPES::CHARGEPHASE* chargephase)
{
    bool bResult = false;
    
    int32 chargeLevel = CGameData::Record().Secret().GetChargeLevel();

    if (((fTime >= 3.0f) || TestAttribute(PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT)) && (chargeLevel >= 2))
    {
        m_chargephase = PLAYERTYPES::CHARGEPHASE_3RD;
        bResult = true;
    }
    else if (((fTime >= 2.0f) || TestAttribute(PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT)) && (chargeLevel >= 1))
    {
        m_chargephase = PLAYERTYPES::CHARGEPHASE_2ND;
		bResult = (chargeLevel < 2);
    }
    else if ((fTime >= 1.0f) || TestAttribute(PLAYERTYPES::ATTRIBUTE_CHARGE_TIME_CUT))
    {
        m_chargephase = PLAYERTYPES::CHARGEPHASE_1ST;
		bResult = (chargeLevel < 1);
    };

    ASSERT(chargephase);
    *chargephase = m_chargephase;
    
    return bResult;
};


void CPlayerCharacter::ClearCharge(void)
{
    m_chargephase = PLAYERTYPES::CHARGEPHASE_ZERO;
};


void CPlayerCharacter::ClearStatusParameter(void)
{
    SetDefaultFlags();
    m_vVelocity     = Math::VECTOR3_ZERO;
    m_vAcceleration = Math::VECTOR3_ZERO;
    m_nNumWallJump  = 0;
};


void CPlayerCharacter::SetDefaultFlags(void)
{
    m_pflag = PLAYERTYPES::FLAG_NONE;

    CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_FIXED_DIRECTION
                               | CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION
                               | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ
                               | CHARACTERTYPES::FLAG_CANCEL_GRAVITY;
    ClearCharacterFlag(cflag);
};


void CPlayerCharacter::ReplayMotion(void)
{
    SetMotionTime(0.0f);
    PlayMotion();
};


void CPlayerCharacter::SetAerialMotionTime(float fMaxSpeed)
{
    float fTime = (m_vVelocity.y / fMaxSpeed);
    fTime = Clamp(fTime, -1.0f, 1.0f);

    float fMotionEndT = GetMotionEndTime();
    float fMotionT = fMotionEndT * (fTime * -0.5f + 0.5f);

    SetMotionTime(fMotionT);
};


void CPlayerCharacter::SetAttributeTime(PLAYERTYPES::ATTRIBUTETIME* pAttributeTime)
{
    ASSERT(pAttributeTime);

    /* set attack remain time */
    CPlayerAttributeControlModule* pCtrlAttackMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP));
    if (pCtrlAttackMod)
        pCtrlAttackMod->SetRemainTime(pAttributeTime->m_fAttackPowerup);

    if (pAttributeTime->m_fAttackPowerup > 0.0f)
    {
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this,
                                                                                EFFECTID::ID_SETUP_ATTACK,
                                                                                pAttributeTime->m_fAttackPowerup));
    };

    /* set defence remain time */
    CPlayerAttributeControlModule* pCtrlDefenceMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP));
    if (pCtrlDefenceMod)
        pCtrlDefenceMod->SetRemainTime(pAttributeTime->m_fDefencePowerup);

    if (pAttributeTime->m_fDefencePowerup > 0.0f)
    {
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this,
                                                                                EFFECTID::ID_SETUP_DEFENCE,
                                                                                pAttributeTime->m_fDefencePowerup));
    };

    /* set charge remain time */
    CPlayerAttributeControlModule* pCtrlChargeMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT));
    if (pCtrlChargeMod)
        pCtrlChargeMod->SetRemainTime(pAttributeTime->m_fChargePowerup);

    if (pAttributeTime->m_fChargePowerup > 0.0f)
    {
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this,
                                                                                EFFECTID::ID_SETUP_CHARGE,
                                                                                pAttributeTime->m_fChargePowerup));
    };

    /* set invincibility remain time */
    CPlayerAttributeControlModule* pCtrlInvinMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY));
    if (pCtrlInvinMod)
        pCtrlInvinMod->SetRemainTime(pAttributeTime->m_fInvincibility);

    if ((pAttributeTime->m_fInvincibility > 0.0f) && pAttributeTime->m_bItemEffectExists)
    {
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this,
                                                                                EFFECTID::ID_SETUP_METUKEI,
                                                                                pAttributeTime->m_fInvincibility));
    };

    /* set infinite knife remain time */
    CPlayerAttributeControlModule* pCtrlKnifeMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE));
    if (pCtrlKnifeMod)
        pCtrlKnifeMod->SetRemainTime(pAttributeTime->m_fKnifePowerup);

    if (pAttributeTime->m_fKnifePowerup > 0.0f)
    {
        m_pModuleMan->Include(CharacterItemEffectModule::CreateItemEffectModule(this,
                                                                                EFFECTID::ID_SETUP_KNIFE,
                                                                                pAttributeTime->m_fKnifePowerup));
    };

    /* set blink remain time */
    CBlinkCharacterModule* pCtrlBlinkMod = static_cast<CBlinkCharacterModule*>(GetModule(MODULETYPE::BLINK_CHARACTER));
    if (pCtrlAttackMod)
        pCtrlBlinkMod->SetRemainTime(pAttributeTime->m_fBlink);
};


void CPlayerCharacter::GetAttributeTime(PLAYERTYPES::ATTRIBUTETIME* pAttributeTime)
{
    ASSERT(pAttributeTime);

    /* get attack power up remain time */
    CPlayerAttributeControlModule* pCtrlAttackMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_ATTACK_POWER_UP));
    if (pCtrlAttackMod)
        pAttributeTime->m_fAttackPowerup = pCtrlAttackMod->GetRemainTime();

    /* get defence power up remain time */
    CPlayerAttributeControlModule* pCtrlDefenceMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_DEFENCE_POWER_UP));
    if (pCtrlDefenceMod)
        pAttributeTime->m_fDefencePowerup = pCtrlDefenceMod->GetRemainTime();

    /* get charge time cut power up remain time */
    CPlayerAttributeControlModule* pCtrlChargeMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_CHANGE_TIME_CUT));
    if (pCtrlChargeMod)
        pAttributeTime->m_fChargePowerup = pCtrlChargeMod->GetRemainTime();

    /* get invincibility remain time */
    CPlayerAttributeControlModule* pCtrlInvinMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_INVINCIBILITY));
    if (pCtrlInvinMod)
        pAttributeTime->m_fInvincibility = pCtrlInvinMod->GetRemainTime();

    if (IsModuleIncluded(MODULETYPE::ITEM_EFFECT_INVINCIBLE))
    {
        pAttributeTime->m_bItemEffectExists = true;
        CharacterItemEffectModule::SetItemEffectModuleEnable(this, MODULETYPE::ITEM_EFFECT_INVINCIBLE, false);
    }
    else
    {
        pAttributeTime->m_bItemEffectExists = false;
    };

    /* get infinite knife power up remain time */
    CPlayerAttributeControlModule* pCtrlKnifeMod = static_cast<CPlayerAttributeControlModule*>(GetModule(MODULETYPE::PLAYERATTRCTRL_INNUMERABLE_KNIFE));
    if (pCtrlKnifeMod)
        pAttributeTime->m_fKnifePowerup = pCtrlKnifeMod->GetRemainTime();

    /* get blink remain time */
    CBlinkCharacterModule* pCtrlBlinkMod = static_cast<CBlinkCharacterModule*>(GetModule(MODULETYPE::BLINK_CHARACTER));
	if (pCtrlBlinkMod)
		pAttributeTime->m_fBlink = pCtrlBlinkMod->GetRemainTime();
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


float CPlayerCharacter::GetStatusDuration(void) const
{
    ASSERT(m_pStateMachine);
    return m_pStateMachine->GetStatusDuration();
};


PLAYERID::VALUE CPlayerCharacter::GetID(void) const
{
    return m_idPlayer;
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
    return TestPlayerFlag(PLAYERTYPES::FLAG_JUMP_2ND);
};


bool CPlayerCharacter::IsEnableJumpWall(void) const
{
    if (m_collisionWallInfo.m_bJumpWallHit)
    {
        if ((m_collisionWallInfo.m_attribute == MAPTYPES::ATTRIBUTE_JUMP) ||
            (m_collisionWallInfo.m_attribute == MAPTYPES::ATTRIBUTE_MISSJUMP))
            return true;
    };

    return false;
};


bool CPlayerCharacter::IsEnableAttackJump(void) const
{
    return TestPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS);
};


bool CPlayerCharacter::IsEnableAttackKnife(void) const
{
    if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHEATCODE_SHURIKEN))
        return true;

    if (TestAttribute(PLAYERTYPES::ATTRIBUTE_INNUMERABLE_KNIFE))
        return true;

    if (CGameProperty::Player(m_nPlayerNo)->GetShurikenNum() <= 0)
        return false;

    return !TestPlayerFlag(PLAYERTYPES::FLAG_DISABLE_THROW_KNIFE);
};


bool CPlayerCharacter::IsEnableAttackKnifeJump(void) const
{
    ASSERT(m_pStateMachine);
    
    if (CGameData::Record().Secret().GetAerialLevel() < 1)
        return false;

    if (!IsEnableAttackKnife())
        return false;

    if (!TestPlayerFlag(PLAYERTYPES::FLAG_AERIAL_STATUS))
        return false;

    return  (GetStatus() != PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP) ||
            (m_pStateMachine->GetStatusDuration() >= 0.15f);
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


bool CPlayerCharacter::IsActive(void) const
{
    return m_bActive;
};