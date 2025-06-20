#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CGuillotineGimmickMove;
class CModuleManager;


class CGuillotineGimmick final : public CGimmick
{
public:
    enum KIND
    {
        KIND_GUILLOTINE = 0,
        KIND_CRANE,

        KINDNUM,
    };

    struct PENDULUMPARAM
    {
        char  m_szName[12];
        float m_fHitRadius;
        RwV3d m_vHitPos;
    };

public:
    CGuillotineGimmick(const char* pszName, void* pParam);
    virtual ~CGuillotineGimmick(void);
    virtual void Run(void) override;
    virtual void Draw(void) const override;
    virtual void PreMove(void) override;
    virtual void PostMove(void) override;
    virtual void OnTouchedDown(void) override;

private:
    void setHitAttack(void);
    void setHitAttackChain(void);
    void setHitAttackCrane(void);
    void setHitAttackGuillotine(void);
    void registHitAttack(const RwSphere* pHitSphere, int32 No);

private:
    CNormalGimmickModel     m_model;
    CModuleManager*         m_pModuleMan;
    CGuillotineGimmickMove* m_pGuillotineMove;
    RwV3d                   m_vFulcrumVector;
    RwV3d                   m_vPrePos;
    RwV3d                   m_vHitSpherePos;
    float                   m_fHitRadius;
    KIND                    m_kind;
};