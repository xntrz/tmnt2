#include "PCFrameTimer.hpp"
#include "PCTimer.hpp"
#include "PCTypedefs.hpp"
#include "PCGraphicsDevice.hpp"

#include "System/Common/Configure.hpp"
#include "System/Common/Screen.hpp"


CPCFrameTimer::CPCFrameTimer(CPCGraphicsDevice& device)
: m_uFrametime(0)
, m_uSyncTime(0)
{
    float fRefreshRate = 60.0f;
#ifdef TMNT2_BUILD_EU
    if ((CConfigure::GetTVMode() == TYPEDEF::CONFIG_TV_PAL) && device.IsPalMode())
        fRefreshRate = 50.0f;
#endif /* TMNT2_BUILD_EU */

    uint32 ms = CPCTimer::Instance().GranularityMillisecond();
    uint32 sec = CPCTimer::Instance().GranularitySecond();
    
    m_uFrametime  = static_cast<uint32>(static_cast<float>(ms) / fRefreshRate);
	m_uFrametime -= (m_uFrametime % sec);

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