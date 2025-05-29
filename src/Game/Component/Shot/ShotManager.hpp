#pragma once

#include "ShotID.hpp"

#include "Game/Component/Effect/MagicID.hpp"


class CGameObject;


class CShotManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Draw(void);
    static void Period(void);
    static uint32 Shot(SHOTID::VALUE idShot, const RwV3d* pvPos, float fDirection, CGameObject* pObject, float fRadian = MATH_DEG2RAD(15.0f), float fLife = 5.0f);
    static uint32 Shot(SHOTID::VALUE idShot, const RwV3d* pvPos, const RwV3d* pvVec, CGameObject* pObject, float fRadian = MATH_DEG2RAD(15.0f), float fLife = 5.0f);
    static uint32 Shot(MAGICID::VALUE idMagic, const RwV3d* pvPos, const RwV3d* pvVec, CGameObject* pObject, float fRadian);
    static bool IsExist(SHOTID::VALUE idShot);
    static void Finish(uint32 hShot);    
};