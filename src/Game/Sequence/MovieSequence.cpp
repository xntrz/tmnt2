#include "MovieSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Movie/MovieManager.hpp"
#include "Game/System/Movie/MovieDataManager.hpp"
#include "System/Common/Movie.hpp"
#include "System/Common/Controller.hpp"


/*static*/ CProcess* CMovieSequence::Instance(void)
{
    return new CMovieSequence;    
};


CMovieSequence::CMovieSequence(void)
: m_pMovie(nullptr)
, m_bOwner(false)
{
    ;
};


CMovieSequence::~CMovieSequence(void)
{
    ;
};


bool CMovieSequence::OnAttach(const void* param)
{
    OUTPUT("%s. MovieID: %d\n", __FUNCTION__, param);
    
    m_bOwner = false;
    m_pMovie = CMovieManager::GetMovieInstance();

    if (!m_pMovie)
    {
        m_bOwner = true;
        CMovieManager::PreCreateMovieInstance(int32(param));
        m_pMovie = CMovieManager::GetMovieInstance();        
    };

    if (CMovieManager::IsCreateSuccess())
    {
        int32 iMovieID = CMovieManager::GetMovieID();
        int32 iFileID = CMovieDataManager::GetFileID(iMovieID);
        int32 iPartitionID = CMovieDataManager::GetPartitionID(iMovieID);
        m_pMovie->StartAfs(iPartitionID, iFileID);
    }
	else
	{
        OUTPUT("Movie create failed");
        ASSERT(false);
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


void CMovieSequence::OnMove(bool bRet, const void* param)
{
    if (!m_pMovie)
        Ret();

    int32 iController = CGameData::Attribute().GetVirtualPad();
    if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
        Ret();

    if (m_pMovie->IsEnded())
        Ret();

    m_pMovie->Update();
};


void CMovieSequence::OnDraw(void) const
{
    m_pMovie->Draw();
};

