#pragma once

#include "PlayerTypes.hpp"


class CStatus;
class CPlayerCharacter;


class CPlayerStateMachine final
{
public:
    CPlayerStateMachine(CPlayerCharacter* pPlayerChr, int32 nNumStatus);
    ~CPlayerStateMachine(void);
    void Run(void);
    void RegistStatus(PLAYERTYPES::STATUS status, CStatus* pStatus);
    void ChangeStatus(PLAYERTYPES::STATUS status);
    PLAYERTYPES::STATUS CurrentStatus(void) const;
    PLAYERTYPES::STATUS PrevStatus(void) const;
    bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) const;
    float GetStatusDuration(void) const;
    
private:
    CStatus** m_apStatus;
    CStatus* m_pCurrentStatus;
    float m_fElapsedTimeOfStatus;
    int32 m_iStatusMax;
    PLAYERTYPES::STATUS m_statusBefore;
    PLAYERTYPES::STATUS m_statusNow;
    CPlayerCharacter* m_pPlayerChr;
};