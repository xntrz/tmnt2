#pragma once

#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/CommonEnemyObserver.hpp"


class CPlayerCharacter;


class CBaseGeneralEnemyChr : public CEnemyCharacter
{
public:
    enum MOVABLETYPE
    {
        MOVABLETYPE_POSSIBLE = 0,
        MOVABLETYPE_IMPOSSIBLE,
        MOVABLETYPE_JUMP,
        MOVABLETYPE_FALLDOWN,
        MOVABLETYPE_HOLE,
        MOVABLETYPE_BODILYCRUSH
    };

    class CHideStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CThinkingStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        ENEMYTYPES::STATUS m_nextStatus;
    };

    class CIdleStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fTime;
    };

    class CTurnStatusObserver : public CStatusObserver
    {
    public:
        CTurnStatusObserver(float fBaseTurnAngleDeg, const char* pszMotionTurnLeft, const char* pszMotionTurnRight);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual bool IsRotation(void);

    private:
        char  m_szTurnL[16];
        char  m_szTurnR[16];
        float m_fDirection;
        float m_fBaseTurnAngle; // in degree
        float m_fRadPerFrame;
        int32 m_iTurnCount;
        float m_fBeforeMotionTime;
    };

    class CDummyTurnStatusObserver : public CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fDirection;
    };

    class CSideWalkStatusObserver : public CStatusObserver
    {
    public:
        CSideWalkStatusObserver(float fMotionSpeed, const char* pszMotionSideWalk);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CAIThinkOrder::ORDER m_order;
        float                m_fMoveSpeed;
        float                m_fMoveTime;
        float                m_fMotionSpeed;
        char                 m_szSideWalkMotion[16];
    };

    class CGuardStatusObserver : public CMotionManagedStatusObserver
    {
    private:
        enum MOTIONID
        {
            MOTIONID_GUARD_BEGIN = 0,
            MOTIONID_GUARD_HOLD,
            MOTIONID_GUARD_END,
        };

    public:
        CGuardStatusObserver(float fGuardTime);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        CAIThinkOrder::ORDER m_orgOrder;
        float m_fGuardTime;
        float m_fRemainGuardTime;
    };

    class CDeathMechStatusObserver : public CStatusObserver
    {
    public:
        CDeathMechStatusObserver(float fSparkPlayTime);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fSparkPlayTime;
        float m_fTimer;
    };

public:
    static void GetMoveAt(CAIThinkOrder& AIThinkOrder, RwV3d* at);

    CBaseGeneralEnemyChr(ENEMYID::VALUE idEnemy);
    virtual ~CBaseGeneralEnemyChr(void);
    virtual void Run(void) override;
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& rCalc) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    virtual int32 GetDamageAccumulation(int32 no) const;
    virtual MOVABLETYPE GetMovableType(RwV3d* pVecAt) const;
    virtual bool IsStayTrajectory(CPlayerCharacter* pPlayerChr, float fRange);

private:
    int32 m_iLastAttackerNo;
    int32 m_aiDamageAccumulation[8];
    RwV3d m_vPrePosition;
}; 