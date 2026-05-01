#pragma once


class CPCGraphicsDevice;


class CPCFrameTimer
{
public:
    CPCFrameTimer(const CPCGraphicsDevice& device);
    virtual ~CPCFrameTimer(void);
    void Update(void);
    void Sync(void);

private:
    uint64 m_uFrametime;
    uint64 m_uSyncTime;
};