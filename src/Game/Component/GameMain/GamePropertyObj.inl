#include "GamePropertyObj.hpp"


inline void CGamePropertyObject::SetRwCamera(RwCamera* pRwCamera)
{
	m_pRwCamera = pRwCamera;
};


inline void CGamePropertyObject::SetMapCamera(CMapCamera* pMapCamera)
{
    m_pMapCamera = pMapCamera;
};


inline float CGamePropertyObject::GetTotalElapsedTime(void) const
{
    return m_fTotalElapsedTime;
};


inline float CGamePropertyObject::GetElapsedTime(void) const
{
    return CScreen::TimerStride();
};


inline float CGamePropertyObject::GetCleartime(void) const
{
    return m_fCleartime;
};


inline float CGamePropertyObject::GetSleepDistance(void) const
{
    return m_fSleepDistance;
};


inline float CGamePropertyObject::GetDeathHeight(void) const
{
    return m_fDeathHeight;
};


inline float CGamePropertyObject::GetGravity(void) const
{
    return m_fGravity;
};


inline RwCamera* CGamePropertyObject::GetRwCamera(void) const
{
    return m_pRwCamera;
};


inline CMapCamera* CGamePropertyObject::GetMapCamera(void) const
{
    return m_pMapCamera;
};


inline void CGamePropertyObject::GetCameraViewMatrix(RwMatrix* pMatrix) const
{
    std::memcpy(pMatrix, RwCameraGetViewMatrixMacro(m_pRwCamera), sizeof(RwMatrix));
};


inline void CGamePropertyObject::GetCameraFrameMatrix(RwMatrix* pMatrix) const
{
    std::memcpy(pMatrix, RwFrameGetMatrixMacro(RwCameraGetFrameMacro(m_pRwCamera)), sizeof(RwMatrix));
};


inline IGamePlayerContainer& CGamePropertyObject::PlayerContainer(void) const
{
    return *m_pPlayerContainer;
};


inline CGameEnemyContainer& CGamePropertyObject::EnemyContainer(void) const
{
    return *m_pEnemyContainer;
};


inline CGameGimmickContainer& CGamePropertyObject::GimmickContainer(void) const
{
    return *m_pGimmickContainer;
};