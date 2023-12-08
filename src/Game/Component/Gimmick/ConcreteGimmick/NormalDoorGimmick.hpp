#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CGimmickMotion;


class CNormalDoorGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_WAIT = 0,
        STATE_OPENING,
        STATE_OPENED,
        
        STATENUM,
    };

    enum MODELTYPE
    {
        MODELTYPE_DISP = 0,
        MODELTYPE_COLL,
        
        MODELTYPENUM,
    };

    enum SUBID
    {
        SUBID_ELEVATOR = 0,
        SUBID_REFRECTION_DOOR,
        SUBID_TRICERATOR_DOOR,
        SUBID_TRICERATOR_SHUTTER,
        SUBID_THINWOOD_DOOR,
        SUBID_RUSTSTEEL_DOOR,
        SUBID_KURAIYAMA_DOOR,
        SUBID_SEETHROUGH_DOOR,

        SUBIDNUM,
    };

    struct KINDINFO
    {
        const char* m_pszModelName;
        int32 m_nSE;
    };

public:
    CNormalDoorGimmick(const char* pszName, void* pParam);
    virtual ~CNormalDoorGimmick(void);
    virtual void PreMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    const KINDINFO& kindinfo(void) const;

protected:
    static const KINDINFO m_aDoorKindInfoList[];
    CGimmickMotion* m_pMotion;
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    CModel* m_apModel[MODELTYPENUM];
    STATE m_state;
    int32 m_subid;
    float m_fMotionRemainTime;
    char m_szMotionName[16];
    bool m_bStart;
};