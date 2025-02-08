#include "BaseBatChr.hpp"

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*virtual*/ void CBaseBatChr::CMoveStatusObserver::OnStart(void) /*override*/
{
    CBaseFlyingEnemyChr::CMoveStatusObserver::OnStart();

    CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4336));

    int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);

    m_target = moveTarget;
};


/*virtual*/ CBaseBatChr::CMoveStatusObserver::RESULT
CBaseBatChr::CMoveStatusObserver::Observing(void) /*override*/
{
    RESULT result = CBaseFlyingEnemyChr::CMoveStatusObserver::Observing();
    if (result == RESULT_END)
        return result;
    
    int32 moveType = EnemyChr().AIThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);

    if (m_target != moveTarget)
        CGameSound::PlayObjectSE(&EnemyChr().Compositor(), SDCODE_SE(4336));

    m_target = moveTarget;

    return result;
};


/*virtual*/ float CBaseBatChr::CMoveStatusObserver::AltitudeNoise(void) /*override*/
{
    return Math::RandFloatRange(-0.5f, 1.0f);
};


//
// *********************************************************************************
//


CBaseBatChr::CBaseBatChr(ENEMYID::VALUE enemyId)
: CBaseFlyingEnemyChr(enemyId)
{
    /* CBaseFlyingEnemyChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_ATTACK_A,   new CBaseFlyingEnemyChr::CAttackAStatusObserver);

    /* CBaseBatChr status */
    AttachStatusObserver(ENEMYTYPES::STATUS_WALK,       new CMoveStatusObserver);
};


/*virtual*/ CBaseBatChr::~CBaseBatChr(void)
{
    ;
};


/*virtual*/ void CBaseBatChr::Run(void) /*override*/
{
    Compositor().SetEnableBodyHit(false);
    CBaseFlyingEnemyChr::Run();
};