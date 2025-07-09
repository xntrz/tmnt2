#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CPushingGimmickMove;
class CBodyHitData;


class CPushingGimmick : public CGimmick
{
protected:
    struct KINDINFO
    {
        const char* m_pszModelName;
        const char* m_pszModelNameAtari;
        float m_fRadius;
        float m_fCollisionRadius;
        float m_fSpeed;
        int32 m_nSE;
    };
    
public:
    CPushingGimmick(const char* pszName, void* pParam);
    virtual ~CPushingGimmick(void);
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    bool IsBig(void) const;

protected:
	void init(void* pParam);
	void startSE(void);
	void stopSE(void);
    const KINDINFO& kindinfo(void) const;

protected:
    static const KINDINFO   m_aPushingGimmickKindInfoList[];
    CPushingGimmickMove*    m_pPushingMove;
    CNormalGimmickModel     m_model;
    RwV3d                   m_vPrePos;
    float                   m_fEffectTimer;
    bool                    m_bSE;
    int32                   m_subid;
    uint32                  m_hAtari;
    CBodyHitData*           m_pBodyHitData;
    float                   m_fInvincibleTimer;
    int32                   m_iLife;
};