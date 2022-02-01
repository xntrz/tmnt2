#pragma once


class CGaugeResult
{
public:
    enum RESULTREQ
    {
        RESULTREQ_NONE = 0,
        RESULTREQ_SUCCESS,
        RESULTREQ_FAIL,
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(void);
    static void SetMissionResult(RESULTREQ req);
    static void SetMissionResult(RESULTREQ req, int32 nPlayerNo);
    static bool IsMissionResultEnd(void);
};