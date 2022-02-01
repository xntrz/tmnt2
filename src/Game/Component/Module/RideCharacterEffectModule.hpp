#pragma once

#include "Module.hpp"


class CPlayerCharacter;
class CRideEffectUnit;


class CRideCharacterEffectModule : public IModule
{
public:
    enum RIDERTYPE
    {
        RIDERTYPE_BOARD = 0,
        RIDERTYPE_SHIP,
    };
    
public:
    static CRideCharacterEffectModule* New(CPlayerCharacter* pPlayerChr, RIDERTYPE ridertype);

protected:
    CRideCharacterEffectModule(CPlayerCharacter* pPlayerChr, RIDERTYPE ridertype);

public:
    virtual ~CRideCharacterEffectModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void SetDrawEnable(bool bEnable);
    void RegistUnit(CRideEffectUnit* pUnit);

protected:
    CList<CRideEffectUnit> m_listRideEffectUnit;
    CPlayerCharacter* m_pPlayerChr;
    RIDERTYPE m_ridertype;
    bool m_bDrawFlag;
};