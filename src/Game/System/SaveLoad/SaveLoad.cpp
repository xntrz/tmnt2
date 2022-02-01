#include "SaveLoad.hpp"
#include "SaveLoadData.hpp"
#include "SaveLoadFrame.hpp"
#include "SaveLoadFlow.hpp"


static CSaveLoadFlow::MODE TypeToMode(CSaveLoad::TYPE type)
{
    switch (type)
    {
    case CSaveLoad::TYPE_CHECK:
        return CSaveLoadFlow::MODE_CHECK;

    case CSaveLoad::TYPE_LOAD:
        return CSaveLoadFlow::MODE_LOAD;

    case CSaveLoad::TYPE_SAVE:
        return CSaveLoadFlow::MODE_SAVE;

    case CSaveLoad::TYPE_AUTOSAVE:
        return CSaveLoadFlow::MODE_AUTOSAVE;

    default:
        ASSERT(false);
        return CSaveLoadFlow::MODE_CHECK;
    };
};


class CSaveLoadSystem
{
public:
    CSaveLoadSystem(void);
    ~CSaveLoadSystem(void);
    void Start(CSaveLoad::TYPE type);
    void Stop(void);
    bool Run(void);    

private:
    CSaveLoadFlow* m_pFlow;
    CSaveLoadFrame* m_pFrame;
    CSaveLoadData* m_pData;
};


CSaveLoadSystem::CSaveLoadSystem(void)
: m_pFlow(nullptr)
, m_pFrame(nullptr)
, m_pData(nullptr)
{
    ;
};


CSaveLoadSystem::~CSaveLoadSystem(void)
{
    ASSERT(!m_pData);
    ASSERT(!m_pFrame);
    ASSERT(!m_pFlow);
};


void CSaveLoadSystem::Start(CSaveLoad::TYPE type)
{
    ASSERT(!m_pData);
    ASSERT(!m_pFrame);
    ASSERT(!m_pFlow);

    m_pData = new CSaveLoadData;
    m_pFrame = new CSaveLoadFrame;
    m_pFlow = CSaveLoadFlow::Create(TypeToMode(type), m_pFrame, m_pData);
	m_pFlow->Initialize();
};


void CSaveLoadSystem::Stop(void)
{
    ASSERT(m_pFlow);
    ASSERT(m_pFrame);
    ASSERT(m_pData);

	m_pFlow->Terminate();
    CSaveLoadFlow::Destroy(m_pFlow);
    m_pFlow = nullptr;

    delete m_pFrame;
    m_pFrame = nullptr;

    delete m_pData;
    m_pData = nullptr;
};


bool CSaveLoadSystem::Run(void)
{
    return m_pFlow->Run();
};


static CSaveLoadSystem* s_pSaveLoadSystem = nullptr;


static inline CSaveLoadSystem& SaveloadSystem(void)
{
    ASSERT(s_pSaveLoadSystem);
    return *s_pSaveLoadSystem;
};


/*static*/ void CSaveLoad::Initialize(TYPE type)
{
    if (!s_pSaveLoadSystem)
    {
        s_pSaveLoadSystem = new CSaveLoadSystem;
        ASSERT(s_pSaveLoadSystem);
        
        s_pSaveLoadSystem->Start(type);
    };
};


/*static*/ void CSaveLoad::Terminate(void)
{
    if (s_pSaveLoadSystem)
    {
        s_pSaveLoadSystem->Stop();
        
        delete s_pSaveLoadSystem;
        s_pSaveLoadSystem = nullptr;
    };
};


/*static*/ bool CSaveLoad::Run(void)
{
    return SaveloadSystem().Run();
};


/*static*/ bool CSaveLoad::IsNewSave(void)
{
    return CSaveLoadFlow::m_bNewSave;
};