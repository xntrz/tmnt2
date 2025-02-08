#include "ParticleSet.hpp"
#include "Particle.hpp"

#include "Game/System/Texture/TextureManager.hpp"


CParticleSet::CParticleSet(void)
: m_nParticleNum(0)
, m_fEffectPlayTime(0.0f)
, m_bLoop(false)
, m_bTrace(false)
, m_bStringOn(true)
, m_clippingtype(PEFINFO::CLIPPINGTYPE_NOCLIPPING)
, m_fDistance(0.0f)
, m_fEffectStartTime(0.0f)
{
    for (int32 i = 0; i < COUNT_OF(m_apParticle); ++i)
        m_apParticle[i] = nullptr;
    
    m_szEffectName[0] = '\0';
    m_szTexDictName[0] = '\0';
};


CParticleSet::~CParticleSet(void)
{
    Reset();
};


CParticleSet* CParticleSet::Clone(void)
{
    CParticleSet* pParticleSet = new CParticleSet;
    ASSERT(pParticleSet);

    std::memcpy(
        pParticleSet->m_szEffectName,
        m_szEffectName,
        sizeof(pParticleSet->m_szEffectName)
    );
    
    std::memcpy(
        pParticleSet->m_szTexDictName,
        m_szTexDictName,
        sizeof(pParticleSet->m_szTexDictName)
    );

    pParticleSet->m_nParticleNum = m_nParticleNum;
    pParticleSet->m_bLoop = m_bLoop;
    pParticleSet->m_bTrace = m_bTrace;
    pParticleSet->m_bStringOn = m_bStringOn;
    pParticleSet->m_fEffectStartTime = m_fEffectStartTime;

    for (int32 i = 0; i < m_nParticleNum; ++i)
    {
        CParticle* pParticle = new CParticle();
        ASSERT(pParticle);

        pParticle->Copy(m_apParticle[i]);
        pParticleSet->m_apParticle[i] = pParticle;
    };

    return pParticleSet;
};


void CParticleSet::Initialize(void)
{
    for (int32 i = 0; i < m_nParticleNum; ++i)
        m_apParticle[i]->Initialize();
};


void CParticleSet::Read(const void* pBuffer, uint32 uBufferSize)
{
    Reset();
    
    RwMemory MemoryStream;
    MemoryStream.start = (RwUInt8*)pBuffer;
    MemoryStream.length = uBufferSize;

    RwStream* pStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &MemoryStream);
    ASSERT(pStream);
    if (!pStream)
        return;
    
    PEFINFO::SETINFO setinfo = { 0 };
    RwStreamRead(pStream, &setinfo, sizeof(setinfo));

    m_nParticleNum = setinfo.m_nParticleNum;

    PEFINFO::EFFECTINFO effectinfo = { 0 };
    RwStreamRead(pStream, &effectinfo, sizeof(effectinfo));

    std::memcpy(m_szTexDictName, effectinfo.m_szTexDictName, sizeof(m_szTexDictName));
    std::memcpy(m_szEffectName, effectinfo.m_szEffectName, sizeof(m_szEffectName));

    m_fEffectPlayTime = effectinfo.m_fEffectPlayTime;
	m_bLoop = bool(effectinfo.m_bLoop > 0);
	m_bTrace = bool(effectinfo.m_bTrace > 0);
    m_clippingtype = PEFINFO::CLIPPINGTYPE(effectinfo.m_nClippingType);
    m_fDistance = effectinfo.m_fDistance;

    for (int32 i = 0; i < m_nParticleNum; ++i)
    {
        CParticle* pParticle = new CParticle();
        ASSERT(pParticle);

        PEFINFO::PARTICLEINFO particleinfo = { 0 };
        RwStreamRead(pStream, &particleinfo, sizeof(particleinfo));

        pParticle->ReadParticleData(&particleinfo);
        
        RwTexture* pTexture = CTextureManager::GetRwTexture(m_szTexDictName);
        if (pTexture)
        {
            pParticle->SetMyTexture(pTexture);            
        }
        else
        {
            CTextureManager::SetCurrentTextureSet("common_ef");
            bool bResult = pParticle->GetMyTexture();
            ASSERT(bResult);
        };

        pParticle->SetClippingInfo(m_clippingtype, m_fDistance);

		m_apParticle[i] = pParticle;
    };
    
    if (m_bLoop)
        SetLoopPlay(true);

    RwStreamClose(pStream, nullptr);
};


void CParticleSet::Reset(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apParticle); ++i)
    {
        if (m_apParticle[i])
        {
            delete m_apParticle[i];
            m_apParticle[i] = nullptr;
        };            
    };

    m_szEffectName[0] = '\0';
    m_szTexDictName[0] = '\0';

    m_fEffectPlayTime = 0.0f;
    m_nParticleNum = 0;
    m_bLoop = false;
    m_bTrace = false;
    m_bStringOn = true;
};


void CParticleSet::Run(float dt)
{
    for (int32 i = m_nParticleNum; i > 0; --i)
    {
        CParticle* pParticle = m_apParticle[i - 1];
        ASSERT(pParticle);

        pParticle->Run(dt);
    };
};


void CParticleSet::Draw(RwCamera* pCamera)
{
    ASSERT(pCamera);

    for (int32 i = m_nParticleNum; i > 0; --i)
    {
        CParticle* pParticle = m_apParticle[i - 1];
        ASSERT(pParticle);

        if(m_bStringOn || !pParticle->IsStringParticle())
            pParticle->Draw(pCamera);        
    };
};


void CParticleSet::SetGravity(float fGravity)
{
    for (int32 i = m_nParticleNum; i > 0; --i)
        m_apParticle[i - 1]->SetGravity(fGravity);
};


void CParticleSet::SetBasisPositionForAll(const RwV3d* pvPosition)
{
    for (int32 i = m_nParticleNum; i > 0; --i)
        m_apParticle[i - 1]->SetBasisPositionForAll(pvPosition);
};


void CParticleSet::SetBasisPosition(const RwV3d* pvPosition)
{
    for (int32 i = m_nParticleNum; i > 0; --i)
        m_apParticle[i - 1]->SetBasisPosition(pvPosition);
};


void CParticleSet::SetStringEffectOn(bool bSet)
{
    m_bStringOn = bSet;
};


void CParticleSet::SetScale(float fScale)
{
    for (int32 i = 0; i < m_nParticleNum; ++i)
        m_apParticle[i]->SetScale(fScale);
};


void CParticleSet::SetVector(const RwV3d* pvAxisX, const RwV3d* pvAxisY, const RwV3d* pvAxisZ)
{
    for (int32 i = 0; i < m_nParticleNum; ++i)
        m_apParticle[i]->SetVector(pvAxisX, pvAxisY, pvAxisZ);
};


void CParticleSet::Play(void)
{
    for (int32 i = 0; i < m_nParticleNum; ++i)
        m_apParticle[i]->StartParticle();
};


void CParticleSet::SetEmitter(bool bSet)
{
    for (int32 i = 0; i < m_nParticleNum; ++i)
        m_apParticle[i]->SetUpdate(bSet);
};


void CParticleSet::SetLoopPlay(bool bSet)
{
    m_bLoop = bSet;

    for (int32 i = 0; i < m_nParticleNum; ++i)
        m_apParticle[i]->SetLoop(m_bLoop);
};


bool CParticleSet::IsEnd(void) const
{
    int32 nNumEnd = 0;

    for (int32 i = 0; i < m_nParticleNum; ++i)
    {
        if (m_apParticle[i]->IsEndParticle())
            ++nNumEnd;
    };

    return (nNumEnd == m_nParticleNum);
};


bool CParticleSet::IsDrawing(void) const
{
    int32 nNumDrawing = 0;
    
    for (int32 i = 0; i < m_nParticleNum; ++i)
    {
        if (m_apParticle[i]->IsDrawingParticle())
            ++nNumDrawing;
    };

    return (nNumDrawing == m_nParticleNum);
};


bool CParticleSet::IsTrace(void) const
{
    return m_bTrace;
};


void CParticleSet::ConvertEffectInfo(PEFINFO::EFFECTINFO* pEffectInfo)
{
    ;
};
