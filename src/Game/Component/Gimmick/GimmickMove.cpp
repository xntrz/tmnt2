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


void CGimmickMove::Start(void)
{
    m_state = STATE_MOVING;
};


void CGimmickMove::Stop(void)
{
    m_state = STATE_NONE;
};


CGimmickMove::MOVESTATE CGimmickMove::Move(float dt)
{
    RESULT OldResult = m_result;
    
    if (m_state == STATE_MOVING)
    {
        m_result = OnMove(dt);
        if (m_movestate == MOVESTATE_NONE)
        {
            OldResult = m_result;
        };
    }
    else
    {
        if (m_movestate == MOVESTATE_NONE)
        {
            return MOVESTATE_NONE;
        };        
    };

    if (m_result)
    {
        if (OldResult == RESULT_COLLIDED)
        {
            m_movestate = MOVESTATE_ON_GROUND;
        }
        else
        {
            m_movestate = MOVESTATE_TOUCHDOWN;
        };
    }
    else
    {
        if (OldResult == RESULT_COLLIDED)
        {
            m_movestate = MOVESTATE_LIFTOFF;
        }
        else
        {
            m_movestate = MOVESTATE_IN_THE_AIR;
        };
    };

    return m_movestate;
};


void CGimmickMove::SetPosition(const RwV3d* pPos)
{
    ASSERT(pPos);

    m_vPosition = *pPos;
};


void CGimmickMove::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);
    
    *pvPosition = m_vPosition;
};


void CGimmickMove::GetLook(RwV3d* pvLook) const
{
    ASSERT(pvLook);

    *pvLook = m_vLook;
};