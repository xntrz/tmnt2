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


bool CAnim2DSequence::Call(int32 iLabel, bool bKeepDraw, const void* param)
{
    //
    //  Intercept the call inside anim2dseq for correctly resume after return child seq
    //
    
    switch (m_step)
    {
	case STEP_CUSTOM_RET:
        m_step = STEP_READFILE;
        return CSequence::Call(iLabel, bKeepDraw, param);

    default:
        ASSERT(false);
        return false;
    };
};


bool CAnim2DSequence::Ret(const void* param)
{
    switch (m_step)
    {
    case STEP_DRAW:
        BeginFadeIn();
        return true;

    case STEP_CUSTOM_RET:
    default:
        return CSequence::Ret(param);
    };        
};


void CAnim2DSequence::OnDetach(void)
{
    CLoadingDisplay::Stop();
    m_pAnimation2D->Stop();
    m_pAnimation2D = nullptr;
    CAnimation2DLoader::Close(m_szAnimName);
    CTextureManager::GenerationDec();
    CAnimation2D::Terminate();
};


void CAnim2DSequence::OnMove(bool bResume, const void* param)
{
    switch (m_step)
    {
    case STEP_READFILE:
        if (CDataLoader::IsLoadEnd())
        {
            CLoadingDisplay::Stop();

            ASSERT(std::strlen(m_szAnimName) > 0);
            
            m_bResumed = bResume;
            m_pAnimation2D = CAnimation2DLoader::Get(m_szAnimName);
            ASSERT(m_pAnimation2D);
           
			if (m_pAnimation2D)
			{
                BeginFadeOut();
			}
			else
			{
                CSequence::Ret();
			};
        }
        else
        {
            CDataLoader::Period();
        };        
        break;
        
    case STEP_FADE_OUT:
        if (!CScreenFade::IsFading())
        {
            m_step = STEP_DRAW;
            CGameData::Attribute().SetInteractive(true);
        };
        break;
        
    case STEP_DRAW:
        m_pAnimation2D->Update();
        m_pAnimation2D->Input();
        break;
        
    case STEP_FADE_IN:
        if (!CScreenFade::IsFading())
            m_step = STEP_END;
        break;
        
    case STEP_END:
        m_bResumed = false;
        m_step = STEP_CUSTOM_RET;

		//
		//	OnRet can modify step var while calling
		//	overrided virtual Call, so check it out
		//
		if (!OnRet())
            CSequence::Ret();
        break;

    case STEP_CUSTOM_RET:
        ASSERT(false, "Return was intercepted but possibly not handled correctly");
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
        CLoadingDisplay::Start();

	m_step = STEP_READFILE;

    return true;
};


bool CAnim2DSequence::OnAttach(const char* pszFilename)
{
    CAnimation2D::Initialize();
    CTextureManager::GenerationInc();
    CDataLoader::Regist(pszFilename);

    if (m_bDisplayLoading)
        CLoadingDisplay::Start();
    
	m_step = STEP_READFILE;

    return true;
};


void CAnim2DSequence::BeginFadeOut(void)
{
    if (!m_bResumed)
    {
        m_pAnimation2D->KeyInfoChange();
        m_pAnimation2D->Start();
    };
    
    CScreenFade::StartOut();
    m_step = STEP_FADE_OUT;
};


void CAnim2DSequence::BeginFadeIn(void)
{
    CGameData::Attribute().SetInteractive(false);
    CScreenFade::StartIn();
    m_step = STEP_FADE_IN;
};


bool CAnim2DSequence::OnRet(void)
{
    return false;
};


bool CAnim2DSequence::IsAnim2DMessageList(const char** pTable, int32 max, int32* index) const
{
    ASSERT(pTable);
    ASSERT(index);
    
    for (int32 i = 0; i < max; ++i)
    {
        if (!m_pAnimation2D->CheckMessageGetURL(pTable[i]))
        {
            *index = i;
            return true;
        };
    };

    return false;
};


bool CAnim2DSequence::IsDrawing(void) const
{
    return (m_step >= STEP_FADE_OUT &&
            m_step <= STEP_FADE_IN);
};


void CAnim2DSequence::SetAnimationName(const char* pszAnimName)
{
    ASSERT(pszAnimName);
    ASSERT(std::strlen(pszAnimName) < COUNT_OF(m_szAnimName));

    std::strcpy(m_szAnimName, pszAnimName);
};