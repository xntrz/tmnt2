#pragma once

#include "MagicTypes.hpp"
#include "MagicID.hpp"
#include "Effect.hpp"


class CTracer;
class CLocus;
class CMagicParameter;
class CHitAttackData;


class CMagic : public CEffect
{
private:
    struct ATTACK
    {
        RwV3d m_vPosition;
        int32 m_nPower;
        int32 m_nAntiguard;
        int32 m_nStatus;
        float m_fStatusDuration;
        float m_fStatusParameter;
        float m_fFlyawayParameter;
        bool m_bConfusion;
    };

    enum LOCUSKIND
    {
        LOCUSKIND_VER = 0,
        LOCUSKIND_HOR,
        
        LOCUSKINDNUM,
    };

public:
    CMagic(const char* pszName);
    virtual ~CMagic(void);
    virtual void Run(void) override;
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    void CreateSubstance(const char* pszName);
    CMagic* Clone(void);
    void Draw(RwCamera* pCamera);
    void StartObject(void);
    void End(void);
    bool IsEnd(void) const;
    void Finish(void);
    void RunFinish(void);
    void SetParameter(CMagicParameter* pMagicParameter);
    void SetDamage(int32 nPower);
    void SetStatusTime(float fTime);
    void SetChangeStatus(int32 nStatus);
    void SetFlyawayXY(float x, float y);
    void SetMovement(MAGICTYPES::MOVEMENT* pMovement);
    void SetLive(float fLivetime);
    void SetParent(CGameObject* pParent);
    void SetTarget(CTracer* pTarget);
    void SetDirectionBase(const RwV3d* pvDirection);
    void SetDirection(const RwV3d* pvDirection);
    void SetDirection(const RwMatrix* pMatrix);
    void SetPosition(const RwV3d* pvPosition);
    void GetPosition(RwV3d* pvPosition) const;
    void SetSpeed(float fSpeed);
    void SetScale(float fScale);
    void Transition(void);
    bool CheckBody(void);
    bool CheckAttack(void);
    void Homing(void);
    void Reflect(RwV3d* pvNormal);
    void Appearing(RwV3d* pvPosition);
    void Reflecting(RwV3d* pvPosition);
    void Vanishing(RwV3d* pvPosition);
    void VanishingHorizontal(RwV3d* pvNormal, RwV3d* pvPosition);
    void VanishingSE(void);
    MAGICID::VALUE GetID(void) const;
    bool IsRivalAttack(CHitAttackData* pAttack, const CGameObject* pObject);
    uint32 GetParent(void) const;
    void SetFeature(MAGICTYPES::FEATURE feature);
    void AddFeature(MAGICTYPES::FEATURE feature);
    bool CheckFeature(MAGICTYPES::FEATURE feature) const;
    
private:
    MAGICTYPES::FEATURE m_feature;
    MAGICTYPES::COLLISIONBODY m_collisionBody;
    MAGICTYPES::COLLISIONATTACK m_collisionAttack;
    MAGICTYPES::MOVEMENT m_movement;
    MAGICTYPES::LOCUSINFO m_locusinfo;
    float m_fLivetime;
    float m_fNowTime;
    float m_fHitTimingStart;
    float m_fHitTimingEnd;
    int32 m_nReflectNumMax;
    int32 m_nMapHitNum;
    int32 m_nCharaHitNum;
    int32 m_nMapObjectHitNum;
    CGameObject* m_pParent;
    uint32 m_hParentObj;
    CTracer* m_pTarget;
    float m_fChangeSize;
    char m_szBaseEffectName[GAMEOBJECTTYPES::NAME_MAX];
    char m_szAppearEffectName[GAMEOBJECTTYPES::NAME_MAX];
    char m_szVanishEffectName[GAMEOBJECTTYPES::NAME_MAX];
    char m_szReflectEffectName[GAMEOBJECTTYPES::NAME_MAX];
    char m_szVanishMagicName[GAMEOBJECTTYPES::NAME_MAX];
    CLocus* m_apLocus[LOCUSKINDNUM];
    float m_fInitBodyRadius;
    float m_fInitAttackRadius;
    bool m_bFinish;
    bool m_bFinishStart;
};