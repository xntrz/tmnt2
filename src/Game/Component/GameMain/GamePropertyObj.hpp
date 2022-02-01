#pragma once


class CMapCamera;
class IGamePlayerContainer;
class CGameEnemyContainer;
class CGameGimmickContainer;


class CGamePropertyObject
{
protected:
    static const float DEFAULT_GRIVITY;
    
public:
    CGamePropertyObject(void);
    virtual ~CGamePropertyObject(void);
    void Period(void);
    void UpdateTime(void);
    void UpdateActiveObject(void);
    void WatchPlayerFallen(void);
    IGamePlayerContainer& PlayerContainer(void) const;
    CGameEnemyContainer& EnemyContainer(void) const;
    CGameGimmickContainer& GimmickContainer(void) const;
    float GetTotalElapsedTime(void) const;
    float GetElapsedTime(void) const;
    float GetCleartime(void) const;
    float GetSleepDistance(void) const;
    float GetDeathHeight(void) const;
    float GetGravity(void) const;
    RwCamera* GetRwCamera(void) const;
    CMapCamera* GetMapCamera(void) const;
    void SetRwCamera(RwCamera* pRwCamera);
    void SetMapCamera(CMapCamera* pMapCamera);
    void GetCameraViewMatrix(RwMatrix* pMatrix);
    void GetCameraFrameMatrix(RwMatrix* pMatrix);
    
protected:
    float m_fTotalElapsedTime;
    float m_fCleartime;
    float m_fSleepDistance;
    float m_fDeathHeight;
    float m_fGravity;
    RwCamera* m_pRwCamera;
    CMapCamera* m_pMapCamera;
    IGamePlayerContainer* m_pPlayerContainer;
    CGameEnemyContainer* m_pEnemyContainer;
    CGameGimmickContainer* m_pGimmickContainer;
};