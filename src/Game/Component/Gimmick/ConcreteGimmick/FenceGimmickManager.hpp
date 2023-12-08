#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"


class CFenceGimmickManager final : public CGimmick
{
public:
    struct ATTACKERINFO
    {
        float m_fBrokenTime;
        int32 m_nPlayerNo;
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void IntrudeObservation(void);
    static bool DispatchInformation(CHitAttackData* pAttack);
    static int32 GetLastAttacker(void);
    static const ATTACKERINFO* GetInfo(int32 nPlayerNo);
    static ATTACKERINFO* AllocNewFenceInfo(void);

    CFenceGimmickManager(const char* pszName, void* pParam);
    virtual ~CFenceGimmickManager(void);
    virtual void Run(void) override;
    virtual void Draw(void) const override;

private:
    static CFenceGimmickManager* m_pInstance;
    static ATTACKERINFO* m_paPlayerInfo[GAMETYPES::PLAYERS_MAX];
};