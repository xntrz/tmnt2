#pragma once

#include "Module.hpp"


class CPlayerCharacter;


class CToGimmickMessageModule : public IModule
{
public:
    static CToGimmickMessageModule* New(CPlayerCharacter* pPlayerCharacter);
    
protected:
    CToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter);
    
public:    
    virtual ~CToGimmickMessageModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual bool DirectionCheck(void);
    void SendMessageCommon(const char* pszGimmickObjName);
    void SetPrevGimmickObjName(const char* pszGimmickObjName = nullptr);

protected:    
    CPlayerCharacter* m_pPlayerCharacter;
    char m_szPrevGimmickObjName[16];
};


class CDefaultCharacterToGimmickMessageModule : public CToGimmickMessageModule
{
public:
    CDefaultCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter);
    virtual ~CDefaultCharacterToGimmickMessageModule(void);
};


class CPowerfullCharacterToGimmickMessageModule : public CToGimmickMessageModule
{
public:
    CPowerfullCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter);
    virtual ~CPowerfullCharacterToGimmickMessageModule(void);
    virtual void Run(void) override;
};


class CConsoleCharacterToGimmickMessageModule : public CToGimmickMessageModule
{
public:
    CConsoleCharacterToGimmickMessageModule(CPlayerCharacter* pPlayerCharacter);
    virtual ~CConsoleCharacterToGimmickMessageModule(void);
    virtual void Run(void) override;
    virtual bool DirectionCheck(void) override;
    virtual bool DistanceCheck(void);
};