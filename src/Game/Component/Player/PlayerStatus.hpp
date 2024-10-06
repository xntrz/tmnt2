#pragma once

#include "PlayerTypes.hpp"


class CPlayerStateMachine;
class CPlayerCharacter;


class CStatus
{
private:    
    friend CPlayerStateMachine;
    
public:
    static bool IsWithinStatusFromArray(PLAYERTYPES::STATUS status, const PLAYERTYPES::STATUS* pStatusArray, int32 nArraySize);
    static void RegistDefaultForStateMachine(CPlayerStateMachine& statemachine);
    
    inline CStatus(void) {};
    virtual ~CStatus(void) {};
    virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) = 0;
    virtual void OnAttach(void) = 0;
    virtual void OnDetach(void) = 0;
    virtual void OnRun(void) = 0;

    
    inline CPlayerCharacter& Character(void)
    {
        ASSERT(m_pPlayerChr);
        return *m_pPlayerChr;
    };


    inline CPlayerStateMachine& StateMachine(void)
    {
        ASSERT(m_pStateMachine);
        return *m_pStateMachine;
    };

    
    inline PLAYERTYPES::STATUS GetID(void) const
    {
        return m_status;
    };

protected:
    CPlayerCharacter* m_pPlayerChr;
    CPlayerStateMachine* m_pStateMachine;
    PLAYERTYPES::STATUS m_status;
};