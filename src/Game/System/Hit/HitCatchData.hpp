#pragma once

#include "Game/System/GameObject/GameObjectType.hpp"


class CHitAttackData;
class CGameObject;


class CHitCatchData : public CListNode<CHitCatchData>
{
public:
    enum SHAPE
    {
        SHAPE_SPHERE = 0,
    };
    
    enum RESULT
    {
        RESULT_INVALID = 0,
        RESULT_HIT,
        RESULT_GUARD,
        RESULT_GUARDIMPACT,
        RESULT_THROWFRONT,
        RESULT_THROWBACK,
    };
    
public:
    CHitCatchData(void);
    ~CHitCatchData(void);
    void Cleanup(void);
    void CopyData(CHitCatchData* pHitCatchData) const;
    void SetShape(SHAPE shape);
    void SetSphere(const RwSphere* pSphere);
    void SetObject(uint32 hObj);
    void SetObjectType(GAMEOBJECTTYPE::VALUE objtype);
    void SetCatchNo(int32 no);
    void SetFlagCounter(bool bSet);
    void SetFlagAllocated(bool bSet);
    void SetFlagAlive(bool bSet);
    void SetFlagHitPosCalculated(bool bSet);
    void SetAttack(CHitAttackData* pAttack);
    void SetResult(RESULT result);
    SHAPE GetShape(void) const;
    const RwSphere* GetSphere(void) const;
    CGameObject* GetObject(void) const;
    uint32 GetObjectHandle(void) const;
    GAMEOBJECTTYPE::VALUE GetObjectType(void) const;
    int32 GetCatchNo(void) const;
    bool IsFlagCounter(void) const;
    bool IsFlagAllocated(void) const;
    bool IsFlagAlive(void) const;
    CHitAttackData* GetAttack(void) const;
    const RwV3d* GetHitPos(void);
    RESULT GetResult(void) const;

private:
    uint32 m_uObjectHandle;
    GAMEOBJECTTYPE::VALUE m_ObjectType;
    int32 m_nCatchNo;
    SHAPE m_shapeType;
    union
    {
        RwSphere m_sphere;
    } m_shape;
    RESULT m_result;
    bool m_bCounter;
    bool m_bIsHitPosCalculated;
    bool m_bNewAllocated;
    bool m_bAlive;
    RwV3d m_vHitPos;
    CHitAttackData* m_pHitAttackData;
};