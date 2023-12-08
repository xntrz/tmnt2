#include "GuillotineGimmickMove.hpp"

#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/Sound/GameSound.hpp"


CGuillotineGimmickMove::CGuillotineGimmickMove(const INITPARAM* pInitParam)
: m_vRotation(Math::VECTOR3_ZERO)
, m_fTheta(0.0f)
, m_fOnceTheta(0.0f)
, m_fMaxMove(0.0f)
, m_fHitRadius(0.0f)
, m_fSwingTimer(0.0f)
, m_fSwingTime(0.0f)
, m_matFulcrumPoint()
, m_vFulcrumVector(Math::VECTOR3_ZERO)
, m_vHitSpherePos(Math::VECTOR3_ZERO)
, m_collisionResult()
, m_ePendulumState()
{
    RwMatrixSetIdentityMacro(&m_matFulcrumPoint);

    std::memset(&m_collisionResult, 0, sizeof(m_collisionResult));

    if (pInitParam)
    {
        m_fTheta        = pInitParam->m_fTheta;
        m_fOnceTheta    = pInitParam->m_fOnceTheta;
        m_fMaxMove      = pInitParam->m_fMaxMove;
        m_fHitRadius    = pInitParam->m_fHitRadius;
        m_vFulcrumVector= pInitParam->m_vFulcrumVector;
        m_vHitSpherePos = pInitParam->m_vHitSpherePos;
		m_fSwingTime = pInitParam->m_fMaxMove * 0.25f;
    };
};


CGuillotineGimmickMove::~CGuillotineGimmickMove(void)
{
    ;
};


CGuillotineGimmickMove::RESULT CGuillotineGimmickMove::OnMove(float dt)
{
    RwMatrix mat;
    RwMatrixSetIdentityMacro(&mat);
    Math::Matrix_AffineTransformation(&mat, nullptr, &m_vRotation, &m_vPosition);

    RwV3d vDltPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vDltPos, &m_vHitSpherePos, &mat);

    pendulumMove(dt);

    Math::Matrix_AffineTransformation(&mat, nullptr, &m_vRotation, &m_vPosition);

    RwV3d vNewPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vNewPos, &m_vHitSpherePos, &mat);

    Math::Vec3_Sub(&vDltPos, &vDltPos, &vNewPos);

    if (!CWorldMap::CheckCollisionSphereMove(&vNewPos, m_fHitRadius, &vDltPos, CWorldMap::CHECKFLAG_NONE))
        return RESULT_NONE;

    const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
    ASSERT(pResult);

    m_collisionResult = *pResult;

    return RESULT_COLLIDED;
};


void CGuillotineGimmickMove::InitFulcrumMatrix(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vPosition, &m_vPosition, &m_vFulcrumVector);

    Math::Matrix_AffineTransformation(&m_matFulcrumPoint, nullptr, &m_vRotation, &vPosition);
};


void CGuillotineGimmickMove::windSE(void)
{
    if (CGimmickUtils::CalcNearestPlayerDistance(&m_vPosition) <= 15.0f)
        CGameSound::PlayPositionSE(&m_vPosition, SDCODE_SE(4175));
};


void CGuillotineGimmickMove::pendulumMove(float dt)
{
    m_vRotation.z = (Math::Sin(m_fTheta) * m_fOnceTheta);

    reviceTheta(dt);

    InvClamp(m_vRotation.z, -Math::PI, Math::PI);

    RwV3d vPos = *RwMatrixGetPos(&m_matFulcrumPoint);
    Math::Matrix_AffineTransformation(&m_matFulcrumPoint, nullptr, &m_vRotation, &vPos);

    RwV3d vFulcrumVec = m_vFulcrumVector;
    vFulcrumVec.y *= -1.0f;

    RwV3dTransformPoint(&m_vPosition, &vFulcrumVec, &m_matFulcrumPoint);
};


void CGuillotineGimmickMove::reviceTheta(float dt)
{
    float afTheta[] =
    {
         MATH_DEG2RAD(90.0f),
         MATH_DEG2RAD(90.0f),
        -MATH_DEG2RAD(90.0f),
         MATH_DEG2RAD(0.0f),

        -MATH_DEG2RAD(90.0f),
        -MATH_DEG2RAD(90.0f),
         MATH_DEG2RAD(90.0f),
         MATH_DEG2RAD(0.0f),
    };

    m_fSwingTimer += dt;
    m_fTheta += (dt / m_fSwingTime) * afTheta[m_ePendulumState * 2];

    if (m_fSwingTimer >= m_fSwingTime)
    {
        m_fTheta = afTheta[(m_ePendulumState * 2) + 1];
        m_fSwingTimer -= m_fSwingTime;

        switch (m_ePendulumState)
        {
        case 0:
            m_ePendulumState = PENDULUMSTATE(1);
            break;

        case 1:
            windSE();
            m_ePendulumState = PENDULUMSTATE(2);
            break;

        case 2:
            m_ePendulumState = PENDULUMSTATE(3);
            break;

        case 3:
            windSE();
            m_ePendulumState = PENDULUMSTATE(0);
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};