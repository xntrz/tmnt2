#pragma once

#include "Module.hpp"


class CPlayerCharacter;


class CWallJumpSignModule : public IModule
{
public:
    CWallJumpSignModule(CPlayerCharacter* pPlayerCharacter);
    virtual ~CWallJumpSignModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void StartEffect(void);
    void StopEffect(void);

protected:
    CPlayerCharacter* m_pPlayerCharacter;
    uint32 m_hEffect;
    bool m_bEffectRun;
};