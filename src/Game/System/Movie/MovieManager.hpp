#pragma once

class CMovie;

class CMovieManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void PreCreateMovieInstance(int32 iMovieID);
    static CMovie* GetMovieInstance(void);
    static int32 GetMovieID(void);
    static void DeleteMovieInstance(void);
    static bool IsCreateSuccess(void);
};