#include "LocusModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/MotionParameter/MotionParameterController.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Particle/Locus.hpp"
#include "Game/System/Texture/TextureManager.hpp"


static const char* LOCUSMODULE_TXD_NAME = "common_ef";
static const char* LOCUSMODULE_TEXTURE_NAME = "sword";


CLocusModule::CLocusModule(CModel* pModel, LOCUSPARAMETER* pLocusParameter, int32 nNumLocus)
: IModule(MODULETYPE::LOCUS)
, m_pModel(pModel)
, m_nNumLocus(nNumLocus)
, m_vPositionHigh(Math::VECTOR3_ZERO)
, m_vPositionLow(Math::VECTOR3_ZERO)
{
    for (int32 i = 0; i < COUNT_OF(m_apLocus); ++i)
        m_apLocus[i] = nullptr;

    std::memset(m_aLocusParameter, 0x00, sizeof(m_aLocusParameter));

    ASSERT(m_pModel);
    ASSERT(m_nNumLocus <= LOCUS_NUM_MAX);

    CTextureManager::SetCurrentTextureSet(LOCUSMODULE_TXD_NAME);
    RwTexture* pTexture = CTextureManager::GetRwTexture(LOCUSMODULE_TEXTURE_NAME);
    ASSERT(pTexture);

    for (int32 i = 0; i < m_nNumLocus; ++i)
    {
        LOCUSPARAMETER* pParameter = &pLocusParameter[i];

        m_apLocus[i] = new CLocus(20, { 0xFF, 0xFF,0xFF,0xFF });
        m_apLocus[i]->SetTexture(pTexture);
        m_apLocus[i]->SetColor(pParameter->m_Color);
        m_apLocus[i]->SetAlphaBasis(pParameter->m_Color.alpha);

        m_aLocusParameter[i].m_Color = pParameter->m_Color;
        m_aLocusParameter[i].m_nBoneID = pParameter->m_nBoneID;
        m_aLocusParameter[i].m_vPositionHigh = pParameter->m_vPositionHigh;
        m_aLocusParameter[i].m_vPositionLow = pParameter->m_vPositionLow;
        m_aLocusParameter[i].m_bTrace = false;
        m_aLocusParameter[i].m_bMove = false;
    };

    CLocusModuleManager::Regist(this);
};


CLocusModule::~CLocusModule(void)
{
    CLocusModuleManager::Remove(this);

    for (int32 i = 0; i < m_nNumLocus; ++i)
    {
        ASSERT(m_apLocus[i]);
        delete m_apLocus[i];
        m_apLocus[i] = nullptr;
    };

    m_nNumLocus = 0;
};


void CLocusModule::Run(void)
{
    for (int32 i = 0; i < m_nNumLocus; ++i)
    {
        LOCUSMODULEPARAMETER* pParameter = &m_aLocusParameter[i];

        if (!pParameter->m_bMove)
            continue;

        CLocus* pLocus = m_apLocus[i];
        ASSERT(pLocus);
        
        if (pParameter->m_bTrace)
        {
            RwMatrix* pMatrix = m_pModel->GetBoneMatrixFromID(pParameter->m_nBoneID);

            RwV3d avPt[2] =
            {
                pParameter->m_vPositionHigh,
                pParameter->m_vPositionLow,
            };

            RwV3dTransformPoints(avPt, avPt, COUNT_OF(avPt), pMatrix);

            m_vPositionHigh = avPt[0];
            m_vPositionLow = avPt[1];

            pLocus->SetPosition(&m_vPositionHigh, &m_vPositionLow);
        }
        else
        {
            pLocus->DeletePair();
        };
    };
};


void CLocusModule::Draw(void)
{
    ;
};


void CLocusModule::StartIndexedLocus(int32 nIndex)
{
    CLocus* pLocus = m_apLocus[nIndex];
    ASSERT(pLocus);
    pLocus->Reset();
    pLocus->SetDrawEnable(true);

    m_aLocusParameter[nIndex].m_bMove = true;
    m_aLocusParameter[nIndex].m_bTrace = true;
};


void CLocusModule::StopIndexedLocus(int32 nIndex)
{
    CLocus* pLocus = m_apLocus[nIndex];
    ASSERT(pLocus);
    pLocus->Reset();
    pLocus->SetDrawEnable(false);

    m_aLocusParameter[nIndex].m_bMove = false;
    m_aLocusParameter[nIndex].m_bTrace = false;
};


void CLocusModule::Reset(void)
{
    for (int32 i = 0; i < m_nNumLocus; ++i)
        StopIndexedLocus(i);
};


void CLocusModule::DrawLocus(void)
{
    for (int32 i = 0; i < m_nNumLocus; ++i)
    {
        if (!m_aLocusParameter[i].m_bMove)
            continue;
            
        CLocus* pLocus = m_apLocus[i];
        ASSERT(pLocus);
        pLocus->Draw();
    };
};


//
// *********************************************************************************
//


/*static*/ CLocusModuleForCharacter* CLocusModuleForCharacter::New(CCharacter* pCharacter)
{    
    ASSERT(pCharacter);

    CMotionParameterController* pMPController = pCharacter->GetMotionParameterController();
    ASSERT(pMPController);

    int32 nNumLocus = pMPController->GetNumLocus();
    if (!nNumLocus)
        return nullptr;
    
    ASSERT(nNumLocus <= LOCUS_NUM_MAX);

    LOCUSPARAMETER aLocusParameter[LOCUS_NUM_MAX] = {};

    for (int32 i = 0; i < nNumLocus; ++i)
    {
        CMotionParameterController::LOCUSPARAMETER MPLocusParameter = {};
        
        pMPController->GetLocusParameter(i, &MPLocusParameter);
        
        aLocusParameter[i].m_nBoneID = MPLocusParameter.m_nBoneID;
        aLocusParameter[i].m_Color = MPLocusParameter.m_Color;
        aLocusParameter[i].m_vPositionHigh = MPLocusParameter.m_vPositionHigh;
        aLocusParameter[i].m_vPositionLow = MPLocusParameter.m_vPositionLow;
    };

    CLocusModuleForCharacter* pModule = new CLocusModuleForCharacter(pCharacter, aLocusParameter, nNumLocus);
    return pModule;
};


CLocusModuleForCharacter::CLocusModuleForCharacter(CCharacter* pCharacter, LOCUSPARAMETER* pLocusParameter, int32 nNumLocus)
: CLocusModule(pCharacter->GetModel(), pLocusParameter, nNumLocus)
, m_pCharacter(pCharacter)
{
    ;
};


CLocusModuleForCharacter::~CLocusModuleForCharacter(void)
{
    ;
};


void CLocusModuleForCharacter::Run(void)
{
    ASSERT(m_pCharacter);
    
    float fNowTime = m_pCharacter->GetMotionTime();
    float fPrevTime = fNowTime - CGameProperty::GetElapsedTime();

    if (fPrevTime < 0.0f)
        fPrevTime = 0.0f;

    CMotionParameterController* pMPController = m_pCharacter->GetMotionParameterController();
    ASSERT(pMPController);

    int32 nNumDrawLocus = pMPController->GetNumDrawLocusData(fPrevTime, fNowTime);
    for (int32 i = 0; i < nNumDrawLocus; ++i)
    {
        CMotionParameterController::DRAW_LOCUS DrawLocus = {};
        pMPController->GetDrawLocusData(i, &DrawLocus);

        if (DrawLocus.m_bDraw)
            StartIndexedLocus(DrawLocus.m_nLocus);
        else
            StopIndexedLocus(DrawLocus.m_nLocus);
    };

    CLocusModule::Run();
};


//
// *********************************************************************************
//


class CLocusModuleContainer
{
public:
    static const int32 LOCUSWORK_MAX = 10;

    struct LOCUSWORK : public CListNode<LOCUSWORK>
    {
        CLocusModule* m_pModule;
    };

public:
    CLocusModuleContainer(void);
    ~CLocusModuleContainer(void);
    void Run(void);
    void Draw(void);
    void Regist(CLocusModule* pLocusModule);
    void Remove(CLocusModule* pLocusModule);

private:
    LOCUSWORK m_aLocusWork[LOCUSWORK_MAX];
    CList<LOCUSWORK> m_listWorkPool;
    CList<LOCUSWORK> m_listWorkAlloc;
};


CLocusModuleContainer::CLocusModuleContainer(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLocusWork); ++i)
    {
        m_aLocusWork[i].m_pModule = nullptr;
        m_listWorkPool.push_back(&m_aLocusWork[i]);
    };
};


CLocusModuleContainer::~CLocusModuleContainer(void)
{
    ASSERT(m_listWorkAlloc.empty());
};


void CLocusModuleContainer::Run(void)
{
    ;
};


void CLocusModuleContainer::Draw(void)
{
    for (LOCUSWORK& it : m_listWorkAlloc)
        it.m_pModule->DrawLocus();
};


void CLocusModuleContainer::Regist(CLocusModule* pLocusModule)
{
    ASSERT(pLocusModule);
    ASSERT(!m_listWorkPool.empty());

    LOCUSWORK* pNode = m_listWorkPool.front();    
    m_listWorkPool.erase(pNode);
    m_listWorkAlloc.push_back(pNode);

    pNode->m_pModule = pLocusModule;
};


void CLocusModuleContainer::Remove(CLocusModule* pLocusModule)
{
    ASSERT(pLocusModule);
    ASSERT(!m_listWorkAlloc.empty());

    LOCUSWORK* pWork = nullptr;
    
    for (LOCUSWORK& it : m_listWorkAlloc)
    {
        if (it.m_pModule == pLocusModule)
        {
            pWork = &it;
            pWork->m_pModule = nullptr;

            m_listWorkAlloc.erase(pWork);
            m_listWorkPool.push_back(pWork);

            return;
        };
    };
};


static CLocusModuleContainer* s_pLocusModuleContainer = nullptr;


static inline CLocusModuleContainer& LocusModuleContainer(void)
{
    ASSERT(s_pLocusModuleContainer);
    return *s_pLocusModuleContainer;
};


/*static*/ void CLocusModuleManager::Initialize(void)
{
    if (!s_pLocusModuleContainer)
    {
        s_pLocusModuleContainer = new CLocusModuleContainer;
    };
};


/*static*/ void CLocusModuleManager::Terminate(void)
{
    if (s_pLocusModuleContainer)
    {
        delete s_pLocusModuleContainer;
        s_pLocusModuleContainer = nullptr;
    };
};


/*static*/ void CLocusModuleManager::Draw(void)
{
    LocusModuleContainer().Draw();
};


/*static*/ void CLocusModuleManager::Run(void)
{
    LocusModuleContainer().Run();
};


/*static*/ void CLocusModuleManager::Regist(CLocusModule* pLocusModule)
{
    LocusModuleContainer().Regist(pLocusModule);
};


/*static*/ void CLocusModuleManager::Remove(CLocusModule* pLocusModule)
{
    LocusModuleContainer().Remove(pLocusModule);
};