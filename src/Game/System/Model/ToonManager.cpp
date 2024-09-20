#include "ToonManager.hpp"
#include "ModelToon.hpp"

#include "Game/System/Texture/TextureManager.hpp"

#include "rptoon.h"


class CToonManagerImplement
{
public:
	static const int32 NAME_MAX = 16;

public:
    CToonManagerImplement(void);
    virtual ~CToonManagerImplement(void);
    void RegistModel(CModelToon* pModel);
    void RemoveMode(CModelToon* pModel);
    void SetTextureSet(const char* pszName);

private:
    char m_szTextureSetName[NAME_MAX];
    RpToonPaint* m_apRpToonPaint[CToonManager::PATTERN_MAX];
    CToonManager::PATTERN m_patternCurrent;
    CList<CModelToon> m_listModel;
    RpToonInk* m_pRpToonInkSilhouette;
};


CToonManagerImplement::CToonManagerImplement(void)
: m_patternCurrent(CToonManager::PATTERN_1)
, m_pRpToonInkSilhouette(nullptr)
{
    m_pRpToonInkSilhouette = RpToonInkCreate();
    ASSERT(m_pRpToonInkSilhouette);
    
    if (m_pRpToonInkSilhouette)
    {
        RpToonInkSetOverallThickness(m_pRpToonInkSilhouette, 1.0f);
        RpToonInkSetColor(m_pRpToonInkSilhouette, { 0x00, 0xFF, 0xFF, 0xFF });
        RpToonInkSetName(m_pRpToonInkSilhouette, "silhouette");
    };

    for (int32 i = 0; i < COUNT_OF(m_apRpToonPaint); ++i)
    {
        m_apRpToonPaint[i] = RpToonPaintCreate();
        ASSERT(m_apRpToonPaint[i]);
    };
    
    m_szTextureSetName[0] = '\0';
};


CToonManagerImplement::~CToonManagerImplement(void)
{
	ASSERT(m_listModel.empty());

	for (int32 i = 0; i < COUNT_OF(m_apRpToonPaint); ++i)
	{
		ASSERT(m_apRpToonPaint[i]);

		RpToonPaintDestroy(m_apRpToonPaint[i]);
		m_apRpToonPaint[i] = nullptr;
	};

	ASSERT(m_pRpToonInkSilhouette);
	
	RpToonInkDestroy(m_pRpToonInkSilhouette);
	m_pRpToonInkSilhouette = nullptr;
};


void CToonManagerImplement::RegistModel(CModelToon* pModel)
{
    m_listModel.push_back(pModel);

	CToonManager::PATTERN pattern = pModel->GetPattern();
	ASSERT(
		pattern >= CToonManager::PATTERN_START &&
		pattern < CToonManager::PATTERN_MAX
	);

	pModel->SetToonObject(m_pRpToonInkSilhouette, m_apRpToonPaint[pattern]);
};


void CToonManagerImplement::RemoveMode(CModelToon* pModel)
{
    ASSERT(m_listModel.contains(pModel));
    
    m_listModel.erase(pModel);
};


void CToonManagerImplement::SetTextureSet(const char* pszName)
{
    static const char* s_apszPatternTextureName[] =
    {
        "pat01",
        "pat02",
        "pat03",
        "pat04",
        "pat05",
	};

    ASSERT(std::strlen(pszName) < NAME_MAX);
	std::strcpy(m_szTextureSetName, pszName);
	
	CTextureManager::SetCurrentTextureSet(m_szTextureSetName);

    for (int32 i = 0; i < COUNT_OF(s_apszPatternTextureName); ++i)
    {
        RwTexture* pTexture = CTextureManager::GetRwTexture(s_apszPatternTextureName[i]);
        ASSERT(pTexture);

        RpToonPaintSetGradientTexture(m_apRpToonPaint[i], pTexture);
    };
};


static CToonManagerImplement* s_pToonManagerImpl = nullptr;


static inline CToonManagerImplement& ToonMan(void)
{
    ASSERT(s_pToonManagerImpl);
    return *s_pToonManagerImpl;
};


/*static*/ void CToonManager::Initialize(void)
{
    if (!s_pToonManagerImpl)
    {
        s_pToonManagerImpl = new CToonManagerImplement;
    };
};


/*static*/ void CToonManager::Terminate(void)
{
    if (s_pToonManagerImpl)
    {
        delete s_pToonManagerImpl;
        s_pToonManagerImpl = nullptr;
    };
};


/*static*/ void CToonManager::RegistModel(CModelToon* pModel)
{
    ToonMan().RegistModel(pModel);
};


/*static*/ void CToonManager::RemoveModel(CModelToon* pModel)
{
    ToonMan().RemoveMode(pModel);
};


/*static*/ void CToonManager::SetTextureSet(const char* pszName)
{
    ToonMan().SetTextureSet(pszName);
};

