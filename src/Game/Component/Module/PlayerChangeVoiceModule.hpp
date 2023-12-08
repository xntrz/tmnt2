#pragma once

#include "Module.hpp"


class CPlayerCharacter;


class CPlayerChangeVoiceModule : public IModule
{
public:
    CPlayerChangeVoiceModule(CPlayerCharacter* pPlayerChr);
    virtual ~CPlayerChangeVoiceModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void PlayChangeVoice(void);
    void Start(void);
    void SetComboWall(void);
    float GetTime(void) const;
    
private:
    CPlayerCharacter* m_pPlayerChr;
    float m_fInitTStamp;
    float m_fChangeVoxRqTime;
    float m_fComboVoxRqTime;
    float m_fRandVoxRqTime;
    bool m_bChangeVoxRq;
    bool m_bComboVoxRq;
};