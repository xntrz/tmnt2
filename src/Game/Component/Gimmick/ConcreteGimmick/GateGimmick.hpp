#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"

#include "Game/Component/Effect/EffectID.hpp"


class CModel;


class CGateGimmick final : public CGimmick
{
private:
    struct EFFECT
    {
        RwV3d m_vEffectPos;
        float m_fTimer;
        float m_fInterval;
        bool m_bBelongToGate;
        CModel* m_pModel;
        float m_fScale;
        EFFECTID::VALUE m_effectId;
    };

    enum STATE
    {
        STATE_WAIT = 0,
        STATE_BEFORE_CAMERA_VIB,
        STATE_GO_DOWN,
        STATE_AFTER_CAMERA_VIB,
        STATE_END,
    };

public:
    CGateGimmick(const char* pszName, void* pParam);
    virtual ~CGateGimmick(void);
    virtual void Run(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    void cameraVibCtrl(void);
    void gateMoveCtrl(void);
    void gateEffectCtrl(void);
    void cameraVibRq(float fPower, float fTime, int32 nFreq);

private:
    CNormalGimmickModel m_model;
    uint32 m_hAtari;
    float m_fTimer;
    bool m_bStart;
    bool m_bMoveEnd;
    STATE m_state;
    EFFECT m_aEffect[7];
};