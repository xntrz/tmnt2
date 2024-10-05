#pragma once

#include "RideTypes.hpp"
#include "RidePlayer.hpp"
#include "RideStage.hpp"

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

    struct PADINFO
    {
        int32 m_nPadNo;
        float m_fStickX;
        float m_fStickY;
        bool m_bJump;
        bool m_bShot;
        bool m_bRightTurn;
        bool m_bLeftTurn;
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
    virtual PLAYERTYPES::STATUS RequestStatusMorphing(PLAYERTYPES::STATUS status) override;
    virtual void OnMessageShotEol(void);
    virtual void OnMessageShotHit(void);
    virtual void OnMessageScoreAdd(void* param);
    void OnAttach(CRideCharacter* pBeforeCharacter, bool bChangeEffectEnable);
    void OnDetach(CRideCharacter* pAfterCharacter);
    void OnCrashWall(void);
    void OnRevive(void);
    void MakePadInfo(void);
    void CalcControlRate(void);
    void Roll(void);
    void MoveForShip(void);
    void Turn(void);
    void Jump(void);
    void Move(void);
    void MoveByScroll(void);
    void MoveLimit(void);
    void Shot(void);
    void HitWall(HIT_SPHERE_KIND hitspherekind);
    void GetHitSphere(RwSphere* pSphere, HIT_SPHERE_KIND hitspherekind);
    HIT_SPHERE_KIND CheckHit(RwV3d& rvVelocityPerFrame);
    void ReplaceVelocityX(float fVelocityX);
    void SetStartPosition(int32 nPlayerNo, bool bRevive);
    void SetEffectDrawEnable(bool bEnable);
    void SetGravityEnable(bool bEnable);
    bool IsEnableMove(void) const;
    bool IsEnableStagger(void) const;
    bool IsEnableShot(void) const;
    bool IsEnableRoll(void) const;

    inline void SetTurnDirection(float fDir) { m_fTurnDirection = fDir; };
    inline float GetTurnDirection(void) const { return m_fTurnDirection; };
    
    inline void SetControlRate(float fRate) { m_fControlRate = fRate; };
    inline float GetControlRate(void) const { return m_fControlRate; };
    
    inline float GetPlaytime(void) const { return m_fPlaytime; };
    inline bool IsShip(void) const{ return CRideStage::m_bSpace;};
    inline bool CheckDispShot(void) const { return (m_nNumDispShot < 4); };
    inline int32 GetScore(RIDETYPES::SCOREKIND scorekind) const { return m_aScore[scorekind]; };
    inline const PADINFO& PadInfo(void) const { return m_padinfo; };
    
private:
    int32 m_aScore[RIDETYPES::SCOREKINDNUM];
    float m_fControlRate;
    float m_fTurnDirection;
    float m_fPlaytime;
    CRidePlayer* m_pPlayer;
    RwV3d m_vRevivePos;
    int32 m_nNumDispShot;
    PADINFO m_padinfo;
};