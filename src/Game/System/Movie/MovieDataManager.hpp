#pragma once


class CMovieDataManager
{
public:
    struct AFS_DATA
    {
        const char* pszFileName;
        int32       includeFileNum;
    };

    enum MOVIE_ATTRIBUTE
    {
        MOVIE_ATTRIBUTE_SKIP = (1 << 0),
    };

    struct MOVIE_DATA
    {
        const char* pszFilename;
        int32       ptid;
        int32       fid;
        uint32      attribute;
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void LoadAfsPartition(const AFS_DATA* pAfsData, int32 nAfsDataCount);
    static void DeleteMovieAfs(void);
    static bool IsValidMovie(MVNAME mvname);
    static int32 GetMovieDataNum(void);
    static const MOVIE_DATA* GetMovieData(int32 id);
    static const MOVIE_DATA* GetMovieDataName(MVNAME mvname);

private:
    static const AFS_DATA s_aFileTable[];
    static const MOVIE_DATA s_aMovieTable[];
    static MOVIE_DATA s_aMovieTableReal[];
    static char** s_apPartitionInfoWork;
};