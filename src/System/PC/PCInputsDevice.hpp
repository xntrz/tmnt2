#pragma once

#include "System/Common/InputsDevice.hpp"


class CPCInputsDevice : public CInputsDevice
{
public:
	CPCInputsDevice(void* hWnd);
	virtual ~CPCInputsDevice(void);
	virtual void Sync(void) override;
	virtual bool Start(void) override;
	virtual void Stop(void) override;

private:
	void* m_hWnd;
};