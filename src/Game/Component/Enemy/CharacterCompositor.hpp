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
    virtual TYPE GetAttackCharacterType(void) const;
    virtual void Draw(void) const;    
    virtual void OnMessageAerial(float fFieldHeight);
    virtual bool OnMessageCanChangingAerial(float fFieldHeight);
    virtual void OnMessageDoChangingAerial(void);
    virtual void OnMessageTouchdown(float fFieldHeight);
    virtual void OnMessageCatchAttack(CHitAttackData* pAttack);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch);
    virtual void OnMessageReceivedDamage(int32 nDamage);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& calculator);
    virtual void OnMessageCatch(void* pParam);
    virtual void OnMessageLift(void* pParam);
    virtual void OnMessageThrow(void* pParam);
    virtual void OnMessageMissThrow(void* pParam);
    virtual void OnMessageCombination(void* pParam);
    virtual void OnChangeMotion(void);
    virtual void OnSteppedDeathFloor(void);
    void ModuleRipple(float fRadius);
    void ModuleShadow(float fRadius);
    ENEMYTYPES::STATUS GetStatus(void) const;
    void RotateDirection(float fDirection, float fRotateRate);
    ENEMYID::VALUE GetID(void);

private:
    CEnemyCharacter& m_rEnemyChr;
    CHARACTERTYPES::ATTACKPARAMETER m_attackparameter;    
};