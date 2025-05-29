#pragma once

#include "BaseEliteChr.hpp"


class CBaseEliteFootChr : public CBaseEliteChr
{
public:
    enum STATUS
    {
        STATUS_BUNSHIN = ENEMYTYPES::STATUS_SPECIAL,
    };

    enum ELITETYPE
    {
        ELITETYPE_A = 0, /* dual swords */
        ELITETYPE_B,     /* trident */
        ELITETYPE_C,     /* spear */
        ELITETYPE_D,     /* axe */

        ELITETYPENUM,
        ELITETYPE_UNKNOWN = -1,
    };

    struct BUNSHIN_INFO
    {
        static const int32 MAX_COUNT = 3;

        uint32 ahOwnerObj[MAX_COUNT];
    };

    class CAppearStatusObserver : public CBaseEliteChr::CAppearStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
    };

    class CEliteDeathStatusObserver : public CBaseEliteChr::CDeathStatusObserver
    {
    public:
        virtual void OnStart(void) override;
    };

    class CEliteDummyDeathStatusObserver : public CBaseEliteChr::CDeathStatusObserver
    {
    public:
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CThinkingStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CDashStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fMoveSpeed;
        float m_fMoveTime;
    };

    class CEliteWarpStatusObserver : public CBaseEliteChr::CWarpBaseStatusObserver
    {
    public:
        CEliteWarpStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        ENEMYTYPES::STATUS m_nextStatus;
        int32 m_attackType;
    };

    class CSpinAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        const char* m_apszSpinMotion[3];
        int32 m_step;
        float m_fSpinTimer;
        float m_fMoveSpeed;
        int32 m_attackType;
        int32 m_orderTargetNo;
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CSwingAttackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        const char* m_apszSwingMotion[3];
        int32 m_step;
        float m_fSwingTimer;
        float m_fMoveSpeed;
        int32 m_orderTargetNo;
    };

    class CBunshinStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    protected:
        static const int32 EXEC_MAX = 2;

    public:
        CBunshinStatusObserver(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void Init(void);
        CBaseEliteFootChr* CreateEliteDummy(void);

    protected:
        BUNSHIN_INFO m_bunshinInfo;
        int32 m_bunshinType;
        float m_fBunshinTime;
        int32 m_execNum;
        RwV3d m_vecDummyPos;
        float m_fPosBias;
        float m_fDirSign;
        int32 m_shakeNum;
        ENEMYID::VALUE m_dummyId;
    };

    class CGetUpStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        float m_fLieDowntime;
    };

public:
    DEFINE_EXTEND_STATUS_FN(STATUS, CBaseEliteChr);
    DEFINE_NEEDED_EFFECTS({ EFFECTID::ID_WARP_START });

    CBaseEliteFootChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseEliteFootChr(void);
    virtual bool OnMessageCanChangingAerial(float fHeight) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    ELITETYPE GetEliteType(void) const;
    bool IsEliteDummy(ENEMYID::VALUE enemyId) const;
};