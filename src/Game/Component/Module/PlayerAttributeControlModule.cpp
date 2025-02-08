#include "PlayerAttributeControlModule.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Model/Model.hpp"


CPlayerAttributeControlModule::CPlayerAttributeControlModule(MODULETYPE::VALUE type, PLAYERTYPES::ATTRIBUTE attribute, CPlayerCharacter* pPlayerChr)
: IModule(type)
, m_attribute(attribute)
, m_pPlayerChr(pPlayerChr)
, m_fElapsedTime(0.0f)
, m_fEndTime(0.0f)
, m_bEnable(false)
{
    ASSERT(m_pPlayerChr);
};


void CPlayerAttributeControlModule::Run(void)
{
    if (!m_bEnable)
        return;

    m_fElapsedTime += CGameProperty::GetElapsedTime();
    if (m_fElapsedTime >= m_fEndTime)
        Disable();
};


void CPlayerAttributeControlModule::Enable(float fEndTime)
{
    ASSERT(fEndTime > 0.0f);

    if (GetRemainTime() <= fEndTime)
    {
        m_pPlayerChr->SetAttribute(m_attribute);

        m_bEnable       = true;
        m_fEndTime      = fEndTime;
        m_fElapsedTime  = 0.0f;
    };
};


void CPlayerAttributeControlModule::Disable(void)
{
    if (m_bEnable)
    {
        m_pPlayerChr->ClearAttribute(m_attribute);

        m_bEnable       = false;
        m_fEndTime      = 0.0f;
        m_fElapsedTime  = 0.0f;
    };
};


void CPlayerAttributeControlModule::SetRemainTime(float fTime)
{
    if (fTime > 0.0f)
        Enable(fTime);
    else
        Disable();
};


float CPlayerAttributeControlModule::GetRemainTime(void) const
{
    return (m_bEnable ? (m_fEndTime - m_fElapsedTime) : 0.0f);    
};


//
// *********************************************************************************
//


/*static*/ const RwRGBA CConfusionAttributeControlModule::CONFUSION_COLOR = { 0xFF, 0x99, 0x00, 0xFF };


CConfusionAttributeControlModule::CConfusionAttributeControlModule(MODULETYPE::VALUE type, CPlayerCharacter* pPlayerChr)
: CPlayerAttributeControlModule(type, PLAYERTYPES::ATTRIBUTE_CONFUSION, pPlayerChr)
{
    ;
};


void CConfusionAttributeControlModule::Run(void)
{
    CPlayerAttributeControlModule::Run();
};


void CConfusionAttributeControlModule::Enable(float fEndTime)
{
    CPlayerAttributeControlModule::Enable(fEndTime);
    m_pPlayerChr->SetAmbientLightEnable(false);

    CModel* pModel = m_pPlayerChr->GetModel();
    ASSERT(pModel);

    pModel->SetColor(CONFUSION_COLOR);
};


void CConfusionAttributeControlModule::Disable(void)
{
    bool bWasEnabled = m_bEnable;

    CPlayerAttributeControlModule::Disable();

    if (!bWasEnabled)
        return;
    
    m_pPlayerChr->SetAmbientLightEnable(true);

    RwV3d vOffset = { 0.0f, 1.5f, 0.0f };

    RwV3d vPos = Math::VECTOR3_ZERO;
    m_pPlayerChr->GetPosition(&vPos);

    Math::Vec3_Add(&vPos, &vPos, &vOffset);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_WAKE, &vPos);
    ASSERT(hEffect);
};