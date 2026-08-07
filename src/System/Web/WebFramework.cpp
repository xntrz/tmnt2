#include "WebFramework.hpp"
#include "WebSystem.hpp"
#include "WebGraphicsDevice.hpp"
#include "WebSoundDevice.hpp"
#include "WebClockDevice.hpp"
#include "WebInputsDevice.hpp"
#include "WebError.hpp"
#include "WebMemory.hpp"

#include "File/WebFileManager.hpp"
#include "System/Common/Process/ProcessDispatcher.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/System2D.hpp"


CWebFramework::CWebFramework(void)
: m_pSystem(nullptr)
, m_pWebGraphicsDevice(nullptr)
, m_pWebSoundDevice(nullptr)
, m_pWebInputsDevice(nullptr)
{
    ;
};


CWebFramework::~CWebFramework(void)
{
    ;
};


bool CWebFramework::Initialize(void)
{
    if (CConfigure::CheckArg("mp"))
        m_pMemory = new CWebMemoryPool;

    m_pSystem = new CWebSystem(this);
    if (!m_pSystem->Initialize())
    {
        OUTPUT("websystem init failed!\n");
        return false;
    };

    m_pClockDevice = new CWebClockDevice;

    m_pWebGraphicsDevice = new CWebGraphicsDevice;
    m_pGraphicsDevice = m_pWebGraphicsDevice;
    m_pGraphicsDevice->SetFlipEnable(true);

    m_pFileManager = new CWebFileManager;

    m_pWebInputsDevice = new CWebInputsDevice;
    m_pInputsDevice = m_pWebInputsDevice;

    m_pWebSoundDevice = new CWebSoundDevice;

    if (!CFramework::Initialize())
    {
        OUTPUT("framework initialize failed\n");
        return false;
    };

    if (!m_pWebSoundDevice->Initialize())
    {
        OUTPUT("adx sound init failed\n");
        CWebError::ShowNoRet("Sound initialize failed");
        return false;
    };

    if (!m_pWebSoundDevice->StartupFramework())
    {
        OUTPUT("lib sound init failed\n");
        CWebError::ShowNoRet("Sound framework initialize failed");
        return false;
    };

    return true;
};


void CWebFramework::Terminate(void)
{
    CFramework::Terminate();

    if (m_pWebSoundDevice)
    {
        m_pWebSoundDevice->ShutdownFramework();
        m_pWebSoundDevice->Terminate();
        
        delete m_pWebSoundDevice;
        m_pWebSoundDevice = nullptr;
    };

    if (m_pInputsDevice)
    {
        delete m_pInputsDevice;
        m_pInputsDevice = nullptr;
        m_pWebInputsDevice = nullptr;
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
        m_pWebGraphicsDevice = nullptr;
    };

    if (m_pClockDevice)
    {
        delete m_pClockDevice;
        m_pClockDevice = nullptr;
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


void CWebFramework::Run(void)
{
    m_pSystem->Run();
};


void CWebFramework::Move(void)
{
    m_pProcessDispatcher->Move();
    m_pProcessDispatcher->Dispatch();
};


void CWebFramework::Render(void)
{
    if (m_pGraphicsDevice->RenderBegin())
    {
        m_pProcessDispatcher->Draw();
        m_pGraphicsDevice->RenderEnd();
    };
};


void CWebFramework::Flip(void)
{
    m_pGraphicsDevice->Flip();
};


void CWebFramework::SyncInput(void)
{
    m_pInputsDevice->Sync();
};


void CWebFramework::SyncFS(void)
{
    m_pFileManager->Sync();
};


void CWebFramework::OnScreenSizeChanged(int32 w, int32 h)
{
    m_pWebGraphicsDevice->DestroyFrameBuffer();
    m_pWebGraphicsDevice->CreateFrameBuffer();

    CSystem2D::Reset();
    CScreen::DeviceChanged();
};


void CWebFramework::OnJoystickAdd(uint32 joystickID)
{
    m_pWebInputsDevice->OnJoystickAdd(joystickID);
};


void CWebFramework::OnJoystickRem(uint32 joystickID)
{
    m_pWebInputsDevice->OnJoystickRem(joystickID);
};