#pragma once

#include "EffectID.hpp"


class CTracer;
class CEffect;
class CMagicParameter;


class CEffectManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(RwCamera* pRwCamera);
    static void Read(const char* pszName, const void* pBuffer, uint32 uBufferSize);
    static CEffect* GetEffectFromPool(const char* pszName);
    static uint32 Play(EFFECTID::VALUE idEffect, const RwV3d* pvPosition, bool bPlaySound = true);
    static uint32 Play(EFFECTID::VALUE idEffect, const RwV3d* pvPosition, float fDirection, bool bPlaySound = true);
    static uint32 Play(const char* pszName, const RwV3d* pvPosition, bool bPlaySound = true);
    static uint32 Play(const char* pszName, const RwV3d* pvPosition, float fDirection, bool bPlaySound = true);
    static uint32 PlayTrace(EFFECTID::VALUE idEffect, CTracer* pTracer, const RwV3d* pvOffset, bool bPlaySound = true);
    static uint32 PlayTrace(EFFECTID::VALUE idEffect, CTracer* pTracer, const RwV3d* pvOffset, float fDirection, bool bPlaySound = true);
    static uint32 PlayTrace(const char* pszName, CTracer* pTracer, const RwV3d* pvOffset, bool bPlaySound = true);
    static uint32 PlayTrace(const char* pszName, CTracer* pTracer, const RwV3d* pvOffset, float fDirection, bool bPlaySound = true);
    static void ConvertWithDestroy(const char* pszName, CMagicParameter* pMagicParameter);
    static void Convert(const char* pszName, CMagicParameter* pMagicParameter);
    static void SetActive(uint32 hEffect, bool bActive);
    static void Finish(uint32 hEffect);
    static void End(uint32 hEffect);
    static bool IsEnd(uint32 hEffect);
    static void SetPosition(uint32 hEffect, const RwV3d* pvPosition);
    static void SetScale(uint32 hEffect, float fScale);
    static void SetDirection(uint32 hEffect, RwMatrix* pMatrix);
    static void SetDirection(uint32 hEffect, const RwV3d* pvDirection);
    static void SetDirection(uint32 hEffect, float fDirection);
    static EFFECTID::VALUE GetEffectID(uint32 hEffect);
};