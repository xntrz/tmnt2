#include "MapClumpModelMan.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "System/Common/RenderState.hpp"


class CMapClumpModelContainer
{
private:
    enum MODE
    {
        MODE_NONE = 0,
        MODE_ADD,
        MODE_SUB,
        MODE_ALPHA,

        MODEMAX,

        MODE_DEFAULT = MODE_NONE,
    };

    class CMapModelInfo : public CListNode<CMapModelInfo>
    {
    public:
        CMapModelInfo(void);
        ~CMapModelInfo(void);
        void Cleanup(void);
        void SetName(const char* pszName, MODE mode);
        void SetModel(CModel* pModel, void* hUVAnim);
        void SetDrawEnable(bool bEnable);
        void Draw(void);
        const char* GetName(void) const;

    private:
        CModel* m_pModel;
        void* m_hUVAnim;
        MODE m_mode;
        char m_szName[16];
        bool m_bDrawEnable;
    };
    
public:
    CMapClumpModelContainer(void);
    ~CMapClumpModelContainer(void);
    void Regist(const char* pszName, int32 mode);
    void CreateModel(void);
    void Draw(void);
    void SetEnableDraw(const char* pszName, bool bEnable);

private:
    CMapModelInfo* findModelInfoFromName(const char* pszName);

private:
    CMapModelInfo m_aMapModelInfo[32];
    CList<CMapModelInfo> m_listFree;
    CList<CMapModelInfo> m_listAlloc;
};


CMapClumpModelContainer::CMapModelInfo::CMapModelInfo(void)
: m_pModel(nullptr)
, m_hUVAnim(nullptr)
, m_mode(MODE_ALPHA)
, m_bDrawEnable(true)
{
    m_szName[0] = '\0';
};


CMapClumpModelContainer::CMapModelInfo::~CMapModelInfo(void)
{
    Cleanup();
};


void CMapClumpModelContainer::CMapModelInfo::Cleanup(void)
{
    if (m_hUVAnim)
    {
        CUVAnim::DestroyAnimatedMaterialsList(m_hUVAnim);
        m_hUVAnim = nullptr;
    };

    if (m_pModel)
    {
        CModelManager::DestroyModel(m_pModel);
        m_pModel = nullptr;
    };
};


void CMapClumpModelContainer::CMapModelInfo::SetName(const char* pszName, MODE mode)
{
    ASSERT(pszName);
    ASSERT(std::strlen(pszName) < COUNT_OF(m_szName));
    ASSERT(mode >= 0 && mode < MODEMAX);

    std::strcpy(m_szName, pszName);
    m_mode = mode;
};


void CMapClumpModelContainer::CMapModelInfo::SetModel(CModel* pModel, void* hUVAnim)
{
    ASSERT(pModel);

    m_hUVAnim = hUVAnim;
    m_pModel = pModel;

    m_pModel->SetPosition(Math::VECTOR3_ZERO);
    m_pModel->UpdateFrame();
    m_pModel->SetLightingEnable(false);
    m_pModel->SetClippingEnable(false);
};


void CMapClumpModelContainer::CMapModelInfo::SetDrawEnable(bool bEnable)
{
    m_bDrawEnable = bEnable;
};


void CMapClumpModelContainer::CMapModelInfo::Draw(void)
{
    if (!m_bDrawEnable)
        return;

    if (m_hUVAnim)
        CUVAnim::UpdateUVAnim(m_hUVAnim, CGameProperty::GetElapsedTime());    

    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    
    switch (m_mode)
    {
    case MODE_NONE:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;

    case MODE_ADD:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        break;

    case MODE_SUB:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDZERO);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;

    case MODE_ALPHA:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;

    default:
        ASSERT(FALSE);
        break;
    };

    ASSERT(m_pModel);
    if (m_pModel)
        m_pModel->Draw();

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
};


const char* CMapClumpModelContainer::CMapModelInfo::GetName(void) const
{
    return m_szName;
};


CMapClumpModelContainer::CMapClumpModelContainer(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aMapModelInfo); ++i)
        m_listFree.push_back(&m_aMapModelInfo[i]);
};


CMapClumpModelContainer::~CMapClumpModelContainer(void)
{
    for (CMapModelInfo& it : m_listAlloc)
        it.Cleanup();
};


void CMapClumpModelContainer::Regist(const char* pszName, int32 mode)
{
    ASSERT(!m_listFree.empty());

    CMapModelInfo* pNode = m_listFree.front();
    m_listFree.erase(pNode);
    m_listAlloc.push_back(pNode);

    pNode->SetName(pszName, MODE(mode));
};


void CMapClumpModelContainer::CreateModel(void)
{
    for (CMapModelInfo& it : m_listAlloc)
    {
        void* hUVAnim = nullptr;
        CModel* pModel = CModelManager::CreateModel(it.GetName());
        
        ASSERT(pModel);
        
        if (pModel)
        {
            if (CUVAnim::IsExistsUVAnimDict(it.GetName()))
            {
                hUVAnim = CUVAnim::CreateAnimatedMaterialsList(pModel->GetClump());
                ASSERT(hUVAnim);
                if (hUVAnim)
                    CUVAnim::InitUVAnim(hUVAnim);
            };

            it.SetModel(pModel, hUVAnim);            
        };
    };
};


void CMapClumpModelContainer::Draw(void)
{
    for (CMapModelInfo& it : m_listAlloc)
        it.Draw();
};


void CMapClumpModelContainer::SetEnableDraw(const char* pszName, bool bEnable)
{
    CMapModelInfo* pMapModelInfo = findModelInfoFromName(pszName);
    ASSERT(pMapModelInfo);

    if (pMapModelInfo)
        pMapModelInfo->SetDrawEnable(bEnable);
};


CMapClumpModelContainer::CMapModelInfo* CMapClumpModelContainer::findModelInfoFromName(const char* pszName)
{
    for (CMapModelInfo& it : m_listAlloc)
    {
        if (!std::strcmp(it.GetName(), pszName))
            return &it;
    };

    return nullptr;
};


static CMapClumpModelContainer* s_pMapClumpModelContainer = nullptr;


static inline CMapClumpModelContainer& MapClumpContainer(void)
{
    ASSERT(s_pMapClumpModelContainer);
    return *s_pMapClumpModelContainer;
};


/*static*/ void CMapClumpModelManager::Initialize(void)
{
    if (!s_pMapClumpModelContainer)
    {
        s_pMapClumpModelContainer = new CMapClumpModelContainer;
    };
};


/*static*/ void CMapClumpModelManager::Terminate(void)
{
    if (s_pMapClumpModelContainer)
    {
        delete s_pMapClumpModelContainer;
        s_pMapClumpModelContainer = nullptr;
    };
};


/*static*/ void CMapClumpModelManager::Read(const char* pszName, int32 mode)
{
    MapClumpContainer().Regist(pszName, mode);
};


/*static*/ void CMapClumpModelManager::Draw(void)
{
    MapClumpContainer().Draw();
};


/*static*/ void CMapClumpModelManager::CreateModel(void)
{
    MapClumpContainer().CreateModel();
};


/*static*/ void CMapClumpModelManager::SetDrawEnable(const char* pszName, bool bEnable)
{
    MapClumpContainer().SetEnableDraw(pszName, bEnable);
};