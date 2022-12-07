#pragma once


class CMovieDataManager
{
public:
    struct AFS_DATA
    {
        const char* m_pszFileName;
        int32 m_iIncludeFileNum;
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void LoadAfsPartition(const AFS_DATA* pAfsData, int32 nAfsDataCount);
    static void DeleteMovieAfs(void);
    static const char* GetLabel(int32 iMovieID);
    static int32 GetPartitionID(int32 iMovieID);
    static int32 GetFileID(int32 iMovieID);
    static bool IsValidMovieID(int32 iMovieID);
};