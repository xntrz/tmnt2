#pragma once

#include "Module.hpp"

#include "Game/Component/Player/PlayerTypes.hpp"


class CPlayerCharacter;


class CPlayerAttributeControlModule : public IModule
{
public:
    CPlayerAttributeControlModule(MODULETYPE::VALUE type, PLAYERTYPES::ATTRIBUTE attribute, CPlayerCharacter* pPlayerChr);
    virtual ~CPlayerAttributeControlModule(void) {};
    virtual void Run(void) override;
    virtual void Draw(void) override {};
    virtual void Enable(float fEndTime);
    virtual void Disable(void);
    virtual void SetRemainTime(float fTime);
    virtual float GetRemainTime(void) const;
    
protected:
    PLAYERTYPES::ATTRIBUTE  m_attribute;
    CPlayerCharacter*       m_pPlayerChr;
    float                   m_fElapsedTime;
    float                   m_fEndTime;
    bool                    m_bEnable;
};


class CConfusionAttributeControlModule : public CPlayerAttributeControlModule
{
private:
    static const RwRGBA CONFUSION_COLOR;

public:
    CConfusionAttributeControlModule(MODULETYPE::VALUE type, CPlayerCharacter* pPlayerChr);
    virtual ~CConfusionAttributeControlModule(void) {};
    virtual void Run(void) override;
    virtual void Enable(float fEndTime) override;
    virtual void Disable(void) override;
};