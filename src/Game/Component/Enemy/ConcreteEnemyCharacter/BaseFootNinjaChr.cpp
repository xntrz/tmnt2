#include "BaseFootNinjaChr.hpp"

#include "Game/Component/Enemy/EnemyUtils.hpp"


/*virtual*/ void CBaseFootNinjaChr::CBackAwayStatusObserver::OnStart(void) /*override*/
{
    EnemyChr().SetFlag(ENEMYTYPES::FLAG_INVINCIBILITY);

    EnemyChr().Compositor().SetVelocity(&Math::VECTOR3_ZERO);
    EnemyChr().Compositor().SetAcceleration(&Math::VECTOR3_ZERO);

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::BACKAWAY, true);
};


/*virtual*/ CBaseFootNinjaChr::CBackAwayStatusObserver::RESULT
CBaseFootNinjaChr::CBackAwayStatusObserver::Observing(void) /*override*/
{
    if (EnemyChr().Compositor().IsMotionEnd())
        return RESULT_END;

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFootNinjaChr::CBackAwayStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_INVINCIBILITY);
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
    
    return ENEMYTYPES::STATUS_THINKING;
};


//
// *********************************************************************************
//


/*virtual*/ void CBaseFootNinjaChr::CJAttackStatusObserver::OnStart(void) /*override*/
{
    float fMoveSpeed = EnemyChr().CharacterParameter().m_feature.m_fRunMoveSpeed;
    float fJumpSpeed = EnemyChr().CharacterParameter().m_feature.m_fJumpInitializeSpeed;
    
    RwV3d vVelocity = { 0.0f, fJumpSpeed, fMoveSpeed };
    EnemyChr().Compositor().RotateVectorByDirection(&vVelocity, &vVelocity);
    EnemyChr().Compositor().SetVelocity(&vVelocity);

    float fAerialSpeed = EnemyChr().CharacterParameter().m_feature.m_fAerialMoveSpeed;
    EnemyChr().Compositor().SetMaxVelocity(fAerialSpeed);

    CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION
                               | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ;
    EnemyChr().Compositor().SetCharacterFlag(cflag);

    EnemyChr().Compositor().ChangeMotion(ENEMYTYPES::MOTIONNAMES::JATTACK, true);

    EnemyChr().SetFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);
};


/*virtual*/ CBaseFootNinjaChr::CJAttackStatusObserver::RESULT
CBaseFootNinjaChr::CJAttackStatusObserver::Observing(void) /*override*/
{
    float fJumpInitSpeed = EnemyChr().CharacterParameter().m_feature.m_fJumpInitializeSpeed;
    CEnemyUtils::ProcAerialMotion(&EnemyChr().Compositor(), fJumpInitSpeed);

    return RESULT_CONTINUE;
};


/*virtual*/ ENEMYTYPES::STATUS CBaseFootNinjaChr::CJAttackStatusObserver::OnEnd(void) /*override*/
{
    EnemyChr().AIThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);

    CHARACTERTYPES::FLAG cflag = CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION
                               | CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ;
    EnemyChr().Compositor().ClearCharacterFlag(cflag);

    EnemyChr().ClearFlag(ENEMYTYPES::FLAG_AERIAL_STATUS);

    return ENEMYTYPES::STATUS_TOUCHDOWN;
};


//
// *********************************************************************************
//


CBaseFootNinjaChr::CBaseFootNinjaChr(ENEMYID::VALUE idEnemy)
: CBaseGrapplerEnemyChr(idEnemy)
{
    ;
};


/*virtual*/ CBaseFootNinjaChr::~CBaseFootNinjaChr(void)
{
    ;
};


/*virtual*/ void CBaseFootNinjaChr::OnMessageAttackResult(CHitCatchData* pCatch) /*override*/
{
    ENEMYTYPES::STATUS nowStatus = GetStatus();
    if ((nowStatus == ENEMYTYPES::STATUS_AERIAL) ||
        (nowStatus == ENEMYTYPES::STATUS_JUMP))
    {
        SetStatus(ENEMYTYPES::STATUS_JUMP_ATTACK);
        return;
    };

    CBaseGrapplerEnemyChr::OnMessageAttackResult(pCatch);
};