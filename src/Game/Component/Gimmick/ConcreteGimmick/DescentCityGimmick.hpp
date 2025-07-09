#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CDescentCityGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_RUN,
        STATE_END,        
    };
    
public:
    CDescentCityGimmick(const char* pszName, void* pParam);
    virtual ~CDescentCityGimmick(void);
    virtual void GetCenterPosition(RwV3d* pvPosition) const override;
    virtual void Draw(void) const override;
    virtual void PostMove(void) override;
    void allClear(void);
    void initParam(void);
    void initModel(void);
    void uvAnimUpdate(void);
    void cityMoveControl(void);
    void cityRise(void);
    void cityRotation(void);
    void timerCount(void);

protected:
    CNormalGimmickModel* m_apModel[4];
    void*                m_ahUVAnim[4];
    STATE                m_state;
    float                m_fElapsedTime;
    float                m_fAddHeightAtFrame;
    float                m_fAddRotateAtFrame;
    RwV3d                m_vPosition;
    RwV3d                m_vRotation;
    float                m_fAccel;
    float                m_fFirstSpeed;
    float                m_fLastSpeed;
    float                m_fSlowDownPoint;
};