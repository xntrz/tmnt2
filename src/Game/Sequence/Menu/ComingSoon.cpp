#include "ComingSoon.hpp"

#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Sprite.hpp"


/*static*/ CProcess* CComingSoonSequence::Instance(void)
{
    return new CComingSoonSequence;
};


CComingSoonSequence::CComingSoonSequence(void)
{
    ;
};


CComingSoonSequence::~CComingSoonSequence(void)
{
    ;
};


bool CComingSoonSequence::OnAttach(const void* pParam)
{
    CTextureManager::GenerationInc();
    
    CDataLoader::Regist(FPATH("Common/Menu/ComingSoon/ComingSoon.lpac"));
    
    m_eStep = STEP_READFILE;
    m_fTimer = 0.0f;
    m_pTexture = nullptr;
    
    return true;
};


void CComingSoonSequence::OnDetach(void)
{
    CTextureManager::GenerationDec();
};


void CComingSoonSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_eStep)
    {
    case STEP_READFILE:
        {
            CDataLoader::Period();
            
            if (CDataLoader::IsLoadEnd())
            {
                CTextureManager::SetCurrentTextureSet("ComingSoon");
                m_pTexture = CTextureManager::GetRwTexture("E3_2004fall");
                m_fTimer = 0.0f;
                m_eStep = STEP_FADEIN;

                CGameSound::PlayBGM(SDCODE_BGM(12300));

                CScreenFade::BlackIn(1.0f);
            };
        }
        break;

    case STEP_FADEIN:
        {
            m_fTimer += CScreen::TimerStride();
            
            if (!CScreenFade::IsFading())
            {
                m_fTimer = 0.0f;
                m_eStep = STEP_DRAW;
            };
        }
        break;
        
    case STEP_DRAW:
        {
            uint32 DigitalTrigger = 0;
            DigitalTrigger |= IPad::GetDigitalTrigger(IPad::CONTROLLER_LOCKED_ON_VIRTUAL);
            DigitalTrigger |= IPad::GetDigitalTrigger(IPad::CONTROLLER_UNLOCKED_ON_VIRTUAL);

            m_fTimer += CScreen::TimerStride();

            if ((m_fTimer >= 5.0f) || (DigitalTrigger & IPad::DIGITAL_START))
            {
                m_fTimer = 0.0f;
				m_eStep = STEP_FADEOUT;

                CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);

                CScreenFade::BlackOut(1.0f);
            };
        }
        break;

    case STEP_FADEOUT:
        {
            m_fTimer += CScreen::TimerStride();

            if (!CScreenFade::IsFading())
            {
                m_fTimer = 0.0f;
                m_eStep = STEP_END;
            };
        }
        break;

    case STEP_END:
        {
            Ret();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CComingSoonSequence::OnDraw(void) const
{
    switch (m_eStep)
    {
    case STEP_FADEIN:
        DrawScreen(m_fTimer);
        break;

    case STEP_DRAW:
        DrawScreen(1.0f);
        break;

    case STEP_FADEOUT:
        DrawScreen(1.0f - m_fTimer);
        break;

    default:
        break;
    };
};


void CComingSoonSequence::DrawScreen(float t) const
{
    float alpha = Clamp(t * 255.0f, 0.0f, 255.0f);
    uint8 alphaBasis = static_cast<uint8>(alpha);

    CSprite::PushRenderStates();

    CSprite sprite;
    sprite.SetTexture(m_pTexture);
	sprite.SetPositionAndSize(0.0f, 0.0f, 640.0f, 480.0f);
    sprite.SetRGBA(255, 255, 255, alphaBasis);
    sprite.SetUV(0.0f, 0.0f, 0.625f, 0.9375f);
    sprite.Draw();

    CSprite::PopRenderStates();
};