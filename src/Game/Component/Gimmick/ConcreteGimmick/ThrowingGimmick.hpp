#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"

#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Character/Character.hpp"


class CThrowingGimmickMove;
class CModuleManager;
class CBodyHitData;


class CThrowingGimmick : public CGimmick
{
protected:
    struct KINDINFO
    {
        const char* m_pszModelName;
        const char* m_pszAtariModelName;
        float       m_fRadius;
        float       m_fHitAttackRadius;
        float       m_fShadowRadius;
        int32       m_nAttackPower;
        int32       m_nLife;
        bool        m_bHeavy;
        int32       m_nBreakSE;
    };
    
    enum STATE
    {
        STATE_NONE = 0,
        STATE_WAIT,
        STATE_INFORM,
        STATE_LIFTED,
        STATE_THROWN,
        STATE_BROKEN,
        STATE_END,
    };

    enum TYPE
    {
        TYPE_NORMAL = 0,
        TYPE_ITEMBOX,
    };

    struct SUBID_THROWING
    {
        enum VALUE
        {
            DRUM = 0,
            DRUM_1,
            DRUM_EKITAI,
            DRUM_SLEEP,
            BOX,
            VASE,
            TRASHBOX_L,
            ROCK_S,
            ROCK_L,
            DESK,
            CHAIR,
            CANDLESTICK,
            TRASHBOX_S,

            MAX,
        };
    };

    struct SUBID_ITEMBOX
    {
        enum VALUE
        {
            BOX = 0,
            VASE,
            VASE_1,

            MAX,
        };
    };

public:
    CThrowingGimmick(const char* pszName, void* pParam);
    virtual ~CThrowingGimmick(void);
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual void Draw(void) const override;
    virtual void GetCenterPosition(RwV3d* pvPosition) const override;
    virtual GIMMICKTYPES::FEATURE GetFeatures(void) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnTouchedDown(void) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    virtual void OnCatch(CHitCatchData::RESULT* pResult);
    virtual void OnLift(CCharacter::MSG_LIFT_INFO* pMsgLiftInfo);
    virtual void OnMissThrow(RwV3d* pvVelocity);
    virtual void OnThrow(RwV3d* pvVelocity);

protected:
    void init(void* pParam);
    void waiting(void);
    void inform(void);
    void lifting(void);
    void throwing(void);
    void broken(void);
    void onInform(void);
    void onBreak(void);
    void setBreakEffect(void);
    void setKagaribiBreakEffect(void);
    void setDefaultBreakEffect(void);
    void setThrowingParam(RwV3d* pvVelocity);
    void updateModule(void);
    void updateKagaribiFireEffect(void);
    void createItem(void);
    void changeItemBySituation(void);
    void setLiftGimmickPosition(RwV3d* pvPosition, CCharacter::MSG_LIFT_INFO* pInfo);
    bool isPlayerEnableLift(const CGameObject* pGameObject);

protected:
    static const KINDINFO   m_aThrowingGimmickKindInfoList[];
    static const KINDINFO   m_aItemBoxGimmickKindInfoList[];
    CThrowingGimmickMove*   m_pThrowingMove;
    CNormalGimmickModel     m_model;
    STATE                   m_state;
    TYPE                    m_type;
    int32                   m_subid;
    int32                   m_nItem;
    float                   m_fTimer;
    float                   m_fTimerPlayerRideOnJudge;
    CBodyHitData*           m_pBodyHitData;
    CModuleManager*         m_pModuleManager;
    uint32                  m_hAtari;
    const KINDINFO*         m_pKindinfo;
    uint32                  m_hKagaribiEffect;
    int32                   m_nLife;
};