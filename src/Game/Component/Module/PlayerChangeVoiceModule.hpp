#pragma once

#include "Module.hpp"


class CPlayerCharacter;


//
//  TODO player change voice module
//
class CPlayerChangeVoiceModule : public IModule
{
public:
    CPlayerChangeVoiceModule(CPlayerCharacter* pPlayerChr);
    virtual ~CPlayerChangeVoiceModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void SetComboWall(void);
    void PlayChangeVoice(void);
    void PlayRandomVoice(void);
    void Start(void);

private:
    CPlayerCharacter* m_pPlayerChr;
    float m_fTimerProcComboWall;
    bool m_bComboWallRequest;
};