#pragma once

#include "System/Common/Screen.hpp"


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
	
	/*inline*/ void SetRwCamera(RwCamera* pRwCamera);
    /*inline*/ void SetMapCamera(CMapCamera* pMapCamera);
    /*inline*/ float GetTotalElapsedTime(void) const;
    /*inline*/ float GetElapsedTime(void) const;
    /*inline*/ float GetCleartime(void) const;
    /*inline*/ float GetSleepDistance(void) const;
    /*inline*/ float GetDeathHeight(void) const;
    /*inline*/ float GetGravity(void) const;
    /*inline*/ RwCamera* GetRwCamera(void) const;
    /*inline*/ CMapCamera* GetMapCamera(void) const;
    /*inline*/ void GetCameraViewMatrix(RwMatrix* pMatrix) const;
    /*inline*/ void GetCameraFrameMatrix(RwMatrix* pMatrix) const;
    /*inline*/ IGamePlayerContainer& PlayerContainer(void) const;
    /*inline*/ CGameEnemyContainer& EnemyContainer(void) const;
    /*inline*/ CGameGimmickContainer& GimmickContainer(void) const;

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
    IGamePlayerContainer* m_pPlayerContainer;
    CGameEnemyContainer* m_pEnemyContainer;
    CGameGimmickContainer* m_pGimmickContainer;
};


#include "GamePropertyObj.inl"