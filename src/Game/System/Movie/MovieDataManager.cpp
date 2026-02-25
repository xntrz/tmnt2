#include "MovieDataManager.hpp"

#include "System/Common/File/File.hpp"

#include "cri_adxf.h"


#if (defined(TARGET_PC))

    #if (defined(TMNT2_BUILD_EU))
        #include "MovieDataManagerTable_PC_EU.hpp"
    #else
        #include "MovieDataManagerTable_PC_NA.hpp"
    #endif

#else 

#error "Not implemented for current target"

#endif


#define MOVIE_BASE_PT (2)


#define GetMovieDataReal(index)                         \
    (s_aMovieTableReal[index] = s_aMovieTable[index],   \
     s_aMovieTableReal[index].ptid += MOVIE_BASE_PT,    \
     &s_aMovieTableReal[index])


/*static*/ CMovieDataManager::MOVIE_DATA CMovieDataManager::s_aMovieTableReal[COUNT_OF(s_aMovieTable)];
/*static*/ char** CMovieDataManager::s_apPartitionInfoWork = nullptr;


/*static*/ void CMovieDataManager::Initialize(void)
{
    LoadAfsPartition(s_aFileTable, COUNT_OF(s_aFileTable));
};


/*static*/ void CMovieDataManager::Terminate(void)
{
    DeleteMovieAfs();
};


/*static*/ void CMovieDataManager::LoadAfsPartition(const AFS_DATA* pAfsData, int32 nAfsDataCount)
{
    ASSERT(!s_apPartitionInfoWork);

    s_apPartitionInfoWork = new char*[nAfsDataCount];
    ASSERT(s_apPartitionInfoWork);

    for (int32 i = 0; i < nAfsDataCount; ++i)
    {
        s_apPartitionInfoWork[i] = new char[ADXF_CALC_PTINFO_SIZE(pAfsData[i].includeFileNum)];
        ASSERT(s_apPartitionInfoWork[i]);

        bool bResult = CFile::LoadPartition(i + MOVIE_BASE_PT, pAfsData[i].pszFileName, nullptr, s_apPartitionInfoWork[i]);
        ASSERT(bResult);
    };
};


/*static*/ void CMovieDataManager::DeleteMovieAfs(void)
{
    if (s_apPartitionInfoWork)
    {
        for (int32 i = 0; i < COUNT_OF(s_aFileTable); ++i)
        {
            if (s_apPartitionInfoWork[i])
            {
                delete[] s_apPartitionInfoWork[i];
                s_apPartitionInfoWork[i] = nullptr;
            };
        };

        delete[] s_apPartitionInfoWork;
        s_apPartitionInfoWork = nullptr;
    };    
};


/*static*/ bool CMovieDataManager::IsValidMovie(MVNAME mvname)
{
    return (GetMovieDataName(mvname) != nullptr);
};


/*static*/ int32 CMovieDataManager::GetMovieDataNum(void)
{
    return COUNT_OF(s_aMovieTable);
};


/*static*/ const CMovieDataManager::MOVIE_DATA*
CMovieDataManager::GetMovieData(int32 id)
{
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(s_aMovieTable));

    return GetMovieDataReal(id);
};


/*static*/ const CMovieDataManager::MOVIE_DATA*
CMovieDataManager::GetMovieDataName(MVNAME mvname)
{
#ifdef TMNT2_IDFSYST
    return GetMovieData(mvname);    
#else /* TMNT2_IDFSYST */
    for (int32 i = 0; i < COUNT_OF(s_aMovieTable); ++i)
    {
        if (!std::strcmp(s_aMovieTable[i].pszFilename, mvname))
            return GetMovieDataReal(i);
    };

    return nullptr;
#endif /* TMNT2_IDFSYST */
};