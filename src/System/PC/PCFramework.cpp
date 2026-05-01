#include "PCFramework.hpp"
#include "PCTypedefs.hpp"
#include "PCSpecific.hpp"
#include "PCSystem.hpp"
#include "PCSetting.hpp"
#include "PCTimer.hpp"
#include "PCMemory.hpp"
#include "PCGraphicsDevice.hpp"
#include "PCInputsDevice.hpp"
#include "PCClockDevice.hpp"
#include "PCSoundDevice.hpp"
#include "PCError.hpp"
#include "PCFrameSkipController.hpp"

#include "File/PCFileManager.hpp"

#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Configure.hpp"


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
, m_pPCSoundDevice(nullptr)
, m_pFrameSkipController(nullptr)
{
    ASSERT(m_pInstance == nullptr);
    m_pInstance = this;
};


CPCFramework::~CPCFramework(void)
{
    ASSERT(m_pInstance == this);
    m_pInstance = nullptr;
};


bool CPCFramework::Initialize(void)
{
    if (CConfigure::CheckArg("mp"))
        m_pMemory = new CPCMemoryPool;
    else
        m_pMemory = new CPCMemory;

    m_pSystem = new CPCSystem(this);
    if (!m_pSystem->Initialize())
    {
        OUTPUT("pcsystem init failed\n");
        return false;
    };

    m_pTimer = new CPCTimer;
    m_pClockDevice = new CPCClockDevice;
    m_pPCGraphicsDevice = new CPCGraphicsDevice;
    m_pGraphicsDevice = m_pPCGraphicsDevice;
#ifdef _DEBUG
    if (CConfigure::CheckArg("readlog"))
        m_pFileManager = new CPCFileManagerReadLog;
    else
        m_pFileManager = new CPCFileManager;
#else /* _DEBUG */
    m_pFileManager = new CPCFileManager;
#endif /* _DEBUG */
    m_pInputsDevice = new CPCInputsDevice;
    m_pPCSoundDevice = new CPCSoundDevice;
    m_pFrameSkipController = new CPCFrameSkipController;

    if (!m_pPCSoundDevice->Initialize())
    {
        OUTPUT("adx sound init failed\n");
        CPCError::ShowNoRet("Sound initialize failed");
        return false;
    };

    if (!m_pPCSoundDevice->StartupFramework())
    {
        OUTPUT("lib sound init failed\n");
        CPCError::ShowNoRet("Sound framework initialize failed");
        return false;
    };

    if (!CFramework::Initialize())
    {
        OUTPUT("framework initialize failed\n");
        return false;
    };

    ShowWindow(CPCSpecific::m_hWnd, SW_SHOW);
    SetForegroundWindow(CPCSpecific::m_hWnd);

    return true;
};


void CPCFramework::Terminate(void)
{
    CFramework::Terminate();

    if (m_pFrameSkipController)
    {
        delete m_pFrameSkipController;
        m_pFrameSkipController = nullptr;
    };

    if (m_pPCSoundDevice)
    {
        m_pPCSoundDevice->ShutdownFramework();
        m_pPCSoundDevice->Terminate();
        delete m_pPCSoundDevice;
        m_pPCSoundDevice = nullptr;
    };

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

    if (m_pMemory)
    {
        delete m_pMemory;
        m_pMemory = nullptr;
    };
};


void CPCFramework::Run(void)
{
    while (!IsEnded())
    {
        if (!m_pSystem->Run())
            break;
    };
};


void CPCFramework::Move(void)
{
    CFramework::Move();
};


void CPCFramework::Render(void)
{
    ASSERT(m_pFrameSkipController);
    ASSERT(m_pGraphicsDevice);

    m_pFrameSkipController->Sync();
    m_pGraphicsDevice->SetFlipEnable(!m_pFrameSkipController->IsSkip());

    if (m_pGraphicsDevice->RenderBegin())
    {
        if (!m_pFrameSkipController->IsSkip())
            m_pProcessDispatcher->Draw();

        m_pGraphicsDevice->RenderEnd();
    };
};


void CPCFramework::FlipNoSync(void)
{
    m_pGraphicsDevice->Flip();
};


CPCGraphicsDevice& CPCFramework::GraphicsDevice(void)
{
    ASSERT(m_pGraphicsDevice);
    return *m_pPCGraphicsDevice;
};


bool CPCFramework::SetVideomode(int32 No)
{
    if (!GraphicsDevice().IsFullscreen())
        ShowWindow(CPCSpecific::m_hWnd, SW_HIDE);

    bool bResult = GraphicsDevice().SetVideomode(No);

    if (!GraphicsDevice().IsFullscreen())
    {
        ShowWindow(CPCSpecific::m_hWnd, SW_SHOW);
        SetForegroundWindow(CPCSpecific::m_hWnd);
    };

    GraphicsDevice().GetVideomode(No, CPCSetting::m_videomode);

    return bResult;
};


bool CPCFramework::GetVideomode(int32 No, PC::VIDEOMODE& vm)
{
    return GraphicsDevice().GetVideomode(No, vm);
};


int32 CPCFramework::GetVideomodeCur(void)
{
    return GraphicsDevice().GetVideomodeCur();
};


int32 CPCFramework::GetVideomodeNum(void)
{
    return GraphicsDevice().GetVideomodeNum();
};


void CPCFramework::SetSkipEnable(bool bState)
{
    m_pFrameSkipController->SetEnable(bState);
};