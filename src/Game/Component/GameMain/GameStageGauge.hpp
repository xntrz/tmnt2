#pragma once


class IGameStageExGauge
{
public:
    virtual bool Start(void) = 0;
    virtual void Stop(void) = 0;
    virtual void Update(void) = 0;
};