#include "AmbientLightModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Model/Model.hpp"


CAmbientLightModule::CAmbientLightModule(CCharacter* pCharacter)
: IModule(MODULETYPE::AMBIENT_LIGHT)
, m_fElapsedTime(0.0f)
, m_ColorTo({ 0.0f,0.0f,0.0f, 0.0f })
, m_ColorFrom({ 0.0f,0.0f,0.0f, 0.0f })
, m_pCharacter(pCharacter)
, m_bEnable(true)
{
    ASSERT(m_pCharacter);

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);
    RwRGBA ColorMat = pModel->GetColor();
    
    RwRGBARealFromRwRGBA(&m_ColorTo, &ColorMat);
    m_ColorFrom = m_ColorTo;
};


CAmbientLightModule::~CAmbientLightModule(void)
{
    ;
};


void CAmbientLightModule::Run(void)
{
    if (!m_bEnable)
        return;
    
    m_fElapsedTime += CGameProperty::GetElapsedTime();

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);
    
    const CCharacter::COLLISIONGROUNDINFO* pGroundInfo = m_pCharacter->GetCollisionGround();
    ASSERT(pGroundInfo);

    RwRGBA ColorModel = pModel->GetColor();
    RwRGBA ColorGnd = pGroundInfo->m_Color;
    RwRGBAReal ColorGndReal = { 0.0f, 0.0f, 0.0f, 0.0f };

    RwRGBARealFromRwRGBAMacro(&ColorGndReal, &ColorGnd);

    if (!Math::FEqual(ColorGndReal.red, m_ColorTo.red)      ||
        !Math::FEqual(ColorGndReal.green, m_ColorTo.green)  ||
        !Math::FEqual(ColorGndReal.blue, m_ColorTo.blue))
    {
        RwRGBARealFromRwRGBAMacro(&m_ColorFrom, &ColorModel);
        
        m_ColorTo = ColorGndReal;
        m_fElapsedTime = 0.0f;
    };

    if (Math::FEqual(m_fElapsedTime, 0.0f))
    {
        RwRGBAFromRwRGBARealMacro(&ColorModel, &m_ColorFrom);
    }
    else if (m_fElapsedTime < 0.5f)
    {
        RwRGBAReal Delta = { 0.0f, 0.0f, 0.0f, 0.0f };
        Delta.red   = m_ColorFrom.red   + ((m_ColorTo.red   - m_ColorFrom.red)  * (m_fElapsedTime / 0.5f));
        Delta.green = m_ColorFrom.green + ((m_ColorTo.green - m_ColorFrom.green)* (m_fElapsedTime / 0.5f));
        Delta.blue  = m_ColorFrom.blue  + ((m_ColorTo.blue  - m_ColorFrom.blue) * (m_fElapsedTime / 0.5f));

        RwRGBAFromRwRGBARealMacro(&ColorModel, &Delta);
    }
    else
    {
        RwRGBAFromRwRGBARealMacro(&ColorModel, &m_ColorTo);
    };

    ColorModel.alpha = 0xFF;
    pModel->SetColor(ColorModel);
};


void CAmbientLightModule::Draw(void)
{
    ;
};


void CAmbientLightModule::SetEnable(bool bEnable)
{
    m_bEnable = bEnable;
};


bool CAmbientLightModule::IsEnable(void) const
{
    return m_bEnable;
};