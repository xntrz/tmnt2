#pragma once

#include "Game/Component/GameMain/StageID.hpp"


class CPadFileStream;


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

public:
    CPadStream(void);
    ~CPadStream(void);
    bool Open(MODE mode, STAGEID::VALUE idStage, int32 nPadNo);
    void Close(void);
    void GetPad(int32 iController);
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


class CPadFileStream
{
private:
    friend CPadStream;

public:
    enum FILEMODE
    {
        FILEMODE_RECORD = 0,
        FILEMODE_PLAY,

        FILEMODEMAX,
    };

    struct packed
    {
        uint32 m_uDigital;
        int16 m_iAnalogX;
        int16 m_iAnalogY;
    };

public:
    CPadFileStream(void);
    virtual ~CPadFileStream(void);
    virtual void GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize);
    virtual void SetPacked(int32 iController);
    virtual void GetPacked(void);
    uint32 Convert(uint32 data);
    uint32 Convert(uint16 data);
    void Play(void);
    void Record(int32 iController);
    void GotoTop(void);
    void ClearPacked(void);
    bool IsEnd(void) const;
    void AllocBuffer(uint32 uBuffSize);

protected:
    packed* m_pPacked;
    packed m_packed;
    uint32 m_uDigital;
    uint32 m_uDigitalOld;
    uint32 m_uTrigger;
    int16 m_iAnalogX;
    int16 m_iAnalogY;
    uint32 m_uEndFrame;
    uint32 m_uFrame;
    void* m_pFile;
};


class CPCPadFileStream : public CPadFileStream
{
public:
    CPCPadFileStream(FILEMODE filemode, STAGEID::VALUE idStage, int32 nControllerNo);
    virtual ~CPCPadFileStream(void);
    virtual void GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize) override;
    virtual void SetPacked(int32 iController) override;
    virtual void GetPacked(void) override;
};