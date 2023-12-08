#include "MovieManager.hpp"
#include "MovieDataManager.hpp"

#include "System/Common/Movie.hpp"

#ifdef TARGET_PC
#include "System/PC/PCMovie.hpp"
#else
#error Not implemented for current target
#endif


static CMovie* s_pMovie = nullptr;
static int32 s_iMovieID = -1;


/*static*/ void CMovieManager::Initialize(void)
{
    CMovieDataManager::Initialize();
};


/*static*/ void CMovieManager::Terminate(void)
{
    CMovieDataManager::Terminate();
};


/*static*/ void CMovieManager::PreCreateMovieInstance(int32 iMovieID)
{
    ASSERT(CMovieDataManager::IsValidMovieID(iMovieID));

    if (CMovieDataManager::IsValidMovieID(iMovieID))
    {
        ASSERT(!s_pMovie);
        
        int32 sw = int32(TYPEDEF::VSCR_W);
        int32 sh = int32(TYPEDEF::VSCR_H);
        int32 bps = 0x200000;
        bool paltv = false;

        CMovie* pMovie = nullptr;
        
#ifdef TARGET_PC
        pMovie = new CPCMovie(sw, sh, bps, paltv);
#else
#error Not implemented for current target
#endif
        
        ASSERT(pMovie);
        
        s_pMovie = pMovie;
        s_iMovieID = iMovieID;        
    }
    else
    {
        s_pMovie = nullptr;
    };
};


/*static*/ CMovie* CMovieManager::GetMovieInstance(void)
{
    return s_pMovie;
};


/*static*/ int32 CMovieManager::GetMovieID(void)
{
    return s_iMovieID;
};


/*static*/ void CMovieManager::DeleteMovieInstance(void)
{
    ASSERT(s_pMovie);
    
    if (s_pMovie)
    {
        delete s_pMovie;
        s_pMovie = nullptr;
    };
    
    s_iMovieID = -1;
};


/*static*/ bool CMovieManager::IsCreateSuccess(void)
{
    if (s_pMovie)
        return s_pMovie->IsCreateSuccess();
    else
        return false;
};