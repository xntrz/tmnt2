#include "WebInputsDevice.hpp"
#include "WebPhysicalController.hpp"
#include "WebSpecific.hpp"


CWebInputsDevice::CWebInputsDevice(void)
: m_iTouchPhysicalPort(-1)
{
    ;
};


void CWebInputsDevice::SyncVirtualController(void)
{
    CInputsDevice::SyncVirtualController();

    int32 port = CWebPhysicalController::GetPort();
    int32 controller = CController::GetController(port);
    
    if ((controller >= 0) &&
        (controller < CController::Max()))
    {
        const uint32 digitalMask = CController::DIGITAL_LUP
                                 | CController::DIGITAL_LDOWN	
                                 | CController::DIGITAL_LLEFT	
                                 | CController::DIGITAL_LRIGHT	
                                 | CController::DIGITAL_SELECT	
                                 | CController::DIGITAL_START;

        IPhysicalController::INFO& device = ControllerResource(controller).Info();
        IPhysicalController::INFO& locked = GetLockedVirtualController();
        IPhysicalController::INFO& unlocked = GetUnlockedVirtualController();
        
        locked.m_digital          |= (device.m_digital & digitalMask);
        locked.m_digitalTrigger   |= (device.m_digitalTrigger & digitalMask);
        locked.m_digitalRelease   |= (device.m_digitalRelease & digitalMask);
        locked.m_digitalRepeat    |= (device.m_digitalRepeat & digitalMask);

        unlocked.m_digital        |= (device.m_digital & digitalMask);
        unlocked.m_digitalTrigger |= (device.m_digitalTrigger & digitalMask);
        unlocked.m_digitalRelease |= (device.m_digitalRelease & digitalMask);
        unlocked.m_digitalRepeat  |= (device.m_digitalRepeat & digitalMask);
    };
};


void CWebInputsDevice::Sync(void)
{
    ASSERT(m_ppPhysicalController);
    
    for (int32 i = 0; i < m_iControllerMax; ++i)
    {
        IPhysicalController* pPhysicalController = m_ppPhysicalController[i];

        if (!pPhysicalController)
            continue;

        if (pPhysicalController->Info().m_iPhysicalPort == m_iTouchPhysicalPort)
            continue;

        pPhysicalController->Update();
    };

    Compaction();
    SyncVirtualController();
};


bool CWebInputsDevice::Start(void)
{
    if (CWebPhysicalController::Initialize())
    {
        m_iControllerMax = CWebPhysicalController::PHYSICALCONTROLLER_MAX;

        ASSERT(!m_ppPhysicalController);
        m_ppPhysicalController = new IPhysicalController*[m_iControllerMax];
        
        for (int32 i = 0; i < m_iControllerMax; ++i)
        {
            m_ppPhysicalController[i] = CWebPhysicalController::Open(i);
            ASSERT(m_ppPhysicalController[i]);
        };

        ASSERT(!m_pbLockList);		
        m_pbLockList = new bool[m_iControllerMax];
        
        for (int32 i = 0; i < m_iControllerMax; ++i)
            m_pbLockList[i] = false;

        CController::DIGITAL_OK = CController::DIGITAL_START |
                                  CController::DIGITAL_RDOWN;
        
        CController::DIGITAL_CANCEL = CController::DIGITAL_SELECT |
                                      CController::DIGITAL_RLEFT;

        m_iTouchPhysicalPort = CWebSpecific::GetTouchPort();

        return true;
    };

    return false;
};


void CWebInputsDevice::Stop(void)
{
    m_iTouchPhysicalPort = -1;

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

    CWebPhysicalController::Terminate();
};


void CWebInputsDevice::OnJoystickAdd(uint32 joystickID)
{
    CWebPhysicalController::OnJoystickAdd(joystickID);
};


void CWebInputsDevice::OnJoystickRem(uint32 joystickID)
{
    ;
};