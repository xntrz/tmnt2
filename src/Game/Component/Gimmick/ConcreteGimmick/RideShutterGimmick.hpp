#pragma once

#include "RideGimmick.hpp"


class CGimmickMotion;

class CRideShutterGimmick final : public CRideGimmick
{
public:
    CRideShutterGimmick(const char* pszName, void* pParam);
    virtual ~CRideShutterGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CGimmickMotion* m_pMotionController;
    CGimmickMotion* m_pMotionControllerAtari;
    uint32 m_hAtari;
    bool m_bRunFlag;
};