#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"


class CPipeGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_NONE = 0,
        STATE_WAIT,
        STATE_CUT,
        STATE_SPRAY,    
        STATE_END,
    };

    enum SUBID
    {
        SUBID_STEAM = 0,
        SUBID_FREEZEGAS,
    };

public:
    CPipeGimmick(const char* pszName, void* pParam);
    virtual ~CPipeGimmick(void);
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual GIMMICKTYPES::FEATURE GetFeatures(void) const override;
    virtual void PostMove(void) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;

protected:
    void init(void* pParam);
    void waiting(void);
    void cutting(void);
    void spray(void);
    void setSprayEffect(const RwV3d* pvPosition);

protected:
    RwV3d m_vPosition;
    RwV3d m_vDirection;
    float m_fTimer;
    int32 m_subid;
    STATE m_state;
    RwLine m_line;
    uint32 m_hMagic;
};