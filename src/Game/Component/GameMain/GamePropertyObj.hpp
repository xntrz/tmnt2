#pragma once

#include "System/Common/Screen.hpp"


class CMapCamera;
class CGamePlayerContainer;
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
    void SetRwCamera(RwCamera* pRwCamera);
    void SetMapCamera(CMapCamera* pMapCamera);
    float GetTotalElapsedTime(void) const;
    float GetElapsedTime(void) const;
    float GetCleartime(void) const;
    float GetSleepDistance(void) const;
    float GetDeathHeight(void) const;
    float GetGravity(void) const;
    RwCamera* GetRwCamera(void) const;
    CMapCamera* GetMapCamera(void) const;
    void GetCameraViewMatrix(RwMatrix* pMatrix) const;
    void GetCameraFrameMatrix(RwMatrix* pMatrix) const;
    CGamePlayerContainer& PlayerContainer(void) const;
    CGameEnemyContainer& EnemyContainer(void) const;
    CGameGimmickContainer& GimmickContainer(void) const;

protected:
    void updateTime(void);
    void updateActiveObject(void);
    void watchPlayerFallen(void);
    
protected:
    float m_fTotalElapsedTime;
    float m_fCleartime;
    float m_fSleepDistance;
    float m_fDeathHeight;
    float m_fGravity;
    RwCamera* m_pRwCamera;
    CMapCamera* m_pMapCamera;
    CGamePlayerContainer* m_pPlayerContainer;
    CGameEnemyContainer* m_pEnemyContainer;
    CGameGimmickContainer* m_pGimmickContainer;
};


#include "GamePropertyObj.inl"