#include "MovieDataManager.hpp"

#include "Game/Component/GameMain/MovieID.hpp"
#include "System/Common/File/AdxFileManager.hpp"

#include "cri_adxf.h"


struct MOVIE_DATA
{
    const char* m_pszFilename;
    int32 m_iPartitionID;
    int32 m_iFileID;
    uint32 m_uAttributeFlag;
};


static const MOVIE_DATA s_aMovieTable[] =
{
    { "OP_TMNT1.sfd",   0,    0,  0x1 },
    { "OP_TMNT2.sfd",   0,    1,  0x1 },
    { "m01t01.sfd",     0,    2,  0x1 },
    { "m02n01.sfd",     0,    3,  0x1 },
    { "m02n01p.sfd",    0,    4,  0x1 },
    { "m02n02.sfd",     0,    5,  0x1 },
    { "m04n01.sfd",     0,    6,  0x1 },
    { "m04n01p.sfd",    0,    7,  0x1 },
    { "m05n01p.sfd",    0,    8,  0x1 },
    { "m06r01.sfd",     0,    9,  0x1 },
    { "m09n01.sfd",     0,    10, 0x1 },
    { "m10n01.sfd",     0,    11, 0x1 },
    { "m11j01.sfd",     0,    12, 0x1 },
    { "m12n01.sfd",     0,    13, 0x1 },
    { "m14n01.sfd",     0,    14, 0x1 },
    { "m15n01.sfd",     0,    15, 0x1 },
    
    { "m17n01.sfd",     1,    0,  0x1 },
    { "m18fb01.sfd",    1,    1,  0x1 },
    { "m18fb02.sfd",    1,    2,  0x1 },
    { "m19n01p.sfd",    1,    3,  0x1 },
    { "m20fb01.sfd",    1,    4,  0x1 },
    { "m20fb02.sfd",    1,    5,  0x1 },
    { "m21n01.sfd",     1,    6,  0x1 },
    { "m21n02.sfd",     1,    7,  0x1 },
    { "m24n01.sfd",     1,    8,  0x1 },
    { "m24n02.sfd",     1,    9,  0x1 },
    { "m25j01.sfd",     1,    10, 0x1 },
    { "m27n01.sfd",     1,    11, 0x1 },
    { "m29n01.sfd",     1,    12, 0x1 },
    { "m29n02.sfd",     1,    13, 0x1 },
    { "m30nb01p.sfd",   1,    14, 0x1 },
    { "m30nb01.sfd",    1,    15, 0x1 },
    
    { "m31nb01p.sfd",   2,    0,  0x1 },
    { "m31nb01.sfd",    2,    1,  0x1 },
    { "m32r01.sfd",     2,    2,  0x1 },
    { "m33j01.sfd",     2,    3,  0x1 },
    { "m38nb01.sfd",    2,    4,  0x1 },
    { "m38nb02.sfd",    2,    5,  0x1 },
    { "m39n01.sfd",     2,    6,  0x1 },
    { "m40ob01.sfd",    2,    7,  0x1 },
    { "m40ob02.sfd",    2,    8,  0x1 },
    { "m41nb01.sfd",    2,    9,  0x1 },
    { "m42nb01p.sfd",   2,    10, 0x1 },
    { "m42nb01.sfd",    2,    11, 0x1 },
    { "m43n01.sfd",     2,    12, 0x1 },
    { "m44nb01p.sfd",   2,    13, 0x1 },
    { "m44nb01.sfd",    2,    14, 0x1 },
    
    { "m45n01.sfd",     3,    0,  0x1 },
    { "m45n02.sfd",     3,    1,  0x1 },
    { "m46r01.sfd",     3,    2,  0x1 },
    { "m47ob01.sfd",    3,    3,  0x1 },
    { "m47ob02.sfd",    3,    4,  0x1 },
    { "m48n01.sfd",     3,    5,  0x1 },
    { "m50nb01.sfd",    3,    6,  0x1 },
    { "m50nb01p.sfd",   3,    7,  0x1 },
    { "m50nb02.sfd",    3,    8,  0x1 },
    { "m52fb01p.sfd",   3,    9,  0x1 },
    { "m52fb02p.sfd",   3,    10, 0x1 },
    { "m53fb01p.sfd",   3,    11, 0x1 },
    { "m53fb02p.sfd",   3,    12, 0x1 },
    { "m54fb01p.sfd",   3,    13, 0x1 },
    { "m54fb02p.sfd",   3,    14, 0x1 },
    { "m55fb01p.sfd",   3,    15, 0x1 },
    { "m55fb02p.sfd",   3,    16, 0x1 },
    
    { "m56nb01.sfd",    4,    0,  0x1 },
    { "m56nb01p.sfd",   4,    1,  0x1 },
    { "m57nb01.sfd",    4,    2,  0x1 },
    { "m57nb02.sfd",    4,    3,  0x1 },
    { "m58ob01.sfd",    4,    4,  0x1 },
    { "m58ob02.sfd",    4,    5,  0x1 },
    { "m58ob02E.sfd",   4,    6,  0x1 },
    { "m59s01.sfd",     4,    7,  0x1 },
    { "m59s02.sfd",     4,    8,  0x1 },
    { "m59s03.sfd",     4,    9,  0x1 },
    { "m60x01p.sfd",    4,    10, 0x1 },
    { "m60x02p.sfd",    4,    11, 0x1 },
    { "m60x03p.sfd",    4,    12, 0x1 },
    
    { "m60x03E.sfd",    5,    0,  0x1 },
    { "m62x01E.sfd",    5,    1,  0x1 },
};


static_assert(COUNT_OF(s_aMovieTable) == MOVIEID::ID_MAX, "update me");


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
        //
        //  TODO pt size
        //
        s_paPartitionInfoWork[i] = new char[(1024 * 1024) * 2];// ADXF_CALC_PTINFO_REAL_SIZE(284 + pAfsData[i].m_iIncludeFileNum)];
        ASSERT(s_paPartitionInfoWork[i]);
        
        bool bResult = CAdxFileManager::Instance().LoadPartition(i + 2, pAfsData[i].m_pszFileName, s_paPartitionInfoWork[i]);
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