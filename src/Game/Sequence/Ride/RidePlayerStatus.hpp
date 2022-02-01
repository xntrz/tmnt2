#pragma once

#include "Game/Component/Player/PlayerStatus.hpp"


class CRideCharacter;


namespace RidePlayerStatus
{
    class CRideStatus : public CStatus
    {
    public:
        virtual void ChangeMotion(PLAYERTYPES::STATUS status);
        CRideCharacter& RideCharacter(void);
    };

    
    class CGroundCommon : public CRideStatus
    {
    public:
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    
    class CRun : public CGroundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };
    

    class CRight : public CGroundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };
    

    class CLeft : public CGroundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };
    

    class CSpeedUp : public CGroundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };
    

    class CSpeedDown : public CGroundCommon
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
    };


    class CCrashWall : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    
    class CRevive : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CStagger : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CJump : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CTouchdown : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };

    
    class CAerial : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
    

    class CAerialStagger : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CShipCrash : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CShipRightRoll : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };


    class CShipLeftRoll : public CRideStatus
    {
    public:
        virtual bool IsEnableChangeStatus(PLAYERTYPES::STATUS status) override;
        virtual void OnAttach(void) override;
        virtual void OnDetach(void) override;
        virtual void OnRun(void) override;
    };
};