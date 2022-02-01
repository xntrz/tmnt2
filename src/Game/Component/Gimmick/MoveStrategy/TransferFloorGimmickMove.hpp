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
        PASSINGWAY_GO,
        PASSINGWAY_BACK,
    };

public:
    CTransferFloorGimmickMove(void);
    virtual ~CTransferFloorGimmickMove(void);
    virtual RESULT OnMove(float dt) override;
    void SetVelocity(const RwV3d* pvVelocity);
    void StartTransfer(void);
    void SetMoveTime(float t);
    void SetStopTime(float t);
    void SetKind(int32 kind);
    int32 GetKind(void) const;

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
    void SetStartPosition(const RwV3d* pvPosition);
    void SetGoalPosition(const RwV3d* pvPosition);
    
protected:
    RwV3d m_vStartPosition;
    RwV3d m_vGoalPosition;
};