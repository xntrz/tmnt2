#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CGearGimmick final : public CGimmick
{
public:
    struct SUBINFO
    {
        const char* pszModelName;
        const char* pszAtariName;
        float       fBSphereRadius;
        bool        bOwnerDraw;
        float       fRotVelocity[2];
    };

    enum SUBID
    {
        SUBID_START = 0,
        
        SUBID_ENGINE = SUBID_START,
        SUBID_CORE,
        SUBID_GEAR_A,
        SUBID_GEAR_B,
        SUBID_GEAR_C,
        SUBID_GEAR_D,
        SUBID_GEAR_F,
        SUBID_GEAR_G,
        SUBID_GEAR_H,
        SUBID_GEAR_DA,
        SUBID_GEAR_DB,
        SUBID_ENERGY,
        SUBID_ENERGY_LIGHT,
        SUBID_ENERGY_GARD,
        SUBID_KOA_LIGHT_02,
        SUBID_KOA_LIGHT_03,
        SUBID_KOA_LIGHT_01,
        
        SUBID_NUM,
    };

    enum STATE
    {
        STATE_NONE = 0,        
        STATE_NORMAL,
        STATE_FAST,
        STATE_SLOW,
    };

public:
    CGearGimmick(const char* pszName, void* pParam);
    virtual ~CGearGimmick(void);
    virtual void Draw(void) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    
private:
    static STATE            m_eState;
    static const SUBINFO    m_aSubInfo[];
    CNormalGimmickModel     m_model;
    uint32                  m_hAtari;
    SUBID                   m_subid;
    float                   m_fRotation;
    float                   m_fRotVelocity;
};