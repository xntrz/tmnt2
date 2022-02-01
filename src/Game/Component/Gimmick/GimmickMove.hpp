#pragma once


class CGimmickMove
{
public:
    enum MOVESTATE
    {
        MOVESTATE_NONE = 0,
        MOVESTATE_ON_GROUND,
        MOVESTATE_TOUCHDOWN,
        MOVESTATE_LIFTOFF,
        MOVESTATE_IN_THE_AIR,
    };

    enum STATE
    {
        STATE_NONE = 0,
        STATE_MOVING,
        STATE_CAPTURED,
    };

    enum RESULT
    {
        RESULT_NONE = 0,
        RESULT_COLLIDED,
    };

public:
    CGimmickMove(void);
    virtual ~CGimmickMove(void);
    virtual RESULT OnMove(float dt);
    void Start(void);
    void Stop(void);
    MOVESTATE Move(float dt);
    void SetPosition(const RwV3d* pPos);
    void GetPosition(RwV3d* pvPosition) const;
    void GetLook(RwV3d* pvLook) const;
    
protected:
    RwMatrix* m_pRotationMatrix;
    RwV3d m_vPosition;
    RwV3d m_vLook;
    RwV3d m_vUp;
    STATE m_state;
    MOVESTATE m_movestate;
    RESULT m_result;
};