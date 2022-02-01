#include "PCFramework.hpp"
#include "PCSystem.hpp"
#include "PCTimer.hpp"
#include "PCSpecific.hpp"
#include "PCMemory.hpp"
#include "PCGraphicsDevice.hpp"
#include "PCInputsDevice.hpp"
#include "PCClockDevice.hpp"
#include "PCTypedefs.hpp"
#include "PCSetting.hpp"
#include "PCDebug.hpp"

#include "File/PCFileManager.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"


/*static*/ CPCFramework* CPCFramework::m_pInstance = nullptr;


/*static*/ CPCFramework& CPCFramework::Instance(void)
{
    ASSERT(m_pInstance);
    return *m_pInstance;
};


CPCFramework::CPCFramework(void)
: m_pSystem(nullptr)
, m_pTimer(nullptr)
, m_pPCGraphicsDevice(nullptr)
, m_bStarted(false)
{
    ;
};


CPCFramework::~CPCFramework(void)
{
    ;
};


void CPCFramework::Startup(void)
{    
    CFramework::Startup();

    m_pMemory = new CPCMemory;

	enum
	{
		STEP_WND    = BIT(0),
		STEP_INIT   = BIT(1),
		STEP_OPEN   = BIT(2),
		STEP_START  = BIT(3),
    };
    
    uint32 uStepFlag = 0;
	HWND hWnd = NULL;
	RwCamera* pCamera = nullptr;

	try
	{
		hWnd = CreateWindowA("STATIC", NULL, NULL, 0, 0, 64, 64, NULL, NULL, NULL, NULL);
		if (hWnd == NULL)
			throw std::exception("creating dummy window");	

		FLAG_SET(uStepFlag, STEP_WND);

		if (!RwEngineInit(nullptr, 0, 0))
            throw std::exception("rw engine init");

		FLAG_SET(uStepFlag, STEP_INIT);

		RwEngineOpenParams OpenParams;
		OpenParams.displayID = hWnd;
		if (!RwEngineOpen(&OpenParams))
			throw std::exception("rw engine open");

        FLAG_SET(uStepFlag, STEP_OPEN);

		if (!RwEngineStart())
			throw std::exception("rw engine start");

		FLAG_SET(uStepFlag, STEP_START);

		pCamera = RwCameraCreate();
		if (pCamera)
		{
			RwCameraEndUpdateFunc pfnCameraEnd = pCamera->endUpdate;			
			SYSTEM_INFO SystemInfo;
			DWORD dwPrevProtect = 0;

			GetSystemInfo(&SystemInfo);

			if (VirtualProtect(pfnCameraEnd, SystemInfo.dwPageSize, PAGE_EXECUTE_READWRITE, &dwPrevProtect))
			{
				const uint8 NOP = 0x90;

#ifdef _DEBUG
                *((uint8*)pfnCameraEnd + 0x74) = NOP;
                *((uint8*)pfnCameraEnd + 0x75) = NOP;
				*((uint8*)pfnCameraEnd + 0x76) = NOP;
				*((uint8*)pfnCameraEnd + 0x77) = NOP;
                *((uint8*)pfnCameraEnd + 0x78) = NOP;
#else /*NDEBUG*/
                *((uint8*)pfnCameraEnd + 0x6) = NOP;
                *((uint8*)pfnCameraEnd + 0x7) = NOP;
                *((uint8*)pfnCameraEnd + 0x8) = NOP;
                *((uint8*)pfnCameraEnd + 0x9) = NOP;
                *((uint8*)pfnCameraEnd + 0xA) = NOP;
#endif                

				VirtualProtect(pfnCameraEnd, SystemInfo.dwPageSize, dwPrevProtect, &dwPrevProtect);
			}
			else
			{
				throw std::exception("change mem page protection");
			};

			RwCameraDestroy(pCamera);
			pCamera = nullptr;
		}
		else
		{
			throw std::exception("create camera");
		};

		OUTPUT("[SYS] Remove Evaluation Logo success!\n");
	}
	catch (std::exception& e)
	{
		REF(e);
		OUTPUT("[SYS] Remove Evaluation Logo failed: %s\n", e.what());		
	};

	if (pCamera)
		RwCameraDestroy(pCamera);

	if (IS_FLAG_SET(uStepFlag, STEP_START))
		RwEngineStop();

	if (IS_FLAG_SET(uStepFlag, STEP_OPEN))
		RwEngineClose();

	if (IS_FLAG_SET(uStepFlag, STEP_INIT))
		RwEngineTerm();

	if (IS_FLAG_SET(uStepFlag, STEP_WND))
		DestroyWindow(hWnd);
};


void CPCFramework::Cleanup(void)
{
    delete m_pMemory;
    m_pMemory = nullptr;

    CFramework::Cleanup();
};


bool CPCFramework::Initialize(void)
{
    bool bResult = false;
    
    try
    {        
        CPCSetting::Initialize();
        
        m_pSystem = new CPCSystem(this);
        if (!m_pSystem->Initialize())
            throw std::exception("PCSystem");        

        m_pTimer = new CPCTimer;        
        m_pClockDevice = new CPCClockDevice;
        m_pPCGraphicsDevice = new CPCGraphicsDevice;
        m_pGraphicsDevice = m_pPCGraphicsDevice;
        m_pFileManager = new CPCFileManager;
        m_pInputsDevice = new CPCInputsDevice(CPCSpecific::m_hWnd);
        
        if (!CFramework::Initialize())
            throw std::exception("Framework");
        
        m_bStarted = true;
        m_pSystem->WindowShow(false);
        
#ifdef _DEBUG
        CPCDebug::ReplaceConsole();
#endif
        
        bResult = true;
    }
    catch (std::exception& e)
    {
		REF(e);
        OUTPUT("[SYS] Failed to initialie framework: %s\n", e.what());      
    };

    return bResult;
};


void CPCFramework::Terminate(void)
{
    CFramework::Terminate();

    if (m_pInputsDevice)
    {
        delete m_pInputsDevice;
        m_pInputsDevice = nullptr;
    };
    
    if (m_pFileManager)
    {
        delete m_pFileManager;
        m_pFileManager = nullptr;
    };

    if (m_pGraphicsDevice)
    {
        delete m_pGraphicsDevice;
        m_pGraphicsDevice = nullptr;
    };
    
    if (m_pClockDevice)
    {
        delete m_pClockDevice;
        m_pClockDevice = nullptr;
    };

    if (m_pTimer)
    {
        delete m_pTimer;
        m_pTimer = nullptr;
    };

    if (m_pSystem)
    {
        m_pSystem->Terminate();
        delete m_pSystem;
        m_pSystem = nullptr;
    };

    CPCSetting::Terminate();
};


void CPCFramework::Run(void)
{
    ASSERT(m_pSystem);
    m_pSystem->Run();
};


void CPCFramework::Move(void)
{
    CFramework::Move();
};


void CPCFramework::Render(void)
{
    ASSERT(m_pGraphicsDevice);
    ASSERT(m_pProcessDispatcher);

    if (m_pGraphicsDevice->RenderBegin())
    {
        m_pProcessDispatcher->Draw();
        m_pGraphicsDevice->RenderEnd();
    };
};


void CPCFramework::UpdateSize(int32 iWidth, int32 iHeight)
{
    if (m_bStarted && !m_pGraphicsDevice->IsExclusive())
    {        
        m_pGraphicsDevice->DestroyFrameBuffer();
        m_pGraphicsDevice->CreateFrameBuffer(iWidth, iHeight);		
    };
};


void CPCFramework::SetFlipEnable(bool bEnable)
{
    ASSERT(m_pGraphicsDevice);
    m_pGraphicsDevice->SetFlipEnable(bEnable);
};


bool CPCFramework::SetVideomode(int32 iVideomodeIndex)
{
    ASSERT(m_pPCGraphicsDevice);
    return m_pPCGraphicsDevice->SetVideomode(iVideomodeIndex);
};


bool CPCFramework::SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth)
{
    ASSERT(m_pPCGraphicsDevice);
    return m_pPCGraphicsDevice->SetVideomode(iWidth, iHeight, iDepth);
};


void CPCFramework::GetVideomode(int32 iVideomodeIndex, int32& iWidth, int32& iHeight, int32& iDepth)
{
    ASSERT(m_pPCGraphicsDevice);
    m_pPCGraphicsDevice->GetVideomode(iVideomodeIndex, iWidth, iHeight, iDepth);
};


const char* CPCFramework::GetVideoDeviceName(void)
{
    ASSERT(m_pPCGraphicsDevice);
    return m_pPCGraphicsDevice->GetDeviceName();
};


int32 CPCFramework::GetVideomodeNum(void)
{
    ASSERT(m_pPCGraphicsDevice);
    return m_pPCGraphicsDevice->GetVideomodeNum();
};