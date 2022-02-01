#pragma once

#include "Module.hpp"


class CCharacter;


class CDamageFloorModule : public IModule
{
public:
    CDamageFloorModule(CCharacter* pCharacter);
    virtual ~CDamageFloorModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;

private:
    CCharacter* m_pCharacter;
    float m_fTimer;
};