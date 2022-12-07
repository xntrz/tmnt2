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
#include "PCSoundDevice.hpp"

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
, m_pPCSoundDevice(nullptr)
, m_bStarted(false)
{
	m_pInstance = this;
};


CPCFramework::~CPCFramework(void)
{
    ;
};


bool CPCFramework::Initialize(void)
{
    m_pSystem = new CPCSystem(this);
    if (!m_pSystem->Initialize())
    {
        OUTPUT("pcsystem init failed");
        return false;
    };

    m_pTimer = new CPCTimer;
    m_pClockDevice = new CPCClockDevice;
    m_pPCGraphicsDevice = new CPCGraphicsDevice;
    m_pGraphicsDevice = m_pPCGraphicsDevice;
    m_pFileManager = new CPCFileManager;
    m_pInputsDevice = new CPCInputsDevice(CPCSpecific::m_hWnd);

    if (!PCSddInitializeADX(CPCSpecific::m_hWnd))
    {
        OUTPUT("adx sd init failed");
        return false;
    };

    if (!PCSddInitializeFramework())
    {
        OUTPUT("framework sd init failed");
        return false;
    };

    if (!CFramework::Initialize())
    {
        OUTPUT("framework initialize failed");
        return false;
    };
    
#ifdef _DEBUG
    CPCDebug::ReplaceConsole();
    CPCDebug::MaximizeConsole(true);
#endif
    m_pSystem->WindowShow(false);
    
    return true;
};


void CPCFramework::Terminate(void)
{
    CFramework::Terminate();

    PCSddTerminateFramework();
    PCSddTerminateADX();

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


CPCGraphicsDevice& CPCFramework::GraphicsDevice(void)
{
    ASSERT(m_pGraphicsDevice);
    return *m_pPCGraphicsDevice;
};