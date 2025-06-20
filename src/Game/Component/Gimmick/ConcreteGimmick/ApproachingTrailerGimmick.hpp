#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"

#include "Game/Component/Player/PlayerTypes.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"


class CApproachingTrailerGimmickMove;
class CApproachingTrailerHeadGimmickMove;
class CGimmickMotion;
class CBodyHitData;


class CApproachingTrailerGimmick final : public CGimmick
{
public:
    enum SUBID
    {
        SUBID_NORMAL = 0,
        SUBID_DESTROYED,

        SUBIDNUM,
    };
    
    enum STATE
    {
        STATE_WAIT = 0,
        STATE_CRUISE,
        STATE_PRE_CHARGE,
        STATE_CHARGE,
        STATE_POST_CHARGE,
        STATE_SLEEP,
        STATE_PRE_ATTACK,
        STATE_ATTACK,
        STATE_POST_ATTACK,
        STATE_EXPLOSE,
        STATE_CHANGE,
        STATE_HEAD_PRE_CHARGE,
        STATE_HEAD_CHARGE,
        STATE_HEAD_POST_CHARGE,
        STATE_STOP,
        
        STATENUM,
    };

    enum ATTACKTYPE
    {
        ATTACKTYPE_THREE_L = 0,
        ATTACKTYPE_THREE_R,
        ATTACKTYPE_SURROUND,
        ATTACKTYPE_CENTER,
        ATTACKTYPE_AREA_L,
        ATTACKTYPE_AREA_R,
        ATTACKTYPE_AIMING_1,
        ATTACKTYPE_AIMING_3,
        ATTACKTYPE_AIMING_5,        

        ATTACKTYPENUM_SHOT, // number of available shot attack types

        ATTACKTYPE_CHARGE = ATTACKTYPENUM_SHOT,

        ATTACKTYPENUM_ALL, // number of available all attacks types
    };

    enum DISTANCE
    {
        DISTANCE_SHORT = 0,
        DISTANCE_MIDDLE,
        DISTANCE_LONG,

        DISTANCENUM,
    };

    struct PARAM : public GIMMICKPARAM::GIMMICK_BASIC
    {
        float fPathTimerate;
    };

    struct PLAYERINFO
    {
        RwV3d               vPosition;
        float               fDistance;
        DISTANCE            eDistance;
        PLAYERTYPES::STATUS eStatus;
    };

    struct LIGHT
    {
        uint32  hLight;
        float   fTimerCounter;
        float   fTime;
        bool    bUpdate;
        bool    bOff;
    };

    class CAttackBase;
    class CAttackAimingShot;
    class CAttackAreaShot;
    class CAttackCenterLinearShot;
    class CAttackSurroundShot;
    class CAttackThreePointShot;

public:
    CApproachingTrailerGimmick(const char* pszName, void* pParam);
    virtual ~CApproachingTrailerGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnAttackResult(CHitCatchData* pCatch) override;
    void Wait(void);
    void Cruise(void);
    void PreCharge(void);
    void Charge(void);
    void PostCharge(void);
    void Sleep(void);
    void PreAttack(void);
    void Attack(void);
    void PostAttack(void);
    void Explose(void);
    void Change(void);
    void HeadPreCharge(void);
    void HeadCharge(void);
    void HeadPostCharge(void);
    void Stop(void);
    void CommonUpdate(void);
    void SetPlayerInfo(void);
    void SEControl(void);
    void SetState(STATE eState);
    void EffectCreate(void);
    void EffectDestroy(void);
    void EffectUpdate(void);
    void LightCreate(void);
    void LightDestroy(void);
    void LightUpdate(void);
    void LightSet(float fTime, bool bOff);
    RwV3d* GetPlayerPos(int32 iPlayerNo = GAMETYPES::PLAYERS_MAX);
    float RateControl(float fCur, float fAim, float dt) const;
    float GetPathTimerateByMPS(float fMPS) const;
    void GetFlyawayPosition(RwV3d* pvResult, const RwV3d* pvPlayerPos, const RwV3d* pvDir) const;
    bool IsDestroyed(void) const;
    bool IsTimingProc(float fTime) const;
    float GetAimAttackPathTime(void) const;
    void GetMovePosition(RwV3d* pvPos) const;
    void GetMoveRotation(RwV3d* pvRot) const;
    float GetPathT(void) const;
    const CNormalGimmickModel* GetNormalModel(void) const;

private:
    using CMove = CApproachingTrailerGimmickMove;
    using CHeadMove = CApproachingTrailerHeadGimmickMove;

    float               m_fFlyawayNearestPathT;
    bool                m_bFlyaway;
    CMove*              m_pApproachingTrailerGimmickMove;
    CHeadMove*          m_pApproachingTrailerHeadGimmickMove;
    CBodyHitData*       m_pBodyHitData;
    CNormalGimmickModel m_model;
    bool                m_bChanged;
    int32               m_nPower;
    float               m_fFlyawayVelXZ;
    float               m_fFlyawayVelY;
    float               m_fAimAttackPathT;
    float               m_fNearestPathT;
    float               m_fAimPathTimerate;
    float               m_fPathTimerate;
    RwV3d*              m_pPos;
    RwV3d*              m_pRot;
    STATE               m_eState;
    float               m_fStepTimer;
    CAttackBase*        m_pAttack;
    CAttackBase*        m_apAttackVariation[ATTACKTYPENUM_SHOT];
    int32               m_nNumAttackVariation;
    int32               m_subid;
    uint32              m_ahEffect[4];
    float               m_fEffectTimer;
    LIGHT               m_aLight[2];
    PLAYERINFO          m_aPlayerInfo[GAMETYPES::PLAYERS_MAX];
    PLAYERINFO          m_playerInfoCenter;
    int32               m_iNearestPlayerNo;
    int32               m_iPlayerNum;
    CGimmickMotion*     m_pMotion;
    RwV3d               m_vHitOffset;
    bool                m_bSEPlay;
    bool                m_bSERun;
    bool                m_bSEStop;
};