#include "MovieDataManager.hpp"

#include "System/Common/File/AdxFileManager.hpp"

#include "cri_adxf.h"


#define MV_ATTRIBUTE_SKIP    (1 << 0)


struct MOVIE_DATA
{
    const char* pszFilename;
    int32       partitionId;
    int32       fileId;
    uint32      attribute;
};


static const MOVIE_DATA s_aMovieTable[] =
{
    //  filename        partitionId     fileId      attribute
    //-------------------------------------------------------------------
    {   "OP_TMNT1.sfd", 0,              0,          MV_ATTRIBUTE_SKIP },
    {   "OP_TMNT2.sfd", 0,              1,          MV_ATTRIBUTE_SKIP },
    {   "m01t01.sfd",   0,              2,          MV_ATTRIBUTE_SKIP },
    {   "m02n01.sfd",   0,              3,          MV_ATTRIBUTE_SKIP },
    {   "m02n01p.sfd",  0,              4,          MV_ATTRIBUTE_SKIP },
    {   "m02n02.sfd",   0,              5,          MV_ATTRIBUTE_SKIP },
    {   "m04n01.sfd",   0,              6,          MV_ATTRIBUTE_SKIP },
    {   "m04n01p.sfd",  0,              7,          MV_ATTRIBUTE_SKIP },
    {   "m05n01p.sfd",  0,              8,          MV_ATTRIBUTE_SKIP },
    {   "m06r01.sfd",   0,              9,          MV_ATTRIBUTE_SKIP },
    {   "m09n01.sfd",   0,              10,         MV_ATTRIBUTE_SKIP },
    {   "m10n01.sfd",   0,              11,         MV_ATTRIBUTE_SKIP },
    {   "m11j01.sfd",   0,              12,         MV_ATTRIBUTE_SKIP },
    {   "m12n01.sfd",   0,              13,         MV_ATTRIBUTE_SKIP },
    {   "m14n01.sfd",   0,              14,         MV_ATTRIBUTE_SKIP },
    {   "m15n01.sfd",   0,              15,         MV_ATTRIBUTE_SKIP },
    
    {   "m17n01.sfd",   1,              0,          MV_ATTRIBUTE_SKIP },
    {   "m18fb01.sfd",  1,              1,          MV_ATTRIBUTE_SKIP },
    {   "m18fb02.sfd",  1,              2,          MV_ATTRIBUTE_SKIP },
    {   "m19n01p.sfd",  1,              3,          MV_ATTRIBUTE_SKIP },
    {   "m20fb01.sfd",  1,              4,          MV_ATTRIBUTE_SKIP },
    {   "m20fb02.sfd",  1,              5,          MV_ATTRIBUTE_SKIP },
    {   "m21n01.sfd",   1,              6,          MV_ATTRIBUTE_SKIP },
    {   "m21n02.sfd",   1,              7,          MV_ATTRIBUTE_SKIP },
    {   "m24n01.sfd",   1,              8,          MV_ATTRIBUTE_SKIP },
    {   "m24n02.sfd",   1,              9,          MV_ATTRIBUTE_SKIP },
    {   "m25j01.sfd",   1,              10,         MV_ATTRIBUTE_SKIP },
    {   "m27n01.sfd",   1,              11,         MV_ATTRIBUTE_SKIP },
    {   "m29n01.sfd",   1,              12,         MV_ATTRIBUTE_SKIP },
    {   "m29n02.sfd",   1,              13,         MV_ATTRIBUTE_SKIP },
    {   "m30nb01p.sfd", 1,              14,         MV_ATTRIBUTE_SKIP },
    {   "m30nb01.sfd",  1,              15,         MV_ATTRIBUTE_SKIP },
    
    {   "m31nb01p.sfd", 2,              0,          MV_ATTRIBUTE_SKIP },
    {   "m31nb01.sfd",  2,              1,          MV_ATTRIBUTE_SKIP },
    {   "m32r01.sfd",   2,              2,          MV_ATTRIBUTE_SKIP },
    {   "m33j01.sfd",   2,              3,          MV_ATTRIBUTE_SKIP },
    {   "m38nb01.sfd",  2,              4,          MV_ATTRIBUTE_SKIP },
    {   "m38nb02.sfd",  2,              5,          MV_ATTRIBUTE_SKIP },
    {   "m39n01.sfd",   2,              6,          MV_ATTRIBUTE_SKIP },
    {   "m40ob01.sfd",  2,              7,          MV_ATTRIBUTE_SKIP },
    {   "m40ob02.sfd",  2,              8,          MV_ATTRIBUTE_SKIP },
    {   "m41nb01.sfd",  2,              9,          MV_ATTRIBUTE_SKIP },
    {   "m42nb01p.sfd", 2,              10,         MV_ATTRIBUTE_SKIP },
    {   "m42nb01.sfd",  2,              11,         MV_ATTRIBUTE_SKIP },
    {   "m43n01.sfd",   2,              12,         MV_ATTRIBUTE_SKIP },
    {   "m44nb01p.sfd", 2,              13,         MV_ATTRIBUTE_SKIP },
    {   "m44nb01.sfd",  2,              14,         MV_ATTRIBUTE_SKIP },
    
    {   "m45n01.sfd",   3,              0,          MV_ATTRIBUTE_SKIP },
    {   "m45n02.sfd",   3,              1,          MV_ATTRIBUTE_SKIP },
    {   "m46r01.sfd",   3,              2,          MV_ATTRIBUTE_SKIP },
    {   "m47ob01.sfd",  3,              3,          MV_ATTRIBUTE_SKIP },
    {   "m47ob02.sfd",  3,              4,          MV_ATTRIBUTE_SKIP },
    {   "m48n01.sfd",   3,              5,          MV_ATTRIBUTE_SKIP },
    {   "m50nb01.sfd",  3,              6,          MV_ATTRIBUTE_SKIP },
    {   "m50nb01p.sfd", 3,              7,          MV_ATTRIBUTE_SKIP },
    {   "m50nb02.sfd",  3,              8,          MV_ATTRIBUTE_SKIP },
    {   "m52fb01p.sfd", 3,              9,          MV_ATTRIBUTE_SKIP },
    {   "m52fb02p.sfd", 3,              10,         MV_ATTRIBUTE_SKIP },
    {   "m53fb01p.sfd", 3,              11,         MV_ATTRIBUTE_SKIP },
    {   "m53fb02p.sfd", 3,              12,         MV_ATTRIBUTE_SKIP },
    {   "m54fb01p.sfd", 3,              13,         MV_ATTRIBUTE_SKIP },
    {   "m54fb02p.sfd", 3,              14,         MV_ATTRIBUTE_SKIP },
    {   "m55fb01p.sfd", 3,              15,         MV_ATTRIBUTE_SKIP },
    {   "m55fb02p.sfd", 3,              16,         MV_ATTRIBUTE_SKIP },
    
    {   "m56nb01.sfd",  4,              0,          MV_ATTRIBUTE_SKIP },
    {   "m56nb01p.sfd", 4,              1,          MV_ATTRIBUTE_SKIP },
    {   "m57nb01.sfd",  4,              2,          MV_ATTRIBUTE_SKIP },
    {   "m57nb02.sfd",  4,              3,          MV_ATTRIBUTE_SKIP },
    {   "m58ob01.sfd",  4,              4,          MV_ATTRIBUTE_SKIP },
    {   "m58ob02.sfd",  4,              5,          MV_ATTRIBUTE_SKIP },
    {   "m58ob02E.sfd", 4,              6,          MV_ATTRIBUTE_SKIP },
    {   "m59s01.sfd",   4,              7,          MV_ATTRIBUTE_SKIP },
    {   "m59s02.sfd",   4,              8,          MV_ATTRIBUTE_SKIP },
    {   "m59s03.sfd",   4,              9,          MV_ATTRIBUTE_SKIP },
    {   "m60x01p.sfd",  4,              10,         MV_ATTRIBUTE_SKIP },
    {   "m60x02p.sfd",  4,              11,         MV_ATTRIBUTE_SKIP },
    {   "m60x03p.sfd",  4,              12,         MV_ATTRIBUTE_SKIP },
    
    {   "m60x03E.sfd",  5,              0,          MV_ATTRIBUTE_SKIP },
    {   "m62x01E.sfd",  5,              1,          MV_ATTRIBUTE_SKIP },
};


static_assert(COUNT_OF(s_aMovieTable) == MOVIEID::ID_MAX, "update table");


static const CMovieDataManager::AFS_DATA s_aFileTable[] =
{
    { "TMNT2V00.DAT", 16    },
    { "TMNT2V01.DAT", 16    },
    { "TMNT2V02.DAT", 15    },
    { "TMNT2V03.DAT", 17    },
    { "TMNT2V04.DAT", 13    },
    { "TMNT2V05.DAT", 2     },
};


static char** s_apPartitionInfoWork = nullptr;


static inline const MOVIE_DATA& GetMovieData(MOVIEID::VALUE movieId)
{
    ASSERT(movieId >= 0);
    ASSERT(movieId < COUNT_OF(s_aMovieTable));

    return s_aMovieTable[movieId];
};


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

        bool bResult = CAdxFileManager::LoadPartition(i + 2, pAfsData[i].pszFileName, nullptr, s_apPartitionInfoWork[i]);
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


/*static*/ const char* CMovieDataManager::GetLabel(MOVIEID::VALUE movieId)
{
    return GetMovieData(movieId).pszFilename;
};


/*static*/ int32 CMovieDataManager::GetPartitionID(MOVIEID::VALUE movieId)
{
    return GetMovieData(movieId).partitionId + 2;
};


/*static*/ int32 CMovieDataManager::GetFileID(MOVIEID::VALUE movieId)
{
    return GetMovieData(movieId).fileId;
};


/*static*/ bool CMovieDataManager::IsValidMovieID(MOVIEID::VALUE movieId)
{
    return ( (movieId >= 0) && (movieId < COUNT_OF(s_aMovieTable)) );
};