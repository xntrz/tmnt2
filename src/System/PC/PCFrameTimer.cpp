#include "PCFrameTimer.hpp"
#include "PCTimer.hpp"
#include "PCTypedefs.hpp"

#include "System/Common/Screen.hpp"


CPCFrameTimer::CPCFrameTimer(void)
: m_uFrametime(0)
, m_uSyncTime(0)
{
	m_uFrametime = uint32(float(CPCTimer::Instance().GranularityMillisecond()) * CScreen::TimerStride());
	m_uFrametime -= (m_uFrametime % CPCTimer::Instance().GranularitySecond());
    Sync();    
};


CPCFrameTimer::~CPCFrameTimer(void)
{
    ;
};


void CPCFrameTimer::Update(void)
{
    uint32 uTimeNow = CPCTimer::Instance().ElapsedTime();
    uint32 uTimeElapsed = uTimeNow - m_uSyncTime;

    if (uTimeElapsed < m_uFrametime)
    {
        uint32 uSleepTime = (m_uFrametime - uTimeElapsed) / CPCTimer::Instance().GranularitySecond();
        if (uSleepTime > 4)
            Sleep(uSleepTime - 1);

        while ((CPCTimer::Instance().ElapsedTime() - m_uSyncTime) < m_uFrametime)
            Sleep(0);
    };
};


void CPCFrameTimer::Sync(void)
{
    m_uSyncTime = CPCTimer::Instance().ElapsedTime();
};