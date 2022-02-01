#include "PhysicalController.hpp"


IPhysicalController::IPhysicalController(void)
{
	m_info = { 0 };
	m_abStickToDigital[CController::STICK_LEFT] = false;
	m_abStickToDigital[CController::STICK_RIGHT] = false;

	for (int32 i = 0; i < COUNT_OF(m_aAnalogMapping); ++i)
		m_aAnalogMapping[i] = i;	

	for (int32 i = 0; i < COUNT_OF(m_aDigitalMapping) / 2; ++i)
		m_aDigitalMapping[i] = 1 << i;
};


IPhysicalController::~IPhysicalController(void)
{
	;
};


void IPhysicalController::Update(void)
{
	for (int32 i = 0; i < COUNT_OF(m_abStickToDigital); ++i)
	{
		if (m_abStickToDigital[i])
			m_info.m_digital |= ConvertAnalogStickToDigital(CController::STICK(i), m_info.m_aAnalog[0], m_info.m_aAnalog[1]);
	};

	int16 aAnalog[CController::ANALOG_NUM] = { 0 };
	for (int32 i = 0; i < COUNT_OF(aAnalog); ++i)
		aAnalog[i] = m_info.m_aAnalog[m_aAnalogMapping[i]];

	for (int32 i = 0; i < COUNT_OF(m_info.m_aAnalog); ++i)
		m_info.m_aAnalog[i] = aAnalog[i];

	uint32 digital = 0;
	for (int32 i = 0; i < CController::DIGITAL_NUM; ++i)
	{
		if (m_info.m_digital & (1 << i))
			digital |= m_aDigitalMapping[i];
	};

	m_info.m_digital = digital;
	m_info.m_digitalTrigger = m_info.m_digital & ~m_info.m_digitalOld;
	m_info.m_digitalRelease = m_info.m_digitalOld & ~m_info.m_digital;
	m_info.m_digitalRepeat = 0;

	for (int32 i = 0; i < CController::DIGITAL_NUM; ++i)
	{
		static const int32 REPEAT_TIME_START	= 12;
		static const int32 REPEAT_DELAY_START = 6;
		static const int32 REPEAT_DELAY_MIN	= 2;

		if(m_info.m_digital & (1 << i))
		{
			if (m_info.m_aDigitalRepeatTime[i] == REPEAT_TIME_START)
				m_info.m_digitalRepeat |= (1 << i);
			
			if (m_info.m_aDigitalRepeatTime[i]-- == 1)
			{
				m_info.m_digitalRepeat |= (1 << i);
				m_info.m_aDigitalRepeatTime[i] = m_info.m_aDigitalRepeatDelay[i]--;
				if (m_info.m_aDigitalRepeatDelay[i] < REPEAT_DELAY_MIN)
					m_info.m_aDigitalRepeatDelay[i] = REPEAT_DELAY_MIN;
			};
		}
		else
		{
			m_info.m_digitalRepeat &= ~(1 << i);
			m_info.m_aDigitalRepeatTime[i] = REPEAT_TIME_START;
			m_info.m_aDigitalRepeatDelay[i] = REPEAT_DELAY_START;
		};
	};
};


void IPhysicalController::Clear(void)
{
	m_info.m_digitalOld = m_info.m_digital;
	m_info.m_digital = 0;
	m_info.m_digitalTrigger = 0;
	m_info.m_digitalRelease = 0;
	m_info.m_digitalRepeat = 0;

	for (int32 i = 0; i < COUNT_OF(m_info.m_aAnalog); ++i)
		m_info.m_aAnalog[i] = 0;
};


void IPhysicalController::SetVibration(uint32 type, int32 iVibFrame)
{
	ASSERT(iVibFrame >= 0);

	if (m_bVibrateEnable)
	{
		m_uVibType = type;
		m_iVibFrame = iVibFrame;
	}
	else
	{
		m_uVibType = 0;
		m_iVibFrame = 0;
	};
};


const IPhysicalController::INFO& IPhysicalController::Info(void) const
{
	return m_info;
};


void IPhysicalController::SetVibrationEnable(bool bEnable)
{
	m_bVibrateEnable = bEnable;
};


uint32* IPhysicalController::GetDigitalMapping(uint32 digital)
{
	int32 i = 0;
	while (!(digital & 1))
	{
		digital >>= 1;
		++i;
		ASSERT(i < CController::DIGITAL_NUM);
	};

	return &m_aDigitalMapping[i];
};


void IPhysicalController::EnableStickToDigitalMapping(CController::STICK stick, bool bEnable)
{
	ASSERT(stick >= 0 && stick < CController::STICK_NUM);
	m_abStickToDigital[stick] = bEnable;
};


/*static*/ uint32 IPhysicalController::ConvertAnalogStickToDigital(CController::STICK stick, int16 x, int16 y)
{
	static uint32 dirbtnRightStick[4] = 
	{ 
		CController::DIGITAL_A, 
		CController::DIGITAL_B, 
		CController::DIGITAL_X, 
		CController::DIGITAL_Y 
	};

	static uint32 dirbtnLeftStick[4] =
	{
		CController::DIGITAL_UP,
		CController::DIGITAL_DOWN,
		CController::DIGITAL_LEFT,
		CController::DIGITAL_RIGHT,
	};

	uint32* dirbtn = nullptr;

	if (stick == CController::STICK_RIGHT)
		dirbtn = dirbtnRightStick;
	else
		dirbtn = dirbtnLeftStick;

	if (x >= int16(0xC000) && x <= int16(0x4000) && 
		y >= int16(0xC000) && y <= int16(0x4000))
		return 0;

	if (y <= x)
	{
		if (y <= -x)
			return dirbtn[1];
		else
			return dirbtn[3];
	}
	else if (y <= -x)
	{
		return dirbtn[2];
	}
	else
	{
		return dirbtn[0];
	};
};