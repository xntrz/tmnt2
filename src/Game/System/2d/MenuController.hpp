#pragma once


class CMenuController
{
public:
    enum CONTROLLER_ID
    {
        CONTROLLER_ID_VIRTUAL = 0,
        CONTROLLER_ID_P1,
        CONTROLLER_ID_P2,
        CONTROLLER_ID_P3,
        CONTROLLER_ID_P4,
        
        CONTROLLER_ID_MAX,
    };

    enum BUTTON_ID
    {
        BUTTON_ID_UP = 0,
        BUTTON_ID_DOWN,
        BUTTON_ID_LEFT,
        BUTTON_ID_RIGHT,
        BUTTON_ID_OK,
        BUTTON_ID_CANCEL,
        BUTTON_ID_SELECT,
        BUTTON_ID_START,
        BUTTON_ID_MENU,
        
        BUTTON_ID_MAX,
    };

    struct ButtonByLabelPacket
    {
        int32 m_iAnimButtonState;
        int32 m_iButtonID;
    };

public:
    static bool Initialize(void);
    static void Terminate(void);
    static void KeyLock(uint32 uKey);
    static void KeyUnlock(uint32 uKey);
    static BUTTON_ID GetPadToButtonID(uint32 uKey);
    
    CMenuController(void);
    ~CMenuController(void);
    void CheckButtonLabelList(Rt2dMaestro* pMaestro);
    void SetButtonState(Rt2dMaestro* pMaestro, CONTROLLER_ID iController, BUTTON_ID iButton, int32 iAnimButtonState);
    void Trigger(Rt2dMaestro* pMaestro);
    void KeyPress(Rt2dMaestro* pMaestro, uint32 uKey);
    void FlashUnlockKeyEnable(bool bEnable);

private:
    static Rt2dMaestro* AnimationsCallBack(Rt2dMaestro* maestro, Rt2dAnim* anim, Rt2dAnimProps* props, void* pData);

private:
    static const char* m_apszButtonLabel[];
    static uint32 m_auDigitalDataTable[];
    static bool m_abLockKeyData[];

    int32 m_aButtonLabelID[CONTROLLER_ID_MAX][BUTTON_ID_MAX];
    uint32 m_aControllerEnableBit[CONTROLLER_ID_MAX];
    bool m_bUnlock;
};