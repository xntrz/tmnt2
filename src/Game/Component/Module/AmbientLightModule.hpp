#pragma once

#include "Module.hpp"


class CCharacter;


class CAmbientLightModule : public IModule
{
public:
    CAmbientLightModule(CCharacter* pCharacter);
	virtual ~CAmbientLightModule(void) {};
    virtual void Run(void) override;
	virtual void Draw(void) override {};

	inline void SetEnable(bool bEnable) { m_bEnable = bEnable; };
	inline bool IsEnable(void) const { return m_bEnable; };

private:
    float m_fElapsedTime;
    RwRGBAReal m_ColorTo;
    RwRGBAReal m_ColorFrom;
    CCharacter* m_pCharacter;
    bool m_bEnable;
};