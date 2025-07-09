#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/MoveStrategy/CrystalGimmickMove.hpp"

#include "Game/Component/Effect/EffectID.hpp"


class CModuleManager;
class CCrystalGimmickMove;


class CCrystalGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_NORMAL = 0,
        STATE_DESTROYED,
        STATE_END,
    };

    class CCrystalEffect
    {
    public:
        CCrystalEffect(void);
        ~CCrystalEffect(void);
        void Start(EFFECTID::VALUE id, const RwV3d* pvPos, float fScale);
        void End(void);
        void Run(void);
        
    private:        
        uint32 m_hEffect;
        float m_fTimer;
        float m_fTargetTime;
    };
    
public:
    CCrystalGimmick(const char* pszName, void* pParam);
    virtual ~CCrystalGimmick(void);
    virtual void Draw(void) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnCatchAttack(CHitAttackData* pAttack) override;
    void crystalCtrl(void);
    void setHitAttack(void);
    bool isValidAttack(CHitAttackData* pAttack);

private:
    CNormalGimmickModel  m_model;    
    CCrystalGimmickMove* m_pCryMove;
    CModuleManager*      m_pModuleMan;
    char                 m_szTargetName[16];
    STATE                m_state;
    int32                m_nLife;
    float                m_fTimer;
    int32                m_nLaserPower;
    CCrystalEffect       m_effectObj;
};