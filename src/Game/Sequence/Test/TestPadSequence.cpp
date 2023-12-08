#include "TestPadSequence.hpp"

#ifdef _DEBUG

#include "Game/System/Misc/Gamepad.hpp"


/*static*/ CProcess* CTestPadSequence::Instance(void)
{
    return new CTestPadSequence;
};


CTestPadSequence::CTestPadSequence(void)
: m_font()
{
    ;
};


CTestPadSequence::~CTestPadSequence(void)
{
    ;
};


bool CTestPadSequence::OnAttach(const void* pParam)
{
    return true;
};


void CTestPadSequence::OnDetach(void)
{
    ;
};


void CTestPadSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (IPad::GetDigital(IPad::CONTROLLER_LOCKED_ON_VIRTUAL, IPad::DIGITAL_START) &&
        IPad::GetDigital(IPad::CONTROLLER_LOCKED_ON_VIRTUAL, IPad::DIGITAL_SELECT))
        Ret();

    for (int32 i = 0; i < IPad::Max(); ++i)
    {
        if (IPad::GetDigital(i, IPad::DIGITAL_LLEFT) && IPad::GetDigital(i, IPad::DIGITAL_RDOWN))
            IPad::Lock(i);
        else if (IPad::GetDigital(i, IPad::DIGITAL_LRIGHT) && IPad::GetDigital(i, IPad::DIGITAL_RRIGHT))
            IPad::Unlock(i);
        else if (IPad::GetDigital(i, IPad::DIGITAL_RRIGHT))
            IPad::StartVibration(i, IPad::VIBRATIONTYPE_LOW, 1.0f);
        else if (IPad::GetDigital(i, IPad::DIGITAL_RUP))
            IPad::StartVibration(i, IPad::VIBRATIONTYPE_NORMAL, 1.0f);
        else if (IPad::GetDigital(i, IPad::DIGITAL_RLEFT))
            IPad::StartVibration(i, IPad::VIBRATIONTYPE_HARD, 1.0f);
    };
};


void CTestPadSequence::OnDraw(void) const
{
    m_font.Color({ 0xFF, 0xFF, 0xFF, 0xFF });
    m_font.Background({ 0x00, 0x00, 0x00, 0x00 });    
    m_font.Position(20, 20);
	m_font.SetAutoStep(0, 20);
    m_font.Print("PAD MAX NUMBER: %d", IPad::Max());
	m_font.Print("<LEFT>  + <A>         - PAD LOCK");
    m_font.Print("<RIGHT> + <B>         - PAD UNLOCK");
    m_font.Print("<START> + <SELECT>    - EXIT");
    m_font.Print("<B>                   - VIBRATION LOW");
    m_font.Print("<Y>                   - VIBRATION NORMAL");
    m_font.Print("<X>                   - VIBRATION HARD");

    for (int32 i = 0; i < IPad::Max(); ++i)
    {
        m_font.Position(i * 160, 150);

        m_font.SetAutoStep(0, 20);
        m_font.Print("NO:    %d", i);
        m_font.Print("PORT:  %d", IPad::GetPhysicalPort(i));
        m_font.Print("STATE: %s", IPad::GetState(i) == IPad::STATE_CONNECT ? "CONNECTED" : "UNCONNECTED");
        m_font.Print("STATUS:%s", IPad::IsLocked(i) ? "LOCKED" : "UNLOCKED");

        m_font.Print("\n");
        m_font.SetAutoStep(20, 0);
        
        for (int32 j = 0; j < IPad::DIGITAL_NUM / 2; ++j)
        {
			if ((j != 0) && ((j % 4) == 0))
			{
				m_font.SetAutoStep(0, 20);
				m_font.Print("\n");
				m_font.SetAutoStep(20, 0);
				m_font.PositionAdd(-80, 0);
			};

            if (IPad::GetDigitalTrigger(i, 1 << j))
                m_font.Print("T");
            else if (IPad::GetDigitalRelease(i, 1 << j))
                m_font.Print("R");
            else if (IPad::GetDigital(i, 1 << j))
                m_font.Print("O");
            else
                m_font.Print("X");
        };
    };
};

#endif /* _DEBUG */