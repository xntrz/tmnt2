#pragma once

#include "Module.hpp"


class CCharacter;
class CPlayerCharacter;


class CFootfallModule : public IModule
{
public:
    CFootfallModule(CCharacter* pCharacter);
    virtual ~CFootfallModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual bool TimingCheck(void);

private:
    CCharacter* m_pCharacter;
};


class CPlayerFootfallModule : public CFootfallModule
{
public:
    CPlayerFootfallModule(CPlayerCharacter* pPlayerChr);
    virtual ~CPlayerFootfallModule(void);
    virtual bool TimingCheck(void) override;

private:
    CPlayerCharacter* m_pPlayerChr;
};