#pragma once

#include "Game/Component/Enemy/CommonEnemyObserver.hpp"
#include "Game/Component/Enemy/EnemyCharacter.hpp"
#include "Game/Component/Enemy/EnemyUtils.hpp"


class CBaseEliteAI;


class CBaseEliteChr : public CEnemyCharacter
{
public:
    class CAppearStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };
    
    class CHideStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CDeathStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CIdleStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fWaitTime;
    };

    class CWalkStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_moveType;
        float m_fMoveSpeed;
        float m_fMoveTime;
    };

    class CDashStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        bool ProcDashTimer(void);

    private:
        const char* m_pszMotionNext;
        float m_fMoveTime;
        int32 m_dashType;
        int32 m_step;
    };

    class CWarpBaseStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        enum WARPTYPE
        {
            WARPTYPE_LEFT = 0,
            WARPTYPE_RIGHT,
        };

    public:
        CWarpBaseStatusObserver(WARPTYPE warpType);
        CWarpBaseStatusObserver(float fEndTime, WARPTYPE warpType);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        void CreateBezierPoint(const RwV3d* pvecTargetPos, float fTargetRot);
        void SetPlayerNo(int32 playerNo);
        void SetTargetPos(const RwV3d* pvecTargetPos);
        float GetThetaVec2Vec(const RwV3d* v1, const RwV3d* v2) const;

    protected:
        WARPTYPE m_warpType;
        float m_fEndTime;
        float m_fTime;
        RwV3d m_p1;
        RwV3d m_p2;
        RwV3d m_p3;
        RwV3d m_vecTargetPos;
        float m_fTargetRot;
        RwV3d m_vecBezierEndPos;
        int32 m_orderTargetNo;
    };

    class CGuardStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        const char* m_apszGuardMotion[2];
        int32 m_step;
        float m_fDefenceTime;
        int32 m_defenceType;
    };

    class CJumpReadyStatusObserver : public CCommonEnemyObserver::CJumpReadyStatus
    {
    public:
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CKnockStatusObserver : public CCommonEnemyObserver::CKnockStatus
    {
    public:
        virtual RESULT Observing(void) override;
    };
    
public:
    CBaseEliteChr(ENEMYID::VALUE enemyId);
    virtual ~CBaseEliteChr(void);
    virtual void Run(void) override;
    virtual void OnMessageReceivedDamage(int32 iAmount) override;
    CBaseEliteAI& AIModerator(void);
    const CBaseEliteAI& AIModerator(void) const;

protected:
    CEnemyUtils::CKnockBackControl m_knockBackCtrl;
};