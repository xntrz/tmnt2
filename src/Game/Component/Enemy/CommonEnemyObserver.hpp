#pragma once

#include "EnemyCharacter.hpp"
#include "CharacterCompositor.hpp"


class CMotionManagedStatusObserver : public CEnemyCharacter::CStatusObserver
{
private:
    static const int32 MOTION_MAX = 8;

public:
    CMotionManagedStatusObserver(void);
    virtual ~CMotionManagedStatusObserver(void);
    virtual void Append(void) override;
    virtual void Remove(void) override;
    virtual void Update(void) override;
    virtual void MotionRegist(int32 id, const char* pszMotion, bool bChainFlag = false);
    virtual void MotionUnregist(int32 id);
    virtual const char* MotionGet(int32 id) const;
    virtual bool MotionPlay(int32 id, bool bReset = false);
    virtual void MotionStop(void);
    virtual int32 MotionCurrent(void) const;
    virtual bool IsMotionEnd(void) const;
    virtual bool OnMotionChange(int32 idOld, int32 idNew);
    virtual void OnMotionEnd(void);

private:
    char  m_aszMotion[MOTION_MAX][16];
    bool  m_abMotionChain[MOTION_MAX];
    int32 m_motionPrev;
    int32 m_motionCurr;
    bool  m_bPlayFlag;
};


class CCommonEnemyObserver
{
public:
    class CIdleStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

	protected:
        float m_fTime;
    };

    class CJumpReadyStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CAerialStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CTouchdownStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CMoveStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        ENEMYTYPES::STATUS m_nextStatus;
        float m_fMoveTime;
        float m_fMoveSpeed;
        int32 m_moveTypeJudgeBegin;
        int32 m_moveTypeJudgeEnd;
        int32 m_moveTypeJudgeMovePos;
    };

    class CDashStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        int32 m_step;
        float m_fDashSpeed;
    };

    class CGuardStatus : public CEnemyCharacter::CStatusObserver
    {
    protected:
        enum STEP
        {
            STEP_START = 0,
            STEP_START_RUN,
            STEP_HOLD,
            STEP_HOLD_RUN,
            STEP_END,
            STEP_END_RUN,
        };

    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        int32 m_step;
        float m_fRemainGuardTime;
    };

    class CKnockStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CKnockFrontStatus : public CKnockStatus {};
    class CKnockBackStatus : public CKnockStatus {};

    class CFlyawayStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        CFlyawayStatus(bool bHandleCrashWallFlag);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        ENEMYTYPES::STATUS m_nextStatus;
        float m_fJumpInitSpeed;
        bool m_bCrashWallFlag;
    };

    class CFlyawayFrontStatus : public CFlyawayStatus {};
    class CFlyawayBackStatus : public CFlyawayStatus {};

    class CFlyawayBoundStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        bool m_bCheckVelocityFlag;
        bool m_bHitFlag;
        bool m_bDirCorrection;
    };

    class CFlyawayBoundFrontStatus : public CFlyawayBoundStatus {};
    class CFlyawayBoundBackStatus : public CFlyawayBoundStatus {};

    class CCrashWallStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fStationaryTime;
    };

    class CCrashWallFrontStatus : public CCrashWallStatus {};
    class CCrashWallBackStatus : public CCrashWallStatus {};

    class CCrashWallFallStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CCrashWallFallFrontStatus : public CCrashWallFallStatus {};
    class CCrashWallFallBackStatus : public CCrashWallFallStatus {};

    class CCrashWallTouchdownStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CCrashWallTouchdownFrontStatus : public CCrashWallTouchdownStatus {};
    class CCrashWallTouchdownBackStatus : public CCrashWallTouchdownStatus {};

    class CGetupStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        bool m_bStepFlag;
    };

    class CCharacterEffectStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    protected:
        float m_fTargetTime;
        float m_fTime;
        bool m_bBodyHitFlag;
    };

    class CStunStatusObserver : public CCharacterEffectStatusObserver {};
    class CSleepStatusObserver : public CCharacterEffectStatusObserver {};
    class CBindStatusObserver : public CCharacterEffectStatusObserver {};
    class CFreezeStatusObserver : public CCharacterEffectStatusObserver {};
    class CDindleStatusObserver : public CCharacterEffectStatusObserver {};

    class CCounteractStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CCaughtStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CLiftedStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CLiftedWalkStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CThrownFrontStatusObserver : public CFlyawayStatus
    {
    public:
        CThrownFrontStatusObserver(bool bHandleCrashWallFlag, float fForce);
        virtual void OnStart(void) override;

    private:
        float m_fThrownForce;
    };

    class CThrownBackStatusObserver : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class COneShotMotion : public CEnemyCharacter::CStatusObserver
    {
    public:
        COneShotMotion(const char* pszMotion);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        char m_szMotion[16];
    };

    class CWalkingAppearStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        CWalkingAppearStatus(float fAppearDistance);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;

    private:
        float m_fAppearDistance;
        RwV3d m_vInitialPos;
        float m_fTime;
    };

    class CNoneAppearStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CFallAppearStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CMotionAppearStatus : public CEnemyCharacter::CStatusObserver
    {
    public:
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
    };

    class CBaseThrow : public CEnemyCharacter::CStatusObserver
    {
    public:
        CBaseThrow(void);
        virtual void OnStart(void) override;
        virtual RESULT Observing(void) override;
        virtual ENEMYTYPES::STATUS OnEnd(void) override;
        virtual void OnThrowStandby(void);
        virtual void OnLift(void);
        virtual void OnThrowHit(void);
        void SetAttackParam(const CHitCatchData* pCatch);
        void SetThrowVector(const RwV3d* pvVec);
        void SetTraceBoneData(int32 boneID, const RwV3d* pvOffset);
        void SendLiftMessage(void);
        void GetLiftPosition(RwV3d* pvPos);        
        bool IsThrowSuccess(void) const;
        bool IsSendCatch(void) const;
        bool IsThrowHit(void) const;

    protected:
        int32 m_throwBoneID;
        RwV3d m_vBoneOffset;
        RwV3d m_vThrowDirection;
        float m_fLiftPlayerDirection;
        int8 m_step;
        bool m_bThrowHit;
        bool m_bSendCatch;
        bool m_bThrowSuccess;
        int32 m_throwDamage;
        int32 m_throwTarget;
        CCharacter::MSG_LIFT_INFO m_liftInfo;
    };
};