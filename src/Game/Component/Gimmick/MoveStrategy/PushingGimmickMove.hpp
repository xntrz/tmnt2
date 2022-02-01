#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CPlayerCharacter;


class CPushingGimmickMove : public CGimmickMove
{
public:
    enum PUSHSTATE
    {
        PUSHSTATE_PUSH = 0,
        PUSHSTATE_ROTATION,
        PUSHSTATE_FALL,
        PUSHSTATE_END,
    };

    enum FALL_ROT_AXIS
    {
        FALL_ROT_AXIS_X_P = 0,
        FALL_ROT_AXIS_X_N,
        FALL_ROT_AXIS_Z_P,
        FALL_ROT_AXIS_Z_N,
    };

public:
    CPushingGimmickMove(void);
    virtual ~CPushingGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    bool RequestPlayerPushGimmick(CPlayerCharacter* pPlayerCharacter);
    void SetRadius(float fRadius);
    void SetVelocity(const RwV3d* pvVelocity);
    void SetRotation(const RwV3d* pvRotation);
    void SetSpeed(float fSpeed);
    void SetName(const char* pszName);
    PUSHSTATE GetPushState(void) const;

private:
    bool isSameDirectionVectors(const RwV3d* pvVec0, const RwV3d* pvVec1, float fDotRange);
    bool isFall(const RwV3d* pvAt, const RwV3d* pvRight);
    bool isExistsCharacterNear(const RwV3d* pvPosition, float fRadius);

private:
    RwV3d m_vVelocity;
    float m_fRadius;
    float m_fSpeed;
    RwV3d m_vRotation;
    RwV3d m_vRotVelocity;
    bool m_bRequestPush;
    char m_szName[16];
    PUSHSTATE m_pushstate;
    FALL_ROT_AXIS m_fallaxis;
};