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
    uint32 m_uFrametime;
    uint32 m_uSyncTime;
};