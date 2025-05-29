#pragma once

#include "EnemyTypes.hpp"
#include "EnemyID.hpp"

#include "Game/System/Character/Character.hpp"


class CEnemyCharacter;


class CCharacterCompositor : public CCharacter
{
public:
    CCharacterCompositor(const char* pszName, TYPE type, CEnemyCharacter& rEnemyChr);
    virtual ~CCharacterCompositor(void);
    virtual void Run(void) override;
    virtual TYPE GetAttackCharacterType(void) const override;
    virtual void Draw(void) const override;
    virtual void OnMessageAerial(float fFieldHeight) override;
    virtual bool OnMessageCanChangingAerial(float fFieldHeight) override;
    virtual void OnMessageDoChangingAerial(void) override;
    virtual void OnMessageTouchdown(float fFieldHeight) override;
    virtual void OnMessageCatchAttack(CHitAttackData* pAttack) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual void OnMessageReceivedDamage(int32 nDamage) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& calculator) override;
    virtual void OnMessageCatch(void* pParam) override;
    virtual void OnMessageLift(void* pParam) override;
    virtual void OnMessageThrow(void* pParam) override;
    virtual void OnMessageMissThrow(void* pParam) override;
    virtual void OnSteppedDeathFloor(void) override;
    void IncludeModule(IModule* pModule);
    void ModuleRipple(float fRadius);
    void ModuleShadow(float fRadius);
    ENEMYTYPES::STATUS GetStatus(void) const;
    void RotateDirection(float fDirection, float fRotateRate);
	ENEMYID::VALUE GetID(void) const;
    CHARACTERTYPES::ATTACKPARAMETER& AttackParameter(void);
    CEnemyCharacter& EnemyChr(void);

private:
    CEnemyCharacter& m_rEnemyChr;
    CHARACTERTYPES::ATTACKPARAMETER m_attackParameter;    
};