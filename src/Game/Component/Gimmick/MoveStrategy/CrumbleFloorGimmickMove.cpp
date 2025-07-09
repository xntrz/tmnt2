#include "CrumbleFloorGimmickMove.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CCrumbleFloorGimmickMove::CCrumbleFloorGimmickMove(void)
: m_invMat()
, m_nIndex(0)
, m_avPosition()
, m_fTheta(0.0f)
, m_fOmega(0.0f)
, m_fTotalMoment(0.0f)
, m_fCounter(0.0f)
, m_vVelocity(Math::VECTOR3_ZERO)
, m_eStep(STEP_WAIT)
, m_bEffect(true)
{
    RwMatrixSetIdentity(&m_invMat);

    for (int32 i = 0; i < COUNT_OF(m_avPosition); ++i)
        m_avPosition[i] = Math::VECTOR3_ZERO;
};


CCrumbleFloorGimmickMove::~CCrumbleFloorGimmickMove(void)
{
    ;
};


CCrumbleFloorGimmickMove::RESULT CCrumbleFloorGimmickMove::OnMove(float dt)
{
    RwV3d vMoment = Math::VECTOR3_ZERO;
    GetTotalMoment(&vMoment);

    m_fTotalMoment += (vMoment.z * dt);

    switch (m_eStep)
    {
    case STEP_WAIT:
        Wait(dt);
        break;

    case STEP_PREMOVE:
        PreMove(dt);
        break;

    case STEP_ENDURE:
        Endure(dt);
        break;

    case STEP_SHAKE:
        Shake(dt);
        break;

    case STEP_FALLDOWN:
        Falldown(dt);
        break;

    default:
        ASSERT(false);
        break;
    };

    return RESULT_NONE;
};


void CCrumbleFloorGimmickMove::Wait(float dt)
{
    if (m_fTotalMoment <= 0.8f)
    {
        if (m_fTotalMoment > 0.0f && m_bEffect)
        {
            RwV3d vPosition = m_vPosition;
            vPosition.x += 2.0f;
            vPosition.z += 2.0f;

            CEffectManager::Play("m12_breakrock", &vPosition);

            m_bEffect = false;
        };        
    }
    else
    {
        RwV3d vPosition = m_vPosition;
        vPosition.x += 2.0f;
        vPosition.z += 2.0f;

        CEffectManager::Play("m12_breakrock", &vPosition);

        CMessageManager::Request(SEGROUPID::VALUE(196));

        m_fCounter = 0.0f;
        m_vVelocity = Math::VECTOR3_ZERO;
        m_eStep = STEP_PREMOVE;
    };
};


void CCrumbleFloorGimmickMove::PreMove(float dt)
{
    if (m_fOmega >= (dt * MATH_DEG2RAD(20.0f)))
        m_fOmega -= (dt * MATH_DEG2RAD(2.0f));
    else
        m_fOmega += (dt * MATH_DEG2RAD(2.0f));

    m_fTheta -= (12.0f * (60.0f * (m_fOmega * dt)));
    
    m_vVelocity.y += (CGameProperty::GetGravity() * dt * 3.5f);
    m_vPosition.y += (m_vVelocity.y * dt);
    
    m_fCounter += dt;

    if (m_fCounter > 0.08f)
    {
        m_fTheta = -MATH_DEG2RAD(6.0f);
        m_eStep = STEP_ENDURE;
        m_fOmega = 0.0f;
        m_fCounter = 0.0f;
    };
};


void CCrumbleFloorGimmickMove::Endure(float dt)
{
    m_eStep = STEP_SHAKE;
    m_fCounter += dt;

    if (m_fCounter > 0.6f)
    {
        m_eStep = STEP_SHAKE;
        m_fCounter = 0.0f;
    };
};


void CCrumbleFloorGimmickMove::Shake(float dt)
{
    m_vVelocity.x = Math::Cos(m_fCounter * 90.0f) * 0.0f;
    m_vVelocity.y = Math::Cos(m_fCounter * 90.0f) * 0.0f;
    m_vVelocity.z = Math::Sin(m_fCounter * 90.0f) * 0.0f;

    Math::Vec3_Add(&m_vPosition, &m_vPosition, &m_vVelocity);

    m_fCounter += dt;

    if (m_fCounter > 0.6f)
    {
        m_eStep = STEP_FALLDOWN;
        m_fCounter = 0.0f;
        m_vVelocity.y = 0.0f;

        CGameSound::PlayPositionSE(&m_vPosition, SDCODE_SE(4353));
    };
};


void CCrumbleFloorGimmickMove::Falldown(float dt)
{
    m_vVelocity.y += (CGameProperty::GetGravity() * dt * 0.9f);
    m_vPosition.y += (dt * m_vVelocity.y);
    m_fTheta -= (dt * MATH_DEG2RAD(30.0f));

    if (m_vPosition.y < -100.0f)
    {
        m_eStep = STEP_WAIT;
        m_vVelocity.y = 0.0f;
        m_fTotalMoment = 0.0f;
        m_fTheta = 0.0f;
        m_fCounter = 0.0f;
    };
};


void CCrumbleFloorGimmickMove::GetTheta(RwV3d* pvTheta)
{
    *pvTheta = { 0.0f, 0.0f, m_fTheta };
};


void CCrumbleFloorGimmickMove::GetOmega(RwV3d* pvOmega)
{
    *pvOmega = { 0.0f, 0.0f, m_fOmega };
};


void CCrumbleFloorGimmickMove::GetTotalMoment(RwV3d* pvMoment)
{
    *pvMoment = Math::VECTOR3_ZERO;

    for (int32 i = 0; i < m_nIndex; ++i)
    {
        RwV3d vPosition = m_avPosition[i];
        RwV3dTransformPoint(&vPosition, &vPosition, &m_invMat);

        pvMoment->z +=
            Math::Sin(Math::ATan2(vPosition.x, vPosition.y)) *
            Math::Sqrt(vPosition.x * vPosition.x + vPosition.y * vPosition.y) *
            Math::Cos(m_fTheta);

        m_avPosition[i] = Math::VECTOR3_ZERO;
    };

    m_nIndex = 0;
};


void CCrumbleFloorGimmickMove::SetCharacterPositionFromName(const char* pszName)
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


void CCrumbleFloorGimmickMove::SetMatrixFromClump(RpClump* pClump)
{
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    RwMatrixInvert(&m_invMat, pMatrix);
};