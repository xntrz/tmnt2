#pragma once


class CPCFrameSkipController final
{
public:
    CPCFrameSkipController(void);
    void Sync(void);
    void SetEnable(bool bState);
    bool IsEnabled(void) const;
    bool IsSkip(void) const;

private:
    uint64  m_uTime;
    uint64  m_uFrametime;
    uint64  m_uFrametimeTimeout;
    int32   m_nNumSkip;
    bool    m_bSkip;
    bool    m_bEnable;
};