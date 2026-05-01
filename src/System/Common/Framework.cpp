#include "Framework.hpp"
#include "GraphicsDevice.hpp"
#include "InputsDevice.hpp"
#include "SystemTime.hpp"
#include "SystemText.hpp"
#include "SoundDevice.hpp"

#include "Process/ProcessDispatcher.hpp"
#include "File/FileManager.hpp"

#if defined(TARGET_PC)
#include "System/PC/PCFramework.hpp"
#elif defined(TARGET_WEB)
#include "System/Web/WebFramework.hpp"
#endif


extern const PROCESSTYPES::PROCESS g_aProcessList[];


/*static*/ CFramework* CFramework::GetConcreteInstance(void)
{
    CFramework* pFramework = nullptr;

#if defined(TARGET_PC)
    pFramework = new CPCFramework;
#elif defined(TARGET_WEB)
    pFramework = new CWebFramework;
#endif
    
    return pFramework;
};


/*static*/ bool CFramework::StartAndRun(void)
{
    bool bResult = false;
    
    CFramework* pFramework = CFramework::GetConcreteInstance();
    if (pFramework)
    {
        if (pFramework->Initialize())
        {
            pFramework->Run();
            pFramework->Terminate();
            bResult = true;
        }
        else
        {
            pFramework->Terminate();
        };

        delete pFramework;
        pFramework = nullptr;
    };

    return bResult;
};


CFramework::CFramework(void)
: m_pMemory(nullptr)
, m_pGraphicsDevice(nullptr)
, m_pInputsDevice(nullptr)
, m_pSoundDevice(nullptr)
, m_pFileManager(nullptr)
, m_pClockDevice(nullptr)
, m_pProcessDispatcher(nullptr)
{
    ;
};


CFramework::~CFramework(void)
{
    ;
};


bool CFramework::Initialize(void)
{
    ASSERT(m_pGraphicsDevice);
    ASSERT(m_pInputsDevice);
    ASSERT(m_pFileManager);
    ASSERT(!m_pProcessDispatcher);

    if (!m_pGraphicsDevice->Initialize())
    {
        OUTPUT("GX device init failed!\n");
        return false;
    };

    if (!m_pFileManager->Start())
    {
        OUTPUT("FS manager start failed!\n");
        return false;
    };

    if (!m_pInputsDevice->Start())
    {
        OUTPUT("INPUT device start failed!\n");
        return false;
    };

    if (!m_pGraphicsDevice->Start())
    {
        OUTPUT("GX device start failed!\n");
        return false;
    };

    CSystemTime::Instance().AttachClock(m_pClockDevice);
    CSystemText::Initialize();

    m_pProcessDispatcher = new CProcessDispatcher(g_aProcessList);

    return m_pProcessDispatcher->Start();
};


void CFramework::Terminate(void)
{    
    if (m_pProcessDispatcher)
    {
        m_pProcessDispatcher->Stop();

        delete m_pProcessDispatcher;
        m_pProcessDispatcher = nullptr;
    };

    CSystemText::Terminate();
    CSystemTime::Instance().DetachClock();
    
    if (m_pGraphicsDevice)
        m_pGraphicsDevice->Stop();

    if (m_pInputsDevice)
        m_pInputsDevice->Stop();

    if (m_pFileManager)
        m_pFileManager->Stop();

    if (m_pGraphicsDevice)
        m_pGraphicsDevice->Terminate();
};


void CFramework::Run(void)
{
    while (!IsEnded())
    {
        Move();
        Render();
        Flip();
    };
};


void CFramework::Move(void)
{
    ASSERT(m_pProcessDispatcher);
    m_pProcessDispatcher->Move();
};


void CFramework::Render(void)
{
    ASSERT(m_pProcessDispatcher);
    ASSERT(m_pGraphicsDevice);

    if (m_pGraphicsDevice->RenderBegin())
    {
        m_pProcessDispatcher->Draw();
        m_pGraphicsDevice->RenderEnd();
    };
};


void CFramework::Flip(void)
{
    ASSERT(m_pProcessDispatcher);
    ASSERT(m_pGraphicsDevice);
    ASSERT(m_pInputsDevice);
    ASSERT(m_pFileManager);

    m_pProcessDispatcher->Dispatch();
    m_pGraphicsDevice->Flip();
    m_pInputsDevice->Sync();
    m_pFileManager->Sync();
};


bool CFramework::IsEnded(void) const
{
    return m_pProcessDispatcher->IsEnded();
};

