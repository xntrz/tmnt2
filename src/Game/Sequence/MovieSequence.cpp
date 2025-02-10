#include "MovieSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Movie/MovieManager.hpp"
#include "Game/System/Movie/MovieID.hpp"
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
    OUTPUT("%s. MovieID: %d\n", __FUNCTION__, reinterpret_cast<int32>(pParam));
    
    m_bOwner = false;
    m_bLoadEndFlag = false;
    m_pMovie = CMovieManager::GetMovieInstance();

    if (!m_pMovie)
    {
        m_bOwner = true;

        MOVIEID::VALUE movieId = static_cast<MOVIEID::VALUE>(reinterpret_cast<int32>(pParam));
        CMovieManager::PreCreateMovieInstance(movieId);
        
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

