#include "PlayDemoSequence.hpp"
#include "PlayDemoSeqState.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Misc/PadStream.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Screen.hpp"


/*static*/ STAGEID::VALUE CPlayDemoSequence::m_idStage = STAGEID::ID_ST02N;


/*static*/ PLAYERID::VALUE CPlayDemoSequence::m_aPlayerTable[] =
{
    PLAYERID::ID_LEO,
    PLAYERID::ID_RAP,
    PLAYERID::ID_MIC,
    PLAYERID::ID_DON,
};


/*static*/ const CPlayDemoSequence::STAGEINFO CPlayDemoSequence::m_aStageTable[] =
{
    { STAGEID::ID_ST02N, GAMETYPES::PLAYERS_MAX },
    { STAGEID::ID_ST21N, GAMETYPES::PLAYERS_MAX },
};


/*static*/ CProcess* CPlayDemoSequence::Instance(void)
{
    CPadStreamSwitch::m_bEnd = false;
    CPadStreamSwitch::m_mode = CPadStream::MODE_PLAY;
    
    return new CPlayDemoSequence;
};


CPlayDemoSequence::CPlayDemoSequence(void)
: m_fTime(0.0f)
, m_iStageIndex(0)
{
    ;
};


CPlayDemoSequence::~CPlayDemoSequence(void)
{
    ;
};


bool CPlayDemoSequence::OnAttach(const void* pParam)
{
#ifdef TMNT2_BUILD_EU
    CScreen::SetPalMode(false);
#endif /* TMNT2_BUILD_EU */
    
    CGameData::Attribute().SetGamemode(GAMETYPES::GAMEMODE_DEMO);
    
    CGameData::PushRecord();
    CGameData::Record().SetDefault();
    CGameData::Record().Item().SetItemTaken(ITEMID::ID_DON_LASER);
    
    CGameData::PushOption();
    CGameData::Option().Play().SetDefault();
    CGameData::Option().Display().SetEnableHelp(false);
    CGameData::Option().Display().SetDefault();

    CGameSound::Stop();

    for (m_iStageIndex = 0; m_iStageIndex < COUNT_OF(m_aStageTable); ++m_iStageIndex)
    {
        if (m_idStage == m_aStageTable[m_iStageIndex].m_idStage)
            break;
    };

    CGameData::PlayParam().ClearPlayer();

    for (int32 i = 0; i < m_aStageTable[m_iStageIndex].m_nNumPlayers; ++i)
    {
        ASSERT(i >= 0);
        ASSERT(i < COUNT_OF(m_aPlayerTable));
        
        CGameData::PlayParam().AddPlayerCharacter(i, m_aPlayerTable[i], GAMETYPES::COSTUME_NONE);
    };

    CGameData::PlayParam().ClearArea();
    CGameData::PlayParam().SetAreaPlaymode(CGamePlayParam::AREAPLAYMODE_ONE_STAGE);
    CGameData::PlayParam().SetStage(m_idStage);
    CGameData::OnBeginArea();

    CStageBaseSequence::OnAttach(pParam);

    RegisterStateObject(STATE_LOAD, new CLoadStageSeqState(CGameData::PlayParam().GetStage()), true);
    RegisterStateObject(STATE_PLAY, new CPlayDemoStageSeqState, true);

    /* TODO: currently simulation brokes after 628 stage tick
             (or after 1000 tick when map effect gimmick is disabled) */
    ChangeState(STATE_LOAD);

    RwCamera* camera = CSystem2D::Camera().GetRwCamera();
    float fDepth = (RwCameraGetNearClipPlane(camera) * 1.01f);
    Rt2dDeviceSetLayerDepth(fDepth);

    Math::SRand(123456);

#ifdef BUILD_TRIAL
    CTimeoutProcess::Enable(this, true);
    CTimeoutProcess::Start(this);
#endif    
    
    return true;
};


void CPlayDemoSequence::OnDetach(void)
{
    Rt2dDeviceSetLayerDepth(1.0f);

    CStageBaseSequence::OnDetach();

    CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_LOCKED_ON_VIRTUAL);
    UnlockAllControllers();

    //if (++m_iStageIndex >= COUNT_OF(m_aStageTable))
    //    m_iStageIndex = 0;

    m_idStage = m_aStageTable[m_iStageIndex].m_idStage;

    CPadStreamSwitch::m_bEnd = false;
    CPadStreamSwitch::m_mode = CPadStream::MODE_NONE;

    CGameData::PopOption();
    CGameData::PopRecord();
    CGameData::Attribute().SetGamemode(GAMETYPES::GAMEMODE_NORMAL);

#ifdef TMNT2_BUILD_EU
    CScreen::SetPalMode(true);
#endif /* TMNT2_BUILD_EU */
};


void CPlayDemoSequence::OnMove(bool bRet, const void* pReturnValue)
{
    CStageBaseSequence::OnMove(bRet, pReturnValue);

    if (GetState() == STATE_PLAY)
    {
        m_fTime += CGameProperty::GetElapsedTime();
        if (m_fTime >= 3.0f)
            m_fTime = 0.0f;
    };
};


void CPlayDemoSequence::OnDraw(void) const
{
    CStageBaseSequence::OnDraw();

    if (GetState() == STATE_PLAY)
    {
        uint8 uAlphaBasis = 0xFF;

        if (m_fTime < 2.5f)
        {
			if(m_fTime >= 2.0f)
				uAlphaBasis = uint8(255.0f * (2.0f * (2.5f - m_fTime)));
        }
        else
        {
            uAlphaBasis = uint8(255.0f * (2.0f * (m_fTime - 2.5f)));
        };

        if (CSystem2D::BeginScene())
        {
            CSystem2D::PushRenderState();
            
            CGameFont::SetHeightScaled(3.0f);
            CGameFont::SetRGBA(255, 124, 0, uAlphaBasis);
            
            const wchar* pwszText = CGameText::GetText(GAMETEXT_MENU_START_BTN);
            CGameFont::Show(pwszText,
                            272.0f - CGameFont::GetStringWidth(pwszText),
                            192.0f);

            m_LogoSprite.Draw();

            CSystem2D::PopRenderState();
            CSystem2D::EndScene();
        };            
    };
};


void CPlayDemoSequence::OnStateDetached(STATE state)
{
    switch (state)
    {
    case STATE_LOAD:
        InitializeLogo();
        ChangeState(STATE_PLAY);
        m_fTime = 0.0f;
        break;

    case STATE_PLAY:
        ChangeState(STATE_END);
        break;

    default:
        break;
    };
};


void CPlayDemoSequence::InitializeLogo(void)
{
    CTextureManager::SetCurrentTextureSet("logo_mini");
    
    RwTexture* pTexture = CTextureManager::GetRwTexture("demo_logo");
    ASSERT(pTexture);

    m_LogoSprite.SetTexture(pTexture);
    m_LogoSprite.SetPositionAndSize(-220.0f, -172.0f, 128.0f, 64.0f);
};