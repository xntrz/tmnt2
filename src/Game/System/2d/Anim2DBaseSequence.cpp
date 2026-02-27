#include "Anim2DBaseSequence.hpp"
#include "Animation2D.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/RenderState.hpp"


CAnim2DSequence::CAnim2DSequence(void)
: CAnim2DSequence("")
{
    ;
};


CAnim2DSequence::CAnim2DSequence(const char* pszAnimName)
: m_szAnimName()
, m_animStep(ANIMSTEP_READFILE)
, m_pAnimation2D(nullptr)
, m_bDisplayLoading(true)
, m_bRet(false)
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

    Animation2D().Stop();
    m_pAnimation2D = nullptr;

    CAnimation2DLoader::Close(m_szAnimName);
    CTextureManager::GenerationDec();
    CAnimation2D::Terminate();
};


void CAnim2DSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_animStep)
    {
    case ANIMSTEP_READFILE:
        {
            CDataLoader::Period();
            
            if (!CDataLoader::IsLoadEnd())
                break;

            CLoadingDisplay::Stop(this);

            m_bRet = bRet;

            ASSERT(std::strlen(m_szAnimName) > 0);
            m_pAnimation2D = CAnimation2DLoader::Get(m_szAnimName);
            ASSERT(m_pAnimation2D);

            if (m_pAnimation2D)
                BeginFadeIn();
            else
                CSequence::Ret();
        }
        break;
        
    case ANIMSTEP_FADEIN:
        {
            if (CScreenFade::IsFading())
                break;
            
            m_animStep = ANIMSTEP_DRAW;
            CGameData::Attribute().SetInteractive(true);
        }
        break;
        
    case ANIMSTEP_DRAW:
        {
            Animation2D().Update();
            Animation2D().Input();
        }
        break;
        
    case ANIMSTEP_FADEOUT:
        {
            if (!CScreenFade::IsFading())
                m_animStep = ANIMSTEP_END;
        }
        break;
        
    case ANIMSTEP_END:
        {
            m_bRet = false;
        }
        break;
        
    default:
        ASSERT(false);
        break;
    };
};


void CAnim2DSequence::OnDraw(void) const
{
    if ((m_animStep == ANIMSTEP_FADEIN)  ||
        (m_animStep == ANIMSTEP_DRAW)    ||
        (m_animStep == ANIMSTEP_FADEOUT) ||
        (m_animStep == ANIMSTEP_END))
    {
        if (CSystem2D::BeginScene())
        {
            CRenderStateManager::SetDefault();
            Animation2D().Draw();

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

    m_animStep = ANIMSTEP_READFILE;

    return true;
};


bool CAnim2DSequence::OnAttach(const char* pszFilename)
{
    CAnimation2D::Initialize();
    CTextureManager::GenerationInc();
    CDataLoader::Regist(pszFilename);

    if (m_bDisplayLoading)
        CLoadingDisplay::Start(this);
    
    m_animStep = ANIMSTEP_READFILE;

    return true;
};


void CAnim2DSequence::BeginFadeIn(void)
{
    if (!m_bRet)
    {
        Animation2D().KeyInfoChange();
        Animation2D().Start();
    };
    
    CScreenFade::BlackIn();
    m_animStep = ANIMSTEP_FADEIN;
};


void CAnim2DSequence::BeginFadeOut(void)
{
    CGameData::Attribute().SetInteractive(false);

    CScreenFade::BlackOut();
    m_animStep = ANIMSTEP_FADEOUT;
};


bool CAnim2DSequence::IsAnim2DMessageList(const char** pTable, int32 max, int32* index) const
{
    ASSERT(pTable);
    ASSERT(index);
    
    for (int32 i = 0; i < max; ++i)
    {
        if (Animation2D().CheckMessageGetURL(pTable[i]))
        {
            *index = i;
            return true;
        };
    };

    return false;
};


bool CAnim2DSequence::IsDrawing(void) const
{
    return ((m_animStep >= ANIMSTEP_FADEIN) &&
            (m_animStep <= ANIMSTEP_FADEOUT));
};


void CAnim2DSequence::SetAnimationName(const char* pszAnimName)
{
    ASSERT(pszAnimName);
    ASSERT(std::strlen(pszAnimName) < sizeof(m_szAnimName));

    std::strcpy(m_szAnimName, pszAnimName);
};


CAnimation2D& CAnim2DSequence::Animation2D(void)
{
    ASSERT(m_pAnimation2D);
    return *m_pAnimation2D;
};


CAnimation2D& CAnim2DSequence::Animation2D(void) const
{
    ASSERT(m_pAnimation2D);
    return *m_pAnimation2D;
};


CAnim2DSequence::ANIMSTEP CAnim2DSequence::AnimStep(void) const
{
    return m_animStep;
};


void CAnim2DSequence::EnableLoadingDisplay(bool bEnable)
{
    m_bDisplayLoading = bEnable;
};