#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"


class CBaseGeneralEnemyChr : public CEnemyCharacter
{
public:
    enum MOVABLETYPE
    {
        MOVABLETYPE_POSSIBLE = 0,
        MOVABLETYPE_IMPOSSIBLE,
        MOVABLETYPE_JUMP,
        MOVABLETYPE_FALLDOWN,
        MOVABLETYPE_HOLE,
        MOVABLETYPE_BODILYCRUSH
    };
    
public:
    CBaseGeneralEnemyChr(ENEMYID::VALUE idEnemy);
    virtual ~CBaseGeneralEnemyChr(void);
    virtual void Run(void) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;

    
    virtual MOVABLETYPE GetMovableType(RwV3d* pVecAt) const;
    virtual int32 GetDamageAccumulation(int32 no) const;

private:
    int32 m_iLastAttackerNo;
    int32 m_aiDamageAccumulation[8];
    RwV3d m_vPrePosition;
}; 