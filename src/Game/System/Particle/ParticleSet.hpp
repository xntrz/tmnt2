#pragma once

#include "PefInfo.hpp"


class CParticle;


class CParticleSet
{
public:
    static const int32 PARTICLEMAX = 10;
    
public:
    CParticleSet(void);
    ~CParticleSet(void);
    CParticleSet* Clone(void);
    void Initialize(void);
    void Read(const void* pBuffer, uint32 uBufferSize);
    void Reset(void);
    void Run(float dt);
    void Draw(RwCamera* pCamera);
    void SetGravity(float fGravity);
    void SetBasisPositionForAll(const RwV3d* pvPosition);
    void SetBasisPosition(const RwV3d* pvPosition);
    void SetStringEffectOn(bool bSet);
    void SetScale(float fScale);
    void SetVector(const RwV3d* pvAxisX, const RwV3d* pvAxisY, const RwV3d* pvAxisZ);
    void Play(void);
    void SetEmitter(bool bSet);
    void SetLoopPlay(bool bSet);
    bool IsEnd(void) const;
    bool IsDrawing(void) const;
    bool IsTrace(void) const;
    void ConvertEffectInfo(PEFINFO::EFFECTINFO* pEffectInfo);

private:
    char m_szEffectName[32];
    CParticle* m_apParticle[PARTICLEMAX];
    int32 m_nParticleNum;
    char m_szTexDictName[32];
    float m_fEffectPlayTime;
    bool m_bLoop;
    bool m_bTrace;
    bool m_bStringOn;
    PEFINFO::CLIPPINGTYPE m_clippingtype;
    float m_fDistance;
    float m_fEffectStartTime;
};