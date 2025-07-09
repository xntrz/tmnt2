#include "ModuleManager.hpp"
#include "Module.hpp"


CModuleManager::CModuleManager(void)
: m_listModule()
{
    ;
};


CModuleManager::~CModuleManager(void)
{
    auto it = m_listModule.begin();
    auto itEnd = m_listModule.end();
    while (it != itEnd)
    {
        IModule* pModule = &(*it);
        it = m_listModule.erase(it);

        delete pModule;
    };
};


void CModuleManager::Run(void)
{
    auto it = m_listModule.begin();
    auto itEnd = m_listModule.end();
	while (it != itEnd)
	{
		IModule* pModule = &(*it);
		++it;

		pModule->Run(); // some modules may unlink and delete self, so increment it before run
	};
};


void CModuleManager::Draw(void)
{
    for (IModule& it : m_listModule)
        it.Draw();
};


void CModuleManager::Include(IModule* pModule)
{
    ASSERT(pModule);
    ASSERT(!m_listModule.contains(pModule));

    auto it = m_listModule.begin();
    auto itEnd = m_listModule.end();
    while (it != itEnd)
    {
        if (pModule->GetType() < (*it).GetType())
            break;

        ++it;
    };

    if (it)
        m_listModule.insert(it, pModule);
    else
        m_listModule.push_back(pModule);
};


bool CModuleManager::IsIncluded(MODULETYPE::VALUE moduletype)
{
    return (GetModule(moduletype) != nullptr);
};


IModule* CModuleManager::GetModule(MODULETYPE::VALUE moduletype)
{
    for (IModule& it : m_listModule)
    {
        if (it.GetType() == moduletype)
            return &it;
    };

    return nullptr;
};