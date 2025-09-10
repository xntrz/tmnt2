#include "PadStream.hpp"
#include "Gamepad.hpp"

#include "System/Common/Controller.hpp"

#ifdef TARGET_PC
#include "System/PC/PCSpecific.hpp"
#endif


class CPadStream::CPadFileStream
{
public:
    friend CPadStream;
    
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
    uint8* m_pRecBuff;
    uint32 m_uRecBuffSize;
};


#ifdef TARGET_PC
class CPadStream::CPCPadFileStream final : public CPadStream::CPadFileStream
{
public:
    CPCPadFileStream(FILEMODE filemode, STAGEID::VALUE idStage, int32 nControllerNo);
    virtual ~CPCPadFileStream(void);
    virtual void GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize) override;
    virtual void SetPacked(int32 iController) override;
    virtual void GetPacked(void) override;
};
#else
#error Not implemented for current target
#endif



CPadStream::CPadFileStream::CPadFileStream(void)
: m_pPacked(nullptr)
, m_packed({ 0 })
, m_uDigital(0)
, m_uDigitalOld(0)
, m_uTrigger(0)
, m_iAnalogX(0)
, m_iAnalogY(0)
, m_uEndFrame(60 * 60)  // 60 seconds each for 60 frames
, m_uFrame(0)
, m_pFile(nullptr)
{
    ;
};


CPadStream::CPadFileStream::~CPadFileStream(void)
{
#ifdef _DEBUG    
    if (m_pRecBuff)
    {
        delete[] m_pRecBuff;
        m_pRecBuff = nullptr;
        m_uRecBuffSize = 0;
    };
#endif
    
    if (m_pFile)
    {
        RwFclose(m_pFile);
        m_pFile = nullptr;
    };
};


void CPadStream::CPadFileStream::GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize)
{
    ;
};


void CPadStream::CPadFileStream::SetPacked(int32 iController)
{
    ASSERT(m_pPacked);

#ifdef TARGET_PS2
    m_pPacked->m_uDigital = Convert(uint16(CController::GetDigital(iController)));
#elif TARGET_PC
    m_pPacked->m_uDigital = CController::GetDigital(iController);
#else
#error Not implemented for current target
#endif    
    m_pPacked->m_iAnalogX = CController::GetAnalog(iController, CController::ANALOG_LSTICK_X);
    m_pPacked->m_iAnalogY = CController::GetAnalog(iController, CController::ANALOG_LSTICK_Y);
};


void CPadStream::CPadFileStream::GetPacked(void)
{
    ASSERT(m_pPacked);

#ifdef TARGET_PS2
    m_uDigital = Convert(uint32(m_pPacked->m_uDigital));
#elif TARGET_PC
    m_uDigital = m_pPacked->m_uDigital;
#else
#error Not implemented for current target
#endif    
    m_uTrigger    = m_uDigital & ~(m_uDigitalOld);
    m_uDigitalOld = m_uDigital;
    m_iAnalogX    = m_pPacked->m_iAnalogX;
    m_iAnalogY    = m_pPacked->m_iAnalogY;
};


uint32 CPadStream::CPadFileStream::Convert(uint32 data)
{
    return data;
};


uint32 CPadStream::CPadFileStream::Convert(uint16 data)
{
    return IGamepad::ConvertToVirtualButton(data);
};


void CPadStream::CPadFileStream::Play(void)
{
    if (IsEnd() || RwFeof(m_pFile) == 1)
    {
        ClearPacked();
    }
    else
    {
        RwFread(&m_packed, sizeof(m_packed), 1, m_pFile);
        GetPacked();
        ++m_uFrame;
    };
};


void CPadStream::CPadFileStream::Record(int32 iController)
{
    if (IsEnd())
    {
        ClearPacked();
    }
    else
    {
        SetPacked(iController);
        RwFwrite(&m_packed, sizeof(m_packed), 1, m_pFile);
        GetPacked();
        ++m_uFrame;
    };
};


void CPadStream::CPadFileStream::GotoTop(void)
{
    m_pPacked = &m_packed;
    m_uFrame = 0;
    ClearPacked();
};


void CPadStream::CPadFileStream::ClearPacked(void)
{
    m_uDigital = 0;
    m_uDigitalOld = 0;
    m_uTrigger = 0;
    m_iAnalogX = 0;
    m_iAnalogY = 0;
};


bool CPadStream::CPadFileStream::IsEnd(void) const
{
    return (m_uFrame >= m_uEndFrame);
};


void CPadStream::CPadFileStream::AllocBuffer(uint32 uBuffSize)
{
#ifdef _DEBUG
    m_uRecBuffSize = uBuffSize;
    m_pRecBuff = new uint8[m_uRecBuffSize];
#endif    
};


#ifdef TARGET_PC

CPadStream::CPCPadFileStream::CPCPadFileStream(FILEMODE filemode, STAGEID::VALUE idStage, int32 nControllerNo)
{
    const char* pszFilemode = nullptr;
    
    char szFilePath[256];
    szFilePath[0] = '\0';

    switch (filemode)
    {
    case FILEMODE_RECORD:
        {
            char szDrive[MAX_PATH];
            szDrive[0] = '\0';

            char szDir[MAX_PATH];
            szDir[0] = '\0';

            GetModulePathSplit(szDrive, szDir, nullptr, nullptr);

            std::sprintf(szFilePath,
                         "%s%s..\\..\\..\\data\\Common\\Demo\\PC\\demo%03d%d.dat",
                         szDrive, szDir,
                         idStage, nControllerNo);

            pszFilemode = "wb";
        }
        break;

    case FILEMODE_PLAY:
        {
            std::sprintf(szFilePath, "Common/Demo/PC/demo%03d%d.dat", idStage, nControllerNo);
            pszFilemode = "rb";
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    m_pFile = RwFopen(szFilePath, pszFilemode);
    ASSERT(m_pFile);

    GotoTop();
};


CPadStream::CPCPadFileStream::~CPCPadFileStream(void)
{
    ;
};


void CPadStream::CPCPadFileStream::GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize)
{
    switch (mode)
    {
    case CPadStream::MODE_RECORD:
        {
            std::memcpy(m_pPacked, pInput, uInputSize);
            m_pPacked = reinterpret_cast<packed*>(reinterpret_cast<uint8*>(m_pPacked + uInputSize));
        }
        break;

    case CPadStream::MODE_PLAY:
        {
            ASSERT(m_pFile);
            RwFread(pInput, uInputSize, 1, m_pFile);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CPadStream::CPCPadFileStream::SetPacked(int32 iController)
{
    CPadFileStream::SetPacked(iController);
};


void CPadStream::CPCPadFileStream::GetPacked(void)
{
    CPadFileStream::GetPacked();
};

#endif /* TARGET_PC */


CPadStream::CPadStream(void)
: m_mode(MODE_NONE)
, m_pStream(nullptr)
{
    ;
};


CPadStream::~CPadStream(void)
{
    Close();
};


bool CPadStream::Open(MODE mode, STAGEID::VALUE idStage, int32 iController)
{
    m_mode = mode;
    
    CPadFileStream::FILEMODE filemode = (mode == MODE_RECORD) ? CPadFileStream::FILEMODE_RECORD :
                                                                CPadFileStream::FILEMODE_PLAY;

#ifdef TARGET_PC
    m_pStream = new CPCPadFileStream(filemode, idStage, iController);
#else
#error Not implemented for current target
#endif

    return (m_pStream != nullptr);
};


void CPadStream::Close(void)
{
    if (m_pStream)
    {
        delete m_pStream;
        m_pStream = nullptr;
    };

    m_mode = MODE_NONE;
};


void CPadStream::GetPadData(int32 iController)
{   
	if (m_pStream)
	{
		switch (m_mode)
		{
		case MODE_RECORD:
			m_pStream->Record(iController);
			break;

		case MODE_PLAY:
			m_pStream->Play();
			break;

		default:
			ASSERT(false);
			break;
		};

		if (m_pStream->IsEnd())
			CPadStreamSwitch::m_bEnd = true;
	};
};


int16 CPadStream::GetAnalogX(int32 iController)
{
    return (m_mode == MODE_PLAY ? m_pStream->m_iAnalogX : CController::GetAnalog(iController, CController::ANALOG_LSTICK_X));
};


int16 CPadStream::GetAnalogY(int32 iController)
{
    return (m_mode == MODE_PLAY ? m_pStream->m_iAnalogY : CController::GetAnalog(iController, CController::ANALOG_LSTICK_Y));
};


uint32 CPadStream::GetDigital(int32 iController)
{
    return (m_mode == MODE_PLAY ? m_pStream->m_uDigital : CController::GetDigital(iController));
};


uint32 CPadStream::GetDigitalTrigger(int32 iController)
{
    return (m_mode == MODE_PLAY ? m_pStream->m_uTrigger : CController::GetDigitalTrigger(iController));
};


void CPadStream::GetInput(void* pInput, uint32 uInputSize)
{
    if (m_mode != MODE_NONE)
        m_pStream->GetInput(m_mode, pInput, uInputSize);
};


/*static*/ bool CPadStreamSwitch::m_bEnd = false;
/*static*/ CPadStream::MODE CPadStreamSwitch::m_mode = CPadStream::MODE_NONE;