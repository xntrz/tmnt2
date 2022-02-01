#include "MotionController.hpp"
#include "Motion.hpp"
#include "MotionManager.hpp"

#include "Game/System/Model/Model.hpp"


CMotionController::CMotionController(CModel* pModel)
: m_pCurrentMotion(nullptr)
, m_pModel(pModel)
, m_pInHierarchy(nullptr)
, m_pOutHierarchy(nullptr)
, m_fTime(0.0f)
, m_fStartTime(0.0f)
, m_fEndTime(0.0f)
, m_fBlendTime(0.0f)
, m_status(STATUS_NORMAL)
, m_playmode(PLAYMODE_REPEAT)
, m_bPause(true)
{
    ASSERT(m_pModel);

    RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyCurrent();
    ASSERT(pHierarchy);
    
    uint32 uFlags = RpHAnimHierarchyGetFlags(pHierarchy);
    uint32 uInFlags = uFlags;
    uint32 uOutFlags = uFlags;

    FLAG_CLEAR(uInFlags, rpHANIMHIERARCHYUPDATELTMS | rpHANIMHIERARCHYUPDATEMODELLINGMATRICES);

    m_pInHierarchy = RpHAnimHierarchyCreateFromHierarchy(
        pHierarchy,
        RpHAnimHierarchyFlag(uInFlags),
        36
    );

    m_pOutHierarchy = RpHAnimHierarchyCreateFromHierarchy(
        pHierarchy,
        RpHAnimHierarchyFlag(uOutFlags),
        36
    );

    ASSERT(m_pInHierarchy);
    ASSERT(m_pOutHierarchy);
    
    RpHAnimHierarchySetKeyFrameCallBacksMacro(m_pOutHierarchy, 1);
    m_pOutHierarchy->parentFrame = pHierarchy->parentFrame;
};


CMotionController::~CMotionController(void)
{
    if (m_pModel->GetHierarchyCurrent() == m_pOutHierarchy)
        m_pModel->SetRpHAnimHierarchy(m_pModel->GetHierarchyOriginal());

    auto it = m_listMotion.begin();
    while (it)
    {
        CMotion* pMotion = &(*it);
        it = m_listMotion.erase(it);

        ASSERT(pMotion);
        delete pMotion;
    };

    if (m_pOutHierarchy)
    {
        RpHAnimHierarchyDestroy(m_pOutHierarchy);
        m_pOutHierarchy = nullptr;
    };

    if (m_pInHierarchy)
    {
        RpHAnimHierarchyDestroy(m_pInHierarchy);
        m_pInHierarchy = nullptr;
    };
};


void CMotionController::AddMotion(const char* pszName)
{
    CMotion* pMotion = CMotionManager::CreateMotion(pszName);
    ASSERT(pMotion);

    m_listMotion.push_back(pMotion);
};


void CMotionController::SetCurrentMotion(const char* pszName, float fStartTime, float fEndTime, float fBlendTime, bool bForce)
{
    ASSERT(pszName);

    CMotion* pMotion = FindMotion(pszName);
    ASSERT(pMotion);

    if (m_pCurrentMotion == pMotion && !bForce)
        return;

    if (m_pCurrentMotion && fBlendTime > 0.0f)
        SetBlendMotion(pMotion, fStartTime, fEndTime, fBlendTime);
    else
        SetMotion(pMotion, fStartTime, fEndTime);

    m_pCurrentMotion = pMotion;
    Update(0.0f);
    m_pModel->SetBoneMatrixInit(true);
};


float CMotionController::GetCurrentMotionEndTime(void) const
{
    ASSERT(m_pCurrentMotion);

    if (m_pCurrentMotion)
        return m_pCurrentMotion->GetEndTime();
    else
        return 0.0f;
};


void CMotionController::Update(float dt)
{
    if (!m_pCurrentMotion)
        return;

    if (!m_bPause)
    {
        switch (m_status)
        {
        case STATUS_NORMAL:
            UpdateMotion(dt);
            break;

        case STATUS_BLEND:
            UpdateBlendMotion(dt);
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    UpdateMatrices();
};


void CMotionController::UpdateMatrices(void)
{
    m_pModel->UpdateFrame();

    if (m_pCurrentMotion)
    {
        RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyCurrent();
        
        ASSERT(pHierarchy);
        
        RpHAnimHierarchyUpdateMatrices(pHierarchy);
    };
};


void CMotionController::SetTime(float fTime)
{
    if (m_pCurrentMotion)
    {
        switch (m_status)
        {
        case STATUS_NORMAL:
            SetTimeMotion(fTime);
            break;

        case STATUS_BLEND:
        default:
            ASSERT(false);
            break;
        };
    };
};


CMotion* CMotionController::FindMotion(const char* pszName)
{
    for (CMotion& it : m_listMotion)
    {
        if (!std::strcmp(it.GetName(), pszName))
            return &it;
    };

    return nullptr;
};


void CMotionController::SetMotion(CMotion* pMotion, float fStartTime, float fEndTime)
{
    ASSERT(pMotion);
    
    m_pModel->SetRpHAnimHierarchy(m_pModel->GetHierarchyOriginal());

    RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyCurrent();
    RtAnimAnimation* pAnimation = pMotion->GetAnimation();

    ASSERT(pHierarchy);
    ASSERT(pAnimation);
    ASSERT(fEndTime <= pMotion->GetEndTime());

    RpHAnimHierarchySetCurrentAnimMacro(pHierarchy, pAnimation);
    RpHAnimHierarchyAttach(pHierarchy);

    if (fStartTime > 0.0f)
        RpHAnimHierarchyAddAnimTimeMacro(pHierarchy, fStartTime);

    if (fEndTime <= 0.0f)
        fEndTime = pMotion->GetEndTime();

    m_fTime         = fStartTime;
    m_fStartTime    = fStartTime;
    m_fEndTime      = fEndTime;
    m_fBlendTime    = 0.0f;
    m_status        = STATUS_NORMAL;
};


void CMotionController::UpdateMotion(float dt)
{
    RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyCurrent();    
    ASSERT(pHierarchy);

    float fTime = m_fTime + dt;
    
    if (fTime > m_fEndTime)
    {
        switch (m_playmode)
        {
        case PLAYMODE_ONESHOT:
            dt = m_fEndTime - fTime;
            fTime = m_fEndTime;
            m_bPause = true;
            break;

        case PLAYMODE_REPEAT:
            dt = 0.0f;
            fTime = fTime - m_fEndTime;
            RpHAnimHierarchySetCurrentAnimTimeMacro(pHierarchy, fTime);
            break;

        default:
            ASSERT(false);
            break;
        };
    };
    
    m_fTime = fTime;
    
    if (dt > 0.0f)
    {
        RpHAnimHierarchyAddAnimTimeMacro(pHierarchy, dt);
    };
};


void CMotionController::SetTimeMotion(float fTime)
{
    ASSERT(fTime >= m_fStartTime && fTime <= m_fEndTime);

    fTime = Math::Clamp(fTime, m_fStartTime, m_fEndTime);

    if (!Math::FEqual(m_fTime, fTime))
    {
        RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyCurrent();
        ASSERT(pHierarchy);

        RpHAnimHierarchySetCurrentAnimTimeMacro(pHierarchy, fTime);
        
        m_fTime = fTime;
    };
};


void CMotionController::SetBlendMotion(CMotion* pMotion, float fStartTime, float fEndTime, float fBlendTime)
{
    ASSERT(pMotion);

    RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyOriginal();
    RtAnimAnimation* pAnimation = pMotion->GetAnimation();
    
    ASSERT(pHierarchy);
    ASSERT(pAnimation);

    RpHAnimHierarchySetCurrentAnimMacro(m_pInHierarchy, pAnimation);
    
    if (fStartTime > 0.0f)
        RpHAnimHierarchyAddAnimTimeMacro(pHierarchy, fStartTime);

    RpHAnimHierarchyUpdateMatrices(m_pInHierarchy);
    
    m_pModel->SetRpHAnimHierarchy(m_pOutHierarchy);

    m_fTime         = 0.0f;
    m_fStartTime    = fStartTime;
    m_fEndTime      = fEndTime;
    m_fBlendTime    = fBlendTime;
    m_status        = STATUS_BLEND;
};


void CMotionController::UpdateBlendMotion(float dt)
{
    ASSERT(m_fBlendTime > 0.0f);

    m_fTime += dt;
    
    if (m_fTime < m_fBlendTime)
    {
        RpHAnimHierarchy* pHierarchy = m_pModel->GetHierarchyOriginal();
        ASSERT(pHierarchy);

        dt = m_fTime / m_fBlendTime;
        RpHAnimHierarchyBlendMacro(m_pOutHierarchy, pHierarchy, m_pInHierarchy, dt);
    }
    else
    {
        float fStartTime = m_fTime - m_fBlendTime + m_fStartTime;
        SetMotion(m_pCurrentMotion, fStartTime, m_fEndTime);
    };
};


void CMotionController::SetPlaymode(PLAYMODE playmode)
{
    m_playmode = playmode;
};


void CMotionController::Pause(void)
{
    ASSERT(m_pCurrentMotion);
    m_bPause = true;
};


void CMotionController::Play(void)
{
    ASSERT(m_pCurrentMotion);
    m_bPause = false;
};


bool CMotionController::IsPaused(void) const
{
    return m_bPause;
};


float CMotionController::GetTime(void) const
{
    return m_fTime;
};


float CMotionController::GetEndTime(void) const
{
    return m_fEndTime;
};


const char* CMotionController::GetMotionName(void) const
{
    if (m_pCurrentMotion)
        return m_pCurrentMotion->GetName();
    else
        return nullptr;
};

CMotionController::PLAYMODE CMotionController::GetPlaymode(void) const
{
    return m_playmode;
};


CMotion* CMotionController::GetCurrentMotion(void) const
{
    return m_pCurrentMotion;
};