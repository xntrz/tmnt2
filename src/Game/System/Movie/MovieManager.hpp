#pragma once

#include "MovieID.hpp"


class CMovie;


class CMovieManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void PreCreateMovieInstance(MOVIEID::VALUE movieId);
    static void DeleteMovieInstance(void);
    static CMovie* GetMovieInstance(void);
    static MOVIEID::VALUE GetMovieID(void);
    static bool IsCreateSuccess(void);
    static void OnCreateSuccess(void);
    static bool IsEnded(void);
    static void Update(void);
    static void Draw(void);
};