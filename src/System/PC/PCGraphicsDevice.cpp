#include "PCGraphicsDevice.hpp"
#include "PCFrameTimer.hpp"
#include "PCSpecific.hpp"
#include "PCSetting.hpp"
#include "PCMonitor.hpp"
#include "PCUtils.hpp"


struct CPCGraphicsDevice::VIDEOMODE : public RwVideoMode
{
    VIDEOMODE(void);

    bool m_bWindow;
    int32 m_iNo;
    int32 m_iMaxMultisamplingLevels;
    char m_szName[64];
};


struct CPCGraphicsDevice::DEVICE : public RwSubSystemInfo
{
    DEVICE(void);
    VIDEOMODE& Videomode(void);
    VIDEOMODE& Videomode(int32 iIndex);
    int32 Videomode(int32 iWidth, int32 iHeight);

    VIDEOMODE* m_paVideomode;
    int32 m_numVideomode;
    int32 m_curVideomode;
    int32 m_iMultiSamplingLevel;
    int32 m_iWindowVideomodeNo;
    int32 m_iWindowVideomodeIndex;
    int32 m_iMonitorWidthWindow;
    int32 m_iMonitorHeightWindow;
    int32 m_iMonitorWidthFullscreen;
    int32 m_iMonitorHeightFullscreen;
    int32 m_iMonitorWidth;
    int32 m_iMonitorHeight;
    int32 m_iMonitorX;
    int32 m_iMonitorY;
    int32 m_iMonitorNo;
    const char* m_pszMonitorName;
};


CPCGraphicsDevice::VIDEOMODE::VIDEOMODE(void)
: m_iNo(-1)
, m_iMaxMultisamplingLevels(0)
{
    m_szName[0] = '\0';    
};


CPCGraphicsDevice::DEVICE::DEVICE(void)
: m_paVideomode(nullptr)
, m_numVideomode(0)
, m_curVideomode(-1)
, m_iMultiSamplingLevel(0)
, m_iWindowVideomodeNo(-1)   
{
    ;
};


CPCGraphicsDevice::VIDEOMODE& CPCGraphicsDevice::DEVICE::Videomode(void)
{
    return Videomode(m_curVideomode);
};


CPCGraphicsDevice::VIDEOMODE& CPCGraphicsDevice::DEVICE::Videomode(int32 iIndex)
{
    ASSERT(iIndex >= 0 && iIndex < m_numVideomode);
    return *&m_paVideomode[iIndex];
};


int32 CPCGraphicsDevice::DEVICE::Videomode(int32 iWidth, int32 iHeight)
{
    for (int32 i = 0; i < m_numVideomode; ++i)
    {
        if (m_paVideomode[i].width == iWidth &&
            m_paVideomode[i].height == iHeight)
            return i;
    };

    return -1;
};


CPCGraphicsDevice::CPCGraphicsDevice(void)
: m_pFrameTimer(nullptr)
, m_paDevice(nullptr)
, m_numDevice(0)
, m_curDevice(-1)
{
    ;
};


CPCGraphicsDevice::~CPCGraphicsDevice(void)
{
    DeviceCleanup();
};


bool CPCGraphicsDevice::Initialize(void)
{
    if (!CPCMonitor::Initialize())
        return false;

    if (!CGraphicsDevice::Initialize())
        return false;

    m_pFrameTimer = new CPCFrameTimer;
    if (!m_pFrameTimer)
        return false;

    ASSERT(m_curDevice != -1);
    DEVICE* pDevice = &m_paDevice[m_curDevice];

    ConfigureUpdate(ScreenWidth(), ScreenHeight());

    return true;
};


void CPCGraphicsDevice::Terminate(void)
{
    CPCSetting::m_videomode.m_iWidth = ScreenWidth();
    CPCSetting::m_videomode.m_iHeight = ScreenHeight();
    CPCSetting::m_videomode.m_iDepth = ScreenDepth();
    
    if (m_pFrameTimer)
    {
        delete m_pFrameTimer;
        m_pFrameTimer = nullptr;
    };

    CPCMonitor::Terminate();
    CGraphicsDevice::Terminate();
};


bool CPCGraphicsDevice::Start(void)
{
    if (CGraphicsDevice::Start())
    {
        RwImageSetGamma(1.2f);
        return true;
    };

    return false;
};


void CPCGraphicsDevice::Flip(void)
{
    ASSERT(m_pFrameTimer);    

    m_pFrameTimer->Update();
    CGraphicsDevice::Flip();
    m_pFrameTimer->Sync();
};


int32 CPCGraphicsDevice::ScreenWidth(void)
{
    switch (m_displaymode)
    {
    case DISPLAYMODE_FULLSCREEN:
        return Device().Videomode().width;

    case DISPLAYMODE_FULLWINDOW:
        return Device().m_iMonitorWidthFullscreen;

    case DISPLAYMODE_WINDOW:
        if (m_pFrameBuffer)
            return RwRasterGetWidth(m_pFrameBuffer);
        else
            return Device().Videomode().width;
        break;

    default:
        ASSERT(false);
        return Device().Videomode().width;
    };
};


int32 CPCGraphicsDevice::ScreenHeight(void)
{
    switch (m_displaymode)
    {
    case DISPLAYMODE_FULLSCREEN:
        return Device().Videomode().height;

    case DISPLAYMODE_FULLWINDOW:
        return Device().m_iMonitorHeightFullscreen;

    case DISPLAYMODE_WINDOW:
        if (m_pFrameBuffer)
            return RwRasterGetHeight(m_pFrameBuffer);
        else
            return Device().Videomode().height;
        break;

    default:
        ASSERT(false);
        return Device().Videomode().height;
    };
};


int32 CPCGraphicsDevice::ScreenDepth(void)
{
    return Device().Videomode().depth;
};


void* CPCGraphicsDevice::Configure(void)
{
    return CPCSpecific::m_hWnd;
};


int32 CPCGraphicsDevice::Subsystem(void)
{
    bool bResult = DeviceStartup();
    ASSERT(bResult);

    m_displaymode = DISPLAYMODE(CPCSetting::m_iDispMode);
    ASSERT(m_displaymode >= DISPLAYMODE_FULLSCREEN && m_displaymode <= DISPLAYMODE_WINDOW);

    if (CPCSetting::m_iMonitorNo == -1)
        m_curDevice = DeviceSyncPrimary();
    else
        m_curDevice = GetDeviceNoByMonitorNo(CPCSetting::m_iMonitorNo);
    ASSERT(m_curDevice != -1);
    ASSERT(m_curDevice >= 0 && m_curDevice < m_numDevice);

	switch (m_displaymode)
	{
    case DISPLAYMODE_FULLSCREEN:
        {
            Device().m_curVideomode = Device().Videomode(CPCSetting::m_videomode.m_iWidth, CPCSetting::m_videomode.m_iHeight);
            ASSERT(
                Device().m_curVideomode != -1,
                "specified videomode (%dx%dx%d) not exist",
                CPCSetting::m_videomode.m_iWidth,
                CPCSetting::m_videomode.m_iHeight,
                CPCSetting::m_videomode.m_iDepth
            );
        }
        break;

	case DISPLAYMODE_FULLWINDOW:
    case DISPLAYMODE_WINDOW:
        {
            Device().m_curVideomode = Device().m_iWindowVideomodeIndex;
        }
        break;

	default:
		ASSERT(false);
		break;
	};

    return m_curDevice;
};


int32 CPCGraphicsDevice::Videomode(void)
{
    switch (m_displaymode)
    {
    case DISPLAYMODE_FULLSCREEN:
        return Device().Videomode().m_iNo;

    case DISPLAYMODE_FULLWINDOW:
    case DISPLAYMODE_WINDOW:
        return Device().m_iWindowVideomodeNo;

    default:
        ASSERT(false);
        return -1;
    };
};


bool CPCGraphicsDevice::IsExclusive(void) const
{
    return (m_displaymode == DISPLAYMODE_FULLSCREEN ||
            m_displaymode == DISPLAYMODE_FULLWINDOW);
};


const char* CPCGraphicsDevice::GetDeviceName(void) const
{
    return Device().m_pszMonitorName;
};


void CPCGraphicsDevice::SetDisplaymode(DISPLAYMODE mode)
{
    m_displaymode = mode;
    DeviceChanged();
};


bool CPCGraphicsDevice::SetVideomode(int32 iVideomodeIndex)
{
    ASSERT(m_displaymode == DISPLAYMODE_FULLSCREEN);
    ASSERT(iVideomodeIndex >= 0 && iVideomodeIndex < Device().m_numVideomode);
    
    Device().m_curVideomode = iVideomodeIndex;
    DeviceChanged();

	return true;
};


bool CPCGraphicsDevice::SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth)
{
    ASSERT(m_displaymode == DISPLAYMODE_FULLSCREEN);
    
    int32 iVideomodeIndex = Device().Videomode(iWidth, iHeight);
    if(iVideomodeIndex == -1)
        return false;

    return SetVideomode(iVideomodeIndex);
};


CPCGraphicsDevice::DISPLAYMODE CPCGraphicsDevice::GetDisplaymode(void) const
{
    return m_displaymode;
};


void CPCGraphicsDevice::GetVideomode(int32 iVideomodeIndex, int32& riWidth, int32& riHeight, int32& riDepth)
{
    ASSERT(iVideomodeIndex >= 0 && iVideomodeIndex < Device().m_numVideomode);
    
    riWidth = Device().Videomode(iVideomodeIndex).width;
    riHeight = Device().Videomode(iVideomodeIndex).height;
    riDepth = Device().Videomode(iVideomodeIndex).depth;
};


int32 CPCGraphicsDevice::GetVideomodeNum(void) const
{
    return Device().m_numVideomode;
};


int32 CPCGraphicsDevice::GetDeviceNoByMonitorNo(int32 iMonitorNo)
{
    ASSERT(m_numDevice == CPCMonitor::GetNum());
    ASSERT(iMonitorNo >= 0 && iMonitorNo < CPCMonitor::GetNum());
    
    for (int32 i = 0; i < m_numDevice; ++i)
    {
        if (m_paDevice[i].m_iMonitorNo == iMonitorNo)
            return i;
    };

    return -1;
};


CPCGraphicsDevice::DEVICE& CPCGraphicsDevice::Device(void)
{
    ASSERT(m_curDevice >= 0 && m_curDevice < m_numDevice);
    return *&m_paDevice[m_curDevice];
};


const CPCGraphicsDevice::DEVICE& CPCGraphicsDevice::Device(void) const
{
    ASSERT(m_curDevice >= 0 && m_curDevice < m_numDevice);
    return *&m_paDevice[m_curDevice];
};


void CPCGraphicsDevice::ConfigureUpdate(int32 iWidth, int32 iHeight)
{
    ASSERT(CPCSpecific::m_hWnd != NULL);

    switch (m_displaymode)
    {
    case DISPLAYMODE_FULLSCREEN:
        {
            SetWindowPos(
                CPCSpecific::m_hWnd,
                HWND_TOPMOST,
                0,
                0,
                0,
                0,
                SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE
            );

            CPCUtils::DisplayCursor(false);
        }
        break;

    case DISPLAYMODE_FULLWINDOW:
        {
            uint32 uWindowStyle = WS_POPUP;

            SetWindowLong(CPCSpecific::m_hWnd, GWL_STYLE, uWindowStyle);
            SetWindowPos(
                CPCSpecific::m_hWnd,
                HWND_TOPMOST,
                Device().m_iMonitorX,
                Device().m_iMonitorY,
                Device().m_iMonitorWidth,
                Device().m_iMonitorHeight,
                NULL
            );

            CPCUtils::DisplayCursor(false);
        }
        break;

    case DISPLAYMODE_WINDOW:
        {
            uint32 uWindowStyle = WS_OVERLAPPEDWINDOW;
            
            RECT rc = { 0, 0, iWidth, iHeight };
            AdjustWindowRectEx(&rc, uWindowStyle, FALSE, NULL);

            int32 cxScreen = Device().m_iMonitorWidth;
            int32 cyScreen = Device().m_iMonitorHeight;

            int32 w = rc.right - rc.left;
            int32 h = rc.bottom - rc.top;
            int32 x = Math::Clamp((cxScreen - w) / 2, 0, cxScreen - w);
            int32 y = Math::Clamp((cyScreen - h) / 2, 0, cyScreen - h);

            SetWindowLong(CPCSpecific::m_hWnd, GWL_STYLE, uWindowStyle);
            SetWindowPos(
                CPCSpecific::m_hWnd,
                HWND_BOTTOM,
                Device().m_iMonitorX + x,
                Device().m_iMonitorY + y,
                w,
                h,
                NULL
            );

            CPCUtils::DisplayCursor(true);
        };
        break;

    default:
        ASSERT(false);
        break;
    };

    InvalidateRect(CPCSpecific::m_hWnd, NULL, TRUE);
    UpdateWindow(CPCSpecific::m_hWnd);
};


void CPCGraphicsDevice::DeviceChanged(void)
{
    switch (m_displaymode)
    {
    case DISPLAYMODE_FULLSCREEN:
        {   
            RwD3D9ChangeVideoMode(Device().Videomode().m_iNo);

            ConfigureUpdate(Device().Videomode().width, Device().Videomode().height);
            
            DestroyFrameBuffer();
            CreateFrameBuffer();

            if (Device().m_iMultiSamplingLevel)
                RwD3D9EngineSetMultiSamplingLevels(Device().m_iMultiSamplingLevel * 2);
            else
                RwD3D9EngineSetMultiSamplingLevels(1);

            RwD3D9EngineSetRefreshRate(60);
            
            CGraphicsDevice::Flip();
        }
        break;

    case DISPLAYMODE_FULLWINDOW:
        {
            if (RwEngineGetCurrentVideoMode() != Device().m_iWindowVideomodeNo)
            {
                RwD3D9ChangeVideoMode(Device().m_iWindowVideomodeNo);
                Device().m_curVideomode = Device().m_iWindowVideomodeIndex;

                ConfigureUpdate(Device().Videomode().width, Device().Videomode().height);

                DestroyFrameBuffer();
                CreateFrameBuffer();

                ShowWindow(CPCSpecific::m_hWnd, SW_SHOW);
                SetForegroundWindow(CPCSpecific::m_hWnd);
            };
        }
        break;

    case DISPLAYMODE_WINDOW:
        {
            if (RwEngineGetCurrentVideoMode() != Device().m_iWindowVideomodeNo)
            {
                RwD3D9ChangeVideoMode(Device().m_iWindowVideomodeNo);
                Device().m_curVideomode = Device().m_iWindowVideomodeIndex;
            };

            ConfigureUpdate(Device().Videomode().width, Device().Videomode().height);

            DestroyFrameBuffer();
            CreateFrameBuffer();

            ShowWindow(CPCSpecific::m_hWnd, SW_SHOW);
            SetForegroundWindow(CPCSpecific::m_hWnd);
        }
        break;
    };
};


bool CPCGraphicsDevice::DeviceStartup(void)
{
    ASSERT(!m_paDevice);
    
    bool bResult = false;

    m_numDevice = RwEngineGetNumSubSystems();
    if (!m_numDevice)
        return bResult;

    m_paDevice = new DEVICE[m_numDevice];
    if (!m_paDevice)
    {
        m_numDevice = 0;
        return bResult;
    };

    for (int32 iDevice = 0; iDevice < m_numDevice; ++iDevice)
    {
        m_curDevice = iDevice;
        DEVICE* pDevice = &m_paDevice[iDevice];
        
        RwEngineGetSubSystemInfo(pDevice, iDevice);
        RwEngineSetSubSystem(iDevice);
        int32 numVideomode = RwEngineGetNumVideoModes();
        
        pDevice->m_iMultiSamplingLevel = 0;
        pDevice->m_iWindowVideomodeNo = -1;
        pDevice->m_numVideomode = 0;
        pDevice->m_curVideomode = 0;
        pDevice->m_paVideomode = new VIDEOMODE[numVideomode];

        for (int32 iVideomode = 0; iVideomode < numVideomode; ++iVideomode)
        {
            VIDEOMODE* pVideomode = &pDevice->m_paVideomode[ pDevice->m_numVideomode ];
            RwEngineGetVideoModeInfo(pVideomode, iVideomode);
            RwEngineSetVideoMode(iVideomode);

            if (!EvalVideomode(pVideomode))
                continue;
           
            ++pDevice->m_numVideomode;
            if (!IS_FLAG_SET(pVideomode->flags, rwVIDEOMODEEXCLUSIVE))
            {
                pDevice->m_iWindowVideomodeNo = iVideomode;
                pDevice->m_iWindowVideomodeIndex = iVideomode;

                pVideomode->m_bWindow = true;
            }
            else
            {
                pVideomode->m_bWindow = false;
            };

			pVideomode->m_iNo = iVideomode;
            pVideomode->m_iMaxMultisamplingLevels = RwD3D9EngineGetMaxMultiSamplingLevels();
            
            std::sprintf(
				pVideomode->m_szName,
				"%d x %d x %d - %d",
				pVideomode->width,
				pVideomode->height,
				pVideomode->depth,
				pVideomode->refRate
			);           
        };

		//
		//	Sort videomodes from low to high
		//
        std::qsort(
            pDevice->m_paVideomode,
            pDevice->m_numVideomode,
            sizeof(VIDEOMODE),
            CPCGraphicsDevice::VideomodeSortCallback
        );

		//
		//	Videomodes are sorted now, update window videomode index
		//
		for (int32 i = 0; i < pDevice->m_numVideomode; ++i)
		{
            if (pDevice->m_paVideomode[i].m_bWindow)
            {
                pDevice->m_iWindowVideomodeIndex = i;
                break;
            };
		};
        
		//
		//	Initialize monitor parameters
		//
        bResult = DeviceInitMonitor(iDevice);
        ASSERT(bResult);

        OUTPUT(
            "[SYS] Monitor found: %s (Device: %s, number of videomodes: %d)\n",
            pDevice->m_pszMonitorName,
            pDevice->name,
            pDevice->m_numVideomode
        );

        for (int32 i = 0; i < pDevice->m_numVideomode; ++i)
        {
            VIDEOMODE* pVideomode = &pDevice->m_paVideomode[i];
            OUTPUT(
                "[SYS] \t\t%d) Videomode: %4d x %4d x %4d - %d (%s)\n",
                i,
                pVideomode->width,
                pVideomode->height,
                pVideomode->depth,
                pVideomode->refRate,
                pVideomode->m_bWindow ? "Window" : "Fullscreen"
            );
        };
    };
    
    bResult = (m_numDevice > 0);
    
    return bResult;
};


void CPCGraphicsDevice::DeviceCleanup(void)
{
    for (int32 i = 0; i < m_numDevice; ++i)
    {
        if (m_paDevice[i].m_numVideomode)
        {
            delete[] m_paDevice[i].m_paVideomode;
            m_paDevice[i].m_paVideomode = nullptr;
            m_paDevice[i].m_numVideomode = 0;
        };
    };

    delete[] m_paDevice;
    m_paDevice = nullptr;
    m_numDevice = 0;
};


int32 CPCGraphicsDevice::DeviceSyncPrimary(void)
{
    int32 iPrimaryMonitorNo = CPCMonitor::GetPrimary();
    ASSERT(iPrimaryMonitorNo >= 0 && iPrimaryMonitorNo < CPCMonitor::GetNum());
    
    for (int32 i = 0; i < m_numDevice; ++i)
    {
        if (m_paDevice[i].m_iMonitorNo == iPrimaryMonitorNo)
            return i;        
    };

    return -1;
};


bool CPCGraphicsDevice::DeviceInitMonitor(int32 iDevice)
{
    ASSERT(iDevice >= 0 && iDevice < m_numDevice);
    
    DEVICE* pDevice = &m_paDevice[iDevice];

    //
    //  Initialize monitor data
    //
    int32 iMonitorNo = CPCMonitor::FindMonitor(
        pDevice->m_paVideomode[pDevice->m_iWindowVideomodeIndex].width,
        pDevice->m_paVideomode[pDevice->m_iWindowVideomodeIndex].height
    );
    ASSERT(iMonitorNo != -1);
    if (iMonitorNo == -1)
        return false;

    pDevice->m_pszMonitorName           = CPCMonitor::GetName(iMonitorNo);
    pDevice->m_iMonitorWidth            = CPCMonitor::GetWidth(iMonitorNo);
    pDevice->m_iMonitorWidthWindow      = CPCMonitor::GetWidth(iMonitorNo);
    pDevice->m_iMonitorWidthFullscreen  = CPCMonitor::GetWidth(iMonitorNo);
    pDevice->m_iMonitorHeight           = CPCMonitor::GetHeight(iMonitorNo);
    pDevice->m_iMonitorHeightWindow     = CPCMonitor::GetHeight(iMonitorNo);
    pDevice->m_iMonitorHeightFullscreen = CPCMonitor::GetHeight(iMonitorNo);
    pDevice->m_iMonitorX                = CPCMonitor::GetOffsetX(iMonitorNo);
    pDevice->m_iMonitorY                = CPCMonitor::GetOffsetY(iMonitorNo);
    pDevice->m_iMonitorNo               = iMonitorNo;

    //
    //  Find a compatible videomode resolution for window displaymode instead fullscreen size
    //
    
    int32 iSearchStep = 20;
    int32 iWindowVideomode = -1;
    int32 iWidth = Device().Videomode(Device().m_iWindowVideomodeIndex).width - iSearchStep;
    int32 iHeight = Device().Videomode(Device().m_iWindowVideomodeIndex).height - iSearchStep;

	while (iWindowVideomode == -1)
	{
		for (int32 i = 0; i < pDevice->m_numVideomode; ++i)
		{
            if (Device().Videomode(i).width  <= iWidth &&
                Device().Videomode(i).height <= iHeight)
            {
                OUTPUT(
                    "[SYS] Correcting window videomode from %d x %d to %d x %d\n",
                    iWidth,
					iHeight,
					Device().Videomode(i).width,
                    Device().Videomode(i).height
                );

                iWindowVideomode = i;
			};
		}

		if (iWindowVideomode == -1)
		{
			iWidth -= iSearchStep;
			iHeight -= iSearchStep;
        };

        if (iWidth <= 0 || iHeight <= 0)
            return false;
    };

    //
    //  Override window videomode with found result
    //
    Device().Videomode(Device().m_iWindowVideomodeIndex) = Device().Videomode(iWindowVideomode);
	Device().Videomode(Device().m_iWindowVideomodeIndex).m_bWindow = true;

    return true;
};


bool CPCGraphicsDevice::EvalVideomode(const VIDEOMODE* pVideomode) const
{
    const int32 iMinWidth = 640;
    const int32 iMinHeight = 480;
    const int32 iMinDepth = 16;
    const int32 iMaxDepth = 32;
    const int32 iRefreshRateMin = 30;
    const int32 iRefreshRateMax = 120;

    if (!IS_FLAG_SET(pVideomode->format, rwRASTERFORMAT888))
        return false;

    if (pVideomode->width < iMinWidth || pVideomode->height < iMinHeight)
        return false;

    if (pVideomode->depth < iMinDepth || pVideomode->depth > iMaxDepth)
        return false;

    if (pVideomode->refRate < iRefreshRateMin || pVideomode->refRate > iRefreshRateMax)
        return false;

    if (!IS_FLAG_SET(pVideomode->flags, rwVIDEOMODEEXCLUSIVE))
        return true;

    static const RwV2d s_aAspectRatio[] =
    {
        { 4,    3   },
        { 5,    4   },
        { 16,   9   },
    };

    for (int32 i = 0; i < COUNT_OF(s_aAspectRatio); ++i)
    {
        if ((pVideomode->width * s_aAspectRatio[i].y) == (pVideomode->height * s_aAspectRatio[i].x))
            return true;
    };

    return false;
};


/*static*/ int32 CPCGraphicsDevice::VideomodeSortCallback(const void* a, const void* b)
{
    const VIDEOMODE* pVideomodeA = (const VIDEOMODE*)a;
    const VIDEOMODE* pVideomodeB = (const VIDEOMODE*)b;

    ASSERT(pVideomodeA);
    ASSERT(pVideomodeB);

    if (pVideomodeA->depth > pVideomodeB->depth)        
        return 1;

    if (pVideomodeA->depth < pVideomodeB->depth)
        return -1;

    if (pVideomodeA->width > pVideomodeB->width)        
        return 1;

    if (pVideomodeA->width < pVideomodeB->width)
        return -1;

    if (pVideomodeA->height > pVideomodeB->height)
        return 1;

    if (pVideomodeA->height < pVideomodeB->height)
        return -1;

    return 0;
};