#include "PCFrameSkipController.hpp"
#include "PCTimer.hpp"

#include "System/Common/Screen.hpp"


CPCFrameSkipController::CPCFrameSkipController(void)
: m_uTime(0)
, m_uFrametime(0)
, m_uFrametimeTimeout(0)
, m_nNumSkip(0)
, m_bSkip(false)
, m_bEnable(false)
{
    double dFramerate = static_cast<double>(CScreen::Framerate());

    uint64 freq = CPCTimer::Instance().GetFreq();

    m_uFrametime = (freq / static_cast<uint64>(dFramerate));
    m_uFrametimeTimeout = (freq / static_cast<uint64>(dFramerate * 0.5));

    SetEnable(true);
};


void CPCFrameSkipController::Sync(void)
{
    uint64 uTimeNow = CPCTimer::Instance().GetElapsedTime();
    uint64 uTimeExpect = (m_uTime + (m_nNumSkip * m_uFrametime));
    uint64 uTimeElapsed = (uTimeNow - uTimeExpect);

    bool bEnable = (m_bEnable);
    bool bMaySkip = (m_nNumSkip < 5);
    bool bFrameTimeout = (uTimeElapsed > m_uFrametimeTimeout);

    if (bEnable && bMaySkip && bFrameTimeout)
    {
        m_bSkip = true;
        ++m_nNumSkip;
    }
    else
    {
        m_bSkip = false;
        m_nNumSkip = 0;
        m_uTime = uTimeNow;

        if (uTimeElapsed > m_uFrametime)
            m_uTime -= (((uTimeElapsed - m_uFrametime) / 2) % uTimeElapsed);
    };
};


void CPCFrameSkipController::SetEnable(bool bState)
{
    if (m_bEnable != bState)
    {
        m_uTime = CPCTimer::Instance().GetElapsedTime();
        m_nNumSkip = 0;
        m_bSkip = false;
    };

    m_bEnable = bState;
};


bool CPCFrameSkipController::IsEnabled(void) const
{
    return m_bEnable;
};


bool CPCFrameSkipController::IsSkip(void) const
{
    return m_bSkip;
};

