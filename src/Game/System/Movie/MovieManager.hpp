#pragma once


class CMovie;


class CMovieManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void PreCreateMovieInstance(MVNAME mvname);
    static void DeleteMovieInstance(void);
    static CMovie* GetMovieInstance(void);
    static MVNAME GetMovieID(void);
    static bool IsCreateSuccess(void);
    static void OnCreateSuccess(void);
    static bool IsEnded(void);
    static void Update(void);
    static void Draw(void);

private:
    static CMovie* s_pMovie;
    static MVNAME s_preCreateMvName;
};