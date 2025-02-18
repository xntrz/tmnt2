#include "PCInputsDevice.hpp"
#include "PCPhysicalController.hpp"
#include "PCSpecific.hpp"


void CPCInputsDevice::SyncVirtualController(void)
{
    CInputsDevice::SyncVirtualController();

    int32 iKbrdPort = CPCPhysicalController::GetPort();
    if (iKbrdPort < CController::Max())
    {
        const uint32 KbrdDigitalMask = CController::DIGITAL_LUP
                                     | CController::DIGITAL_LDOWN	
                                     | CController::DIGITAL_LLEFT	
                                     | CController::DIGITAL_LRIGHT	
                                     | CController::DIGITAL_SELECT	
                                     | CController::DIGITAL_START;

        IPhysicalController::INFO& KbrdInfo = ControllerResource(iKbrdPort).Info();
        IPhysicalController::INFO& LockedInfo = GetLockedVirtualController();
        IPhysicalController::INFO& UnlockedInfo = GetUnlockedVirtualController();
        
        LockedInfo.m_digital 			|= (KbrdInfo.m_digital & KbrdDigitalMask);
        LockedInfo.m_digitalTrigger 	|= (KbrdInfo.m_digitalTrigger & KbrdDigitalMask);
        LockedInfo.m_digitalRelease 	|= (KbrdInfo.m_digitalRelease & KbrdDigitalMask);
        LockedInfo.m_digitalRepeat 		|= (KbrdInfo.m_digitalRepeat & KbrdDigitalMask);

        UnlockedInfo.m_digital 			|= (KbrdInfo.m_digital & KbrdDigitalMask);
        UnlockedInfo.m_digitalTrigger 	|= (KbrdInfo.m_digitalTrigger & KbrdDigitalMask);
        UnlockedInfo.m_digitalRelease 	|= (KbrdInfo.m_digitalRelease & KbrdDigitalMask);
        UnlockedInfo.m_digitalRepeat 	|= (KbrdInfo.m_digitalRepeat & KbrdDigitalMask);
    };
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
    if (CPCPhysicalController::Initialize())
    {
        m_iControllerMax = CPCPhysicalController::PHYSICALCONTROLLER_MAX;

        ASSERT(!m_ppPhysicalController);
        m_ppPhysicalController = new IPhysicalController*[m_iControllerMax];
        
        for (int32 i = 0; i < m_iControllerMax; ++i)
        {
            m_ppPhysicalController[i] = CPCPhysicalController::Open(i);
            ASSERT(m_ppPhysicalController[i]);
        };

        ASSERT(!m_pbLockList);		
        m_pbLockList = new bool[m_iControllerMax];
        
        for (int32 i = 0; i < m_iControllerMax; ++i)
            m_pbLockList[i] = false;

        CController::DIGITAL_OK 	= CController::DIGITAL_START  | CController::DIGITAL_RDOWN;
        CController::DIGITAL_CANCEL = CController::DIGITAL_SELECT | CController::DIGITAL_RLEFT;

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