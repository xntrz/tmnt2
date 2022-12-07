#include "DatabaseViewer.hpp"
#include "DatabaseSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


struct VIEWERINFO
{
    FILEID::VALUE idFile;
    int32 TexNum;
    const char* TxdName;
    const char* TexPrefix;
};


static const VIEWERINFO s_aViewerInfo[] =
{
    { FILEID::ID_INVALID,       -1, nullptr,    nullptr },

    //
    //  Characters
    //
    { FILEID::ID_DB_LEO_A,      10, "db_leo_A",     "db_ch_la_" },
    { FILEID::ID_DB_LEO_B,      10, "db_leo_B",     "db_ch_lb_" },
    { FILEID::ID_DB_SLA,        10, "db_sla",       "db_ch_sl_" },
    { FILEID::ID_DB_RAP_A,      10, "db_rap_A",     "db_ch_ra_" },
    { FILEID::ID_DB_RAP_B,      10, "db_rap_B",     "db_ch_rb_" },
    { FILEID::ID_DB_CAS,        10, "db_cas",       "db_ch_ca_" },
    { FILEID::ID_DB_MIC_A,      10, "db_mic_A",     "db_ch_ma_" },
    { FILEID::ID_DB_MIC_B,      10, "db_mic_B",     "db_ch_mb_" },
    { FILEID::ID_DB_KAR,        10, "db_kar",       "db_ch_ka_" },
    { FILEID::ID_DB_DON_A,      10, "db_don_A",     "db_ch_da_" },
    { FILEID::ID_DB_DON_B,      10, "db_don_B",     "db_ch_db_" },
    { FILEID::ID_DB_SPL,        10, "db_spl",       "db_ch_sp_" },
    { FILEID::ID_DB_APL,        10, "db_apl",       "db_ch_ap_" },
    { FILEID::ID_DB_MIYAMOTO,   2,  "db_miyamoto",  "db_en_mi_" },

    //
    //  Enemies
    //
    { FILEID::ID_DB_SHREDDER,   10, "db_shredder",  "db_en_sh_" },
    { FILEID::ID_DB_DORAKO,     2,  "db_dorako",    "db_en_do_" },
    { FILEID::ID_DB_RATS,       2,  "db_rats",      "db_en_ra_" },
    { FILEID::ID_DB_TRAXIMUS,   2,  "db_traximus",  "db_en_tr_" },
    { FILEID::ID_DB_LEATHER,    2,  "db_leather",   "db_en_le_" },
    { FILEID::ID_DB_FOOT,       2,  "db_foot",      "db_en_el_" },
    { FILEID::ID_DB_SPASMO,     2,  "db_spasmo",    "db_en_sp_" },
    { FILEID::ID_DB_HUN,        2,  "db_hun",       "db_en_hu_" },
    { FILEID::ID_DB_ULTIMATE,   2,  "db_ultimate",  "db_en_ul_" },
    { FILEID::ID_DB_ZAKO_A,     10, "db_zako_A",    "db_en_za_" },
    { FILEID::ID_DB_ZAKO_B,     10, "db_zako_B",    "db_en_zb_" },
    { FILEID::ID_DB_OTHERS,     10, "db_other",     "db_en_ot_" },

    //
    //  Background arts
    //
    { FILEID::ID_DB_NY,         5,  "db_ny",        "db_bg_ny_" },
    { FILEID::ID_DB_DHOONIB,    5,  "db_dhoonib",   "db_bg_dh_" },
    { FILEID::ID_DB_JAPAN,      5,  "db_japan",     "db_bg_ja_" },
    { FILEID::ID_DB_KURAIYAMA,  5,  "db_kuraiyama", "db_bg_ku_" },
    { FILEID::ID_DB_DIMENSION,  5,  "db_dimension", "db_bg_bn_" },
    { FILEID::ID_DB_VEHICLE,    10, "db_vehicle",   "db_bg_ve_" },
    { FILEID::ID_DB_ANIME,      10, "db_anime",     "db_bg_sb_" },
    { FILEID::ID_DB_CG,         10, "db_cg",        "db_bg_cg_" },

    //
    //  Others
    //
    { FILEID::ID_DB_PLAY_A,     8,  "db_play_A",    "db_et_pa_" },
    { FILEID::ID_DB_PLAY_B,     12, "db_play_B",    "db_et_pb_" },
    { FILEID::ID_DB_PLAY_C,     10, "db_play_C",    "db_et_pc_" },
    { FILEID::ID_DB_PLAY_D,     11, "db_play_D",    "db_et_pd_" },
    { FILEID::ID_DB_GALLERY,    11, "db_gallery",   "db_et_ga_" },
};


static inline const VIEWERINFO& GetDBViewerInfo(int32 iDbViewerNo)
{
    ASSERT((iDbViewerNo >= 1) && (iDbViewerNo < COUNT_OF(s_aViewerInfo)));
    return s_aViewerInfo[iDbViewerNo];
};


class CDatabaseViewer_Container
{
private:
    enum MOVEDIR
    {
        MOVEDIR_NONE = 0,
        MOVEDIR_RIGHT,
        MOVEDIR_LEFT,
    };
    
public:
    CDatabaseViewer_Container(void);
    ~CDatabaseViewer_Container(void);
    void Initialize(void);
    void Setting(void);
    bool Run(void);
    void Draw(void);
    void ArrowDraw(void);
    int32 GetViewerCGPos(int32 Dir);
    
private:
    int32 m_iTexNum;
    int32 m_iDbViewerNo;
    bool m_bSettingFlag;
    RwTexture* m_pTexBace;
    RwTexture* m_pTexBtWinpc;
    RwTexture* m_pTexBtn03ps;
    RwTexture* m_pTexBtn04ps;
    RwTexture* m_pTexBtn05ps;
    RwTexture* m_pTexBtn06ps;
    RwTexture* m_pTexBtn07ps;
    RwTexture* m_apTexViewer[12];
    CSprite m_Sprite;
    MOVEDIR m_eMovedir;
    int32 m_iImgCursor;
    RwV2d m_vImgPos;    
    bool m_bFlagImgZoom;
    bool m_bFlagImgChange;
    float m_fImageZoom;
    bool m_bFlagCancelZoom;
    uint32 m_uAppearAnimCnt;
    uint32 m_uCancelAnimCnt;
    RwV2d m_vCancelStartPos;    
    uint32 m_uArrowAnimCnt;
    bool m_bFlagArrowAnimReverse;
};


CDatabaseViewer_Container::CDatabaseViewer_Container(void)
: m_iTexNum(0)
, m_iDbViewerNo(0)
, m_bSettingFlag(false)
, m_pTexBace(nullptr)
, m_pTexBtWinpc(nullptr)
, m_pTexBtn03ps(nullptr)
, m_pTexBtn04ps(nullptr)
, m_pTexBtn05ps(nullptr)
, m_pTexBtn06ps(nullptr)
, m_pTexBtn07ps(nullptr)
, m_apTexViewer()
, m_Sprite()
, m_eMovedir(MOVEDIR_NONE)
, m_vImgPos(Math::VECTOR2_ZERO)
, m_bFlagImgZoom(false)
, m_bFlagImgChange(false)
, m_fImageZoom(0.0f)
, m_bFlagCancelZoom(false)
, m_uAppearAnimCnt(0)
, m_uCancelAnimCnt(0)
, m_vCancelStartPos(Math::VECTOR2_ZERO)
, m_uArrowAnimCnt(0)
, m_bFlagArrowAnimReverse(false)
{
    ;
};


CDatabaseViewer_Container::~CDatabaseViewer_Container(void)
{
    ;
};


void CDatabaseViewer_Container::Initialize(void)
{
    m_iDbViewerNo = CDatabaseSequence::GetDBViewerNo();
    ASSERT( (m_iDbViewerNo >= 0) && (m_iDbViewerNo < COUNT_OF(s_aViewerInfo)) );
    m_iTexNum = s_aViewerInfo[m_iDbViewerNo].TexNum;
    CDataLoader::Regist(s_aViewerInfo[m_iDbViewerNo].idFile);

    m_bSettingFlag = false;
    m_eMovedir = MOVEDIR_NONE;
    m_iImgCursor = 0;
    m_bFlagImgChange = false;
    m_bFlagImgZoom = false;
    m_vImgPos = Math::VECTOR2_ZERO;
    m_bFlagCancelZoom = false;
    m_uAppearAnimCnt = 0;
    m_uArrowAnimCnt = 0;
    m_bFlagArrowAnimReverse = false;
    m_fImageZoom = 1.0f;

    if (!CGameData::Record().Database().IsItemRead(DBITEMID::VALUE(m_iDbViewerNo)))
        CGameData::Record().Database().SetItemRead(DBITEMID::VALUE(m_iDbViewerNo));
};


void CDatabaseViewer_Container::Setting(void)
{
    if (m_bSettingFlag)
        return;

    m_bSettingFlag = true;

    CTextureManager::SetCurrentTextureSet("viewer_base");
    m_pTexBace = CTextureManager::GetRwTexture("DB_VieBace01ps");
    m_pTexBtWinpc = CTextureManager::GetRwTexture("DB_BtWinpc");
    m_pTexBtn03ps = CTextureManager::GetRwTexture("DB_Button03ps");
    m_pTexBtn04ps = CTextureManager::GetRwTexture("DB_Button04ps");
    m_pTexBtn05ps = CTextureManager::GetRwTexture("DB_Button05ps");
    m_pTexBtn06ps = CTextureManager::GetRwTexture("DB_Button06ps");
    m_pTexBtn07ps = CTextureManager::GetRwTexture("DB_Button07ps");

	CTextureManager::SetCurrentTextureSet(GetDBViewerInfo(m_iDbViewerNo).TxdName);

    char TexPrefix[256];
    TexPrefix[0] = '\0';
    std::sprintf(TexPrefix, GetDBViewerInfo(m_iDbViewerNo).TexPrefix);

    int32 TexCnt = GetDBViewerInfo(m_iDbViewerNo).TexNum;
    for (int32 i = 0; i < TexCnt; ++i)
    {
        char TexName[256];
        TexName[0] = '\0';
        std::sprintf(TexName, "%s%02d", TexPrefix, i + 1);
        m_apTexViewer[i] = CTextureManager::GetRwTexture(TexName);
    };
};


bool CDatabaseViewer_Container::Run(void)
{
    const float ZoomMin = 1.0f;
    const float ZoomMax = 3.0f;
    const float ZoomStep = 0.05f;
    const float MoveStep = 32.0f;
    const float ScrollStep = 4.0f;

    int32 iPad = CGameData::Attribute().GetVirtualPad();
    
    m_bFlagImgChange = ( (!Math::FEqual(m_vImgPos.x, 0.0f)) || (!Math::FEqual(m_vImgPos.y, 0.0f)) );
    if (m_fImageZoom > 1.0f)
    {
        m_bFlagImgZoom = true;
    }
    else
    {
        m_bFlagImgZoom = false;
    };

    if (m_bFlagCancelZoom)
    {
        float fAnimStep = float(m_uCancelAnimCnt) / (CScreen::Framerate() * 0.5f);
        if (float(m_uCancelAnimCnt) < (CScreen::Framerate() * 0.5f))
            ++m_uCancelAnimCnt;

        if (m_bFlagImgChange)
        {
            if (m_vImgPos.x > 0.0f)
                m_vImgPos.x = m_vCancelStartPos.x - (m_vCancelStartPos.x * fAnimStep);
            else
                m_vImgPos.x = (m_vCancelStartPos.x * fAnimStep) - m_vCancelStartPos.x;

            if (m_vImgPos.y > 0.0f)
                m_vImgPos.y = m_vCancelStartPos.y - (m_vCancelStartPos.y * fAnimStep);
            else
                m_vImgPos.y = (m_vCancelStartPos.y * fAnimStep) - m_vCancelStartPos.y;
        }
        else if (m_bFlagImgZoom)
        {
            if (m_fImageZoom > ZoomMin)
                m_fImageZoom -= ZoomStep;
        }
        else
        {
            m_bFlagCancelZoom = false;
        };

        return false;
    };

    if (m_bFlagImgZoom || m_bFlagImgChange)
    {
        if (m_eMovedir == MOVEDIR_NONE)
        {
            if (CController::GetDigitalTrigger(iPad, CController::DIGITAL_CANCEL))
            {
                CGameSound::PlaySE(SDCODE_SE(0x1001));
                m_bFlagCancelZoom = true;
                m_uCancelAnimCnt = 0;

                float x = m_vImgPos.x;
                if (x < 0.0f)
                    x = -x;
                
                float y = m_vImgPos.y;
                if (y < 0.0f)
                    y = -y;

                m_vCancelStartPos = { x , y };                
            };
        };
    }
    else
    {
        if (CController::GetDigitalTrigger(iPad, CController::DIGITAL_CANCEL))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1001));
            return true;
        }
        else if (CController::GetDigitalTrigger(iPad, CController::DIGITAL_LEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            m_eMovedir = MOVEDIR_LEFT;
        }
        else if (CController::GetDigitalTrigger(iPad, CController::DIGITAL_RIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            m_eMovedir = MOVEDIR_RIGHT;
        };
    };

    if (CController::GetDigital(iPad, CController::DIGITAL_OK))
    {
        if (m_bFlagImgZoom)
        {
            float EndY = 178.0f;
            float EndX = 320.0f;

            if (CController::GetDigital(iPad, CController::DIGITAL_UP))
            {
                if (m_vImgPos.y < (EndY * (m_fImageZoom - 1.0f)))
                    m_vImgPos.y += ScrollStep;
            }
            else if (CController::GetDigital(iPad, CController::DIGITAL_DOWN))
            {
                if (m_vImgPos.y > (-EndY * (m_fImageZoom - 1.0f)))
                    m_vImgPos.y -= ScrollStep;
            };

            if (CController::GetDigital(iPad, CController::DIGITAL_LEFT))
            {
                if (m_vImgPos.x < (EndX * (m_fImageZoom - 1.0f)))
                    m_vImgPos.x += ScrollStep;
            }
            else if (CController::GetDigital(iPad, CController::DIGITAL_RIGHT))
            {
                if (m_vImgPos.x > (-EndX * (m_fImageZoom - 1.0f)))
                    m_vImgPos.x -= ScrollStep;
            };
        };
    }
    else if (!m_bFlagImgChange)
    {
        if (CController::GetDigital(iPad, CController::DIGITAL_UP))
        {
            if (m_fImageZoom < ZoomMax)
                m_fImageZoom += ZoomStep;
        }
        else if (CController::GetDigital(iPad, CController::DIGITAL_DOWN))
        {
            if (m_fImageZoom > ZoomMin)
                m_fImageZoom -= ZoomStep;
        };
    };

    switch (m_eMovedir)
    {
    case MOVEDIR_LEFT:
        {
            if (m_vImgPos.x >= 640.0f)
            {
                m_iImgCursor = GetViewerCGPos(MOVEDIR_LEFT);
                m_eMovedir = MOVEDIR_NONE;
                m_vImgPos.x = 0.0f;
                m_bFlagArrowAnimReverse = false;
                m_uArrowAnimCnt = 0;
            }
            else
            {
                m_vImgPos.x += MoveStep;
            };
        }
        break;

    case MOVEDIR_RIGHT:
        {
            if (m_vImgPos.x <= -640.0f)
            {
                m_iImgCursor = GetViewerCGPos(MOVEDIR_RIGHT);
                m_eMovedir = MOVEDIR_NONE;
                m_vImgPos.x = 0.0f;
                m_bFlagArrowAnimReverse = false;
                m_uArrowAnimCnt = 0;
            }
            else
            {
                m_vImgPos.x -= MoveStep;
            };
        }
        break;
    };

    return false;
};


void CDatabaseViewer_Container::Draw(void)
{
    if (!m_bSettingFlag)
        return;

    bool ArrowDrawFlag = false;

    float fAnimStep = (float(m_uAppearAnimCnt) / (CScreen::Framerate() * 0.5f));
    if (float(m_uAppearAnimCnt) >= (CScreen::Framerate() * 0.5f))
    {
        ArrowDrawFlag = true;
    }
    else
    {
        ++m_uAppearAnimCnt;
    };

    float sw = float(CScreen::Width());
    float sh = float(CScreen::Height());

    m_Sprite.SetOffset(0.5f, 0.5f);
    m_Sprite.SetTexture(m_pTexBace);
    m_Sprite.Move(0.0f, 0.0f);
    m_Sprite.ResizeRealScreen(sw, sh);
    m_Sprite.SetAlpha(255);
    m_Sprite.Draw();

    m_Sprite.SetOffset(0.5f, 0.5f);
    m_Sprite.SetTexture(m_apTexViewer[GetViewerCGPos(2)]);
    m_Sprite.Move(m_vImgPos.x - 640.0f, m_vImgPos.y - 50.0f);
    m_Sprite.Resize(640.0, 524.0f);
    m_Sprite.SetAlpha(255);
    m_Sprite.Draw();
    
    m_Sprite.SetOffset(0.5f, 0.5f);
    m_Sprite.SetTexture(m_apTexViewer[GetViewerCGPos(1)]);
    m_Sprite.Move(m_vImgPos.x + 640.0f, m_vImgPos.y - 50.0f);
    m_Sprite.Resize(640.0, 524.0f);
    m_Sprite.SetAlpha(255);
    m_Sprite.Draw();

    m_Sprite.SetOffset(0.5f, 0.5f);
    m_Sprite.SetTexture(m_apTexViewer[GetViewerCGPos(0)]);
    m_Sprite.Move(m_vImgPos.x, m_vImgPos.y - 50.0f);
    m_Sprite.Resize(
        (fAnimStep * m_fImageZoom) * 640.0f,
        (fAnimStep * m_fImageZoom) * 524.0f
    );
    m_Sprite.SetAlpha(uint8(fAnimStep * 255.0f));
    m_Sprite.Draw();

    m_Sprite.SetOffset(0.0f, 0.5f);
    m_Sprite.Resize(640.0f, 128.0f);
    m_Sprite.SetAlpha(255);
    m_Sprite.SetTexture(m_pTexBtWinpc);
    m_Sprite.Move(
        -320.0f,
        316.0f - (fAnimStep * 128.0f)
    );
    m_Sprite.Draw();

    if (ArrowDrawFlag)
    {
        ArrowDraw();
        
        if (!m_bFlagImgZoom)
        {
            char Buff[128];
            Buff[0] = '\0';
            std::sprintf(Buff, "%d/%d", m_iImgCursor + 1, m_iTexNum);
            
            CSystem2D::PushRenderState();
            CGameFont::m_fHeight = CGameFont::GetScreenSize() / 447.0f * 3.0f;
            CGameFont::SetRGBA(255, 180, 0, 255);
            CGameFont::Show(Buff, -260.0f, -152.0f);
            CSystem2D::PopRenderState();
        };
    };
};


void CDatabaseViewer_Container::ArrowDraw(void)
{
	uint8 AlphaBasis = uint8((float(m_uArrowAnimCnt) / (CScreen::Framerate() * 0.5f)) * 255.0f);
    if (m_bFlagArrowAnimReverse)
        AlphaBasis = (255 - AlphaBasis);

    m_Sprite.SetOffset(0.5f, 0.5f);
    m_Sprite.SetAlpha(AlphaBasis);

    if (!m_bFlagImgChange)
    {
        if (m_fImageZoom < 3.0f)
        {
            m_Sprite.SetTexture(m_pTexBtn03ps);
            m_Sprite.Resize(32.0f, 16.0f);
            m_Sprite.Move(-263.0f, 190.0f);
            m_Sprite.Draw();
        };

        if (m_fImageZoom > 1.0f)
        {
            m_Sprite.SetTexture(m_pTexBtn04ps);
            m_Sprite.Resize(32.0f, 16.0f);
            m_Sprite.Move(-106.0f, 190.0f);
            m_Sprite.Draw();
        };
    };

    if (!m_bFlagImgChange)
    {
        if (!m_bFlagImgZoom)
        {
            m_Sprite.SetTexture(m_pTexBtn06ps);
            m_Sprite.Resize(16.0f, 32.0f);
            m_Sprite.Move(-266.0f, 160.0f);
            m_Sprite.Draw();

            m_Sprite.SetTexture(m_pTexBtn05ps);
            m_Sprite.Resize(16.0f, 32.0f);
            m_Sprite.Move(-106.0f, 160.0f);
            m_Sprite.Draw();
        };
    };

    if (m_bFlagImgZoom)
    {
        m_Sprite.SetTexture(m_pTexBtn07ps);
        m_Sprite.Resize(64.0f, 64.0f);
        m_Sprite.Move(96.0f, 177.0f);
        m_Sprite.Draw();
    };

    if (float(m_uArrowAnimCnt) >= (CScreen::Framerate() * 0.5f))
    {
        m_bFlagArrowAnimReverse = !m_bFlagArrowAnimReverse;
        m_uArrowAnimCnt = 0;
    }
    else
    {
        ++m_uArrowAnimCnt;
    };

    m_Sprite.SetAlpha(255);
};


int32 CDatabaseViewer_Container::GetViewerCGPos(int32 Dir)
{
    if (Dir == MOVEDIR_RIGHT)
    {
        if (m_iImgCursor == (m_iTexNum - 1))
            return 0;
        else
            return (m_iImgCursor + 1);
    }
    else if (Dir == MOVEDIR_LEFT)
    {
        if (m_iImgCursor)
            return (m_iImgCursor - 1);
        else
            return (m_iTexNum - 1);
    };

    return m_iImgCursor;
};


static CDatabaseViewer_Container* s_pDatabaseViewer_Container = nullptr;


/*static*/ CProcess* CDatabaseViewer::Instance(void)
{
    return new CDatabaseViewer;
};


CDatabaseViewer::CDatabaseViewer(void)
: m_ePhase(PHASE_LOAD)
{
    ;
};


CDatabaseViewer::~CDatabaseViewer(void)
{
    ;
};


bool CDatabaseViewer::OnAttach(const void* param)
{
    if (!s_pDatabaseViewer_Container)
        s_pDatabaseViewer_Container = new CDatabaseViewer_Container;

    CTextureManager::GenerationInc();
    s_pDatabaseViewer_Container->Initialize();
    m_ePhase = PHASE_LOAD;

    return true;
};


void CDatabaseViewer::OnDetach(void)
{
    CTextureManager::GenerationDec();
    
    if (s_pDatabaseViewer_Container)
    {
        delete s_pDatabaseViewer_Container;
        s_pDatabaseViewer_Container = nullptr;
    };
};


void CDatabaseViewer::OnMove(bool bRet, const void* param)
{
    switch (m_ePhase)
    {
    case PHASE_LOAD:
        {
            CDataLoader::Period();
            
            if (CDataLoader::IsLoadEnd())
            {
                CGameSound::PlayBGM(SDCODE_BGM(0x3024));
                CScreenFade::StartOut(1.0f);
                m_ePhase = PHASE_RUN;
            };
        }
        break;

    case PHASE_RUN:
        {
            if (CScreenFade::IsFading())
                break;

            if (s_pDatabaseViewer_Container->Run())
            {
                CGameSound::FadeOut(CGameSound::FADESPEED_FAST);
                CScreenFade::StartIn(1.0f);
                m_ePhase = PHASE_END;
            };
        }
        break;

    case PHASE_END:
        {
            if (!CScreenFade::IsFading())
                Ret();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CDatabaseViewer::OnDraw(void) const
{
    if ((m_ePhase > PHASE_LOAD) && (m_ePhase <= PHASE_END))
    {
        CRenderStateManager::SetDefault();

        RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
        RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
        RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
        RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
        RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEFLAT);
        RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, 0);
        RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

        s_pDatabaseViewer_Container->Setting();
        s_pDatabaseViewer_Container->Draw();

        RENDERSTATE_POP(rwRENDERSTATECULLMODE);
        RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
        RENDERSTATE_POP(rwRENDERSTATESHADEMODE);
        RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
        RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
        RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESS);
        RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
        RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
        RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
        RENDERSTATE_POP(rwRENDERSTATETEXTUREFILTER);
        RENDERSTATE_POP(rwRENDERSTATETEXTUREPERSPECTIVE);
        RENDERSTATE_POP(rwRENDERSTATEBORDERCOLOR);

        CRenderStateManager::SetDefault();
    };
};