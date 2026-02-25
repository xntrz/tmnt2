#include "AmbientLightModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Model/Model.hpp"


CAmbientLightModule::CAmbientLightModule(CCharacter* pCharacter)
: IModule(MODULETYPE::AMBIENT_LIGHT)
, m_fElapsedTime(0.0f)
, m_ColorTo({ 0.0f, 0.0f, 0.0f, 0.0f })
, m_ColorFrom({ 0.0f, 0.0f, 0.0f, 0.0f })
, m_pCharacter(pCharacter)
, m_bEnable(true)
{
    ASSERT(m_pCharacter);

    CModel* pModel = m_pCharacter->GetModel();
	ASSERT(pModel);
	
	RwRGBA colorModel = pModel->GetColor();
    
    RwRGBARealFromRwRGBA(&m_ColorTo, &colorModel);
    m_ColorFrom = m_ColorTo;
};


void CAmbientLightModule::Run(void)
{
	m_fElapsedTime += CGameProperty::GetElapsedTime();

	RwRGBA colorGround = m_pCharacter->GetCollisionGround()->m_Color;
	RwRGBA colorModel = m_pCharacter->GetModel()->GetColor();

	RwRGBAReal colorGroundReal;
	RwRGBARealFromRwRGBA(&colorGroundReal, &colorGround);

	RwRGBAReal colorModelReal;
	RwRGBARealFromRwRGBA(&colorModelReal, &colorModel);

	if ((colorGroundReal.red   != m_ColorTo.red)    &&
		(colorGroundReal.green != m_ColorTo.green) 	&&
		(colorGroundReal.blue  != m_ColorTo.blue))
	{
		m_ColorFrom = colorModelReal;
		m_ColorTo   = colorGroundReal;
		m_fElapsedTime = 0.0f;
	};

	if (m_bEnable)
	{
		RwRGBA colorModelNew = { 0 };
		float t = m_fElapsedTime;

		if (t == 0.0f)
		{
			RwRGBAFromRwRGBAReal(&colorModelNew, &m_ColorFrom);
		}
		else if (t < 0.5)
		{
			RwRGBAReal color;
			color.red   = ((m_ColorTo.red   - m_ColorFrom.red)   * (t / 0.5f)) + m_ColorFrom.red;
			color.green = ((m_ColorTo.green - m_ColorFrom.green) * (t / 0.5f)) + m_ColorFrom.green;
			color.blue  = ((m_ColorTo.blue  - m_ColorFrom.blue)  * (t / 0.5f)) + m_ColorFrom.blue;
			color.alpha = 0;

			RwRGBAFromRwRGBAReal(&colorModelNew, &color);
		}
		else
		{
			RwRGBAFromRwRGBAReal(&colorModelNew, &m_ColorTo);
		};

		colorModelNew.alpha = 0xFF;

		m_pCharacter->GetModel()->SetColor(colorModelNew);
	};
};