#pragma once

#include "PlayerID.hpp"
#include "PlayerTypes.hpp"

#include "Game/Component/Module/ModuleType.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameMain/ItemID.hpp"
#include "Game/System/Character/Character.hpp"


class CPlayerStateMachine;


class CPlayerCharacter : public CCharacter
{
public:
    struct PARAMETER
    {
        CCharacter::PARAMETER m_chrparameter;
        PLAYERTYPES::FEATURE m_feature;
        CPlayerStateMachine* m_pStateMachine;
    };
    
public:
    static CPlayerCharacter* New(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume);
    static void Delete(CPlayerCharacter* pPlayerChr);
    
    CPlayerCharacter(const char* pszName, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume);
    virtual ~CPlayerCharacter(void);
    virtual void Run(void) override;
    virtual void Draw(void) const override;
    virtual void IncludeBasicModule(void) override;
    virtual void CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame) override;    
    virtual bool OnMessageCanChangingAerial(float fFieldHeight) override;
    virtual void OnMessageDoChangingAerial(void) override;
    virtual void OnMessageTouchdown(float fFieldHeight) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual void OnMessageReceivedDamage(int32 nDamage) override;
    virtual void CalcAttackPower(CHitAttackData& rAttack) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& calculator) override;
    virtual void OnMessageCatch(void* pParam) override;
    virtual void OnMessageLift(void* pParam) override;
    virtual void OnMessageThrow(void* pParam) override;
    virtual void OnMessageMissThrow(void* pParam) override;
    virtual void OnMessageCombination(void* pParam) override;
    virtual void OnSteppedDeathFloor(void) override;
    virtual int32 GetPlayerIndex(void) const;
    virtual void ShootingKnife(void);
    virtual PLAYERTYPES::STATUS RequestStatusMorphing(PLAYERTYPES::STATUS status);
    CHARACTERTYPES::DEFENCERSTATUSFLAG CheckDefenceStatusFlag(const CHitAttackData& rAttack);
    void Initialize(PARAMETER* pParameter);
    void OnAttach(CPlayerCharacter* pBeforeCharacter, bool bChangeEffectEnable);
    void OnDetach(CPlayerCharacter* pAfterCharacter);
    void RotateDirection(float fDirection);
    void Relocation(const RwV3d* pvPosition, float fDirection, bool bBlink);
    void GenerateItemEffect(ITEMID::VALUE idItem);
    void SetPlayerNo(int32 nPlayerNo);
    void SetPadID(int32 nPadID);
    void SetBlinkEnable(bool bEnable, float fDuration);
    void SetInvincibilityEnable(bool bEnable, float fDuration);
    void SetAttackPowerupEnable(bool bEnable, float fDuration);
    void SetDefencePowerupEnable(bool bEnable, float fDuration);
    void SetChargePowerupEnable(bool bEnable, float fDuration);
    void SetKnifePowerupEnable(bool bEnable, float fDuration);
    void SetConfusionEnable(bool bEnable, float fDuration);
    void SetAmbientLightEnable(bool bEnable);
    void SetEffectEnable(bool bEnable);
    void SetLastGroundingStatus(PLAYERTYPES::STATUS status);
    PLAYERTYPES::STATUS GetLastGroundingStatus(void) const;
    PLAYERTYPES::STATUS ChangeStatus(PLAYERTYPES::STATUS status);
    void ChangeKnockStatus(void);
    void ChangeFlyawayStatus(void);
    void ChangeThrowStatus(CCharacterAttackCalculator& calculator);
    void ApplyStatusEffect(void);
    void CheckPartyAttack(CCharacterAttackCalculator& calculator);
    void RequestAttackA(void);
    void RequestAttackB(void);
    void RequestAttackC(void);
    PLAYERTYPES::FLAG CheckAttackConnect(PLAYERTYPES::FLAG mask);
    bool CheckChargeTime(float fTime, PLAYERTYPES::CHARGEPHASE* chargephase);
    void ClearCharge(void);
    void ClearStatusParameter(void);
    void SetDefaultFlags(void);
    void ReplayMotion(void);
    void SetAerialMotionTime(float fMaxSpeed);
    void SetAttributeTime(PLAYERTYPES::ATTRIBUTETIME* pAttributeTime);
    void GetAttributeTime(PLAYERTYPES::ATTRIBUTETIME* pAttributeTime);    
    PLAYERTYPES::STATUS GetStatus(void) const;
    PLAYERTYPES::STATUS GetStatusPrev(void) const;
    float GetStatusDuration(void) const;
    PLAYERID::VALUE GetID(void) const;
    int32 GetPadID(void) const;
    int32 GetPlayerNo(void) const;
    void GetReplacePosition(RwV3d* pvPosition) const;
    void SetReplacePosition(RwV3d* pvPosition);
    bool IsEnableCharacterChanging(void) const;
    bool IsEnableJump2nd(void) const;
    bool IsEnableJumpWall(void) const;
    bool IsEnableAttackJump(void) const;
    bool IsEnableAttackKnife(void) const;
    bool IsEnableAttackKnifeJump(void) const;
    bool IsThrowable(void) const;
    const PLAYERTYPES::FEATURE& Feature(void) const;
    CHARACTERTYPES::ATTACKPARAMETER& AttackParameter(void);
    PLAYERTYPES::CHARGEPHASE GetChargePhase(void) const;
	bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) const;
    bool IsActive(void) const;
    
    /*inline*/ void SetPlayerFlag(PLAYERTYPES::FLAG flag);
    /*inline*/ void ClearPlayerFlag(PLAYERTYPES::FLAG flag);
    /*inline*/ bool TestPlayerFlag(PLAYERTYPES::FLAG flag) const;
    /*inline*/ bool CheckPlayerFlag(PLAYERTYPES::FLAG flag) const;

    /*inline*/ void SetAttribute(PLAYERTYPES::ATTRIBUTE attribute);
    /*inline*/ void ClearAttribute(PLAYERTYPES::ATTRIBUTE attribute);
    /*inline*/ bool TestAttribute(PLAYERTYPES::ATTRIBUTE attribute) const;

	/*inline*/ int32 GetNumWallJump() const;

private:
    PLAYERID::VALUE m_idPlayer;
    GAMETYPES::COSTUME m_costume;
    int32 m_nPlayerIndex;
    int32 m_nPlayerNo;
    int32 m_nPadID;
    int32 m_nNumWallJump;
    CHARACTERTYPES::ATTACKPARAMETER m_attackparameter;
    PLAYERTYPES::ATTRIBUTE m_attribute;
    PLAYERTYPES::FLAG m_pflag;
    CPlayerStateMachine* m_pStateMachine;
    PLAYERTYPES::CHARGEPHASE m_chargephase;
    PLAYERTYPES::FEATURE m_feature;
    PLAYERTYPES::STATUS m_lastGrondingStatus;
    RwV3d m_vReplacepoint;
    bool m_bActive;
};


inline void CPlayerCharacter::SetPlayerFlag(PLAYERTYPES::FLAG flag)
{
    m_pflag |= flag;
};


inline void CPlayerCharacter::ClearPlayerFlag(PLAYERTYPES::FLAG flag)
{
    m_pflag &= (~flag);
};


inline bool CPlayerCharacter::TestPlayerFlag(PLAYERTYPES::FLAG flag) const
{
    return ((m_pflag & flag) == flag);
};


inline bool CPlayerCharacter::CheckPlayerFlag(PLAYERTYPES::FLAG flag) const
{
    return ((m_pflag & flag) != 0);
};


inline void CPlayerCharacter::SetAttribute(PLAYERTYPES::ATTRIBUTE attribute)
{
    m_attribute |= attribute;
};


inline void CPlayerCharacter::ClearAttribute(PLAYERTYPES::ATTRIBUTE attribute)
{
    m_attribute &= (~attribute);
};


inline bool CPlayerCharacter::TestAttribute(PLAYERTYPES::ATTRIBUTE attribute) const
{
    return ((m_attribute & attribute) == attribute);
};


inline int32 CPlayerCharacter::GetNumWallJump() const
{
	return m_nNumWallJump;
};