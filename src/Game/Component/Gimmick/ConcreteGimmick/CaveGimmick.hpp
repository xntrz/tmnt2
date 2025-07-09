#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/Utils/GeneratorObj.hpp"


class CCaveGimmick final : public CGimmick
{
private:
    enum STATUS
    {
        STATUS_IDLE = 0,
        STATUS_RUN,
        STATUS_STOP,
        STATUS_END,
    };
    
public:
    CCaveGimmick(const char* pszName, void* pParam);
    virtual ~CCaveGimmick(void);
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void PreMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

private:
    CMultipleEnemyGenerator m_generator;
    STATUS                  m_status;
};