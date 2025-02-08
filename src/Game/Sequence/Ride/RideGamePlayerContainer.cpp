#include "RideGamePlayerContainer.hpp"
#include "RideGamePlayer.hpp"


CRideGamePlayerContainer::CRideGamePlayerContainer(void)
: m_pRideGamePlayerTable(nullptr)
, m_numPlayer(0)
{
    m_pRideGamePlayerTable = new CRideGamePlayer[GAMETYPES::PLAYERS_MAX];
};


CRideGamePlayerContainer::~CRideGamePlayerContainer(void)
{
    if (m_pRideGamePlayerTable)
    {
        delete[] m_pRideGamePlayerTable;
        m_pRideGamePlayerTable = nullptr;
    };

    m_numPlayer = 0;
};


IGamePlayer* CRideGamePlayerContainer::GamePlayer(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < m_numPlayer);

    if ((no >= 0) && (no < m_numPlayer))
    {
        CRideGamePlayer* pRideGameplayer = getRideGamePlayer(no);
        return pRideGameplayer;
    };
    
    return CGamePlayer::Dummy();
};


int32 CRideGamePlayerContainer::GetPlayerNum(void) const
{
    return m_numPlayer;
};


void CRideGamePlayerContainer::AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    ASSERT(nPlayerNo >= 0);
    ASSERT(nPlayerNo < GAMETYPES::PLAYERS_MAX);

    CRideGamePlayer* pRideGameplayer = getRideGamePlayer(nPlayerNo);
    ASSERT(pRideGameplayer != nullptr);

    if (!pRideGameplayer->IsAlive())
    {
        pRideGameplayer->CreatePlayer(nPlayerNo);
        ++m_numPlayer;
    };

    pRideGameplayer->AddPlayerCharacter(idPlayer, costume);
};


void CRideGamePlayerContainer::Remove(IGamePlayer* pGameplayer)
{
    ;
};


CRideGamePlayer* CRideGamePlayerContainer::getRideGamePlayer(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < GAMETYPES::PLAYERS_MAX);
    ASSERT(m_pRideGamePlayerTable != nullptr);

    return &m_pRideGamePlayerTable[no];
};