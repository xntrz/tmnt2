#include "RideFlagGimmickMove.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/RideFlagGimmick.hpp"


CRideFlagGimmickMove::CRideFlagGimmickMove(int32 iType)
: m_fA(0.0f)
, m_fT(0.0f)
{
    struct INFO
    {
        float m_fA;
        float m_fT;        
    };

    static INFO s_aInfo[CRideFlagGimmick::FLAGTYPENUM] =
    {
        { 0.0f, 1.0f },
        { 0.0f, 1.0f },
        { 2.0f, 2.0f },
        { 2.0f, 2.0f },
        { 1.0f, 2.0f },
        { 1.0f, 2.0f },
    };

    ASSERT(iType >= 0);
    ASSERT(iType < COUNT_OF(s_aInfo));

    static RwV3d s_avDirection[CRideFlagGimmick::FLAGTYPENUM] =
    {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
    };

    ASSERT(iType >= 0);
    ASSERT(iType < COUNT_OF(s_avDirection));

    m_vMoveDirection = s_avDirection[iType];
    
    m_fA = s_aInfo[iType].m_fA;
    m_fT = s_aInfo[iType].m_fT;

    if (iType % 2)
        m_fTimer = m_fT * 0.5f;
    else
        m_fTimer = 0.0f;
};


CRideFlagGimmickMove::~CRideFlagGimmickMove(void)
{
    ;
};


CRideFlagGimmickMove::RESULT CRideFlagGimmickMove::OnMove(float dt)
{
    m_fSpeed = Math::Cos((MATH_PI2 / m_fT) * m_fTimer) * m_fA * (MATH_PI2 / m_fT);
    
    m_fTimer += dt;
    
    m_vVelocity.x = m_vMoveDirection.x * (dt * m_fSpeed);
    m_vVelocity.y = m_vMoveDirection.y * (dt * m_fSpeed);
    m_vVelocity.z = m_vMoveDirection.z * (dt * m_fSpeed);

    m_vPosition.x += m_vVelocity.x;
    m_vPosition.y += m_vVelocity.y;
    m_vPosition.z += m_vVelocity.z;

    m_vRotation.y += (dt * MATH_PI2);

	return RESULT_NONE;
};

