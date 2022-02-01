#include "InputsDevice.hpp"


CInputsDevice* CInputsDevice::m_pInstance = nullptr;


/*static*/ CInputsDevice& CInputsDevice::Instance(void)
{
	ASSERT(m_pInstance);
	return *m_pInstance;
};


CInputsDevice::CInputsDevice(void)
: m_ppPhysicalController(nullptr)
, m_pbLockList(nullptr)
, m_iControllerMax(0)
, m_iLockedMax(4)
, m_iLockedCount(0)
{
	m_pInstance = this;
};


CInputsDevice::~CInputsDevice(void)
{
	m_pInstance = nullptr;
};


int32 CInputsDevice::Max(void)
{
	return m_iControllerMax;
};


bool CInputsDevice::Lock(int32 iController)
{
	ASSERT(iController >= 0);
	ASSERT(iController < m_iControllerMax);
	ASSERT(m_pbLockList);

	if (m_pbLockList[iController])
		return true;

	if (m_iLockedCount < m_iLockedMax)
	{
		++m_iLockedCount;

		for (int32 i = 0; i < iController; ++i)
		{
			if (!m_pbLockList[i])
			{
				ExchangePhysicalController(i, iController);
				iController = i;
				break;
			};
		};

		m_pbLockList[iController] = true;
		Compaction();		
		return true;
	};

	return false;
};


bool CInputsDevice::Unlock(int32 iController)
{
	ASSERT(iController >= 0);
	ASSERT(iController < m_iControllerMax);
	ASSERT(m_pbLockList);

	if (m_pbLockList[iController])
	{
		ASSERT(m_iLockedCount > 0);
		--m_iLockedCount;
		m_pbLockList[iController] = false;
		Compaction();
	};

	return true;
};


bool CInputsDevice::IsLocked(int32 iController)
{
	ASSERT(iController >= 0);
	ASSERT(iController < m_iControllerMax);
	ASSERT(m_pbLockList);

	return m_pbLockList[iController];
};


void CInputsDevice::SetMaximumLocked(int32 num)
{
	m_iLockedMax = num;
};


const IPhysicalController::INFO& CInputsDevice::GetLockedVirtualController(void) const
{
	return m_aInfoVController[0];
};


const IPhysicalController::INFO& CInputsDevice::GetUnlockedVirtualController(void) const
{
	return m_aInfoVController[1];
};


void CInputsDevice::Compaction(void)
{
	ASSERT(m_ppPhysicalController);
	ASSERT(m_pbLockList);

	for (int32 i = 0; i < m_iControllerMax; ++i)
	{
		if (m_pbLockList[i])
			continue;

		if(ControllerResource(i).Info().m_state == CController::STATE_UNCONNECT)
		{
			for (int32 j = i + 1; j < m_iControllerMax; ++j)
			{
				if (ControllerResource(j).Info().m_state == CController::STATE_CONNECT)
				{
					ExchangePhysicalController(i, j);
					break;
				};
			};
		}
		else if(ControllerResource(i).Info().m_state == CController::STATE_CONNECT)
		{
			for (int32 j = i + 1; j < m_iControllerMax; ++j)
			{
				if (!m_pbLockList[j] &&
					ControllerResource(j).Info().m_state == CController::STATE_CONNECT &&
					ControllerResource(i).Info().m_iPhysicalPort > ControllerResource(j).Info().m_iPhysicalPort)
				{
					ExchangePhysicalController(i, j);
				};
			};
		};
	};
};


void CInputsDevice::ExchangePhysicalController(int32 iController1, int32 iController2)
{
	ASSERT(iController1 >= 0);
	ASSERT(iController1 < m_iControllerMax);
	ASSERT(iController2 >= 0);
	ASSERT(iController2 < m_iControllerMax);
	ASSERT(m_ppPhysicalController);
	ASSERT(m_pbLockList);

	IPhysicalController* pPhysicalController = m_ppPhysicalController[iController1];
	m_ppPhysicalController[iController1] = m_ppPhysicalController[iController2];
	m_ppPhysicalController[iController2] = pPhysicalController;
};


void CInputsDevice::SyncVirtualController(void)
{
	m_aInfoVController[0] = { 0 };
	m_aInfoVController[0].m_iPhysicalPort	= CController::CONTROLLER_LOCKED_ON_VIRTUAL;
	m_aInfoVController[0].m_state			= CController::STATE_CONNECT;
	
	m_aInfoVController[1] = { 0 };
	m_aInfoVController[1].m_iPhysicalPort	= CController::CONTROLLER_UNLOCKED_ON_VIRTUAL;
	m_aInfoVController[1].m_state			= CController::STATE_CONNECT;

	for (int32 i = 0; i < m_iControllerMax; ++i)
	{
		ASSERT(m_ppPhysicalController[i]);
		ASSERT(m_pbLockList);

		IPhysicalController* pPhysicalController = m_ppPhysicalController[i];
		IPhysicalController::INFO* info = &m_aInfoVController[0];
		if (!m_pbLockList[i])
			info = &m_aInfoVController[1];

		info->m_digital			|= pPhysicalController->Info().m_digital;
		info->m_digitalOld		|= pPhysicalController->Info().m_digitalOld;
		info->m_digitalTrigger	|= pPhysicalController->Info().m_digitalTrigger;
		info->m_digitalRelease	|= pPhysicalController->Info().m_digitalRelease;
		info->m_digitalRepeat	|= pPhysicalController->Info().m_digitalRepeat;

		for (int32 i = 0; i < CController::ANALOG_NUM; ++i)
		{
			if (info->m_aAnalog[i] < 0)
			{
				if (pPhysicalController->Info().m_aAnalog[i] >= 0)
					info->m_aAnalog[i] += pPhysicalController->Info().m_aAnalog[i];
				else if (pPhysicalController->Info().m_aAnalog[i] < info->m_aAnalog[i])
					info->m_aAnalog[i] = pPhysicalController->Info().m_aAnalog[i];
			}
			else
			{
				if (pPhysicalController->Info().m_aAnalog[i] < 0)
					info->m_aAnalog[i] += pPhysicalController->Info().m_aAnalog[i];
				else if (info->m_aAnalog[i] < pPhysicalController->Info().m_aAnalog[i])
					info->m_aAnalog[i] = pPhysicalController->Info().m_aAnalog[i];
			};
		};
	};
};


IPhysicalController& CInputsDevice::ControllerResource(int32 iController)
{
	ASSERT(iController >= 0);
	ASSERT(iController < m_iControllerMax);
	ASSERT(m_ppPhysicalController[iController]);
	return *m_ppPhysicalController[iController];
};