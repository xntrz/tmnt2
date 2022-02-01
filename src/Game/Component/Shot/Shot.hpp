#pragma once

#include "ShotID.hpp"

#include "Game/System/GameObject/GameObject.hpp"


class CTracer;
class CModel;
class CLocus;


class CShot : public CGameObject
{
public:
    struct PARAMETER
    {
        SHOTID::VALUE m_idShot;
        RwV3d m_vPosition;
        RwV3d m_vDirection;
        CGameObject* m_pObject;
        float m_fLifetime;
    };
    
public:
    static CShot* New(const PARAMETER* pParameter);
    
protected:
    enum LOCUSKIND
    {
        LOCUSKIND_HOR = 0,
        LOCUSKIND_VER,

        LOCUSKINDNUM,
    };
    
    enum STEP
    {
        STEP_FLY = 0,
        STEP_FALL,
        STEP_STOP,
        STEP_UP,
        STEP_DOWN,
    };

    CShot(const PARAMETER* pParameter);
    
public:
    virtual ~CShot(void);
    virtual void Run(void) override;
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual void Draw(void);
    virtual void SetModel(void);
    virtual void Move(void);
    void Kill(void);
    void Finish(void);
    bool IsEnd(void) const;
    void CheckAttack(void);
    bool CheckFinishByHit(void);
    bool CheckHit(RwV3d* pvCollisionPoint);
    void SetHitSphere(RwSphere* pSphere);
    void SetLocus(void);
    void RotateModel(float fSpeed);
    float GetDirection(void) const;    
    void GetPosition(RwV3d* pvPosition) const;
    void GetVelocity(RwV3d* pvVelocity) const;
    void GetAcceleration(RwV3d* pvAcceleration) const;
    uint32 GetParentHandle(void) const;
    SHOTID::VALUE GetID(void) const;

protected:
    uint32 m_hParentObj;
    RwV3d m_vPosition;
    RwV3d m_vVelocity;
    RwV3d m_vAcceleration;
    float m_fTimer;
    float m_fLife;
    CModel* m_pModel;
    CLocus* m_apLocus[LOCUSKINDNUM];
    float m_fRadius;
    int32 m_nPower;
    uint32 m_uTarget;
    SHOTID::VALUE m_id;
    STEP m_step;
    uint32 m_hEffect;
    bool m_bReflection;
    bool m_bFinish;
};