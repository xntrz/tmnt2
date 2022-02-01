#pragma once

#include "RideTypes.hpp"
#include "RidePlayer.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"


class CRidePlayer;


class CRideCharacter : public CPlayerCharacter
{
public:
    enum RIDERTYPE
    {
        RIDERTYPE_SHIP = 0,
        RIDERTYPE_BOARD,
    };

    enum HIT_SPHERE_KIND
    {
        HIT_SPHERE_KIND_NONE = -1,
        HIT_SPHERE_KIND_CENTER = 0,
        HIT_SPHERE_KIND_LEFT,
        HIT_SPHERE_KIND_RIGHT,
        
        HIT_SPHERE_KIND_NUM,
    };

public:
    static CRideCharacter* New(PLAYERID::VALUE idChr, CRidePlayer* pRidePlayer, GAMETYPES::COSTUME costume);
    static void Delete(CRideCharacter* pRideCharacter);
    
    CRideCharacter(const char* pszName, PLAYERID::VALUE idPlayer, CRidePlayer* pRidePlayer, GAMETYPES::COSTUME costume);
    virtual ~CRideCharacter(void);
    virtual void Run(void) override;
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual void Draw(void) const override;
    virtual void CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame) override;
    virtual void CheckCollisionForBody(RwV3d& rvVelocityPerFrame) override;
    virtual void CheckCollisionForWall(RwV3d& rvVelocityPerFrame) override;
    virtual void OnMessageAerial(float fFieldHeight) override;
    virtual bool OnMessageCanChangingAerial(float fFieldHeight) override;
    virtual void OnMessageDoChangingAerial(void) override;
    virtual void OnMessageTouchdown(float fFieldHeight) override;
    virtual void OnMessageCatchAttack(CHitAttackData* pAttack) override;
    virtual void OnMessageReceivedDamage(int32 nDamage) override;
    virtual void OnSteppedDeathFloor(void) override;
    virtual void ShootingKnife(void) override;
    virtual PLAYERTYPES::STATUS RequesStatusMorphing(PLAYERTYPES::STATUS status) override;
    virtual void OnMessageShotEol(void);
    virtual void OnMessageShotDestroy(void);
    virtual void OnMessageScoreAdd(void* param);
    void OnAttach(CRideCharacter* pBeforeCharacter, bool bChangeEffectEnable);
    void OnDetach(CRideCharacter* pAfterCharacter);
    bool IsEnableMove(void) const;
    bool IsEnableStagger(void) const;
    bool IsEnableShot(void) const;
    bool IsEnableRoll(void) const;
    void SetStartPosition(int32 nPlayerNo, bool bRevive);
    void SetEffectDrawEnable(bool bEnable);
    void SetGravityEnable(bool bEnable);
    void HitWall(HIT_SPHERE_KIND hidspherekind);
    void GetHitSphere(RwSphere* pSphere, HIT_SPHERE_KIND hitspherekind);
    HIT_SPHERE_KIND CheckHit(RwV3d& rvVelocityPerFrame);
    void ReplaceVelocityX(float fVelocityX);
    void CalcControlRate(void);
    void MoveLimit(void);
    void SetTurnDirection(float fDir);
    float GetTurnDirection(void) const;
    float GetPlaytime(void) const;
    float GetControlRate(void) const;
    bool IsShip(void) const;
    void ResetTurnDirection(void);
    void OnCrashWall(void);
    void OnRevive(void);
    void InitBandana(void);
    void UpdateBandana(void);
    bool CheckDispShot(void) const;
    int32 GetScore(RIDETYPES::SCOREKIND scorekind) const;

private:
    int32 m_aScore[RIDETYPES::SCOREKINDNUM];
    float m_fControlRate;
    float m_fTurnDirection;
    float m_fPlaytime;
    CRidePlayer* m_pPlayer;
    RwV3d m_vRevivePos;
    int32 m_nNumDispShot;
};