#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/Enemy/EnemyID.hpp"


class CEnemyCommonLoadInfo final : public CGameObjLoadInfo
{
public:
    CEnemyCommonLoadInfo(void) {};
    virtual ~CEnemyCommonLoadInfo(void) {};
    virtual bool MakeFileName(char* pszFilenameBuff) const override;
};


class CEnemyLoadInfo final : public CGameObjLoadInfo
{
public:
    CEnemyLoadInfo(ENEMYID::VALUE enemyID);
    virtual ~CEnemyLoadInfo(void) {};
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    ENEMYID::VALUE m_enemyID;
};