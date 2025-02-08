#pragma once

#include "PlayerTypes.hpp"


#define DEFINE_ENABLED_STATUS_FOR(T, ...)                                               \
    bool T::IsEnableChangeStatus(PLAYERTYPES::STATUS status)                            \
    {                                                                                   \
        PLAYERTYPES::STATUS aStatusArray[] = __VA_ARGS__;                               \
        return IsWithinStatusFromArray(status, aStatusArray, COUNT_OF(aStatusArray));   \
    }


#define DEFINE_EMPTY_ENABLED_STATUS_FOR(T, ret)                                         \
    bool T::IsEnableChangeStatus(PLAYERTYPES::STATUS status)                            \
    {                                                                                   \
        return ret;                                                                     \
    }


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
    
    /*inline*/ CPlayerCharacter& Character(void);
    /*inline*/ const CPlayerCharacter& Character(void) const;    
    /*inline*/ CPlayerStateMachine& StateMachine(void);
    /*inline*/ const CPlayerStateMachine& StateMachine(void) const;    
    /*inline*/ PLAYERTYPES::STATUS GetID(void) const;

protected:
    CPlayerCharacter*    m_pPlayerChr;
    CPlayerStateMachine* m_pStateMachine;
    PLAYERTYPES::STATUS  m_status;
};


inline CPlayerCharacter& CStatus::Character(void)
{
    ASSERT(m_pPlayerChr);
    return *m_pPlayerChr;
};


inline const CPlayerCharacter& CStatus::Character(void) const
{
    ASSERT(m_pPlayerChr);
    return *m_pPlayerChr;
};


inline CPlayerStateMachine& CStatus::StateMachine(void)
{
    ASSERT(m_pStateMachine);
    return *m_pStateMachine;
};


inline const CPlayerStateMachine& CStatus::StateMachine(void) const
{
    ASSERT(m_pStateMachine);
    return *m_pStateMachine;
};


inline PLAYERTYPES::STATUS CStatus::GetID(void) const
{
    return m_status;
};
