#include "PCFrameTimer.hpp"
#include "PCTimer.hpp"
#include "PCTypedefs.hpp"
#include "PCGraphicsDevice.hpp"

#include "System/Common/Configure.hpp"
#include "System/Common/Screen.hpp"


CPCFrameTimer::CPCFrameTimer(const CPCGraphicsDevice& device)
: m_uFrametime(0)
, m_uSyncTime(0)
{
    double dRefreshRate = 60.0;
#ifdef TMNT2_BUILD_EU
    if ((CConfigure::GetTVMode() == TYPEDEF::CONFIG_TV_PAL) && device.IsPalMode())
        dRefreshRate = 50.0;
#endif /* TMNT2_BUILD_EU */

    uint64 ticksPerSec = CPCTimer::Instance().GetFreq();
    uint64 ticksPerMSec = CPCTimer::Instance().GetFreqMs();
    
    m_uFrametime  = static_cast<uint64>(static_cast<double>(ticksPerSec) / dRefreshRate);
    m_uFrametime -= (m_uFrametime % ticksPerMSec);

    Sync();
};


CPCFrameTimer::~CPCFrameTimer(void)
{
    ;
};


void CPCFrameTimer::Update(void)
{
    uint64 uTimeNow = CPCTimer::Instance().GetElapsedTime();
    uint64 uTimeElapsed = uTimeNow - m_uSyncTime;

    if (uTimeElapsed < m_uFrametime)
    {
        uint64 uSleepTime = (m_uFrametime - uTimeElapsed) / CPCTimer::Instance().GetFreqMs();
        if (uSleepTime > 4)
            Sleep(static_cast<DWORD>(uSleepTime - 1));

        while ((CPCTimer::Instance().GetElapsedTime() - m_uSyncTime) < m_uFrametime)
            Sleep(0);
    };
};


void CPCFrameTimer::Sync(void)
{
    m_uSyncTime = CPCTimer::Instance().GetElapsedTime();
};