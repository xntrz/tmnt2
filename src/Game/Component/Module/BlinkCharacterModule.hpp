#pragma once

#include "Module.hpp"


class CPlayerCharacter;
class CModel;


class CBlinkCharacterModule : public IModule
{
public:
    CBlinkCharacterModule(CPlayerCharacter* pPlayerCharacter);
    virtual ~CBlinkCharacterModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void Start(float fEndTime);
    void End(void);
    void SetRemainTime(float fTime);
    float GetRemainTime(void) const;

private:
    CPlayerCharacter* m_pPlayerChr;
    CModel* m_pModel;
    float m_fElapsedTime;
    float m_fNextIntervalTime;
    float m_fEndTime;
    bool m_bBlinking;    
};