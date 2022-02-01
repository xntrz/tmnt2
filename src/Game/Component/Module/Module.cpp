#include "Module.hpp"


IModule::IModule(MODULETYPE::VALUE type)
: m_moduletype(type)
{
    ;
};


IModule::~IModule(void)
{
    ;
};


MODULETYPE::VALUE IModule::GetType(void) const
{
    return m_moduletype;
};