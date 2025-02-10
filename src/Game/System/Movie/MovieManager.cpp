#include "MovieManager.hpp"
#include "MovieDataManager.hpp"
#include "MovieText.hpp"

#include "Game/System/DataLoader/DataLoader.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Movie.hpp"

#ifdef TARGET_PC
#include "System/PC/PCMovie.hpp"
#else
#error Not implemented for current target
#endif


static CMovie*          s_pMovie  = nullptr;
static MOVIEID::VALUE   s_movieId = MOVIEID::ID_INVALID;


/*static*/ void CMovieManager::Initialize(void)
{
    CMovieDataManager::Initialize();
};


/*static*/ void CMovieManager::Terminate(void)
{
    CMovieDataManager::Terminate();
};


/*static*/ void CMovieManager::PreCreateMovieInstance(MOVIEID::VALUE movieId)
{
    ASSERT(CMovieDataManager::IsValidMovieID(movieId));

    if (!CMovieDataManager::IsValidMovieID(movieId))
    {
        s_pMovie = nullptr;
        return;
    };

    s_movieId = movieId;

    ASSERT(!s_pMovie);

    int32 width       = 640;
    int32 height      = 448;
    int32 bps         = ((1024 * 512) * 4);
    bool  bUsePalMode = false;

#ifdef TMNT2_BUILD_EU
    if (CConfigure::GetTVMode() == TYPEDEF::CONFIG_TV_PAL)
        bUsePalMode = true;
#endif /* TMNT2_BUILD_EU */

#ifdef TARGET_PC
    s_pMovie = new CPCMovie(width, height, bps, bUsePalMode);
#else
#error Not implemented for current target
#endif

#ifdef TMNT2_BUILD_EU
    CMovieText::Initialize();
    CMovieText::LoadFor(movieId);
#endif /* TMNT2_BUILD_EU */
};


/*static*/ void CMovieManager::DeleteMovieInstance(void)
{
#ifdef TMNT2_BUILD_EU
    CMovieText::Terminate();
#endif /* TMNT2_BUILD_EU */
    
    ASSERT(s_pMovie);
    
    if (s_pMovie)
    {
        delete s_pMovie;
        s_pMovie = nullptr;
    };
    
    s_movieId = MOVIEID::ID_INVALID;
};


/*static*/ CMovie* CMovieManager::GetMovieInstance(void)
{
    return s_pMovie;
};


/*static*/ MOVIEID::VALUE CMovieManager::GetMovieID(void)
{
    return s_movieId;
};


/*static*/ bool CMovieManager::IsCreateSuccess(void)
{
#ifdef TMNT2_BUILD_EU        
    if (CMovieText::IsLoadEnd() && s_pMovie)
        return s_pMovie->IsCreateSuccess();    
#else /* TMNT2_BUILD_EU */    
    if (s_pMovie)
        return s_pMovie->IsCreateSuccess();    
#endif /* TMNT2_BUILD_EU */

    CDataLoader::Period();

    return false;
};


/*static*/ void CMovieManager::OnCreateSuccess(void)
{
    if (!s_pMovie)
        return;

    int32 partitionId = CMovieDataManager::GetPartitionID(s_movieId);
    int32 fileId      = CMovieDataManager::GetFileID(s_movieId);

    s_pMovie->StartAfs(partitionId, fileId);
    
#ifdef TMNT2_BUILD_EU
    CMovieText::OnLoadEnd();
#endif /* TMNT2_BUILD_EU */
};


/*static*/ bool CMovieManager::IsEnded(void)
{
    if (s_pMovie)
        return s_pMovie->IsEnded();

    return false;
};


/*static*/ void CMovieManager::Update(void)
{
    if (s_pMovie)
    {
        s_pMovie->Update();

#ifdef TMNT2_BUILD_EU
        CMovieText::UPDATEINFO updateInfo;
        updateInfo.pFrmObj = &s_pMovie->FrameObj();

        CMovieText::Update(&updateInfo);
#endif /* TMNT2_BUILD_EU */
    };
};


/*static*/ void CMovieManager::Draw(void)
{
    if (s_pMovie)
    {
        s_pMovie->Draw();

#ifdef TMNT2_BUILD_EU
        CMovieText::Draw();
#endif /* TMNT2_BUILD_EU */
    };
};