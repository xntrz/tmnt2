#pragma once

#include "System/Common/InputsDevice.hpp"


class CPCInputsDevice : public CInputsDevice
{
public:
	inline CPCInputsDevice(void) {};
	virtual ~CPCInputsDevice(void) {};
	virtual void SyncVirtualController(void) override;
	virtual void Sync(void) override;
	virtual bool Start(void) override;
	virtual void Stop(void) override;
};