#include "GeneratorGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"


CEnemySettingGimmick::CEnemySettingGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_mode(MODE_GROUND)
, m_bAutoActivate(false)
, m_move()
, m_generator()
{
    GIMMICKPARAM::GIMMICK_ENEMY_PLACE* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_PLACE*>(pParam);

    m_bAutoActivate = ((pInitParam->m_subid == 0) ||
                       (pInitParam->m_subid == 1));

    m_mode = (pInitParam->m_subid == 2 ? MODE_AIR : MODE_GROUND);

    //
    //  init generator
    //
    int32 nEnemyNum = 1;

    if ((pInitParam->m_subid == 1) ||
        (pInitParam->m_subid == 4))
    {
        if (CGameData::PlayParam().GetPlayerNum() >= (GAMETYPES::PLAYERS_MAX - 1))
            nEnemyNum *= 2;
    };
    
    m_generator.Init(pInitParam, nEnemyNum, m_bAutoActivate);

    if (m_bAutoActivate || (m_mode == MODE_AIR))
        m_generator.Start();

    //
    //  init move
    //
    m_move.SetPosition(&pInitParam->m_vPosition);

    //
    //  setup movement strat
    //
    SetMoveStrategy(&m_move);
};


CEnemySettingGimmick::~CEnemySettingGimmick(void)
{
    ;
};


bool CEnemySettingGimmick::Query(CGimmickQuery* pQuery) const
{
    if (m_generator.HandleQuery(pQuery))
        return true;

    return CGimmick::Query(pQuery);
};


void CEnemySettingGimmick::PreMove(void)
{
    float dt = CGameProperty::GetElapsedTime();

    CEnemyGeneratorBase::STATE state = m_generator.Run(dt);
    switch (state)
    {
    case CEnemyGeneratorBase::STATE_GENERATE_ENEMY:
        {
            if (m_mode == MODE_GROUND)
                m_generator.ActivateEnemy();
        }
        break;

    default:
        break;
    };
};


void CEnemySettingGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        switch (m_mode)
        {
        case MODE_GROUND:
            {
                if (!m_bAutoActivate)
                    m_generator.Start();

                m_generator.HandleEvent(pszSender, eventtype);
            }
            break;

        case MODE_AIR:
            {
                ASSERT(m_generator.CountAliveEnemy() > 0);
                m_generator.ActivateEnemy();
            }
            break;

        default:
            break;
        };
    }
    else
    {
        m_generator.HandleEvent(pszSender, eventtype);
    };
};


//
// *********************************************************************************
//


CEnemyGeneratePointGimmick::CEnemyGeneratePointGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_mode(MODE_GROUND)
, m_bAutoActivate(false)
, m_move()
, m_generator()
{
    GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_SINGLE* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_SINGLE*>(pParam);

    m_bAutoActivate = (pInitParam->m_subid ? false : true);
    m_mode          = (pInitParam->m_id == GIMMICKID::ID_G_AIR ? MODE_AIR : MODE_GROUND);

    //
    //  init generator
    //
    m_generator.Init(pInitParam, m_bAutoActivate);

    if (m_bAutoActivate)
        m_generator.Start();
    
    //
    //  init move
    //    
    m_move.SetPosition(&pInitParam->m_vPosition);

    //
    //  setup movement strat
    //
    SetMoveStrategy(&m_move);
};


CEnemyGeneratePointGimmick::~CEnemyGeneratePointGimmick(void)
{
    ;
};


bool CEnemyGeneratePointGimmick::Query(CGimmickQuery* pQuery) const
{
    if (m_generator.HandleQuery(pQuery))
        return true;

    return CGimmick::Query(pQuery);
};


void CEnemyGeneratePointGimmick::PreMove(void)
{
    m_generator.Run(CGameProperty::GetElapsedTime());
};


void CEnemyGeneratePointGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (m_bAutoActivate)
    {
        m_generator.HandleEvent(pszSender, eventtype);
        return;
    };

    switch (eventtype)
    {
    case GIMMICKTYPES::EVENTTYPE_ACTIVATE:
        m_generator.Start();
        break;

    case GIMMICKTYPES::EVENTTYPE_INACTIVATE:
        m_generator.Stop();
        break;

    default:
        break;
    };

    m_generator.HandleEvent(pszSender, eventtype);
};