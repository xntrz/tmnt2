#include "Home2D.hpp"
#include "HomeTypes.hpp"
#include "HomeCamera.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Sprite.hpp"


class CHomeSprite : public CSprite
{
public:
    CHomeSprite(void);
    virtual ~CHomeSprite(void);
    void SetTextureAndVirtualSize(RwTexture* pTexture);
};


class CHome2DContainer
{
public:
    CHome2DContainer(CHomeCamera* pHomeCamera);
    ~CHome2DContainer(void);
    void Period(void);
    void Draw(void);
    void DrawDoorName(void);
    void DrawArrow(void);
    void DrawArrow(HOMETYPES::DOORPOSITION doorposition);
    void ResetTime(void);

private:
    float        m_fTimeRate;
    int32        m_iStep;
    CHomeCamera* m_pHomeCamera;
    CHomeSprite  m_sprite;
    RwTexture*   m_apTextureSel[HOMETYPES::DOORKINDNUM];
    RwTexture*   m_apTextureTxt[HOMETYPES::DOORKINDNUM];
    RwTexture*   m_apTextureArr[HOMETYPES::DOORPOSITIONNUM];    
    bool         m_bTextureSettingFlag;
};


CHomeSprite::CHomeSprite(void)
{
    ;
};


CHomeSprite::~CHomeSprite(void)
{
    ;
};


void CHomeSprite::SetTextureAndVirtualSize(RwTexture* pTexture)
{
    ASSERT(pTexture);

    RwRaster* pRaster = RwTextureGetRasterMacro(pTexture);
    ASSERT(pRaster);
    
    SetTexture(pTexture);

    Resize(static_cast<float>(RwRasterGetWidthMacro(pRaster)), 
		   static_cast<float>(RwRasterGetHeightMacro(pRaster)));
};


CHome2DContainer::CHome2DContainer(CHomeCamera* pHomeCamera)
: m_fTimeRate(0.0f)
, m_iStep(0)
, m_pHomeCamera(pHomeCamera)
, m_bTextureSettingFlag(false)
{
    ASSERT(m_pHomeCamera);
    
    std::memset(m_apTextureSel, 0x00, sizeof(m_apTextureSel));
    std::memset(m_apTextureTxt, 0x00, sizeof(m_apTextureTxt));
    std::memset(m_apTextureArr, 0x00, sizeof(m_apTextureArr));
};


CHome2DContainer::~CHome2DContainer(void)
{
    ;
};


void CHome2DContainer::Period(void)
{
    switch (m_iStep)
    {
    case 0:
        {
            m_fTimeRate += CGameProperty::GetElapsedTime();
            if (m_fTimeRate >= 1.0f)
            {
                m_iStep = 1;
                ResetTime();
            };
        }
        break;

    case 1:
        {
            m_fTimeRate += CGameProperty::GetElapsedTime();
            if (m_fTimeRate >= 1.0f)
            {
                m_iStep = 0;
                ResetTime();
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (!m_bTextureSettingFlag)
    {
        m_bTextureSettingFlag = true;

        CTextureManager::SetCurrentTextureSet("Home");
        
        m_apTextureSel[0] = CTextureManager::GetRwTexture("hm_sel_exit");
        m_apTextureSel[1] = CTextureManager::GetRwTexture("hm_sel_april");
        m_apTextureSel[2] = CTextureManager::GetRwTexture("hm_sel_computer");
        m_apTextureSel[3] = CTextureManager::GetRwTexture("hm_sel_tournament");
        
        m_apTextureTxt[0] = CTextureManager::GetRwTexture("hm_txt_exit");
        m_apTextureTxt[1] = CTextureManager::GetRwTexture("hm_txt_april");
        m_apTextureTxt[2] = CTextureManager::GetRwTexture("hm_txt_computer");
        m_apTextureTxt[3] = CTextureManager::GetRwTexture("hm_txt_tounament");
        
        m_apTextureArr[0] = CTextureManager::GetRwTexture("hm_up");
        m_apTextureArr[1] = CTextureManager::GetRwTexture("hm_left");
        m_apTextureArr[2] = CTextureManager::GetRwTexture("hm_down");
        m_apTextureArr[3] = CTextureManager::GetRwTexture("hm_right");
    };
};


void CHome2DContainer::Draw(void)
{
    if (!m_bTextureSettingFlag)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE,    false);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE,  false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE,     rwCULLMODECULLNONE);

    switch (m_pHomeCamera->GetMode())
    {
    case CHomeCamera::MODE_OVERLOOK:
        DrawArrow();
        break;
        
    case CHomeCamera::MODE_DOOR:
        DrawArrow();
        DrawDoorName();
        break;

    default:
        break;
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
};


void CHome2DContainer::DrawDoorName(void)
{
    HOMETYPES::DOORSTATUS doorstatus = m_pHomeCamera->GetDoorStatus(HOMETYPES::DOORPOSITION_FRONT);
    HOMETYPES::DOORKIND doorkind = m_pHomeCamera->GetDoorKind(HOMETYPES::DOORPOSITION_FRONT);
    float fOfsY = 0.0f;
    
    switch (m_iStep)
    {
    case 0:
        {
            fOfsY = (m_fTimeRate * 27.0f) - 30.0f;
        }
        break;

    case 1:
        {
            fOfsY = -3.0f - (m_fTimeRate * 27.0f);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    m_sprite.SetTextureAndVirtualSize(m_apTextureSel[doorkind]);
    m_sprite.Move(-6.0f, fOfsY);
    
    switch (doorstatus)
    {
    case HOMETYPES::DOORSTATUS_OPEN:
        m_sprite.SetRGBA(214, 255, 0, 255);
        break;

    case HOMETYPES::DOORSTATUS_NEW:
        m_sprite.SetRGBA(255, 0, 75, 255);
        break;

    case HOMETYPES::DOORSTATUS_UPDATE:
        m_sprite.SetRGBA(255, 127, 0, 255);
        break;

    default:
        break;
    };
    
    m_sprite.Draw();
};


void CHome2DContainer::DrawArrow(void)
{ 
    if (m_pHomeCamera->GetMode() == CHomeCamera::MODE_OVERLOOK)
        DrawArrow(HOMETYPES::DOORPOSITION_FRONT);

    DrawArrow(HOMETYPES::DOORPOSITION_LEFT);
    DrawArrow(HOMETYPES::DOORPOSITION_BACK);
    DrawArrow(HOMETYPES::DOORPOSITION_RIGHT);
};


void CHome2DContainer::DrawArrow(HOMETYPES::DOORPOSITION doorposition)
{
    HOMETYPES::DOORSTATUS doorstatus = m_pHomeCamera->GetDoorStatus(doorposition);
    if (doorstatus == HOMETYPES::DOORSTATUS_CLOSE)
        return;

	HOMETYPES::DOORKIND doorkind = m_pHomeCamera->GetDoorKind(doorposition);

    switch (doorstatus)
    {
    case HOMETYPES::DOORSTATUS_OPEN:
        m_sprite.SetRGBA(127, 255, 0, 255);
        break;

    case HOMETYPES::DOORSTATUS_NEW:
        m_sprite.SetRGBA(255, 0, 85, 255);
        break;

    case HOMETYPES::DOORSTATUS_UPDATE:
        m_sprite.SetRGBA(255, 127, 0, 255);
        break;

    default:
        ASSERT(false);
        break;
    };

    float x = 0.0f;
    float y = 0.0f;
    
    switch (doorposition)
    {
    case HOMETYPES::DOORPOSITION_FRONT:
        x = 0.0f;
        y = -67.0f;        
        break;

    case HOMETYPES::DOORPOSITION_LEFT:
        x = -217.0f;
        y = -27.0f;
        break;

    case HOMETYPES::DOORPOSITION_BACK:
        x = 2.0f;
        y = 110.0f;
        break;

    case HOMETYPES::DOORPOSITION_RIGHT:
        x = 220.0f;
        y = -26.0f;
        break;

    default:
        ASSERT(false);
        break;
    };

    m_sprite.SetTextureAndVirtualSize(m_apTextureArr[doorposition]);
    m_sprite.Move(x, y);
    m_sprite.Draw();

    switch (doorposition)
    {
    case HOMETYPES::DOORPOSITION_FRONT:
        x = 0.0f;
        y = -118.0f;
        break;

    case HOMETYPES::DOORPOSITION_LEFT:
        x = -208.0f;
        y = 33.0f;
        break;

    case HOMETYPES::DOORPOSITION_BACK:
        x = 0.0f;
        y = 173.0f;
        break;

    case HOMETYPES::DOORPOSITION_RIGHT:
        x = 219.0f;
        y = 33.0f;
        break;

    default:
        ASSERT(false);
        break;
    };

    m_sprite.SetTextureAndVirtualSize(m_apTextureTxt[doorkind]);
    m_sprite.Move(x, y);
    m_sprite.Draw();
};


void CHome2DContainer::ResetTime(void)
{
    m_fTimeRate = 0.0f;
};


static CHome2DContainer* s_pHome2DContainer = nullptr;


static inline CHome2DContainer& Home2DContainer(void)
{
    ASSERT(s_pHome2DContainer);
    return *s_pHome2DContainer;
};


/*static*/ void CHome2D::Initialize(CHomeCamera* pHomeCamera)
{
    if (!s_pHome2DContainer)
        s_pHome2DContainer = new CHome2DContainer(pHomeCamera);
};


/*static*/ void CHome2D::Terminate(void)
{
    if (s_pHome2DContainer)
    {
        delete s_pHome2DContainer;
        s_pHome2DContainer = nullptr;
    };
};


/*static*/ void CHome2D::Period(void)
{
    Home2DContainer().Period();
};


/*static*/ void CHome2D::Draw(void)
{
    Home2DContainer().Draw();
};


/*static*/ void CHome2D::ResetTime(void)
{
    Home2DContainer().ResetTime();
};

