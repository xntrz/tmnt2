#include "BaseFlyingEnemyAI.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/Enemy/ConcreteEnemyCharacter/BaseFlyingEnemyChr.hpp"


CBaseFlyingEnemyAI::CBaseFlyingEnemyAI(CEnemyCharacter* pEnemyChr)
: CBaseGeneralEnemyAIModerator(pEnemyChr)
, m_state(STATE_IDLE)
, m_fCancelTimer(0.0f)
{
    ;
};


/*virtual*/ CBaseFlyingEnemyAI::~CBaseFlyingEnemyAI(void)
{
    ;
};


/*virtual*/ CBaseFlyingEnemyAI::UNDERACTION CBaseFlyingEnemyAI::OnUnderMove(void) /*override*/
{
    UNDERACTION underAction = OnUnderWait();
    if (underAction)
        return underAction;

    if ((m_fCancelTimer <= 0.0f) ||
        (m_fCancelTimer >  10.0f))
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
        m_targetInfo.Reset();
        SetState(STATE_PURPOSE);
        return UNDERACTIONS_THINKOVER;
    };

#ifdef _DEBUG
    DebugDrawMovePoint();
#endif /* _DEBUG */

    int32 moveType   = ThinkOrder().OrderMove().m_iMoveType;
    int32 moveTarget = AIOT::GetTarget(moveType);

    if (moveTarget)
    {
        if (IsOutsidePatrolArea())
        {
            ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
            return UNDERACTIONS_THINKOVER;
		};

        if (IsMoveEndForTargetNumber())
        {
            ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
            return UNDERACTIONS_THINKOVER;
        };
    }
    else if (IsMoveEndForTargetPosition())
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_ACCEPT);
        SetState(STATE_IDLE);
        return UNDERACTIONS_THINKOVER;
    };

    float fDirection = EnemyCharacter().Compositor().GetDirection();
    if (IsHitWall(fDirection))
    {
        ThinkOrder().SetAnswer(CAIThinkOrder::RESULT_REFUSE);
        m_targetInfo.Reset();
        SetState(STATE_PURPOSE);
        return UNDERACTIONS_THINKOVER;
    };

    return UNDERACTIONS_CONTINUOUS;
};


/*virtual*/ bool CBaseFlyingEnemyAI::IsHitWall(float fDirection)
{
    RwMatrix matRotY;
    RwMatrixSetIdentity(&matRotY);
    Math::Matrix_RotateY(&matRotY, fDirection);

    float fRadius = EnemyCharacter().Compositor().GetCollisionParameter().m_fRadius;
    RwV3d vecStep = { 0.0f, 0.2f, (4.0f * fRadius) };
    RwV3dTransformVector(&vecStep, &vecStep, &matRotY);

    RwV3d vecPos = Math::VECTOR3_ZERO;
    EnemyCharacter().Compositor().GetBodyPosition(&vecPos);

    Math::Vec3_Add(&vecPos, &vecPos, &vecStep);

    CBaseFlyingEnemyChr::MOVABLETYPE movableType = static_cast<CBaseFlyingEnemyChr&>(EnemyCharacter()).GetMovableType(&vecPos);
	if (movableType == CBaseFlyingEnemyChr::MOVABLETYPE_IMPOSSIBLE)
		return true;

    const CCharacterCompositor::COLLISIONWALLINFO* pWallInfo = EnemyCharacter().Compositor().GetCollisionWall();
    ASSERT(pWallInfo != nullptr);

	if (pWallInfo->m_bHit)
		return true;

	return false;
};


void CBaseFlyingEnemyAI::SetCancelTimer(float t)
{
    m_fCancelTimer = t;
};


void CBaseFlyingEnemyAI::UpdateCancelTimer(void)
{
    m_fCancelTimer -= CGameProperty::GetElapsedTime();
};


void CBaseFlyingEnemyAI::SetState(STATE state)
{
    m_state = state;
};


CBaseFlyingEnemyAI::STATE CBaseFlyingEnemyAI::GetState(void) const
{
    return m_state;
};