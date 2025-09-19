#include "MenuController.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Misc/Gamepad.hpp"


/*static*/ const char* CMenuController::m_apszButtonLabel[] =
{
    //
    //  Virtual controller
    //
    "btUp",
    "btDown",
    "btLeft",
    "btRight",
    "btOk",
    "btCancel",
    "btSelect",
    "btStart",
    "btMenu",

    //
    //  Player 1
    //
    "btP1Up",
    "btP1Down",
    "btP1Left",
    "btP1Right",
    "btP1Ok",
    "btP1Cancel",
    "btP1Select",
    "btP1Start",
    "btP1Menu",

    //
    //  Player 2
    //
    "btP2Up",
    "btP2Down",
    "btP2Left",
    "btP2Right",
    "btP2Ok",
    "btP2Cancel",
    "btP2Select",
    "btP2Start",
    "btP2Menu",

    //
    //  Player 3
    //
    "btP3Up",
    "btP3Down",
    "btP3Left",
    "btP3Right",
    "btP3Ok",
    "btP3Cancel",
    "btP3Select",
    "btP3Start",
    "btP3Menu",

    //
    //  Player 4
    //
    "btP4Up",
    "btP4Down",
    "btP4Left",
    "btP4Right",
    "btP4Ok",
    "btP4Cancel",
    "btP4Select",
    "btP4Start",
    "btP4Menu",
};


/*static*/ uint32 CMenuController::m_aDigitalDataTable[] =
{
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
    IPad::DIGITAL,
};


/*static*/ bool CMenuController::m_abLockKeyData[] =
{
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
};


/*static*/ bool CMenuController::Initialize(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aDigitalDataTable); ++i)
    {
        uint32 uDigital = IPad::DIGITAL;
        
        switch (i)
        {
        case BUTTON_ID_UP:
            uDigital = IPad::DIGITAL_LUP;
            break;
            
        case BUTTON_ID_DOWN:
            uDigital = IPad::DIGITAL_LDOWN;
            break;
            
        case BUTTON_ID_LEFT:
            uDigital = IPad::DIGITAL_LLEFT;
            break;
            
        case BUTTON_ID_RIGHT:
            uDigital = IPad::DIGITAL_LRIGHT;
            break;
            
        case BUTTON_ID_OK:
            uDigital = IPad::DIGITAL_OK;
            break;
            
        case BUTTON_ID_CANCEL:
            uDigital = IPad::DIGITAL_CANCEL;
            break;
            
        case BUTTON_ID_SELECT:
            uDigital = IPad::DIGITAL_SELECT;
            break;
            
        case BUTTON_ID_START:
            uDigital = IPad::DIGITAL_START;
            break;
            
        case BUTTON_ID_MENU:
            uDigital = IPadFunctionMask(IPad::FUNCTION_GUARD);
            break;

        default:
            ASSERT(false);
            break;
        };

        m_aDigitalDataTable[i] = uDigital;
        m_abLockKeyData[i] = false;		
    };

    return true;
};


/*static*/ void CMenuController::Terminate(void)
{
    ;
};


/*static*/ void CMenuController::KeyLock(uint32 uKey)
{
    m_abLockKeyData[GetPadToButtonID(uKey)] = true;
};


/*static*/ void CMenuController::KeyUnlock(uint32 uKey)
{
    m_abLockKeyData[GetPadToButtonID(uKey)] = false;
};


/*static*/ CMenuController::BUTTON_ID CMenuController::GetPadToButtonID(uint32 uKey)
{
    if (uKey == IPad::DIGITAL_OK)
        return BUTTON_ID_OK;

    if (uKey == IPad::DIGITAL_CANCEL)
        return BUTTON_ID_CANCEL;

    switch (uKey)
    {
    case IPad::DIGITAL_LUP:        
        return BUTTON_ID_UP;

    case IPad::DIGITAL_LDOWN:
        return BUTTON_ID_DOWN;

    case IPad::DIGITAL_LLEFT:
        return BUTTON_ID_LEFT;

    case IPad::DIGITAL_LRIGHT:
        return BUTTON_ID_RIGHT;

    case IPad::DIGITAL_SELECT:
        return BUTTON_ID_SELECT;

    case IPad::DIGITAL_START:
        return BUTTON_ID_START;

    case IPadFunctionMask(IPad::FUNCTION_GUARD):
        return BUTTON_ID_MENU;

    default:
        ASSERT(false);
        return BUTTON_ID_MAX;
    };
};


CMenuController::CMenuController(void)
{
	m_aControllerEnableBit[0] = 0;
	m_aControllerEnableBit[1] = 0;
	m_aControllerEnableBit[2] = 0;
	m_aControllerEnableBit[3] = 0;
	m_aControllerEnableBit[4] = 0;

    static_assert(COUNT_OF(m_aDigitalDataTable) == BUTTON_ID_MAX, "update me");
    static_assert(COUNT_OF(m_abLockKeyData) == BUTTON_ID_MAX, "update me");
    static_assert(COUNT_OF(m_apszButtonLabel) == (CONTROLLER_ID_MAX * BUTTON_ID_MAX), "update me");
};


CMenuController::~CMenuController(void)
{
    ;
};


void CMenuController::CheckButtonLabelList(Rt2dMaestro* pMaestro)
{
    ASSERT(pMaestro);

    for (int32 i = 0; i < COUNT_OF(m_aButtonLabelID); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(m_aButtonLabelID[0]); ++j)
        {
            int32 index = -1;
			int32 idx = (i * COUNT_OF(m_aButtonLabelID[0])) + j;
            if (Rt2dMaestroFindStringLabel(pMaestro, rt2dANIMLABELTYPEBUTTON, m_apszButtonLabel[idx], &index))
            {
                m_aButtonLabelID[i][j] = index;
                m_aControllerEnableBit[i] |= m_aDigitalDataTable[j];
            }
            else
            {
                m_aButtonLabelID[i][j] = -1;
            };
        };        
    };
};


void CMenuController::SetButtonState(Rt2dMaestro* pMaestro, CONTROLLER_ID iController, BUTTON_ID iButton, int32 iAnimButtonState)
{
    ASSERT(pMaestro);
    ASSERT(iController >= 0);
    ASSERT(iController < CONTROLLER_ID_MAX);
    ASSERT(iController >= 0);
    ASSERT(iController < COUNT_OF(m_aButtonLabelID));
    ASSERT(iButton >= 0);
    ASSERT(iButton < BUTTON_ID_MAX);
    ASSERT(iButton >= 0);
    ASSERT(iButton < COUNT_OF(m_aButtonLabelID[0]));

    int32 index = m_aButtonLabelID[iController][iButton];
    if (index >= 0)
    {
        ButtonByLabelPacket packet;
        packet.btnId = index;
        packet.btnState = iAnimButtonState;

        Rt2dMaestroForAllVisibleAnimations(pMaestro, AnimationsCallBack, &packet);
    };
};


void CMenuController::Trigger(Rt2dMaestro* pMaestro)
{    
    ASSERT(pMaestro);

    int32 aController[] = { IPad::CONTROLLER_LOCKED_ON_VIRTUAL,
                            IPad::CONTROLLER_1,
                            IPad::CONTROLLER_2,
                            IPad::CONTROLLER_3,
                            IPad::CONTROLLER_4, };

    static_assert(COUNT_OF(aController) == CONTROLLER_ID_MAX, "checkout");

    aController[0] = CGameData::Attribute().GetVirtualPad();

    int32 iControllerMax = IPad::Max();
    if (iControllerMax > (CONTROLLER_ID_MAX - 1))
        iControllerMax = (CONTROLLER_ID_MAX - 1);

    for (int32 i = 0; i < iControllerMax; ++i)
    {
        uint32 digital = IPad::GetDigitalTrigger(aController[i]);
        
        if (!(m_aControllerEnableBit[i] & digital))
            continue;

		bool bEnableSetState = true;

        for (int32 j = 0; j < BUTTON_ID_MAX; ++j)
        {
			bool b1 = (j != BUTTON_ID_START);
			bool b2 = ((m_aDigitalDataTable[BUTTON_ID_START] & m_aDigitalDataTable[BUTTON_ID_OK]) == 0);
			bool b3 = (m_abLockKeyData[j] != true);

			if ((b1 || b2) && b3)
			{
				if (IPad::GetDigitalTrigger(aController[i], m_aDigitalDataTable[j]))
				{
					if (bEnableSetState)
					{
                        if (j < BUTTON_ID_MENU)
							bEnableSetState = false;

                        SetButtonState(pMaestro,
                                       static_cast<CONTROLLER_ID>(i),
                                       static_cast<BUTTON_ID>(j),
                                       rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN);
                    };
				};
			};
        };
    };
};


void CMenuController::KeyPress(Rt2dMaestro* pMaestro, uint32 uKey)
{
    ASSERT(pMaestro);

    BUTTON_ID iButton = GetPadToButtonID(uKey);
    ASSERT(iButton >= 0);
    ASSERT(iButton < BUTTON_ID_MAX);

    int32 index = m_aButtonLabelID[CONTROLLER_ID_VIRTUAL][iButton];
    if (index > -1)
    {
        ButtonByLabelPacket Packet;
        Packet.btnId = index;
        Packet.btnState = rt2dANIMBUTTONSTATEOVERUPTOOVERDOWN;

        Rt2dMaestroForAllVisibleAnimations(pMaestro, AnimationsCallBack, &Packet);
    };
};


void CMenuController::FlashUnlockKeyEnable(bool bEnable)
{
    m_bUnlock = bEnable;
};


/*static*/ Rt2dMaestro* CMenuController::AnimationsCallBack(Rt2dMaestro* maestro, Rt2dAnim* anim, Rt2dAnimProps* props, void* pData)
{
    ButtonByLabelPacket* pPacket = static_cast<ButtonByLabelPacket*>(pData);
    ASSERT(pPacket);

    Rt2dMessage msg = { 0 };    
    msg.messageType = rt2dMESSAGETYPEBUTTONBYLABEL;
    msg.index       = -1;
    msg.intParam1   = pPacket->btnId;
    msg.intParam2   = pPacket->btnState;
    
    return Rt2dMaestroPostMessages(maestro, &msg, 1);
};