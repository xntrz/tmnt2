#pragma once

#include "PhysicalController.hpp"


class CInputsDevice
{
public:
	static CInputsDevice& Instance(void);

	CInputsDevice(void);
	virtual ~CInputsDevice(void);
	virtual int32 Max(void);
	virtual bool Lock(int32 iController);
	virtual bool Unlock(int32 iController);
	virtual bool IsLocked(int32 iController);
	virtual void SetMaximumLocked(int32 num);
	virtual const IPhysicalController::INFO& GetLockedVirtualController(void) const;
	virtual const IPhysicalController::INFO& GetUnlockedVirtualController(void) const;
	virtual void Compaction(void);
	virtual void ExchangePhysicalController(int32 iController1, int32 iController2);
	virtual void SyncVirtualController(void);
	virtual void Sync(void) = 0;
	virtual bool Start(void) = 0;
	virtual void Stop(void) = 0;
	IPhysicalController& ControllerResource(int32 iController);

protected:
	static CInputsDevice* m_pInstance;
	IPhysicalController** m_ppPhysicalController;
	bool* m_pbLockList;
	int32 m_iControllerMax;
	int32 m_iLockedMax;
	int32 m_iLockedCount;
	IPhysicalController::INFO m_aInfoVController[2];
};