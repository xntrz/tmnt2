#include "PlayerStatus.hpp"
#include "PlayerStateMachine.hpp"

#include "ConcretePlayerStatus/PlayerStatusBasic.hpp"
#include "ConcretePlayerStatus/PlayerStatusJump.hpp"
#include "ConcretePlayerStatus/PlayerStatusGuard.hpp"
#include "ConcretePlayerStatus/PlayerStatusAttack.hpp"
#include "ConcretePlayerStatus/PlayerStatusDamage.hpp"
#include "ConcretePlayerStatus/PlayerStatusThrow.hpp"


/*static*/ bool CStatus::IsWithinStatusFromArray(
    PLAYERTYPES::STATUS         status,
    const PLAYERTYPES::STATUS*  pStatusArray,
    int32                       nArraySize
)
{
    ASSERT(pStatusArray);
    ASSERT(nArraySize > 0);
    
    for (int32 i = 0; i < nArraySize; ++i)
    {
        if (status == pStatusArray[i])
            return true;
    };

    return false;
};


/*static*/ void CStatus::RegistDefaultForStateMachine(CPlayerStateMachine& statemachine)
{
    //
    //  Basic
    //
    statemachine.RegistStatus(PLAYERTYPES::STATUS_IDLE,         new PlayerStatus::CIdle);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_WALK,         new PlayerStatus::CWalk);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_RUN,          new PlayerStatus::CRun);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_DASH_CANCEL,  new PlayerStatus::CDashCancel);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_DASH,         new PlayerStatus::CDash);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_DASH_FINISH,  new PlayerStatus::CDashFinish);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_DOWN_FRONT,   new PlayerStatus::CDownFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_DOWN_BACK,    new PlayerStatus::CDownBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_TUMBLER_FRONT,new PlayerStatus::CTumblerFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_TUMBLER_BACK, new PlayerStatus::CTumblerBack);

    //
    //  Jump
    //
    statemachine.RegistStatus(PLAYERTYPES::STATUS_JUMP_READY,       new PlayerStatus::CJumpReady);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_JUMP,             new PlayerStatus::CJump);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_JUMP_2ND,         new PlayerStatus::CJump2nd);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_JUMP_WALL,        new PlayerStatus::CJumpWall);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_JUMP_WALL_FAILURE,new PlayerStatus::CJumpWallFailure);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_AERIAL,           new PlayerStatus::CAerial);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_AERIAL_MOVE,      new PlayerStatus::CAerialMove);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_TOUCHDOWN,        new PlayerStatus::CTouchdown);

    //
    //  Guard
    //
    statemachine.RegistStatus(PLAYERTYPES::STATUS_GUARD_READY,      new PlayerStatus::CGuardReady);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_GUARD,            new PlayerStatus::CGuard);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_GUARD_BACK,       new PlayerStatus::CGuardBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_GUARD_IMPACT,     new PlayerStatus::CGuardImpact);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_GUARD_FINISH,     new PlayerStatus::CGuardFinish);

    //
    //  Attack
    //
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_A,         new PlayerStatus::CAttackA);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AA,        new PlayerStatus::CAttackAA);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AAB,       new PlayerStatus::CAttackAAB);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AAC,       new PlayerStatus::CAttackAAC);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABB,      new PlayerStatus::CAttackAABB);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABC,      new PlayerStatus::CAttackAABC);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBB,     new PlayerStatus::CAttackAABBB);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_AABBC,     new PlayerStatus::CAttackAABBC);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_B,         new PlayerStatus::CAttackB);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_B_CHARGE,  new PlayerStatus::CAttackBCharge);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE,     new PlayerStatus::CAttackKnife);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_KNIFE_JUMP,new PlayerStatus::CAttackKnifeJump);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_ATTACK_RUN,       new PlayerStatus::CAttackRun);

    //
    //  Damage
    //
    statemachine.RegistStatus(PLAYERTYPES::STATUS_KNOCK_FRONT,                  new PlayerStatus::CKnockFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_KNOCK_BACK,                   new PlayerStatus::CKnockBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_FLYAWAY_FRONT,                new PlayerStatus::CFlyawayFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_FLYAWAY_BACK,                 new PlayerStatus::CFlyawayBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_FLYAWAY_BOUND_FRONT,          new PlayerStatus::CFlyawayBoundFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK,           new PlayerStatus::CFlyawayBoundBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CRASH_WALL_FRONT,             new PlayerStatus::CCrashWallFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CRASH_WALL_BACK,              new PlayerStatus::CCrashWallBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CRASH_WALL_FALL_FRONT,        new PlayerStatus::CCrashWallFallFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CRASH_WALL_FALL_BACK,         new PlayerStatus::CCrashWallFallBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_FRONT,   new PlayerStatus::CCrashWallTouchdownFront);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK,    new PlayerStatus::CCrashWallTouchdownBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_FREEZE,                       new PlayerStatus::CFreeze);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_DINDLE,                       new PlayerStatus::CDindle);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_SLEEP,                        new PlayerStatus::CSleep);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_STUN,                         new PlayerStatus::CStun);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_BIND,                         new PlayerStatus::CBind);    

    //
    //  Throw
    //
    statemachine.RegistStatus(PLAYERTYPES::STATUS_LIFT_CHALLENGE,           new PlayerStatus::CLiftChallenge);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_LIFT_SUCCESS,             new PlayerStatus::CLiftSuccess);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_LIFT,                     new PlayerStatus::CLift);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_LIFT_WALK,                new PlayerStatus::CLiftWalk);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROW,                    new PlayerStatus::CThrow);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROW_BACK,               new PlayerStatus::CThrowBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROW_COMBINATION,        new PlayerStatus::CThrowCombination);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_CAUGHT,                   new PlayerStatus::CCaught);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_LIFTED,                   new PlayerStatus::CLifted);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_LIFTED_WALK,              new PlayerStatus::CLiftedWalk);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROWN,                   new PlayerStatus::CThrown);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROWN_BACK,              new PlayerStatus::CThrownBack);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROWN_COMBINATION,       new PlayerStatus::CThrownCombination);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROWN_COMBINATION_END,   new PlayerStatus::CThrownCombinationEnd);
    statemachine.RegistStatus(PLAYERTYPES::STATUS_THROWN_MISS,              new PlayerStatus::CThrownMiss);
};