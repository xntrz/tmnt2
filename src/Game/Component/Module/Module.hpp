#pragma once

#include "ModuleType.hpp"


class IModule : public CListNode<IModule>
{
public:
    IModule(MODULETYPE::VALUE type);
    virtual ~IModule(void);
    virtual void Run(void) = 0;
    virtual void Draw(void) = 0;
    MODULETYPE::VALUE GetType(void) const;

private:
    MODULETYPE::VALUE m_moduletype;
};