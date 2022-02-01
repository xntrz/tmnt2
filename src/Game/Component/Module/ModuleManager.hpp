#pragma once

#include "ModuleType.hpp"


class IModule;


class CModuleManager
{
public:
    CModuleManager(void);
    virtual ~CModuleManager(void);
    void Run(void);
    void Draw(void);
    void Include(IModule* pModule);
    bool IsIncluded(MODULETYPE::VALUE moduletype);
    IModule* GetModule(MODULETYPE::VALUE moduletype);

private:
    CList<IModule> m_listModule;
};