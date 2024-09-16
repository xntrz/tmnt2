#include "Anim2DBaseSequence.hpp"
#include "Animation2D.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/RenderState.hpp"


CAnim2DSequence::CAnim2DSequence(void)
: m_pAnimation2D(nullptr)
, m_bDisplayLoading(true)
, m_bResumed(false)
{
    m_szAnimName[0] = '\0';
};


CAnim2DSequence::CAnim2DSequence(const char* pszAnimName)
: m_pAnimation2D(nullptr)
, m_bDisplayLoading(true)
, m_bResumed(false)
{
    SetAnimationName(pszAnimName);
};


CAnim2DSequence::~CAnim2DSequence(void)
{
    ASSERT(!m_pAnimation2D);
};


void CAnim2DSequence::OnDetach(void)
{
    CLoadingDisplay::Stop(this);
    m_pAnimation2D->Stop();
    m_pAnimation2D = nullptr;
    CAnimation2DLoader::Close(m_szAnimName);
    CTextureManager::GenerationDec();
    CAnimation2D::Terminate();
};


void CAnim2DSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_animstep)
    {
    case ANIMSTEP_READFILE:
        {
            CDataLoader::Period();
            
            if (!CDataLoader::IsLoadEnd())
                break;

            CLoadingDisplay::Stop(this);

            ASSERT(std::strlen(m_szAnimName) > 0);

            m_bResumed = bRet;
            m_pAnimation2D = CAnimation2DLoader::Get(m_szAnimName);
            ASSERT(m_pAnimation2D);
            if (m_pAnimation2D)
                BeginFadein();
            else
                CSequence::Ret();
        }
        break;
        
    case ANIMSTEP_FADEIN:
        {
            if (CScreenFade::IsFading())
                break;
            
            m_animstep = ANIMSTEP_DRAW;
            CGameData::Attribute().SetInteractive(true);
        }
        break;
        
    case ANIMSTEP_DRAW:
        {
            m_pAnimation2D->Update();
            m_pAnimation2D->Input();
        }
        break;
        
    case ANIMSTEP_FADEOUT:
        {
            if (!CScreenFade::IsFading())
                m_animstep = ANIMSTEP_END;
        }
        break;
        
    case ANIMSTEP_END:
        {
            m_bResumed = false;
        }
        break;
        
    default:
        ASSERT(false);
        break;
    };
};


void CAnim2DSequence::OnDraw(void) const
{
	if (IsDrawing())
	{
		CRenderStateManager::SetDefault();

		if (CSystem2D::BeginScene())
		{
			m_pAnimation2D->Draw();
			CSystem2D::EndScene();
		};
	};
};


bool CAnim2DSequence::OnAttach(int32 iFileID)
{
    CAnimation2D::Initialize();
    CTextureManager::GenerationInc();
    CDataLoader::Regist(iFileID);

    if (m_bDisplayLoading)
        CLoadingDisplay::Start(this);

	m_animstep = ANIMSTEP_READFILE;

    return true;
};


bool CAnim2DSequence::OnAttach(const char* pszFilename)
{
    CAnimation2D::Initialize();
    CTextureManager::GenerationInc();
    CDataLoader::Regist(pszFilename);

    if (m_bDisplayLoading)
        CLoadingDisplay::Start(this);
    
	m_animstep = ANIMSTEP_READFILE;

    return true;
};


void CAnim2DSequence::BeginFadein(void)
{
    if (!m_bResumed)
    {
        m_pAnimation2D->KeyInfoChange();
        m_pAnimation2D->Start();
    };
    
    CScreenFade::BlackIn();
    m_animstep = ANIMSTEP_FADEIN;
};


void CAnim2DSequence::BeginFadeout(void)
{
    CGameData::Attribute().SetInteractive(false);
    CScreenFade::BlackOut();
    m_animstep = ANIMSTEP_FADEOUT;
};


bool CAnim2DSequence::IsAnim2DMessageList(const char** pTable, int32 max, int32* index) const
{
    ASSERT(pTable);
    ASSERT(index);
    
    for (int32 i = 0; i < max; ++i)
    {
        if (m_pAnimation2D->CheckMessageGetURL(pTable[i]))
        {
            *index = i;
            return true;
        };
    };

    return false;
};


bool CAnim2DSequence::IsDrawing(void) const
{
    return ((m_animstep >= ANIMSTEP_FADEIN) &&
            (m_animstep <= ANIMSTEP_FADEOUT));
};


void CAnim2DSequence::SetAnimationName(const char* pszAnimName)
{
    ASSERT(pszAnimName);
    ASSERT(std::strlen(pszAnimName) < COUNT_OF(m_szAnimName));

    std::strcpy(m_szAnimName, pszAnimName);
};