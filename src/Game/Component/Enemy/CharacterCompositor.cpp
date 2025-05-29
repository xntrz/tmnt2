#include "CharacterCompositor.hpp"
#include "EnemyCharacter.hpp"

#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Module/RippleEffectModule.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"


CCharacterCompositor::CCharacterCompositor(const char* pszName, TYPE type, CEnemyCharacter& rEnemyChr)
: CCharacter(pszName, type)
, m_rEnemyChr(rEnemyChr)
, m_attackParameter()
{
    std::memset(&m_attackParameter, 0, sizeof(m_attackParameter));
    m_attackParameter.m_vPosition = Math::VECTOR3_ZERO;
    m_attackParameter.m_vVelocity = Math::VECTOR3_ZERO;
    m_attackParameter.m_vDirection = Math::VECTOR3_ZERO;
    m_attackParameter.m_pHitAttackData = nullptr;
};


CCharacterCompositor::~CCharacterCompositor(void)
{
    m_rEnemyChr.Delete();
};


void CCharacterCompositor::Run(void)
{
    m_rEnemyChr.Run();
    CCharacter::Run();
};


CCharacterCompositor::TYPE CCharacterCompositor::GetAttackCharacterType(void) const
{
    CEnemyCharacter::CHRTYPE chrType = m_rEnemyChr.GetAttackCharacterType();
    switch (chrType)
    {
    case CEnemyCharacter::CHRTYPE_ENEMY:
        return TYPE_ENEMY;

    case CEnemyCharacter::CHRTYPE_PLAYER:
        return TYPE_PLAYER;

    default:
        ASSERT(false);
        return TYPE_ENEMY;
    };
};


void CCharacterCompositor::Draw(void) const
{
    m_rEnemyChr.Draw();
    CCharacter::Draw();
};


void CCharacterCompositor::OnMessageAerial(float fFieldHeight)
{
    CCharacter::OnMessageAerial(fFieldHeight);
};


bool CCharacterCompositor::OnMessageCanChangingAerial(float fFieldHeight)
{
    CCharacter::OnMessageCanChangingAerial(fFieldHeight);
    return m_rEnemyChr.OnMessageCanChangingAerial(fFieldHeight);
};


void CCharacterCompositor::OnMessageDoChangingAerial(void)
{
    CCharacter::OnMessageDoChangingAerial();
    m_rEnemyChr.OnMessageDoChangingAerial();
};


void CCharacterCompositor::OnMessageTouchdown(float fFieldHeight)
{
    m_rEnemyChr.OnMessageTouchdown(fFieldHeight);
    CCharacter::OnMessageTouchdown(fFieldHeight);
};


void CCharacterCompositor::OnMessageCatchAttack(CHitAttackData* pAttack)
{
    m_rEnemyChr.OnMessageCatchAttack(pAttack);
    CCharacter::OnMessageCatchAttack(pAttack);
};


void CCharacterCompositor::OnMessageAttackResult(CHitCatchData* pCatch)
{
    m_rEnemyChr.OnMessageAttackResult(pCatch);
    CCharacter::OnMessageAttackResult(pCatch);
};


void CCharacterCompositor::OnMessageReceivedDamage(int32 nDamage)
{
    m_rEnemyChr.OnMessageReceivedDamage(nDamage);
};


CHARACTERTYPES::ATTACKRESULTTYPE CCharacterCompositor::OnDamage(CCharacterAttackCalculator& calculator)
{
    return m_rEnemyChr.OnDamage(calculator);
};


void CCharacterCompositor::OnMessageCatch(void* pParam)
{
    m_rEnemyChr.OnMessageCatch(pParam);
    CCharacter::OnMessageCatch(pParam);
};


void CCharacterCompositor::OnMessageLift(void* pParam)
{
    m_rEnemyChr.OnMessageLift(pParam);
    CCharacter::OnMessageLift(pParam);
};


void CCharacterCompositor::OnMessageThrow(void* pParam)
{
    m_rEnemyChr.OnMessageThrow(pParam);
    CCharacter::OnMessageThrow(pParam);
};


void CCharacterCompositor::OnMessageMissThrow(void* pParam)
{
    m_rEnemyChr.OnMessageMissThrow(pParam);
    CCharacter::OnMessageMissThrow(pParam);
};


void CCharacterCompositor::OnSteppedDeathFloor(void)
{
    m_rEnemyChr.OnSteppedDeathFloor();
};


void CCharacterCompositor::IncludeModule(IModule* pModule)
{
    m_pModuleMan->Include(pModule);
};


void CCharacterCompositor::ModuleRipple(float fRadius)
{
    CRippleEffectModule* pModule = new CRippleEffectModule(this, fRadius);
    m_pModuleMan->Include(pModule);
};


void CCharacterCompositor::ModuleShadow(float fRadius)
{
    CCircleShadowModule* pModule = CCircleShadowModule::New(this, fRadius, fRadius, true);
    m_pModuleMan->Include(pModule);
};


ENEMYTYPES::STATUS CCharacterCompositor::GetStatus(void) const
{
    return m_rEnemyChr.GetStatus();
};


void CCharacterCompositor::RotateDirection(float fDirection, float fRotateRate)
{
    float fRotateDirectionMax = CGameProperty::GetElapsedTime() * (fRotateRate * Math::PI);
    float fDirectionDiff = fDirection - m_fDirection;

	fDirectionDiff = Math::RadianCorrect(fDirectionDiff);
    ASSERT(std::fabs(fDirectionDiff) <= Math::PI);
    
    fDirectionDiff = Clamp(fDirectionDiff, -fRotateDirectionMax, fRotateDirectionMax);

    SetDirection(m_fDirection + fDirectionDiff);
};


ENEMYID::VALUE CCharacterCompositor::GetID(void) const
{
    return m_rEnemyChr.GetID();
};


CHARACTERTYPES::ATTACKPARAMETER& CCharacterCompositor::AttackParameter(void)
{
    return m_attackParameter;
};


CEnemyCharacter& CCharacterCompositor::EnemyChr(void)
{
    return m_rEnemyChr;
};