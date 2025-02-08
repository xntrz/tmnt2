#include "BlinkCharacterModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Model/Model.hpp"


static const float BLINKINTERVAL = 0.05f;


CBlinkCharacterModule::CBlinkCharacterModule(CPlayerCharacter* pPlayerCharacter)
: IModule(MODULETYPE::BLINK_CHARACTER)
, m_pPlayerChr(pPlayerCharacter)
, m_pModel(nullptr)
, m_fElapsedTime(0.0f)
, m_fNextIntervalTime(0.0f)
, m_fEndTime(0.0f)
, m_bBlinking(false)
{
    ASSERT(m_pPlayerChr);
    
    m_pModel = m_pPlayerChr->GetModel();
    ASSERT(m_pModel);
};


CBlinkCharacterModule::~CBlinkCharacterModule(void)
{
    ;
};


void CBlinkCharacterModule::Run(void)
{
    if (!m_bBlinking)
        return;

    m_fElapsedTime += CGameProperty::GetElapsedTime();
    if (m_fElapsedTime < m_fEndTime)
    {
        if (m_fElapsedTime >= m_fNextIntervalTime)
        {
            m_pModel->SetDrawEnable(!m_pModel->IsDrawEnable());
            m_fNextIntervalTime += BLINKINTERVAL;
        };
    }
    else
    {
        End();
    };
};


void CBlinkCharacterModule::Draw(void)
{
    ;
};


void CBlinkCharacterModule::Start(float fEndTime)
{
    ASSERT(fEndTime > 0.0f);

    float fRemainTime = GetRemainTime();
    if (fRemainTime <= fEndTime)
    {
        m_fEndTime          = fEndTime;
        m_fElapsedTime      = 0.0f;
        m_fNextIntervalTime = BLINKINTERVAL;
        m_bBlinking         = true;
    };
};


void CBlinkCharacterModule::End(void)
{
    m_bBlinking = false;
    m_pModel->SetDrawEnable(true);
};


void CBlinkCharacterModule::SetRemainTime(float fTime)
{
    if (fTime <= 0.0f)
    {
        End();
    }
    else
    {
        m_fEndTime = fTime;
        m_fElapsedTime = 0.0f;
        m_fNextIntervalTime = BLINKINTERVAL;
        m_bBlinking = true;
    };
};


float CBlinkCharacterModule::GetRemainTime(void) const
{
    if (m_bBlinking)
        return m_fEndTime - m_fElapsedTime;
    
    return 0.0f;
};