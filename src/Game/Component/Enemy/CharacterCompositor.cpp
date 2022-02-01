#include "CharacterCompositor.hpp"
#include "EnemyCharacter.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"


CCharacterCompositor::CCharacterCompositor(const char* pszName, TYPE type, CEnemyCharacter& rEnemyChr)
: CCharacter(pszName, type)
, m_rEnemyChr(rEnemyChr)
{
    ;
};


CCharacterCompositor::~CCharacterCompositor(void)
{
    ;
};


void CCharacterCompositor::Run(void)
{
    ;
};


CCharacterCompositor::TYPE CCharacterCompositor::GetAttackCharacterType(void) const
{
    return TYPE_ENEMY;
};


void CCharacterCompositor::Draw(void) const
{
    ;
};


void CCharacterCompositor::OnMessageAerial(float fFieldHeight)
{
    ;
};


bool CCharacterCompositor::OnMessageCanChangingAerial(float fFieldHeight)
{
    return false;
};


void CCharacterCompositor::OnMessageDoChangingAerial(void)
{
    ;
};


void CCharacterCompositor::OnMessageTouchdown(float fFieldHeight)
{
    ;
};


void CCharacterCompositor::OnMessageCatchAttack(CHitAttackData* pAttack)
{
    ;
};


void CCharacterCompositor::OnMessageAttackResult(CHitCatchData* pCatch)
{
    ;
};


void CCharacterCompositor::OnMessageReceivedDamage(int32 nDamage)
{
    ;
};


CHARACTERTYPES::ATTACKRESULTTYPE CCharacterCompositor::OnDamage(CCharacterAttackCalculator& calculator)
{
	return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
};


void CCharacterCompositor::OnMessageCatch(void* pParam)
{
    ;
};


void CCharacterCompositor::OnMessageLift(void* pParam)
{
    ;
};


void CCharacterCompositor::OnMessageThrow(void* pParam)
{
    ;
};


void CCharacterCompositor::OnMessageMissThrow(void* pParam)
{
    ;
};


void CCharacterCompositor::OnMessageCombination(void* pParam)
{
    ;
};


void CCharacterCompositor::OnChangeMotion(void)
{
    ;
};


void CCharacterCompositor::OnSteppedDeathFloor(void)
{
    ;
};


void CCharacterCompositor::ModuleRipple(float fRadius)
{
    ;
};


void CCharacterCompositor::ModuleShadow(float fRadius)
{
    ;
};


ENEMYTYPES::STATUS CCharacterCompositor::GetStatus(void) const
{
    return ENEMYTYPES::STATUS_HIDE;
};


void CCharacterCompositor::RotateDirection(float fDirection, float fRotateRate)
{
    float fRotateDirectionMax = CGameProperty::GetElapsedTime() * (fRotateRate * Math::PI);
    float fDirectionDiff = fDirection - m_fDirection;

    fDirectionDiff = Math::RadianCorrect(fDirectionDiff);
    
    ASSERT(Math::FAbs(fDirectionDiff) <= Math::PI);
    
    fDirectionDiff = Math::Clamp(fDirectionDiff, -fRotateDirectionMax, fRotateDirectionMax);

    SetDirection(m_fDirection + fDirectionDiff);
};


ENEMYID::VALUE CCharacterCompositor::GetID(void)
{
    return ENEMYID::VALUE(ENEMYID::ID_DUMMY);
};