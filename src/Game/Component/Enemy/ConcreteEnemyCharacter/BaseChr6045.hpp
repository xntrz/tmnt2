#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"


class CBaseAI6045;


class CBaseChr6045 : public CEnemyCharacter
{
public:
    class CJumpActionStatusObserver : public CStatusObserver
    {
    protected:
        enum STEP
        {
            STEP_JUMP_READY = 0,
            STEP_JUMP,
            STEP_TOUCHDOWN,
        };

    public:
        CJumpActionStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void SetJumpSetting(void);
        virtual void SetTouchdownSetting(void);
        void SetParameter(const char* pszMotionJumpReady,
                          const char* pszMotionJump,
                          const char* pszMotionTouchdown,
                          float fJumpInitSpeed);

    protected:        
        int32 m_step;
        float m_fJumpInitSpeed;
        const char* m_pszMotionJumpReady;
        const char* m_pszMotionJump;
        const char* m_pszMotionTouchdown;
    };
    
    class CKnockForGuardPriorityStatusObserver : public CCommonEnemyObserver::CKnockStatus
    {
    public:
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CGuardStatusObserver : public CStatusObserver
    {
    protected:
        enum STEP
        {
            STEP_GUARD_BEGIN = 0,
            STEP_GUARD,
            STEP_GUARD_END,
        };

    public:
        CGuardStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual RESULT PassageGuardTotalTime(void);
        void SetParameter(float fGuardTime, float fGuardTotalTime);
        
    protected:
        int32 m_step;
        float m_fGuardTime;
        float m_fGuardTotalTime;
        float m_fTimer;
        float m_fTotalTimer;
        bool m_bCounterActionFlag;
    };

    class COneShotMotionForAttackPlayerAim : public CStatusObserver
    {
    public:
        COneShotMotionForAttackPlayerAim(const char* pszMotion);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        const char* m_pszMotion;
        bool m_bMotionEnd;
        int32 m_orderTargetNo;
    };

    class COneShotMotionForAttack : public CStatusObserver
    {
    public:
        COneShotMotionForAttack(const char* pszMotion);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        const char* m_pszMotion;
        bool m_bMotionEnd;
    };

    class CDeathStatusObserver : public CStatusObserver
    {
    public:
        enum DEATHKIND
        {
            DEATHKIND_NONE = 0,
            DEATHKIND_NORMAL,
            DEATHKIND_MACHINE,
            DEATHKIND_MONSTER,
            DEATHKIND_MOTION,
            DEATHKIND_NEXUS,
        };

    public:
        CDeathStatusObserver(void);
        CDeathStatusObserver(const char* pszMotion, DEATHKIND deathKind, float fEffectScale);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void EntryDeathEffect(void);
        void SetParameter(DEATHKIND deathKind, const char* pszMotion, float fEffectScale);

    private:
        const char* m_pszMotion;
        DEATHKIND m_deathKind;
        float m_fEffectScale;
    };


    class CNothingStatusObserver : public CStatusObserver
    {
    public:
        CNothingStatusObserver(const char* pszMotion, ENEMYTYPES::STATUS nextStatus);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        const char* m_pszMotion;
        ENEMYTYPES::STATUS m_nextStatus;
    };

public:
    CBaseChr6045(ENEMYID::VALUE enemyId);
    virtual ~CBaseChr6045(void);
    virtual bool Initialize(PARAMETER* pParameter, bool bReplaceParameter) override;
    virtual void Run(void) override;
    virtual void OnMessageAttackResult(CHitCatchData* pCatch) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    CBaseAI6045& AIModerator(void);
    void SetKnockBackCtrlEnable(bool bEnable);
    bool IsKnockBackCtrlActive(void) const;

protected:
    CCommonEnemyObserver::CBaseThrow* m_pThrowObserver;
    CEnemyUtils::CKnockBackControl m_knockBackCtrl;
    bool m_bKnockBackCtrlActive;
    CBaseAI6045* m_pAIModerator;
}; 