#include "PlayerAttributeControlModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


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


CPlayerAttributeControlModule::~CPlayerAttributeControlModule(void)
{
    ;
};


void CPlayerAttributeControlModule::Run(void)
{
    if (!m_bEnable)
        return;

    m_fElapsedTime += CGameProperty::GetElapsedTime();
    if (m_fElapsedTime >= m_fEndTime)
        Disable();
};


void CPlayerAttributeControlModule::Draw(void)
{
    ;
};


void CPlayerAttributeControlModule::Enable(float fEndTime)
{
    ASSERT(fEndTime > 0.0f);

    if (GetRemainTime() <= fEndTime)
    {
        m_pPlayerChr->SetAttributeFlag(m_attribute, true);
        m_bEnable = true;
        m_fEndTime = fEndTime;
        m_fElapsedTime = 0.0f;
    };
};


void CPlayerAttributeControlModule::Disable(void)
{
    if (m_bEnable)
    {
        m_pPlayerChr->SetAttributeFlag(m_attribute, false);
        m_bEnable = false;
        m_fEndTime = 0.0f;
        m_fElapsedTime = 0.0f;
    };
};


void CPlayerAttributeControlModule::SetRemainTime(float fTime)
{
    if (fTime <= 0.0f)
    {
        Disable();
    }
    else
    {
        m_bEnable = true;
        m_fEndTime = fTime;
        m_fElapsedTime = 0.0f;
    };
};


float CPlayerAttributeControlModule::GetRemainTime(void) const
{
    if (m_bEnable)
        return m_fEndTime - m_fElapsedTime;
    else
        return 0.0f;
};