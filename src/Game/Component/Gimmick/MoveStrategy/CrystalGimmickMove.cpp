#include "CrystalGimmickMove.hpp"

#include "System/Common/Screen.hpp"


/*static*/ int32 CCrystalGimmickMove::m_nCrystal = 0;


CCrystalGimmickMove::CCrystalGimmickMove(const RwV3d* pvInitialPos)
: m_vRotation(Math::VECTOR3_ZERO)
, m_vRotationVelocity(Math::VECTOR3_ZERO)
, m_vInitPos(Math::VECTOR3_ZERO)
, m_fTheta(0.0f)
, m_fThetaAdd(0.0f)
, m_movetype(MOVETYPE_NORMAL)
{
    m_vInitPos = *pvInitialPos;
    m_vInitPos.y += 0.5f;

    m_fTheta = float(m_nCrystal) * MATH_DEG2RAD(18.0f) - (Math::PI * 0.5f);
    m_fThetaAdd = Math::PI2 / (CScreen::Framerate() * 5.0f);

    ++m_nCrystal;
};


CCrystalGimmickMove::~CCrystalGimmickMove(void)
{
    ASSERT(m_nCrystal > 0);
    --m_nCrystal;
};


CCrystalGimmickMove::RESULT CCrystalGimmickMove::OnMove(float dt)
{
    switch (m_movetype)
    {
    case MOVETYPE_NORMAL:
        UpDownMove(dt);
        break;

    case MOVETYPE_SHAKE:
        SpasmMove(dt);
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_NONE;
};


void CCrystalGimmickMove::SpasmMove(float dt)
{
    m_vRotation.x += (Math::RandFloatRange(-1.0f, 1.0f) * MATH_DEG2RAD(20.0f));
    m_vRotation.y += (Math::RandFloatRange(-1.0f, 1.0f) * MATH_DEG2RAD(20.0f));
    m_vRotation.z += (Math::RandFloatRange(-1.0f, 1.0f) * MATH_DEG2RAD(20.0f));
};


void CCrystalGimmickMove::UpDownMove(float dt)
{
    RwV3d vDltRot = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltRot, &m_vRotationVelocity, dt);

    RwV3d vNewRot = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewRot, &m_vRotation, &vDltRot);

    m_vRotation = vNewRot;

    float fOfsY = Math::Sin(m_fTheta) * 0.5f;
    
    m_fTheta += m_fThetaAdd;
    m_fThetaAdd = (m_fTheta < -(Math::PI * 0.5f) ? -m_fThetaAdd : m_fThetaAdd);
    m_fThetaAdd = (m_fTheta >  (Math::PI * 0.5f) ? -m_fThetaAdd : m_fThetaAdd);
    m_fTheta = Clamp(m_fTheta, -(Math::PI * 0.5f), (Math::PI * 0.5f));

    m_vPosition = m_vInitPos;
    m_vPosition.y += fOfsY;
};