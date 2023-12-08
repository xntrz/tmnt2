#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"
#include "Game/Component/Gimmick/Utils/GeneratorObj.hpp"


class CDoorGimmick final : public CGimmick
{
private:
    enum STATUS
    {
        STATUS_IDLE = 0,
        STATUS_OPENING,
        STATUS_OPENED,
        STATUS_CLOSING,
        STATUS_CLOSED,
        STATUS_END,
    };
    
public:
    CDoorGimmick(const char* pszName, void* pParam);
    virtual ~CDoorGimmick(void);
    virtual void Run(void) override;
    virtual void Draw(void) const;
    virtual bool Query(CGimmickQuery* pQuery) const;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype);

private:
    void forceGenerateEnd(void);
    void doorMoveOperate(void);

private:
    CDoorEnemyGenerator m_generator;
    STATUS m_status;
    float m_fMoveVolume;
    CModel* m_apModel[2];
    float m_fTimer;
    bool m_bEnd;
};