#include "MessageManager.hpp"
#include "VoiceManager.hpp"
#include "GameSound.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Sprite.hpp"


class CMessageContainer
{
private:
    static const float APPEAR_TIME;
    static const float SHOW_TIME;
    static const float DISAPPEAR_TIME;
    static const RwRGBA s_initialTextRGBA;
    static const RwRGBA s_lastTextRGBA;
    static const RwRGBA s_initialVertexRGBA;
    static const RwRGBA s_aLastVerexRGBA[6 * 2];

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
    void vertexColorLinerMove(bool FadeInFlag);
    void stringColorLinerMove(bool FadeInFlag);
    void vertexSet(void);
    void rsPush(void);
    void rsPop(void);
    int32 getStageKind(void);
    float getStringBoxHeight(void);
    bool isHelpEnable(void);
    bool isValidID(SEGROUPID::VALUE idGroup);
    void calcAnchorAndPos(void);
    
private:
    SEGROUPID::VALUE m_TextSeGroupID;
    float m_fTimer;
    RwIm2DVertex m_aVertices[6];
    RwRGBA m_aVerticesColor[6];
    RwRGBA m_MsgColor;
    Rt2dBBox m_MsgBBox;
    STATE m_State;
    bool m_SeGroupIDPlayFlag[SEGROUPID::ID_MAX];
    int32 m_StageKind;
    int32 m_NumStringLines;
    RwV2d m_vBasePosition;
    RwV2d m_vStringBoxWH;
    RwV2d m_vStringBoxOffset;
    float m_fFontW;
};


/*static*/ const float CMessageContainer::APPEAR_TIME = 0.5f;
/*static*/ const float CMessageContainer::SHOW_TIME = 4.0f;
/*static*/ const float CMessageContainer::DISAPPEAR_TIME = 0.5f;
/*static*/ const RwRGBA CMessageContainer::s_initialTextRGBA = { 0xFF, 0xFF, 0xFF, 0x00 };
/*static*/ const RwRGBA CMessageContainer::s_lastTextRGBA = { 0xFF, 0xFF, 0xFF, 0xFF };
/*static*/ const RwRGBA CMessageContainer::s_initialVertexRGBA = { 0x00 };
/*static*/ const RwRGBA CMessageContainer::s_aLastVerexRGBA[6 * 2] =
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


CMessageContainer::CMessageContainer(void)
: m_TextSeGroupID(SEGROUPID::ID_INVALID)
, m_fTimer(0.0f)
, m_aVertices()
, m_aVerticesColor()
, m_MsgColor()
, m_MsgBBox()
, m_State(STATE_IDLE)
, m_SeGroupIDPlayFlag()
, m_StageKind(getStageKind())
, m_NumStringLines(0)
, m_vBasePosition(Math::VECTOR2_ZERO)
, m_vStringBoxWH(Math::VECTOR2_ZERO)
, m_vStringBoxOffset(Math::VECTOR2_ZERO)
, m_fFontW(0.0f)
{
    calcAnchorAndPos();
};


CMessageContainer::~CMessageContainer(void)
{
    ;
};


void CMessageContainer::Period(void)
{
    switch (m_State)
    {
    case STATE_IDLE:
        {
            ;
        }
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
                m_State = STATE_SHOW;
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
                m_State = STATE_DISAPPEAR;
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
                m_State = STATE_IDLE;
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
    return (m_State != STATE_IDLE);
};


bool CMessageContainer::Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer)
{
    return Request(idGroup, idPlayer, SEGROUPID::IsHelpID(idGroup));
};


bool CMessageContainer::Request(SEGROUPID::VALUE idGroup, PLAYERID::VALUE idPlayer, bool bIsHelpID)
{
    if (isValidID(idGroup))
    {
        if (CVoiceManager::SetVoice(idGroup, bIsHelpID))
        {
            OnlyTextRequest(idGroup);
            m_SeGroupIDPlayFlag[idGroup] = true;
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

    m_TextSeGroupID = idGroup;
    m_fTimer = 0.0f;
    m_State = STATE_APPEAR;
    calcAnchorAndPos();
    messageSet();
    vertexSet();
    std::memset(m_aVerticesColor, 0x00, sizeof(m_aVerticesColor));
    m_MsgColor = { 0xFF, 0xFF, 0xFF, 0x00 };
    
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
	float fHeight = CGameFont::GetScreenSize() / (((CSprite::VIRTUALSCREEN_DEFAULT_H / float(CScreen::Height())) * m_fFontW) * 1.25f);
    CGameFont::m_pFont->SetRGBA(m_MsgColor);
    CGameFont::m_pFont->Flow(
        CGameText::GetText(SEGROUPID::GetGameText(m_TextSeGroupID)),
        fHeight,
        &m_MsgBBox
    );
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
	*result = uint8(Math::Lerp(float(start), float(end), m_fTimer * 2.0f));
};


void CMessageContainer::vertexColorLinerMove(bool FadeInFlag)
{
    for (int32 i = 0; i < COUNT_OF(m_aVerticesColor); ++i)
    {
        RwRGBA ColorStart;
        RwRGBA ColorEnd;

        ColorStart = (FadeInFlag ? s_initialVertexRGBA : s_aLastVerexRGBA[i + 6 * m_StageKind]);
        ColorEnd = (FadeInFlag ? s_aLastVerexRGBA[i + 6 * m_StageKind] : s_initialVertexRGBA);        

        colorLinerMove(&m_aVerticesColor[i].red,    ColorStart.red,     ColorEnd.red);
        colorLinerMove(&m_aVerticesColor[i].green,  ColorStart.green,   ColorEnd.green);
        colorLinerMove(&m_aVerticesColor[i].blue,   ColorStart.blue,    ColorEnd.blue);
        colorLinerMove(&m_aVerticesColor[i].alpha,  ColorStart.alpha,   ColorEnd.alpha); 
    };
};


void CMessageContainer::stringColorLinerMove(bool FadeInFlag)
{
    RwRGBA ColorStart;
    RwRGBA ColorEnd;

    ColorStart = (FadeInFlag ? s_initialTextRGBA : s_lastTextRGBA);
    ColorEnd = (FadeInFlag ? s_lastTextRGBA : s_initialTextRGBA);

    colorLinerMove(&m_MsgColor.red,     ColorStart.red,     ColorEnd.red);
    colorLinerMove(&m_MsgColor.green,   ColorStart.green,   ColorEnd.green);
    colorLinerMove(&m_MsgColor.blue,    ColorStart.blue,    ColorEnd.blue);
    colorLinerMove(&m_MsgColor.alpha,   ColorStart.alpha,   ColorEnd.alpha);
};


void CMessageContainer::messageSet(void)
{
    float sw = float(CScreen::Width());
    float sh = float(CScreen::Height());

    m_MsgBBox.x = (((m_vBasePosition.x + m_vStringBoxOffset.x) * (sw / 640.0f)) * (640.0f / sw)) - 320.0f;
    m_MsgBBox.y = ((((m_vBasePosition.y + m_vStringBoxOffset.y) * (sh / m_fFontW)) * (480.0f / sh)) - 240.0f) * -1.0f;
    m_MsgBBox.w = m_vStringBoxWH.x;
    m_MsgBBox.h = getStringBoxHeight();
};


void CMessageContainer::vertexSet(void)
{
	float sw = float(CScreen::Width());
	float sh = float(CScreen::Height());
    float fOfsY = 0.0f;
    float fHeight = CGameFont::GetScreenSize() / m_fFontW;
    
    if (m_NumStringLines < 4)
    {
        fOfsY = m_vBasePosition.y;
    }
    else
    {
        fOfsY = m_vBasePosition.y - (float(m_NumStringLines - 3) * fHeight);
        if (fOfsY < 0.0f)
            fOfsY = 0.0f;
    };

    float x0 = m_vBasePosition.x;
    float x1 = m_vBasePosition.x + sw;
    float y0 = fOfsY + (sh - fOfsY);
    float y1 = fOfsY + (0.1f * (sh - fOfsY));
    float z = RwIm2DGetNearScreenZ();
    
    RwV2d aPos[] =
    {
        { x0, y0    },
        { x1, y0    },
        { x0, y1    },
        { x1, y1    },
        { x0, fOfsY },
        { x1, fOfsY },
    };

    static_assert(COUNT_OF(aPos) == COUNT_OF(m_aVertices), "update me");
    
    for (int32 i = 0; i < COUNT_OF(aPos); ++i)
    {
		m_aVertices[i].x = aPos[i].x * (sw / 640.0f);
		m_aVertices[i].y = aPos[i].y * (sh / 480.0f);
        m_aVertices[i].z = z;
    };
};


void CMessageContainer::rsPush(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
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
    float sh = float(CScreen::Height());

    float fHeight = CGameFont::GetScreenSize() / m_fFontW;
    GAMETEXT::VALUE idText = SEGROUPID::GetGameText(m_TextSeGroupID);
    const wchar* pwszText = CGameText::GetText(idText);
    
    m_NumStringLines = CGameFont::m_pFont->CountFlowLine(pwszText, fHeight, m_fFontW);
    if (m_NumStringLines < 3)
        return 70.0f;

    float fResult = (m_NumStringLines - 3) * fHeight + 70.0f;
    ASSERT(fResult < 400.0f);
    return fResult;
};


bool CMessageContainer::isHelpEnable(void)
{
    return CGameData::Option().Display().IsHelpEnabled();
};


bool CMessageContainer::isValidID(SEGROUPID::VALUE idGroup)
{
    if (!isHelpEnable() || SEGROUPID::IsCallFree(idGroup))
        return false;

    if (!SEGROUPID::IsCallFree(idGroup))
        return false;

    if (m_SeGroupIDPlayFlag[idGroup])
        return false;

    return true;
};


void CMessageContainer::calcAnchorAndPos(void)
{
	m_vBasePosition = { 0.0f, 336.0f };
	m_vStringBoxWH = { 488.0f, 70.0f };
	m_vStringBoxOffset = { 75.0f, 72.0f };
	m_fFontW = 448.0f;
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