#pragma once


class CGameEnemyContainer;
class CMapCamera;


class CGameRadar
{
public:
    static void Initialize(float fRadius);
    static void Terminate(void);
    static void Update(const CGameEnemyContainer* pEnemyContainer, CMapCamera* pMapCamera);
    static void Reset(void);
    static void SetFindEnemyInfo(RwV2d, float);
    static void Draw(void);
};