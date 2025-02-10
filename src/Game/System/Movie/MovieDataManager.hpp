#pragma once

#include "MovieID.hpp"


class CMovieDataManager
{
public:
    struct AFS_DATA
    {
        const char* pszFileName;
        int32       includeFileNum;
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void LoadAfsPartition(const AFS_DATA* pAfsData, int32 nAfsDataCount);
    static void DeleteMovieAfs(void);
    static const char* GetLabel(MOVIEID::VALUE movieId);
    static int32 GetPartitionID(MOVIEID::VALUE movieId);
    static int32 GetFileID(MOVIEID::VALUE movieId);
    static bool IsValidMovieID(MOVIEID::VALUE movieId);
};