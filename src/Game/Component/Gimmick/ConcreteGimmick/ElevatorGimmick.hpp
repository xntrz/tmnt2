#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/Utils/GeneratorObj.hpp"


class CElevatorPartsGimmick;


class CElevatorGimmick final : public CGimmick
{
private:
    enum ID
    {
        ID_M02N_ELEVATOR = 0,
        ID_M05N_ELEVATOR,
    };

    enum STATE
    {
        STATE_NONE = 0,
        STATE_MOVE_START,
        STATE_MOVE,
        STATE_MOVE_END,
        STATE_ENEMY_START,
        STATE_END,
    };

public:
    CElevatorGimmick(const char* pszName, void* pParam);
    virtual ~CElevatorGimmick(void);
    virtual void Draw(void) const override;
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    
private:
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    CElevatorPartsGimmick* m_apParts[3];
    ID m_id;
    STATE m_state;
    float m_fTimer;
    CSettingEnemyGenerator m_generator;    
};


class CElevatorPartsGimmick
{
protected:
    enum STATE
    {
        STATE_NONE = 0,
        STATE_MOVE_START,
        STATE_MOVE,
        STATE_MOVE_END,
    };
    
public:
    CElevatorPartsGimmick(void);
    virtual ~CElevatorPartsGimmick(void);
    virtual void Run(float dt) = 0;
    void Draw(void) const;
    void RegistCollisionModel(RwV3d* pvDltPos);
    void CreateModel(const char* pszModelName, bool bAtari);
    
    inline void MoveStart(void) { m_state = STATE_MOVE_START; };
    inline bool IsMoveEnd(void) const { return (m_state == STATE_MOVE_END); };
    
    inline void SetPosition(const RwV3d* pvPosition) { m_vPosition = *pvPosition;  m_model.SetPosition(&m_vPosition); };
    inline void GetPosition(RwV3d* pvPosition) const { *pvPosition = m_vPosition; };
    
    inline void SetGoalPosition(const RwV3d* pvPosition) { m_vGoalPosition = *pvPosition; };
    inline void GetGoalPosition(RwV3d* pvPosition) const { *pvPosition = m_vGoalPosition; };
    
    inline void SetVelocity(const RwV3d* pvVelocity) { m_vVelocity = *pvVelocity; };

protected:
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    STATE m_state;
    RwV3d m_vPosition;
    RwV3d m_vGoalPosition;
    RwV3d m_vVelocity;
};


class CElevatorPartsFloorGimmick final : public CElevatorPartsGimmick
{
public:
    inline CElevatorPartsFloorGimmick(void) {};
    virtual ~CElevatorPartsFloorGimmick(void) {};
    virtual void Run(float dt) override;
    void Move(RwV3d* pvDltPos, float dt);
};


class CElevatorPartsHatchGimmick final : public CElevatorPartsGimmick
{
public:
    inline CElevatorPartsHatchGimmick(void) {};
    virtual ~CElevatorPartsHatchGimmick(void) {};
    virtual void Run(float dt) override;
    void Move(RwV3d* pvDltPos, float dt);
};