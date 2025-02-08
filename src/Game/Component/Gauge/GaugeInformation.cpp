#include "GaugeInformation.hpp"
#include "GaugeAnim.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/MapInfo.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/TextData.hpp"


class CGaugeInformation_Container
{
private:
    using PAUSESTATUS = CGaugeInformation::PAUSESTATUS;

    struct BOSSGAUGEINFO
    {
        enum STATE
        {
            STATE_INIT = 0,
            STATE_IDLE,
            STATE_DAMAGE,
            STATE_RECOVER,
        };

        ENEMYID::VALUE m_IdEnemy;
        STATE   m_eState;
        int32   m_nHp;
        int32   m_nHpOld;
        int32   m_nHpTotal;
        int32   m_nHpMove;
        uint32  m_uAnimCnt;
        bool    m_bEnable;
    };

    enum PAUSERESULT
    {
        PAUSERESULT_BACK = 0,
        PAUSERESULT_INFO,
        PAUSERESULT_RET_WORLD,
        PAUSERESULT_RET_TITLE,
        
        PAUSERESULTNUM,
    };

    enum NEXUSSTEP
    {
        NEXUSSTEP_ENEMY_IN = 0,
        NEXUSSTEP_ENEMY_WAIT,
        NEXUSSTEP_ENEMY_OUT,
        NEXUSSTEP_STEADY,
        NEXUSSTEP_FIGHT,
        NEXUSSTEP_END,
        
        NEXUSSTEP_NUM,
    };

public:
    CGaugeInformation_Container(void);
    ~CGaugeInformation_Container(void);
    void DispInitSub(void);
    void PausePeriodSub(void);
    void PauseDrawSub(void);
    PAUSESTATUS GetPauseStatusSub(void);
    void MissionInfoSetSub(STAGEID::VALUE idStage);
    void MissionPeriodSub(void);
    void MissionInfoDrawSub(void);
    void SetBattleNexusInfoSub(void);
    bool IsBattleNexusInfoEndSub(void);
    void DispBattleNexusInfoSub(void);
    void BossGaugeEnableSub(bool bEnable, int32 no, ENEMYID::VALUE idEnemy);
    void BossGaugeSetSub(int32 no, int32 hp);
    void BossGaugeDrawSub(int32 no);
    void InfoSettings(void);
    void InfoDispBase(bool bFullScreen);
    void InfoDispRenderStatePush(void);
    void InfoDispRenderStatePop(void);

private:
    CSprite         m_sprite;
    RwTexture*      m_pTextureFont;
    RwTexture*      m_pTextureGGBar;
    RwTexture*      m_pTextureGGIconSel1;
    RwTexture*      m_pTextureGGIconOff;
    RwTexture*      m_pTextureGGIconOn;
    RwTexture*      m_pTextureBtReady;
    RwTexture*      m_pTextureBtFight;
    RwTexture*      m_pTextureGGEnemyGauge;
    RwTexture*      m_pTextureGGEnemyBG;
    MAPID::VALUE    m_idMap;
    bool            m_bMapSetFlag;
    bool            m_bTextureInitedFlag;
    bool            m_bShowInformationFlag;
    PAUSESTATUS     m_pausestatus;
    float           m_fCursorRot;
    bool            m_bCursorAnimFlag;
    uint32          m_uCursorAnimCount;
    int32           m_nCursor;
    CDialog*        m_pConfirmDlg;
    bool            m_bDlgRunFlag;
    BOSSGAUGEINFO   m_aBossGaugeInfo[2];
    uint32          m_uNexusAnimCnt;
    NEXUSSTEP       m_eNexusStep;
    bool            m_bNexusDispFlag;
};


CGaugeInformation_Container::CGaugeInformation_Container(void)
: m_idMap(MAPID::ID_NONE)
, m_bMapSetFlag(false)
, m_bTextureInitedFlag(false)
, m_bShowInformationFlag(false)
, m_pausestatus(CGaugeInformation::PAUSESTATUS_INIT)
, m_fCursorRot(0.0f)
, m_bCursorAnimFlag(false)
, m_uCursorAnimCount(0)
, m_nCursor(0)
, m_pConfirmDlg(nullptr)
, m_bDlgRunFlag(false)
, m_aBossGaugeInfo()
, m_uNexusAnimCnt(0)
, m_eNexusStep(NEXUSSTEP_ENEMY_IN)
, m_bNexusDispFlag(false)
{
    std::memset(m_aBossGaugeInfo, 0x00, sizeof(m_aBossGaugeInfo));

    m_pConfirmDlg = new CDialog(CDialog::COLOR_NORMAL, CDialog::STATUS_NO, CGameData::Attribute().GetVirtualPad());

    m_pConfirmDlg->Set(0.0f, 105.0f, 640.0f, 140.0f);
    m_pConfirmDlg->SetOpenAction(true);
    m_pConfirmDlg->SetStatus(CDialog::STATUS_NO);
    m_pConfirmDlg->SetText(
		CGameText::GetText(GAMETEXT(758)), 
		CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f),
		{ 0xFF, 0xFF, 0xFF, 0xFF }
	);
    
    m_bDlgRunFlag = false;
};


CGaugeInformation_Container::~CGaugeInformation_Container(void)
{
    if (m_pConfirmDlg)
    {
        delete m_pConfirmDlg;
        m_pConfirmDlg = nullptr;
    };
};


void CGaugeInformation_Container::DispInitSub(void)
{
    m_nCursor = 0;
    m_fCursorRot = 0.0f;
    m_pausestatus = CGaugeInformation::PAUSESTATUS_INIT;
    m_bShowInformationFlag = false;
};


void CGaugeInformation_Container::PausePeriodSub(void)
{
    InfoSettings();

    if (!m_bDlgRunFlag)
    {
        switch (m_pausestatus)
        {
        case CGaugeInformation::PAUSESTATUS_INIT:
            {
                m_pausestatus = CGaugeInformation::PAUSESTATUS_RUN;
            }
            break;

        case CGaugeInformation::PAUSESTATUS_RUN:
            {
                int32 iController = CGameData::Attribute().GetVirtualPad();

                if (m_bShowInformationFlag)
                {
                    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
                        m_bShowInformationFlag = false;                    
                }
                else
                {
                    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
                    {
                        switch (m_nCursor)
                        {
                        case PAUSERESULT_BACK:
                            {
                                m_pausestatus = CGaugeInformation::PAUSESTATUS_BACK;
                            }
                            break;

                        case PAUSERESULT_INFO:
                            {
                                m_bShowInformationFlag = true;
                                m_pausestatus = CGaugeInformation::PAUSESTATUS_RUN;
                            }
                            break;

                        case PAUSERESULT_RET_WORLD:
                        case PAUSERESULT_RET_TITLE:
                            {
                                m_bDlgRunFlag = true;
                                m_pConfirmDlg->Open();
                            }
                            break;

                        default:
                            break;
                        };
                    }
                    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
					{
						CGameSound::PlaySE(SDCODE_SE(4100));
						if (m_nCursor != PAUSERESULT_BACK)
                            m_nCursor = PAUSERESULT_BACK;
                        else
                            m_pausestatus = CGaugeInformation::PAUSESTATUS_BACK;
                    }
                    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP))
					{
						CGameSound::PlaySE(SDCODE_SE(4100));
						m_nCursor = InvClamp(--m_nCursor, 0, PAUSERESULTNUM - 1);
                    }
                    else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
					{
						CGameSound::PlaySE(SDCODE_SE(4100));
						m_nCursor = InvClamp(++m_nCursor, 0, PAUSERESULTNUM - 1);
                    };
                };
            }
            break;

        default:
            break;
        };
    }
    else
    {
        if (!m_pConfirmDlg->IsOpen())
        {
            m_bDlgRunFlag = false;
            switch (m_pConfirmDlg->GetStatus())
            {
            case CDialog::STATUS_YES:
                {
                    switch (m_nCursor)
                    {
                    case PAUSERESULT_RET_WORLD:
                        m_pausestatus = CGaugeInformation::PAUSESTATUS_RET_WORLD;
                        break;
                        
                    case PAUSERESULT_RET_TITLE:
                        m_pausestatus = CGaugeInformation::PAUSESTATUS_RET_TITLE;
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
        };
    };
};


void CGaugeInformation_Container::PauseDrawSub(void)
{
    if (!m_bTextureInitedFlag)
        return;

    InfoDispRenderStatePush();
    InfoDispBase(true);

    CSystem2D::PushRenderState();

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 3.75f));
    CGameFont::SetRGBA(255, 170, 0, 255);
    CGameFont::Show(CGameText::GetText(GAMETEXT(908)), -220.0f, -100.0f);

    float fDuration = GAUGE_ANIM_DURATION_FRAMES(30);
    float fStartValue = (m_bCursorAnimFlag ? 255.0f : 0.0f);
    float fEndValue = (m_bCursorAnimFlag ? -255.0f : 255.0f);
    uint8 uAlphaBasis = uint8(Math::LinearTween(fStartValue, fEndValue, float(m_uCursorAnimCount), fDuration));

    if (m_uCursorAnimCount >= uint32(fDuration))
    {
        m_bCursorAnimFlag = !m_bCursorAnimFlag;
        m_uCursorAnimCount = 0;
    }
    else
    {
        ++m_uCursorAnimCount;
    };

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

    CGameFont::SetRGBA(255, 170, 0, uAlphaBasis);
    CGameFont::Show(CGameText::GetText(GAMETEXT(908)), -220.0f, -100.0f);
    
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::SetRGBA(255, 255, 255, 255);    
    CGameFont::Show(CGameText::GetText(GAMETEXT(m_bShowInformationFlag ? 8 : 9)), 50.0f, -85.0f);

    m_sprite.Resize(float(CScreen::Width()), 16.0f);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetTexture(m_pTextureGGBar);
    m_sprite.Move(0.0f, -68.0f);
    m_sprite.Draw();

    if (m_bShowInformationFlag)
    {
        if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
        {
			CGameFont::SetHeight(CGameFont::GetScreenHeight() * 2.0f);
            CGameFont::SetRGBA(255, 170, 0, 255);
            CGameFont::Show(CGameText::GetText(GAMETEXT(933)), -220.0f, -50.0f);

            float fOfsY = -30.0f;
			CGameFont::SetHeight(CGameFont::GetScreenHeight() * 1.85f);
            CGameFont::SetRGBA(255, 255, 255, 255);
            
            if (CGameData::PlayParam().GetArea() == AREAID::ID_AREA22 ||
                CGameData::PlayParam().GetArea() == AREAID::ID_AREA32)
            {
                CGameFont::Show(CGameText::GetText(GAMETEXT(935)), -200.0f, -30.0f);
                CGameFont::Show(CGameText::GetText(GAMETEXT(936)), -200.0f, -10.0f);
                fOfsY = 10.0f;
            };

            CGameFont::Show(CGameText::GetText(GAMETEXT(937)), -200.0f, fOfsY);
            fOfsY += 20.0f;
            
            CGameFont::Show(CGameText::GetText(GAMETEXT(938)), -200.0f, fOfsY);
            fOfsY += 20.0f;
            
            if (CGameData::PlayParam().GetArea() == AREAID::ID_AREA22 ||
                CGameData::PlayParam().GetArea() == AREAID::ID_AREA32)
            {
                CGameFont::Show(CGameText::GetText(GAMETEXT(939)), -200.0f, fOfsY);
                fOfsY += 20.0f;
            };

            CGameFont::Show(CGameText::GetText(GAMETEXT(940)), -200.0f, fOfsY);
            fOfsY += 20.0f;
            
            CGameFont::Show(CGameText::GetText(GAMETEXT(941)), -200.0f, fOfsY);
            fOfsY += 20.0f;
            
            CGameFont::Show(CGameText::GetText(GAMETEXT(942)), -200.0f, fOfsY);
            fOfsY += 20.0f;
            
            CGameFont::Show(CGameText::GetText(GAMETEXT(943)), -200.0f, fOfsY);
        }
        else
        {
			CGameFont::SetHeight(CGameFont::GetScreenHeight() * 2.0f);
            CGameFont::SetRGBA(255, 170, 0, 255);
            CGameFont::Show(CGameText::GetText(GAMETEXT(911)), -220.0f, -50.0f);

            CGameFont::SetRGBA(255, 255, 255, 255);
            CGameFont::Show(CGameText::GetText(GAMETEXT(786)), -200.0f, -25.0f);
            CGameFont::Show(CGameText::GetText(GAMETEXT(787)), -200.0f, 35.0f);
            CGameFont::Show(CGameText::GetText(GAMETEXT(788)), 50.0f, -25.0f);
            CGameFont::Show(CGameText::GetText(GAMETEXT(789)), 50.0f, 35.0f);

            m_sprite.SetOffset(0.0f, 0.0f);
            m_sprite.Resize(64.0f, 64.0f);

            for (int32 i = 0; i < 4; ++i)
            {
                int32 nCryNum = 0;
                
                switch (i)
                {
                case 0:
                    nCryNum = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_RED);
                    break;

                case 1:
                    nCryNum = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_GREEN);
                    break;

                case 2:
                    nCryNum = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_ORANGE);
                    break;

                case 3:
                    nCryNum = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_WHITE);
                    break;

                default:
                    break;
                };

                int32 idx = 0;
                for (int32 j = 10; j < 40; j += 10)
                {
                    if (nCryNum < j)
                        m_sprite.SetTexture(m_pTextureGGIconOff);
                    else
                        m_sprite.SetTexture(m_pTextureGGIconOn);

                    float x = 0.0f;
                    float y = 0.0f;
                    
                    if (i >= 2)
                    {
                        x = float(idx) * 48.0f + 50.0f;
                        y = float(i - 2) * 60.0f - 25.0f;
                    }
                    else
                    {
                        x = float(idx) * 48.0f - 200.0f;
                        y = float(i) * 60.0f - 25.0f;
                    };

                    m_sprite.Move(x, y);
                    m_sprite.Draw();

                    ++idx;
                };
            };
        };
    }
    else
    {
        m_sprite.Move(0.0f, 45.0f);
        m_sprite.Draw();

        m_fCursorRot += 0.1f;
        if (m_fCursorRot >= 360.0f)
            m_fCursorRot -= 360.0f;

        m_sprite.SetTexture(m_pTextureGGIconSel1);
        m_sprite.SetOffset(0.0f, 0.0f);
        m_sprite.ResizeStrict(19.0f, 19.0f);
        m_sprite.Move(-220.0f, m_nCursor * 25.0f - 58.0f);
        m_sprite.SetRotate(m_fCursorRot);
        m_sprite.DrawRotate();

        for (int32 i = 0; i < PAUSERESULTNUM; ++i)
        {
            const wchar* pwszText = nullptr;
            
            switch (i)
            {
            case PAUSERESULT_BACK:
                pwszText = CGameText::GetText(GAMETEXT(749));
                break;

            case PAUSERESULT_INFO:
                pwszText = CGameText::GetText(GAMETEXT(912));
                break;

            case PAUSERESULT_RET_WORLD:
                if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_NEXUS)
                    pwszText = CGameText::GetText(GAMETEXT(923));
                else
                    pwszText = CGameText::GetText(GAMETEXT(913));
                break;
                
            case PAUSERESULT_RET_TITLE:
                pwszText = CGameText::GetText(GAMETEXT(914));
                break;

            default:
                break;
            };

            if (m_nCursor == i)
                CGameFont::SetRGBA(50, 200, 0, 255);
            else
                CGameFont::SetRGBA(255, 170, 0, 255);

			CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
            CGameFont::Show(pwszText, -200.0f, i * 25.0f - 50.0f);
        };

		CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 1.65f));
        CGameFont::SetRGBA(0, 128, 200, 255);

        Rt2dBBox bbox = { -220.0f, -350.0f, 440.0f, 300.0f };
        if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_NEXUS)
        {
            const wchar* pwszText = nullptr;
            AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
            
            switch (idArea)
            {
            case AREAID::ID_AREA60_A:
                pwszText = CGameText::GetText(GAMETEXT(878));
                break;

            case AREAID::ID_AREA60_B:
                pwszText = CGameText::GetText(GAMETEXT(879));
                break;

            case AREAID::ID_AREA60_C:
                pwszText = CGameText::GetText(GAMETEXT(880));
                break;
                
            case AREAID::ID_AREA60_D:
                pwszText = CGameText::GetText(GAMETEXT(881));
                break;

            default:
                ASSERT(false);
                break;
            };

            CGameFont::Flow(pwszText, &bbox);
        }
        else
        {
            AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
            CGameFont::Flow(CGameText::GetText(GAMETEXT(idArea + 823)), &bbox);
        };
    };

    CSystem2D::PopRenderState();
};


CGaugeInformation_Container::PAUSESTATUS CGaugeInformation_Container::GetPauseStatusSub(void)
{
    return m_pausestatus;
};


void CGaugeInformation_Container::MissionInfoSetSub(STAGEID::VALUE idStage)
{
    m_idMap = CStageInfo::GetMapID(idStage);
    m_bMapSetFlag = true;
};


void CGaugeInformation_Container::MissionPeriodSub(void)
{
    InfoSettings();
};


void CGaugeInformation_Container::MissionInfoDrawSub(void)
{
    if (!m_bMapSetFlag)
        return;

    m_bMapSetFlag = false;

    InfoDispRenderStatePush();
    InfoDispBase(false);

    CSystem2D::PushRenderState();

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::SetRGBA(255, 170, 0, 255);

    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_NEXUS)
    {
        CGameFont::Show(CGameText::GetText(GAMETEXT(929)), -240.0f, -140.0f);
    }
    else
    {
        CGameFont::Show(CGameText::GetText(GAMETEXT(931)), -240.0f, -140.0f);
        
        float fStrW = CGameFont::GetStringWidth(CGameText::GetText(GAMETEXT(931)));
        CGameFont::Show(CAreaInfo::GetEpisode(CGameData::PlayParam().GetArea()), fStrW - 240.0f, -140.0f);
    };

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.5f));
    CGameFont::SetRGBA(255, 170, 0, 255);
    CGameFont::Show(CAreaInfo::GetDispName(CGameData::PlayParam().GetArea()), -230.0f, -110.0f);

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::SetRGBA(202, 0, 103, 255);
    CGameFont::Show(CGameText::GetText(GAMETEXT(932)), -230.0f, -80.0f);

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::SetRGBA(202, 0, 103, 255);
    
    Rt2dBBox bbox = { -220.0f, -230.0f, 440.0f, 300.0f };
    const wchar* pwszText = nullptr;
    
    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_NEXUS)
    {
        switch (CGameData::PlayParam().GetArea())
        {
        case AREAID::ID_AREA60_A:
            pwszText = CGameText::GetText(GAMETEXT(878));
            break;
            
        case AREAID::ID_AREA60_B:
            pwszText = CGameText::GetText(GAMETEXT(879));
            break;
            
        case AREAID::ID_AREA60_C:
            pwszText = CGameText::GetText(GAMETEXT(880));
            break;
            
        case AREAID::ID_AREA60_D:
            pwszText = CGameText::GetText(GAMETEXT(881));
            break;
            
        default:
            ASSERT(false);
            break;
        };
    }
    else
    {
        pwszText = CGameText::GetText(GAMETEXT(823 + CGameData::PlayParam().GetArea()));        
    };

    ASSERT(pwszText);
    CGameFont::Flow(pwszText, &bbox);

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::SetRGBA(255, 255, 255, 255);
    CGameFont::Show(CGameText::GetText(GAMETEXT(1226)), -230.0f, 135.0f);

    CSystem2D::PopRenderState();
};


void CGaugeInformation_Container::SetBattleNexusInfoSub(void)
{
    m_uNexusAnimCnt = 0;
    m_eNexusStep = NEXUSSTEP_ENEMY_IN;
    m_bNexusDispFlag = true;
};


bool CGaugeInformation_Container::IsBattleNexusInfoEndSub(void)
{
    return (m_eNexusStep >= NEXUSSTEP_FIGHT);
};


void CGaugeInformation_Container::DispBattleNexusInfoSub(void)
{
    if (!m_bTextureInitedFlag)
        return;

    if (!m_bNexusDispFlag)
        return;

	InfoDispRenderStatePush();
    
    float SpriteW = 512.0f;
    float SpriteH = 128.0f;
    uint8 Alpha = 255;

    switch (m_eNexusStep)
    {
    case NEXUSSTEP_ENEMY_IN:
        {
            float fDuration = GAUGE_ANIM_DURATION_FRAMES(15);

            Alpha = uint8(Math::LinearTween(0.0f, 255.0f, float(m_uNexusAnimCnt), fDuration));

            if (float(m_uNexusAnimCnt) >= fDuration)
            {
                m_eNexusStep = NEXUSSTEP_ENEMY_WAIT;
                m_uNexusAnimCnt = 0;
            }
            else
            {
                ++m_uNexusAnimCnt;
            };
        }
        break;

    case NEXUSSTEP_ENEMY_WAIT:
        {
            float fDuration = GAUGE_ANIM_DURATION_FRAMES(30);

            if (float(m_uNexusAnimCnt) >= fDuration)
            {
                m_eNexusStep = NEXUSSTEP_ENEMY_OUT;
                m_uNexusAnimCnt = 0;
            }
            else
            {
                ++m_uNexusAnimCnt;
            };
        }
        break;

    case NEXUSSTEP_ENEMY_OUT:
        {
            float fDuration = GAUGE_ANIM_DURATION_FRAMES(15);

            Alpha = uint8(Math::LinearTween(255.0f, -255.0f, float(m_uNexusAnimCnt), fDuration));

            if (float(m_uNexusAnimCnt) >= fDuration)
            {
                m_eNexusStep = NEXUSSTEP_STEADY;                
                m_uNexusAnimCnt = 0;
            }
            else
            {
                ++m_uNexusAnimCnt;
            };
        }
        break;

    case NEXUSSTEP_STEADY:
        {
            float fDuration = GAUGE_ANIM_DURATION_FRAMES(30);

            Alpha = uint8(Math::LinearTween(255.0f, -255.0f, float(m_uNexusAnimCnt), fDuration));
            SpriteW = Math::LinearTween(512.0f, -512.0f, float(m_uNexusAnimCnt), fDuration);
            SpriteH = Math::LinearTween(128.0f, -128.0f, float(m_uNexusAnimCnt), fDuration);

            if (float(m_uNexusAnimCnt) >= fDuration)
            {
                m_eNexusStep = NEXUSSTEP_FIGHT;
                m_uNexusAnimCnt = 0;
            }
            else
            {
                ++m_uNexusAnimCnt;
            };
        }
        break;

    case NEXUSSTEP_FIGHT:
        {
            float fDuration = (CScreen::Framerate());

            Alpha = uint8(Math::LinearTween(255.0f, -255.0f, float(m_uNexusAnimCnt), fDuration));
            SpriteW = Math::LinearTween(0.0f, 512.0f, float(m_uNexusAnimCnt), fDuration);
            SpriteH = Math::LinearTween(0.0f, 128.0f, float(m_uNexusAnimCnt), fDuration);

            if (float(m_uNexusAnimCnt) >= fDuration)
            {
                m_eNexusStep = NEXUSSTEP_END;
                m_uNexusAnimCnt = 0;
            }
            else
            {
                ++m_uNexusAnimCnt;
            };
        }
        break;

    case NEXUSSTEP_END:
        {
            m_bNexusDispFlag = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if ((m_eNexusStep == NEXUSSTEP_ENEMY_IN)
        || (m_eNexusStep == NEXUSSTEP_ENEMY_WAIT)
        || (m_eNexusStep == NEXUSSTEP_ENEMY_OUT))
    {                
        STAGEID::VALUE IdStage = CGameData::PlayParam().GetStage();
        AREAID::VALUE IdArea = CGameData::PlayParam().GetArea();
        int32 StageIndex = CAreaInfo::IndexOfStage(IdArea, IdStage);
        int32 StringOffset = -1;

        const int32 aStringOffset[] = { 1157, 1167, 1177, 1187 };

#ifdef _DEBUG        
        int32 idx = (IdArea - AREAID::NEXUSSTART);
        ASSERT(idx >= 0);
        ASSERT(idx < COUNT_OF(aStringOffset));;
#endif /* _DEBUG */
        
        StringOffset = aStringOffset[IdArea - AREAID::NEXUSSTART];

        CSystem2D::PushRenderState();
        
        wchar wszBuff[128];
        const wchar* pwszFormat = CGameText::GetText(GAMETEXT(0x39E));
        CTextData::Sprintf(wszBuff, pwszFormat, StageIndex + 1);

		CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.87f));
        CGameFont::SetRGBA(255, 170, 0, Alpha);
        CGameFont::Show(
            wszBuff,
            -(CGameFont::GetStringWidth(wszBuff) * 0.5f),
            -20.0f
        );

        const wchar* pwszText = CGameText::GetText(GAMETEXT(StringOffset + StageIndex));
        CGameFont::SetRGBA(255, 170, 0, Alpha);
        CGameFont::Show(
            pwszText,
            -(CGameFont::GetStringWidth(pwszText) * 0.5f),
            20.0f
        );

        CSystem2D::PopRenderState();
    };

    if ((m_eNexusStep == NEXUSSTEP_STEADY) ||
        (m_eNexusStep == NEXUSSTEP_FIGHT))
    {
        if (m_eNexusStep == NEXUSSTEP_STEADY)
            m_sprite.SetTexture(m_pTextureBtReady);
        else if (m_eNexusStep == NEXUSSTEP_FIGHT)
            m_sprite.SetTexture(m_pTextureBtFight);
        else
            ASSERT(false);

        m_sprite.SetOffset(0.5f, 0.5f);
        m_sprite.Move(0.0f, 0.0f);
        m_sprite.Resize(SpriteW, SpriteH);
        m_sprite.SetRGBA(255, 255, 255, Alpha);
        m_sprite.Draw();
    };
};


void CGaugeInformation_Container::BossGaugeEnableSub(bool bEnable, int32 no, ENEMYID::VALUE idEnemy)
{
    ASSERT(no >= 0);
    ASSERT(no < COUNT_OF(m_aBossGaugeInfo));

    m_aBossGaugeInfo[no].m_IdEnemy = idEnemy;
    m_aBossGaugeInfo[no].m_nHp = 0;
    m_aBossGaugeInfo[no].m_bEnable = bEnable;
	m_aBossGaugeInfo[no].m_eState = BOSSGAUGEINFO::STATE_INIT;
};


void CGaugeInformation_Container::BossGaugeSetSub(int32 no, int32 hp)
{
    ASSERT(no >= 0);
    ASSERT(no < COUNT_OF(m_aBossGaugeInfo));

    m_aBossGaugeInfo[no].m_nHp = hp;
};


void CGaugeInformation_Container::BossGaugeDrawSub(int32 no)
{
    ASSERT(no >= 0);
    ASSERT(no < COUNT_OF(m_aBossGaugeInfo));

    BOSSGAUGEINFO* pBossGaugeInfo = &m_aBossGaugeInfo[no];
    if (!pBossGaugeInfo->m_bEnable)
        return;

	float fOfsY = 30.0f;
	if (!no)
		fOfsY = 0.0f;

    InfoDispRenderStatePush();

    if (pBossGaugeInfo->m_eState)
    {
		if((pBossGaugeInfo->m_eState == BOSSGAUGEINFO::STATE_IDLE) ||
		   (pBossGaugeInfo->m_nHp != pBossGaugeInfo->m_nHpOld))
		{
			int32 HpNow = pBossGaugeInfo->m_nHp;
			int32 HpOld = pBossGaugeInfo->m_nHpOld;

			pBossGaugeInfo->m_nHpMove = pBossGaugeInfo->m_nHpOld;
			pBossGaugeInfo->m_nHpOld = pBossGaugeInfo->m_nHp;

			if (HpOld > HpNow)
			{
				pBossGaugeInfo->m_eState = BOSSGAUGEINFO::STATE_DAMAGE;
				pBossGaugeInfo->m_uAnimCnt = 0;
			}
			else
			{
				pBossGaugeInfo->m_eState = BOSSGAUGEINFO::STATE_RECOVER;
				pBossGaugeInfo->m_uAnimCnt = 0;
			};
		};
    }
    else
    {
        pBossGaugeInfo->m_nHpTotal = pBossGaugeInfo->m_nHp;
        pBossGaugeInfo->m_nHpMove = pBossGaugeInfo->m_nHp;
        pBossGaugeInfo->m_nHpOld = pBossGaugeInfo->m_nHp;
        pBossGaugeInfo->m_eState = BOSSGAUGEINFO::STATE_IDLE;        
	};

    float fDuration = GAUGE_ANIM_DURATION_FRAMES(8);

    float fAnimStep = (static_cast<float>(pBossGaugeInfo->m_uAnimCnt) / fDuration);
    float fHpMove = static_cast<float>(pBossGaugeInfo->m_nHpMove);
    float fHpOld = static_cast<float>(pBossGaugeInfo->m_nHpOld);

    if (pBossGaugeInfo->m_eState == BOSSGAUGEINFO::STATE_RECOVER)
    {
        fHpMove = (fHpMove - ((fHpMove - fHpOld) * fAnimStep));
    }
    else if (pBossGaugeInfo->m_eState == BOSSGAUGEINFO::STATE_DAMAGE)
    {
        fHpMove = (fHpMove + ((fHpOld - fHpMove) * fAnimStep));
    };

    if (pBossGaugeInfo->m_eState > BOSSGAUGEINFO::STATE_IDLE)
    {
        if (static_cast<float>(pBossGaugeInfo->m_uAnimCnt) >= fDuration)
            pBossGaugeInfo->m_eState = BOSSGAUGEINFO::STATE_IDLE;
        else
            ++pBossGaugeInfo->m_uAnimCnt;
    };

	m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetOffset(1.0f, 0.5f);

    float fHpTotal = static_cast<float>(pBossGaugeInfo->m_nHpTotal);

    m_sprite.Resize(541.0f, 32.0f);
    m_sprite.SetTexture(m_pTextureGGEnemyBG);
    m_sprite.Move(270.0f, 192.0f - fOfsY);
    m_sprite.Draw();

    m_sprite.Resize(541.0f, 16.0f);
    m_sprite.SetTexture(m_pTextureGGEnemyGauge);
    m_sprite.SetUV((536.0f - (fHpMove * (536.0f / fHpTotal))) * (1.0f / 536.0f),
                    0.0f,
                    1.0f,
                    1.0f);
    m_sprite.Resize((fHpMove * (536.0f / fHpTotal)), 16.0f);
    m_sprite.Move(268.0f, 191.0f - fOfsY);
    m_sprite.Draw();

    m_sprite.SetUV(0.0f, 0.0f, 1.0f, 1.0f); // restore uv

    ENEMYID::VALUE IdEnemy = pBossGaugeInfo->m_IdEnemy;
    if (!IdEnemy)
    {
        STAGEID::VALUE IdStage = CGameData::PlayParam().GetStage();
        IdEnemy = CStageInfo::GetGaugeEnemyID(IdStage, 0);
    };

    const wchar* pwszEnemyName = ENEMYID::GetDispName(IdEnemy);

    CSystem2D::PushRenderState();
    
    CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::SetRGBA(255, 255, 255, 255);
    CGameFont::Show(pwszEnemyName,
                    268.0f - CGameFont::GetStringWidth(pwszEnemyName),
                    175.0f - fOfsY);
                    
    CSystem2D::PopRenderState();
};


void CGaugeInformation_Container::InfoSettings(void)
{
    if (m_bTextureInitedFlag)
        return;

    m_bTextureInitedFlag = true;

    CTextureManager::SetCurrentTextureSet("gg_clear");
    m_pTextureFont = CTextureManager::GetRwTexture("font");
    m_pTextureGGBar = CTextureManager::GetRwTexture("gg_bar");
    m_pTextureGGIconSel1 = CTextureManager::GetRwTexture("gg_icon_select1");
    m_pTextureGGIconOff = CTextureManager::GetRwTexture("gg_icon_off");
    m_pTextureGGIconOn = CTextureManager::GetRwTexture("gg_icon_on");
    m_pTextureBtReady = CTextureManager::GetRwTexture("bt_ready");
    m_pTextureBtFight = CTextureManager::GetRwTexture("bt_fight");

    ASSERT(m_pTextureFont);
    ASSERT(m_pTextureGGBar);
    ASSERT(m_pTextureGGIconSel1);
    ASSERT(m_pTextureGGIconOff);
    ASSERT(m_pTextureGGIconOn);
    ASSERT(m_pTextureBtReady);
    ASSERT(m_pTextureBtFight);

    CTextureManager::SetCurrentTextureSet("gg_e_guage");
    m_pTextureGGEnemyGauge = CTextureManager::GetRwTexture("gg_enemy_guage");
    m_pTextureGGEnemyBG = CTextureManager::GetRwTexture("gg_enemy_bg");
    
    ASSERT(m_pTextureGGEnemyGauge);
    ASSERT(m_pTextureGGEnemyBG);
};


void CGaugeInformation_Container::InfoDispBase(bool bFullScreen)
{
    if (bFullScreen)
		m_sprite.Resize(CSprite::m_fVirtualScreenW, CSprite::m_fVirtualScreenH);
    else
        m_sprite.Resize(512.0f, 320.0f);

    m_sprite.SetTextureEmpty();
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(0, 0, 0, 200);
    m_sprite.Move(0.0f, 0.0f);
    
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    
    m_sprite.Draw();
};


void CGaugeInformation_Container::InfoDispRenderStatePush(void)
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


void CGaugeInformation_Container::InfoDispRenderStatePop(void)
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


static CGaugeInformation_Container* s_pGaugeInformationContainer = nullptr;


static inline CGaugeInformation_Container& GaugeInformationContainer(void)
{
    ASSERT(s_pGaugeInformationContainer);
    return *s_pGaugeInformationContainer;
};


/*static*/ void CGaugeInformation::Initialize(void)
{
    if (!s_pGaugeInformationContainer)
        s_pGaugeInformationContainer = new CGaugeInformation_Container;        
};


/*static*/ void CGaugeInformation::Terminate(void)
{
    if (s_pGaugeInformationContainer)
    {
        delete s_pGaugeInformationContainer;
        s_pGaugeInformationContainer = nullptr;
    };
};


/*static*/ void CGaugeInformation::DispInit(void)
{
    GaugeInformationContainer().DispInitSub();
};


/*static*/ void CGaugeInformation::PausePeriod(void)
{
    GaugeInformationContainer().PausePeriodSub();
};


/*static*/ void CGaugeInformation::PauseDraw(void)
{
    GaugeInformationContainer().PauseDrawSub();
};


/*static*/ CGaugeInformation::PAUSESTATUS CGaugeInformation::GetPauseStatus(void)
{
    return GaugeInformationContainer().GetPauseStatusSub();
};


/*static*/ void CGaugeInformation::MissionInfoSet(STAGEID::VALUE idStage)
{
    GaugeInformationContainer().MissionInfoSetSub(idStage);
};


/*static*/ void CGaugeInformation::MissionInfoPeriod(void)
{
    GaugeInformationContainer().MissionPeriodSub();
};


/*static*/ void CGaugeInformation::MissionInfoDraw(void)
{
    GaugeInformationContainer().MissionInfoDrawSub();
};


/*static*/ void CGaugeInformation::SetBattleNexusInfo(void)
{
    GaugeInformationContainer().SetBattleNexusInfoSub();
};


/*static*/ bool CGaugeInformation::IsBattleNexusInfoEnd(void)
{
    return GaugeInformationContainer().IsBattleNexusInfoEndSub();
};


/*static*/ void CGaugeInformation::DispBattleNexusInfo(void)
{
    GaugeInformationContainer().DispBattleNexusInfoSub();
};


/*static*/ void CGaugeInformation::BossGaugeEnable(bool bEnable, int32 no, ENEMYID::VALUE idEnemy)
{
    GaugeInformationContainer().BossGaugeEnableSub(bEnable, no, idEnemy);
};


/*static*/ void CGaugeInformation::BossGaugeSet(int32 no, int32 hp)
{
    GaugeInformationContainer().BossGaugeSetSub(no, hp);
};


/*static*/ void CGaugeInformation::BossGaugeDraw(int32 no)
{
    GaugeInformationContainer().BossGaugeDrawSub(no);
};

