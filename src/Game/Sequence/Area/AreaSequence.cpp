#include "AreaSequence.hpp"
#include "AreaMenu.hpp"
#include "AreaTypes.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/MapInfo.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/TextData.hpp"


template<class T>
static inline float
LinearTween(float startValue, float changeValue, T time, T duration)
{
    return (startValue + ((static_cast<float>(time) / static_cast<float>(duration)) * changeValue));
};


class CAreaWorkPool
{
public:
    enum
    {
        DIRECTION_UP = 0,
        DIRECTION_DOWN,
        DIRECTION_LEFT,
        DIRECTION_RIGHT,

        DIRECTIONNUM,
    };

    enum ANIMATION
    {
        ANIMATION_NONE = 0,
        ANIMATION_KAMEMOVE,
        ANIMATION_CLEARED,
    };

    struct CLEARANIM
    {
        int32   step;
        uint32  counter;
        uint8   alphaBasis;
        float   scale;
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
    void OnAreaChanged(void);
    void DebugStartPlayClearAnim(void);
    
private:
    AREATYPES::NEXTSEQUENCE m_nextSequence;
    RwV2d         m_vKamePos;
    AREAID::VALUE m_areaOld;
    AREAID::VALUE m_areaNew;
    AREAID::VALUE m_areaNow;
    AREAID::VALUE m_areaCleared;
    int32         m_direction;
    bool          m_bMenuOpenFlag;
    bool          m_bStationWarpFlag;
    bool          m_bAreaDiskFlag;
    bool          m_bTextureSettingFlag;
    RwTexture*    m_apAreaSelTexture[17];
    RwTexture*    m_apAreaLineTexture[19];
    RwTexture*    m_apAreaTextTexture[6];
    CSprite       m_sprite;
    CLEARANIM     m_aClearAnim[4];
    float         m_fClearAnimRot;
    ANIMATION     m_animation;
    uint32        m_kameMoveAnimCount;
    uint32        m_kameIconAnimCount;
    bool          m_bKameClearRotAnimFlag;
    bool          m_bKameIconAnimFlag;
    bool          m_bMenuInfoDispFlag;
};


CAreaWorkPool::CAreaWorkPool(void)
: m_nextSequence(AREATYPES::NEXTSEQUENCE_NONE)
, m_vKamePos(Math::VECTOR2_ZERO)
, m_areaOld(AREAID::ID_NONE)
, m_areaNew(AREAID::ID_NONE)
, m_areaNow(AREAID::ID_NONE)
, m_areaCleared(AREAID::ID_NONE)
, m_direction(DIRECTION_UP)
, m_bMenuOpenFlag(false)
, m_bStationWarpFlag(false)
, m_bAreaDiskFlag(false)
, m_bTextureSettingFlag(false)
, m_apAreaSelTexture()
, m_apAreaLineTexture()
, m_apAreaTextTexture()
, m_sprite()
, m_aClearAnim()
, m_fClearAnimRot(0.0f)
, m_animation(ANIMATION_NONE)
, m_kameMoveAnimCount(0)
, m_kameIconAnimCount(0)
, m_bKameClearRotAnimFlag(false)
, m_bKameIconAnimFlag(false)
, m_bMenuInfoDispFlag(false)
{
    ;
};


CAreaWorkPool::~CAreaWorkPool(void)
{
    ;
};


void CAreaWorkPool::Attach(void)
{
    m_animation = ANIMATION_NONE;

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    if (idArea == AREAID::ID_AREA03)
        idArea = AREAID::ID_MNY_STN;

    m_areaOld = idArea;
    m_areaNew = idArea;
    m_areaNow = idArea;
    m_areaCleared = CGameData::Record().Area().GetNowClearArea();

    if (m_areaCleared)
        AreaClearedAnimationInit();

    std::memset(m_apAreaSelTexture, 0x00, sizeof(m_apAreaSelTexture));
    std::memset(m_apAreaLineTexture, 0x00, sizeof(m_apAreaLineTexture));
    std::memset(m_apAreaTextTexture, 0x00, sizeof(m_apAreaTextTexture));
    
    m_bTextureSettingFlag = false;
    m_bKameIconAnimFlag = false;
    m_kameIconAnimCount = 0;
    m_kameMoveAnimCount = 0;
    m_bMenuOpenFlag = false;
    m_bStationWarpFlag = false;

    GetAreaPosition(&m_vKamePos, m_areaNew);

    m_bMenuInfoDispFlag = false;
    
    m_bAreaDiskFlag = false;

#ifdef _DEBUG        
    OnAreaChanged();
#endif /* _DEBUG */
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
        m_nextSequence = CAreaMenu::AreaMenuSelect();
        switch (m_nextSequence)
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

        default:
            break;
        };

        return false;
    };

    switch (m_animation)
    {
    case ANIMATION_KAMEMOVE:
        AreaMoveAnimation();
        return false;

    case ANIMATION_CLEARED:
        AreaClearedAnimation();
        return false;

    default:
        break;
    };

    if (m_areaNow >= AREAID::SELECTABLEMAX)
    {
        AreaMoveAnimationInit(m_direction);
        return false;
    };

    m_bMenuInfoDispFlag = true;
    int32 iController = CGameData::Attribute().GetVirtualPad();

    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP))
    {
        AreaMoveAnimationInit(DIRECTION_UP);
        m_direction = DIRECTION_UP;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
    {
        AreaMoveAnimationInit(DIRECTION_DOWN);
        m_direction = DIRECTION_DOWN;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LLEFT))
    {
        AreaMoveAnimationInit(DIRECTION_LEFT);
        m_direction = DIRECTION_LEFT;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LRIGHT))
    {
        AreaMoveAnimationInit(DIRECTION_RIGHT);
        m_direction = DIRECTION_RIGHT;
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
    {
        if (m_areaNow >= AREAID::NORMALMAX)
        {
            if ((m_areaNow >= AREAID::WARPSTART) &&
                (m_areaNow < AREAID::WARPMAX))
            {
                CGameSound::PlaySE(SDCODE_SE(4112));
                m_nextSequence = AREATYPES::NEXTSEQUENCE_AREA;
                return true;
            };

            if (m_areaNow != AREAID::ID_MNY_STN)
                return false;

            CGameSound::PlaySE(SDCODE_SE(4098));
            m_nextSequence = AREATYPES::NEXTSEQUENCE_AREA;
            return true;
        }
        else
        {
            CGameSound::PlaySE(SDCODE_SE(8196));
            m_nextSequence = AREATYPES::NEXTSEQUENCE_AREA;
            return true;
        };
    }
    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_R1))
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

    switch (m_areaNow)
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
        if ((m_areaNow < AREAID::ID_MNY_E01) ||
            (m_areaNow > AREAID::ID_KUR_E01))
        {
            if (m_areaNow < AREAID::NORMALMAX)
            {
                CGameData::PlayParam().ClearArea();
                CGameData::PlayParam().SetStartArea(m_areaNow, 0);
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

    int32 numPath = 0;

    const char* pszTxdName = nullptr;
    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();
    WORLDID::VALUE idWorld = CAreaInfo::GetWorldNo(idArea);
    switch (idWorld)
    {
    case WORLDID::ID_MNY:
        pszTxdName  = "area_ny";
        numPath     = 19;
        break;

    case WORLDID::ID_DHO:
        pszTxdName  = "area_dho";
        numPath     = 14;
        break;

    case WORLDID::ID_TRI:
        pszTxdName  = "area_tri";
        numPath     = 11;
        break;

    case WORLDID::ID_JPN:
        pszTxdName  = "area_jpn";
        numPath     = 9;
        break;

    case WORLDID::ID_FNY:
        pszTxdName  = "area_fny";
        numPath     = 5;
        break;

    case WORLDID::ID_KUR:
        pszTxdName  = "area_kur";
        numPath     = 6;
        break;

    default:
        ASSERT(false);
        break;
    };

#ifdef TMNT2_BUILD_EU
    CTextureManager::SetCurrentTextureSet("area");
#else /* TMNT2_BUILD_EU */
    CTextureManager::SetCurrentTextureSet(pszTxdName);
#endif /* TMNT2_BUILD_EU */

    for (int32 i = 0; i < numPath; i++)
    {
        static char s_szLineTexName[256];
        s_szLineTexName[0] = '\0';

        std::sprintf(s_szLineTexName, "ar_sen%03d", i + 1);
        
        m_apAreaLineTexture[i] = CTextureManager::GetRwTexture(s_szLineTexName);
        ASSERT(m_apAreaLineTexture[i]);
    };

    m_apAreaSelTexture[0] = CTextureManager::GetRwTexture("ar_aricon0");
    m_apAreaSelTexture[1] = CTextureManager::GetRwTexture("ar_aricon1");
    m_apAreaSelTexture[2] = CTextureManager::GetRwTexture("ar_aricon2");
    m_apAreaSelTexture[3] = CTextureManager::GetRwTexture("ar_station_icon1");
    m_apAreaSelTexture[4] = CTextureManager::GetRwTexture("ar_kame_icon");
    m_apAreaSelTexture[5] = CTextureManager::GetRwTexture("ar_kame_up");
    m_apAreaSelTexture[6] = CTextureManager::GetRwTexture("ar_kame_down");
    m_apAreaSelTexture[7] = CTextureManager::GetRwTexture("ar_kame_left");
    m_apAreaSelTexture[8] = CTextureManager::GetRwTexture("ar_kame_right");
    m_apAreaSelTexture[9] = CTextureManager::GetRwTexture("ar_kemuri");
    m_apAreaSelTexture[10] = CTextureManager::GetRwTexture("ar_iconpf_e");
    m_apAreaSelTexture[11] = CTextureManager::GetRwTexture("ar_iconpf_d");
    m_apAreaSelTexture[12] = CTextureManager::GetRwTexture("ar_iconpf_c");
    m_apAreaSelTexture[13] = CTextureManager::GetRwTexture("ar_iconpf_b");
    m_apAreaSelTexture[14] = CTextureManager::GetRwTexture("ar_iconpf_a");
    m_apAreaSelTexture[15] = CTextureManager::GetRwTexture("ar_iconpf_s");
    m_apAreaSelTexture[16] = CTextureManager::GetRwTexture("ar_iconpf_ss");

    ASSERT(m_apAreaSelTexture[0]);
    ASSERT(m_apAreaSelTexture[1]);
    ASSERT(m_apAreaSelTexture[2]);
    ASSERT(m_apAreaSelTexture[3]);
    ASSERT(m_apAreaSelTexture[4]);
    ASSERT(m_apAreaSelTexture[5]);
    ASSERT(m_apAreaSelTexture[5]);
    ASSERT(m_apAreaSelTexture[6]);
    ASSERT(m_apAreaSelTexture[7]);
    ASSERT(m_apAreaSelTexture[8]);
    ASSERT(m_apAreaSelTexture[9]);
    ASSERT(m_apAreaSelTexture[10]);
    ASSERT(m_apAreaSelTexture[11]);
    ASSERT(m_apAreaSelTexture[12]);
    ASSERT(m_apAreaSelTexture[13]);
    ASSERT(m_apAreaSelTexture[14]);
    ASSERT(m_apAreaSelTexture[15]);
    ASSERT(m_apAreaSelTexture[16]);

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        m_apAreaTextTexture[0] = CTextureManager::GetRwTexture("ar_ny_text_homepsd");
        m_apAreaTextTexture[1] = CTextureManager::GetRwTexture("ar_ny_text_start");
        m_apAreaTextTexture[2] = CTextureManager::GetRwTexture("ar_ny_text_tohatena");    // fny
        m_apAreaTextTexture[3] = CTextureManager::GetRwTexture("ar_ny_text_tojapan");
        m_apAreaTextTexture[4] = CTextureManager::GetRwTexture("ar_ny_text_tospace");
        m_apAreaTextTexture[5] = CTextureManager::GetRwTexture("ar_ny_text_tokuraiyama");
        ASSERT(m_apAreaTextTexture[0]);
        ASSERT(m_apAreaTextTexture[1]);
        ASSERT(m_apAreaTextTexture[2]);
        ASSERT(m_apAreaTextTexture[3]);
        ASSERT(m_apAreaTextTexture[4]);
        ASSERT(m_apAreaTextTexture[5]);
        break;

    case WORLDID::ID_DHO:
        m_apAreaTextTexture[0] = CTextureManager::GetRwTexture("ar_dh_text_tony");
        m_apAreaTextTexture[1] = CTextureManager::GetRwTexture("ar_dh_text_toriceraton");
        ASSERT(m_apAreaTextTexture[0]);
        ASSERT(m_apAreaTextTexture[1]);
        break;

    case WORLDID::ID_TRI:
        m_apAreaTextTexture[0] = CTextureManager::GetRwTexture("ar_tr_text_todhoonib");
        m_apAreaTextTexture[1] = CTextureManager::GetRwTexture("ar_tr_text_tojapan");
        ASSERT(m_apAreaTextTexture[0]);
        ASSERT(m_apAreaTextTexture[1]);
        break;

    case WORLDID::ID_JPN:
        m_apAreaTextTexture[0] = CTextureManager::GetRwTexture("ar_jp_text_tony");
        m_apAreaTextTexture[1] = CTextureManager::GetRwTexture("ar_jp_text_tospace");
        ASSERT(m_apAreaTextTexture[0]);
        ASSERT(m_apAreaTextTexture[1]);
        break;

    case WORLDID::ID_FNY:
        m_apAreaTextTexture[0] = CTextureManager::GetRwTexture("ar_fny_text_tony");
        ASSERT(m_apAreaTextTexture[0]);
        break;

    case WORLDID::ID_KUR:
        m_apAreaTextTexture[0] = CTextureManager::GetRwTexture("ar_ku_text_tony");
        ASSERT(m_apAreaTextTexture[0]);
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

    CRenderStateManager::SetDefault();

    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);

    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();
    WORLDID::VALUE idWorld = CAreaInfo::GetWorldNo(idArea);
    
    AreaDisp(idWorld);
    KameIconDisp();
    ClearedAnimDisp();
    AreaSelectGuide();

    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);

    if (static_cast<float>(m_kameIconAnimCount) >= ANIM_DURATION_FRAMES(30))
    {
        m_bKameIconAnimFlag = !m_bKameIconAnimFlag;
        m_kameIconAnimCount = 0;
    }
    else
    {
        ++m_kameIconAnimCount;
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
    
        if ((CAreaInfo::GetWorldNo(idArea) == idWorld)
            && ((idArea < AREAID::ID_MNY_J01) || (idArea > AREAID::ID_KUR_J01))
            && (idArea < AREAID::STATIONMAX)
            && (idArea != AREAID::HOME))
        {
            AreaMarkDisp(idArea);
        };
    };
};


void CAreaWorkPool::AreaTextDisp(int32 nIndex)
{
    static const Rt2dBBox s_aTextDispTable[] =
    {        
        //
        //  NY
        //
        {    1.0f,    53.0f,    64.0f,   64.0f,  },
        { -145.0f,  -130.0f,    64.0f,   32.0f,  },
        { -264.0f,    56.0f,    64.0f,   64.0f,  },
        {  202.0f,   -50.0f,    64.0f,  128.0f,  },
        {   90.0f,  -202.0f,   128.0f,   64.0f,  },
        { -282.0f,     -8.0,   128.0f,   64.0f,  },

        //
        //  DHO
        //
        { -276.0f,    56.0f,    64.0f,   64.0f,  },
        {  116.0f,    93.0f,   128.0f,   64.0f,  },

        //
        //  TRI
        //
        { -196.0f,   116.0f,   128.0f,   32.0f,  },
        {  188.0f,   102.0f,    64.0f,   64.0f,  },

        //
        //  JPN
        //
        { -272.0f,    92.0f,   128.0f,   32.0f,  },
        {  172.0f,  -200.0f,   128.0f,   64.0f,  },

        //
        //  FNY
        //
        { -268.0f,    32.0f,    64.0f,   64.0f,  },

        //
        //  KURA
        //
        {  172.0f,    92.0f,   128.0f,   64.0f,  },
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

    default:
        break;
    };

    ASSERT(nTextDispIdx >= 0);
    ASSERT(nTextDispIdx < COUNT_OF(s_aTextDispTable));

    if (GetTextDrawEnable(nIndex))
    {
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetTexture(m_apAreaTextTexture[nIndex]);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Move(s_aTextDispTable[nTextDispIdx].x,
                      s_aTextDispTable[nTextDispIdx].y);
        m_sprite.Resize(s_aTextDispTable[nTextDispIdx].w,
                        s_aTextDispTable[nTextDispIdx].h);
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
        {  -83.0f,  -158.0f,    32.0f,    64.0f   },
        {  -84.0f,  -188.0f,   256.0f,   256.0f,  },
        {  -33.0f,    66.0f,   128.0f,    64.0f,  },
        {   97.0f,    92.0f,    64.0f,    32.0f,  },
        {  151.0f,    69.0f,    64.0f,    32.0f,  },
        {  108.0f,  -108.0f,    32.0f,    64.0f,  },
        {  158.0f,  -112.0f,    32.0f,    16.0f,  },
        {  181.0f,  -226.0f,    32.0f,   128.0f,  },
        {  208.0f,  -218.0f,    64.0f,   128.0f,  },
        {  219.0f,   -66.0f,    64.0f,    32.0f,  },
        {  177.0f,   -53.0f,    64.0f,    16.0f,  },
        {   79.0f,   -23.0f,   128.0f,    32.0f,  },
        { -151.0f,   -53.0f,   128.0f,   128.0f,  },
        { -105.0f,    87.0f,    16.0f,    32.0f,  },
        { -216.0f,   -51.0f,    64.0f,    32.0f,  },
        { -158.0f,    23.0f,     8.0f,    32.0f,  },
        { -192.0f,    64.0f,    64.0f,    64.0f,  },
        { -267.0f,   113.0f,    64.0f,    32.0f,  },
        { -279.0f,   -33.0f,   128.0f,    64.0f,  },

        //
        //  DHO
        //
        {  128.0f,   -36.0f,    32.0f,    16.0f,  },
        {  166.0f,   -10.0f,    16.0f,    16.0f,  },
        {  164.0f,    34.0f,    16.0f,    32.0f,  },
        {  176.0f,    88.0f,    32.0f,    16.0f,  },
        {  240.0f,    76.0f,    32.0f,    32.0f,  },
        {   56.0f,   -18.0f,    64.0f,    64.0f,  },
        {   92.0f,    64.0f,    16.0f,    32.0f,  },
        {   20.0f,    28.0f,    32.0f,    32.0f,  },
        {   40.0f,   -44.0f,    32.0f,    16.0f,  },
        { -200.0f,   -28.0f,   256.0f,   256.0f,  },
        { -108.0f,   -64.0f,   128.0f,    32.0f,  },
        { -224.0f,   -64.0f,   128.0f,    32.0f,  },
        { -228.0f,   -32.0f,    32.0f,    64.0f,  },
        { -260.0f,    20.0f,   128.0f,   128.0f,  },

        //
        //  TRI
        //
        { -264.0f,   100.0f,   128.0f,    64.0f,  },
        { -168.0f,   -24.0f,   256.0f,   128.0f,  },
        {   -4.0f,   -40.0f,    32.0f,    16.0f,  },
        {   60.0f,   -52.0f,    32.0f,    16.0f,  },
        {  112.0f,   -50.0f,    64.0f,    16.0f,  },
        {  179.0f,   -23.0f,    32.0f,     8.0f,  },
        {  220.0f,   -16.0f,    16.0f,    64.0f,  },
        {  148.0f,    52.0f,   128.0f,   128.0f,  },
        {  136.0f,   -12.0f,    32.0f,   128.0f,  },
        {   60.0f,    60.0f,    64.0f,    32.0f,  },
        {   -8.0f,    -8.0f,    64.0f,    64.0f,  },

        //
        //  JPN
        //
        {  224.0f,  -160.0f,    64.0f,   128.0f,  },
        {  188.0f,   -64.0f,    64.0f,    32.0f,  },
        {  172.0f,   -28.0f,    64.0f,    64.0f,  },
        {   64.0f,    60.0f,   256.0f,    64.0f,  },
        { -172.0f,    64.0f,   256.0f,    64.0f,  },
        { -276.0f,    64.0f,   128.0f,    32.0f,  },
        { -184.0f,   -10.0f,    64.0f,    64.0f,  },
        { -104.0f,   -48.0f,   128.0f,    32.0f,  },
        {   48.0f,   -52.0f,   128.0f,    16.0f,  },

        //
        //  FNY
        //
        { -268.0f,   100.0f,   128.0f,    32.0f,  },
        { -116.0f,    32.0f,   128.0f,   128.0f,  },
        {   24.0f,   -44.0f,    64.0f,   128.0f,  },
        {   12.0f,  -124.0f,    64.0f,    64.0f,  },
        { -204.0f,  -120.0f,   256.0f,   256.0f,  },

        //
        //  KURA
        //
        {  116.0f,   -36.0f,   128.0f,   128.0f,  },
        {   92.0f,    52.0f,    16.0f,    64.0f,  },
        {  120.0f,   112.0f,   128.0f,    64.0f,  },
        { -100.0f,    30.0f,   256.0f,    16.0f,  },
        { -150.0f,   -48.0f,    32.0f,   128.0f,  },
        { -144.0f,   -64.0f,   128.0f,    64.0f,  },
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
        break;
    };

    ASSERT(nLineDispIdx >= 0);
    ASSERT(nLineDispIdx < COUNT_OF(s_aLineDispTable));

    if (GetLineDrawEnable(nIndex))
    {
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetTexture(m_apAreaLineTexture[nIndex - 1]);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Move(s_aLineDispTable[nLineDispIdx].x,
                      s_aLineDispTable[nLineDispIdx].y);
        m_sprite.Resize(s_aLineDispTable[nLineDispIdx].w,
                        s_aLineDispTable[nLineDispIdx].h);
        m_sprite.Draw();
    };
};


void CAreaWorkPool::AreaMarkDisp(AREAID::VALUE idArea)
{
    CAreaRecord::STATE AreaState = GetAreaState(idArea);

    if ((idArea >= AREAID::ID_MNY_STN) &&
        (idArea <= AREAID::ID_KUR_STN))
    {
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Resize(128.0f, 64.0f);
        m_sprite.SetTexture(m_apAreaSelTexture[3]);

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

    if (m_areaCleared && (idArea == m_areaCleared))
        AreaState = CAreaRecord::STATE_OPEN;    

    RwV2d vAreaPos = Math::VECTOR2_ZERO;
    GetAreaPosition(&vAreaPos, idArea);

    m_sprite.ResetUV();
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetOffset(0.0f, 0.0f);
    m_sprite.Resize(64.0f, 64.0f);
    m_sprite.SetTexture(m_apAreaSelTexture[AreaState]);
    m_sprite.Move(vAreaPos.x, vAreaPos.y);
    m_sprite.Draw();

    if (AreaState == CAreaRecord::STATE_CLEAR)
    {
        GAMETYPES::CLEARRANK AreaClearrank = CGameData::Record().Area().GetAreaClearRank(idArea);
        if (AreaClearrank)
        {
            m_sprite.SetTexture(m_apAreaSelTexture[AreaClearrank + 9]);
            m_sprite.Draw();
        };
    };
};


void CAreaWorkPool::KameIconDisp(void)
{
    if (m_areaCleared != AREAID::ID_NONE)
        return;

    m_sprite.ResetUV();
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetTexture(m_apAreaSelTexture[4]);
    m_sprite.Move(m_vKamePos.x + 32.0f, m_vKamePos.y + 32.0f);
    m_sprite.Resize(64.0f, 64.0f);
    m_sprite.Draw();

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

    uint8 AlphaBasis = uint8(Math::LinearTween(0.0f,
                                               255.0f,
                                               float(m_kameIconAnimCount),
                                               float(CScreen::Framerate() * 0.5f)));

    if (!m_bKameIconAnimFlag)
        AlphaBasis = 255 - AlphaBasis;
    
    m_sprite.SetAlpha(AlphaBasis);
    m_sprite.Draw();

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);

    m_sprite.SetAlpha(255);

    if ((m_animation != ANIMATION_KAMEMOVE) &&
        (m_areaNow < AREAID::SELECTABLEMAX))
    {
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.Resize(32.0f, 32.0f);

        for (int32 i = 0; i < DIRECTIONNUM; ++i)
        {
            if (!CheckAreaMove(m_areaNow, i))
                continue;

            static const RwV2d s_aArrowOffsetTable[] =
            {
                {   16.0f, -16.0f,  },
                {   16.0f,  48.0f,  },
                {  -12.0f,  16.0f,  },
                {   44.0f,  16.0f,  },
            };

            static_assert(COUNT_OF(s_aArrowOffsetTable) == DIRECTIONNUM, "update me");

            ASSERT(i >= 0);
            ASSERT(i < COUNT_OF(s_aArrowOffsetTable));

            m_sprite.SetTexture(m_apAreaSelTexture[5 + i]);
            m_sprite.Move(s_aArrowOffsetTable[i].x + m_vKamePos.x,
                          s_aArrowOffsetTable[i].y + m_vKamePos.y);
            m_sprite.Draw();
        };
    };

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
};


void CAreaWorkPool::ClearedAnimDisp(void)
{
    if (m_animation != ANIMATION_CLEARED)
        return;

    RwV2d vAreaPos = Math::VECTOR2_ZERO;
    GetAreaPosition(&vAreaPos, m_areaCleared);

    m_sprite.ResetUV();
    m_sprite.SetOffset(0.5f, 0.5f);

    //
    //  Kemuri anim
    //
    m_sprite.SetTexture(m_apAreaSelTexture[9]);
    m_sprite.Move(vAreaPos.x + 32.0f, vAreaPos.y + 32.0f);
    m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[0].alphaBasis);
    m_sprite.Resize(m_aClearAnim[0].scale, m_aClearAnim[0].scale);
    m_sprite.Draw();

    //
    //  Area complete icon
    //
    m_sprite.SetTexture(m_apAreaSelTexture[2]);
    m_sprite.Move(vAreaPos.x + 32.0f, vAreaPos.y + 32.0f);
    m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[1].alphaBasis);
    m_sprite.Resize(m_aClearAnim[1].scale, m_aClearAnim[1].scale);
    m_sprite.Draw();

    //
    //  Area clearrank
    //
    GAMETYPES::CLEARRANK areaClearedRank = CGameData::Record().Area().GetAreaClearRank(m_areaCleared);
    if (areaClearedRank)
    {
        m_sprite.SetTexture(m_apAreaSelTexture[areaClearedRank + 9]);
        m_sprite.Move(vAreaPos.x + 32.0f, vAreaPos.y + 32.0f);
        m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[2].alphaBasis);
        m_sprite.Resize(m_aClearAnim[2].scale, m_aClearAnim[2].scale);
        m_sprite.Draw();
    };

    if (m_bKameClearRotAnimFlag)
    {
        m_sprite.SetTexture(m_apAreaSelTexture[4]);
        m_sprite.Move(m_vKamePos.x + 32.0f, m_vKamePos.y + 32.0f);
        m_sprite.SetRGBA(255, 255, 255, m_aClearAnim[3].alphaBasis);
        m_sprite.Resize(m_aClearAnim[3].scale, m_aClearAnim[3].scale);
        m_sprite.SetRotate(m_fClearAnimRot);
        m_sprite.DrawRotate();
    };
};


void CAreaWorkPool::AreaSelectGuide(void)
{
    CSystem2D::PushRenderState();

    CGameFont::SetRGBA(255, 170, 0, 255);

    if ((CAreaInfo::GetWorldNo(m_areaNow) == WORLDID::ID_FNY) &&
        (GetAreaState(AREAID::ID_AREA50) == CAreaRecord::STATE_CLEAR))
    {
        if (m_areaNow != AREAID::ID_FNY_STN)
        {
            const wchar* pwszText = CGameText::GetText(GAMETEXT_FUTURE);

            CGameFont::SetHeightScaled(2.5f);
            CGameFont::Show(pwszText, -269.0f, -175.0f);
        };
    }
    else
    {
        const wchar* pwszText = CAreaInfo::GetDispWorldName(m_areaNow);

        CGameFont::SetHeightScaled(2.5f);
        CGameFont::Show(pwszText, -269.0f, -175.0f);
    };

    if (m_bAreaDiskFlag)
    {
        const wchar* pwszText = CSystemText::GetText(SYSTEXT_GAME_DISC);

        wchar wszBuffer[1024];
        wszBuffer[0] = UTEXT('\0');
        
        CTextData::Sprintf(wszBuffer, pwszText, CAreaInfo::GetDiscNo(m_areaNow));
        
        CGameFont::SetRGBA(255, 0, 0, 255);
        CGameFont::SetHeightScaled(2.0f);
        CGameFont::Show(wszBuffer, -269.0f, 152.0f);
    };

    CSystem2D::PopRenderState();
};


void CAreaWorkPool::AreaMoveAnimationInit(int32 nDirection)
{
    AREAID::VALUE idAreaNew = GetMoveToArea(m_areaNow, nDirection);
    if (CheckAreaMove(m_areaNow, nDirection))
    {
        m_areaOld = m_areaNew;
        m_areaNew = idAreaNew;
        m_animation = ANIMATION_KAMEMOVE;
        CGameSound::PlaySE(SDCODE_SE(4110));
    };
};


void CAreaWorkPool::AreaMoveAnimation(void)
{
    RwV2d vOldPos = Math::VECTOR2_ZERO;
    GetAreaPosition(&vOldPos, m_areaOld);

    RwV2d vNewPos = Math::VECTOR2_ZERO;
    GetAreaPosition(&vNewPos, m_areaNew);

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

    float duration = ANIM_DURATION_FRAMES(24);
    float len = std::sqrt(x * x + y * y);
    float step = len / duration;

    x *= (1.0f / len) * step;
    y *= (1.0f / len) * step;

    if (static_cast<float>(m_kameMoveAnimCount) >= duration)
    {        
        GetAreaPosition(&m_vKamePos, m_areaNew);
        
        m_kameMoveAnimCount = 0;
        m_animation = ANIMATION_NONE;
        m_areaNow = m_areaNew;

#ifdef _DEBUG        
        OnAreaChanged();
#endif /* _DEBUG */
        
        if (m_areaNow < AREAID::SELECTABLEMAX)
            CGameData::Record().Area().SetCurrentSelectedArea(m_areaNow);
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
        
        ++m_kameMoveAnimCount;
    };
};


void CAreaWorkPool::AreaClearedAnimationInit(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aClearAnim); ++i)
    {
        m_aClearAnim[i].step = 0;
        m_aClearAnim[i].counter = 0;
        m_aClearAnim[i].alphaBasis = 0;
        m_aClearAnim[i].scale = 0.0f;
    };

    m_fClearAnimRot = 0.0f;
    m_animation = ANIMATION_CLEARED;
    m_bKameClearRotAnimFlag = false;
};


void CAreaWorkPool::AreaClearedAnimation(void)
{
    if (m_bKameClearRotAnimFlag)
    {
        CLEARANIM* anim = &m_aClearAnim[3];

        const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(30));

        if (anim->counter >= duration)
        {
            m_areaCleared = AREAID::ID_NONE;
            m_animation = ANIMATION_NONE;
        }
        else
        {
            ++anim->counter;
        };

        anim->alphaBasis = static_cast<uint8>(LinearTween(0.0f, 255.0f, anim->counter, duration));
        anim->scale = LinearTween(192.0f, -128.0f, anim->counter, duration);        

        m_fClearAnimRot = LinearTween(360.0f, -360.0f, anim->counter, duration);
    }
    else
    {
        CLEARANIM* anim = nullptr;

        //
        //  Kemuri
        //
        anim = &m_aClearAnim[0];
        switch (anim->step)
        {
        case 0:
            {
                if (anim->counter >= static_cast<uint32>(ANIM_DURATION_FRAMES(8)))
                {
                    ++anim->step;
                    anim->counter = 0;
                }
                else
                {
                    ++anim->counter;
                };
            }
            break;

        case 1:
            {
                const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(10));

                anim->alphaBasis = static_cast<uint8>(LinearTween(0.0f, 255.0f, anim->counter, duration));
                anim->scale = LinearTween(64.0f, -38.4f, anim->counter, duration);

                if (anim->counter >= duration)
                {
                    ++anim->step;
                    anim->counter = 0;
                }
                else
                {
                    ++anim->counter;
                };
            }
            break;

        case 2:
            {
                const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(26));

                anim->alphaBasis = static_cast<uint8>(LinearTween(255.0f, -255.0f, anim->counter, duration));
                anim->scale = LinearTween(89.6f, 64.0f, anim->counter, duration);

                if (anim->counter >= duration)
                {
                    m_bKameClearRotAnimFlag = true;
                }
                else
                {
                    ++anim->counter;
                };
            }
            break;

        default:
            break;
        };

        //
        //  Status
        //
        anim = &m_aClearAnim[1];
        switch (anim->step)
        {
        case 0:
            {
                const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(8));

                anim->alphaBasis = static_cast<uint8>(LinearTween(0.0f, 255.0f, anim->counter, duration));
                anim->scale = LinearTween(192.0f, -128.0f, anim->counter, duration);

                if (anim->counter >= duration)
                {
                    ++anim->step;
                    anim->counter = 0;
                }
                else
                {
                    ++anim->counter;
                };
            }
            break;

        case 1:
            {
                const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(6));

                anim->scale = LinearTween(44.8f, 19.2f, anim->counter, duration);

                if (anim->counter < duration)
                    ++anim->counter;
            }
            break;

        default:
            break;
        };

        //
        //  Rank
        //
        anim = &m_aClearAnim[2];
        switch (anim->step)
        {
        case 0:
            {
                if (anim->counter >= static_cast<uint32>(ANIM_DURATION_FRAMES(10)))
                {
                    ++anim->step;
                    anim->counter = 0;
                }
                else
                {
                    ++anim->counter;
                };
            }
            break;
            
        case 1:
            {
                const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(6));

                anim->alphaBasis = static_cast<uint8>(LinearTween(0.0f, 255.0f, anim->counter, duration));
                anim->scale = LinearTween(192.0f, -128.0f, anim->counter, duration);

                if (anim->counter >= duration)
                {
                    ++anim->step;
                    anim->counter = 0;
                }
                else
                {
                    ++anim->counter;
                };
            }
            break;

        case 2:
            {
                const uint32 duration = static_cast<uint32>(ANIM_DURATION_FRAMES(6));

                anim->scale = LinearTween(51.2f, 12.8f, anim->counter, duration);

                if (anim->counter < duration)
                    ++anim->counter;
            }
            break;

        default:
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
        break; /* case WORLDID::ID_MNY */

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
                if (IsAreaRootCleared(AREAID::ID_AREA11, CAreaRecord::CLEAR_ROOT_B))
                    bResult = true;
                break;

            case 11:
                if (GetAreaState(AREAID::ID_AREA14))
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
        break; /* case WORLDID::ID_DHO */

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
        break; /* case WORLDID::ID_TRI */

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
        break; /* case WORLDID::ID_JPN */

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
        break; /* case WORLDID::ID_FNY */

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
        break; /* case WORLDID::ID_KUR */

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
        break; /* case WORLDID::ID_MNY */

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
        break; /* case WORLDID::ID_DHO */

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
        break; /* case WORLDID::ID_TRI */

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
        break; /* case WORLDID::ID_JPN */

    case WORLDID::ID_FNY:
        if (GetAreaState(AREAID::ID_AREA48))
            bResult = true;
        break; /* case WORLDID::ID_FNY */

    case WORLDID::ID_KUR:
        if (GetAreaState(AREAID::ID_AREA52))
            bResult = true;
        break; /* case WORLDID::ID_KUR */

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
        {    0.0f,     0.0f, },
        { -110.0f,  -126.0f, },
        {  -75.0f,  -178.0f, },
        {  -40.0f,    20.0f, },
        {   60.0f,    87.0f, },
        {  112.0f,    48.0f, },
        {  175.0f,    79.0f, },
        {   85.0f,   -76.0f, },
        {  118.0f,  -129.0f, },
        {  170.0f,  -148.0f, },
        { -212.0f,   -13.0f, },
        { -254.0f,   -68.0f, },
        { -147.0f,   -91.0f, },
        {   -5.0f,   -65.0f, },
        {   20.0f,   -10.0f, },
        {  -25.0f,    23.0f, },
        {   73.0f,    23.0f, },
        {   62.0f,    74.0f, },
        {  147.0f,   -60.0f, },
        {  139.0f,    -9.0f, },
        {  152.0f,    40.0f, },
        {  193.0f,    61.0f, },
        {  -42.0f,   -51.0f, },
        {   32.0f,    12.0f, },
        {  100.0f,    44.0f, },
        {   12.0f,   -75.0f, },
        {   77.0f,   -82.0f, },
        {  132.0f,   -50.0f, },
        {  195.0f,   -52.0f, },
        {  201.0f,    19.0f, },
        {  152.0f,   -73.0f, },
        {    8.0f,   -79.0f, },
        { -149.0f,   -47.0f, },
        {  201.0f,    19.0f, },
        {   47.0f,    81.0f, },
        { -208.0f,    39.0f, },
        {  195.0f,   -81.0f, },
        {  146.0f,   -72.0f, },
        { -126.0f,    44.0f, },
        { -132.0f,    97.0f, },
        { -177.0f,   -68.0f, },
        { -238.0f,   -93.0f, },
        { -187.0f,   -19.0f, },
        { -184.0f,    32.0f, },
        { -223.0f,    82.0f, },
        { -158.0f,    75.0f, },
        {   33.0f,   -89.0f, },
        {  -32.0f,  -152.0f, },
        {   84.0f,    83.0f, },
        {   82.0f,   -44.0f, },
        {  144.0f,   -73.0f, },
        {  201.0f,   -36.0f, },
        { -132.0f,    -4.0f, },
        { -179.0f,   -87.0f, },
        {  -64.0f,   -31.0f, },
        {  -37.0f,   -45.0f, },
        {   80.0f,   -35.0f, },
        { -120.0f,   -30.0f, },
        {   73.0f,   -24.0f, },
        { -216.0f,    76.0f, },
        {  140.0f,   -20.0f, },
        {  -40.0f,    20.0f, },
        {   67.0f,   -64.0f, },
        { -196.0f,    60.0f, },
        {  200.0f,  -104.0f, },
        {  -32.0f,    -4.0f, },
        {   68.0f,    12.0f, },
        {  167.0f,  -220.0f, },
        {  217.0f,  -220.0f, },
        {  230.0f,   -83.0f, },
        { -284.0f,   100.0f, },
        { -286.0f,   -50.0f, },
        { -274.0f,    92.0f, },
        { -221.0f,    99.0f, },
        {  226.0f,    62.0f, },
        { -284.0f,   104.0f, },
        {  132.0f,   116.0f, },
        {  224.0f,  -180.0f, },
        { -296.0f,    48.0f, },
        { -288.0f,    84.0f, },
        {  168.0f,   120.0f, },
        {  -40.0f,  -190.0f, },
        {  -40.0f,    87.0f, },
        {  112.0f,    82.0f, },
        {  170.0f,    44.0f, },
        {   80.0f,   -35.0f, },
        {   85.0f,  -120.0f, },
        {  202.0f,  -150.0f, },
        {  140.0f,   -35.0f, },
        {  -70.0f,     5.0f, },
        {  -67.0f,   -37.0f, },
        { -120.0f,   -73.0f, },
        { -233.0f,   -63.0f, },
        { -180.0f,    70.0f, },
        { -252.0f,   -14.0f, },
        { -222.0f,    31.0f, },
        { -251.0f,   -14.0f, },
        { -202.0f,   -74.0f, },
        {  -13.0f,   -35.0f, },
        {    7.0f,    23.0f, },
        {  149.0f,    67.0f, },
        { -212.0f,    80.0f, },
        { -148.0f,   -12.0f, },
        {   -8.0f,    16.0f, },
        {   44.0f,    52.0f, },
        {  124.0f,   -80.0f, },
        {  196.0f,   -76.0f, },
        { -196.0f,   -32.0f, },
        {  156.0f,   -36.0f, },
        {  192.0f,   -12.0f, },
        {  188.0f,    64.0f, },
        {  -80.0f,    76.0f, },
        { -156.0f,    44.0f, },
        { -120.0f,    72.0f, },
        {   28.0f,    -8.0f, },
        {   16.0f,  -132.0f, },
        {  -64.0f,  -136.0f, },
        {   64.0f,    52.0f, },
        {  120.0f,    12.0f, },
        { -172.0f,    -8.0f, },
        { -104.0f,   -80.0f, },
        {    0.0f,     0.0f, },
        {    0.0f,     0.0f, },
        {    0.0f,     0.0f, },
        {    0.0f,     0.0f, },
    };

    static_assert(COUNT_OF(s_aAreaPosTable) == AREAID::NEXUSMAX, "update me");

    ASSERT(idArea >= 0);
    ASSERT(idArea < COUNT_OF(s_aAreaPosTable));

    ASSERT(pPosition);
    
    *pPosition = s_aAreaPosTable[idArea];
};


AREAID::VALUE CAreaWorkPool::GetMoveToArea(AREAID::VALUE idArea, int32 nDirection)
{
    static const AREAID::VALUE s_aAreaMoveTable[][DIRECTIONNUM] =
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
        { AREAID::ID_AREA57,    AREAID::ID_NONE,    AREAID::ID_NONE,    AREAID::ID_AREA58,      },
    };

    ASSERT(idArea >= 0);
    ASSERT(idArea < COUNT_OF(s_aAreaMoveTable));

    ASSERT(nDirection >= 0);
    ASSERT(nDirection < COUNT_OF(s_aAreaMoveTable[0]));

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
    if (idAreaToMove != m_areaNow)
    {
        while (idAreaToMove)
        {
            if (idAreaToMove < AREAID::NORMALMAX)
                return GetAreaState(idAreaToMove) != CAreaRecord::STATE_NONE;
            
            if (idAreaToMove <= AREAID::ID_KUR_J01)
            {
                for (int32 i = 0; i < DIRECTIONNUM; i++)
                {
                    if (CheckAreaMove(idAreaToMove, i))
                        return true;
                };

                return false;
            };
            
            if (idAreaToMove <= AREAID::ID_KUR_E01)
                return GetAreaState(idAreaToMove) != CAreaRecord::STATE_NONE;
            
            idAreaToMove = GetMoveToArea(idAreaToMove, nDirection);
            if (idAreaToMove == m_areaNow)
                break;
        };
    };

    return false;
};


AREATYPES::NEXTSEQUENCE CAreaWorkPool::GetNextSequence(void) const
{
    return m_nextSequence;
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


void CAreaWorkPool::OnAreaChanged(void)
{
#ifdef _DEBUG
    char szBuff[256];
    szBuff[0] = '\0';

    const wchar* pwszDispName = CAreaInfo::GetDispName(m_areaNow);
    
    if (pwszDispName)
        CTextData::ToMultibyte(szBuff, COUNT_OF(szBuff), pwszDispName);
    
    OUTPUT("Area changed! Id: %d, (%s - %s)\n",
           m_areaNew,
           szBuff,
           CMapInfo::GetName(CStageInfo::GetMapID(CAreaInfo::GetStageID(m_areaNow, 0))));
#endif /* _DEBUG */
};


void CAreaWorkPool::DebugStartPlayClearAnim(void)
{
#ifdef _DEBUG    
    AREAID::VALUE idArea = CGameData::Record().Area().GetCurrentSelectedArea();

    switch (CAreaInfo::GetWorldNo(idArea))
    {
    case WORLDID::ID_MNY:
        m_areaCleared = AREAID::ID_AREA02;
        break;

    case WORLDID::ID_DHO:
        m_areaCleared = AREAID::ID_DHO_STN;
        break;

    case WORLDID::ID_TRI:
        m_areaCleared = AREAID::ID_TRI_STN;
        break;

    case WORLDID::ID_JPN:
        m_areaCleared = AREAID::ID_JPN_STN;
        break;

    case WORLDID::ID_FNY:
        m_areaCleared = AREAID::ID_FNY_STN;
        break;

    case WORLDID::ID_KUR:
        m_areaCleared = AREAID::ID_KUR_STN;
        break;

    default:
        ASSERT(false);
        break;
    };

    GetAreaPosition(&m_vKamePos, m_areaCleared);

    AreaClearedAnimationInit();
#endif /* _DEBUG */
};


/*static*/ CProcess* CAreaSequence::Instance(void)
{
    return new CAreaSequence;
};


CAreaSequence::CAreaSequence(void)
: m_pWorkPool(nullptr)
, m_pDlgSure(nullptr)
, m_bDlgRunning(false)
#ifdef _DEBUG
, m_bDebugCall(false)
#endif    
{
    ;
};


CAreaSequence::~CAreaSequence(void)
{
    ;
};


bool CAreaSequence::OnAttach(const void* pParam)
{
#ifdef _DEBUG
    m_bDebugCall = static_cast<bool>(pParam != nullptr);
    if (m_bDebugCall)
    {
        CGameData::PushRecord();
        CGameData::Record().SetDefault();

        for (int32 i = AREAID::SELECTABLESTART; i < AREAID::SELECTABLEMAX; ++i)
        {
            CGameData::Record().Area().SetAreaOpened(AREAID::VALUE(i));
            CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_C);
        };

        int32 areaId = reinterpret_cast<int32>(pParam);
        CGameData::Record().Area().SetCurrentSelectedArea(AREAID::VALUE(areaId));
    };
#endif /* _DEBUG */   

    m_pWorkPool = new CAreaWorkPool;
    m_pWorkPool->Attach();

    
    m_pDlgSure = new CDialog(CDialog::COLOR_NORMAL, CDialog::STATUS_NO, CGameData::Attribute().GetVirtualPad());
    m_pDlgSure->Set(0.0f, 0.0f, CSprite::m_fVirtualScreenW, 140.0f);
    m_pDlgSure->SetOpenAction(true);
    m_pDlgSure->SetText(CGameText::GetText(GAMETEXT_AREA_RET),
                        CGameFont::GetHeightScaled() * 2.0f,
                        { 0xFF, 0xFF, 0xFF, 0xFF });
    
    m_bDlgRunning = false;

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

#ifdef _DEBUG
    if (m_bDebugCall)
        CGameData::PopRecord();
#endif /* _DEBUG */

    CAnim2DSequence::OnDetach();
};


void CAreaSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (AnimStep())
    {
    case ANIMSTEP_FADEIN:
        {
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
        }
        break;

    case ANIMSTEP_DRAW:
        {
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
                        BeginFadeOut();
                        break;

                    default:
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
                    BeginFadeOut();
                };
            };
        }
        break;

    case ANIMSTEP_END:
        {
            m_pWorkPool->UpdateAreaWarp();

            switch (m_pWorkPool->GetNextSequence())
            {
            case AREATYPES::NEXTSEQUENCE_AREA:
            case AREATYPES::NEXTSEQUENCE_WARP:
                Ret();
                break;

            case AREATYPES::NEXTSEQUENCE_CHARASEL:
                Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_CHARASELECT));
                break;

            case AREATYPES::NEXTSEQUENCE_MENUSAVE:
                Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_SAVELOADMENUSAVE));
                break;

            case AREATYPES::NEXTSEQUENCE_MENULOAD:
                Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_SAVELOADMENULOAD));
                break;

            case AREATYPES::NEXTSEQUENCE_OPTIONS:
                Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_OPTIONS));
                break;

            case AREATYPES::NEXTSEQUENCE_TITLE:
                Ret(reinterpret_cast<const void*>(PROCLABEL_SEQ_TITLE));
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    default:
        break;
    };

    CAnim2DSequence::OnMove(bRet, pReturnValue);
};


void CAreaSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();
    CRenderStateManager::SetDefault();

    if (IsDrawing())
    {
        m_pWorkPool->TextureDraw();
        m_pWorkPool->AreaMenuDraw();

        CRenderStateManager::SetDefault();
    };

#ifdef TMNT2_BUILD_EU
    CSystem2D::PushRenderState();

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, 255);

    const wchar* pwszText = CGameText::GetText(GAMETEXT_EU_HELP_5);

    if (CGameFont::GetStringWidth(pwszText) >= 500.0f)
    {
        Rt2dBBox bbox;
        bbox.x = -272.0f;
        bbox.y = -230.0f;
        bbox.w =  500.0f;
        bbox.h =   60.0f;

        CGameFont::Flow(pwszText, &bbox);
    }
    else
    {
        CGameFont::Show(pwszText, -272.0f, 190.0f);
    };

    CSystem2D::PopRenderState();
#endif /* TMNT2_BUILD_EU */
};


bool CAreaSequence::AreaSelectLoad(void)
{
    FILEID::VALUE fileId = FILEID::ID_INVALID;
    const char* pszAnimName = nullptr;
    
    AREAID::VALUE areaId = CGameData::Record().Area().GetCurrentSelectedArea();
    WORLDID::VALUE worldId = CAreaInfo::GetWorldNo(areaId);
    switch (worldId)
    {
    case WORLDID::ID_MNY:
        pszAnimName = "area_ny";
        fileId      = FILEID::ID_AREA_NY;
        break;

    case WORLDID::ID_DHO:
        pszAnimName = "area_dho";
        fileId      = FILEID::ID_AREA_DHO;
        break;

    case WORLDID::ID_TRI:
        pszAnimName = "area_tri";
        fileId      = FILEID::ID_AREA_TRI;
        break;

    case WORLDID::ID_JPN:
        pszAnimName = "area_jpn";
        fileId      = FILEID::ID_AREA_JPN;
        break;

    case WORLDID::ID_FNY:
        pszAnimName = "area_fny";
        fileId      = FILEID::ID_AREA_FNY;
        break;

    case WORLDID::ID_KUR:
        pszAnimName = "area_kur";
        fileId      = FILEID::ID_AREA_KUR;
        break;

    default:
        ASSERT(false);
        break;
    };

#ifdef TMNT2_BUILD_EU
    SetAnimationName("area");
    CDataLoader::Regist(fileId);
    return CAnim2DSequence::OnAttach(FILEID::ID_LANG_AREASELECT);
#else /* TMNT2_BUILD_EU */
    SetAnimationName(pszAnimName);
    return CAnim2DSequence::OnAttach(fileId);
#endif /* TMNT2_BUILD_EU */
};