#include "PCInputsDevice.hpp"
#include "PCPhysicalController.hpp"
#include "PCSpecific.hpp"


void CPCInputsDevice::SyncVirtualController(void)
{
    CInputsDevice::SyncVirtualController();

    int32 keyboardController = CController::GetController(CPCPhysicalController::GetPort());
    if (keyboardController < CController::Max())
    {
        const uint32 keyboardDigitalMask = CController::DIGITAL_LUP
                                         | CController::DIGITAL_LDOWN	
                                         | CController::DIGITAL_LLEFT	
                                         | CController::DIGITAL_LRIGHT	
                                         | CController::DIGITAL_SELECT	
                                         | CController::DIGITAL_START;

        IPhysicalController::INFO& keyboard = ControllerResource(keyboardController).Info();
        IPhysicalController::INFO& locked = GetLockedVirtualController();
        IPhysicalController::INFO& unlocked = GetUnlockedVirtualController();
        
        locked.m_digital          |= (keyboard.m_digital & keyboardDigitalMask);
        locked.m_digitalTrigger   |= (keyboard.m_digitalTrigger & keyboardDigitalMask);
        locked.m_digitalRelease   |= (keyboard.m_digitalRelease & keyboardDigitalMask);
        locked.m_digitalRepeat    |= (keyboard.m_digitalRepeat & keyboardDigitalMask);

        unlocked.m_digital        |= (keyboard.m_digital & keyboardDigitalMask);
        unlocked.m_digitalTrigger |= (keyboard.m_digitalTrigger & keyboardDigitalMask);
        unlocked.m_digitalRelease |= (keyboard.m_digitalRelease & keyboardDigitalMask);
        unlocked.m_digitalRepeat  |= (keyboard.m_digitalRepeat & keyboardDigitalMask);
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

        CController::DIGITAL_OK = CController::DIGITAL_START | CController::DIGITAL_RDOWN;
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