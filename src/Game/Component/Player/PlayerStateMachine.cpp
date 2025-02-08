#include "PlayerStateMachine.hpp"
#include "PlayerStatus.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"


CPlayerStateMachine::CPlayerStateMachine(CPlayerCharacter* pPlayerChr, int32 nNumStatus)
: m_apStatus(nullptr)
, m_pCurrentStatus(nullptr)
, m_fElapsedTimeOfStatus(0.0f)
, m_iStatusMax(nNumStatus)
, m_statusBefore(PLAYERTYPES::STATUS_INVALID)
, m_statusNow(PLAYERTYPES::STATUS_INVALID)
, m_pPlayerChr(pPlayerChr)
{
    ASSERT(m_iStatusMax > 0);

    m_apStatus = new CStatus*[m_iStatusMax];
    
	for (int32 i = 0; i < m_iStatusMax; ++i)
		m_apStatus[i] = nullptr;
};


CPlayerStateMachine::~CPlayerStateMachine(void)
{
    if (m_apStatus)
    {
        for (int32 i = 0; i < m_iStatusMax; ++i)
        {
            if (m_apStatus[i])
            {
                delete m_apStatus[i];
                m_apStatus[i] = nullptr;
            };
        };

        delete[] m_apStatus;
        m_apStatus = nullptr;
        
        m_iStatusMax = 0;
    };
};


void CPlayerStateMachine::Run(void)
{
    m_fElapsedTimeOfStatus += CGameProperty::GetElapsedTime();
    
    if (m_pCurrentStatus)
        m_pCurrentStatus->OnRun();
};


void CPlayerStateMachine::RegistStatus(PLAYERTYPES::STATUS status, CStatus* pStatus)
{
    ASSERT(pStatus);
    ASSERT(status >= 0);
    ASSERT(status < m_iStatusMax);

    if (m_apStatus[status])
    {
        delete m_apStatus[status];
        m_apStatus[status] = nullptr;
    };

    m_apStatus[status] = pStatus;
    m_apStatus[status]->m_pStateMachine = this;
    m_apStatus[status]->m_pPlayerChr    = m_pPlayerChr;
    m_apStatus[status]->m_status        = status;
};


void CPlayerStateMachine::ChangeStatus(PLAYERTYPES::STATUS status)
{
    if (!IsEnableChangeStatus(status))
        return;

    if (m_pCurrentStatus)
    {
        m_statusBefore = m_pCurrentStatus->m_status;
        m_pCurrentStatus->OnDetach();
    };

    if (m_apStatus[status])
    {
        m_pCurrentStatus = m_apStatus[status];
        m_statusNow      = m_pCurrentStatus->m_status;

        m_pCurrentStatus->OnAttach();

        m_fElapsedTimeOfStatus = 0.0f;
    };
};


PLAYERTYPES::STATUS CPlayerStateMachine::CurrentStatus(void) const
{
    return m_statusNow;    
};


PLAYERTYPES::STATUS CPlayerStateMachine::PrevStatus(void) const
{
    return m_statusBefore;
};


bool CPlayerStateMachine::IsEnableChangeStatus(PLAYERTYPES::STATUS status) const
{
    ASSERT(status >= 0);
    ASSERT(status < m_iStatusMax);

    if (!m_pCurrentStatus)
        return true;

    CStatus* pAfterStatus = m_apStatus[status];
    ASSERT(pAfterStatus);
    
    return pAfterStatus->IsEnableChangeStatus(m_pCurrentStatus->m_status);
};


float CPlayerStateMachine::GetStatusDuration(void) const
{
    return m_fElapsedTimeOfStatus;
};