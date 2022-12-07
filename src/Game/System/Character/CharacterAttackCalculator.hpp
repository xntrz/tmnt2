#pragma once

#include "CharacterTypes.hpp"


class CCharacter;
class CGameObject;
class CHitAttackData;
class CHitCatchData;


class CCharacterAttackCalculator
{
public:
    CCharacterAttackCalculator(CCharacter& rCharacter, CHitAttackData& rAttack);
    ~CCharacterAttackCalculator(void);
    CHARACTERTYPES::ATTACKRESULTTYPE CalcAtackParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter, CHARACTERTYPES::DEFENCERSTATUSFLAG defenceflag);
    float CalcDamage(CHARACTERTYPES::ATTACKRESULTTYPE attackresult);
    void PlayEffect(CHARACTERTYPES::ATTACKRESULTTYPE attackresult);
    void PlaySE(CHARACTERTYPES::ATTACKRESULTTYPE attackresult);
    void CheckAttackType(void);
    CHARACTERTYPES::ATTACKTYPE CheckDamageAttackType(void);
    void CheckAttackerPosition(void);
    void CalcDirection(void);
    CHARACTERTYPES::ATTACKTYPE MorphinAttackType(CHARACTERTYPES::ATTACKTYPE attacktype, CHARACTERTYPES::DEFENCERSTATUSFLAG defenceflag);
    CHARACTERTYPES::ATTACKRESULTTYPE CheckAttackResult(CHARACTERTYPES::ATTACKTYPE attacktype, CHARACTERTYPES::DEFENCERSTATUSFLAG defenceflag);
    void CalcKnockParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter);
    void CalcFlyawayParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter);
    void CalcGuardParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter);
    void CalcStatusDamageParameter(CHARACTERTYPES::ATTACKPARAMETER& rAttackParameter);
    float CalcAttackPowerRatio(int32 iMinPower, int32 iMaxPower);
    void SetCatchResult(int32 iResult);
    bool IsGuardableAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const;
    bool IsTroubleAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const;
    bool IsThrowAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const;
    bool IsPartyAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const;
    bool IsConfusionAttack(void) const;
    bool IsDamageAttack(CHARACTERTYPES::ATTACKTYPE attacktype) const;
    bool IsSliceAttack(void) const;
    CGameObject* GetDefender(void) const;
    CGameObject* GetAttacker(void) const;
    CHitAttackData& GetAttack(void) const;
    CHARACTERTYPES::ATTACKDIRECTIONTYPE DirectionType(void) const;

private:
    CCharacter& m_rCharacter;
    CHitAttackData& m_rAttack;
    RwV3d m_vAttackerPosition;
    RwV3d m_vDirection;
    CHARACTERTYPES::ATTACKTYPE m_attacktype;
    CHARACTERTYPES::ATTACKDIRECTIONTYPE m_directiontype;
    CHARACTERTYPES::DEFENCERSTATUSFLAG m_defenceflag;
    bool m_bCountered;
};