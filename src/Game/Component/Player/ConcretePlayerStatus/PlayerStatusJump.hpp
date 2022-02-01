#pragma once

#include "Game/Component/Player/PlayerStatus.hpp"


namespace PlayerStatus
{
    class CJumpReady : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    
    class CJumpCommon : public CStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnRun(void) override;
    };


    class CJump : public CJumpCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CJump2nd : public CJumpCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    
    class CJumpWall : public CJumpCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CJumpWallFailure : public CJumpCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CAerialCommon : public CJumpCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
    };


    class CAerial : public CAerialCommon
    {
    public:
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CAerialMove : public CAerialCommon
    {
    public:
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CTouchdown : public CStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
};