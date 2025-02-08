#pragma once

#include "Game/Component/Player/PlayerStatus.hpp"


namespace PlayerStatus
{
    class CKnockCommon : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CKnockFront : public CKnockCommon
    {
    public:
        virtual void OnAttach(void) override;
    };

    
    class CKnockBack : public CKnockCommon
    {
    public:
        virtual void OnAttach(void) override;
    };


    class CFlyawayCommon : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual void OnCrashWall(void) = 0;
    };


    class CFlyawayFront : public CFlyawayCommon
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnCrashWall(void) override;
    };

    
    class CFlyawayBack : public CFlyawayCommon
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnCrashWall(void) override;
    };


    class CFlyawayBoundCommon : public CStatus
    {
    public:
        virtual void OnAttach(void);
        virtual void OnDetach(void);
        virtual void OnRun(void);
        virtual const char* GetMotionName(void) const = 0;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const = 0;
        virtual PLAYERTYPES::STATUS GetNextStatusOfPassive(void) const = 0;
    };


    class CFlyawayBoundFront : public CFlyawayBoundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatusOfPassive(void) const override;
    };


    class CFlyawayBoundBack : public CFlyawayBoundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnDetach(void) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatusOfPassive(void) const override;
    };


    class CCrashWallCommon : public CStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual const char* GetMotionName(void) const = 0;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const = 0;
    };


    class CCrashWallFront : public CCrashWallCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
    };


    class CCrashWallBack : public CCrashWallCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
    };


    class CCrashWallFallCommon : public CStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CCrashWallFallFront : public CCrashWallFallCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };


    class CCrashWallFallBack : public CCrashWallFallCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };


    class CCrashWallTouchdownCommon : public CStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual const char* GetMotionName(void) const = 0;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const = 0;
    };

    
    class CCrashWallTouchdownFront : public CCrashWallTouchdownCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
    };
    

    class CCrashWallTouchdownBack : public CCrashWallTouchdownCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
    };


    class CStatusDamage : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void);
        virtual void OnDetach(void);
        virtual void OnRun(void);
        bool IsEnd(void) const;

    protected:
        float m_fRecoverWait;
        float m_fEndTime;
    };


    class CDindle : public CStatusDamage
    {
    public:
        virtual void OnAttach(void);
    };


    class CStun : public CStatusDamage
    {
    public:
        virtual void OnAttach(void);
    };


    class CSleep : public CStatusDamage
    {
    public:
        virtual void OnAttach(void);
    };


    class CBind : public CStatusDamage
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void);
    };


    class CFreeze : public CStatusDamage
    {
    public:
        virtual void OnAttach(void);
    };
};