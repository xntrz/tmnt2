#include "MovieSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Movie/MovieManager.hpp"
#include "System/Common/Movie.hpp"
#include "System/Common/Controller.hpp"


/*static*/ CProcess* CMovieSequence::Instance(void)
{
    return new CMovieSequence;    
};


CMovieSequence::CMovieSequence(void)
: m_pMovie(nullptr)
, m_bOwner(false)
, m_bLoadEndFlag(false)
{
    ;
};


CMovieSequence::~CMovieSequence(void)
{
    ;
};


bool CMovieSequence::OnAttach(const void* pParam)
{
    OUTPUT("%s. MovieID: %" PRImvn "\n", __FUNCTION__, mvname_cast(pParam));
    
    m_bOwner = false;
    m_bLoadEndFlag = false;
    m_pMovie = CMovieManager::GetMovieInstance();

    if (!m_pMovie)
    {
        m_bOwner = true;

        CMovieManager::PreCreateMovieInstance(mvname_cast(pParam));        
        m_pMovie = CMovieManager::GetMovieInstance();
    };

    return true;
};


void CMovieSequence::OnDetach(void)
{
    if (m_bOwner)
    {
        CMovieManager::DeleteMovieInstance();
        m_pMovie = nullptr;        
    };
};


void CMovieSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (!m_bLoadEndFlag)
    {
        if (!CMovieManager::IsCreateSuccess())
            return;
        
        CMovieManager::OnCreateSuccess();
        m_bLoadEndFlag = true;
    };

    int32 virtualPad = CGameData::Attribute().GetVirtualPad();
    if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_OK) ||
        CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_START))
        Ret();

    if (CMovieManager::IsEnded())
        Ret();

    CMovieManager::Update();
};


void CMovieSequence::OnDraw(void) const
{
    CMovieManager::Draw();
};

