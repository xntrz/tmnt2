#include "Effect.hpp"
#include "Tracer.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Particle/ParticleSet.hpp"
#include "Game/System/Sound/GameSound.hpp"


CEffect::CEffect(const char* pszName)
: CGameObject(pszName, GAMEOBJECTTYPE::EFFECT)
, m_pParticleSet(nullptr)
, m_vPosition(Math::VECTOR3_ZERO)
, m_pTracer(nullptr)
, m_vOffset(Math::VECTOR3_ZERO)
, m_type(TYPE_NORMAL)
, m_fDirectionOffset(0.0f)
, m_bTraceDirection(false)
, m_bPlay(false)
, m_bSoundPlay(true)
, m_bFinish(false)
, m_bFinishStart(false)
{
    ;
};


CEffect::~CEffect(void)
{
    if (m_pParticleSet)
    {
        m_pParticleSet->Reset();
        delete m_pParticleSet;
        m_pParticleSet = nullptr;

        if (m_bSoundPlay)
            CGameSound::StopEffectSE(this, GetID());
    };

    if (m_pTracer)
    {
        delete m_pTracer;
        m_pTracer = nullptr;
    };
};


void CEffect::Run(void)
{
    ASSERT(m_pParticleSet);
    
    if (!m_bPlay)
        return;

    RunFinish();

    if (m_pTracer)
    {
        RwV3d vPos = Math::VECTOR3_ZERO;
        RwV3d vOffset = Math::VECTOR3_ZERO;

        if (m_bTraceDirection)
        {
            RwMatrix matrix;
            RwMatrixSetIdentityMacro(&matrix);

            Math::Matrix_Update(
                &matrix,
                &Math::VECTOR3_AXIS_X,
                &Math::VECTOR3_AXIS_Y,
                &Math::VECTOR3_AXIS_Z,
                &Math::VECTOR3_ZERO
            );

            Math::Matrix_RotateY(&matrix, GetDirectionFromObject());
            m_pParticleSet->SetVector(&matrix.right, &matrix.up, &matrix.at);
            RwV3dTransformPoint(&vOffset, &m_vOffset, &matrix);
        }
        else
        {
            vOffset = m_vOffset;
        };
        
        if (GetPositionFromObject(&vPos))
        {
            Math::Vec3_Add(&vPos, &vPos, &vOffset);
            SetPosition(&vPos);
        };
    };

    float dt = CGameProperty::GetElapsedTime();
    m_pParticleSet->Run(dt);
};


CEffect* CEffect::Clone(void)
{
    CEffect* pEffect = new CEffect(GetName());
    ASSERT(pEffect);

    pEffect->m_vPosition = m_vPosition;
    pEffect->m_pParticleSet = m_pParticleSet->Clone();

    return pEffect;
};


CParticleSet* CEffect::CloneParticle(void)
{
    ASSERT(m_pParticleSet);
    return m_pParticleSet->Clone();
};


void CEffect::ReadEffect(const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    if (m_pParticleSet)
    {
        m_pParticleSet->Reset();
        delete m_pParticleSet;
        m_pParticleSet = nullptr;
    };

    m_pParticleSet = new CParticleSet();
    ASSERT(m_pParticleSet);

    m_pParticleSet->Read(pBuffer, uBufferSize);
    m_pParticleSet->SetGravity(CGameProperty::GetGravity());
};


void CEffect::Draw(RwCamera* pCamera)
{
    ASSERT(pCamera);
    ASSERT(m_pParticleSet);

    if (m_bPlay)
        m_pParticleSet->Draw(pCamera);
};


EFFECTID::VALUE CEffect::GetID(void) const
{
    return EFFECTID::GetIDFromName(GetName());
};


void CEffect::Start(void)
{
    ASSERT(m_pParticleSet);

    m_bPlay = true;
    m_pParticleSet->Initialize();
    m_pParticleSet->SetGravity(CGameProperty::GetGravity());
    m_pParticleSet->SetBasisPositionForAll(&m_vPosition);
    m_pParticleSet->Play();

    if (m_bSoundPlay)
        CGameSound::PlayEffectSE(this, GetID());    
};


void CEffect::End(void)
{
    Finish();
};


void CEffect::Finish(void)
{
    ASSERT(m_pParticleSet);
    
    m_bFinishStart = true;
    m_pParticleSet->SetEmitter(false);
};


void CEffect::RunFinish(void)
{
    ASSERT(m_pParticleSet);
    
    if (m_bFinishStart && m_pParticleSet->IsDrawing())
        m_bFinish = true;
};


void CEffect::SetDirection(const RwMatrix* pMatrix)
{
    ASSERT(pMatrix);
    ASSERT(m_pParticleSet);

    m_pParticleSet->SetVector(&pMatrix->right, &pMatrix->up, &pMatrix->at);
};


void CEffect::SetDirection(const RwV3d* pvDirection)
{
    ASSERT(pvDirection);
    ASSERT(m_pParticleSet);
    
    RwV3d vBuffer = Math::VECTOR3_ZERO;
    RwV3d vAxisX = Math::VECTOR3_ZERO;
    RwV3d vAxisY = Math::VECTOR3_ZERO;
    RwV3d vAxisZ = Math::VECTOR3_ZERO;

    Math::Vec3_Normalize(&vBuffer, pvDirection);
    vBuffer.y = std::fabs(vBuffer.y);
    
    if (vBuffer.y > 0.3f)
    {
        vBuffer.z = std::fabs(vBuffer.z);
        if (vBuffer.z > 0.3f)
            Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_X, pvDirection);
        else
            Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Z, pvDirection);
        
        Math::Vec3_Cross(&vAxisY, pvDirection, &vAxisX);
    }
    else
    {
        Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Y, pvDirection);
        Math::Vec3_Cross(&vAxisY, pvDirection, &vAxisX);
    };

    Math::Vec3_Normalize(&vAxisX, &vAxisX);
    Math::Vec3_Normalize(&vAxisY, &vAxisY);
    Math::Vec3_Normalize(&vAxisZ, pvDirection);

    m_pParticleSet->SetVector(&vAxisX, &vAxisY, &vAxisZ);
};


void CEffect::SetDirection(float fDirection)
{
    ASSERT(m_pParticleSet);

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);

    Math::Matrix_Update(
        &matrix,
        &Math::VECTOR3_AXIS_X,
        &Math::VECTOR3_AXIS_Y,
        &Math::VECTOR3_AXIS_Z,
        &Math::VECTOR3_ZERO
    );

    Math::Matrix_RotateY(&matrix, fDirection);
    
    m_pParticleSet->SetVector(&matrix.right, &matrix.up, &matrix.at);
};


void CEffect::SetScale(float fScale)
{
    ASSERT(m_pParticleSet);

    m_pParticleSet->SetScale(fScale);
};


void CEffect::SetSoundPlay(bool bPlaySd)
{
    m_bSoundPlay = bPlaySd;
};


void CEffect::SetTracer(CTracer* pTracer, const RwV3d* pvOffset, float fDirection)
{
    ASSERT(pTracer);
    ASSERT(pvOffset);
    
    m_pTracer = pTracer;
    m_vOffset = *pvOffset;
    m_fDirectionOffset = fDirection;
};


void CEffect::SetDirectionTraceFlag(bool bTraceDir)
{
    m_bTraceDirection = bTraceDir;
};


void CEffect::SetStringEffectOn(bool bSet)
{
    ASSERT(m_pParticleSet);
    
    m_pParticleSet->SetStringEffectOn(bSet);
};


void CEffect::SetActive(bool bSet)
{
    ASSERT(m_pParticleSet);
    
    if (bSet)
        CGameSound::PlayEffectSE(this, GetID());
    else
        CGameSound::StopEffectSE(this, GetID());

    m_pParticleSet->SetEmitter(bSet);
};


void CEffect::SetPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    ASSERT(m_pParticleSet);

    m_vPosition = *pvPosition;

    m_pParticleSet->SetBasisPositionForAll(&m_vPosition);
    if (m_pParticleSet->IsTrace())
        m_pParticleSet->SetBasisPosition(&m_vPosition);
};


void CEffect::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);

    *pvPosition = m_vPosition;
};


bool CEffect::GetPositionFromObject(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);
    
    if (m_pTracer)
    {
        m_pTracer->GetPosition(pvPosition);
        return true;
    };

    return false;
};


float CEffect::GetDirectionFromObject(void)
{
    if (m_pTracer)
        return m_pTracer->GetDirection();
    else
        return 0.0f;
};


bool CEffect::IsEnd(void) const
{
    if (!m_pParticleSet || !m_bPlay)
        return false;

    if (m_bFinish)
        return true;

    return m_pParticleSet->IsEnd();
};


CEffect::TYPE CEffect::GetEffectType(void) const
{
    return m_type;
};