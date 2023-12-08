#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CPropellerGimmick final : public CGimmick
{
private:
    struct SUBINFO
    {
        const char* pszModelName;
        float fRotVelocity;
    };

    enum SUBID
    {
        SUBID_START = 0,

        SUBID_PROPELLER_A = SUBID_START,
        SUBID_PROPELLER_B,
        SUBID_PROPELLER_C,

        SUBID_NUM,
    };

public:
    CPropellerGimmick(const char* pszName, void* pParam);
    virtual ~CPropellerGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    static bool m_bHitAttack;
    static const SUBINFO m_aSubInfo[];
    CNormalGimmickModel m_model;
    SUBID m_subid;
    RwV3d m_vPos;
    float m_fRotation;
    float m_fRotVelocity;
};