#pragma once


class CPCFrameTimer
{
public:
    CPCFrameTimer(void);
    virtual ~CPCFrameTimer(void);
    void Update(void);
    void Sync(void);

private:
    uint32 m_uFrametime;
    uint32 m_uSyncTime;
};