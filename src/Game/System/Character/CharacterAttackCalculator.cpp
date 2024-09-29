#include "CharacterAttackCalculator.hpp"
#include "Character.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Shot/Shot.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"


namespace EFFECTNAMES
{
    static const char* GUARD        = "all_guard1";
    static const char* GUARD_IMPACT = "all_guard2";
    static const char* GUARD_BREAK  = "all_guardbreak";
    static const char* COUNTER      = "all_count";
    static const char* STUN_HIT     = "all_piyohit";
};


CCharacterAttackCalculator::CCharacterAttackCalculator(CCharacter& rCharacter, CHitAttackData& rAttack)
: m_rCharacter(rCharacter)
, m_rAttack(rAttack)
, m_vAttackerPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_attacktype(CHARACTERTYPES::ATTACKTYPE_UNKNOWN)
, m_directiontype(CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT)
, m_defenceflag(CHARACTERTYPES::DEFENCERSTATUSFLAG_NONE)
, m_bCountered(false)
{
    CheckAttackType();
    CheckAttackerPosition();
    CalcDirection();
};


CCharacterAttackCalculator::~CCharacterAttackCalculator(void)
{
    ;
};


CHARACTERTYPES::ATTACKRESULTTYPE CCharacterAttackCalculator::CalcAtackParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter, CHARACTERTYPES::DEFENCERSTATUSFLAG defenceflag)
{
    CHARACTERTYPES::ATTACKTYPE attacktype = MorphinAttackType(m_attacktype, defenceflag);

    rAttackParameter.m_vPosition = m_vAttackerPosition;
    rAttackParameter.m_vDirection = m_vDirection;
    rAttackParameter.m_vVelocity = Math::VECTOR3_ZERO;
    rAttackParameter.m_direction = m_directiontype;
    rAttackParameter.m_pHitAttackData = &m_rAttack;

    if (!FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW))
    {
        CGameObject* pAttacker = m_rAttack.GetObject();
        if (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CCharacter* pAttackerChr = (CCharacter*)pAttacker;
            if (pAttackerChr->GetAttackCharacterType() == CCharacter::TYPE_PLAYER)
            {
                CPlayerCharacter* pAttackerPlr = (CPlayerCharacter*)pAttackerChr;
                if (pAttackerPlr->GetStatus() == PLAYERTYPES::STATUS_THROW_BACK)
                    rAttackParameter.m_direction = CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT;
            };
        };
    };

    CHARACTERTYPES::ATTACKRESULTTYPE attackresult = CheckAttackResult(attacktype, defenceflag);
    switch (attackresult)
    {
    case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK:
        CalcKnockParameter(rAttackParameter);
        m_defenceflag = defenceflag;
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY:
        CalcFlyawayParameter(rAttackParameter);
        m_defenceflag = defenceflag;
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
    case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
    case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
    case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
    case CHARACTERTYPES::ATTACKRESULTTYPE_BIND:
        CalcStatusDamageParameter(rAttackParameter);
        m_defenceflag = defenceflag;
        break;
        
    case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD:
        CalcGuardParameter(rAttackParameter);
        m_defenceflag = defenceflag;
        break;

    default:
        m_defenceflag = defenceflag;
        break;
    };

    return attackresult;
};


float CCharacterAttackCalculator::CalcDamage(CHARACTERTYPES::ATTACKRESULTTYPE attackresult)
{
    float fResult = 0.0f;

    if (m_rAttack.GetPower() <= 0)
        return fResult;

    if (attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT ||
        attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_PARTY_ATTACK ||
        attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_THROW ||
		attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE)
        return fResult;

    float fPowerRatio = 1.0f;

    if (attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD ||
        attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK)
    {
        fPowerRatio = 0.2f;
    }
    else if (FLAG_TEST(m_defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_ITEMUP))
    {
        fPowerRatio = 0.5f;
    };

    if (FLAG_TEST(m_defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DEFENCEPOWER_GROWUP))
        fPowerRatio *= 0.5f;

    if (m_rCharacter.GetAttackCharacterType() == CCharacter::TYPE_PLAYER)
    {
        fPowerRatio /= CGameData::Record().Secret().GetDefenceEnchanceValue();

        if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_CHALLENGE_NIGHTMARE))
            fPowerRatio += fPowerRatio;
    };

    fResult = float(m_rAttack.GetPower()) * fPowerRatio;

    if (m_bCountered)
    {
        CGameObject* pAttacker = m_rAttack.GetObject();
        if (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CCharacter* pAttackerChr = (CCharacter*)pAttacker;
            if (pAttackerChr->GetAttackCharacterType() == CCharacter::TYPE_PLAYER)
            {
                CPlayerCharacter* pAttackerPlayer = (CPlayerCharacter*)pAttacker;
                CGameEvent::SetPlayerTechnicalAction(
                    pAttackerPlayer->GetPlayerNo(),
                    GAMETYPES::TECACT_GUARD_IMPACT
                );
            };
        };
    };

    if (fResult < 1.0f)
        fResult = 1.0f;

    return fResult;
};


void CCharacterAttackCalculator::PlayEffect(CHARACTERTYPES::ATTACKRESULTTYPE attackresult)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;

    switch (m_rAttack.GetShape())
    {
    case CHitAttackData::SHAPE_LINE:
        vPosition = *m_rAttack.GetHitPos();
        break;

    case CHitAttackData::SHAPE_SPHERE:
        vPosition = m_rAttack.GetSphere()->center;
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_bCountered)
    {
        switch (attackresult)
        {
        case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK:
        case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY:
        case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION:
        case CHARACTERTYPES::ATTACKRESULTTYPE_THROW:
        case CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE:
        case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
        case CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP:
        case CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE:
            {
                CEffectManager::Play(EFFECTNAMES::COUNTER, &vPosition);
            }
            break;
        };
    }
    else
    {
        switch (attackresult)
        {
        case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_KNOCK:
        case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_FLYAWAY:
        case CHARACTERTYPES::ATTACKRESULTTYPE_DAMAGE_NOREACTION:
            {
                RwV3d vHitPos = *m_rAttack.GetHitPos();
                EFFECT_GENERIC::CallHitEffect(&m_rAttack, &vHitPos);
            }
            break;

        case CHARACTERTYPES::ATTACKRESULTTYPE_STUN:
            {
                CEffectManager::Play(EFFECTNAMES::STUN_HIT, &vPosition);
            }
            break;
            
        case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD:
            {
                CEffectManager::Play(EFFECTNAMES::GUARD, &vPosition);
            }
            break;
            
        case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT:
            {
                CEffectManager::Play(EFFECTNAMES::GUARD_IMPACT, &vPosition);
            }
            break;

        case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK:
            {
                CEffectManager::Play(EFFECTNAMES::GUARD_BREAK, &vPosition);
            }
            break;
        };
    };
};


void CCharacterAttackCalculator::PlaySE(CHARACTERTYPES::ATTACKRESULTTYPE attackresult)
{
	if (attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE ||
		attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_THROW ||
		attackresult == CHARACTERTYPES::ATTACKRESULTTYPE_PARTY_ATTACK)
		return;

	CGameObject* pObjAttacker = GetAttacker();
	ASSERT(pObjAttacker != nullptr);

	SE_DAMAGE_PARAM param;
	std::memset(&param, 0, sizeof(param));

	param.Attacker = pObjAttacker;
	param.AttackerType = pObjAttacker->GetType();

	switch (pObjAttacker->GetType())
	{
	case GAMEOBJECTTYPE::GIMMICK:
		param.AttackerSubType = static_cast<CGimmick*>(pObjAttacker)->GetID();
		break;

	case GAMEOBJECTTYPE::CHARACTER:
		param.AttackerSubType = static_cast<CCharacter*>(pObjAttacker)->GetCharacterType();
		param.AttackerMotion = static_cast<CCharacter*>(pObjAttacker)->GetMotionName();
		param.AttackerId = GetCharacterID(static_cast<CCharacter*>(pObjAttacker));
		break;

	case GAMEOBJECTTYPE::EFFECT:
		param.AttackerSubType = static_cast<CEffect*>(pObjAttacker)->GetID();
		break;

	case GAMEOBJECTTYPE::SHOT:
		param.AttackerSubType = static_cast<CShot*>(pObjAttacker)->GetID();
		break;

	default:
		break;
	};

	param.DefenderType = m_rCharacter.GetCharacterType();
	param.DefenderId = GetCharacterID(&m_rCharacter);
	m_rCharacter.GetBodyPosition(&param.Pos);
	param.AttackResult = attackresult;

	CGameSound::PlayDamageSE(&param);
};


void CCharacterAttackCalculator::CheckAttackType(void)
{
    CGameObject* pAttacker = m_rAttack.GetObject();
    ASSERT(pAttacker);

    switch (pAttacker->GetType())
    {
    case GAMEOBJECTTYPE::GIMMICK:
    case GAMEOBJECTTYPE::EFFECT:
    case GAMEOBJECTTYPE::SHOT:
        m_attacktype = CheckDamageAttackType();
        break;
        
    case GAMEOBJECTTYPE::CHARACTER:
        {
            CCharacter* pAttackerChr = (CCharacter*)pAttacker;
            
            if (pAttackerChr->GetAttackCharacterType() != m_rCharacter.GetAttackCharacterType() ||
                pAttackerChr->GetAttackCharacterType() == CCharacter::TYPE_ENEMY)
            {
                m_attacktype = CheckDamageAttackType();
            }
            else
            {
                m_attacktype = CHARACTERTYPES::ATTACKTYPE_PARTY_ATTACK;

                if (m_rCharacter.GetAttackCharacterType() == CCharacter::TYPE_PLAYER)
                {
                    if (m_rAttack.GetStatus() == CHitAttackData::STATUS_THROW)
                    {
                        m_attacktype = CHARACTERTYPES::ATTACKTYPE_THROW;
                        m_rAttack.SetPower(0);
                    };
                };
            };
        };
        break;
    };
};


CHARACTERTYPES::ATTACKTYPE CCharacterAttackCalculator::CheckDamageAttackType(void)
{
    struct ATTACKTYPEINFO
    {
        CHitAttackData::STATUS m_status;
        CHARACTERTYPES::ATTACKTYPE m_attacktype;
    };

    static const ATTACKTYPEINFO s_aAttackTypeInfo [] =
    {
        { CHitAttackData::STATUS_KNOCK,     CHARACTERTYPES::ATTACKTYPE_DAMAGE_KNOCK,    },
        { CHitAttackData::STATUS_FLYAWAY,   CHARACTERTYPES::ATTACKTYPE_DAMAGE_FLYAWAY,  },
        { CHitAttackData::STATUS_THROW,     CHARACTERTYPES::ATTACKTYPE_THROW,           },
        { CHitAttackData::STATUS_STUN,      CHARACTERTYPES::ATTACKTYPE_STUN,            },
        { CHitAttackData::STATUS_DINDLE,    CHARACTERTYPES::ATTACKTYPE_DINDLE,          },
        { CHitAttackData::STATUS_SLEEP,     CHARACTERTYPES::ATTACKTYPE_SLEEP,           },
        { CHitAttackData::STATUS_FREEZE,    CHARACTERTYPES::ATTACKTYPE_FREEZE,          },
        { CHitAttackData::STATUS_BIND,      CHARACTERTYPES::ATTACKTYPE_BIND,            },
    };

    for (int32 i = 0; i < COUNT_OF(s_aAttackTypeInfo); ++i)
    {
        if (m_rAttack.GetStatus() == s_aAttackTypeInfo[i].m_status)
            return s_aAttackTypeInfo[i].m_attacktype;
    };

    return CHARACTERTYPES::ATTACKTYPE_UNKNOWN;
};


void CCharacterAttackCalculator::CheckAttackerPosition(void)
{
    CGameObject* pAttacker = m_rAttack.GetObject();
    ASSERT(pAttacker);
    
    if (pAttacker->GetType() == GAMEOBJECTTYPE::EFFECT)
    {
        m_vAttackerPosition = *m_rAttack.GetObjectPos();
    }
    else
    {
        switch (m_rAttack.GetShape())
        {
        case CHitAttackData::SHAPE_SPHERE:
            m_vAttackerPosition = *m_rAttack.GetObjectPos();
            break;

        case CHitAttackData::SHAPE_LINE:
            m_vAttackerPosition = *m_rAttack.GetHitPos();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CCharacterAttackCalculator::CalcDirection(void)
{
    RwV3d vPos = Math::VECTOR3_ZERO;
    m_rCharacter.GetBodyPosition(&vPos);
    Math::Vec3_Sub(&m_vDirection, &m_vAttackerPosition, &vPos);
    m_vDirection.y = 0.0f;
    Math::Vec3_Normalize(&m_vDirection, &m_vDirection);
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    Math::Matrix_RotateY(&matrix, m_rCharacter.GetDirection());    

    if (Math::Vec3_Dot(&m_vDirection, &matrix.at) >= 0.0f)
    {
        m_directiontype = CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT;
    }
    else
    {
        m_directiontype = CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK;
        Math::Vec3_Negate(&m_vDirection, &m_vDirection);
    };    
};


CHARACTERTYPES::ATTACKTYPE CCharacterAttackCalculator::MorphinAttackType(CHARACTERTYPES::ATTACKTYPE attacktype, CHARACTERTYPES::DEFENCERSTATUSFLAG defenceflag)
{
    CHARACTERTYPES::ATTACKTYPE Result = attacktype;

    if (FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_AERIAL) &&
        (IsTroubleAttack(attacktype) || attacktype == CHARACTERTYPES::ATTACKTYPE_DAMAGE_KNOCK))
    {
        Result = CHARACTERTYPES::ATTACKTYPE_DAMAGE_FLYAWAY;
    };

    if (FLAG_TEST_ANY(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_STATUS_MASK))
    {
        switch (attacktype)
        {
        case CHARACTERTYPES::ATTACKTYPE_DAMAGE_KNOCK:
        case CHARACTERTYPES::ATTACKTYPE_DAMAGE_FLYAWAY:
            Result = CHARACTERTYPES::ATTACKTYPE_DAMAGE_NOREACTION;
            break;

        case CHARACTERTYPES::ATTACKTYPE_DINDLE:
        case CHARACTERTYPES::ATTACKTYPE_STUN:
        case CHARACTERTYPES::ATTACKTYPE_SLEEP:
        case CHARACTERTYPES::ATTACKTYPE_FREEZE:
            Result = CHARACTERTYPES::ATTACKTYPE_DAMAGE_FLYAWAY;
            break;

        case CHARACTERTYPES::ATTACKTYPE_BIND:
            Result = CHARACTERTYPES::ATTACKTYPE_UNKNOWN;
            break;
        };
    }
    else if (
        (FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_STUN)  ||
        FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DINDLE) ||
        FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_FREEZE) ||
        FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_SLEEP))
        &&
        (IsDamageAttack(attacktype) || IsTroubleAttack(attacktype)))        
    {
        Result = CHARACTERTYPES::ATTACKTYPE_DAMAGE_FLYAWAY;
    };

    if(IsThrowAttack(attacktype) && FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_DISABLE_THROW))
    {
        Result = CHARACTERTYPES::ATTACKTYPE_UNKNOWN;
    };

    if (IsTroubleAttack(attacktype) && FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_CHEATCODE_HEALTH))
    {
        Result = CHARACTERTYPES::ATTACKTYPE_DAMAGE_KNOCK;
    };

    return Result;
};


CHARACTERTYPES::ATTACKRESULTTYPE CCharacterAttackCalculator::CheckAttackResult(CHARACTERTYPES::ATTACKTYPE attacktype, CHARACTERTYPES::DEFENCERSTATUSFLAG defenceflag)
{
    CHARACTERTYPES::ATTACKRESULTTYPE Result = CHARACTERTYPES::ATTACKRESULTTYPE(attacktype);

    if (!IsGuardableAttack(attacktype) &&
        (FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD_ALLRANGE) ||
        FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD)) &&
        (!FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD) || m_directiontype == CHARACTERTYPES::ATTACKDIRECTIONTYPE_FRONT))
    {
        switch (m_rAttack.GetAntiguard())
        {
        case CHitAttackData::ANTIGUARD_BREAK:
            Result = CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_BREAK;
            break;

        default:
            Result = CHARACTERTYPES::ATTACKRESULTTYPE_GUARD;
            break;
        };

        if (FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_GUARD_IMPACT))
            Result = CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT;
    };

    switch (Result)
    {
    case CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE:
    case CHARACTERTYPES::ATTACKRESULTTYPE_THROW:
    case CHARACTERTYPES::ATTACKRESULTTYPE_PARTY_ATTACK:
    case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD:
    case CHARACTERTYPES::ATTACKRESULTTYPE_GUARD_IMPACT:
        break;
        
    default:
        if (FLAG_TEST(defenceflag, CHARACTERTYPES::DEFENCERSTATUSFLAG_STAGGER))
            m_bCountered = true;
        break;
    };

    return Result;
};


void CCharacterAttackCalculator::CalcKnockParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter)
{
    float fRatio = CalcAttackPowerRatio(5, 20);
    float fVelocity = -(fRatio * 4.0f + 4.0f);

    if (m_directiontype == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
        fVelocity = -fVelocity;

    Math::Vec3_Scale(&rAttackParameter.m_vVelocity, &rAttackParameter.m_vDirection, fVelocity);
};


void CCharacterAttackCalculator::CalcFlyawayParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter)
{
    float fRatio = 0.0f;
    float fVelXZ = 0.0f;
    float fVelY = 0.0f;
    
    bool bIsStatusMissing = (m_rAttack.GetStatus() != CHitAttackData::STATUS_FLYAWAY);
    bool bIsStatusParameterMissing = (Math::FEqual(m_rAttack.GetFlyawayVelXZ(), 0.0f) && Math::FEqual(m_rAttack.GetFlyawayVelY(), 0.0f));

    if (bIsStatusMissing || bIsStatusParameterMissing)
    {
        fRatio = CalcAttackPowerRatio(5, 20);
        fVelY = fRatio * 2.0f + 2.0f;
        fVelXZ = -(fRatio * 10.0f + 10.0f);
    }
    else
    {
        fVelY = m_rAttack.GetFlyawayVelY();
        fVelXZ = m_rAttack.GetFlyawayVelXZ();
    };

    if (m_directiontype == CHARACTERTYPES::ATTACKDIRECTIONTYPE_BACK)
        fVelXZ = -fVelXZ;

    Math::Vec3_Scale(&rAttackParameter.m_vVelocity, &rAttackParameter.m_vDirection, fVelXZ);
    rAttackParameter.m_vVelocity.y = fVelY;
};


void CCharacterAttackCalculator::CalcGuardParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter)
{
    float fRatio = CalcAttackPowerRatio(5, 20);
    float fVelocity = (fRatio * 4.0f + 4.0f);

    Math::Vec3_Scale(&rAttackParameter.m_vVelocity, &rAttackParameter.m_vDirection, fVelocity);
};


void CCharacterAttackCalculator::CalcStatusDamageParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter)
{
    rAttackParameter.m_fTroubleTime = m_rAttack.GetStatusDuration();
};


float CCharacterAttackCalculator::CalcAttackPowerRatio(int32 iMinPower, int32 iMaxPower)
{
    ASSERT(iMinPower < iMaxPower);

    return Clamp(
        float((m_rAttack.GetPower() - iMinPower) / (iMaxPower - iMinPower)),
        0.0f,
        1.0f
    );
};


void CCharacterAttackCalculator::SetCatchResult(int32 iResult)
{
    m_rAttack.GetCatch()->SetResult(CHitCatchData::RESULT(iResult));
};


bool CCharacterAttackCalculator::IsGuardableAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const
{
    return (attacktype != CHARACTERTYPES::ATTACKTYPE_PARTY_ATTACK);
};


bool CCharacterAttackCalculator::IsTroubleAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const
{
    return (attacktype == CHARACTERTYPES::ATTACKRESULTTYPE_DINDLE   ||
            attacktype == CHARACTERTYPES::ATTACKRESULTTYPE_STUN     ||
            attacktype == CHARACTERTYPES::ATTACKRESULTTYPE_SLEEP    ||
            attacktype == CHARACTERTYPES::ATTACKRESULTTYPE_FREEZE   ||
            attacktype == CHARACTERTYPES::ATTACKRESULTTYPE_BIND);
};


bool CCharacterAttackCalculator::IsThrowAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const
{
    return (attacktype == CHARACTERTYPES::ATTACKTYPE_THROW);
};


bool CCharacterAttackCalculator::IsPartyAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const
{
    return (attacktype == CHARACTERTYPES::ATTACKTYPE_PARTY_ATTACK);
};


bool CCharacterAttackCalculator::IsConfusionAttack(void) const
{
    return m_rAttack.IsFlagConfusion();
};


bool CCharacterAttackCalculator::IsDamageAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const
{
    return (attacktype == CHARACTERTYPES::ATTACKTYPE_DAMAGE_KNOCK ||
            attacktype == CHARACTERTYPES::ATTACKTYPE_DAMAGE_FLYAWAY);
};


bool CCharacterAttackCalculator::IsSliceAttack(void) const
{
    return m_rAttack.IsFlagSlice();
};


CGameObject* CCharacterAttackCalculator::GetDefender(void) const
{
    return m_rAttack.GetCatch()->GetObject();
};


CGameObject* CCharacterAttackCalculator::GetAttacker(void) const
{
    return m_rAttack.GetObject();
};


CHitAttackData& CCharacterAttackCalculator::GetAttack(void) const
{
    return m_rAttack;
};


CHARACTERTYPES::ATTACKDIRECTIONTYPE CCharacterAttackCalculator::DirectionType(void) const
{
    return m_directiontype;
};


int32 CCharacterAttackCalculator::GetCharacterID(CCharacter* pCharacter) const
{
	ASSERT(pCharacter);

	if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
		return static_cast<CPlayerCharacter*>(pCharacter)->GetID();
	else
		return static_cast<CCharacterCompositor*>(pCharacter)->GetID();
};