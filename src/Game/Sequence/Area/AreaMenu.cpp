#include "AreaMenu.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/AreaID.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/TextData.hpp"
#include "System/Common/System2D.hpp"


class CAreaMenu_Container
{
public:
    static const int32 MENUITEM_MAX = 10;
    static const int32 MENUITEM_INVALID = -1;
    
    enum
    {
        MAINSTART = 0,
        MAIN_WARP = MAINSTART,
        MAIN_CHARSEL,
        MAIN_INFO,
        MAIN_OPTIONS,
        MAIN_SAVE,
        MAIN_LOAD,
        MAIN_TITLE,
        MAIN_BACK,
        MAINMAX,

        WARPSTART = 0,
        WARP_NY = WARPSTART,
        WARP_DHO,
        WARP_TRI,
        WARP_JPN,
        WARP_FNY,
        WARP_KUR,
        WARPMAX,

        COMBOSTART = 0,
        COMBO_A = COMBOSTART,
        COMBO_B,
        COMBO_CHARGE1,
        COMBO_CHARGE2,
        COMBO_CHARGE3,
        COMBO_JUMPKNIFE,
        COMBO_GUARDIMPACT,
        COMBO_AAB,
        COMBO_AAC,
        COMBO_AABB,
        COMBO_AABC,
        COMBO_AABBB,
        COMBO_AABBC,
        COMBOMAX,
        COMBOPAGEMAX = (COMBOMAX / 2) + 1,
    };

    enum SUBMENUTYPE
    {
        SUBMENUTYPE_NONE = 0,
        SUBMENUTYPE_WARP,
        SUBMENUTYPE_COMBO,
    };

    enum WINDOWTYPE
    {        
        WINDOWTYPE_NORMAL = 0,
        WINDOWTYPE_INFO,
        
        WINDOWTYPENUM,
    };

public:
    CAreaMenu_Container(void);
    ~CAreaMenu_Container(void);
    void AreaMenuInit_Sub(void);
    AREATYPES::NEXTSEQUENCE AreaMenuSelect_Sub(void);
    void AreaMenuStationWarp_Sub(void);
    void AreaMenuPeriod_Sub(void);
    void AreaMenuDraw_Sub(void);
    void AreaInfoMenuDraw_Sub(void);
    void AreaMenuDisp(void);
    void MenuDispSub_Station(void);
    void MenuDispSub_Combo(void);
    bool IsComboMenuSecret(int32);
    void WindowDisp(WINDOWTYPE type, float x, float y, float w, float h);
    const wchar* GetAreaMenuName(int32);
    const wchar* GetStationName(int32);
    void RenderStatePush(void);
    void RenderStatePop(void);

private:
    RwTexture* m_AreaMenuTexture[9];
    CSprite m_sprite;
    bool m_bTextureSettingFlag;
    bool m_bMenuDispFlag;
    bool m_bSubMenuDispFlag;
    uint32 m_MenuAnimCount;
    uint32 m_SubMenuAnimCount;
    int32 m_nMenuItemCount;
    int32 m_aMenuItemTable[MENUITEM_MAX];
    int32 m_nSubMenuItemCount;
    int32 m_aSubMenuItemTable[MENUITEM_MAX];
    SUBMENUTYPE m_SubmenuType;
    int32 m_nCursor;
    float m_fCursorRot;
    int32 m_nSubCursor;
    int32 m_nSubCursorPage;
    float m_fSubCursorRot;
};


CAreaMenu_Container::CAreaMenu_Container(void)
{
	m_bTextureSettingFlag = false;
};


CAreaMenu_Container::~CAreaMenu_Container(void)
{
    ;
};


void CAreaMenu_Container::AreaMenuInit_Sub(void)
{
    m_bMenuDispFlag     = true;
    m_bSubMenuDispFlag  = false;
    m_MenuAnimCount     = 0;
    m_SubMenuAnimCount  = 0;
    m_SubmenuType       = SUBMENUTYPE_NONE;
    m_nCursor           = 0;
    m_fCursorRot        = 0.0f;
    m_nSubCursor        = 0;
    m_nSubCursorPage    = 0;
    m_fSubCursorRot     = 0.0f;


    m_nSubMenuItemCount = 0;
    for (int32 i = WARPSTART, j = AREAID::ID_MNY_STN; i < WARPMAX; ++i, ++j)
    {
        AREAID::VALUE idAreaStn = AREAID::VALUE(j);
        AREAID::VALUE idAreaCur = CGameData::Record().Area().GetCurrentSelectedArea();

        m_aSubMenuItemTable[m_nSubMenuItemCount] = MENUITEM_INVALID;
        if (CGameData::Record().Area().GetAreaState(idAreaStn) &&
            CAreaInfo::GetWorldNo(idAreaCur) != CAreaInfo::GetWorldNo(idAreaStn))
        {
            m_aSubMenuItemTable[m_nSubMenuItemCount++] = i;
        };
    };

    m_nMenuItemCount = 0;
    for (int32 i = MAINSTART; i < MAINMAX; ++i)
    {
        m_aMenuItemTable[m_nMenuItemCount] = MENUITEM_INVALID;
        if (i || m_nSubMenuItemCount)
            m_aMenuItemTable[m_nMenuItemCount++] = i;
    };
};


AREATYPES::NEXTSEQUENCE CAreaMenu_Container::AreaMenuSelect_Sub(void)
{
    if (!m_bTextureSettingFlag)
		return AREATYPES::NEXTSEQUENCE_NONE;

    uint32 uAnimDur = uint32(CScreen::Framerate() * 0.2f);
    int32 iController = CGameData::Attribute().GetVirtualPad();

    switch (m_SubmenuType)
    {
    case SUBMENUTYPE_WARP:
        {
            if (m_SubMenuAnimCount < uAnimDur)
                break;

            if (!m_bSubMenuDispFlag)
            {
                m_SubmenuType = SUBMENUTYPE_NONE;
                break;
            };

            if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP))
            {
                m_nSubCursor = InvClamp(--m_nSubCursor, 0, m_nSubMenuItemCount - 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
            {
                m_nSubCursor = InvClamp(++m_nSubCursor, 0, m_nSubMenuItemCount - 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
            {
                m_bSubMenuDispFlag = false;
                m_SubMenuAnimCount = 0;
                CGameSound::PlaySE(SDCODE_SE(4097));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
            {
                CGameSound::PlaySE(SDCODE_SE(4098));
                return AREATYPES::NEXTSEQUENCE_WARP;
            };
        }
        break;

    case SUBMENUTYPE_COMBO:
        {
            if (m_SubMenuAnimCount < uAnimDur)
                break;

            if (!m_bSubMenuDispFlag)
            {
                m_SubmenuType = SUBMENUTYPE_NONE;
                break;
            };

            int32 nSubMenuItemCount = (m_nSubCursorPage ? COMBOPAGEMAX - 1 : COMBOPAGEMAX);
            
            if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP))
            {
                m_nSubCursor = InvClamp(--m_nSubCursor, 0, nSubMenuItemCount - 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
            {
                m_nSubCursor = InvClamp(++m_nSubCursor, 0, nSubMenuItemCount - 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LLEFT))
            {
                m_nSubCursorPage = InvClamp(--m_nSubCursorPage, 0, 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
				if (m_nSubCursorPage && (m_nSubCursor == (nSubMenuItemCount - 1)))
					--m_nSubCursor;
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LRIGHT))
            {
                m_nSubCursorPage = InvClamp(++m_nSubCursorPage, 0, 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
                if (m_nSubCursorPage && (m_nSubCursor == (nSubMenuItemCount - 1)))
                    --m_nSubCursor;
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
            {
                m_bSubMenuDispFlag = false;
                m_SubMenuAnimCount = 0;
                CGameSound::PlaySE(SDCODE_SE(4097));
            };
        }
        break;

    default:
        {
            ASSERT(m_nCursor >= 0 && m_nCursor < COUNT_OF(m_aMenuItemTable));
            ASSERT(m_nCursor >= 0 && m_nCursor < m_nMenuItemCount);
            
            if (m_MenuAnimCount < uAnimDur)
                break;

            if (!m_bMenuDispFlag)
                return AREATYPES::NEXTSEQUENCE_AREA;

            if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP))
            {
                m_nCursor = InvClamp(--m_nCursor, 0, m_nMenuItemCount - 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
            {
                m_nCursor = InvClamp(++m_nCursor, 0, m_nMenuItemCount - 1);
                CGameSound::PlaySE(SDCODE_SE(4100));
            }
			else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL | CController::DIGITAL_R1))
            {
                m_bMenuDispFlag = false;
                m_MenuAnimCount = 0;
                CGameSound::PlaySE(SDCODE_SE(4097));
            }
            else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
			{
				CGameSound::PlaySE(SDCODE_SE(4098));
				switch (m_aMenuItemTable[m_nCursor])
                {
                case MAIN_WARP:
                    m_SubmenuType = SUBMENUTYPE_WARP;
                    m_bSubMenuDispFlag = true;
                    m_nSubCursor = 0;
                    m_SubMenuAnimCount = 0;                    
                    break;
                    
                case MAIN_CHARSEL:
                    return AREATYPES::NEXTSEQUENCE_CHARASEL;
                    
                case MAIN_INFO:
                    m_SubmenuType = SUBMENUTYPE_COMBO;
                    m_bSubMenuDispFlag = true;
                    m_nSubCursor = 0;
                    m_SubMenuAnimCount = 0;
                    break;
                    
                case MAIN_OPTIONS:
                    return AREATYPES::NEXTSEQUENCE_OPTIONS;
                    
                case MAIN_SAVE:
                    return AREATYPES::NEXTSEQUENCE_MENUSAVE;
                    
                case MAIN_LOAD:
                    return AREATYPES::NEXTSEQUENCE_MENULOAD;
                    
                case MAIN_TITLE:
                    return AREATYPES::NEXTSEQUENCE_TITLE;
                    
				case MAIN_BACK:
					/**
					 *	NOTE:
					 *	in retail game there is call for playing CANCEL SE when back is pressed
					 *	that overlaps OK SE (see before switch case) and they are playing together
					 */
					CGameSound::PlaySE(SDCODE_SE(4097));

					m_bMenuDispFlag = false;
					m_MenuAnimCount = 0;
                    break;
                };
            };
        }
        break;
    };

    return AREATYPES::NEXTSEQUENCE_NONE;
};


void CAreaMenu_Container::AreaMenuStationWarp_Sub(void)
{
    ASSERT(m_SubmenuType == SUBMENUTYPE_WARP);
    ASSERT(m_nSubCursor >= 0 && m_nSubCursor < COUNT_OF(m_aSubMenuItemTable));
    ASSERT(m_nSubCursor >= 0 && m_nSubCursor < m_nSubMenuItemCount);

    m_SubmenuType = SUBMENUTYPE_NONE;
    
    AREAID::VALUE idAreaWarp = AREAID::ID_MNY_STN;

    switch (m_aSubMenuItemTable[m_nSubCursor])
    {
    case WARP_NY:
        idAreaWarp = AREAID::ID_MNY_STN;
        break;

    case WARP_DHO:
        idAreaWarp = AREAID::ID_DHO_STN;
        break;

    case WARP_TRI:
        idAreaWarp = AREAID::ID_TRI_STN;
        break;

    case WARP_JPN:
        idAreaWarp = AREAID::ID_JPN_STN;
        break;

    case WARP_FNY:
        idAreaWarp = AREAID::ID_FNY_STN;
        break;

    case WARP_KUR:
        idAreaWarp = AREAID::ID_KUR_STN;
        break;

    default:
        ASSERT(false);
        break;
    };

    CGameData::Record().Area().SetCurrentSelectedArea(idAreaWarp);    
};


void CAreaMenu_Container::AreaMenuPeriod_Sub(void)
{
    if (m_bTextureSettingFlag)
        return;

    m_bTextureSettingFlag = true;

    CTextureManager::SetCurrentTextureSet("area_common");
    m_AreaMenuTexture[0] = CTextureManager::GetRwTexture("ar_win");
    m_AreaMenuTexture[1] = CTextureManager::GetRwTexture("ar_cursor");
    m_AreaMenuTexture[2] = CTextureManager::GetRwTexture("ar_info_win");
    m_AreaMenuTexture[3] = CTextureManager::GetRwTexture("ar_antique");
    m_AreaMenuTexture[4] = CTextureManager::GetRwTexture("ar_arrow");
    m_AreaMenuTexture[5] = CTextureManager::GetRwTexture("ar_combo_a");
    m_AreaMenuTexture[6] = CTextureManager::GetRwTexture("ar_combo_b");
    m_AreaMenuTexture[7] = CTextureManager::GetRwTexture("ar_combo_c");
    m_AreaMenuTexture[8] = CTextureManager::GetRwTexture("ar_combo_arrow");
};


void CAreaMenu_Container::AreaMenuDraw_Sub(void)
{
    if (!m_bTextureSettingFlag)
        return;

    AreaMenuDisp();
};


void CAreaMenu_Container::AreaInfoMenuDraw_Sub(void)
{
    if (!m_bTextureSettingFlag)
        return;

    AREAID::VALUE idAreaCur = CGameData::Record().Area().GetCurrentSelectedArea();

    if (idAreaCur == AREAID::ID_MNY_STN)
        idAreaCur = AREAID::HOME;

    if (idAreaCur < AREAID::NORMALMAX)
    {
        static const RwV2d s_aWindowPos[] =
        {
            { -296.0f, -194.0f, },
            { -150.0f, -194.0f, },
            { -296.0f, 24.0f,   },
            { -150.0f, 24.0f,   },
        };

        int32 nWindowNo = CAreaInfo::GetWindowNo(idAreaCur);
    
        RenderStatePush();
        WindowDisp(
            WINDOWTYPE_INFO,
            s_aWindowPos[nWindowNo].x,
            s_aWindowPos[nWindowNo].y,
            320.0f,
            25.0f
        );
        RenderStatePop();

        CSystem2D::PushRenderState();
        
        if (idAreaCur == AREAID::HOME)
        {
			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
            CGameFont::SetRGBA(255, 255, 255, 255);
            CGameFont::Show(
                CAreaInfo::GetDispName(idAreaCur),
                s_aWindowPos[nWindowNo].x + 60.0f,
                s_aWindowPos[nWindowNo].y + 50.0f
            );
            
			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 1.85f));
            CGameFont::SetRGBA(134, 238, 0, 255);

            float fOffsetX = 70.0f;
            float fOffsetY = 70.0f;

            if (CGameData::Record().Antique().GetShopState())
            {
                CGameFont::Show(
                    CGameText::GetText(GAMETEXT(1224)),
                    s_aWindowPos[nWindowNo].x + fOffsetX,
                    s_aWindowPos[nWindowNo].y + fOffsetY
                );
                fOffsetY += 15.0f;
            };

            if (CGameData::Record().Nexus().GetNexusState())
            {
                CGameFont::Show(
                    CGameText::GetText(GAMETEXT(1223)),
                    s_aWindowPos[nWindowNo].x + fOffsetX,
                    s_aWindowPos[nWindowNo].y + fOffsetY
                );
                fOffsetY += 15.0f;
            };

            if (CGameData::Record().Database().GetDatabaseState())
            {
                CGameFont::Show(
                    CGameText::GetText(GAMETEXT(1225)),
                    s_aWindowPos[nWindowNo].x + fOffsetX,
                    s_aWindowPos[nWindowNo].y + fOffsetY
                );
            };
        }
        else
        {
            static wchar s_wszEpisodeBuffer[128];
            static wchar s_wszEpisodeNoBuffer[128];
            
            s_wszEpisodeBuffer[0]   = UTEXT('\0');
            s_wszEpisodeNoBuffer[0] = UTEXT('\0');

            CGameFont::ConvertToUnicode(s_wszEpisodeNoBuffer, CAreaInfo::GetEpisode(idAreaCur));
            
            const wchar* pszText = CGameText::GetText(GAMETEXT(752));    // EPISODE
            CTextData::Sprintf(s_wszEpisodeBuffer, UTEXT("%s %s"), pszText, s_wszEpisodeNoBuffer);
        
			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));

            CGameFont::SetRGBA(255, 170, 0, 255);
            CGameFont::Show(
                s_wszEpisodeBuffer,
                s_aWindowPos[nWindowNo].x + 60.0f,
                s_aWindowPos[nWindowNo].y + 65.0f
            );
            
            CGameFont::SetRGBA(255, 255, 255, 255);
            CGameFont::Show(
                CAreaInfo::GetDispName(idAreaCur),
                s_aWindowPos[nWindowNo].x + 60.0f,
                s_aWindowPos[nWindowNo].y + 85.0f
            );

            if (CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_SEARCH_ANTIQUE))
            {
                ANTIQUEID::VALUE idAntique = CAreaInfo::GetAntiqueID(idAreaCur);
                
                if (!CGameData::Record().Antique().IsAntiqueTaken(idAntique))
                {
                    m_sprite.ResetUV();
                    m_sprite.SetRGBA(255, 255, 255, 255);
                    m_sprite.SetOffset(0.0f, 0.0f);
                    m_sprite.SetTexture(m_AreaMenuTexture[3]);
                    m_sprite.Resize(64.0f, 64.0f);
                    m_sprite.Move(
                        s_aWindowPos[nWindowNo].x + 220.0f,
                        s_aWindowPos[nWindowNo].y + 30.0f
                    );
                    m_sprite.Draw();
                };
            };
        };

        CSystem2D::PopRenderState();
    };
};


void CAreaMenu_Container::AreaMenuDisp(void)
{
    uint32 uAnimDur = uint32(CScreen::Framerate() * 0.2f);
    
    if (m_MenuAnimCount < uAnimDur)
        ++m_MenuAnimCount;

    float fWidth = Math::LinearTween(
        (m_bMenuDispFlag ? 0.0f : 300.0f),
        (m_bMenuDispFlag ? 300.0f : -300.0f),
        float(m_MenuAnimCount),
        float(uAnimDur)
    );
    
    float fHeight = Math::LinearTween(
        (m_bMenuDispFlag ? 0.0f : m_nMenuItemCount * 23.0f),
        (m_bMenuDispFlag ? m_nMenuItemCount * 23.0f : -(m_nMenuItemCount * 23.0f)),
        float(m_MenuAnimCount),
        float(uAnimDur)
    );
    
    uint8 AlphaBasis = uint8(Math::LinearTween(
        (m_bMenuDispFlag ? 0.0f : 255.0f),
        (m_bMenuDispFlag ? 255.0f : -255.0f),
        float(m_MenuAnimCount),
        float(uAnimDur)
    ));

	RenderStatePush();
    WindowDisp(WINDOWTYPE_NORMAL, -296.0f, -194.0f, fWidth, fHeight);
    RenderStatePop();

    CSystem2D::PushRenderState();
    if (m_bMenuDispFlag)
    {
        if (m_SubmenuType == SUBMENUTYPE_NONE)
            m_fCursorRot = InvClamp(m_fCursorRot + 0.1f, 0.0f, 360.0f);

        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, AlphaBasis);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.SetTexture(m_AreaMenuTexture[ 1 ]);
        m_sprite.Move(-253.0f, (m_nCursor * 25.0f) - 145.0f);
        m_sprite.SetRotate(m_fCursorRot);
        m_sprite.ResizeStrict(32.0f, 32.0f);
        m_sprite.DrawRotate();
    };

    for (int32 i = 0; i < m_nMenuItemCount; ++i)
    {
        if (m_nCursor == i)
            CGameFont::SetRGBA(50, 200, 0, AlphaBasis);
        else
            CGameFont::SetRGBA(255, 170, 0, AlphaBasis);
        
        if (m_bMenuDispFlag)
        {
            float fPosX = Math::LinearTween(
                -213.0f - ((i + 1) * 20.0f),
                ((i + 1) * 20.0f),
                float(m_MenuAnimCount),
                float(uAnimDur)
            );
            
			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.15f));
            CGameFont::Show(
                GetAreaMenuName(m_aMenuItemTable[i]),
                fPosX,
                (i * 25.0f) - 129.0f
            );            
        };
    };
    CSystem2D::PopRenderState();

    if (m_SubmenuType != SUBMENUTYPE_NONE)
    {
        RenderStatePush();
        WindowDisp(WINDOWTYPE_NORMAL, -296.0f, -194.0f, fWidth, fHeight);
        RenderStatePop();

        switch (m_SubmenuType)
        {
        case SUBMENUTYPE_WARP:
            MenuDispSub_Station();
            break;

        case SUBMENUTYPE_COMBO:
            MenuDispSub_Combo();
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CAreaMenu_Container::MenuDispSub_Station(void)
{
    uint32 uAnimDur = uint32(CScreen::Framerate() * 0.2f);

    if (m_SubMenuAnimCount < uAnimDur)
        ++m_SubMenuAnimCount;

    float fWidth = Math::LinearTween(
        (m_bSubMenuDispFlag ? 0.0f : 300.0f),
        (m_bSubMenuDispFlag ? 300.0f : -300.0f),
        float(m_SubMenuAnimCount),
        float(uAnimDur)
    );

    float fHeight = Math::LinearTween(
        (m_bSubMenuDispFlag ? 0.0f : m_nSubMenuItemCount * 23.0f),
        (m_bSubMenuDispFlag ? m_nSubMenuItemCount * 23.0f : -(m_nSubMenuItemCount * 23.0f)),
        float(m_SubMenuAnimCount),
        float(uAnimDur)
    );

    uint8 AlphaBasis = uint8(Math::LinearTween(
        (m_bSubMenuDispFlag ? 0.0f : 255.0f),
        (m_bSubMenuDispFlag ? 255.0f : -255.0f),
        float(m_SubMenuAnimCount),
        float(uAnimDur)
    ));

    RenderStatePush();
    WindowDisp(WINDOWTYPE_NORMAL,
        -256.0f,
        (m_nCursor * 25.0f) - 144.0f,
        fWidth,
        fHeight
    );
    RenderStatePop();

    CSystem2D::PushRenderState();
    if (m_bSubMenuDispFlag)
    {
        m_fSubCursorRot = InvClamp(m_fSubCursorRot + 0.1f, 0.0f, 360.0f);

        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, AlphaBasis);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.SetTexture(m_AreaMenuTexture[1]);
        m_sprite.Move(
            -213.0f,
            (m_nSubCursor * 25.0f) + (m_nCursor * 25.0f) - 95.0f
        );
        m_sprite.SetRotate(m_fSubCursorRot);
        m_sprite.ResizeStrict(32.0f, 32.0f);
        m_sprite.DrawRotate();
    };

    for (int32 i = 0; i < m_nSubMenuItemCount; ++i)
    {
        if (m_nSubCursor == i)
            CGameFont::SetRGBA(50, 200, 0, AlphaBasis);
        else
            CGameFont::SetRGBA(255, 170, 0, AlphaBasis);

        if (m_bSubMenuDispFlag)
        {
            float fPosX = Math::LinearTween(
                -173.0f - ((i + 1) * 20.0f),
                ((i + 1) * 20.0f),
                float(m_SubMenuAnimCount),
                float(uAnimDur)
            );

			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.15f));
            
            CGameFont::Show(
                GetStationName(m_aSubMenuItemTable[i]),
                fPosX,
                (m_nCursor * 25.0f) + (i * 25.0f) - 79.0f
            );
        };
    };
    CSystem2D::PopRenderState();
};


void CAreaMenu_Container::MenuDispSub_Combo(void)
{    
    uint32 uAnimDur = uint32(CScreen::Framerate() * 0.2f);

    if (m_SubMenuAnimCount < uAnimDur)
        ++m_SubMenuAnimCount;

    float fWidth = Math::LinearTween(
        (m_bSubMenuDispFlag ? 0.0f : 400.0f),
        (m_bSubMenuDispFlag ? 400.0f : -400.0f),
        float(m_SubMenuAnimCount),
        float(uAnimDur)
    );

    float fHeight = Math::LinearTween(
        (m_bSubMenuDispFlag ? 0.0f : 221.0f),
        (m_bSubMenuDispFlag ? 221.0f : -221.0f),
        float(m_SubMenuAnimCount),
        float(uAnimDur)
    );

    uint8 AlphaBasis = uint8(Math::LinearTween(
        (m_bSubMenuDispFlag ? 0.0f : 255.0f),
        (m_bSubMenuDispFlag ? 255.0f : -255.0f),
        float(m_SubMenuAnimCount),
        float(uAnimDur)
    ));

    RenderStatePush();
    WindowDisp(WINDOWTYPE_NORMAL,
        -276.0f,
        -174.0f,
        fWidth,
        fHeight
    );
    RenderStatePop();

    CSystem2D::PushRenderState();
    if (m_bSubMenuDispFlag)
    {
        m_fSubCursorRot = InvClamp(m_fSubCursorRot + 0.1f, 0.0f, 360.0f);

        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, AlphaBasis);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.SetTexture(m_AreaMenuTexture[1]);
        m_sprite.Move(
            -233.0f,
            (m_nSubCursor * 25.0f) - 125.0f
        );
        m_sprite.SetRotate(m_fSubCursorRot);
        m_sprite.ResizeStrict(32.0f, 32.0f);
        m_sprite.DrawRotate();

        m_sprite.SetRGBA(255, 255, 255, AlphaBasis);
        m_sprite.SetTexture(m_AreaMenuTexture[4]);
        m_sprite.Move(
            127.0f,
            -125.0f
        );
        m_sprite.Resize(64.0f, 32.0f);
        m_sprite.Draw();
    };


    int32 nPageElements = (m_nSubCursorPage ? COMBOPAGEMAX - 1 : COMBOPAGEMAX);
    int32 nComboNameText = (m_nSubCursorPage ? GAMETEXT(1204) : GAMETEXT(1197));
    
    for (int32 i = 0; i < nPageElements; ++i, ++nComboNameText)
    {
        if (m_nSubCursor == i)
            CGameFont::SetRGBA(50, 200, 0, AlphaBasis);
        else
            CGameFont::SetRGBA(128, 128, 128, AlphaBasis);

        if (m_bSubMenuDispFlag)
        {
            float fPosX = Math::LinearTween(
                -193.0f - ((i + 1) * 20.0f),
                ((i + 1) * 20.0f),
                float(m_SubMenuAnimCount),
                float(uAnimDur)
            );

			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.15f));
            
            if (IsComboMenuSecret(i + (COMBOPAGEMAX * m_nSubCursorPage)))
                CGameFont::Show(CGameText::GetText(GAMETEXT(nComboNameText)), fPosX, (i * 25.0f) - 109.0f);
            else
                CGameFont::Show(CGameText::GetText(GAMETEXT(901)), fPosX, (i * 25.0f) - 109.0f);            
        };
    };

    enum
    {
        COMBOTYPE_NONE = -1,
        COMBOTYPE_A,
        COMBOTYPE_B,
        COMBOTYPE_C,
        
        COMBOTYPENUM,
    };

    static const int32 s_aComboDescInfo[][5] =
    {
        { COMBOTYPE_A, COMBOTYPE_NONE, COMBOTYPE_NONE, COMBOTYPE_NONE, COMBOTYPE_NONE, },
        { COMBOTYPE_B, COMBOTYPE_NONE, COMBOTYPE_NONE, COMBOTYPE_NONE, COMBOTYPE_NONE, },
        { { COMBOTYPE_NONE }, },
        { { COMBOTYPE_NONE }, },
        { { COMBOTYPE_NONE }, },
        { { COMBOTYPE_NONE }, },
        { { COMBOTYPE_NONE }, },
        { COMBOTYPE_A, COMBOTYPE_A, COMBOTYPE_B, COMBOTYPE_NONE, COMBOTYPE_NONE, },
        { COMBOTYPE_A, COMBOTYPE_A, COMBOTYPE_C, COMBOTYPE_NONE, COMBOTYPE_NONE, },
        { COMBOTYPE_A, COMBOTYPE_A, COMBOTYPE_B, COMBOTYPE_B, COMBOTYPE_NONE, },
        { COMBOTYPE_A, COMBOTYPE_A, COMBOTYPE_B, COMBOTYPE_C, COMBOTYPE_NONE, },
        { COMBOTYPE_A, COMBOTYPE_A, COMBOTYPE_B, COMBOTYPE_B, COMBOTYPE_B, },
        { COMBOTYPE_A, COMBOTYPE_A, COMBOTYPE_B, COMBOTYPE_B, COMBOTYPE_C, },
    };
    
    int32 nComboDescText = (m_nSubCursorPage ? GAMETEXT(1217) : GAMETEXT(1210));
    int32 nSubCursorReal = (m_nSubCursor + (COMBOPAGEMAX * m_nSubCursorPage));
    
    if (m_bSubMenuDispFlag && IsComboMenuSecret(nSubCursorReal))
    {
        if (s_aComboDescInfo[nSubCursorReal][0] == COMBOTYPE_NONE)
        {
            Rt2dBBox bbox;
            bbox.x = -210.0f;
            bbox.y = -370.0f;
            bbox.w = 400.0f;
            bbox.h = 300.0f;

			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 1.65f));
            CGameFont::SetRGBA(0, 128, 200, 255);
            CGameFont::Flow(CGameText::GetText(GAMETEXT(nComboDescText + m_nSubCursor)), &bbox);
        }
        else
        {
            m_sprite.ResetUV();
            m_sprite.SetOffset(0.0f, 0.0f);
            m_sprite.SetRGBA(255, 255, 255, 255);

            float fPosX = -220.0f;
            
            for (int32 i = 0; i < COUNT_OF(s_aComboDescInfo[0]); ++i)
            {
                int32 nComboType = s_aComboDescInfo[nSubCursorReal][i];
                
                if(nComboType != COMBOTYPE_NONE)
                {
                    m_sprite.SetTexture(m_AreaMenuTexture[nComboType + 5]);
                    m_sprite.Resize(64.0f, 64.0f);
                    m_sprite.Move(fPosX, 50.0f);
                    m_sprite.Draw();
                    
                    fPosX += 80.0f;
                };
            };

            fPosX = -220.0f;
            
            for (int32 i = 1; i < COUNT_OF(s_aComboDescInfo[0]); ++i)
            {
                int32 nComboType = s_aComboDescInfo[nSubCursorReal][i];

                if (nComboType != COMBOTYPE_NONE && nComboType < COMBOTYPENUM)
                {
                    m_sprite.SetTexture(m_AreaMenuTexture[8]);
                    m_sprite.Resize(32.0f, 32.0f);
                    m_sprite.Move(fPosX + 55.0f, 68.0f);
                    m_sprite.Draw();

                    fPosX += 80.0f;
                };
            };
        };
    };

    CSystem2D::PopRenderState();
};


bool CAreaMenu_Container::IsComboMenuSecret(int32 nSubCursor)
{
    ASSERT(nSubCursor >= 0 && nSubCursor < COMBOMAX);
    
    bool bResult = false;
    int32 nCryRedNum    = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_RED);
    int32 nCryGreenNum  = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_GREEN);
    int32 nCryOrangeNum = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_ORANGE);
    int32 nCryWhiteNum  = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_WHITE);
    
    switch (nSubCursor)
    {
    case COMBO_CHARGE2:
        if (nCryOrangeNum >= 10)
            bResult = true;        
        break;
        
    case COMBO_CHARGE3:
        if (nCryOrangeNum >= 20)
            bResult = true;
        break;
        
    case COMBO_JUMPKNIFE:
        if (nCryWhiteNum >= 20)
            bResult = true;
        break;
        
    case COMBO_GUARDIMPACT:
        if (nCryGreenNum >= 10)
            bResult = true;        
        break;
    
    case COMBO_AAC:
        if (nCryRedNum >= 10)
            bResult = true;
        break;
        
    case COMBO_AABB:
        if (nCryRedNum >= 20)
            bResult = true;
        break;
        
    case COMBO_AABC:
        if (nCryGreenNum >= 20)
            bResult = true;        
        break;
        
    case COMBO_AABBB:
        //
        //  Aerial combo
        //
        if (nCryWhiteNum == 30)
            bResult = true;
        break;
        
    case COMBO_AABBC:
        //
        //  Charge combo
        //
        if (nCryOrangeNum == 30)
            bResult = true;
        break;

    default:
        bResult = true;
        break;
    };
    
    return bResult;
};


void CAreaMenu_Container::WindowDisp(WINDOWTYPE type, float x, float y, float w, float h)
{
    static const int32 WINDOWPARTNUM = 9;
    
    static const Rt2dBBox s_aWindowUV[] =
    {
        // u0            v0          u1          v1
        { 0.0f,         0.0f,       0.5f,       0.5f,       },
        { 0.5f,         0.0f,       1.0f,       0.5f,       },
        { 0.0f,         0.5f,       0.5f,       1.0f,       },
        { 0.5f,         0.5f,       1.0f,       1.0f,       },
        { 0.4453125f,   0.0f,       0.546875f,  0.5f,       },
        { 0.0f,         0.4453125f, 0.5f,       0.546875f,  },
        { 0.5f,         0.4453125f, 1.0f,       0.546875f,  },
        { 0.4453125f,   0.5f,       0.546875f,  1.0f,       },
        { 0.390625f,    0.359375f,  0.640625f,  0.609375f,  },
    };

    static_assert(COUNT_OF(s_aWindowUV) == WINDOWPARTNUM, "update me");

    ASSERT(type >= 0 && type < WINDOWTYPENUM);
    
    m_sprite.ResetUV();
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetTexture(m_AreaMenuTexture[type == WINDOWTYPE_NORMAL ? 0 : 2 ]);
    m_sprite.SetOffset(0.0f, 0.0f);
    
    for (int32 i = 0; i < WINDOWPARTNUM; ++i)
    {
        float fOffsetX = 0.0f;
        float fOffsetY = 0.0f;
        float fSizeW = 0.0f;
        float fSizeH = 0.0f;

        switch (i)
        {
        case 0:
            fOffsetX = 0.0f;
            fOffsetY = 0.0f;
            fSizeW = 64.0f;
            fSizeH = 64.0f;
            break;

        case 1:
            fOffsetX = w + 64.0f;
            fOffsetY = 0.0f;
            fSizeW = 64.0f;
            fSizeH = 64.0f;
            break;

        case 2:
            fOffsetX = 0.0f;
            fOffsetY = h + 64.0f;
            fSizeW = 64.0f;
            fSizeH = 64.0f;
            break;

        case 3:
            fOffsetX = w + 64.0f;
            fOffsetY = h + 64.0f;
            fSizeW = 64.0f;
            fSizeH = 64.0f;
            break;

        case 4:
            fOffsetX = 64.0f;
            fOffsetY = 0.0f;
            fSizeW = w;
            fSizeH = 64.0f;
            break;

        case 5:
            fOffsetX = 0.0f;
            fOffsetY = 64.0f;
            fSizeW = 64.0f;
            fSizeH = h;
            break;

        case 6:
            fOffsetX = w + 64.0f;
            fOffsetY = 64.0f;
            fSizeW = 64.0f;
            fSizeH = h;
            break;

        case 7:
            fOffsetX = 64.0f;
            fOffsetY = h + 64.0f;
            fSizeW = w;
            fSizeH = 64.0f;
            break;

        case 8:
            fOffsetX = 64.0f;
            fOffsetY = 64.0f;
            fSizeW = w;
            fSizeH = h;
            break;

        default:
            ASSERT(false);
            break;
        };

        m_sprite.Move(x + fOffsetX, y + fOffsetY);
        m_sprite.Resize(fSizeW, fSizeH);
        m_sprite.SetUV(
            s_aWindowUV[i].x,
            s_aWindowUV[i].y,
            s_aWindowUV[i].w,
            s_aWindowUV[i].h
        );
        m_sprite.Draw();
    };
};


const wchar* CAreaMenu_Container::GetAreaMenuName(int32 nMenuNo)
{
    static const GAMETEXT s_aAreaMenuGametext[] =
    {
        GAMETEXT(741),
        GAMETEXT(742),
        GAMETEXT(744),
        GAMETEXT(745),
        GAMETEXT(746),
        GAMETEXT(747),
        GAMETEXT(748),
        GAMETEXT(749),
    };

    static_assert(COUNT_OF(s_aAreaMenuGametext) == MAINMAX, "update me");

    //ASSERT(nMenuNo >= 0 && nMenuNo < m_nMenuItemCount);
    ASSERT(nMenuNo >= 0 && nMenuNo < MAINMAX);
    ASSERT(nMenuNo >= 0 && nMenuNo < COUNT_OF(s_aAreaMenuGametext));

    return CGameText::GetText(s_aAreaMenuGametext[nMenuNo]);
};


const wchar* CAreaMenu_Container::GetStationName(int32 nWarpNo)
{
    ASSERT(m_SubmenuType == SUBMENUTYPE_WARP);
    ASSERT(nWarpNo >= 0 && nWarpNo < WARPMAX);

    switch (nWarpNo)
    {
    case WARP_NY:
        return CGameText::GetText(GAMETEXT(89));
        
    case WARP_DHO:
        return CGameText::GetText(GAMETEXT(90));
        
    case WARP_TRI:
        return CGameText::GetText(GAMETEXT(91));
        
    case WARP_JPN:
        return CGameText::GetText(GAMETEXT(92));
        
    case WARP_FNY:
        if (CGameData::Record().Area().GetAreaState(AREAID::ID_AREA50) == CAreaRecord::STATE_CLEAR)
            return CGameText::GetText(GAMETEXT(95));
        else
            return CGameText::GetText(GAMETEXT(93));

    case WARP_KUR:
        return CGameText::GetText(GAMETEXT(94));

    default:
        ASSERT(false);
        return nullptr;
    };
};


void CAreaMenu_Container::RenderStatePush(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEGOURAUD);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
};


void CAreaMenu_Container::RenderStatePop(void)
{
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATESHADEMODE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREFILTER);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREPERSPECTIVE);
    RENDERSTATE_POP(rwRENDERSTATEBORDERCOLOR);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESS);
};


static CAreaMenu_Container* s_pAreaMenuContainer = nullptr;


static inline CAreaMenu_Container& AreaMenuContaienr(void)
{
    ASSERT(s_pAreaMenuContainer);
    return *s_pAreaMenuContainer;
};


/*static*/ void CAreaMenu::Initialize(void)
{
    ASSERT(!s_pAreaMenuContainer);
    
    if (!s_pAreaMenuContainer)
        s_pAreaMenuContainer = new CAreaMenu_Container;
};


/*static*/ void CAreaMenu::Terminate(void)
{
    ASSERT(s_pAreaMenuContainer);
    
    if (s_pAreaMenuContainer)
    {
        delete s_pAreaMenuContainer;
        s_pAreaMenuContainer = nullptr;
    };
};


/*static*/ void CAreaMenu::AreaMenuInit(void)
{
    AreaMenuContaienr().AreaMenuInit_Sub();
};


/*static*/ AREATYPES::NEXTSEQUENCE CAreaMenu::AreaMenuSelect(void)
{
    return AreaMenuContaienr().AreaMenuSelect_Sub();
};


/*static*/ void CAreaMenu::AreaMenuStationWarp(void)
{
    AreaMenuContaienr().AreaMenuStationWarp_Sub();
};


/*static*/ void CAreaMenu::AreaMenuPeriod(void)
{
    AreaMenuContaienr().AreaMenuPeriod_Sub();
};


/*static*/ void CAreaMenu::AreaMenuDraw(void)
{
    AreaMenuContaienr().AreaMenuDraw_Sub();
};


/*static*/ void CAreaMenu::AreaInfoMenuDraw(void)
{
    AreaMenuContaienr().AreaInfoMenuDraw_Sub();
};

