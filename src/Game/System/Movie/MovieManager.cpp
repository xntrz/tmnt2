#include "MovieManager.hpp"
#include "MovieDataManager.hpp"
#include "MovieText.hpp"

#include "Game/System/DataLoader/DataLoader.hpp"
#include "System/Common/File/FileData.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Movie.hpp"

#ifdef TARGET_PC
#include "System/PC/PCMovie.hpp"
#else
#error Not implemented for current target
#endif


/*static*/ CMovie* CMovieManager::s_pMovie = nullptr;
/*static*/ MVNAME CMovieManager::s_preCreateMvName = MVNAME_INVALID;


/*static*/ void CMovieManager::Initialize(void)
{
    CMovieDataManager::Initialize();
};


/*static*/ void CMovieManager::Terminate(void)
{
    CMovieDataManager::Terminate();
};


/*static*/ void CMovieManager::PreCreateMovieInstance(MVNAME mvname)
{
    ASSERT(CMovieDataManager::IsValidMovie(mvname));

    if (!CMovieDataManager::IsValidMovie(mvname))
    {
        s_pMovie = nullptr;
        return;
    };

    s_preCreateMvName = mvname;

    ASSERT(!s_pMovie);

    bool bUsePalMode = false;
#ifdef TMNT2_BUILD_EU
    if (CConfigure::GetTVMode() == TYPEDEF::CONFIG_TV_PAL)
        bUsePalMode = true;
#endif /* TMNT2_BUILD_EU */

#ifdef TARGET_PC
    s_pMovie = new CPCMovie(640, 448, MV_BPS, bUsePalMode);
#else
#error Not implemented for current target
#endif

#ifdef TMNT2_BUILD_EU
    CMovieText::Initialize();
    CMovieText::LoadFor(mvname);
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

    s_preCreateMvName = MVNAME_INVALID;
};


/*static*/ CMovie* CMovieManager::GetMovieInstance(void)
{
    return s_pMovie;
};


/*static*/ MVNAME CMovieManager::GetMovieID(void)
{
    return s_preCreateMvName;
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

#ifdef TMNT2_IDFSYST
    const CMovieDataManager::MOVIE_DATA* pMovieData = CMovieDataManager::GetMovieData(s_preCreateMvName);
    ASSERT(pMovieData != nullptr);

    s_pMovie->StartAfs(pMovieData->ptid, pMovieData->fid);
#else /* TMNT2_IDFSYST */
    if (CFileAccessFname::IsExist(s_preCreateMvName))
    {
        s_pMovie->StartFname(s_preCreateMvName);
        OUTPUT("Playing \"%s\" movie from disc\n", s_preCreateMvName);
    }
    else
    {        
        const CMovieDataManager::MOVIE_DATA* pMovieData = CMovieDataManager::GetMovieDataName(s_preCreateMvName);
        ASSERT(pMovieData != nullptr);

        s_pMovie->StartAfs(pMovieData->ptid, pMovieData->fid);
        OUTPUT("Playing \"%s\" movie from AFS\n", s_preCreateMvName);
    };
#endif /* TMNT2_IDFSYST */
    
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