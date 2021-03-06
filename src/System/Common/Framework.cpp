#include "Framework.hpp"
#include "GraphicsDevice.hpp"
#include "InputsDevice.hpp"
#include "SystemTime.hpp"
#include "SystemText.hpp"

#include "Process/ProcessDispatcher.hpp"
#include "Process/ProcessList.hpp"
#include "File/FileManager.hpp"

#ifdef _TARGET_PC
#include "System/PC/PCFramework.hpp"
#else
#error Not implemented for current target
#endif


/*static*/ CFramework* CFramework::GetConcreteInstance(void)
{
    CFramework* pFramework = nullptr;

#ifdef _TARGET_PC
    pFramework = new CPCFramework();
#else
#error Not implemented for current target
#endif

    return pFramework;
};


CFramework::CFramework(void)
: m_pMemory(nullptr)
, m_pGraphicsDevice(nullptr)
, m_pInputsDevice(nullptr)
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


void CFramework::Startup(void)
{
    ;
};


void CFramework::Cleanup(void)
{
    ;
};


bool CFramework::Initialize(void)
{
    ASSERT(!m_pProcessDispatcher);
    
    ASSERT(m_pGraphicsDevice);
    ASSERT(m_pInputsDevice);
    ASSERT(m_pFileManager);

    if (!m_pGraphicsDevice->Initialize() ||
        !m_pGraphicsDevice->Start())
        return false;

    if (!m_pInputsDevice->Start())
        return false;

    if (!m_pFileManager->Start())
        return false;
    
    CSystemTime::Instance().AttachClock(m_pClockDevice);
    CSystemText::Initialize();

    m_pProcessDispatcher = new CProcessDispatcher(g_aProcessList);
    ASSERT(m_pProcessDispatcher);
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

    if(m_pFileManager)
        m_pFileManager->Stop();

    if(m_pInputsDevice)
        m_pInputsDevice->Stop();

    if (m_pGraphicsDevice)
    {
        m_pGraphicsDevice->Stop();
        m_pGraphicsDevice->Terminate();
    };
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

    // TODO impl
    //m_pSoundDevice->Sync();
};


bool CFramework::IsEnded(void) const
{
    return m_pProcessDispatcher->IsEnded();
};

