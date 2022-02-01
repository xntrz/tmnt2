#include "GamePlayerContainer.hpp"
#include "GamePlayer.hpp"


CGamePlayerContainer::CGamePlayerContainer(void)
: m_pGamePlayerTable(nullptr)
, m_nNumPlayer(0)
{
    m_pGamePlayerTable = new CGamePlayer[GAMETYPES::PLAYERS_MAX];
    ASSERT(m_pGamePlayerTable);

    CGamePlayer::ClearDamageNegation();
};


CGamePlayerContainer::~CGamePlayerContainer(void)
{
    if (m_pGamePlayerTable)
    {
        delete[] m_pGamePlayerTable;
        m_pGamePlayerTable = nullptr;
    };

    m_nNumPlayer = 0;
};


void CGamePlayerContainer::AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    ASSERT(nPlayerNo >= 0 && nPlayerNo < GAMETYPES::PLAYERS_MAX);

    if (!getGamePlayer(nPlayerNo)->IsAlive())
    {
        getGamePlayer(nPlayerNo)->CreatePlayer(nPlayerNo);
        ++m_nNumPlayer;
    };

    getGamePlayer(nPlayerNo)->AddPlayerCharacter(idPlayer, costume);
};


IGamePlayer& CGamePlayerContainer::GamePlayer(int32 no) const
{
	if (no >= 0 && no < m_nNumPlayer)
		return *getGamePlayer(no);
    else
        return CGamePlayer::Dummy();
};


int32 CGamePlayerContainer::GetPlayerNum(void) const
{
    return m_nNumPlayer;
};


CGamePlayer* CGamePlayerContainer::getGamePlayer(int32 no) const
{
    ASSERT(no >= 0 && no < GAMETYPES::PLAYERS_MAX);
    return &m_pGamePlayerTable[no];
};