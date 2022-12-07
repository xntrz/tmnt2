#include "AreaSequence.hpp"
#include "AreaMenu.hpp"
#include "AreaTypes.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/MapInfo.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/TextData.hpp"
#include "System/Common/Process/ProcessList.hpp"


class CAreaWorkPool
{
public:
    enum
    {
        DIRECTION_UP = 0,
        DIRECTION_DOWN,
        DIRECTION_LEFT,
        DIRECTION_RIGHT,

        DIRECTIONUM,
    };

    enum ANIMATION
    {
        ANIMATION_NONE = 0,
        ANIMATION_KAMEMOVE,
        ANIMATION_CLEARED,
    };

    struct CLEARANIM
    {
        enum
        {
            STEP_NONE = 0,
            STEP_FADEOUT,
            STEP_SIZEIN,
            STEP_FADEIN,
        };

        int32 m_nStep;
        uint32 m_uCounter;
        uint8 m_uAlphaBasis;
        float m_fScale;
    };

public:
    CAreaWorkPool(void);
    ~CAreaWorkPool(void);
    void Attach(void);
    void Detach(void);
    bool Move(void);
    void UpdateAreaWarp(void);
    void AreaMenuSetting(void);
    void AreaMenuDraw(void);
    void TextureSetting(void);
    void TextureDraw(void);
    void AreaDisp(WORLDID::VALUE idWorld);
    void AreaTextDisp(int32 nIndex);
    void AreaLineDisp(int32 nIndex);
    void AreaMarkDisp(AREAID::VALUE idArea);
    void KameIconDisp(void);
    void ClearedAnimDisp(void);
    void AreaSelectGuide(void);
    void AreaMoveAnimationInit(int32 nDirection);
    void AreaMoveAnimation(void);
    void AreaClearedAnimationInit(void);
    void AreaClearedAnimation(void);
    bool GetLineDrawEnable(int32 nIndex);
    bool GetTextDrawEnable(int32 nIndex);
    void GetAreaPosition(RwV2d* pPosition, AREAID::VALUE idArea);
    AREAID::VALUE GetMoveToArea(AREAID::VALUE idArea, int32 nDirection);
    bool CheckAreaMove(AREAID::VALUE idArea, int32 nDirection);
    AREATYPES::NEXTSEQUENCE GetNextSequence(void) const;
    CAreaRecord::STATE GetAreaState(AREAID::VALUE idArea) const;
    void SetCurrentSelectedArea(AREAID::VALUE idArea);
    bool IsAreaRootCleared(AREAID::VALUE idArea, CAreaRecord::CLEAR_ROOT root);
    void DebugStartPlayClearAnim(void);
    void DebugAreaChanged(void);
    
private:
    AREATYPES::NEXTSEQUENCE m_NextSequence;
    RwV2d m_vKamePos;
    AREAID::VALUE m_OldArea;
    AREAID::VALUE m_NewArea;
    AREAID::VALUE m_NowArea;
    AREAID::VALUE m_ClearedArea;
    int32 m_Direction;
    bool m_bMenuOpenFlag;
    bool m_bStationWarpFlag;
    bool m_bAreaDiskFlag;
    bool m_bTextureSettingFlag;
    RwTexture* m_AreaSelTexture[17];
    RwTexture* m_AreaLineTexture[19];
    RwTexture* m_AreaTextTexture[6];
    CSprite m_sprite;
    CLEARANIM m_aClearAnim[4];
    float m_fClearAnimRot;
    ANIMATION m_Animation;
    uint32 m_KameMoveAnimCount;
    uint32 m_KameIconAnimCount;
    bool m_bKameClearRotAnimFlag;
    bool m_KameIconAnimFlag;
    bool m_bMenuInfoDispFlag;
};


CAreaWorkPool::CAreaWorkPool(void)
{
    ;
};


CAreaWorkPool::~CAreaWorkPool(void)
{
    ;
};


void CAreaWorkPool::Attach(void)
{
	m_Animation = ANIMATION_NONE;

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    if (idArea == AREAID::ID_AREA03)
        idArea = AREAID::ID_MNY_STN;

    m_OldArea = idArea;
    m_NewArea = idArea;
    m_NowArea = idArea;
    m_ClearedArea = CGameData::Record().Area().GetNowClearArea();

    if (m_ClearedArea)
        AreaClearedAnimationInit();

    std::memset(m_AreaSelTexture, 0x00, sizeof(m_AreaSelTexture));
    std::memset(m_AreaLineTexture, 0x00, sizeof(m_AreaLineTexture));
    std::memset(m_AreaTextTexture, 0x00, sizeof(m_AreaTextTexture));
    
    m_bTextureSettingFlag = false;
    m_KameIconAnimFlag = false;
    m_KameIconAnimCount = 0;
    m_KameMoveAnimCount = 0;
    m_bMenuOpenFlag = false;
	m_bStationWarpFlag = false;

    GetAreaPosition(&m_vKamePos, m_NewArea);

    m_bMenuInfoDispFlag = false;
};


void CAreaWorkPool::Detach(void)
{
    ;
};


bool CAreaWorkPool::Move(void)
{    
    m_bMenuInfoDispFlag = false;
    
    if (m_bMenuOpenFlag)
    {
        m_NextSequence = CAreaMenu::AreaMenuSelect();
        switch (m_NextSequence)
        {
        case AREATYPES::NEXTSEQUENCE_AREA:
            m_bMenuOpenFlag = false;
            break;

        case AREATYPES::NEXTSEQUENCE_WARP:
            m_bStationWarpFlag = true;
            return true;

        case AREATYPES::NEXTSEQUENCE_CHARASEL:
        case AREATYPES::NEXTSEQUENCE_MENUSAVE:
        case AREATYPES::NEXTSEQUENCE_MENULOAD:
        case AREATYPES::NEXTSEQUENCE_OPTIONS:
        case AREATYPES::NEXTSEQUENCE_TITLE:            
            return true;
        };

        return false;
    };

    switch (m_Animation)
    {
    case ANIMATION_KAMEMOVE:
        AreaMoveAnimation();
        return false;

    case ANIMATION_CLEARED:
        AreaClearedAnimation();
        return false;
    };

    if (m_NowArea >= AREAID::SELECTABLEMAX)
    {
        AreaMoveAnimationInit(m_Direction);
        return false;
    };

    m_bMenuInfoDispFlag = true;
    int32 iController = CGameData::Attribute().GetVirtualPad();

    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_UP))
    {
        AreaMoveAnimationInit(DIRECTION_UP);
        m_Direction = DIRECTION_UP;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_DOWN))
    {
        AreaMoveAnimationInit(DIRECTION_DOWN);
        m_Direction = DIRECTION_DOWN;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LEFT))
    {
        AreaMoveAnimationInit(DIRECTION_LEFT);
        m_Direction = DIRECTION_LEFT;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_RIGHT))
    {
        AreaMoveAnimationInit(DIRECTION_RIGHT);
        m_Direction = DIRECTION_RIGHT;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
    {
        if (CAreaSequence::m_bDebugClearAnim)
        {
            DebugStartPlayClearAnim();
            return false;
        }
        else
        {
            if (m_NowArea >= AREAID::NORMALMAX)
            {
                if (m_NowArea >= AREAID::WARPSTART && m_NowArea < AREAID::WARPMAX)
                {
                    CGameSound::PlaySE(SDCODE_SE(4112));
                    m_NextSequence = AREATYPES::NEXTSEQUENCE_AREA;
                    return true;
                }

                if (m_NowArea != AREAID::ID_MNY_STN)
                    return false;

                CGameSound::PlaySE(SDCODE_SE(4098));
                m_NextSequence = AREATYPES::NEXTSEQUENCE_AREA;
                return true;
            }
            else
            {
                CGameSound::PlaySE(SDCODE_SE(8196));
                m_NextSequence = AREATYPES::NEXTSEQUENCE_AREA;
                return true;
            };
        };     
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LEFT_BUMPER))
    {

        m_bMenuOpenFlag = true;
        CAreaMenu::AreaMenuInit();
    };

    return false;
};


void CAreaWorkPool::UpdateAreaWarp(void)
{
    if (m_bStationWarpFlag)
    {
        m_bStationWarpFlag = false;
        CAreaMenu::AreaMenuStationWarp();
        return;
    };

    switch (m_NowArea)
    {
    case AREAID::ID_MNY_STN:
        SetCurrentSelectedArea(AREAID::HOME);
        break;
        
    case AREAID::ID_MNY_E01:
        SetCurrentSelectedArea(AREAID::ID_DHO_E01);
        break;

    case AREAID::ID_MNY_E02:
        SetCurrentSelectedArea(AREAID::ID_DHO_E02);
        break;

    case AREAID::ID_MNY_E03:
        SetCurrentSelectedArea(AREAID::ID_JPN_E02);
        break;

    case AREAID::ID_MNY_E04:
        SetCurrentSelectedArea(AREAID::ID_FNY_E01);
        break;

    case AREAID::ID_MNY_E05:
        SetCurrentSelectedArea(AREAID::ID_KUR_E01);
        break;

    case AREAID::ID_DHO_E01:
        SetCurrentSelectedArea(AREAID::ID_MNY_E01);
        break;

    case AREAID::ID_DHO_E02:
        SetCurrentSelectedArea(AREAID::ID_MNY_E02);
        break;

    case AREAID::ID_DHO_E03:
        SetCurrentSelectedArea(AREAID::ID_TRI_E01);
        break;

    case AREAID::ID_TRI_E01:
        SetCurrentSelectedArea(AREAID::ID_DHO_E03);
        break;

    case AREAID::ID_TRI_E02:
        SetCurrentSelectedArea(AREAID::ID_JPN_E01);
        break;

    case AREAID::ID_JPN_E01:
        SetCurrentSelectedArea(AREAID::ID_TRI_E02);
        break;

    case AREAID::ID_JPN_E02:
        SetCurrentSelectedArea(AREAID::ID_MNY_E03);
        break;

    case AREAID::ID_FNY_E01:
        SetCurrentSelectedArea(AREAID::ID_MNY_E04);
        break;

    case AREAID::ID_KUR_E01:
        SetCurrentSelectedArea(AREAID::ID_MNY_E05);
        break;

    default:
        if (m_NowArea < AREAID::ID_MNY_E01 || m_NowArea > AREAID::ID_KUR_E01)
        {
            if (m_NowArea < AREAID::NORMALMAX)
            {
                CGameData::PlayParam().ClearArea();
                CGameData::PlayParam().SetStartArea(m_NowArea, 0);
            };
        };
        break;
    };
};


void CAreaWorkPool::AreaMenuSetting(void)
{
    CAreaMenu::AreaMenuPeriod();
};


void CAreaWorkPool::AreaMenuDraw(void)
{
    if (m_bMenuOpenFlag)
    {
        CAreaMenu::AreaMenuDraw();
    }
    else if (m_bMenuInfoDispFlag)
    {
        CAreaMenu::AreaInfoMenuDraw();
    };
};


void CAreaWorkPool::TextureSetting(void)
{
    if (m_bTextureSettingFlag)
        return;

    m_bTextureSettingFlag = true;

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();
    int32 nLineNum = 0;
    
    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        CTextureManager::SetCurrentTextureSet("area_ny");
        nLineNum = 19;
        break;

    case WORLDID::ID_DHO:
        CTextureManager::SetCurrentTextureSet("area_dho");
        nLineNum = 14;
        break;

    case WORLDID::ID_TRI:
        CTextureManager::SetCurrentTextureSet("area_tri");
        nLineNum = 11;
        break;

    case WORLDID::ID_JPN:
        CTextureManager::SetCurrentTextureSet("area_jpn");
        nLineNum = 9;
        break;

    case WORLDID::ID_FNY:
        CTextureManager::SetCurrentTextureSet("area_fny");
        nLineNum = 5;
        break;

    case WORLDID::ID_KUR:
        CTextureManager::SetCurrentTextureSet("area_kur");
        nLineNum = 6;
        break;

    default:
        ASSERT(false);
        break;
    };

    for (int32 i = 0; i < nLineNum; i++)
    {
        static char s_szLineTexName[256];
        s_szLineTexName[0] = '\0';

        std::sprintf(s_szLineTexName, "ar_sen%03d", i + 1);
        m_AreaLineTexture[i] = CTextureManager::GetRwTexture(s_szLineTexName);
        ASSERT(m_AreaLineTexture[i]);
    };

    m_AreaSelTexture[0] = CTextureManager::GetRwTexture("ar_aricon0");
    m_AreaSelTexture[1] = CTextureManager::GetRwTexture("ar_aricon1");
    m_AreaSelTexture[2] = CTextureManager::GetRwTexture("ar_aricon2");
    m_AreaSelTexture[3] = CTextureManager::GetRwTexture("ar_station_icon1");
    m_AreaSelTexture[4] = CTextureManager::GetRwTexture("ar_kame_icon");
    m_AreaSelTexture[5] = CTextureManager::GetRwTexture("ar_kame_up");
    m_AreaSelTexture[6] = CTextureManager::GetRwTexture("ar_kame_down");
    m_AreaSelTexture[7] = CTextureManager::GetRwTexture("ar_kame_left");
    m_AreaSelTexture[8] = CTextureManager::GetRwTexture("ar_kame_right");
    m_AreaSelTexture[9] = CTextureManager::GetRwTexture("ar_kemuri");
    m_AreaSelTexture[10] = CTextureManager::GetRwTexture("ar_iconpf_e");
    m_AreaSelTexture[11] = CTextureManager::GetRwTexture("ar_iconpf_d");
    m_AreaSelTexture[12] = CTextureManager::GetRwTexture("ar_iconpf_c");
    m_AreaSelTexture[13] = CTextureManager::GetRwTexture("ar_iconpf_b");
    m_AreaSelTexture[14] = CTextureManager::GetRwTexture("ar_iconpf_a");
    m_AreaSelTexture[15] = CTextureManager::GetRwTexture("ar_iconpf_s");
    m_AreaSelTexture[16] = CTextureManager::GetRwTexture("ar_iconpf_ss");

    ASSERT(m_AreaSelTexture[0]);
    ASSERT(m_AreaSelTexture[1]);
    ASSERT(m_AreaSelTexture[2]);
    ASSERT(m_AreaSelTexture[3]);
    ASSERT(m_AreaSelTexture[4]);
    ASSERT(m_AreaSelTexture[5]);
    ASSERT(m_AreaSelTexture[5]);
    ASSERT(m_AreaSelTexture[6]);
    ASSERT(m_AreaSelTexture[7]);
    ASSERT(m_AreaSelTexture[8]);
    ASSERT(m_AreaSelTexture[9]);
    ASSERT(m_AreaSelTexture[10]);
    ASSERT(m_AreaSelTexture[11]);
    ASSERT(m_AreaSelTexture[12]);
    ASSERT(m_AreaSelTexture[13]);
    ASSERT(m_AreaSelTexture[14]);
    ASSERT(m_AreaSelTexture[15]);
    ASSERT(m_AreaSelTexture[16]);

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        m_AreaTextTexture[0] = CTextureManager::GetRwTexture("ar_ny_text_homepsd");
        m_AreaTextTexture[1] = CTextureManager::GetRwTexture("ar_ny_text_start");
        m_AreaTextTexture[2] = CTextureManager::GetRwTexture("ar_ny_text_tohatena");    // fny
        m_AreaTextTexture[3] = CTextureManager::GetRwTexture("ar_ny_text_tojapan");
        m_AreaTextTexture[4] = CTextureManager::GetRwTexture("ar_ny_text_tospace");
        m_AreaTextTexture[5] = CTextureManager::GetRwTexture("ar_ny_text_tokuraiyama");
        ASSERT(m_AreaTextTexture[0]);
        ASSERT(m_AreaTextTexture[1]);
        ASSERT(m_AreaTextTexture[2]);
        ASSERT(m_AreaTextTexture[3]);
        ASSERT(m_AreaTextTexture[4]);
        ASSERT(m_AreaTextTexture[5]);
        break;

    case WORLDID::ID_DHO:
        m_AreaTextTexture[0] = CTextureManager::GetRwTexture("ar_dh_text_tony");
        m_AreaTextTexture[1] = CTextureManager::GetRwTexture("ar_dh_text_toriceraton");
        ASSERT(m_AreaTextTexture[0]);
        ASSERT(m_AreaTextTexture[1]);
        break;

    case WORLDID::ID_TRI:
        m_AreaTextTexture[0] = CTextureManager::GetRwTexture("ar_tr_text_todhoonib");
        m_AreaTextTexture[1] = CTextureManager::GetRwTexture("ar_tr_text_tojapan");
        ASSERT(m_AreaTextTexture[0]);
        ASSERT(m_AreaTextTexture[1]);
        break;

    case WORLDID::ID_JPN:
        m_AreaTextTexture[0] = CTextureManager::GetRwTexture("ar_jp_text_tony");
        m_AreaTextTexture[1] = CTextureManager::GetRwTexture("ar_jp_text_tospace");
        ASSERT(m_AreaTextTexture[0]);
        ASSERT(m_AreaTextTexture[1]);
        break;

    case WORLDID::ID_FNY:
        m_AreaTextTexture[0] = CTextureManager::GetRwTexture("ar_fny_text_tony");
        ASSERT(m_AreaTextTexture[0]);
        break;

    case WORLDID::ID_KUR:
        m_AreaTextTexture[0] = CTextureManager::GetRwTexture("ar_ku_text_tony");
        ASSERT(m_AreaTextTexture[0]);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CAreaWorkPool::TextureDraw(void)
{
    if (!m_bTextureSettingFlag)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, RwCullMode::rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();
    WORLDID::VALUE idWorld = CAreaInfo::GetWorldNo(idArea);
    
    AreaDisp(idWorld);
    KameIconDisp();
    ClearedAnimDisp();
    AreaSelectGuide();

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);

    if (m_KameIconAnimCount >= CScreen::Framerate() * 0.5f)
    {
        m_KameIconAnimFlag = !m_KameIconAnimFlag;
        m_KameIconAnimCount = 0;
    }
    else
    {
        ++m_KameIconAnimCount;
    };
};


void CAreaWorkPool::AreaDisp(WORLDID::VALUE idWorld)
{
    int32 nTextDisp = 0;
    int32 nLineDisp = 0;

    switch (idWorld)
    {
    case WORLDID::ID_MNY:
        CTextureManager::SetCurrentTextureSet("area_ny");
        nTextDisp = 6;
        nLineDisp = 19;
        break;

    case WORLDID::ID_DHO:
        CTextureManager::SetCurrentTextureSet("area_dho");
        nTextDisp = 2;
        nLineDisp = 14;
        break;

    case WORLDID::ID_TRI:
        CTextureManager::SetCurrentTextureSet("area_tri");
        nTextDisp = 2;
        nLineDisp = 11;
        break;

    case WORLDID::ID_JPN:
        CTextureManager::SetCurrentTextureSet("area_jpn");
        nTextDisp = 2;
        nLineDisp = 9;
        break;

    case WORLDID::ID_FNY:
        CTextureManager::SetCurrentTextureSet("area_fny");
        nTextDisp = 1;
        nLineDisp = 5;
        break;

    case WORLDID::ID_KUR:
        CTextureManager::SetCurrentTextureSet("area_kur");
        nTextDisp = 1;
        nLineDisp = 6;
        break;

    default:
        ASSERT(false);
        break;
    };

    for (int32 i = 1; i <= nLineDisp; ++i)
        AreaLineDisp(i);

    for (int32 i = 0; i < nTextDisp; ++i)
        AreaTextDisp(i);

    for (int32 i = AREAID::SELECTABLESTART; i < AREAID::SELECTABLEMAX; ++i)
    {
        AREAID::VALUE idArea = AREAID::VALUE(i);
	
        if (CAreaInfo::GetWorldNo(idArea) == idWorld                     &&
            (idArea < AREAID::ID_MNY_J01 || idArea > AREAID::ID_KUR_J01) &&
            idArea < AREAID::STATIONMAX                                  &&
            idArea != AREAID::HOME)
        {
            AreaMarkDisp(idArea);
        };
    };
};


void CAreaWorkPool::AreaTextDisp(int32 nIndex)
{
    static const Rt2dBBox s_aTextDispTable[] =
    {
        //  x         y           w       h

        //
        //  NY
        //
        { 1.0f,     53.0f,      64.0f,  64.0f,  },
        { -145.0f,  -130.0f,    64.0f,  32.0f,  },
        { -264.0f,  56.0f,      64.0f,  64.0f,  },
        { 202.0f,   -50.0f,     64.0f,  128.0f, },
        { 90.0f,    -202.0f,    128.0f, 64.0f,  },
        { -282.0f,  -8.0,       128.0f, 64.0f,  },

        //
        //  DHO
        //
        { -276.0f,  56.0f,      64.0f,  64.0f,  },
        { 116.0f,   93.0f,      128.0f, 64.0f,  },

        //
        //  TRI
        //
        { -196.0f,  116.0f,     128.0f, 32.0f,  },
        { 188.0f,   102.0f,     64.0f,  64.0f,  },

        //
        //  JPN
        //
        { -272.0f,  92.0f,      128.0f, 32.0f,  },
        { 172.0f,   -200.0f,    128.0f, 64.0f,  },

        //
        //  FNY
        //
        { -268.0f,  32.0f,      64.0f,  64.0f,  },

        //
        //  KURA
        //
        { 172.0f,   92.0f,      128.0f, 64.0f,  },
    };

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();
    int32 nTextDispIdx = nIndex;

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_DHO:
        nTextDispIdx += 6;
        break;

    case WORLDID::ID_TRI:
        nTextDispIdx += 8;
        break;

    case WORLDID::ID_JPN:
        nTextDispIdx += 10;
        break;

    case WORLDID::ID_FNY:
        nTextDispIdx += 12;
        break;

    case WORLDID::ID_KUR:
        nTextDispIdx += 13;
        break;
    };

    ASSERT(nTextDispIdx >= 0 && nTextDispIdx < COUNT_OF(s_aTextDispTable));

    if (GetTextDrawEnable(nIndex))
    {
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetTexture(m_AreaTextTexture[ nIndex ]);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Move(
            s_aTextDispTable[nTextDispIdx].x,
            s_aTextDispTable[nTextDispIdx].y
        );
        m_sprite.Resize(
            s_aTextDispTable[nTextDispIdx].w,
            s_aTextDispTable[nTextDispIdx].h
        );
        m_sprite.Draw();
    };
};


void CAreaWorkPool::AreaLineDisp(int32 nIndex)
{
    static const Rt2dBBox s_aLineDispTable[] =
    {
        //
        //  NY
        //
        { -83.0f,   -158.0f,    32.0f,  64.0f   },
        { -84.0f,   -188.0f,    256.0f, 256.0f, },        
        { -33.0f,   66.0f,      128.0f, 64.0f,  },    
        { 97.0f,    92.0f,      64.0f,  32.0f,  },        
        { 151.0f,   69.0f,      64.0f,  32.0f,  },        
        { 108.0f,   -108.0f,    32.0f,  64.0f,  },        
        { 158.0f,   -112.0f,    32.0f,  16.0f,  },        
        { 181.0f,   -226.0f,    32.0f,  128.0f, },        
        { 208.0f,   -218.0f,    64.0f,  128.0f, },        
        { 219.0f,   -66.0f,     64.0f,  32.0f,  },        
        { 177.0f,   -53.0f,     64.0f,  16.0f,  },        
        { 79.0f,    -23.0f,     128.0f, 32.0f,  },        
        { -151.0f,  -53.0f,     128.0f, 128.0f, },        
        { -105.0f,  87.0f,      16.0f,  32.0f,  },        
        { -216.0f,  -51.0f,     64.0f,  32.0f,  },        
        { -158.0f,  23.0f,      8.0f,   32.0f,  },        
        { -192.0f,  64.0f,      64.0f,  64.0f,  },
        { -267.0f,  113.0f,     64.0f,  32.0f,  },
        { -279.0f,  -33.0f,     128.0f, 64.0f,  },

        //
        //  DHO
        //
        { 128.0f,   -36.0f,     32.0f,  16.0f,  },
        { 166.0f,   -10.0f,     16.0f,  16.0f,  },
        { 164.0f,   34.0f,      16.0f,  32.0f,  },
        { 176.0f,   88.0f,      32.0f,  16.0f,  },
        { 240.0f,   76.0f,      32.0f,  32.0f,  },
        { 56.0f,    -18.0f,     64.0f,  64.0f,  },
        { 92.0f,    64.0f,      16.0f,  32.0f,  },
        { 20.0f,    28.0f,      32.0f,  32.0f,  },
        { 40.0f,    -44.0f,     32.0f,  16.0f,  },
        { -200.0f,  -28.0f,     256.0f, 256.0f, },
        { -108.0f,  -64.0f,     128.0f, 32.0f,  },
        { -224.0f,  -64.0f,     128.0f, 32.0f,  },
        { -228.0f,  -32.0f,     32.0f,  64.0f,  },
        { -260.0f,  20.0f,      128.0f, 128.0f, },

        //
        //  TRI
        //
        { -264.0f,  100.0f,     128.0f, 64.0f,  },
        { -168.0f,  -24.0f,     256.0f, 128.0f, },
        { -4.0f,    -40.0f,     32.0f,  16.0f,  },
        { 60.0f,    -52.0f,     32.0f,  16.0f,  },
        { 112.0f,   -50.0f,     64.0f,  16.0f,  },
        { 179.0f,   -23.0f,     32.0f,  8.0f,   },
        { 220.0f,   -16.0f,     16.0f,  64.0f,  },
        { 148.0f,   52.0f,      128.0f, 128.0f, },
        { 136.0f,   -12.0f,     32.0f,  128.0f, },
        { 60.0f,    60.0f,      64.0f,  32.0f,  },
        { -8.0f,    -8.0f,      64.0f,  64.0f,  },

        //
        //  JPN
        //
        { 224.0f,   -160.0f,    64.0f,  128.0f, },
        { 188.0f,   -64.0f,     64.0f,  32.0f,  },
        { 172.0f,   -28.0f,     64.0f,  64.0f,  },
        { 64.0f,    60.0f,      256.0f, 64.0f,  },
        { -172.0f,  64.0f,      256.0f, 64.0f,  },
        { -276.0f,  64.0f,      128.0f, 32.0f,  },
        { -184.0f,  -10.0f,     64.0f,  64.0f,  },
        { -104.0f,  -48.0f,     128.0f, 32.0f,  },
        { 48.0f,    -52.0f,     128.0f, 16.0f,  },

        //
        //  FNY
        //
        { -268.0f,  100.0f,     128.0f, 32.0f,  },
        { -116.0f,  32.0f,      128.0f, 128.0f, },
        { 24.0f,    -44.0f,     64.0f,  128.0f, },
        { 12.0f,    -124.0f,    64.0f,  64.0f,  },
        { -204.0f,  -120.0f,    256.0f, 256.0f, },

        //
        //  KURA
        //
        { 116.0f,   -36.0f,     128.0f, 128.0f, },
        { 92.0f,    52.0f,      16.0f,  64.0f,  },
        { 120.0f,   112.0f,     128.0f, 64.0f,  },
        { -100.0f,  30.0f,      256.0f, 16.0f,  },
        { -150.0f,  -48.0f,     32.0f,  128.0f, },        
        { -144.0f,  -64.0f,     128.0f, 64.0f,  },
    };

    if (!nIndex)
        return;

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();
    int32 nLineDispIdx = nIndex - 1;

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_DHO:
        nLineDispIdx += 19;
        break;

    case WORLDID::ID_TRI:
        nLineDispIdx += 33;
        break;

    case WORLDID::ID_JPN:
        nLineDispIdx += 44;
        break;

    case WORLDID::ID_FNY:
        nLineDispIdx += 53;
        break;

    case WORLDID::ID_KUR:
        nLineDispIdx += 58;
        break;

    default:
        //ASSERT(false);
        break;
    };

    ASSERT(nLineDispIdx >= 0 && nLineDispIdx < COUNT_OF(s_aLineDispTable));
    
    if (GetLineDrawEnable(nIndex))
    {
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetTexture(m_AreaLineTexture[ nIndex - 1 ]);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Move(
            s_aLineDispTable[nLineDispIdx].x,
            s_aLineDispTable[nLineDispIdx].y
        );
        m_sprite.Resize(
            s_aLineDispTable[nLineDispIdx].w,
            s_aLineDispTable[nLineDispIdx].h
        );
        m_sprite.Draw();
    };
};


void CAreaWorkPool::AreaMarkDisp(AREAID::VALUE idArea)
{
    CAreaRecord::STATE AreaState = GetAreaState(idArea);

    if (idArea >= AREAID::ID_MNY_STN && idArea <= AREAID::ID_KUR_STN)
    {
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Resize(128.0f, 64.0f);
        m_sprite.SetTexture(m_AreaSelTexture[3]);

        switch (idArea)
        {
        case AREAID::ID_MNY_STN:
            m_sprite.Move(-72.0f, 22.0f);
            break;
            
        case AREAID::ID_DHO_STN:
            m_sprite.Move(35.0f, -63.0f);
            break;
            
        case AREAID::ID_TRI_STN:
            m_sprite.Move(-229.0f, 60.0f);
            break;
            
        case AREAID::ID_JPN_STN:
            m_sprite.Move(170.0f, -104.0f);
            break;
            
        case AREAID::ID_FNY_STN:
            m_sprite.Move(-62.0f, -3.0f);
            break;
            
        case AREAID::ID_KUR_STN:
            m_sprite.Move(35.0f, 11.0f);
            break;
            
        default:
            ASSERT(false);
            break;
        };

        m_sprite.Draw();
        return;
    };

    if (m_ClearedArea && idArea == m_ClearedArea)
        AreaState = CAreaRecord::STATE_OPEN;    

    RwV2d vAreaPos = Math::VECTOR2_ZERO;
    GetAreaPosition(&vAreaPos, idArea);

    m_sprite.ResetUV();
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetOffset(0.0f, 0.0f);
    m_sprite.Resize(64.0f, 64.0f);
    m_sprite.SetTexture(m_AreaSelTexture[AreaState]);
    m_sprite.Move(vAreaPos.x, vAreaPos.y);
    m_sprite.Draw();

    if (AreaState == CAreaRecord::STATE_CLEAR)
    {
        GAMETYPES::CLEARRANK AreaClearrank = CGameData::Record().Area().GetAreaClearRank(idArea);
        if (AreaClearrank)
        {
            m_sprite.SetTexture(m_AreaSelTexture[AreaClearrank + 9]);
            m_sprite.Draw();
        };
    };
};


void CAreaWorkPool::KameIconDisp(void)
{
    if (m_ClearedArea != AREAID::ID_NONE)
        return;

    m_sprite.ResetUV();
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetTexture(m_AreaSelTexture[ 4 ]);
    m_sprite.Move(m_vKamePos.x + 32.0f, m_vKamePos.y + 32.0f);
    m_sprite.Resize(64.0f, 64.0f);
    m_sprite.Draw();

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

    uint8 AlphaBasis = uint8(Math::LinearTween(
        0.0f,
        255.0f,
        float(m_KameIconAnimCount),
        float(CScreen::Framerate() * 0.5f)
    ));

    if (!m_KameIconAnimFlag)
        AlphaBasis = 255 - AlphaBasis;
    
    m_sprite.SetAlpha(AlphaBasis);
    m_sprite.Draw();
    m_sprite.SetAlpha(255);
    
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);

    if (m_Animation != ANIMATION_KAMEMOVE && m_NowArea < AREAID::SELECTABLEMAX)
    {
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Resize(32.0f, 32.0f);

        for (int32 i = 0; i < DIRECTIONUM; ++i)
        {
            if (!CheckAreaMove(m_NowArea, i))
                continue;

            static const RwV2d s_aArrowOffsetTable[] =
            {
                {   16.0f,  -16.0f, },
                {   16.0f,  48.0f,  },
                {   -12.0f, 16.0f,  },
                {   44.0f,  16.0f,  },
            };

            static_assert(COUNT_OF(s_aArrowOffsetTable) == DIRECTIONUM, "update me");
            ASSERT(i >= 0 && i < COUNT_OF(s_aArrowOffsetTable));

            m_sprite.SetTexture(m_AreaSelTexture[5 + i]);
            m_sprite.Move(
                s_aArrowOffsetTable[i].x + m_vKamePos.x,
                s_aArrowOffsetTable[i].y + m_vKamePos.y
            );
            m_sprite.Draw();
        };
    };
};


void CAreaWorkPool::ClearedAnimDisp(void)
{
    if (m_Animation != ANIMATION_CLEARED)
        return;

    RwV2d vAreaPos = Math::VECTOR2_ZERO;
    GetAreaPosition(&vAreaPos, m_ClearedArea);

    m_sprite.ResetUV();
    m_sprite.SetOffset(0.5f, 0.5f);

    //
    //  Kemuri anim
    //
    m_sprite.SetTexture(m_AreaSelTexture[9]);
    m_sprite.Move(vAreaPos.x + 32.0f, vAreaPos.y + 32.0f);
    m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[0].m_uAlphaBasis);
    m_sprite.Resize(m_aClearAnim[0].m_fScale, m_aClearAnim[0].m_fScale);
    m_sprite.Draw();

    //
    //  Area complete icon
    //
    m_sprite.SetTexture(m_AreaSelTexture[2]);
    m_sprite.Move(vAreaPos.x + 32.0f, vAreaPos.y + 32.0f);
    m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[1].m_uAlphaBasis);
    m_sprite.Resize(m_aClearAnim[1].m_fScale, m_aClearAnim[1].m_fScale);
    m_sprite.Draw();

    //
    //  Area clearrank
    //
    GAMETYPES::CLEARRANK AreaClearrank = CGameData::Record().Area().GetAreaClearRank(m_ClearedArea);
    if(AreaClearrank)
    {
        m_sprite.SetTexture(m_AreaSelTexture[AreaClearrank + 9]);
        m_sprite.Move(vAreaPos.x + 32.0f, vAreaPos.y + 32.0f);
        m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[2].m_uAlphaBasis);
        m_sprite.Resize(m_aClearAnim[2].m_fScale, m_aClearAnim[2].m_fScale);
        m_sprite.Draw();
    };

    if (m_bKameClearRotAnimFlag)
    {
        m_sprite.SetTexture(m_AreaSelTexture[4]);
        m_sprite.Move(m_vKamePos.x + 32.0f, m_vKamePos.y + 32.0f);
        m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[3].m_uAlphaBasis);
        m_sprite.Resize(m_aClearAnim[3].m_fScale, m_aClearAnim[3].m_fScale);
        m_sprite.SetRotate(m_fClearAnimRot);
        m_sprite.DrawRotate();
    };
};


void CAreaWorkPool::AreaSelectGuide(void)
{
    CGameFont::m_pFont->SetRGBA(255, 170, 0, 255);

    if (CAreaInfo::GetWorldNo(m_NowArea) == WORLDID::ID_FNY &&
        GetAreaState(AREAID::ID_AREA50) == CAreaRecord::STATE_CLEAR)
    {
        if (m_NowArea != AREAID::ID_FNY_STN)
        {
            const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(155));
            CGameFont::m_pFont->Show(
                pwszText,
                CGameFont::GetScreenSize() / 179,
                -269.0f,
                -175.0f
            );
        };
    }
    else
    {
        const wchar* pwszText = CAreaInfo::GetDispWorldName(m_NowArea);
        CGameFont::m_pFont->Show(
            pwszText,
            CGameFont::GetScreenSize() * 0.0055803573f,
            -269.0f,
            -175.0f
        );
    };

    if (m_bAreaDiskFlag)
    {
        const wchar* pwszText = CSystemText::GetText(SYSTEXT::VALUE(107));

        wchar wszBuffer[1024];
        wszBuffer[0] = UTEXT('\0');
        
        CTextData::Sprintf(wszBuffer, pwszText, CAreaInfo::GetDiscNo(m_NowArea));
        
        CGameFont::m_pFont->SetRGBA(255, 0, 0, 255);
        CGameFont::m_pFont->Show(
            wszBuffer,
            CGameFont::GetScreenSize() * 0.0044642859f,
            -269.0f,
            152.0f
        );
    };
};


void CAreaWorkPool::AreaMoveAnimationInit(int32 nDirection)
{
    AREAID::VALUE idAreaNew = GetMoveToArea(m_NowArea, nDirection);
    if (CheckAreaMove(m_NowArea, nDirection))
    {
        m_OldArea = m_NewArea;
        m_NewArea = idAreaNew;
        m_Animation = ANIMATION_KAMEMOVE;
        CGameSound::PlaySE(SDCODE_SE(4110));
    };
};


void CAreaWorkPool::AreaMoveAnimation(void)
{
    RwV2d vOldPos = Math::VECTOR2_ZERO;
    RwV2d vNewPos = Math::VECTOR2_ZERO;

    GetAreaPosition(&vOldPos, m_OldArea);
    GetAreaPosition(&vNewPos, m_NewArea);

    float x = 0.0f;
    float y = 0.0f;

    if (vOldPos.x <= vNewPos.x)
        x = vNewPos.x - vOldPos.x;
    else
        x = vOldPos.x - vNewPos.x;

    if (vOldPos.y <= vNewPos.y)
        y = vNewPos.y - vOldPos.y;
    else
        y = vOldPos.y - vNewPos.y;

    float len = std::sqrt(x * x + y * y);    
    float step = len / (CScreen::Framerate() * 0.4f);

    x *= (1.0f / len) * step;
    y *= (1.0f / len) * step;

    if (m_KameMoveAnimCount >= CScreen::Framerate() * 0.4f)
    {        
        GetAreaPosition(&m_vKamePos, m_NewArea);
        
		m_KameMoveAnimCount = 0;
        m_Animation = ANIMATION_NONE;
        m_NowArea = m_NewArea;

#ifdef _DEBUG        
        DebugAreaChanged();
#endif
        
        if (m_NowArea < AREAID::SELECTABLEMAX)
            CGameData::Record().Area().SetCurrentSelectedArea(m_NowArea);
    }
    else
    {
        if (vOldPos.x <= vNewPos.x)
            m_vKamePos.x = x + m_vKamePos.x;
        else
            m_vKamePos.x = m_vKamePos.x - x;

        if (vOldPos.y <= vNewPos.y)
            m_vKamePos.y = y + m_vKamePos.y;
        else
            m_vKamePos.y = m_vKamePos.y - y;
        
        ++m_KameMoveAnimCount;
    };
};


void CAreaWorkPool::AreaClearedAnimationInit(void)
{
    for (int32 i = 0;i < COUNT_OF(m_aClearAnim); ++i)
    {
        m_aClearAnim[i].m_nStep         = 0;
        m_aClearAnim[i].m_uCounter      = 0;
        m_aClearAnim[i].m_uAlphaBasis   = 0;
        m_aClearAnim[i].m_fScale        = 0.0f;
    };

    m_fClearAnimRot         = 0.0f;
    m_Animation             = ANIMATION_CLEARED;
    m_bKameClearRotAnimFlag = false;

    //
    //  Animation sequence:
    //  Status -> Rank -> Kemuri -> Kame    
    // 
    m_aClearAnim[1].m_nStep = CLEARANIM::STEP_FADEOUT;
};


void CAreaWorkPool::AreaClearedAnimation(void)
{
    if (m_bKameClearRotAnimFlag)
    {
        uint32 uKameClearAnimDur = uint32(CScreen::Framerate() * 0.5f);
        
        if (m_aClearAnim[3].m_uCounter >= uKameClearAnimDur)
        {
            m_ClearedArea = AREAID::ID_NONE;
            m_Animation = ANIMATION_NONE;
        }
        else
        {
            ++m_aClearAnim[3].m_uCounter;
        };

        m_aClearAnim[3].m_fScale = Math::LinearTween(
            192.0f,
            -128.0f,
            float(m_aClearAnim[3].m_uCounter),
            float(uKameClearAnimDur)
        );
        
        m_aClearAnim[3].m_uAlphaBasis = uint8(Math::LinearTween(
            0.0f,
            255.0f,
            float(m_aClearAnim[3].m_uCounter),
            float(uKameClearAnimDur)
        ));

        m_fClearAnimRot = Math::LinearTween(
            360.0f,
            -360.0f,
            float(m_aClearAnim[3].m_uCounter),
            float(uKameClearAnimDur)
        );
    }
    else
    {
        //
        //  Kemuri
        //
        switch (m_aClearAnim[0].m_nStep)
        {
        case CLEARANIM::STEP_FADEOUT:
            {
                uint32 uAnimKemuriFadeoutDur = uint32(CScreen::Framerate() * 0.25f);
                CLEARANIM* pAnimKemuri = &m_aClearAnim[0];

                pAnimKemuri->m_uAlphaBasis = uint8(Math::LinearTween(
                    0.0f,
                    255.0f,
                    float(pAnimKemuri->m_uCounter),
                    float(uAnimKemuriFadeoutDur)
                ));

                pAnimKemuri->m_fScale = Math::LinearTween(
                    64.0f,
                    32.0f,
                    float(pAnimKemuri->m_uCounter),
                    float(uAnimKemuriFadeoutDur)
                );

                if (pAnimKemuri->m_uCounter >= uAnimKemuriFadeoutDur)
                {
                    pAnimKemuri->m_nStep = CLEARANIM::STEP_FADEIN;
                    pAnimKemuri->m_uCounter = 0;
                }
                else
                {
                    ++pAnimKemuri->m_uCounter;
                };
            }
            break;

        case CLEARANIM::STEP_FADEIN:
            {
                uint32 uAnimKemuriFadeinDur = uint32(CScreen::Framerate() * 0.25f);
                CLEARANIM* pAnimKemuri = &m_aClearAnim[0];

                pAnimKemuri->m_uAlphaBasis = uint8(Math::LinearTween(
                    255.0f,
                    -255.0f,
                    float(pAnimKemuri->m_uCounter),
                    float(uAnimKemuriFadeinDur)
                ));

                pAnimKemuri->m_fScale = Math::LinearTween(
                    96.0f,
                    32.0f,
                    float(pAnimKemuri->m_uCounter),
                    float(uAnimKemuriFadeinDur)
                );

                if (pAnimKemuri->m_uCounter >= uAnimKemuriFadeinDur)
                {
                    pAnimKemuri->m_nStep = CLEARANIM::STEP_NONE;
                    pAnimKemuri->m_uCounter = 0;

                    //
                    //  Start kame anim
                    //
                    m_bKameClearRotAnimFlag = true;
                }
                else
                {
                    ++pAnimKemuri->m_uCounter;
                };
            }
            break;
        };

        //
        //  Status
        //
        switch (m_aClearAnim[1].m_nStep)
        {
        case CLEARANIM::STEP_FADEOUT:
            {
                uint32 uAnimStatusFadeoutDur = uint32(CScreen::Framerate() * 0.25f);
                CLEARANIM* pAnimStatus = &m_aClearAnim[1];

                pAnimStatus->m_uAlphaBasis = uint8(Math::LinearTween(
                    0.0f,
                    255.0f,
                    float(pAnimStatus->m_uCounter),
                    float(uAnimStatusFadeoutDur)
                ));

                pAnimStatus->m_fScale = Math::LinearTween(
                    192.0f,
                    -144.0f,
                    float(pAnimStatus->m_uCounter),
                    float(uAnimStatusFadeoutDur)
                );

                if (pAnimStatus->m_uCounter >= uAnimStatusFadeoutDur)
                {
                    pAnimStatus->m_nStep = CLEARANIM::STEP_SIZEIN;
                    pAnimStatus->m_uCounter = 0;

                    //
                    //  Start rank anim
                    //
                    m_aClearAnim[2].m_nStep = CLEARANIM::STEP_FADEOUT;
                }
                else
                {
                    ++pAnimStatus->m_uCounter;
                };
            }
            break;

        case CLEARANIM::STEP_SIZEIN:
            {
                uint32 uAnimStatusSizeinDur = uint32(CScreen::Framerate() * 0.25f);
                CLEARANIM* pAnimStatus = &m_aClearAnim[1];

                pAnimStatus->m_fScale = Math::LinearTween(
                    48.0f,
                    16.0f,
                    float(pAnimStatus->m_uCounter),
                    float(uAnimStatusSizeinDur)
                );

                if (pAnimStatus->m_uCounter >= uAnimStatusSizeinDur)
                {
                    pAnimStatus->m_uCounter = 0;
                    pAnimStatus->m_nStep = CLEARANIM::STEP_NONE;
                }
                else
                {
                    ++pAnimStatus->m_uCounter;
                };
            }
            break;
        };

        //
        //  Rank
        //
        switch (m_aClearAnim[2].m_nStep)
        {
        case CLEARANIM::STEP_FADEOUT:
            {
                uint32 uAnimRankFadeoutDur = uint32(CScreen::Framerate() * 0.25f);
                CLEARANIM* pAnimRank = &m_aClearAnim[2];

                pAnimRank->m_uAlphaBasis = uint8(Math::LinearTween(
                    0.0f,
                    255.0f,
                    float(pAnimRank->m_uCounter),
                    float(uAnimRankFadeoutDur)
                ));

                pAnimRank->m_fScale = Math::LinearTween(
                    192.0f,
                    -144.0f,
                    float(pAnimRank->m_uCounter),
                    float(uAnimRankFadeoutDur)
                );

                if (pAnimRank->m_uCounter >= uAnimRankFadeoutDur)
                {
                    pAnimRank->m_nStep = CLEARANIM::STEP_SIZEIN;
                    pAnimRank->m_uCounter = 0;

                    //
                    //  Start kemuri anim
                    //
                    m_aClearAnim[0].m_nStep = CLEARANIM::STEP_FADEOUT;
                }
                else
                {
                    ++pAnimRank->m_uCounter;
                };
            }
            break;

        case CLEARANIM::STEP_SIZEIN:
            {
                uint32 uAnimRankSizeinDur = uint32(CScreen::Framerate() * 0.25f);
                CLEARANIM* pAnimRank = &m_aClearAnim[2];

                pAnimRank->m_fScale = Math::LinearTween(
                    48.0f,
                    16.0f,
                    float(pAnimRank->m_uCounter),
                    float(uAnimRankSizeinDur)
                );

                if (pAnimRank->m_uCounter >= uAnimRankSizeinDur)
                {
                    pAnimRank->m_uCounter = 0;
                    pAnimRank->m_nStep = CLEARANIM::STEP_NONE;
                }
                else
                {
                    ++pAnimRank->m_uCounter;
                };
            }
            break;
        };
    };
};


bool CAreaWorkPool::GetLineDrawEnable(int32 nIndex)
{
    bool bResult = false;
    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        {
            switch (nIndex)
            {
            case 1:
                if (GetAreaState(AREAID::ID_AREA02))
                    bResult = true;
                break;
                
            case 2:
                if (GetAreaState(AREAID::ID_AREA04))
                    bResult = true;
                break;
                
            case 3:
                if (GetAreaState(AREAID::ID_AREA04))
                    bResult = true;
                break;
                
            case 4:
                if (GetAreaState(AREAID::ID_AREA05))
                    bResult = true;
                break;
                
            case 5:
                if (GetAreaState(AREAID::ID_AREA06))
                    bResult = true;
                break;
                
            case 6:
                if (GetAreaState(AREAID::ID_AREA10))
                    bResult = true;
                break;
                
            case 7:
                if (GetAreaState(AREAID::ID_AREA11))
                    bResult = true;
                break;
                
            case 8:
                if (IsAreaRootCleared(AREAID::ID_AREA11, CAreaRecord::CLEAR_ROOT_A))
                    bResult = true;                
                break;
                
            case 9:
                if (IsAreaRootCleared(AREAID::ID_AREA11, CAreaRecord::CLEAR_ROOT_B))
                    bResult = true;
                break;
                
            case 10:
                if (GetAreaState(AREAID::ID_AREA39))
                    bResult = true;
                break;

            case 11:
                if (GetAreaState(AREAID::ID_AREA40))
                    bResult = true;
                break;

            case 12:
                if (GetAreaState(AREAID::ID_AREA41))
                    bResult = true;
                break;

            case 13:
                if (GetAreaState(AREAID::ID_AREA41))
                    bResult = true;
                break;

            case 14:
                if (GetAreaState(AREAID::ID_AREA42))
                    bResult = true;
                break;

            case 15:
                if (GetAreaState(AREAID::ID_AREA44))
                    bResult = true;
                break;

            case 16:
                if (GetAreaState(AREAID::ID_AREA46))
                    bResult = true;
                break;

            case 17:
                if (GetAreaState(AREAID::ID_AREA47))
                    bResult = true;
                break;

            case 18:
                if (GetAreaState(AREAID::ID_AREA48))
                    bResult = true;
                break;

            case 19:
                if (GetAreaState(AREAID::ID_AREA52))
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_DHO:
        {
            switch (nIndex)
            {
            case 1:
                if (GetAreaState(AREAID::ID_AREA21))
                    bResult = true;                
                break;

            case 2:
                if (GetAreaState(AREAID::ID_AREA22))
                    bResult = true;
                break;

            case 3:
                if (GetAreaState(AREAID::ID_AREA23))
                    bResult = true;
                break;

            case 4:
                if (GetAreaState(AREAID::ID_AREA24))
                    bResult = true;
                break;

            case 5:
                if (GetAreaState(AREAID::ID_AREA25))
                    bResult = true;
                break;

            case 6:
                if (GetAreaState(AREAID::ID_AREA17))
                    bResult = true;
                break;

            case 7:
                if (GetAreaState(AREAID::ID_AREA20))
                    bResult = true;
                break;

            case 8:
                if (GetAreaState(AREAID::ID_AREA18))
                    bResult = true;
                break;

            case 9:
                if (GetAreaState(AREAID::ID_AREA17))
                    bResult = true;
                break;

            case 10:
                if (IsAreaRootCleared(AREAID::ID_AREA14, CAreaRecord::CLEAR_ROOT_B))
                    bResult = true;                
                break;

            case 11:
                if (GetAreaState(AREAID::ID_AREA14) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 12:
                if (GetAreaState(AREAID::ID_AREA14))
                    bResult = true;
                break;

            case 13:
                if (GetAreaState(AREAID::ID_AREA13))
                    bResult = true;
                break;

            case 14:
                if (GetAreaState(AREAID::ID_AREA12))
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_TRI:
        {
            switch (nIndex)
            {
            case 1:
                if (GetAreaState(AREAID::ID_AREA25))
                    bResult = true;
                break;

            case 2:
                if (GetAreaState(AREAID::ID_AREA25))
                    bResult = true;
                break;

            case 3:
                if (GetAreaState(AREAID::ID_AREA28))
                    bResult = true;
                break;

            case 4:
                if (GetAreaState(AREAID::ID_AREA29))
                    bResult = true;
                break;

            case 5:
                if (GetAreaState(AREAID::ID_AREA29) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 6:
                if (GetAreaState(AREAID::ID_AREA31))
                    bResult = true;
                break;

            case 7:
                if (GetAreaState(AREAID::ID_AREA32))
                    bResult = true;
                break;

            case 8:
                if (GetAreaState(AREAID::ID_AREA33))
                    bResult = true;
                break;

            case 9:
                if (GetAreaState(AREAID::ID_AREA27) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 10:
                if (GetAreaState(AREAID::ID_AREA27))
                    bResult = true;
                break;

            case 11:
                if (GetAreaState(AREAID::ID_AREA26))
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_JPN:
        {
            switch (nIndex)
            {
            case 1:
                if (GetAreaState(AREAID::ID_AREA33))
                    bResult = true;
                break;

            case 2:
                if (GetAreaState(AREAID::ID_AREA33))
                    bResult = true;
                break;

            case 3:
                if (GetAreaState(AREAID::ID_AREA36))
                    bResult = true;
                break;

            case 4:
                if (GetAreaState(AREAID::ID_AREA37))
                    bResult = true;
                break;

            case 5:
                if (GetAreaState(AREAID::ID_AREA37) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 6:
                if (GetAreaState(AREAID::ID_AREA39))
                    bResult = true;
                break;

            case 7:
                if (GetAreaState(AREAID::ID_AREA35) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 8:
                if (GetAreaState(AREAID::ID_AREA35))
                    bResult = true;
                break;

            case 9:
                if (GetAreaState(AREAID::ID_AREA34))
                    bResult = true;
                break;
                
            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_FNY:
        {
            switch (nIndex)
            {
            case 1:
                if (GetAreaState(AREAID::ID_AREA48))
                    bResult = true;
                break;

            case 2:
                if (GetAreaState(AREAID::ID_AREA49))
                    bResult = true;
                break;

            case 3:
                if (GetAreaState(AREAID::ID_AREA49))
                    bResult = true;
                break;

            case 4:
                if (GetAreaState(AREAID::ID_AREA50))
                    bResult = true;
                break;

            case 5:
                if (GetAreaState(AREAID::ID_AREA52))
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_KUR:
        {
            switch (nIndex)
            {
            case 1:
                if (GetAreaState(AREAID::ID_AREA53))
                    bResult = true;
                break;

            case 2:
                if (GetAreaState(AREAID::ID_AREA53))
                    bResult = true;
                break;

            case 3:
                if (GetAreaState(AREAID::ID_AREA52))
                    bResult = true;
                break;

            case 4:
                if (GetAreaState(AREAID::ID_AREA56))
                    bResult = true;
                break;

            case 5:
                if (GetAreaState(AREAID::ID_AREA57))
                    bResult = true;
                break;

            case 6:
                if (GetAreaState(AREAID::ID_AREA58))
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


bool CAreaWorkPool::GetTextDrawEnable(int32 nIndex)
{
    bool bResult = false;
    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        {
            switch (nIndex)
            {
            case 0:
                if (GetAreaState(AREAID::ID_MNY_STN))
                    bResult = true;
                break;

            case 1:
                bResult = true;
                break;

            case 2:
                if (GetAreaState(AREAID::ID_AREA47) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 3:
                if (GetAreaState(AREAID::ID_AREA39))
                    bResult = true;
                break;

            case 4:
                if (GetAreaState(AREAID::ID_AREA11) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 5:
                if (GetAreaState(AREAID::ID_AREA50) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_DHO:
        {
            switch (nIndex)
            {
            case 0:
                if (GetAreaState(AREAID::ID_AREA12) ||
                    GetAreaState(AREAID::ID_AREA15))
                    bResult = true;
                break;

            case 1:
                if (GetAreaState(AREAID::ID_AREA24) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_TRI:
        {
            switch (nIndex)
            {
            case 0:
                if (GetAreaState(AREAID::ID_AREA25))
                    bResult = true;
                break;

            case 1:
                if (GetAreaState(AREAID::ID_AREA32) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_JPN:
        {
            switch (nIndex)
            {
            case 0:
                if (GetAreaState(AREAID::ID_AREA38) == CAreaRecord::STATE_CLEAR)
                    bResult = true;
                break;

            case 1:
                if (GetAreaState(AREAID::ID_AREA33))
                    bResult = true;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case WORLDID::ID_FNY:
        if (GetAreaState(AREAID::ID_AREA48))
            bResult = true;
        break;

    case WORLDID::ID_KUR:
        if (GetAreaState(AREAID::ID_AREA52))
            bResult = true;
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


void CAreaWorkPool::GetAreaPosition(RwV2d* pPosition, AREAID::VALUE idArea)
{
    static const RwV2d s_aAreaPosTable[] =
    {
        { 0.0f,     0.0f,       },
        { -110.0f,  -126.0f,    },
        { -75.0f,   -178.0f,    },
        { -40.0f,   20.0f,      },
        { 60.0f,    87.0f,      },
        { 112.0f,   48.0f,      },
        { 175.0f,   79.0f,      },
        { 85.0f,    -76.0f,     },
        { 118.0f,   -129.0f,    },
        { 170.0f,   -148.0f,    },
        { -212.0f,  -13.0f,     },
        { -254.0f,  -68.0f,     },
        { -147.0f,  -91.0f,     },
        { -5.0f,    -65.0f,     },
        { 20.0f,    -10.0f,     },
        { -25.0f,   23.0f,      },
        { 73.0f,    23.0f,      },
        { 62.0f,    74.0f,      },
        { 147.0f,   -60.0f,     },
        { 139.0f,   -9.0f,      },
        { 152.0f,   40.0f,      },
        { 193.0f,   61.0f,      },
        { -42.0f,   -51.0f,     },
        { 32.0f,    12.0f,      },
        { 100.0f,   44.0f,      },
        { 12.0f,    -75.0f,     },
        { 77.0f,    -82.0f,     },
        { 132.0f,   -50.0f,     },
        { 195.0f,   -52.0f,     },
        { 201.0f,   19.0f,      },
        { 152.0f,   -73.0f,     },
        { 8.0f,     -79.0f,     },
        { -149.0f,  -47.0f,     },
        { 201.0f,   19.0f,      },
        { 47.0f,    81.0f,      },
        { -208.0f,  39.0f,      },
        { 195.0f,   -81.0f,     },
        { 146.0f,   -72.0f,     },
        { -126.0f,  44.0f,      },
        { -132.0f,  97.0f,      },
        { -177.0f,  -68.0f,     },
        { -238.0f,  -93.0f,     },
        { -187.0f,  -19.0f,     },
        { -184.0f,  32.0f,      },
        { -223.0f,  82.0f,      },
        { -158.0f,  75.0f,      },
        { 33.0f,    -89.0f,     },
        { -32.0f,   -152.0f,    },
        { 84.0f,    83.0f,      },
        { 82.0f,    -44.0f,     },
        { 144.0f,   -73.0f,     },
        { 201.0f,   -36.0f,     },
        { -132.0f,  -4.0f,      },
        { -179.0f,  -87.0f,     },
        { -64.0f,   -31.0f,     },
        { -37.0f,   -45.0f,     },
        { 80.0f,    -35.0f,     },
        { -120.0f,  -30.0f,     },
        { 73.0f,    -24.0f,     },
        { -216.0f,  76.0f,      },
        { 140.0f,   -20.0f,     },
        { -40.0f,   20.0f,      },
        { 67.0f,    -64.0f,     },
        { -196.0f,  60.0f,      },
        { 200.0f,   -104.0f,    },
        { -32.0f,   -4.0f,      },
        { 68.0f,    12.0f,      },
        { 167.0f,   -220.0f,    },
        { 217.0f,   -220.0f,    },
        { 230.0f,   -83.0f,     },
        { -284.0f,  100.0f,     },
        { -286.0f,  -50.0f,     },
        { -274.0f,  92.0f,      },
        { -221.0f,  99.0f,      },
        { 226.0f,   62.0f,      },
        { -284.0f,  104.0f,     },
        { 132.0f,   116.0f,     },
        { 224.0f,   -180.0f,    },
        { -296.0f,  48.0f,      },
        { -288.0f,  84.0f,      },
        { 168.0f,   120.0f,     },
        { -40.0f,   -190.0f,    },
        { -40.0f,   87.0f,      },
        { 112.0f,   82.0f,      },
        { 170.0f,   44.0f,      },
        { 80.0f,    -35.0f,     },
        { 85.0f,    -120.0f,    },
        { 202.0f,   -150.0f,    },
        { 140.0f,   -35.0f,     },
        { -70.0f,   5.0f,       },
        { -67.0f,   -37.0f,     },
        { -120.0f,  -73.0f,     },
        { -233.0f,  -63.0f,     },
        { -180.0f,  70.0f,      },
        { -252.0f,  -14.0f,     },
        { -222.0f,  31.0f,      },
        { -251.0f,  -14.0f,     },
        { -202.0f,  -74.0f,     },
        { -13.0f,   -35.0f,     },
        { 7.0f,     23.0f,      },
        { 149.0f,   67.0f,      },
        { -212.0f,  80.0f,      },
        { -148.0f,  -12.0f,     },
        { -8.0f,    16.0f,      },
        { 44.0f,    52.0f,      },
        { 124.0f,   -80.0f,     },
        { 196.0f,   -76.0f,     },
        { -196.0f,  -32.0f,     },
        { 156.0f,   -36.0f,     },
        { 192.0f,   -12.0f,     },
        { 188.0f,   64.0f,      },
        { -80.0f,   76.0f,      },
        { -156.0f,  44.0f,      },
        { -120.0f,  72.0f,      },
        { 28.0f,    -8.0f,      },
        { 16.0f,    -132.0f,    },
        { -64.0f,   -136.0f,    },
        { 64.0f,    52.0f,      },
        { 120.0f,   12.0f,      },
        { -172.0f,  -8.0f,      },
        { -104.0f,  -80.0f,     },
        { 0.0f,     0.0f,       },
        { 0.0f,     0.0f,       },
        { 0.0f,     0.0f,       },
        { 0.0f,     0.0f,       },
    };

    static_assert(COUNT_OF(s_aAreaPosTable) == AREAID::NEXUSMAX, "update me");

    ASSERT(idArea >= 0 && idArea < COUNT_OF(s_aAreaPosTable));
    ASSERT(pPosition);
    
    *pPosition = s_aAreaPosTable[idArea];
};


AREAID::VALUE CAreaWorkPool::GetMoveToArea(AREAID::VALUE idArea, int32 nDirection)
{
    static const AREAID::VALUE s_aAreaMoveTable[][DIRECTIONUM] =
    {
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_AREA02,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA01,  AREAID::ID_MNY_C01,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_C02, AREAID::ID_MNY_C03,     },
        { AREAID::ID_NONE,      AREAID::ID_MNY_C03, AREAID::ID_NONE,    AREAID::ID_MNY_C04,     },
        { AREAID::ID_MNY_C04,   AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_MNY_C06,   AREAID::ID_MNY_J02, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_C06, AREAID::ID_AREA11,      },
        { AREAID::ID_MNY_E01,   AREAID::ID_NONE,    AREAID::ID_AREA10,  AREAID::ID_MNY_C07,     },
        { AREAID::ID_NONE,      AREAID::ID_DHO_C01, AREAID::ID_DHO_C02, AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_DHO_C02, AREAID::ID_NONE,    AREAID::ID_DHO_C03,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_DHO_C03, AREAID::ID_AREA15,      },
        { AREAID::ID_NONE,      AREAID::ID_DHO_C04, AREAID::ID_AREA14,  AREAID::ID_DHO_STN,     },
        { AREAID::ID_NONE,      AREAID::ID_DHO_C05, AREAID::ID_NONE,    AREAID::ID_DHO_J01,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_DHO_C05,     },
        { AREAID::ID_DHO_J01,   AREAID::ID_AREA20,  AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_AREA19,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA22,  AREAID::ID_DHO_STN, AREAID::ID_NONE,        },
        { AREAID::ID_AREA21,    AREAID::ID_AREA23,  AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_AREA22,    AREAID::ID_DHO_C06, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_DHO_C06, AREAID::ID_DHO_E03,     },
        { AREAID::ID_NONE,      AREAID::ID_TRI_C03, AREAID::ID_TRI_C02, AREAID::ID_AREA28,      },
        { AREAID::ID_NONE,      AREAID::ID_TRI_C04, AREAID::ID_TRI_C03, AREAID::ID_NONE,        },
        { AREAID::ID_AREA30,    AREAID::ID_NONE,    AREAID::ID_TRI_C04, AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA25,  AREAID::ID_AREA29,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA28,  AREAID::ID_TRI_C05,     },
        { AREAID::ID_TRI_C05,   AREAID::ID_AREA27,  AREAID::ID_NONE,    AREAID::ID_AREA31,      },
        { AREAID::ID_NONE,      AREAID::ID_AREA32,  AREAID::ID_AREA30,  AREAID::ID_NONE,        },
        { AREAID::ID_AREA31,    AREAID::ID_TRI_E02, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_JPN_C03, AREAID::ID_AREA34,  AREAID::ID_JPN_C01,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA35,  AREAID::ID_AREA33,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_JPN_C02, AREAID::ID_AREA34,      },
        { AREAID::ID_JPN_C04,   AREAID::ID_JPN_C05, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_JPN_C06, AREAID::ID_JPN_C05,     },
        { AREAID::ID_JPN_C02,   AREAID::ID_NONE,    AREAID::ID_JPN_E02, AREAID::ID_JPN_C07,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA40,  AREAID::ID_MNY_E03,     },
        { AREAID::ID_NONE,      AREAID::ID_MNY_C08, AREAID::ID_NONE,    AREAID::ID_AREA39,      },
        { AREAID::ID_MNY_J03,   AREAID::ID_AREA42,  AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_AREA41,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_C12, AREAID::ID_MNY_C11,     },
        { AREAID::ID_NONE,      AREAID::ID_MNY_C12, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA46,  AREAID::ID_MNY_C14, AREAID::ID_MNY_J03,     },
        { AREAID::ID_AREA45,    AREAID::ID_MNY_C13, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_E04, AREAID::ID_MNY_C13,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_FNY_J01, AREAID::ID_FNY_C01,     },
        { AREAID::ID_FNY_C03,   AREAID::ID_FNY_C02, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_FNY_C04, AREAID::ID_FNY_C03,     },
        { AREAID::ID_KUR_C01,   AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_KUR_E01,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_KUR_J01,     },
        { AREAID::ID_NONE,      AREAID::ID_KUR_J01, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_KUR_J01, AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_KUR_C03, AREAID::ID_KUR_STN,     },
        { AREAID::ID_NONE,      AREAID::ID_KUR_C03, AREAID::ID_NONE,    AREAID::ID_KUR_C04,     },
        { AREAID::ID_KUR_C04,   AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_MNY_C01,   AREAID::ID_MNY_STN, AREAID::ID_NONE,    AREAID::ID_MNY_J02,     },
        { AREAID::ID_AREA09,    AREAID::ID_NONE,    AREAID::ID_MNY_J01, AREAID::ID_MNY_C08,     },
        { AREAID::ID_MNY_C11,   AREAID::ID_AREA41,  AREAID::ID_AREA45,  AREAID::ID_MNY_C10,     },
        { AREAID::ID_DHO_STN,   AREAID::ID_AREA19,  AREAID::ID_AREA17,  AREAID::ID_NONE,        },
        { AREAID::ID_FNY_C04,   AREAID::ID_NONE,    AREAID::ID_FNY_E01, AREAID::ID_AREA48,      },
        { AREAID::ID_AREA54,    AREAID::ID_KUR_C02, AREAID::ID_AREA53,  AREAID::ID_AREA55,      },
        { AREAID::ID_MNY_J01,   AREAID::ID_MNY_C02, AREAID::ID_MNY_C09, AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_DHO_J01, AREAID::ID_AREA15,  AREAID::ID_AREA21,      },
        { AREAID::ID_TRI_C02,   AREAID::ID_NONE,    AREAID::ID_TRI_C01, AREAID::ID_NONE,        },
        { AREAID::ID_JPN_E01,   AREAID::ID_JPN_C01, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_FNY_C01, AREAID::ID_FNY_C02,     },
        { AREAID::ID_NONE,      AREAID::ID_KUR_C01, AREAID::ID_AREA56,  AREAID::ID_KUR_C02,     },
        { AREAID::ID_NONE,      AREAID::ID_AREA11,  AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_MNY_C07, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA39,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA47,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_MNY_C14,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_DHO_C01,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_DHO_C04,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA24,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_TRI_C01,     },
        { AREAID::ID_AREA32,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_JPN_STN, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA38,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_FNY_J01,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA52,  AREAID::ID_NONE,        },
        { AREAID::ID_AREA02,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_MNY_J01,     },
        { AREAID::ID_NONE,      AREAID::ID_AREA04,  AREAID::ID_MNY_STN, AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA04,  AREAID::ID_NONE,    AREAID::ID_AREA05,      },
        { AREAID::ID_AREA05,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA06,      },
        { AREAID::ID_NONE,      AREAID::ID_MNY_J01, AREAID::ID_NONE,    AREAID::ID_AREA09,      },
        { AREAID::ID_AREA10,    AREAID::ID_NONE,    AREAID::ID_AREA09,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA11,  AREAID::ID_NONE,    AREAID::ID_MNY_E02,     },
        { AREAID::ID_NONE,      AREAID::ID_MNY_J02, AREAID::ID_NONE,    AREAID::ID_AREA40,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_C10, AREAID::ID_MNY_STN,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_J03, AREAID::ID_MNY_C09,     },
        { AREAID::ID_AREA43,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_MNY_J03,     },
        { AREAID::ID_NONE,      AREAID::ID_AREA43,  AREAID::ID_AREA44,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA47,  AREAID::ID_NONE,    AREAID::ID_AREA46,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_MNY_E05, AREAID::ID_AREA45,      },
        { AREAID::ID_NONE,      AREAID::ID_DHO_E01, AREAID::ID_NONE,    AREAID::ID_AREA12,      },
        { AREAID::ID_NONE,      AREAID::ID_AREA12,  AREAID::ID_AREA13,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA13,  AREAID::ID_AREA14,      },
        { AREAID::ID_NONE,      AREAID::ID_DHO_E02, AREAID::ID_NONE,    AREAID::ID_AREA15,      },
        { AREAID::ID_NONE,      AREAID::ID_AREA18,  AREAID::ID_NONE,    AREAID::ID_AREA17,      },
        { AREAID::ID_NONE,      AREAID::ID_AREA24,  AREAID::ID_AREA23,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_TRI_E01, AREAID::ID_TRI_STN,     },
        { AREAID::ID_AREA25,    AREAID::ID_NONE,    AREAID::ID_TRI_STN, AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA26,  AREAID::ID_AREA25,  AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA27,  AREAID::ID_AREA26,  AREAID::ID_NONE,        },
        { AREAID::ID_AREA29,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA30,      },
        { AREAID::ID_NONE,      AREAID::ID_AREA33,  AREAID::ID_NONE,    AREAID::ID_JPN_STN,     },
        { AREAID::ID_AREA35,    AREAID::ID_NONE,    AREAID::ID_AREA38,  AREAID::ID_NONE,        },
        { AREAID::ID_AREA33,    AREAID::ID_JPN_C04, AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_JPN_C03,   AREAID::ID_AREA36,  AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_AREA37,  AREAID::ID_NONE,    AREAID::ID_AREA36,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_JPN_C07, AREAID::ID_AREA37,      },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA38,  AREAID::ID_JPN_C06,     },
        { AREAID::ID_NONE,      AREAID::ID_NONE,    AREAID::ID_AREA48,  AREAID::ID_FNY_STN,     },
        { AREAID::ID_NONE,      AREAID::ID_FNY_STN, AREAID::ID_NONE,    AREAID::ID_AREA49,      },
        { AREAID::ID_AREA50,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA49,      },
        { AREAID::ID_AREA50,    AREAID::ID_NONE,    AREAID::ID_FNY_J01, AREAID::ID_NONE,        },
        { AREAID::ID_KUR_STN,   AREAID::ID_AREA52,  AREAID::ID_NONE,    AREAID::ID_NONE,        },
        { AREAID::ID_NONE,      AREAID::ID_KUR_STN, AREAID::ID_NONE,    AREAID::ID_KUR_J01,     },
        { AREAID::ID_NONE,      AREAID::ID_AREA56,  AREAID::ID_AREA57,  AREAID::ID_NONE,        },
        { AREAID::ID_AREA57,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA58, },
    };

    ASSERT(idArea >= 0 && idArea < COUNT_OF(s_aAreaMoveTable));
    ASSERT(nDirection >= 0 && nDirection < COUNT_OF(s_aAreaMoveTable[0]));

    AREAID::VALUE idAreaToMove = s_aAreaMoveTable[idArea][nDirection];

    if (idArea == AREAID::ID_AREA48 && nDirection == DIRECTION_LEFT)
    {
        if (GetAreaState(AREAID::ID_AREA50) != CAreaRecord::STATE_CLEAR)
            idAreaToMove = AREAID::ID_FNY_E01;
    };
    
    if (idArea == AREAID::ID_FNY_E01 && nDirection == DIRECTION_RIGHT)
    {
        if (GetAreaState(AREAID::ID_AREA50) != CAreaRecord::STATE_CLEAR)
            idAreaToMove = AREAID::ID_AREA48;
    };
    
    if (idArea == AREAID::ID_MNY_J01 && nDirection == DIRECTION_RIGHT)
    {
        if (GetAreaState(AREAID::ID_AREA40) != CAreaRecord::STATE_CLEAR)
            idAreaToMove = AREAID::ID_MNY_C05;
    };
    
    if (idArea == AREAID::ID_AREA09 && nDirection == DIRECTION_DOWN)
    {
        if (GetAreaState(AREAID::ID_AREA40) != CAreaRecord::STATE_CLEAR)
            idAreaToMove = AREAID::ID_MNY_C05;
    };

    if (GetAreaState(AREAID::ID_AREA40) != CAreaRecord::STATE_CLEAR &&
        idArea == AREAID::ID_AREA40 &&
        nDirection == DIRECTION_DOWN)
    {
        idAreaToMove = AREAID::ID_NONE;
    };

    if (GetAreaState(AREAID::ID_AREA14) != CAreaRecord::STATE_CLEAR)
    {
        if (idArea == AREAID::ID_AREA14 && nDirection == DIRECTION_RIGHT)
        {
            idAreaToMove = AREAID::ID_NONE;
        }
        else if (idArea == AREAID::ID_AREA15 && nDirection == DIRECTION_LEFT)
        {
            idAreaToMove = AREAID::ID_NONE;
        };
    };

    if (GetAreaState(AREAID::ID_AREA29) != CAreaRecord::STATE_CLEAR)
    {
        if (idArea == AREAID::ID_AREA29 && nDirection == DIRECTION_RIGHT)
        {
            idAreaToMove = AREAID::ID_NONE;
        }
        else if (idArea == AREAID::ID_AREA30 && !nDirection)
        {
            idAreaToMove = AREAID::ID_NONE;
        }
    };

    if (GetAreaState(AREAID::ID_AREA27) != CAreaRecord::STATE_CLEAR)
    {
        if (idArea != AREAID::ID_AREA27 || nDirection)
        {
            if (idArea == AREAID::ID_AREA30 && nDirection == DIRECTION_DOWN)
                idAreaToMove = AREAID::ID_NONE;
        }
        else
        {
            idAreaToMove = AREAID::ID_NONE;
        };
    };

    if (GetAreaState(AREAID::ID_AREA35) != CAreaRecord::STATE_CLEAR)
    {
        if (idArea == AREAID::ID_AREA35 && nDirection == DIRECTION_LEFT)
        {
            idAreaToMove = AREAID::ID_NONE;
        }
        else if (idArea == AREAID::ID_AREA38 && !nDirection)
        {
            idAreaToMove = AREAID::ID_NONE;
        };
    };

    if (GetAreaState(AREAID::ID_AREA37) != CAreaRecord::STATE_CLEAR)
    {
        if (idArea == AREAID::ID_AREA37 && nDirection == DIRECTION_LEFT)
        {
            idAreaToMove = AREAID::ID_NONE;
        }
        else if (idArea == AREAID::ID_AREA38 && nDirection == DIRECTION_RIGHT)
        {
            idAreaToMove = AREAID::ID_NONE;
        };
    };

    if (GetAreaState(AREAID::ID_AREA50) != CAreaRecord::STATE_CLEAR &&
        idArea == AREAID::ID_AREA50 &&
        nDirection == DIRECTION_LEFT)
    {
        idAreaToMove = AREAID::ID_NONE;
    };
    
    return idAreaToMove;
};


bool CAreaWorkPool::CheckAreaMove(AREAID::VALUE idArea, int32 nDirection)
{
    AREAID::VALUE idAreaToMove = GetMoveToArea(idArea, nDirection);
    if (idAreaToMove != m_NowArea)
    {
        while (idAreaToMove)
        {
            if (idAreaToMove < AREAID::NORMALMAX)
                return GetAreaState(idAreaToMove) != CAreaRecord::STATE_NONE;
            
            if (idAreaToMove <= AREAID::ID_KUR_J01)
            {
                for (int32 i = 0; i < DIRECTIONUM; i++)
                {
                    if (CheckAreaMove(idAreaToMove, i))
                        return true;
                };

                return false;
            };
            
            if (idAreaToMove <= AREAID::ID_KUR_E01)
                return GetAreaState(idAreaToMove) != CAreaRecord::STATE_NONE;
            
            idAreaToMove = GetMoveToArea(idAreaToMove, nDirection);
            if (idAreaToMove == m_NowArea)
                break;
        };
    };

    return false;
};


AREATYPES::NEXTSEQUENCE CAreaWorkPool::GetNextSequence(void) const
{
    return m_NextSequence;
};


CAreaRecord::STATE CAreaWorkPool::GetAreaState(AREAID::VALUE idArea) const
{
    return CGameData::Record().Area().GetAreaState(idArea);
};


void CAreaWorkPool::SetCurrentSelectedArea(AREAID::VALUE idArea)
{
    CGameData::Record().Area().SetCurrentSelectedArea(idArea);
};


bool CAreaWorkPool::IsAreaRootCleared(AREAID::VALUE idArea, CAreaRecord::CLEAR_ROOT root)
{
    return CGameData::Record().Area().IsAreaRootCleared(idArea, root);
};


void CAreaWorkPool::DebugStartPlayClearAnim(void)
{
    //
    //  For debug only
    //  Start clear anim at station of the current world
    //

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        m_ClearedArea = AREAID::ID_MNY_STN;
        break;

    case WORLDID::ID_DHO:
        m_ClearedArea = AREAID::ID_DHO_STN;
        break;

    case WORLDID::ID_TRI:
        m_ClearedArea = AREAID::ID_TRI_STN;
        break;

    case WORLDID::ID_JPN:
        m_ClearedArea = AREAID::ID_JPN_STN;
        break;

    case WORLDID::ID_FNY:
        m_ClearedArea = AREAID::ID_FNY_STN;
        break;

    case WORLDID::ID_KUR:
        m_ClearedArea = AREAID::ID_KUR_STN;
        break;

    default:
        ASSERT(false);
        break;
    };

    AreaClearedAnimationInit();
};


void CAreaWorkPool::DebugAreaChanged(void)
{
    OUTPUT("Now area %d (name: %s)\n", m_NewArea, CMapInfo::GetName(CStageInfo::GetMapID(CAreaInfo::GetStageID(m_NowArea, 0))));
};


/*static*/ bool CAreaSequence::m_bDebugClearAnim = false;


/*static*/ CProcess* CAreaSequence::Instance(void)
{
    return new CAreaSequence;
};


CAreaSequence::CAreaSequence(void)
: m_pWorkPool(nullptr)
, m_pDlgSure(nullptr)
, m_bDlgRunning(false)
{
    ;
};


CAreaSequence::~CAreaSequence(void)
{
    ;
};


bool CAreaSequence::OnAttach(const void* param)
{
    m_pWorkPool = new CAreaWorkPool;
    ASSERT(m_pWorkPool);
    m_pWorkPool->Attach();

    m_bDlgRunning = false;
    m_pDlgSure = new CDialog(CDialog::COLOR_NORMAL, CDialog::STATUS_NO, CGameData::Attribute().GetVirtualPad());
    ASSERT(m_pDlgSure);
    m_pDlgSure->Set(0.0f, 0.0f, CSprite::m_fVirtualScreenW, 140.0f);
    m_pDlgSure->SetOpenAction(true);
    m_pDlgSure->SetText(
        CGameText::GetText(GAMETEXT::VALUE(758)),
        CGameFont::GetScreenSize() / 223,
        { 0xFF, 0xFF, 0xFF, 0xFF }
    );

    CAreaMenu::Initialize();

    return AreaSelectLoad();
};


void CAreaSequence::OnDetach(void)
{
    CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);

    CAreaMenu::Terminate();

    if (m_pWorkPool)
    {    
        delete m_pWorkPool;
        m_pWorkPool = nullptr;
    };

    if (m_pDlgSure)
    {
        delete m_pDlgSure;
        m_pDlgSure = nullptr;
    };

    CAnim2DSequence::OnDetach();
};


void CAreaSequence::OnMove(bool bRet, const void* param)
{
    switch (m_step)
    {
    case STEP_FADE_OUT:
        m_pWorkPool->TextureSetting();
        m_pWorkPool->AreaMenuSetting();

        if (!CScreenFade::IsFading())
        {
            AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

            switch (CAreaInfo::GetWorldNo(idArea))
            {
            case WORLDID::ID_MNY:
                CGameSound::PlayBGM(SDCODE_BGM(12327));
                break;

            case WORLDID::ID_DHO:
            case WORLDID::ID_TRI:
                CGameSound::PlayBGM(SDCODE_BGM(12328));
                break;

            case WORLDID::ID_JPN:
                CGameSound::PlayBGM(SDCODE_BGM(12329));
                break;

            case WORLDID::ID_FNY:
                CGameSound::PlayBGM(SDCODE_BGM(12330));
                break;

            case WORLDID::ID_KUR:
                CGameSound::PlayBGM(SDCODE_BGM(12331));
                break;

            default:
                ASSERT(false);
                break;
            };
        };
        break;

    case STEP_DRAW:
        m_pWorkPool->TextureSetting();
        m_pWorkPool->AreaMenuSetting();

        if (m_bDlgRunning)
        {
            if (!m_pDlgSure->IsOpen())
            {
                m_bDlgRunning = false;

                switch (m_pDlgSure->GetStatus())
                {
                case CDialog::STATUS_YES:
                    Ret();
                    break;
                };
            };
        }
        else if (m_pWorkPool->Move())
        {
            if (m_pWorkPool->GetNextSequence() == AREATYPES::NEXTSEQUENCE_TITLE)
            {
                m_bDlgRunning = true;
                m_pDlgSure->Open();
            }
            else
            {
                Ret();
            };
        };
        break;
    };

    CAnim2DSequence::OnMove(bRet, param);
};


void CAreaSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();

    if (IsDrawing())
    {
        m_pWorkPool->TextureDraw();
        m_pWorkPool->AreaMenuDraw();
    };
};


bool CAreaSequence::OnRet(void)
{
    m_pWorkPool->UpdateAreaWarp();

    switch (m_pWorkPool->GetNextSequence())
    {
    case AREATYPES::NEXTSEQUENCE_AREA:
    case AREATYPES::NEXTSEQUENCE_WARP:
        Ret();
        break;
        
    case AREATYPES::NEXTSEQUENCE_CHARASEL:
        Ret((const void*)PROCESSTYPES::LABEL_SEQ_CHARSELECT);
        break;
        
    case AREATYPES::NEXTSEQUENCE_MENUSAVE:
        Ret((const void*)PROCESSTYPES::LABEL_SEQ_SAVELOADMENUSAVE);
        break;
        
    case AREATYPES::NEXTSEQUENCE_MENULOAD:
        Ret((const void*)PROCESSTYPES::LABEL_SEQ_SAVELOADMENULOAD);
        break;
        
    case AREATYPES::NEXTSEQUENCE_OPTIONS:
        Ret((const void*)PROCESSTYPES::LABEL_SEQ_OPTION);
        break;
        
    case AREATYPES::NEXTSEQUENCE_TITLE:
        Ret((const void*)PROCESSTYPES::LABEL_SEQ_TITLE);
        break;

    default:
		return false;
    };
    
	return true;
};


bool CAreaSequence::AreaSelectLoad(void)
{
    bool bResult = false;
    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        SetAnimationName("area_ny");
        bResult = CAnim2DSequence::OnAttach(FILEID::ID_AREA_NY);
        break;

    case WORLDID::ID_DHO:
        SetAnimationName("area_dho");
        bResult = CAnim2DSequence::OnAttach(FILEID::ID_AREA_DHO);
        break;

    case WORLDID::ID_TRI:
        SetAnimationName("area_tri");
        bResult = CAnim2DSequence::OnAttach(FILEID::ID_AREA_TRI);
        break;

    case WORLDID::ID_JPN:
        SetAnimationName("area_jpn");
        bResult = CAnim2DSequence::OnAttach(FILEID::ID_AREA_JPN);
        break;

    case WORLDID::ID_FNY:
        SetAnimationName("area_fny");
        bResult = CAnim2DSequence::OnAttach(FILEID::ID_AREA_FNY);
        break;

    case WORLDID::ID_KUR:
        SetAnimationName("area_kur");
        bResult = CAnim2DSequence::OnAttach(FILEID::ID_AREA_KUR);
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};