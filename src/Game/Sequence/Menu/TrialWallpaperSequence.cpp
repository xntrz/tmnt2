#include "TrialWallpaperSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Screen.hpp"


bool CTrialWallpaperSequence::OnAttach(const void* pParam)
{
    m_pTexture = nullptr;
    m_fTimer = 0.0f;
    m_eStep = STEP_READFILE;

    CTextureManager::GenerationInc();

    CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
    UnlockAllControllers();

    CDataLoader::Regist(FPATH("Common/Trial/Trial.lpac"));
    CLoadingDisplay::Start(this);

    return true;
};


void CTrialWallpaperSequence::OnDetach(void)
{
    CTextureManager::GenerationDec();
    CLoadingDisplay::Stop(this);
};


void CTrialWallpaperSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_eStep)
    {
    case STEP_READFILE:
        {
            CDataLoader::Period();

            if (!CDataLoader::IsLoadEnd())
                break;

            CLoadingDisplay::Stop(this);

            static const char* s_apszTrialTexName[] =
            {
                /* TYPE_UNKNOWN      */ nullptr, 
                /* TYPE_CONTROL_INFO */ "opm_controls",
                /* TYPE_COMING_SOON  */ "opm_coming",
            };

            static_assert(COUNT_OF(s_apszTrialTexName) == TYPENUM, "update tex table");

            CTextureManager::SetCurrentTextureSet("trial");
            m_pTexture = CTextureManager::GetRwTexture(s_apszTrialTexName[m_type]);

            m_sprite.SetTexture(m_pTexture);
            m_sprite.SetPositionAndSize(0.0f, 0.0f, 1024.0f, 512.0f);
        }
        break;

    case STEP_FADEIN:
        {
            CScreenFade::BlackIn();
            m_eStep = STEP_FADEIN_WAIT;
        }
        break;

    case STEP_FADEIN_WAIT:
        {
            if (!CScreenFade::IsFading())
            {
                CGameData::Attribute().SetInteractive(true);
                m_eStep = STEP_DRAW;
            };
        }
        break;
        
    case STEP_DRAW:
        {
            int32 iPad = CGameData::Attribute().GetVirtualPad();

            if (IGamepad::GetDigitalTrigger(iPad, IGamepad::DIGITAL_RDOWN))
                m_eStep = STEP_FADEOUT;

            if (m_fTimer > 15.0f)
                m_eStep = STEP_FADEOUT;

            m_fTimer += CScreen::TimerStride();
        }
        break;

    case STEP_FADEOUT:
        {
            CScreenFade::BlackOut();
            m_eStep = STEP_FADEOUT_WAIT;
        }
        break;

    case STEP_FADEOUT_WAIT:
        {
            if (!CScreenFade::IsFading())
                m_eStep = STEP_END;
        }
        break;

    case STEP_END:
        {
            CGameData::Attribute().SetInteractive(false);
            Ret();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CTrialWallpaperSequence::OnDraw(void) const
{
    switch (m_eStep)
    {
    case STEP_FADEIN:
    case STEP_FADEIN_WAIT:
    case STEP_DRAW:
    case STEP_FADEOUT:
    case STEP_FADEOUT_WAIT:
        CSprite::PushRenderStates();
        m_sprite.Draw();
        CSprite::PopRenderStates();
        break;

    default:
        break;
    };
};


//
// *********************************************************************************
//


/*static*/ CProcess* CTrialComingSoon::Instance(void)
{
    return new CTrialComingSoon;
};


bool CTrialComingSoon::OnAttach(const void* pParam)
{
    m_type = TYPE_COMING_SOON;

    CTrialWallpaperSequence::OnAttach(pParam);

    return true;
};


//
// *********************************************************************************
//


/*static*/ CProcess* CTrialControlInformation::Instance(void)
{
    return new CTrialControlInformation;
};


bool CTrialControlInformation::OnAttach(const void* pParam)
{
    m_type = TYPE_CONTROL_INFO;

    CTrialWallpaperSequence::OnAttach(pParam);
    CTimeoutProcess::Enable(this, true);
    CTimeoutProcess::Start(this);

    return true;
};