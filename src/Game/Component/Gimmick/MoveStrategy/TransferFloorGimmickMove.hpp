#pragma once

#include "Game/Component/Gimmick/GimmickMove.hpp"


class CTransferFloorGimmickMove : public CGimmickMove
{
protected:
    enum PHASE
    {
        PHASE_NONE = 0,
        PHASE_STOP,
        PHASE_MOVE,
    };

    enum PASSINGWAY
    {
        PASSINGWAY_GO = 0,
        PASSINGWAY_BACK,
    };

public:
    CTransferFloorGimmickMove(void);
    virtual ~CTransferFloorGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void StartTransfer(void);
    
    inline void SetVelocity(const RwV3d* pvVelocity) { m_vVelocity = *pvVelocity; };
    inline void SetMoveTime(float t) { m_fMoveTime = t; };
    inline void SetStopTime(float t) { m_fStopTime = t; };
    inline void SetKind(int32 kind) { m_type = kind; };
    inline int32 GetKind(void) const { return m_type; };

protected:
    float m_fMoveTime;
    float m_fStopTime;
    float m_fTimer;
    PHASE m_phase;
    PASSINGWAY m_passingway;
    int32 m_type;
    RwV3d m_vVelocity;
};


class CLinearTransferFloorGimmickMove : public CTransferFloorGimmickMove
{
public:
    CLinearTransferFloorGimmickMove(void);
    virtual ~CLinearTransferFloorGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    
    inline void SetStartPosition(const RwV3d* pvPosition) { m_vStartPosition = *pvPosition; };
    inline void SetGoalPosition(const RwV3d* pvPosition) { m_vGoalPosition = *pvPosition; };

protected:
    RwV3d m_vStartPosition;
    RwV3d m_vGoalPosition;
};


class CPathTransferFloorGimmickMove : public CTransferFloorGimmickMove
{
public:
    CPathTransferFloorGimmickMove(void);
    virtual ~CPathTransferFloorGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetPathName(const char* pszName);

protected:
    float m_fPathT;
    char m_szPathName[16];
};


class CRotateTransferFloorGimmickMove : public CTransferFloorGimmickMove
{
public:
    CRotateTransferFloorGimmickMove(void);
    virtual ~CRotateTransferFloorGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetRoundTime(float t);
    
    inline void SetRotAxis(RwMatrix* mat) { m_vRotAxis = Math::VECTOR3_AXIS_Y; };    
    inline void GetRotation(RwV3d* pvRot) const { *pvRot = m_vRotation; };

protected:
    float m_fRoundTime;
    RwV3d m_vRotAxis;
    RwV3d m_vRotation;
    bool m_bTurnRight;
};