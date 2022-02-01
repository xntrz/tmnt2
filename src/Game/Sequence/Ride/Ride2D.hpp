#pragma once

#include "RideTypes.hpp"


class CRide2D
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(void);
    static void DrawGoldMedal(float fScale, float fAlpha);
    static void DrawSilverMedal(float fScale, float fAlpha);
    static void AddMedalAnimation(RIDETYPES::SCOREKIND scorekind);
    static void DrawPersonalInfo(void);
    static void DrawPersonalWindow(float fOfsX, int32 nPlayerNo);
    static void DrawPersonalPlayer(float fOfsX, int32 nPlayerNo);
    static void DrawPersonalMedal(float fOfsX);
    static void DrawPersonalScore(float x, float y, int32 nValue);
    static void DrawPersonalNumber(float x, float y, int32 nValue);
    static void DrawGoldScore(void);
    static void DrawSilverScore(void);
    static void DrawScore(float x, float y, int32 nValue);
    static void DrawNumber(float x, float y, int32 nValue);
    static void DrawMedalAnimation(void);
};