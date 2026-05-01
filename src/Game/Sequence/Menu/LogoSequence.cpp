#include "LogoSequence.hpp"

#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Configure.hpp"


/*static*/ const CLogoDisplaySequence::LOGOPROPERTY CLogoDisplaySequence::m_aLogoProperties[] =
{
#if defined(TMNT2_TRIAL) && !defined(TMNT2_BUILD_EU)
    //{ "esrb_E",     4.0f, 4.0f,     FADETYPE_BLACK, 1.0f, FADETYPE_BLACK, 1.0f, SDCODE_SE(0x2009), nullptr },
    { "esrb_T",     4.0f, 4.0f,     FADETYPE_BLACK, 1.0f, FADETYPE_BLACK, 1.0f, SDCODE_SE(0x200A), nullptr },
    //{ "esrb_RP",    4.0f, 4.0f,     FADETYPE_BLACK, 1.0f, FADETYPE_BLACK, 1.0f, SDCODE_SE(0x200B), nullptr },
#endif /* defined(TMNT2_TRIAL) && !defined(TMNT2_BUILD_EU) */
    { "tit_konami", 2.0f, 2.0f,     FADETYPE_BLACK, 1.0f, FADETYPE_BLACK, 1.0f, SDCODE_SE(0), nullptr },
    { "tit_studio", 2.0f, 1.0f,     FADETYPE_BLACK, 1.0f, FADETYPE_BLACK, 1.0f, SDCODE_SE(0), nullptr },
};


/*static*/ CProcess* CLogoDisplaySequence::Instance(void)
{
    return new CLogoDisplaySequence;
};


CLogoDisplaySequence::CLogoDisplaySequence(void)
: m_fTime(0.0f)
, m_iLogoIndex(0)
, m_phase(PHASE_LOAD)
, m_sprite()
{
    ;
};


CLogoDisplaySequence::~CLogoDisplaySequence(void)
{
    ;
};


bool CLogoDisplaySequence::OnAttach(const void* pParam)
{
    m_sprite.SetRGB(0, 0, 0);

    m_fTime = 0.0f;
    m_iLogoIndex = 0;

    CTextureManager::GenerationInc();

    if (CConfigure::GetLanguage() != TYPEDEF::CONFIG_LAUNCH_NORMAL)
    {
        m_phase = PHASE_RET;
    }
    else
    {
        m_phase = PHASE_LOAD;
#ifdef TMNT2_TRIAL
        CGameSound::LoadWave(13);
        CDataLoader::Regist(FPATH("Common/Menu/Logo/LogoTR.lpac"));
#else /* TMNT2_TRIAL */
        CDataLoader::Regist(FPATH("Common/Menu/Logo/Logo.lpac"));
#endif /* TMNT2_TRIAL */
    };

#ifdef TMNT2_TRIAL
    CTimeoutProcess::Enable(this, true);
    CTimeoutProcess::Start(this);
#endif /* TMNT2_TRIAL */
    
    return true;
};


void CLogoDisplaySequence::OnDetach(void)
{
    CTextureManager::GenerationDec();
};


void CLogoDisplaySequence::OnMove(bool bRet, const void* pReturnValue)
{
    float w = static_cast<float>(CScreen::Width());
    float h = static_cast<float>(CScreen::Height());

    m_sprite.SetPositionAndSizeRealScreen((w * 0.5f), (h * 0.5f), w, h);

    switch (m_phase)
    {
    case PHASE_LOAD:
        {
            CDataLoader::Period();
            
            if (CDataLoader::IsLoadEnd() && CGameSound::IsLoadEnd())
                m_phase = PHASE_BEGIN;
        }
        break;
        
    case PHASE_BEGIN:
        {
            if (m_aLogoProperties[m_iLogoIndex].m_pszTextureName)
            {
                RwTexture* pRwTexture = CTextureManager::GetRwTexture(m_aLogoProperties[m_iLogoIndex].m_pszTextureName);

                if (pRwTexture)
                    m_sprite.SetTexture(pRwTexture);
                
                m_sprite.SetUV(0.1875f, 0.0625f, 0.8125f, 0.9375f);
                m_sprite.SetRGBA(255, 255, 255, 255);
            }
            else
            {
                m_sprite.SetRGBA(0, 0, 0, 0);
            };

            m_fTime = 0.0f;
            FadeinScreen(m_iLogoIndex);
        }
        break;
        
    case PHASE_FADEIN:
        {
            FadeinWait();        
        }
        break;
        
    case PHASE_DISPLAY:
        {
            bool bDispTimeout   = (m_fTime >= m_aLogoProperties[m_iLogoIndex].m_fDisplayTime);
            bool bStressTime    = (m_fTime >= m_aLogoProperties[m_iLogoIndex].m_fStressDisplayTime);
            bool bStressRequest = CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL,
                                                                 CController::DIGITAL_OK | CController::DIGITAL_CANCEL);

            if (bDispTimeout || (bStressTime && bStressRequest))
                FadeoutScreen(m_iLogoIndex);

            m_fTime += CScreen::TimerStride();
        }
        break;
        
    case PHASE_FADEOUT:
        {
            FadeoutWait();
        }
        break;
        
    case PHASE_END:
        {
            ++m_iLogoIndex;
            m_phase = (m_iLogoIndex < COUNT_OF(m_aLogoProperties) ? PHASE_LOAD : PHASE_RET);
        }
        break;

    case PHASE_RET:
        {
            Ret();
        }        
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CLogoDisplaySequence::OnDraw(void) const
{
    CSprite::PushRenderStates();    
    m_sprite.Draw();
    CSprite::PopRenderStates();
};


void CLogoDisplaySequence::FadeinScreen(int32 index)
{    
    switch (m_aLogoProperties[index].m_eFadeinType)
    {
    case FADETYPE_BLACK:
        CScreenFade::BlackIn(m_aLogoProperties[index].m_fFadeinTime);
        break;

    case FADETYPE_WHITE:
        CScreenFade::WhiteIn(m_aLogoProperties[index].m_fFadeinTime);
        break;

    default:
        ASSERT(false);
        break;
    };

    m_phase = PHASE_FADEIN;
};


void CLogoDisplaySequence::FadeinWait(void)
{
    if (!CScreenFade::IsFading())
    {
        CGameSound::PlaySE(m_aLogoProperties[m_iLogoIndex].m_iSound);
        m_phase = PHASE_DISPLAY;
    };
};


void CLogoDisplaySequence::FadeoutScreen(int32 index)
{
    switch (m_aLogoProperties[index].m_eFadeoutType)
    {
    case FADETYPE_BLACK:
        CScreenFade::BlackOut(m_aLogoProperties[index].m_fFadeoutTime);
        break;

    case FADETYPE_WHITE:
        CScreenFade::WhiteOut(m_aLogoProperties[index].m_fFadeoutTime);
        break;

    default:
        ASSERT(false);
        break;
    };

    m_phase = PHASE_FADEOUT;
};


void CLogoDisplaySequence::FadeoutWait(void)
{
    if (!CScreenFade::IsFading())
        m_phase = PHASE_END;
};