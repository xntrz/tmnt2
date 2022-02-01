#pragma once

#include "EffectID.hpp"

#include "Game/System/GameObject/GameObject.hpp"


class CTracer;
class CParticleSet;


class CEffect : public CGameObject
{
public:
    enum TYPE
    {
        TYPE_NORMAL = 0,
        TYPE_WITHHIT,        
    };
    
public:
    CEffect(const char* pszName);
    virtual ~CEffect(void);
    virtual void Run(void) override;
    CEffect* Clone(void);
    CParticleSet* CloneParticle(void);
    void ReadEffect(const void* pBuffer, uint32 uBufferSize);
    void Draw(RwCamera* pCamera);
    EFFECTID::VALUE GetID(void) const;
    void Start(void);
    void End(void);
    void Finish(void);
    void RunFinish(void);
    void SetDirection(const RwMatrix* pMatrix);
    void SetDirection(const RwV3d* pvDirection);
    void SetDirection(float fDirection);
    void SetScale(float fScale);
    void SetSoundPlay(bool bPlaySd);
    void SetTracer(CTracer* pTracer, const RwV3d* pvOffset, float fDirection);
    void SetDirectionTraceFlag(bool bTraceDir);
    void SetStringEffectOn(bool bSet);
    void SetActive(bool bSet);
    void SetPosition(const RwV3d* pvPosition);
    void GetPosition(RwV3d* pvPosition) const;
    bool GetPositionFromObject(RwV3d* pvPosition) const;
    float GetDirectionFromObject(void);
    bool IsEnd(void) const;
    TYPE GetEffectType(void) const;

protected:
    CParticleSet* m_pParticleSet;
    RwV3d m_vPosition;
    CTracer* m_pTracer;
    RwV3d m_vOffset;
    TYPE m_type;
    float m_fDirectionOffset;
    bool m_bTraceDirection;
    bool m_bPlay;
    bool m_bSoundPlay;
    bool m_bFinish;
    bool m_bFinishStart;
};