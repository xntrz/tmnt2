#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GeneratorObj.hpp"


class CEnemySettingGimmick final : public CGimmick
{
private:
    enum MODE
    {
        MODE_GROUND = 0,
        MODE_AIR,
    };
    
public:
    CEnemySettingGimmick(const char* pszName, void* pParam);
    virtual ~CEnemySettingGimmick(void);
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PreMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    
private:
    MODE m_mode;
    bool m_bAutoActivate;
    CGimmickMove m_move;
    CSettingEnemyGenerator m_generator;
};


class CEnemyGeneratePointGimmick final : public CGimmick
{
private:
    enum MODE
    {
        MODE_GROUND = 0,
        MODE_AIR,
    };

public:
    CEnemyGeneratePointGimmick(const char* pszName, void* pParam);
    virtual ~CEnemyGeneratePointGimmick(void);
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PreMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    MODE m_mode;
    bool m_bAutoActivate;
    CGimmickMove m_move;
    CSingleEnemyGenerator m_generator;
};