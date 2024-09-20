#pragma once

#include "Module.hpp"


class CCharacter;


class CInkThicknessCtrlModule : public IModule
{
public:
	CInkThicknessCtrlModule(CCharacter* pCharacter);
	virtual ~CInkThicknessCtrlModule(void) {};
	virtual void Run(void) override;
	virtual void Draw(void) override {};

private:
	CCharacter* m_pCharacter;
	float m_fScaling;
};