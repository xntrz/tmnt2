#pragma once

#include "PlayerTypes.hpp"

#include "Game/System/Misc/PadStream.hpp"
#include "Game/System/GameObject/GameObject.hpp"


class CPlayerCharacter;


class CManipulator : public CGameObject
{
protected:
    enum MOVE
    {
        MOVE_IDLE = 0,
        MOVE_WALK,
        MOVE_RUN,
    };

    enum DASH
    {
        DASH_OFF = 0,
        DASH_ON,
    };

    enum JUMP
    {
        JUMP_OFF = 0,
        JUMP_ON
    };

    enum ATTACK
    {
        ATTACK_OFF = 0,
        ATTACK_A,
        ATTACK_B,
        ATTACK_C,
    };

    enum ATTACK_CHARGE
    {
        ATTACK_CHARGE_OFF = 0,
        ATTACK_CHARGE_ON,
    };

    enum KNIFE
    {
        KNIFE_OFF = 0,
        KNIFE_ON,
    };

    enum GUARD
    {
        GUARD_OFF = 0,
        GUARD_ON
    };

    enum LIFT
    {
        LIFT_OFF = 0,
        LIFT_ON,
    };

    enum RECOVER
    {
        RECOVER_OFF = 0,
        RECOVER_ON,
    };

    enum PASSIVE
    {
        PASSIVE_OFF = 0,
        PASSIVE_ON
    };

    struct INPUT
    {
        MOVE            m_move;
        DASH            m_dash;
        JUMP            m_jump;
        ATTACK          m_attack;
        ATTACK_CHARGE   m_attackCharge;
        KNIFE           m_knife;
        GUARD           m_guard;
        LIFT            m_lift;
        RECOVER         m_recover;
        PASSIVE         m_passive;
        float           m_fDirection;
    };
    
public:
    static CManipulator* New(CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    static void Delete(CManipulator* pManip);
    
    CManipulator(const char* pszName, CPlayerCharacter* pPlayerChr, int32 nControllerNo);
    virtual ~CManipulator(void);
    virtual void Run(void) override;
    virtual void BranchForStatus(PLAYERTYPES::STATUS status);
    virtual void RunIdle(void);
    virtual void RunWalk(void);
    virtual void RunRun(void);
    virtual void RunGrounding(void);
    virtual void RunJump(void);
    virtual void RunJump2nd(void);
    virtual void RunJumpWall(void);
    virtual void RunAerial(void);
    virtual void RunAerialMove(void);
    virtual bool RunAerialCommon(void);
    virtual void RunGuardReady(void);
    virtual void RunGuard(void);
    virtual void RunGuardCommon(void);
    virtual void RunAttackA(void);
    virtual void RunAttackAA(void);
    virtual void RunAttackAAB(void);
    virtual void RunAttackAAC(void);
    virtual void RunAttackAABB(void);
    virtual void RunAttackAABC(void);
    virtual void RunAttackAABBB(void);
    virtual void RunAttackAABBC(void);
    virtual void RunAttackCommon(void);
    virtual void RunAttackBCharge(void);
    virtual void RunAttackKnife(void);
    virtual void RunAttackKnifeJump(void);
    virtual void RunLift(void);
    virtual void RunLiftWalk(void);
    virtual void RunLiftCommon(void);
    virtual void RunThrownCombination(void);
    virtual void RunFlyawayBound(void);
    virtual void RunStatusDamage(void);
    virtual void RunPush(void);
    void AnalyzeInputDevice(void);
    void AnalyzeInputVector(RwV3d& rvInputVector, float& rfInputVectorLength);
    void SetSpecificAbilityFlag(void);
    void SetConfusionInput(void);
    void ClearInput(void);

protected:
    CPlayerCharacter* m_pPlayerChr;
    INPUT m_input;
    int32 m_nControllerNo;
    CPadStream m_padstream;
};