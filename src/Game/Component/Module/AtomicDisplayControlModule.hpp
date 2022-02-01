#pragma once

#include "Module.hpp"


class CCharacter;
class CModel;


class CAtomicDisplayControlModule : public IModule
{
public:
    CAtomicDisplayControlModule(CCharacter* pCharacter);
    virtual ~CAtomicDisplayControlModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void Reset(void);    

private:
    CModel* m_pModel;
    CCharacter* m_pCharacter;    
};