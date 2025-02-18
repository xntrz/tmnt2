#include "MessageManager.hpp"
#include "VoiceManager.hpp"
#include "GameSound.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Sprite.hpp"


static const float APPEAR_TIME      = 0.5f;
static const float SHOW_TIME        = 4.0f;
static const float DISAPPEAR_TIME   = 0.5f;

static const RwV2d s_vBasePosition      = {   0.0f,  336.0f };
static const RwV2d s_vStringBoxWH       = { 488.0f,   70.0f };
static const RwV3d s_vStringBoxOffset   = {  75.0f,   72.0f };

static const RwRGBA s_initialTextRGBA   = { 0xFF, 0xFF, 0xFF, 0x00 };
static const RwRGBA s_lastTextRGBA      = { 0xFF, 0xFF, 0xFF, 0xFF };
static const RwRGBA s_initialVertexRGBA = { 0x00 };

static const RwRGBA s_aLastVerexRGBA[6 * 2] =
{
    { 0x00, 0x00, 0x00, 0x36 },
    { 0x00, 0x00, 0x00, 0x36 },
    { 0x00, 0x00, 0x00, 0x36 },
    { 0x00, 0x00, 0x00, 0x36 },
    { 0x00 },
    { 0x00 },

    { 0x00, 0x00, 0x00, 0xD6 },
    { 0x00, 0x00, 0x00, 0xD6 },
    { 0x00, 0x00, 0x00, 0x59 },
    { 0x00, 0x00, 0x00, 0x59 },
    { 0x00 },
    { 0x00 },
};


class CMessageContainer
{
private:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_APPEAR,
        STATE_SHOW,
        STATE_DISAPPEAR,
    };
    
public:
    CMessageContainer(void);
    ~CMessageContainer(void);
    void Period(void);
    void Draw(void);
    bool IsDrawing(void);
    bool Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer);
    bool Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer, bool bIsHelpID);
    bool OnlyTextRequest(SEGROUPID::VALUE idGroup);
    
private:
    void drawWindow(void);
    void drawMessage(void);
    void messageFadeIn(void);
    void messageFadeOut(void);
    void messageSet(void);
    void colorLinerMove(uint8*, uint8, uint8);
    void vertexColorLinerMove(bool bFadeInFlag);
    void stringColorLinerMove(bool bFadeInFlag);
    void vertexSet(void);
    void rsPush(void);
    void rsPop(void);
    int32 getStageKind(void);
    float getStringBoxHeight(void);
    bool isHelpEnable(void);
    bool isValidID(SEGROUPID::VALUE idGroup);
    
private:
    SEGROUPID::VALUE m_seGroupId;
    float            m_fTimer;
    RwIm2DVertex     m_aVertices[6];
    RwRGBA           m_aVerticesColor[6];
    RwRGBA           m_msgColor;
    Rt2dBBox         m_msgBBox;
    STATE            m_state;
    bool             m_abSeGroupIdPlayFlag[SEGROUPID::ID_MAX];
    int32            m_stageKind;
    int32            m_numStringLines;
};


CMessageContainer::CMessageContainer(void)
: m_seGroupId(SEGROUPID::ID_INVALID)
, m_fTimer(0.0f)
, m_aVertices()
, m_aVerticesColor()
, m_msgColor()
, m_msgBBox()
, m_state(STATE_IDLE)
, m_abSeGroupIdPlayFlag()
, m_stageKind(getStageKind())
, m_numStringLines(0)
{
    std::memset(m_aVertices, 0, sizeof(m_aVertices));
    std::memset(m_aVerticesColor, 0, sizeof(m_aVerticesColor));
    
    for (int32 i = 0; i < COUNT_OF(m_abSeGroupIdPlayFlag); ++i)
        m_abSeGroupIdPlayFlag[i] = false;
};


CMessageContainer::~CMessageContainer(void)
{
    ;
};


void CMessageContainer::Period(void)
{
    switch (m_state)
    {
    case STATE_IDLE:
        break;

    case STATE_APPEAR:
        {
            if (m_fTimer < APPEAR_TIME)
            {
                messageFadeIn();
                m_fTimer += CScreen::TimerStride();
            }
            else
            {
                m_fTimer -= APPEAR_TIME;
                m_state = STATE_SHOW;
            };
        }
        break;

    case STATE_SHOW:
        {
            if (m_fTimer < SHOW_TIME)
            {
                m_fTimer += CScreen::TimerStride();
            }
            else
            {
                m_fTimer -= SHOW_TIME;
                m_state = STATE_DISAPPEAR;
            };
        }
        break;

    case STATE_DISAPPEAR:
        {
            if (m_fTimer < DISAPPEAR_TIME)
            {
                messageFadeOut();
                m_fTimer += CScreen::TimerStride();
            }
            else
            {
                m_fTimer -= DISAPPEAR_TIME;
                m_state = STATE_IDLE;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CMessageContainer::Draw(void)
{
    if (IsDrawing())
    {
        rsPush();
        drawWindow();
        drawMessage();
        rsPop();
    };
};


bool CMessageContainer::IsDrawing(void)
{
    return (m_state != STATE_IDLE);
};


bool CMessageContainer::Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer)
{
    return Request(idGroup, idPlayer, SEGROUPID::IsHelpID(idGroup));
};


bool CMessageContainer::Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer, bool bIsHelpID)
{
    if (isValidID(idGroup))
    {
        if (CVoiceManager::SetVoice(idGroup, idPlayer, bIsHelpID))
        {
            OnlyTextRequest(idGroup);
            m_abSeGroupIdPlayFlag[idGroup] = true;
            return true;
        };
    };

    return false;
};


bool CMessageContainer::OnlyTextRequest(SEGROUPID::VALUE idGroup)
{
    if (!isHelpEnable())
        return false;    

    if (!SEGROUPID::GetGameText(idGroup))
        return false;

    m_seGroupId = idGroup;
    m_fTimer = 0.0f;
    m_state = STATE_APPEAR;
    
    messageSet();
    vertexSet();
    
    for (int32 i = 0; i < COUNT_OF(m_aVerticesColor); ++i)
        m_aVerticesColor[i] = s_initialVertexRGBA;
    
    m_msgColor = { 0xFF, 0xFF, 0xFF, 0x00 };
    
    CGameSound::PlaySE(SDCODE_SE(0x100D));

    return true;
};


void CMessageContainer::drawWindow(void)
{
    if (CCamera::CameraCurrent())
    {
        for (int32 i = 0; i < COUNT_OF(m_aVertices); ++i)
            m_aVertices[i].emissiveColor = RWRGBALONGEX(m_aVerticesColor[i]);

        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_aVertices, COUNT_OF(m_aVertices));
    };
};


void CMessageContainer::drawMessage(void)
{
#ifdef TMNT2_BUILD_EU
    CGameFont::SetHeightScaled(1.75f);
#else /* TMNT2_BUILD_EU */
    CGameFont::SetHeightScaled(2.0f);
#endif /* TMNT2_BUILD_EU */        
    CGameFont::SetRGBA(m_msgColor);

    GAMETEXT helpTextId = SEGROUPID::GetGameText(m_seGroupId);
    const wchar* pwszText = CGameText::GetText(helpTextId);
    
    CGameFont::Flow(pwszText, &m_msgBBox);
};


void CMessageContainer::messageFadeIn(void)
{
    vertexColorLinerMove(true);
    stringColorLinerMove(true);
};


void CMessageContainer::messageFadeOut(void)
{
    vertexColorLinerMove(false);
    stringColorLinerMove(false);
};


void CMessageContainer::colorLinerMove(uint8* result, uint8 start, uint8 end)
{
    *result = static_cast<uint8>(Math::Lerp(static_cast<float>(start),
                                            static_cast<float>(end),
                                            m_fTimer * 2.0f));
};


void CMessageContainer::vertexColorLinerMove(bool bFadeInFlag)
{
    for (int32 i = 0; i < COUNT_OF(m_aVerticesColor); ++i)
    {
        RwRGBA colorStart = (bFadeInFlag ? s_initialVertexRGBA                   : s_aLastVerexRGBA[i + 6 * m_stageKind]);
        RwRGBA colorEnd   = (bFadeInFlag ? s_aLastVerexRGBA[i + 6 * m_stageKind] : s_initialVertexRGBA);        

        colorLinerMove(&m_aVerticesColor[i].red,    colorStart.red,     colorEnd.red);
        colorLinerMove(&m_aVerticesColor[i].green,  colorStart.green,   colorEnd.green);
        colorLinerMove(&m_aVerticesColor[i].blue,   colorStart.blue,    colorEnd.blue);
        colorLinerMove(&m_aVerticesColor[i].alpha,  colorStart.alpha,   colorEnd.alpha); 
    };
};


void CMessageContainer::stringColorLinerMove(bool bFadeInFlag)
{
    RwRGBA colorStart = (bFadeInFlag ? s_initialTextRGBA : s_lastTextRGBA);
    RwRGBA colorEnd   = (bFadeInFlag ? s_lastTextRGBA    : s_initialTextRGBA);

    colorLinerMove(&m_msgColor.red,     colorStart.red,     colorEnd.red);
    colorLinerMove(&m_msgColor.green,   colorStart.green,   colorEnd.green);
    colorLinerMove(&m_msgColor.blue,    colorStart.blue,    colorEnd.blue);
    colorLinerMove(&m_msgColor.alpha,   colorStart.alpha,   colorEnd.alpha);
};


void CMessageContainer::messageSet(void)
{
    float fScrW = static_cast<float>(CScreen::Width());
    float fScrH = static_cast<float>(CScreen::Height());

    float x = (s_vBasePosition.x + s_vStringBoxOffset.x);
    float y = (s_vBasePosition.y + s_vStringBoxOffset.y);

    m_msgBBox.x =  (x * (fScrW / TYPEDEF::VSCR_W)) * (CSprite::m_fVirtualScreenW / fScrW) - -CSprite::m_fVirtualScreenX;
    m_msgBBox.y = ((y * (fScrH / TYPEDEF::VSCR_H)) * (CSprite::m_fVirtualScreenH / fScrH) - -CSprite::m_fVirtualScreenY) * -1.0f;
    m_msgBBox.w = s_vStringBoxWH.x;
    m_msgBBox.h = getStringBoxHeight();
};


void CMessageContainer::vertexSet(void)
{
	float fScrW = static_cast<float>(CScreen::Width());
    float fScrH = static_cast<float>(CScreen::Height());
    
    float fOfsY = 0.0f;
    float fHeight = CGameFont::GetHeight();
    
    if (m_numStringLines < 4)
    {
        fOfsY = s_vBasePosition.y;
    }
    else
    {
        fOfsY = s_vBasePosition.y - (static_cast<float>(m_numStringLines - 3) * fHeight);
        if (fOfsY < 0.0f)
            fOfsY = 0.0f;
    };

    float x0 = s_vBasePosition.x;
    float x1 = s_vBasePosition.x + fScrW;

    float y0 = fOfsY + (fScrH - fOfsY);
    float y1 = fOfsY + (0.1f * (fScrH - fOfsY));

    float z = RwIm2DGetNearScreenZMacro();
    
    RwV2d aPos[] =
    {
        { x0, y0    },
        { x1, y0    },
        { x0, y1    },
        { x1, y1    },
        { x0, fOfsY },
        { x1, fOfsY },
    };

    static_assert(COUNT_OF(aPos) == COUNT_OF(m_aVertices), "update table");
    
    for (int32 i = 0; i < COUNT_OF(aPos); ++i)
    {
        m_aVertices[i].x = aPos[i].x * (fScrW / TYPEDEF::VSCR_W); // to real screen pos
		m_aVertices[i].y = aPos[i].y * (fScrH / TYPEDEF::VSCR_H); // to real screen pos
        m_aVertices[i].z = z;
    };
};


void CMessageContainer::rsPush(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE,       false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE,      false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND,          rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND,         rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE,         false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGCOLOR,          0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE,          rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER,     0);
};


void CMessageContainer::rsPop(void)
{
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGCOLOR);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


int32 CMessageContainer::getStageKind(void)
{
    STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();
    switch (idStage)
    {
    case STAGEID::ID_ST02NB:
    case STAGEID::ID_ST04NB:
    case STAGEID::ID_ST18FB:
    case STAGEID::ID_ST20FB:
    case STAGEID::ID_ST30NB:
    case STAGEID::ID_ST31NB:
    case STAGEID::ID_ST38NB:
    case STAGEID::ID_ST40OB:
    case STAGEID::ID_ST42NB:
    case STAGEID::ID_ST44NB:
    case STAGEID::ID_ST47OB:
    case STAGEID::ID_ST50NB:
    case STAGEID::ID_ST52FB:
    case STAGEID::ID_ST53FB:
    case STAGEID::ID_ST54FB:
    case STAGEID::ID_ST55FB:
    case STAGEID::ID_ST56NB:
    case STAGEID::ID_ST57NB:
    case STAGEID::ID_ST58OB1:
    case STAGEID::ID_ST58OB2:
        return 1;

    default:
        return 0;
    };
};


float CMessageContainer::getStringBoxHeight(void)
{
    CGameFont::SetHeightScaled(2.0f);

    GAMETEXT helpTextId = SEGROUPID::GetGameText(m_seGroupId);
    const wchar* pwszText = CGameText::GetText(helpTextId);
    
    m_numStringLines = CGameFont::CountFlowLine(pwszText, s_vStringBoxWH.x);
    if (m_numStringLines < 3)
        return 70.0f;

    float fResult = static_cast<float>(m_numStringLines - 3) * CGameFont::GetHeight() + 70.0f;
    ASSERT(fResult <= 400.0f, "%d", (m_numStringLines - 3));

    return fResult;
};


bool CMessageContainer::isHelpEnable(void)
{
    return CGameData::Option().Display().IsHelpEnabled();
};


bool CMessageContainer::isValidID(SEGROUPID::VALUE idGroup)
{
    if (isHelpEnable() || !SEGROUPID::IsHelpID(idGroup))
    {
        if (SEGROUPID::IsCallFree(idGroup) || !m_abSeGroupIdPlayFlag[idGroup])
            return true;
    };

    return false;
};


static CMessageContainer* s_pMessageContainer = nullptr;


static inline CMessageContainer& MessageContainer(void)
{
    ASSERT(s_pMessageContainer);
    return *s_pMessageContainer;
};


/*static*/ void CMessageManager::Initialize(void)
{
    if (!s_pMessageContainer)
        s_pMessageContainer = new CMessageContainer;
};


/*static*/ void CMessageManager::Terminate(void)
{
    if (s_pMessageContainer)
    {
        delete s_pMessageContainer;
        s_pMessageContainer = nullptr;
    };
};


/*static*/ void CMessageManager::Draw(void)
{
    MessageContainer().Draw();
};


/*static*/ void CMessageManager::Period(void)
{
    MessageContainer().Period();
};


/*static*/ bool CMessageManager::Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer)
{
    return MessageContainer().Request(idGroup, idPlayer);
};


/*static*/ bool CMessageManager::OnlyTextRequest(SEGROUPID::VALUE idGroup)
{
    return MessageContainer().OnlyTextRequest(idGroup);
};


/*static*/ bool CMessageManager::DirectCall(int32 nVoiceCode, bool bBusyCheck)
{
    return CVoiceManager::PlayVoice(nVoiceCode, bBusyCheck);
};