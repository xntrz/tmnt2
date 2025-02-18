#pragma once

#include "Game/Component/GameMain/StageID.hpp"


class CPadStream
{
public:
    enum MODE
    {
        MODE_NONE = 0,
        MODE_RECORD,
        MODE_PLAY,

        MODEMAX,
    };
    
    class CPadFileStream;
    class CPCPadFileStream;

public:
    CPadStream(void);
    ~CPadStream(void);
    bool Open(MODE mode, STAGEID::VALUE idStage, int32 iController);
    void Close(void);
    void GetPadData(int32 iController);
    int16 GetAnalogX(int32 iController);
    int16 GetAnalogY(int32 iController);
    uint32 GetDigital(int32 iController);
    uint32 GetDigitalTrigger(int32 iController);
    void GetInput(void* pInput, uint32 uInputSize);

private:
    MODE m_mode;
    CPadFileStream* m_pStream;
};


class CPadStreamSwitch
{
public:
    static bool m_bEnd;
    static CPadStream::MODE m_mode;
};