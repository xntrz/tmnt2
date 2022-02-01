#pragma once

#include "Module.hpp"


class CCharacter;


class CAmbientLightModule : public IModule
{
public:
    CAmbientLightModule(CCharacter* pCharacter);
    virtual ~CAmbientLightModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void SetEnable(bool bEnable);
    bool IsEnable(void) const;

private:
    float m_fElapsedTime;
    RwRGBAReal m_ColorTo;
    RwRGBAReal m_ColorFrom;
    CCharacter* m_pCharacter;
    bool m_bEnable;
};