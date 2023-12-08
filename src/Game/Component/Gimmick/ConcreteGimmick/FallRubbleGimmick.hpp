#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CFallRubbleGimmickMove;
class CModuleManager;


class CFallRubbleGimmick final : public CGimmick
{
private:
    enum STATE
    {
        STATE_FALL = 0,
        STATE_ERASE,
    };
    
    enum KIND
    {
        KIND_ROCK = 0,
        KIND_HUN_STEEL_ONE,
        KIND_HUN_STEEL_TWO,
        
        KINDNUM,
        
        KIND_UNKNOWN = -1,
    };

    struct INITPARAM
    {
        const char* m_pszName;
        float m_fRefrectionCoefficient;
    };

public:
    CFallRubbleGimmick(const char* pszName, void* pParam);
    virtual ~CFallRubbleGimmick(void);
    virtual void Run(void) override;
    virtual void Draw(void) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnTouchedDown(void) override;

private:
    bool decreaseMaterialAlpha(void);
    void setHitAttack(void);
    void onOnGround(void);

private:
    static const INITPARAM m_aInitParam[];
    CNormalGimmickModel* m_pModel;
    CFallRubbleGimmickMove* m_pFallRubbleMove;
    CModuleManager* m_pModuleMan;
    int32 m_nDamage;
    RwV3d m_vPreMovePosition;
    STATE m_eState;
    KIND m_eKind;
    float m_fAliveTime;
    bool m_bErase;
};