#pragma once

#include "Game/Component/Player/PlayerStatus.hpp"


namespace PlayerStatus
{
    class CIdle : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CWalk : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CRun : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CDashCancel : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CDash : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CDashFinish : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CDownCommon : public CStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual const char* GetMotionName(void) const = 0;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const = 0;
    };

    
    class CDownFront : public CDownCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
    };

    
    class CDownBack : public CDownCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
        virtual PLAYERTYPES::STATUS GetNextStatus(void) const override;
    };

    
    class CTumblerCommon : public CStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
        virtual const char* GetMotionName(void) const = 0;
    };


    class CTumblerFront : public CTumblerCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
    };


    class CTumblerBack : public CTumblerCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual const char* GetMotionName(void) const override;
    };
};