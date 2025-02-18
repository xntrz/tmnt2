#include "GaugeStatus.hpp"
#include "GaugeManager.hpp"
#include "GaugeAnim.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Controller.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


static inline bool rect_intersect(float r1_x, float r1_y, float r1_w, float r1_h,
                                  float r2_x, float r2_y, float r2_w, float r2_h)
{
    return (r1_x < (r2_x + r2_w)) &&
           (r2_x < (r1_x + r1_w)) &&
           (r1_y < (r2_y + r2_h)) &&
           (r2_y < (r1_y + r1_h));
};


class CGaugeStatus_Container
{
private:
    struct TIMERINFO
    {
        bool  m_bWork;
        int32 m_nNo;
        float m_fValue;
    };

    struct COUNTERINFO
    {
        bool  m_bWork;
        int32 m_nNo;
        int32 m_nValue;
    };

    struct ZAKOINFO
    {
        enum STATE
        {
            STATE_INIT = 0,
            STATE_IDLE,
            STATE_RECOVER,
            STATE_DAMAGE,
        };

        bool   m_bWork;
        int32  m_nNo;
        STATE  m_eState;
        int32  m_nHp;
        int32  m_nHpOld;
        int32  m_nHpMove;
        int32  m_nHpTotal;
        uint32 m_uAnimCnt;
    };

public:
    CGaugeStatus_Container(void);
    ~CGaugeStatus_Container(void);
    void Period(void);
    void Draw(void);
    void TimerDispEnable(bool bEnable);
    void TimerDispSet(int32 no, float fValue);
    void CounterDispEnable(bool bEnable);
    void CounterDispSet(int32 no, int32 value);
    void ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_TYPE type, bool bEnable);
    void ExGaugeDispInit(CGaugeStatus::EX_GAUGE_TYPE type, int32 value);
    void ExGaugeDispSet(int32 no, CGaugeStatus::EX_GAUGE_TYPE type, int32 value);
    void StatusWakuDisp(void);
    float StatusWakuDispSub(int32 nPlayerNo, float fX, float fY);
    void StatusWakuDispSub_Parts(int32 index, int32 no, float fX, float fY, float fW, float fH);
    void CharacterIconDisp(int32 nPlayerNo, IGamePlayer* pGameplayer, float fX, float fY);
    void PlayerMarkerDisp(int32 nPlayerNo);
    float GetMarkerPosY(int32 nPlayerNo) const;
    void TimerDispProc(void);
    void CounterDispProc(void);
    void ExGaugeDispProc(void);
    void ZakoGaugeDisp(int32 nZakoNo, float fX, float fY);
    void NumDraw(int32 nFontNo, float fOfsX, float fOfsY, uint8 uAlpha, const char* pszFormat, ...);
    void SetNumColor(int32 nFontNo, int32 nPlayerNo);

private:
    uint32      m_uGaugeCnt;
    uint32      m_uGaugeStep;
    bool        m_bWakuCheck;
    bool        m_bWakuSayonara;
    bool        m_bGaugeSetting;
    RwV2d       m_aScreenPosBuff[4];
    CSprite     m_aSprite[12];
    float       m_fWakuStartX;
    float       m_fWakuStartY;
    float       m_fCondStartX;
    float       m_fCondStartY;
    TIMERINFO   m_timer;
    COUNTERINFO m_counter;
    ZAKOINFO    m_aZako[CGaugeStatus::EX_GAUGE_ZAKO_NUM];
    CSprite     m_SpriteMarker;
    CSprite     m_SpriteMarkerArr;
};


CGaugeStatus_Container::CGaugeStatus_Container(void)
: m_uGaugeCnt(0)
, m_uGaugeStep(0)
, m_bWakuCheck(false)
, m_bWakuSayonara(false)
, m_bGaugeSetting(false)
, m_fWakuStartX(0.0f)
, m_fWakuStartY(0.0f)
, m_fCondStartX(0.0f)
, m_fCondStartY(0.0f)
, m_timer({ 0 })
, m_counter({ 0 })
{
    std::memset(m_aScreenPosBuff, 0x00, sizeof(m_aScreenPosBuff));
    std::memset(m_aZako, 0x00, sizeof(m_aZako));
};


CGaugeStatus_Container::~CGaugeStatus_Container(void)
{
    ;
};


void CGaugeStatus_Container::Period(void)
{
    if (m_bGaugeSetting)
    {
        uint32 digitalTrigger = 0;
        digitalTrigger |= CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
        digitalTrigger |= CController::GetDigitalTrigger(CController::CONTROLLER_LOCKED_ON_VIRTUAL);
		
        if (IGamepad::CheckFunction(digitalTrigger, IGamepad::FUNCTION_SWITCH_GAUGE) ||
            IGamepad::CheckFunction(digitalTrigger, IGamepad::FUNCTION_SWITCH_GAUGE))
        {
            m_bWakuCheck = !m_bWakuCheck;
        };
    }
    else
    {
        m_fCondStartX =  -93.0f;
        m_fCondStartY = -184.0f;
        
        m_fWakuStartX =  270.0f;
        m_fWakuStartY = -200.0f;

        CTextureManager::SetCurrentTextureSet("gg_status");
        m_aSprite[1].SetTexture(CTextureManager::GetRwTexture("gg_chara2_bg"));
        m_aSprite[2].SetTexture(CTextureManager::GetRwTexture("gg_chara1_bg_hikari"));
        m_aSprite[3].SetTexture(CTextureManager::GetRwTexture("gg_face"));
        m_aSprite[4].SetTexture(CTextureManager::GetRwTexture("gg_icon_shuriken"));
        m_aSprite[5].SetTexture(CTextureManager::GetRwTexture("gg_chara_p"));

        m_SpriteMarker.SetTexture(CTextureManager::GetRwTexture("gg_marker"));
        m_SpriteMarkerArr.SetTexture(CTextureManager::GetRwTexture("gg_chara_ya"));

        m_aSprite[6].SetTexture(CTextureManager::GetRwTexture("gg_cond"));
        m_aSprite[7].SetTexture(CTextureManager::GetRwTexture("gg_icon"));
        m_aSprite[8].SetTexture(CTextureManager::GetRwTexture("gg_chara_shuri_num"));
        m_aSprite[9].SetTexture(CTextureManager::GetRwTexture("gg_chara_dame_num"));
        m_aSprite[10].SetTexture(CTextureManager::GetRwTexture("gg_cond_num"));
        
        m_aSprite[11].SetTexture(CTextureManager::GetRwTexture("gg_e3_start_ps2"));
        m_aSprite[11].Move(234.0f, -149.0f);
        m_aSprite[11].Resize(64.0f, 64.0f);

        m_bGaugeSetting = true;
    };
};


void CGaugeStatus_Container::Draw(void)
{
    if (!m_bGaugeSetting)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEGOURAUD);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);

    StatusWakuDisp();
    
    if (m_timer.m_bWork)
        TimerDispProc();

    if (m_counter.m_bWork)
        CounterDispProc();

    ExGaugeDispProc();
};


void CGaugeStatus_Container::TimerDispEnable(bool bEnable)
{
    m_timer.m_bWork = bEnable;
};


void CGaugeStatus_Container::TimerDispSet(int32 no, float fValue)
{
    m_timer.m_nNo = no;
	m_timer.m_fValue = fValue;
};


void CGaugeStatus_Container::CounterDispEnable(bool bEnable)
{
    m_counter.m_bWork = bEnable;
};


void CGaugeStatus_Container::CounterDispSet(int32 no, int32 value)
{
    m_counter.m_nNo = no;
    m_counter.m_nValue = value;
};


void CGaugeStatus_Container::ExGaugeDispEnable(CGaugeStatus::EX_GAUGE_TYPE type, bool bEnable)
{
    m_aZako[type].m_bWork = bEnable;
};


void CGaugeStatus_Container::ExGaugeDispInit(CGaugeStatus::EX_GAUGE_TYPE type, int32 value)
{
    m_aZako[type].m_nHpTotal = value;
};


void CGaugeStatus_Container::ExGaugeDispSet(int32 no, CGaugeStatus::EX_GAUGE_TYPE type, int32 value)
{
    m_aZako[type].m_nNo = no;
    m_aZako[type].m_nHp = value;
};


void CGaugeStatus_Container::StatusWakuDisp(void)
{
    float x = m_fWakuStartX;
    float y = m_fWakuStartY;
    
    int32 nPlayerNum = CGameProperty::GetPlayerNum();
	if (!nPlayerNum)
		return;

    for (int32 i = 0; i < nPlayerNum; ++i)
    {
        if ((CGameData::PlayParam().GetStageMode() != GAMETYPES::STAGEMODE_RIDE) && !m_bWakuCheck)
            x = StatusWakuDispSub(nPlayerNum - i - 1, x, y) - 3.0f;

        PlayerMarkerDisp(nPlayerNum - i - 1);
    };

    m_bWakuSayonara = false;

    float fScrW = static_cast<float>(CScreen::Width());
    float fScrH = static_cast<float>(CScreen::Height());

    for (int32 i = 0; i < CGameProperty::GetPlayerNum(); ++i)
    {
        IGamePlayer* pGamePlayer = CGameProperty::Player(i);
        if (!pGamePlayer->IsAlive())
            continue;

        RwV2d plScrPos = m_aScreenPosBuff[i];
        RwV2d plScrSize = { 16.0f, 18.0f };

        RwV2d rcScrPos = { ((fScrW * 0.5f) + x), 0.0f };
        RwV2d rcScrSize = { (fScrW - rcScrPos.x),
                            ((fScrH * 0.5f) + y + 80.0f) };

        if (rect_intersect(plScrPos.x, plScrPos.y, plScrSize.x, plScrSize.y,
                           rcScrPos.x, rcScrPos.y, rcScrSize.x, rcScrSize.y))
        {
            m_bWakuSayonara = true;
        };
    };
};


float CGaugeStatus_Container::StatusWakuDispSub(int32 nPlayerNo, float fX, float fY)
{
	IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(nPlayerNo);

    float afStoreX[4] =
    {
        30.0f,
        42.0f,
        59.0f,
        72.0f,
    };

	m_aSprite[1].SetRGBA(PLAYERID::GetColor(pGamePlayer->GetCurrentCharacterID()));

    if (!m_bWakuSayonara)
        StatusWakuDispSub_Parts(0, 1, fX, fY, 20.0f, 80.0f);
    
    float fBuffX = fX - 2.0f;
    float store_x = afStoreX[pGamePlayer->GetCharacterNum() - 1];

    fX -= store_x;

    if (!m_bWakuSayonara)
        StatusWakuDispSub_Parts(1, 1, fX + store_x - 20.0f, fY, store_x, 80.0f);

    fX -= 20.0f;

    if (!m_bWakuSayonara)
        StatusWakuDispSub_Parts(2, 1, fX, fY, 20.0f, 80.0f);

    fX -= 20.0f;

    if (!m_bWakuSayonara)
    {
        float x = fBuffX - 29.0f;
        float y = fY + 43.0f;

        m_aSprite[4].Move(x, y);
        m_aSprite[4].Resize(32.0f, 16.0f);
        m_aSprite[4].Draw();
        
        x = fBuffX - 16.0f;
        y = fY + 11.0f;

        m_aSprite[5].Move(x, y);
        m_aSprite[5].Resize(64.0f, 32.0f);
        m_aSprite[5].SetRGBA(PLAYERID::GetColor(pGamePlayer->GetCurrentCharacterID()));
        m_aSprite[5].SetUV(0.0f,
                           (nPlayerNo * 32.0f) * (1.0f / 128.0f),
                           1.0f,
                           ((nPlayerNo * 32.0f) + 32.0f) * (1.0f / 128.0f));
        m_aSprite[5].Draw();

        CharacterIconDisp(nPlayerNo, pGamePlayer, fBuffX, fY);

        SetNumColor(8, nPlayerNo);
        NumDraw(8, fBuffX + 5.0f, fY + 43.0f, 255, "%02d", pGamePlayer->GetShurikenNum());

        SetNumColor(9, nPlayerNo);
        NumDraw(9, fBuffX + 6.0f, fY + 29.0f, 255, "-%04d", pGamePlayer->GetDamage());
    };

	pGamePlayer->Release();

	return fX;
};


void CGaugeStatus_Container::StatusWakuDispSub_Parts(int32 index, int32 no, float fX, float fY, float fW, float fH)
{
    static const float s_afStatusWakuUvTbl[][4] =
    {
        { 0.84375f, 0.0f, 1.0f,     0.625f   },
        { 0.625f,   0.0f, 0.84375f, 0.625f   },
        { 0.46875f, 0.0f, 0.625f,   0.625f   },
    };

    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(s_afStatusWakuUvTbl));

    m_aSprite[no].SetUV(s_afStatusWakuUvTbl[index][0],
                        s_afStatusWakuUvTbl[index][1],
                        s_afStatusWakuUvTbl[index][2],
                        s_afStatusWakuUvTbl[index][3]);
    m_aSprite[no].Resize(fW, fH);
    m_aSprite[no].Move(fX, fY);
    m_aSprite[no].SetOffset(1.0f, 0.0f);
    m_aSprite[no].Draw();
};


void CGaugeStatus_Container::CharacterIconDisp(int32 nPlayerNo, IGamePlayer* pGameplayer, float fX, float fY)
{
    int32 nCharacterNum = pGameplayer->GetCharacterNum();
    int32 nStart = 0;

    if (nCharacterNum)
    {
        for (int32 i = 0; i < nCharacterNum; ++i)
        {
            nStart = nCharacterNum - i - 1;

            if (pGameplayer->GetCharacterID(nStart) == pGameplayer->GetCurrentCharacterID())
                break;
        };
    }
    else
    {
        nStart = nPlayerNo;
    };

    if (!nStart)
        nStart = nCharacterNum;

    nStart -= 1;

    for (int32 i = 0; i < nCharacterNum; ++i)
    {
        switch (pGameplayer->GetCharacterID(nStart))
        {
        case PLAYERID::ID_LEO:
        case PLAYERID::ID_SLA:
            m_aSprite[7].SetUV(0.0f, 0.0f, 1.0f, 0.25f);
            break;

        case PLAYERID::ID_RAP:
        case PLAYERID::ID_CAS:
            m_aSprite[7].SetUV(0.0f, 0.25f, 1.0f, 0.5f);
            break;

        case PLAYERID::ID_MIC:
        case PLAYERID::ID_KAR:
            m_aSprite[7].SetUV(0.0f, 0.5f, 1.0f, 0.75f);
            break;

        case PLAYERID::ID_DON:
        case PLAYERID::ID_SPL:
            m_aSprite[7].SetUV(0.0f, 0.75f, 1.0f, 1.0f);
            break;

        default:
            ASSERT(false);
            break;
        };

        if (nCharacterNum - i - 1)
        {
            m_aSprite[7].Resize(16.0f, 16.0f);
            m_aSprite[7].Move(fX - 40.0f - (nCharacterNum * 8.0f),
                              fY + 12.0f);
        }
        else
        {
            m_aSprite[7].Resize(19.2f, 19.2f);
            m_aSprite[7].Move(fX - 40.0f - (nCharacterNum * 8.0f),
                              fY + 11.0f);
        };
        
        m_aSprite[7].Draw();

        if (!nStart)
            nStart = nCharacterNum;

		fX += 8.0f;
        --nStart;
    };
};


void CGaugeStatus_Container::PlayerMarkerDisp(int32 nPlayerNo)
{
    if (!CGameData::Option().Display().IsEnabledPlayerMarker())
        return;

    float fMarkerPosY = 1.0f;

    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
        fMarkerPosY = GetMarkerPosY(nPlayerNo);

        if ((CGameProperty::Player(nPlayerNo)->GetStatus() == PLAYERTYPES::RIDESTATUS_CRASH_WALL) ||
            (CGameProperty::Player(nPlayerNo)->GetStatus() == PLAYERTYPES::RIDESTATUS_SHIP_CRASH))
            return;
    };
    
    RwV3d vPlayerPos = Math::VECTOR3_ZERO;
    CGameProperty::Player(nPlayerNo)->GetPosition(&vPlayerPos);
    vPlayerPos.y += fMarkerPosY;

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGameProperty::GetCameraViewMatrix(&matrix);

    RwV3d vScreenPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vScreenPos, &vPlayerPos, &matrix);

    if (vScreenPos.z > 0.0f)
    {
        vScreenPos.x *= (1.0f / vScreenPos.z);
        vScreenPos.y *= (1.0f / vScreenPos.z);
    };

    float screenW = static_cast<float>(CScreen::Width());
    float screenH = static_cast<float>(CScreen::Height());

    vScreenPos.x *= screenW;
    vScreenPos.y *= screenH;
    vScreenPos.z  = RwIm2DGetNearScreenZMacro();

    enum
    {
        OUTFLAG_RIGHT   = (1 << 0),
        OUTFLAG_LEFT    = (1 << 1),
        OUTFLAG_BOTTOM  = (1 << 2),
        OUTFLAG_TOP     = (1 << 3),
    };

    uint32 outflag = 0;

    if (vScreenPos.x >= screenW)
    {
        vScreenPos.x = (screenW - 16.0f);
        outflag |= OUTFLAG_RIGHT;
    }
    else if (vScreenPos.x <= 0.0f)
    {
        vScreenPos.x = 16.0f;
        outflag |= OUTFLAG_LEFT;
    };

    if (vScreenPos.y >= screenH)
    {
		vScreenPos.y = (screenH - 18.0f);
        outflag |= OUTFLAG_BOTTOM;
    }
    else if (vScreenPos.y <= 0.0f)
    {
		vScreenPos.y = 18.0f;
        outflag |= OUTFLAG_TOP;
    };

    m_aScreenPosBuff[nPlayerNo] = { vScreenPos.x, vScreenPos.y };

    if (outflag)
    {
        for (int32 i = 0; i < nPlayerNo; ++i)
        {
            RwV2d& scrPos = m_aScreenPosBuff[i];

            if (rect_intersect(vScreenPos.x, vScreenPos.y, 32.0f, 18.0f,
                               scrPos.x, scrPos.y, 32.0f, 18.0f))
            {
                if (outflag & OUTFLAG_RIGHT)
                {
                    vScreenPos.x -= 32.0f;
                }
                else if (outflag & OUTFLAG_LEFT)
                {
                    vScreenPos.x += 32.0f;
                }
                else if (outflag & OUTFLAG_BOTTOM)
                {
                    vScreenPos.y -= 18.0f;
                }
                else if (outflag & OUTFLAG_TOP)
                {
                    vScreenPos.y += 18.0f;
                };
            };
        };
    };

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);

    m_SpriteMarker.SetRGBA(PLAYERID::GetColor(CGameProperty::Player(nPlayerNo)->GetCurrentCharacterID()));
    m_SpriteMarker.SetOffset(0.5f, 0.0f);

    m_SpriteMarker.SetPositionAndSizeRealScreen(vScreenPos.x, vScreenPos.y - 18.0f, 32.0f, 18.0f);
    m_SpriteMarker.SetUV(0.0f,
                         (nPlayerNo * 22.0f) * (1.0f / 128.0f),
                         1.0f,
                         ((nPlayerNo * 22.0f) + 18.0f) * (1.0f / 128.0f));
    m_SpriteMarker.Draw();

    if (outflag)
    {
        float fRotate = Math::ATan2(((screenH * 0.5f) + 16.0f) - ((vScreenPos.y - 18.0f) + 16.0f),
                                    ((screenW * 0.5f) + 16.0f) - (vScreenPos.x - 16.0f));

        fRotate += Math::PI;

        PLAYERID::VALUE playerId = CGameProperty::Player(nPlayerNo)->GetCurrentCharacterID();
        RwRGBA playerColor = PLAYERID::GetColor(playerId);

        m_SpriteMarkerArr.SetRGBA(playerColor);
        m_SpriteMarkerArr.SetRotate(fRotate);
        m_SpriteMarkerArr.SetPositionAndSizeRealScreen(vScreenPos.x - 8.0f,
                                                       vScreenPos.y - 18.0f + 24.0f,
                                                       16.0f,
                                                       16.0f);
        m_SpriteMarkerArr.SetUV(0.0f, 0.0f, 1.0f, 1.0f);
        m_SpriteMarkerArr.DrawRotate();
    };    
};


float CGaugeStatus_Container::GetMarkerPosY(int32 nPlayerNo) const
{
    if ((CGameData::PlayParam().GetArea() == AREAID::ID_AREA22) ||
        (CGameData::PlayParam().GetArea() == AREAID::ID_AREA32))
        return 0.6f;

    if (CGameProperty::Player(nPlayerNo)->GetCurrentCharacterID() == PLAYERID::ID_SLA)
        return 1.2f;
    
    return 1.7f;
};


void CGaugeStatus_Container::TimerDispProc(void)
{
    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ADD);

    float x = m_fCondStartX;
    float y = (float(m_timer.m_nNo) * 42.0f) + m_fCondStartY;

    m_aSprite[6].Move(x, y);
    m_aSprite[6].Resize(128.0f, 32.0f);
    m_aSprite[6].SetUV(0.0f, 0.0f, 1.0f, 0.125f);
    m_aSprite[6].Draw();

    x = -30.0f;
    y = (float(m_timer.m_nNo) * 42.0f) - 174.0f;

    NumDraw(10,
            x,
            y,
            128,
            "%02d:%02d:%02d",
            static_cast<int32>(m_timer.m_fValue) / 60,
            static_cast<int32>(m_timer.m_fValue) % 60,
            static_cast<int32>(m_timer.m_fValue * 100.0f) % 100);

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);
};


void CGaugeStatus_Container::CounterDispProc(void)
{
    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ADD);

    float x = m_fCondStartX;
    float y = (float(m_counter.m_nNo) * 42.0f) + m_fCondStartY;

    m_aSprite[6].Move(x, y);
    m_aSprite[6].Resize(128.0f, 32.0f);
    m_aSprite[6].SetUV(0.0f, 0.125f, 1.0f, 0.25f);
    m_aSprite[6].Draw();

    x = -30.0f;
    y = (float(m_counter.m_nNo) * 42.0f) - 174.0f;

    NumDraw(10, x, y, 128, "%02d", m_counter.m_nValue);

    CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);
};


void CGaugeStatus_Container::ExGaugeDispProc(void)
{   
    for (int32 i = 0; i < COUNT_OF(m_aZako); ++i)
    {
        if (!m_aZako[i].m_bWork)
            continue;

        CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ADD);

        m_aSprite[6].Resize(128.0f, 32.0f);
        m_aSprite[6].SetUV(0.0f,
                           (float(i + 2) * 32.0f) * (1.0f / 256.0f),
                           1.0f,
                           ((float(i + 2) * 32.0f) + 32.0f) * (1.0f / 256.0f));

        if (i == CGaugeStatus::EX_GAUGE_ZAKO_FUGITOID_LIVE)
        {
            float OfsY = (float(m_aZako[i].m_nNo) * 42.0f);
            
            m_aSprite[6].Move(m_fCondStartX, m_fCondStartY + OfsY);
            m_aSprite[6].Draw();

            ZakoGaugeDisp(i, -30.0f, (OfsY - 174.0f));
        }
        else
        {
            float OfsX = (float(i - 1) * 200.0f);
            m_aSprite[6].Move(OfsX - 194.0f, 181.0f);
            m_aSprite[6].Draw();

            ZakoGaugeDisp(i, OfsX - 130.0f, 191.0f);
        };

        CGaugeManager::SetGaugeAlphaMode(CGaugeManager::ALPHAMODE_ALPHA);
    };
};


void CGaugeStatus_Container::ZakoGaugeDisp(int32 nZakoNo, float fX, float fY)
{
    ZAKOINFO* pZakoInfo = &m_aZako[nZakoNo];

    if (pZakoInfo->m_eState)
    {
        if ((pZakoInfo->m_eState == ZAKOINFO::STATE_IDLE) ||
            (pZakoInfo->m_nHp != pZakoInfo->m_nHpOld))
        {
            int32 HpNow = pZakoInfo->m_nHp;
            int32 HpOld = pZakoInfo->m_nHpOld;

            pZakoInfo->m_nHpMove = pZakoInfo->m_nHpOld;
            pZakoInfo->m_nHpOld = pZakoInfo->m_nHp;

            if (HpOld > HpNow)
            {
                pZakoInfo->m_eState = ZAKOINFO::STATE_DAMAGE;
                pZakoInfo->m_uAnimCnt = 0;
            }
            else
            {
                pZakoInfo->m_eState = ZAKOINFO::STATE_RECOVER;
                pZakoInfo->m_uAnimCnt = 0;
            };
        };
    }
    else
    {
        pZakoInfo->m_nHpMove = pZakoInfo->m_nHpTotal;
        pZakoInfo->m_nHpOld = pZakoInfo->m_nHpTotal;
        pZakoInfo->m_eState = ZAKOINFO::STATE_IDLE;
    };

    float fAnimDuration = GAUGE_ANIM_DURATION_FRAMES(4);
    float fAnimStep = (float(pZakoInfo->m_uAnimCnt) / fAnimDuration);
    float hpMove = float(pZakoInfo->m_nHpMove);

    if (pZakoInfo->m_eState == ZAKOINFO::STATE_RECOVER)
    {
        hpMove = (hpMove - ((hpMove - float(pZakoInfo->m_nHpOld)) * fAnimStep));
    }
    else if (pZakoInfo->m_eState == ZAKOINFO::STATE_DAMAGE)
    {
        hpMove = (hpMove + ((float(pZakoInfo->m_nHpOld) - hpMove) * fAnimStep));
    };

    if (pZakoInfo->m_eState > ZAKOINFO::STATE_IDLE)
    {
        if (float(pZakoInfo->m_uAnimCnt) >= fAnimDuration)
            pZakoInfo->m_eState = ZAKOINFO::STATE_IDLE;
        else
            ++pZakoInfo->m_uAnimCnt;
    };

    const RwRGBA aZakoColor[] =
    {
        { 0x70, 0xFF, 0xFF, 0x80 },
        { 0xB3, 0x28, 0xFF, 0x80 },
        { 0xFF, 0x28, 0x41, 0x80 },
        { 0x28, 0xB0, 0xFF, 0x80 },
    };

    static_assert(COUNT_OF(aZakoColor) == COUNT_OF(m_aZako), "update me");

    float fHeight = 13.0f;
    if (nZakoNo)
        fHeight = 7.0f;

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    
    m_aSprite[0].SetOffset(1.0f, 0.0f);
    m_aSprite[0].SetUV((140.0f - ((140.0f / float(pZakoInfo->m_nHpTotal)) * hpMove)) * (1.0f / 140.0f),
                        0.0f,
                        1.0f,
                        1.0f);
    m_aSprite[0].Resize((140.0f / float(pZakoInfo->m_nHpTotal)) * hpMove,
                         fHeight);
    m_aSprite[0].Move(fX, fY);
    m_aSprite[0].SetRGBA(aZakoColor[nZakoNo]);
    m_aSprite[0].Draw();
};


void CGaugeStatus_Container::NumDraw(int32 nFontNo, float fOfsX, float fOfsY, uint8 uAlpha, const char* pszFormat, ...)
{
    char szBuffer[256];
    szBuffer[0] = '\0';

    va_list vl;
    va_start(vl, pszFormat);
    std::vsprintf(szBuffer, pszFormat, vl);
    va_end(vl);

    int32 numDelimiters = 0;
    int32 len = static_cast<int32>(std::strlen(szBuffer));

    float rasterW = 0.0f;
    float digitW = 0.0f;
    float digitH = 0.0f;
    float startPosX = 0.0f;
    float startPosY = 0.0f;
    float spriteW = 0.0f;
    float spriteH = 0.0f;
    float stepX = 0.0f;
    float stepY = 0.0f;

    switch (nFontNo)
    {
    case 8:
        {
            m_aSprite[8].SetOffset(1.0f, 0.0f);
            digitW = 12.0f;
            digitH = 14.0f;
            spriteW = 12.0f;
            spriteH = 16.0f;
            rasterW = 256.0f;
            stepX = 10.0f;
            startPosY = fOfsY - 8.0f;
            startPosX = fOfsX - 5.0f - (len - 1) * 10.0f;
        }
        break;

    case 9:
        {
            m_aSprite[9].SetOffset(1.0f, 0.0f);
            digitW = 16.0f;
            digitH = 20.5f;
            spriteW = 16.0f;
            spriteH = 16.0f;
            rasterW = 256.0f;
            stepX = 13.0f;
            startPosY = fOfsY - 8.0f;
            startPosX = fOfsX - 6.0f - (len - 1) * 13.0f;
        }
        break;

    case 10:
        {
            m_aSprite[10].SetOffset(1.0f, 0.0f);
            digitW = 20.0f;
            digitH = 24.0f;
            spriteW = 20.0f;
            spriteH = 32.0f;
            rasterW = 512.0f;
            stepX = 20.0f;
            startPosY = fOfsY - 6.0f;

            for (int32 i = 0; i < len; ++i)
            {
                if (szBuffer[i] == ':')
                    ++numDelimiters;
            };

            startPosX = (fOfsX - (float(len - 1) * 20.0f)) + (numDelimiters * 8.0f);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    float posX = 0.0f;
    float posY = 0.0f;
    float u_start = 0.0f;
    float u_end = 0.0f;
    float u_factor = (1.0f / rasterW);
    int32 i = 0;
    
    while (szBuffer[i])
    {
        char ch = szBuffer[i];

        if ((ch == '\n') ||
            (ch == '\r'))
        {
            posX = 0.0f;
            ++i;
            posY += spriteH;
        }
        else if (nFontNo == 10)
        {
            if (ch == ':')
            {
                stepX = 16.0f;
                posX -= 4.0f;
            }
            else
            {
                stepX = 20.0f;
            };
        }
        else if ((ch == '-') && (nFontNo == 9))
        {
            szBuffer[i] = ':';
        };

        float t = ((szBuffer[i] - '0') * digitH);
        u_end = (digitW + t) * u_factor;
        u_start = u_factor * t;

        m_aSprite[nFontNo].SetAlpha(uAlpha);
        m_aSprite[nFontNo].Resize(spriteW, spriteH);
        m_aSprite[nFontNo].SetUV(u_start, 0.0f, u_end, 1.0f);
        m_aSprite[nFontNo].Move(posX + startPosX, posY + startPosY);
        m_aSprite[nFontNo].Draw();

        posX += stepX;
        ++i;
    };
};


void CGaugeStatus_Container::SetNumColor(int32 nFontNo, int32 nPlayerNo)
{
    int32 nColorNo = 0;

    switch (nFontNo)
    {
    case 8:
        {
            if (CGameProperty::Player(nPlayerNo)->GetShurikenNum() > 0)
                nColorNo = 1;
            else
                nColorNo = 2;
        }
        break;

    case 9:
        {
            int32 nWorstDamage = 0;
            
            if (CGameProperty::GetPlayerNum() > 1)
            {
                int32 nWorstPlayer = 0;

                for (int32 i = 0; i < CGameProperty::GetPlayerNum(); ++i)
                {
                    IGamePlayer* pGamePlayer = CGameProperty::Player(i);
                    if (!pGamePlayer->IsAlive())
                        continue;

                    int32 nDamage = pGamePlayer->GetDamage();
                    if (nDamage)
                    {
                        if (nDamage == nWorstDamage)
                            break;

                        if (nWorstDamage <= nDamage)
                        {
                            nWorstPlayer = i;
                            nWorstDamage = nDamage;
                        };
                    };
                };

                if (nWorstDamage && (nPlayerNo == nWorstPlayer))
                    nColorNo = 3;
            };
        }
        break;

    default:
        break;
    };

    static const RwRGBA s_aColor[] =
    {
        { 0xFF, 0xFF, 0xFF, 0xFF },
        { 0xC2, 0xE0, 0xED, 0xFF },
        { 0xFF, 0x5A, 0x00, 0xFF },
        { 0xFF, 0x5A, 0x00, 0xFF },
    };

    m_aSprite[nFontNo].SetRGBA(s_aColor[nColorNo]);
};


static CGaugeStatus_Container* s_pGaugeStatusContainer = nullptr;


static CGaugeStatus_Container& GaugeStatusContainer(void)
{
    ASSERT(s_pGaugeStatusContainer);
    return *s_pGaugeStatusContainer;
};


/*static*/ void CGaugeStatus::Initialize(void)
{
    if (!s_pGaugeStatusContainer)
        s_pGaugeStatusContainer = new CGaugeStatus_Container;
};


/*static*/ void CGaugeStatus::Termiante(void)
{
    if (s_pGaugeStatusContainer)
    {
        delete s_pGaugeStatusContainer;
        s_pGaugeStatusContainer = nullptr;
    };
};


/*static*/ void CGaugeStatus::Period(void)
{
    GaugeStatusContainer().Period();
};


/*static*/ void CGaugeStatus::Draw(void)
{
    GaugeStatusContainer().Draw();
};


/*static*/ void CGaugeStatus::TimerDispEnable(bool bEnable)
{
    GaugeStatusContainer().TimerDispEnable(bEnable);
};


/*static*/ void CGaugeStatus::TimerDispSet(int32 no, float fValue)
{
    GaugeStatusContainer().TimerDispSet(no, fValue);
};


/*static*/ void CGaugeStatus::CounterDispEnable(bool bEnable)
{
    GaugeStatusContainer().CounterDispEnable(bEnable);
};


/*static*/ void CGaugeStatus::CounterDispSet(int32 no, int32 value)
{
    GaugeStatusContainer().CounterDispSet(no, value);
};


/*static*/ void CGaugeStatus::ExGaugeDispEnable(EX_GAUGE_TYPE type, bool bEnable)
{
    GaugeStatusContainer().ExGaugeDispEnable(type, bEnable);
};


/*static*/ void CGaugeStatus::ExGaugeDispInit(EX_GAUGE_TYPE type, int32 value)
{
    GaugeStatusContainer().ExGaugeDispInit(type, value);
};


/*static*/ void CGaugeStatus::ExGaugeDispSet(int32 no, EX_GAUGE_TYPE type, int32 value)
{
    GaugeStatusContainer().ExGaugeDispSet(no, type, value);
};