#include "PadStream.hpp"
#include "Gamepad.hpp"

#include "System/Common/Controller.hpp"
#include "System/PC/PCSpecific.hpp"


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


bool CPadStream::Open(MODE mode, STAGEID::VALUE idStage, int32 nPadNo)
{
	m_mode = mode;
#ifdef _TARGET_PC
    switch (mode)
    {
    case MODE_RECORD:
        m_pStream = new CPCPadFileStream(CPadFileStream::FILEMODE_RECORD, idStage, nPadNo);
        break;

    case MODE_PLAY:
        m_pStream = new CPCPadFileStream(CPadFileStream::FILEMODE_PLAY, idStage, nPadNo);
        break;

    default:
        ASSERT(false);
        break;
    };
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


void CPadStream::GetPad(int32 iController)
{
    switch (m_mode)
    {
    case MODE_RECORD:
        {
            ASSERT(m_pStream);
            
            m_pStream->Record(iController);
            if (m_pStream->IsEnd())
            {
                CPadStreamSwitch::m_bEnd = true;
            };
        };
        break;

    case MODE_PLAY:
        {
            ASSERT(m_pStream);

            m_pStream->Play();
            if (m_pStream->IsEnd())
            {
                CPadStreamSwitch::m_bEnd = true;
            };
        };
        break;
    };
};


int16 CPadStream::GetAnalogX(int32 iController)
{
    if (m_mode == MODE_PLAY)
        return m_pStream->m_iAnalogX;
    else
        return CController::GetAnalog(iController, CController::ANALOG_LSTICK_X);
};


int16 CPadStream::GetAnalogY(int32 iController)
{
    if (m_mode == MODE_PLAY)
        return m_pStream->m_iAnalogY;
    else
        return CController::GetAnalog(iController, CController::ANALOG_LSTICK_Y);
};


uint32 CPadStream::GetDigital(int32 iController)
{
    if (m_mode == MODE_PLAY)
        return m_pStream->m_uDigital;
    else
        return CController::GetDigital(iController);
};


uint32 CPadStream::GetDigitalTrigger(int32 iController)
{
    if (m_mode == MODE_PLAY)
        return m_pStream->m_uTrigger;
    else
        return CController::GetDigitalTrigger(iController);
};


void CPadStream::GetInput(void* pInput, uint32 uInputSize)
{
    if (m_mode != MODE_NONE)
        m_pStream->GetInput(m_mode, pInput, uInputSize);
};


/*static*/ bool CPadStreamSwitch::m_bEnd = false;
/*static*/ CPadStream::MODE CPadStreamSwitch::m_mode = CPadStream::MODE_NONE;


CPadFileStream::CPadFileStream(void)
: m_pPacked(nullptr)
, m_packed({ 0 })
, m_uDigital(0)
, m_uDigitalOld(0)
, m_uTrigger(0)
, m_iAnalogX(0)
, m_iAnalogY(0)
, m_uEndFrame(3600)
, m_uFrame(0)
, m_pFile(nullptr)
{
    ;
};


CPadFileStream::~CPadFileStream(void)
{
    if (m_pFile)
    {
        RwFclose(m_pFile);
        m_pFile = nullptr;
    };
};


void CPadFileStream::GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize)
{
    ;
};


void CPadFileStream::SetPacked(int32 iController)
{
    ASSERT(m_pPacked);

    m_pPacked->m_uDigital = CController::GetDigital(iController);
    m_pPacked->m_iAnalogX = CController::GetAnalog(iController, CController::ANALOG_LSTICK_X);
    m_pPacked->m_iAnalogY = CController::GetAnalog(iController, CController::ANALOG_LSTICK_Y);
};


void CPadFileStream::GetPacked(void)
{
    ASSERT(m_pPacked);

    m_uDigital = m_pPacked->m_uDigital;
    m_uTrigger = m_uDigital & ~(m_uDigitalOld);
    m_uDigitalOld = m_uDigital;
    m_iAnalogX = m_pPacked->m_iAnalogX;
    m_iAnalogY = m_pPacked->m_iAnalogY;
};


uint32 CPadFileStream::Convert(uint32 data)
{
    return data;
};


uint32 CPadFileStream::Convert(uint16 data)
{
    return CGamepad::ConvertToVirtualButton(data);
};


void CPadFileStream::Play(void)
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


void CPadFileStream::Record(int32 iController)
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


void CPadFileStream::GotoTop(void)
{
    m_pPacked = &m_packed;
    m_uFrame = 0;
    ClearPacked();
};


void CPadFileStream::ClearPacked(void)
{
    m_uDigital = 0;
    m_uDigitalOld = 0;
    m_uTrigger = 0;
    m_iAnalogX = 0;
    m_iAnalogY = 0;
};


bool CPadFileStream::IsEnd(void) const
{
    return (m_uFrame >= m_uEndFrame);
};


void CPadFileStream::AllocBuffer(uint32 uBuffSize)
{
    ;    
};


CPCPadFileStream::CPCPadFileStream(FILEMODE filemode, STAGEID::VALUE idStage, int32 nControllerNo)
{
    const char* pszFilemode = nullptr;
    char szFilePath[256];
    szFilePath[0] = '\0';

    switch (filemode)
    {
    case FILEMODE_RECORD:
        {
            char szDrive[256];
            char szDir[256];
            char szModulePath[256];

            szDrive[0] = '\0';
            szDir[0] = '\0';
            szModulePath[0] = '\0';

            CPCSpecific::GetModulePath(szModulePath, sizeof(szModulePath));
            _splitpath(szModulePath, szDrive, szDrive, nullptr, nullptr);
            sprintf(
                szFilePath,
                "%s%s..\\..\\..\\data\\Common\\Demo\\PC\\demo%03d%d.dat",
                szDrive,
                szDir,
                idStage,
                nControllerNo
            );
        }
        break;

    case FILEMODE_PLAY:
        {
            sprintf(szFilePath, "Common/Demo/PC/demo%03d%d.dat", idStage, nControllerNo);
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


CPCPadFileStream::~CPCPadFileStream(void)
{
    ;
};


void CPCPadFileStream::GetInput(CPadStream::MODE mode, void* pInput, uint32 uInputSize)
{
    switch (mode)
    {
    case CPadStream::MODE_RECORD:
        std::memcpy(m_pPacked, pInput, uInputSize);
        m_pPacked = (packed*)((uint8*)m_pPacked + uInputSize);
        break;

    case CPadStream::MODE_PLAY:
        ASSERT(m_pFile);
        RwFread(pInput, uInputSize, 1, m_pFile);
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CPCPadFileStream::SetPacked(int32 iController)
{
    CPadFileStream::SetPacked(iController);
};


void CPCPadFileStream::GetPacked(void)
{
    CPadFileStream::GetPacked();
};