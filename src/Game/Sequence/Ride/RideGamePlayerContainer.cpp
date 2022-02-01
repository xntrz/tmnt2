#include "RideGamePlayerContainer.hpp"
#include "RideGamePlayer.hpp"


CRideGamePlayerContainer::CRideGamePlayerContainer(void)
: m_pRideGamePlayerTable(nullptr)
, m_nNumPlayer(0)
{
    m_pRideGamePlayerTable = new CRideGamePlayer[GAMETYPES::PLAYERS_MAX];
    ASSERT(m_pRideGamePlayerTable);
};


CRideGamePlayerContainer::~CRideGamePlayerContainer(void)
{
    if (m_pRideGamePlayerTable)
    {
        delete[] m_pRideGamePlayerTable;
        m_pRideGamePlayerTable = nullptr;
    };

    m_nNumPlayer = 0;
};


void CRideGamePlayerContainer::AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    ASSERT(nPlayerNo >= 0 && nPlayerNo < GAMETYPES::PLAYERS_MAX);

    if (!getRideGamePlayer(nPlayerNo)->IsAlive())
    {
        getRideGamePlayer(nPlayerNo)->CreatePlayer(nPlayerNo);
        ++m_nNumPlayer;
    };

    getRideGamePlayer(nPlayerNo)->AddPlayerCharacter(idPlayer, costume);
};


IGamePlayer& CRideGamePlayerContainer::GamePlayer(int32 no) const
{
    if (no >= 0 && no < m_nNumPlayer)
        return *getRideGamePlayer(no);
    else
        return CRideGamePlayer::Dummy();
};


int32 CRideGamePlayerContainer::GetPlayerNum(void) const
{
    return m_nNumPlayer;
};


CRideGamePlayer* CRideGamePlayerContainer::getRideGamePlayer(int32 no) const
{
    ASSERT(no >= 0 && no < GAMETYPES::PLAYERS_MAX);
    return &m_pRideGamePlayerTable[no];
};