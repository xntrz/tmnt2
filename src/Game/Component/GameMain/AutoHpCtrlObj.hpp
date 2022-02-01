#pragma once

#include "Game/System/GameObject/GameObject.hpp"


class CAutoHpCtrlObj : public CGameObject
{
public:
    enum MODE
    {
        MODE_DAMAGE = 0,
        MODE_RECOVER,
    };
    
public:
    CAutoHpCtrlObj(MODE mode);
    virtual ~CAutoHpCtrlObj(void);
    virtual void Run(void) override;
    
private:
    MODE m_mode;
    float m_fSpeed;
    float m_fValue;
};