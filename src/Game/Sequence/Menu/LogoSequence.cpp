#include "LogoSequence.hpp"

#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/File/FileID.hpp"


namespace
{
    struct LOGOPROPERTY
    {
        const char* m_pszTextureName;
        float m_fDisplayTime;
        float m_fStressDisplayTime;
        RwRGBA m_FadeinColor;
        RwRGBA m_FadeoutColor;
        float m_fFadeinTime;
        float m_fFadeoutTime;
        int32 m_iSound;
        void* m_pLoadData;
    };    
};


static const LOGOPROPERTY s_aLogoProperty[] =
{
    { nullptr,      0.7f, 0.7f, { 0, 0, 0 }, { 0, 0, 0 }, 0.0f, 0.0f, SDCODE_SE(0), nullptr },
    { "tit_konami", 2.0f, 2.0f, { 0, 0, 0 }, { 0, 0, 0 }, 1.0f, 1.0f, SDCODE_SE(0), nullptr },
    { "tit_studio", 2.0f, 1.0f, { 0, 0, 0 }, { 0, 0, 0 }, 1.0f, 1.0f, SDCODE_SE(0), nullptr },
};


/*static*/ CProcess* CLogoSequence::Instance(void)
{
    return new CLogoSequence;
};


CLogoSequence::CLogoSequence(void)
: m_fTime(0.0f)
, m_iLogoIndex(0)
, m_phase(PHASE_LOAD)
{
    ;
};


CLogoSequence::~CLogoSequence(void)
{
    ;
};


bool CLogoSequence::OnAttach(const void* param)
{
    CTextureManager::GenerationInc();

    CDataLoader::Regist(FILEID::ID_LOGO);
    m_phase = PHASE_LOAD;
    m_fTime = 0.0f;
    m_iLogoIndex = 0;

    return true;
};


void CLogoSequence::OnDetach(void)
{
    CTextureManager::GenerationDec();
};


void CLogoSequence::OnMove(bool bRet, const void* param)
{
    switch (m_phase)
    {
    case PHASE_LOAD:
        if (CDataLoader::IsLoadEnd() && CGameSound::IsLoadEnd())
            m_phase = PHASE_BEGIN;
        else
            CDataLoader::Period();      
        break;
        
    case PHASE_BEGIN:
        if (s_aLogoProperty[m_iLogoIndex].m_pszTextureName)
        {
            RwTexture* pRwTexture = CTextureManager::GetRwTexture(s_aLogoProperty[m_iLogoIndex].m_pszTextureName);
			m_Sprite.SetTexture(pRwTexture);
			m_Sprite.Move(0.0f, 0.0f);
            m_Sprite.Resize(CSprite::VIRTUALSCREEN_DEFAULT_W, CSprite::VIRTUALSCREEN_DEFAULT_H);
            m_Sprite.SetUV(0.1875f, 0.0625f, 0.8125f, 0.9375f);
            m_Sprite.SetRGBA(255, 255, 255, 255);
        }
        else
        {
            m_Sprite.SetRGBA(0, 0, 0, 0);
        };

        CScreenFade::SetColor(
            s_aLogoProperty[m_iLogoIndex].m_FadeoutColor.red,
            s_aLogoProperty[m_iLogoIndex].m_FadeoutColor.green,
            s_aLogoProperty[m_iLogoIndex].m_FadeoutColor.blue
        );
        CScreenFade::StartOut();
        m_fTime = 0.0f;
		m_phase = PHASE_FADEOUT;
        break;
        
    case PHASE_FADEOUT: 
        if (!CScreenFade::IsFading())
        {
            CGameSound::PlaySE(s_aLogoProperty[m_iLogoIndex].m_iSound);
            m_phase = PHASE_DISPLAY;
        };
        break;
        
    case PHASE_DISPLAY:
        if (m_fTime >= s_aLogoProperty[m_iLogoIndex].m_fDisplayTime)
        {
            CScreenFade::SetColor(
                s_aLogoProperty[m_iLogoIndex].m_FadeoutColor.red,
                s_aLogoProperty[m_iLogoIndex].m_FadeoutColor.green,
                s_aLogoProperty[m_iLogoIndex].m_FadeoutColor.blue
            );
            CScreenFade::StartIn();
            m_phase = PHASE_FADEIN;
        };

        m_fTime += CScreen::TimerStride();
        break;
        
    case PHASE_FADEIN:
        if (!CScreenFade::IsFading())
        {
            CGameSound::PlaySE(s_aLogoProperty[m_iLogoIndex].m_iSound);
            m_phase = PHASE_END;
        };
        break;
        
    case PHASE_END:
        ++m_iLogoIndex;
        m_phase = (m_iLogoIndex < COUNT_OF(s_aLogoProperty) ? PHASE_LOAD : PHASE_RET);
        break;

    case PHASE_RET:
        Ret();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CLogoSequence::OnDraw(void) const
{
    CSprite::PushRenderStates();    
    m_Sprite.Draw();
    CSprite::PopRenderStates();
};