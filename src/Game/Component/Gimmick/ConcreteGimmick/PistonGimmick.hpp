#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CPistonGimmick final : public CGimmick
{
private:
    struct SUBINFO
    {
        const char* pszModelName;
        const char* pszAtariName;
        float fInitRot;
        float fRotVelocity;
    };

    enum SUBID
    {
        SUBID_START = 0,
        
        SUBID_PISTON_A = SUBID_START,
        SUBID_PISTON_B,
        SUBID_PISTON_C,
        SUBID_PISTON_D,
        SUBID_PISTON_E,
        SUBID_PISTON_F,

        SUBID_NUM,
    };

    enum STATE
    {
        STATE_NONE = 0,
        STATE_RUN,
        STATE_END,
    };

public:
    CPistonGimmick(const char* pszName, void* pParam);
    virtual ~CPistonGimmick(void);
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    static STATE m_eState;
    static const SUBINFO m_aSubInfo[];
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    SUBID m_subid;
    float m_fRotation;
    float m_fRotVelocity;
    RwV3d m_vInitPos;
    RwV3d m_vPos;
    RwV3d m_vPrePos;
};