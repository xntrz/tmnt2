#include "GimmickMove.hpp"


CGimmickMove::CGimmickMove(void)
: m_pRotationMatrix(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vLook(Math::VECTOR3_ZERO)
, m_vUp(Math::VECTOR3_ZERO)
, m_state(STATE_NONE)
, m_movestate(MOVESTATE_NONE)
, m_result(RESULT_NONE)
{
    ;
};


CGimmickMove::~CGimmickMove(void)
{
    if (m_pRotationMatrix)
    {
        RwMatrixDestroy(m_pRotationMatrix);
        m_pRotationMatrix = nullptr;
    };
};


CGimmickMove::RESULT CGimmickMove::OnMove(float dt)
{
    return RESULT_NONE;
};


CGimmickMove::MOVESTATE CGimmickMove::Move(float dt)
{
    RESULT OldResult = m_result;
    
    if (m_state == STATE_MOVING)
    {
        m_result = OnMove(dt);
        if (m_movestate == MOVESTATE_NONE)
            OldResult = m_result;
    }
    else
    {
        if (m_movestate == MOVESTATE_NONE)
            return MOVESTATE_NONE;
    };

    if (m_result != RESULT_NONE)
        m_movestate = (OldResult == RESULT_COLLIDED ? MOVESTATE_ON_GROUND : MOVESTATE_TOUCHDOWN);
    else
        m_movestate = (OldResult == RESULT_COLLIDED ? MOVESTATE_LIFTOFF : MOVESTATE_IN_THE_AIR);

    return m_movestate;
};