#include "SeesawGimmickMove.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CSeesawGimmickMove::CSeesawGimmickMove(void)
: m_invMat()
, m_fTheta(0.0f)
, m_fThetaLimit(0.0f)
, m_fOmega(0.0f)
, m_fOmegaLimit(0.0f)
, m_fMoment(0.0f)
, m_fRotateY(0.0f)
, m_nIndex(0)
, m_avPosition()
, m_fHorizonizeTimer(0.0f)
{
    m_fTheta        = 0.0f;
    m_fThetaLimit   = MATH_DEG2RAD(90.0f);
    
    m_fOmega        = 0.0f;
    m_fOmegaLimit   = MATH_DEG2RAD(1.0f);
    
    m_fMoment       = 0.0f;

    RwMatrixSetIdentityMacro(&m_invMat);

    for (int32 i = 0; i < COUNT_OF(m_avPosition); ++i)
        m_avPosition[i] = Math::VECTOR3_ZERO;
};


CSeesawGimmickMove::~CSeesawGimmickMove(void)
{
    ;
};


CSeesawGimmickMove::RESULT CSeesawGimmickMove::OnMove(float dt)
{
    RwV3d vMoment = Math::VECTOR3_ZERO;
    GetTotalMoment(&vMoment);

    m_fMoment = vMoment.z;
    m_fOmega += m_fMoment * dt * 0.01f;
    
    float fOmegaLimit = (m_fOmegaLimit * dt) * 60.0f;
    m_fOmega = Clamp(m_fOmega, -fOmegaLimit, fOmegaLimit);

    float fPrevTheta = m_fTheta;
    m_fTheta += ((dt * m_fOmega) * 60.0f);

    m_fTheta = Clamp(m_fTheta, -m_fThetaLimit, m_fThetaLimit);

    if (m_fTheta <= m_fThetaLimit)
    {
        if (m_fTheta < -m_fThetaLimit)
            m_fOmega = (-m_fThetaLimit - fPrevTheta);
    }
    else
    {
        m_fOmega = (m_fThetaLimit - fPrevTheta);
    };
    
    return RESULT_NONE;
};


void CSeesawGimmickMove::GetTheta(RwV3d* pvTheta)
{
    *pvTheta = { 0.0f, m_fRotateY, m_fTheta };
};


void CSeesawGimmickMove::GetOmega(RwV3d* pvOmega)
{
    *pvOmega = { 0.0f, 0.0f, m_fOmega };
};


void CSeesawGimmickMove::GetTotalMoment(RwV3d* pvMoment)
{
    *pvMoment = Math::VECTOR3_ZERO;

    m_fHorizonizeTimer = (m_nIndex ? (0.0f) : (m_fHorizonizeTimer + CGameProperty::GetElapsedTime()));
    if (m_fHorizonizeTimer > 5.0f)
        pvMoment->z -= (m_fTheta * 4.0f);

    for (int32 i = 0; i < m_nIndex; ++i)
    {
        RwV3d vPosition = m_avPosition[i];
        RwV3dTransformPoint(&vPosition, &vPosition, &m_invMat);
        
        if (std::fabs(vPosition.x) >= 0.1f)
        {
            pvMoment->z -=
                Math::Sin(Math::ATan2(vPosition.x, vPosition.y)) *
                Math::Sqrt(vPosition.x * vPosition.x + vPosition.y * vPosition.y) *
                Math::Cos(m_fTheta);
        };

        m_avPosition[i] = Math::VECTOR3_ZERO;
    };

    pvMoment->z -= (m_nIndex ? (m_fOmega * 80.0f) : (m_fOmega * 160.0f));

    m_nIndex = 0;
};


void CSeesawGimmickMove::SetCharacterPositionFromName(const char* pszName)
{
    CGameObject* pObj = CGameObjectManager::GetObject(pszName);
    ASSERT(pObj);

    if (pObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    if (static_cast<CCharacter*>(pObj)->GetCharacterType() != CCharacter::TYPE_PLAYER)
        return;

    ASSERT(m_nIndex < COUNT_OF(m_avPosition));

    static_cast<CCharacter*>(pObj)->GetBodyPosition(&m_avPosition[m_nIndex++]);
};


void CSeesawGimmickMove::SetMatrixFromClump(RpClump* pClump)
{
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    RwMatrixInvert(&m_invMat, pMatrix);
};