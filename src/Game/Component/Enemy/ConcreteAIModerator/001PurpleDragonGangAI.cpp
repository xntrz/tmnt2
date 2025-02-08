#include "001PurpleDragonGangAI.hpp"

#include "Game/Component/Enemy/EnemyParameter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"


/*static*/ CAIModerator* C001PurpleDragonGangAI::Instance(CEnemyCharacter* pEnemyChr)
{
	return new C001PurpleDragonGangAI(pEnemyChr);
};


C001PurpleDragonGangAI::C001PurpleDragonGangAI(CEnemyCharacter* pEnemyChr)
: CBaseGrapplerAI(pEnemyChr)
, m_bThrowGrenadeRequest(false)
, m_bCheckThrowGrenadeRequest(false)
, m_prevHP(0)
, m_fThrowGrenadeTimer(0.0f)
{
    ;
};


/*virtual*/ C001PurpleDragonGangAI::~C001PurpleDragonGangAI(void)
{
    ;
};


/*virtual*/ CAIThinkOrder::ORDER C001PurpleDragonGangAI::ActionSelector(int32 iNo) /*override*/
{
    CAIThinkOrder::ORDER order = CBaseGrapplerAI::ActionSelector(iNo);
    if ((order == CAIThinkOrder::ORDER_MOVE) && m_bThrowGrenadeRequest)
        return CAIThinkOrder::ORDER_ATTACK;

    return CBaseGrapplerAI::ActionSelector(iNo);
};


/*virtual*/ bool C001PurpleDragonGangAI::OnActionOfAttack(int32 iNo) /*override*/
{
    if (!m_bThrowGrenadeRequest)
        return CBaseGrapplerAI::OnActionOfAttack(iNo);

    AIOT::SetAttackOrder(ThinkOrder(), AIOT::AttackC, iNo);
    
    m_bThrowGrenadeRequest      = false;
    m_bCheckThrowGrenadeRequest = false;
    m_fThrowGrenadeTimer        = GetFrequency(CEnemyParameter::FREQUENCY_COMMON_ATTACKINTERVAL);

    return true;
};


/*virtual*/ C001PurpleDragonGangAI::UNDERACTION C001PurpleDragonGangAI::OnUnderAllActions(void) /*override*/
{
    int32 currHP = EnemyCharacter().CharacterParameter().m_feature.m_iHP;
    if (currHP < m_prevHP)
    {
        float fRand = Math::RandFloat();
        float fRatioOfActivity = Characteristic().m_fRatioOfActivity;
        if (fRand < fRatioOfActivity)
            m_bCheckThrowGrenadeRequest = true;
    };
    m_prevHP = currHP;

    m_fThrowGrenadeTimer -= CGameProperty::GetElapsedTime();
    if (m_fThrowGrenadeTimer < 0.0f)
        m_bCheckThrowGrenadeRequest = true;

    return CBaseGrapplerAI::OnUnderAllActions();
};


/*virtual*/ bool C001PurpleDragonGangAI::IsTakeAttack(void) /*override*/
{
    if (IsTakeThrowGrenade())
        return true;

    return CBaseGrapplerAI::IsTakeAttack();
};


/*virtual*/ bool C001PurpleDragonGangAI::IsTakeThrowGrenade(void)
{
    if (!m_bCheckThrowGrenadeRequest)
        return false;

    if (!IsSatifyFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE_RATE))
        return false;

    float fFireRange = GetFrequency(CEnemyParameter::FREQUENCY_FIRE_RANGE);
    float fTargetDist = m_targetInfo.GetDistance();

    SUITABLEDISTANCE suitableDistance = GetDistanceOfSuitable(fTargetDist, fFireRange);
    if (suitableDistance != SUITABLEDISTANCE_EQUAL)
        return false;

    float fDirTarget = m_targetInfo.GetAngle();
    float fDirMe = EnemyCharacter().Compositor().GetDirection();

    float fDirDiff = fDirTarget - fDirMe;
    fDirDiff = std::fabs(fDirDiff);

    float fRatioOfFrontView = Characteristic().m_fRatioOfFrontView;
    float fFrontView = (fRatioOfFrontView * MATH_DEG2RAD(36.0f));

    if (fFrontView < fDirDiff)
        return false;

    m_bThrowGrenadeRequest = true;
    return true;
};