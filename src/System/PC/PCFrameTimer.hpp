#pragma once


class CPCFrameTimer
{
public:
    CPCFrameTimer(void);
    virtual ~CPCFrameTimer(void);
    void Update(void);
    void Sync(void);
    uint32 FPS(void) const;
    uint32 Frametime(void) const;
    
private:
    uint32 m_uFrametime;
    uint32 m_uSyncTime;
};