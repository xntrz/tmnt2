#include "PCInputsDevice.hpp"
#include "PCPhysicalController.hpp"


CPCInputsDevice::CPCInputsDevice(void* hWnd)
: m_hWnd(hWnd)
{
	;
};


CPCInputsDevice::~CPCInputsDevice(void)
{
	;
};


void CPCInputsDevice::Sync(void)
{
	ASSERT(m_ppPhysicalController);
	for (int32 i = 0; i < m_iControllerMax; ++i)
	{
		if (m_ppPhysicalController[i])
			m_ppPhysicalController[i]->Update();
	};

	Compaction();
	SyncVirtualController();
};


bool CPCInputsDevice::Start(void)
{
	if (CPCPhysicalController::Initialize(m_hWnd))
	{
		m_iControllerMax = CPCPhysicalController::PHYSICALCONTROLLER_MAX;

		ASSERT(!m_ppPhysicalController);
		m_ppPhysicalController = new IPhysicalController*[m_iControllerMax];
		ASSERT(m_ppPhysicalController);
		for (int32 i = 0; i < m_iControllerMax; ++i)
		{
			m_ppPhysicalController[i] = CPCPhysicalController::New(i);
			ASSERT(m_ppPhysicalController[i]);
		};

		ASSERT(!m_pbLockList);		
		m_pbLockList = new bool[m_iControllerMax];
		ASSERT(m_pbLockList);
		for (int32 i = 0; i < m_iControllerMax; ++i)
			m_pbLockList[i] = false;

		return true;
	};

	return false;
};


void CPCInputsDevice::Stop(void)
{
	if (m_pbLockList)
	{
		delete[] m_pbLockList;
		m_pbLockList = nullptr;
	};

	if (m_ppPhysicalController)
	{
		for (int32 i = 0; i < m_iControllerMax; ++i)
		{
			if (m_ppPhysicalController[i])
			{
				m_ppPhysicalController[i]->Close();
				m_ppPhysicalController[i] = nullptr;
			};
		};

		delete[] m_ppPhysicalController;
		m_ppPhysicalController = nullptr;
	};

	CPCPhysicalController::Terminate();
};