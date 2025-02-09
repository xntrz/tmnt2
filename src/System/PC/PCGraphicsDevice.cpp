#include "PCGraphicsDevice.hpp"
#include "PCFrameTimer.hpp"
#include "PCSpecific.hpp"
#include "PCSetting.hpp"
#include "PCError.hpp"

#include "System/Common/Configure.hpp"
#include "System/Common/Screen.hpp"


struct CPCGraphicsDevice::VIDEOMODE : public RwVideoMode
{
    int32 m_index;
    int32 m_maxMultiSamplingLevels;
    char m_szName[64];

    static int32 SortCallback(const void* a, const void* b);
    static bool Eval(const VIDEOMODE* pVideomode);
};


struct CPCGraphicsDevice::DEVICEINFO : public RwSubSystemInfo
{
    VIDEOMODE* m_pModes;
    int32 m_numModes;
    int32 m_curMode;
    int32 m_numMultisamplingLvls;
    int32 m_idxModeWnd;
    bool m_bModeWndExist;
};


/*static*/ int32 CPCGraphicsDevice::VIDEOMODE::SortCallback(const void* a, const void* b)
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


/*static*/ bool CPCGraphicsDevice::VIDEOMODE::Eval(const VIDEOMODE* pVideomode)
{
    const int32 iMinWidth = int32(TYPEDEF::DEFAULT_SCREEN_WIDTH);
    const int32 iMinHeight = int32(TYPEDEF::DEFAULT_SCREEN_HEIGHT);
    const int32 iMinDepth = 16;
    const int32 iMaxDepth = 32;
    
    if (pVideomode->width < iMinWidth || pVideomode->height < iMinHeight)
        return false;

    if (pVideomode->depth < iMinDepth || pVideomode->depth > iMaxDepth)
        return false;

    if (!(pVideomode->flags & rwVIDEOMODEEXCLUSIVE))
        return true;

	if (!(pVideomode->format & rwRASTERFORMAT8888))
		return false;

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


CPCGraphicsDevice::CPCGraphicsDevice(void)
: m_pFrameTimer(nullptr)
, m_pDeviceInfo(nullptr)
, m_numDevices(0)
, m_curDevice(-1)
, m_multisamplingLvl(0)
, m_bFullscreen(false)
, m_bHighReso(false)
{
    ;
};


CPCGraphicsDevice::~CPCGraphicsDevice(void)
{
    ;
};


bool CPCGraphicsDevice::Initialize(void)
{
    if (!CGraphicsDevice::Initialize())
    {
        CPCError::ShowNoRet("Video Initialization Failed");
        return false;
    };

    m_pFrameTimer = new CPCFrameTimer(*this);
    if (!m_pFrameTimer)
        return false;

    if (m_bFullscreen)
    {
        int32 numMsLevels = m_pDeviceInfo[m_curDevice].m_numMultisamplingLvls;

        RwD3D9EngineSetMultiSamplingLevels(numMsLevels ?  numMsLevels * 2 : 1);

        uint32 refreshRate = 60;
#ifdef TMNT2_BUILD_EU
        if ((CConfigure::GetTVMode() == TYPEDEF::CONFIG_TV_PAL) && IsPalMode())
            refreshRate = 50;
#endif /* TMNT2_BUILD_EU */

        RwD3D9EngineSetRefreshRate(refreshRate);
    };

    SetWindowRect(m_pDeviceInfo[m_curDevice].m_pModes[m_pDeviceInfo[m_curDevice].m_curMode].width,
                  m_pDeviceInfo[m_curDevice].m_pModes[m_pDeviceInfo[m_curDevice].m_curMode].height);
    
    return true;
};


void CPCGraphicsDevice::Terminate(void)
{
    Cleanup();
    
    if (m_pFrameTimer)
    {
        delete m_pFrameTimer;
        m_pFrameTimer = nullptr;
    };

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
    ASSERT(m_pDeviceInfo);
    ASSERT(m_numDevices > 0);
    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);

    int32 curMode = m_pDeviceInfo[m_curDevice].m_curMode;

    ASSERT(curMode >= 0);
    ASSERT(curMode <= m_pDeviceInfo[m_curDevice].m_numModes);

    return m_pDeviceInfo[m_curDevice].m_pModes[curMode].width;
};


int32 CPCGraphicsDevice::ScreenHeight(void)
{
    ASSERT(m_pDeviceInfo);
    ASSERT(m_numDevices > 0);
    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);

    int32 curMode = m_pDeviceInfo[m_curDevice].m_curMode;

    ASSERT(curMode >= 0);
    ASSERT(curMode <= m_pDeviceInfo[m_curDevice].m_numModes);

    return m_pDeviceInfo[m_curDevice].m_pModes[curMode].height;
};


int32 CPCGraphicsDevice::ScreenDepth(void)
{
    ASSERT(m_pDeviceInfo);
    ASSERT(m_numDevices > 0);
    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);

    int32 curMode = m_pDeviceInfo[m_curDevice].m_curMode;

    ASSERT(curMode >= 0);
    ASSERT(curMode <= m_pDeviceInfo[m_curDevice].m_numModes);

    return m_pDeviceInfo[m_curDevice].m_pModes[curMode].depth;
};


void* CPCGraphicsDevice::Configure(void)
{
    return CPCSpecific::m_hWnd;
};


int32 CPCGraphicsDevice::Subsystem(void)
{
    bool bResult = EnumerateVideomodes();
    ASSERT(bResult);

#ifdef _DEBUG
    OutputInfo();
#endif    

    m_bFullscreen = !CPCSetting::m_bWindowMode;
    m_curDevice = SearchAndSetVideomode(CPCSetting::m_videomode, false);
    
    if (m_curDevice == -1)
	{
        OUTPUT(
            "Settings videomode \"%d x %d x %d\" not found. Set to default.\n",
            CPCSetting::m_videomode.w,
            CPCSetting::m_videomode.h,
            CPCSetting::m_videomode.d
        );
        
		m_curDevice = SearchAndSetVideomode(CPCSetting::VIDEOMODE_DEFAULT, false);
	};

    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);

    return m_curDevice;
};


int32 CPCGraphicsDevice::Videomode(void)
{
    if (m_bFullscreen)
        return m_pDeviceInfo[m_curDevice].m_pModes[m_pDeviceInfo->m_curMode].m_index;
    
    return m_pDeviceInfo[m_curDevice].m_idxModeWnd;
};


bool CPCGraphicsDevice::EnumerateVideomodes(void)
{
    ASSERT(!m_pDeviceInfo);
    
    m_numDevices = RwEngineGetNumSubSystems();
    if (!m_numDevices)
        return false;

    m_pDeviceInfo = new DEVICEINFO[m_numDevices];
    if (!m_pDeviceInfo)
    {
        m_numDevices = 0;
        return false;
    };

    for (int32 i = 0; i < m_numDevices; ++i)
    {
        DEVICEINFO* pDeviceInfo = &m_pDeviceInfo[i];
        
        RwEngineGetSubSystemInfo(pDeviceInfo, i);
        RwEngineSetSubSystem(i);
        int32 numModes = RwEngineGetNumVideoModes();
        
        pDeviceInfo->m_numMultisamplingLvls = 0;
        pDeviceInfo->m_idxModeWnd = -1;
        pDeviceInfo->m_numModes = 0;
        pDeviceInfo->m_curMode = 0;
        pDeviceInfo->m_pModes = new VIDEOMODE[numModes];

        VIDEOMODE* pVideomode = &pDeviceInfo->m_pModes[0];
        
        for (int32 j = 0; j < numModes; ++j)
        {
            RwEngineGetVideoModeInfo(pVideomode, j);

			if (!VIDEOMODE::Eval(pVideomode))
				continue;
           
            if (pVideomode->flags & rwVIDEOMODEEXCLUSIVE)
            {
                pVideomode->m_index = j;
                std::sprintf(
                    pVideomode->m_szName,
                    "%d x %d x %d",
                    pVideomode->width,
                    pVideomode->height,
                    pVideomode->depth
                );

                RwEngineSetVideoMode(j);
                pVideomode->m_maxMultiSamplingLevels = RwD3D9EngineGetMaxMultiSamplingLevels();

                ++pDeviceInfo->m_numModes;
                ++pVideomode;
            }
            else
            {
                pDeviceInfo->m_bModeWndExist = true;
                pDeviceInfo->m_idxModeWnd = j;
            };
        };

        std::qsort(pDeviceInfo->m_pModes, pDeviceInfo->m_numModes, sizeof(VIDEOMODE), VIDEOMODE::SortCallback);
    };

    if (m_numDevices > 0)
        return true;

    Cleanup();
    
    return false;
};


void CPCGraphicsDevice::Cleanup(void)
{
    for (int32 i = 0; i < m_numDevices; ++i)
    {
        if (m_pDeviceInfo[i].m_numModes)
        {
            delete[] m_pDeviceInfo[i].m_pModes;
            m_pDeviceInfo[i].m_pModes = nullptr;
            m_pDeviceInfo[i].m_numModes = 0;
        };
    };

    delete[] m_pDeviceInfo;
    m_pDeviceInfo = nullptr;
    m_numDevices = 0;
};


int32 CPCGraphicsDevice::SearchAndSetVideomode(const PC::VIDEOMODE& vm, bool bSearchInProgress)
{
    ASSERT(m_pDeviceInfo);
    ASSERT(m_numDevices > 0);
    ASSERT((!bSearchInProgress) || ((bSearchInProgress) && ((m_curDevice >= 0) && (m_curDevice < m_numDevices))));

    if (m_curDevice >= 0)
    {
        //
        //  first attempt is search videomode in current device
        //
        for (int32 i = 0; i < m_pDeviceInfo[m_curDevice].m_numModes; ++i)
        {
            VIDEOMODE* pVideomode = &m_pDeviceInfo[m_curDevice].m_pModes[i];

            if ((pVideomode->width  == vm.w)    &&
                (pVideomode->height == vm.h)    &&
                (pVideomode->depth  == vm.d))
            {
                if (!bSearchInProgress)
                    m_pDeviceInfo[m_curDevice].m_curMode = i;

                return m_curDevice;
            };
        };
    };
    
    //
    //  second attempt is search videomode across all devices
    //
    for (int32 i = 0; i < m_numDevices; ++i)
    {
        DEVICEINFO* pDeviceInfo = &m_pDeviceInfo[i];

        for (int32 j = 0; j < pDeviceInfo->m_numModes; ++j)
        {
            VIDEOMODE* pVideomode = &pDeviceInfo->m_pModes[j];

            if ((pVideomode->width  != vm.w)    ||
                (pVideomode->height != vm.h)    ||
                (pVideomode->depth  != vm.d))
                continue;

            if (!bSearchInProgress)
                pDeviceInfo->m_curMode = j;

            return i;
        };
    };

    //
    //  third attempt is the same as second except depth and refrate matching
    //
    for (int32 i = 0; i < m_numDevices; ++i)
    {
        DEVICEINFO* pDeviceInfo = &m_pDeviceInfo[i];

        for (int32 j = 0; j < pDeviceInfo->m_numModes; ++j)
        {
            VIDEOMODE* pVideomode = &pDeviceInfo->m_pModes[j];

            if ((pVideomode->width  != vm.w)    ||
                (pVideomode->height != vm.h))
                continue;

            if (!bSearchInProgress)
                pDeviceInfo->m_curMode = j;

            return i;
        };
    };
    
    return -1;
};


void CPCGraphicsDevice::SetWindowRect(int32 iWidth, int32 iHeight)
{
    HWND hWnd = CPCSpecific::m_hWnd;
    
    ASSERT(hWnd != NULL);

    if (m_bFullscreen)
    {
        SetWindowLongA(hWnd, GWL_STYLE, WS_POPUP);
        SetWindowLongA(hWnd, GWL_EXSTYLE, 0);
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }
    else
    {
        const uint32 WndStyle = (WS_OVERLAPPEDWINDOW) ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);
        const uint32 WndStyleEx = WS_EX_CLIENTEDGE;

        static_assert(WndStyle == 0xCA0000, "checkout");
        static_assert(WndStyleEx == 0x200, "checkout");

        int32 cxScreen = GetSystemMetrics(SM_CXSCREEN);
        int32 cyScreen = GetSystemMetrics(SM_CYSCREEN);

        RECT rc;
        SetRect(&rc, 0, 0, iWidth, iHeight);

        AdjustWindowRectEx(&rc, WndStyle, 0, WndStyleEx);

        int32 w = rc.right - rc.left;
        int32 h = rc.bottom - rc.top;

        int32 x = Clamp((cxScreen - w) / 2, 0, cxScreen - w);
        int32 y = Clamp((cyScreen - h) / 2, 0, cyScreen - h);
        
        SetWindowLongA(hWnd, GWL_STYLE, WndStyle);
        SetWindowLongA(hWnd, GWL_EXSTYLE, WndStyleEx);
        SetWindowPos(hWnd, HWND_NOTOPMOST, x, y, w, h, SWP_NOACTIVATE);
    };
};


bool CPCGraphicsDevice::SetVideomode(const PC::VIDEOMODE& vm)
{
    //
    //  search for new videomode
    //
    int32 curDevice = SearchAndSetVideomode(vm, true);
    if ((curDevice < 0) || (curDevice != m_curDevice))
        return false;

    //
    //  save current videomode for failure case
    //
    PC::VIDEOMODE vmbuff(ScreenWidth(), ScreenHeight(), ScreenDepth());

    //
    //  set new video mode
    //
    curDevice = SearchAndSetVideomode(vm, false);
    ASSERT(curDevice == m_curDevice);

    //
    //  adjust window size and recreate frame buffers
    //
    if (!m_bFullscreen)
    {
        DestroyFrameBuffer();
        RwD3D9EngineSetRefreshRate(0);
    };

    SetWindowRect(vm.w, vm.h);

    bool bVideomodeChangedFlag = false;

    if (RwD3D9ChangeVideoMode(Videomode()))
    {
        bVideomodeChangedFlag = true;

        if (m_bFullscreen)
        {
            RwD3D9EngineSetMultiSamplingLevels(
                m_pDeviceInfo[m_curDevice].m_numMultisamplingLvls ? (m_pDeviceInfo[m_curDevice].m_numMultisamplingLvls * 2) : (1)
            );
            RwD3D9EngineSetRefreshRate(60);
        };

        if (m_bFullscreen || CreateFrameBuffer())
        {
            CScreen::DeviceChanged();
            return true;
        };
    };

    //
    //  setting new videomode failed
    //  set saved old videomode, recreate framebuffers and adjust window rect... again
    //
    SearchAndSetVideomode(vmbuff, false);
    SetWindowRect(CPCSetting::m_videomode.w, CPCSetting::m_videomode.h);

    if (bVideomodeChangedFlag)
        RwD3D9ChangeVideoMode(Videomode());

    if (!m_bFullscreen)
    {
        bool bResult = CreateFrameBuffer();
        ASSERT(bResult);
    };

    return false;
};


bool CPCGraphicsDevice::SetVideomode(int32 No)
{
    PC::VIDEOMODE vm;

    if (GetVideomode(No, vm))
        return SetVideomode(vm);

    return false;
};


bool CPCGraphicsDevice::GetVideomode(int32 No, PC::VIDEOMODE& vm) const
{
    ASSERT(m_pDeviceInfo);
    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);
    ASSERT(No >= 0);
    ASSERT(No < m_pDeviceInfo[m_curDevice].m_numModes);

    if (No < 0 && No >= m_pDeviceInfo[m_curDevice].m_numModes)
        return false;

    vm.w    = m_pDeviceInfo[m_curDevice].m_pModes[No].width;
    vm.h    = m_pDeviceInfo[m_curDevice].m_pModes[No].height;
    vm.d    = m_pDeviceInfo[m_curDevice].m_pModes[No].depth;

    return true;
};


int32 CPCGraphicsDevice::GetVideomodeCur(void) const
{
    ASSERT(m_pDeviceInfo);
    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);

    return m_pDeviceInfo[m_curDevice].m_curMode;
};


int32 CPCGraphicsDevice::GetVideomodeNum(void) const
{
    ASSERT(m_pDeviceInfo);
    ASSERT(m_curDevice >= 0);
    ASSERT(m_curDevice < m_numDevices);

    return m_pDeviceInfo[m_curDevice].m_numModes;
};


bool CPCGraphicsDevice::IsFullscreen(void) const
{
    return m_bFullscreen;
};


void CPCGraphicsDevice::OutputInfo(void)
{
    for (int32 i = 0; i < m_numDevices; ++i)
    {
        DEVICEINFO* pDeviceInfo = &m_pDeviceInfo[i];
        
        OUTPUT(
            "Device: %s, number of videomodes: %d, window mode support: %s\n",
            pDeviceInfo->name,
            pDeviceInfo->m_numModes,
            pDeviceInfo->m_bModeWndExist ? "true" : "false"
        );

        for (int32 j = 0; j < pDeviceInfo->m_numModes; ++j)
        {
            VIDEOMODE* pVideomode = &pDeviceInfo->m_pModes[j];

            OUTPUT(
                "\t\t%d) Videomode: %4d x %4d x %4d - %d (FLAGS: 0x%x, FORMAT: 0x%x)\n",
                j + 1,
                pVideomode->width,
                pVideomode->height,
                pVideomode->depth,
                pVideomode->refRate,
				pVideomode->flags,
				pVideomode->format
            );
        };
    };    
};