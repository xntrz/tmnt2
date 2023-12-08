#include "MovieDataManager.hpp"

#include "System/Common/File/AdxFileManager.hpp"

#include "cri_adxf.h"


#define MV_ATTR_FLAG_CANSKIP    (1 << 0)
#define MV_ATTR_FLAG_NOSKIP     (1 << 1)


struct MOVIE_DATA
{
    const char* m_pszFilename;
    int32 m_iPartitionID;
    int32 m_iFileID;
    uint32 m_uAttributeFlag;
};


static const MOVIE_DATA s_aMovieTable[] =
{
    { "OP_TMNT1.sfd",   0,    0,  MV_ATTR_FLAG_CANSKIP },
    { "OP_TMNT2.sfd",   0,    1,  MV_ATTR_FLAG_CANSKIP },
    { "m01t01.sfd",     0,    2,  MV_ATTR_FLAG_CANSKIP },
    { "m02n01.sfd",     0,    3,  MV_ATTR_FLAG_CANSKIP },
    { "m02n01p.sfd",    0,    4,  MV_ATTR_FLAG_CANSKIP },
    { "m02n02.sfd",     0,    5,  MV_ATTR_FLAG_CANSKIP },
    { "m04n01.sfd",     0,    6,  MV_ATTR_FLAG_CANSKIP },
    { "m04n01p.sfd",    0,    7,  MV_ATTR_FLAG_CANSKIP },
    { "m05n01p.sfd",    0,    8,  MV_ATTR_FLAG_CANSKIP },
    { "m06r01.sfd",     0,    9,  MV_ATTR_FLAG_CANSKIP },
    { "m09n01.sfd",     0,    10, MV_ATTR_FLAG_CANSKIP },
    { "m10n01.sfd",     0,    11, MV_ATTR_FLAG_CANSKIP },
    { "m11j01.sfd",     0,    12, MV_ATTR_FLAG_CANSKIP },
    { "m12n01.sfd",     0,    13, MV_ATTR_FLAG_CANSKIP },
    { "m14n01.sfd",     0,    14, MV_ATTR_FLAG_CANSKIP },
    { "m15n01.sfd",     0,    15, MV_ATTR_FLAG_CANSKIP },
    
    { "m17n01.sfd",     1,    0,  MV_ATTR_FLAG_CANSKIP },
    { "m18fb01.sfd",    1,    1,  MV_ATTR_FLAG_CANSKIP },
    { "m18fb02.sfd",    1,    2,  MV_ATTR_FLAG_CANSKIP },
    { "m19n01p.sfd",    1,    3,  MV_ATTR_FLAG_CANSKIP },
    { "m20fb01.sfd",    1,    4,  MV_ATTR_FLAG_CANSKIP },
    { "m20fb02.sfd",    1,    5,  MV_ATTR_FLAG_CANSKIP },
    { "m21n01.sfd",     1,    6,  MV_ATTR_FLAG_CANSKIP },
    { "m21n02.sfd",     1,    7,  MV_ATTR_FLAG_CANSKIP },
    { "m24n01.sfd",     1,    8,  MV_ATTR_FLAG_CANSKIP },
    { "m24n02.sfd",     1,    9,  MV_ATTR_FLAG_CANSKIP },
    { "m25j01.sfd",     1,    10, MV_ATTR_FLAG_CANSKIP },
    { "m27n01.sfd",     1,    11, MV_ATTR_FLAG_CANSKIP },
    { "m29n01.sfd",     1,    12, MV_ATTR_FLAG_CANSKIP },
    { "m29n02.sfd",     1,    13, MV_ATTR_FLAG_CANSKIP },
    { "m30nb01p.sfd",   1,    14, MV_ATTR_FLAG_CANSKIP },
    { "m30nb01.sfd",    1,    15, MV_ATTR_FLAG_CANSKIP },
    
    { "m31nb01p.sfd",   2,    0,  MV_ATTR_FLAG_CANSKIP },
    { "m31nb01.sfd",    2,    1,  MV_ATTR_FLAG_CANSKIP },
    { "m32r01.sfd",     2,    2,  MV_ATTR_FLAG_CANSKIP },
    { "m33j01.sfd",     2,    3,  MV_ATTR_FLAG_CANSKIP },
    { "m38nb01.sfd",    2,    4,  MV_ATTR_FLAG_CANSKIP },
    { "m38nb02.sfd",    2,    5,  MV_ATTR_FLAG_CANSKIP },
    { "m39n01.sfd",     2,    6,  MV_ATTR_FLAG_CANSKIP },
    { "m40ob01.sfd",    2,    7,  MV_ATTR_FLAG_CANSKIP },
    { "m40ob02.sfd",    2,    8,  MV_ATTR_FLAG_CANSKIP },
    { "m41nb01.sfd",    2,    9,  MV_ATTR_FLAG_CANSKIP },
    { "m42nb01p.sfd",   2,    10, MV_ATTR_FLAG_CANSKIP },
    { "m42nb01.sfd",    2,    11, MV_ATTR_FLAG_CANSKIP },
    { "m43n01.sfd",     2,    12, MV_ATTR_FLAG_CANSKIP },
    { "m44nb01p.sfd",   2,    13, MV_ATTR_FLAG_CANSKIP },
    { "m44nb01.sfd",    2,    14, MV_ATTR_FLAG_CANSKIP },
    
    { "m45n01.sfd",     3,    0,  MV_ATTR_FLAG_CANSKIP },
    { "m45n02.sfd",     3,    1,  MV_ATTR_FLAG_CANSKIP },
    { "m46r01.sfd",     3,    2,  MV_ATTR_FLAG_CANSKIP },
    { "m47ob01.sfd",    3,    3,  MV_ATTR_FLAG_CANSKIP },
    { "m47ob02.sfd",    3,    4,  MV_ATTR_FLAG_CANSKIP },
    { "m48n01.sfd",     3,    5,  MV_ATTR_FLAG_CANSKIP },
    { "m50nb01.sfd",    3,    6,  MV_ATTR_FLAG_CANSKIP },
    { "m50nb01p.sfd",   3,    7,  MV_ATTR_FLAG_CANSKIP },
    { "m50nb02.sfd",    3,    8,  MV_ATTR_FLAG_CANSKIP },
    { "m52fb01p.sfd",   3,    9,  MV_ATTR_FLAG_CANSKIP },
    { "m52fb02p.sfd",   3,    10, MV_ATTR_FLAG_CANSKIP },
    { "m53fb01p.sfd",   3,    11, MV_ATTR_FLAG_CANSKIP },
    { "m53fb02p.sfd",   3,    12, MV_ATTR_FLAG_CANSKIP },
    { "m54fb01p.sfd",   3,    13, MV_ATTR_FLAG_CANSKIP },
    { "m54fb02p.sfd",   3,    14, MV_ATTR_FLAG_CANSKIP },
    { "m55fb01p.sfd",   3,    15, MV_ATTR_FLAG_CANSKIP },
    { "m55fb02p.sfd",   3,    16, MV_ATTR_FLAG_CANSKIP },
    
    { "m56nb01.sfd",    4,    0,  MV_ATTR_FLAG_CANSKIP },
    { "m56nb01p.sfd",   4,    1,  MV_ATTR_FLAG_CANSKIP },
    { "m57nb01.sfd",    4,    2,  MV_ATTR_FLAG_CANSKIP },
    { "m57nb02.sfd",    4,    3,  MV_ATTR_FLAG_CANSKIP },
    { "m58ob01.sfd",    4,    4,  MV_ATTR_FLAG_CANSKIP },
    { "m58ob02.sfd",    4,    5,  MV_ATTR_FLAG_CANSKIP },
    { "m58ob02E.sfd",   4,    6,  MV_ATTR_FLAG_CANSKIP },
    { "m59s01.sfd",     4,    7,  MV_ATTR_FLAG_CANSKIP },
    { "m59s02.sfd",     4,    8,  MV_ATTR_FLAG_CANSKIP },
    { "m59s03.sfd",     4,    9,  MV_ATTR_FLAG_CANSKIP },
    { "m60x01p.sfd",    4,    10, MV_ATTR_FLAG_CANSKIP },
    { "m60x02p.sfd",    4,    11, MV_ATTR_FLAG_CANSKIP },
    { "m60x03p.sfd",    4,    12, MV_ATTR_FLAG_CANSKIP },
    
    { "m60x03E.sfd",    5,    0,  MV_ATTR_FLAG_CANSKIP },
    { "m62x01E.sfd",    5,    1,  MV_ATTR_FLAG_CANSKIP },
};


static const CMovieDataManager::AFS_DATA s_aFileTable[] =
{
    { "TMNT2V00.DAT", 16    },
    { "TMNT2V01.DAT", 16    },
    { "TMNT2V02.DAT", 15    },
    { "TMNT2V03.DAT", 17    },
    { "TMNT2V04.DAT", 13    },
    { "TMNT2V05.DAT", 2     },
};


static char** s_paPartitionInfoWork = nullptr;


static inline const MOVIE_DATA& GetMovieData(int32 iMovieID)
{
    ASSERT(iMovieID >= 0 && iMovieID < COUNT_OF(s_aMovieTable));
    return s_aMovieTable[iMovieID];
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
    ASSERT(!s_paPartitionInfoWork);

    s_paPartitionInfoWork = new char*[nAfsDataCount];
    ASSERT(s_paPartitionInfoWork);

    for (int32 i = 0; i < nAfsDataCount; ++i)
    {
        s_paPartitionInfoWork[i] = new char[ADXF_CALC_PTINFO_SIZE(pAfsData[i].m_iIncludeFileNum)];
        ASSERT(s_paPartitionInfoWork[i]);

        bool bResult = CAdxFileManager::LoadPartition(i + 2, pAfsData[i].m_pszFileName, nullptr, s_paPartitionInfoWork[i]);
        ASSERT(bResult);
    };
};


/*static*/ void CMovieDataManager::DeleteMovieAfs(void)
{
    if (s_paPartitionInfoWork)
    {
        for (int32 i = 0; i < COUNT_OF(s_aFileTable); ++i)
        {
            if (s_paPartitionInfoWork[i])
            {
                delete[] s_paPartitionInfoWork[i];
                s_paPartitionInfoWork[i] = nullptr;
            };
        };

        delete[] s_paPartitionInfoWork;
        s_paPartitionInfoWork = nullptr;
    };    
};


/*static*/ const char* CMovieDataManager::GetLabel(int32 iMovieID)
{
    return GetMovieData(iMovieID).m_pszFilename;
};


/*static*/ int32 CMovieDataManager::GetPartitionID(int32 iMovieID)
{
    return GetMovieData(iMovieID).m_iPartitionID + 2;
};


/*static*/ int32 CMovieDataManager::GetFileID(int32 iMovieID)
{
    return GetMovieData(iMovieID).m_iFileID;
};


/*static*/ bool CMovieDataManager::IsValidMovieID(int32 iMovieID)
{
    return ( (iMovieID >= 0) && (iMovieID < COUNT_OF(s_aMovieTable)) );
};