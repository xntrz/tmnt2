#pragma once

#include "PlayerTypes.hpp"

#include "Game/System/Misc/PadStream.hpp"
#include "Game/System/GameObject/GameObject.hpp"


class CPlayerCharacter;


class CManipulator : public CGameObject
{
protected:
    struct INPUT
    {
        uint32 m_uMove;
        uint32 m_uDash;
        uint32 m_uJump;
        uint32 m_uAttack;
        uint32 m_uAttackCharge;
        uint32 m_uKnife;
        uint32 m_uGuard;
        uint32 m_uLift;
        uint32 m_uRecover;
        uint32 m_uPassive;
        float m_fDirection;
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

protected:
    CPlayerCharacter* m_pPlayerChr;
    INPUT m_input;
    int32 m_nControllerNo;
    CPadStream m_padstream;
};