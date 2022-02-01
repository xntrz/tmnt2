#include "PCMonitor.hpp"
#include "PCTypedefs.hpp"


namespace
{
    struct MONITORDATA
    {
        static const int32 DEVICENAME_MAX = 32;
        
        HMONITOR m_handle;
        int32 m_iOffsetX;
        int32 m_iOffsetY;
        int32 m_iWidth;
        int32 m_iHeight;
        int32 m_iWorkWidth;
        int32 m_iWorkHeight;
        char m_szDeviceName[DEVICENAME_MAX];
    };
};


static MONITORDATA* s_paMonitorData = nullptr;
static int32 s_iNumMonitorData = 0;
static int32 s_iPrimaryMonitorNo = 0;


static inline const MONITORDATA& GetMonitorData(int32 iMonitorNo)
{
    ASSERT(s_paMonitorData);
    ASSERT(iMonitorNo >= 0 && iMonitorNo < s_iNumMonitorData);

    return s_paMonitorData[iMonitorNo];
};


/*static*/ bool CPCMonitor::Initialize(void)
{
    if (s_paMonitorData)
        return true;

    const POINT Point = { 0, 0 };
    
    HMONITOR hPrimaryMonitor = MonitorFromPoint(Point, MONITOR_DEFAULTTOPRIMARY);
    if (hPrimaryMonitor == NULL)
        return false;

    struct ENUMPARAM
    {      
        int32 m_num;
        HMONITOR m_aHMonitor[32];
    };

    auto fnEnumMonitorsCallback = [](HMONITOR hMonitor, HDC hDC, LPRECT lpRecv, LPARAM param) -> BOOL
    {
        ENUMPARAM* pEnumPara = (ENUMPARAM*)param;
        ASSERT(pEnumPara->m_num < COUNT_OF(ENUMPARAM::m_aHMonitor));

        pEnumPara->m_aHMonitor[pEnumPara->m_num++] = hMonitor;

        return TRUE;
    };

    ENUMPARAM EnumParam = { 0, { 0 } };
    if (!EnumDisplayMonitors(NULL, NULL, fnEnumMonitorsCallback, LPARAM(&EnumParam)))
        return false;

    s_iNumMonitorData = EnumParam.m_num;
    s_iPrimaryMonitorNo = -1;
    s_paMonitorData = new MONITORDATA[s_iNumMonitorData];
    if (!s_paMonitorData)
        return false;
    
    //
    //  Set DPI aware for disable getting modified monitor size if UI scale is set
    //    
    SetProcessDPIAware();
    
    int32 iMonitorNo = 0;
    while (iMonitorNo < EnumParam.m_num)
    {
        MONITORINFOEXA MonitorInfoEx;
        std::memset(&MonitorInfoEx, 0x00, sizeof(MonitorInfoEx));
        MonitorInfoEx.cbSize = sizeof(MonitorInfoEx);

        if (!GetMonitorInfoA(EnumParam.m_aHMonitor[iMonitorNo], &MonitorInfoEx))
            return false;

        if (EnumParam.m_aHMonitor[iMonitorNo] == hPrimaryMonitor)
            s_iPrimaryMonitorNo = iMonitorNo;

        s_paMonitorData[iMonitorNo].m_handle = EnumParam.m_aHMonitor[iMonitorNo];
        s_paMonitorData[iMonitorNo].m_iWidth = MonitorInfoEx.rcMonitor.right - MonitorInfoEx.rcMonitor.left;
        s_paMonitorData[iMonitorNo].m_iHeight = MonitorInfoEx.rcMonitor.bottom - MonitorInfoEx.rcMonitor.top;
        s_paMonitorData[iMonitorNo].m_iWorkWidth = MonitorInfoEx.rcWork.right - MonitorInfoEx.rcWork.left;
        s_paMonitorData[iMonitorNo].m_iWorkHeight = MonitorInfoEx.rcWork.bottom - MonitorInfoEx.rcWork.top;
        s_paMonitorData[iMonitorNo].m_iOffsetX = MonitorInfoEx.rcMonitor.left;
        s_paMonitorData[iMonitorNo].m_iOffsetY = MonitorInfoEx.rcMonitor.top;

        ++iMonitorNo;
    };

    //
    //  Find monitor that name matching device
    //
    DISPLAY_DEVICEA DisplayDevice;
    std::memset(&DisplayDevice, 0x00, sizeof(DisplayDevice));
    DisplayDevice.cb = sizeof(DisplayDevice);

    int32 iDeviceNo = 0;
    while (EnumDisplayDevicesA(0, iDeviceNo, &DisplayDevice, 0) && iDeviceNo < s_iNumMonitorData)
    {
        std::string DeviceName = DisplayDevice.DeviceName;
        if (EnumDisplayDevicesA(DeviceName.c_str(), 0, &DisplayDevice, 0))
        {
            std::string DeviceID = DisplayDevice.DeviceID;
            std::size_t posBegin = DeviceID.find_first_of('\\');
            std::size_t posEnd = DeviceID.find_first_of('\\', posBegin + 1);

            if ((posBegin != std::string::npos) && (posEnd != std::string::npos))
            {
                ++posBegin;

                DeviceName = DeviceID.substr(posBegin, posEnd - posBegin);

                ASSERT(DeviceName.size() < sizeof(MONITORDATA::m_szDeviceName));
                std::strcpy(s_paMonitorData[iDeviceNo].m_szDeviceName, &DeviceName[0]);
            };
        }
        else
        {
            return false;
        };

		++iDeviceNo;
    };
    
    return true;
};


/*static*/ void CPCMonitor::Terminate(void)
{
    if (s_paMonitorData)
    {
        delete[] s_paMonitorData;
        s_paMonitorData = nullptr;
        s_iPrimaryMonitorNo = -1;
        s_iNumMonitorData = 0;
    };
};


/*static*/ int32 CPCMonitor::FindMonitor(int32 iWidth, int32 iHeight)
{
    ASSERT(s_paMonitorData);
    
    for (int32 i = 0; i < s_iNumMonitorData; ++i)
    {
        MONITORDATA* pMonitorData = &s_paMonitorData[i];
        
        if (pMonitorData->m_iWidth == iWidth &&
            pMonitorData->m_iHeight == iHeight)
            return i;
    };

    return -1;
};


/*static*/ int32 CPCMonitor::GetPrimary(void)
{
    return s_iPrimaryMonitorNo;
};


/*static*/ int32 CPCMonitor::GetNum(void)
{
    return s_iNumMonitorData;
};


/*static*/ int32 CPCMonitor::GetWidth(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_iWidth;    
};


/*static*/ int32 CPCMonitor::GetHeight(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_iHeight;
};


/*static*/ int32 CPCMonitor::GetOffsetX(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_iOffsetX;
};


/*static*/ int32 CPCMonitor::GetOffsetY(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_iOffsetY;
};


/*static*/ int32 CPCMonitor::GetWorkWidth(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_iWorkWidth;
};


/*static*/ int32 CPCMonitor::GetWorkHeight(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_iWorkHeight;
};


/*static*/ const char* CPCMonitor::GetName(int32 iMonitorNo)
{
    return GetMonitorData(iMonitorNo).m_szDeviceName;
};

