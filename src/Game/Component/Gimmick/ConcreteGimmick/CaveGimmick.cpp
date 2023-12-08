#include "CaveGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"


CCaveGimmick::CCaveGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_generator()
, m_status(STATUS_IDLE)
{
    GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE*>(pParam);

    m_generator.Init(pInitParam, true);
};


CCaveGimmick::~CCaveGimmick(void)
{

};


bool CCaveGimmick::Query(CGimmickQuery* pQuery) const
{
    if (m_generator.HandleQuery(pQuery))
        return true;

    return CGimmick::Query(pQuery);
};


void CCaveGimmick::PreMove(void)
{
    if (m_status == STATUS_RUN)
    {
        if (m_generator.Run(CGameProperty::GetElapsedTime()) == CMultipleEnemyGenerator::STATE_END)
            m_status = STATUS_END;
    };
};


void CCaveGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_status)
        {
            m_generator.Finish();
            m_status = STATUS_END;
        }
        else
        {
            m_generator.Start();
            m_status = STATUS_RUN;
        };

    }
    else
    {
        m_generator.HandleEvent(pszSender, eventtype);
    };
};