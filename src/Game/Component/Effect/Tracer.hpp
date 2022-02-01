#pragma once


class CTracer
{
public:
    CTracer(void) {};
    virtual ~CTracer(void) {};
    virtual float GetDirection(void) = 0;
    virtual void GetPosition(RwV3d* pvPosition) = 0;
};