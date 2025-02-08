#include "GamePlayerContainer.hpp"
#include "GamePlayer.hpp"


CGamePlayerContainer::CGamePlayerContainer(void)
: m_pGamePlayerTable(nullptr)
, m_numPlayer(0)
{
    CGamePlayer::AttachContainer(this);
    CGamePlayer::ClearDamageNegation();
    
    m_pGamePlayerTable = new CGamePlayer[GAMETYPES::PLAYERS_MAX];
};


CGamePlayerContainer::~CGamePlayerContainer(void)
{
    if (m_pGamePlayerTable)
    {
        delete[] m_pGamePlayerTable;
        m_pGamePlayerTable = nullptr;
    };

    m_numPlayer = 0;

    CGamePlayer::DetachContainer();
};


IGamePlayer* CGamePlayerContainer::GamePlayer(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < m_numPlayer);

    if ((no >= 0) && (no < m_numPlayer))
    {
        CGamePlayer* pGameplayer = getGamePlayer(no);
        return pGameplayer;
    };

    return CGamePlayer::Dummy();
};


int32 CGamePlayerContainer::GetPlayerNum(void) const
{
    return m_numPlayer;
};


void CGamePlayerContainer::AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    ASSERT(nPlayerNo >= 0);
    ASSERT(nPlayerNo < GAMETYPES::PLAYERS_MAX);

    CGamePlayer* pGameplayer = getGamePlayer(nPlayerNo);
    ASSERT(pGameplayer != nullptr);

    if (!pGameplayer->IsAlive())
    {
        pGameplayer->CreatePlayer(nPlayerNo);
        ++m_numPlayer;
    };

    pGameplayer->AddCharacter(idPlayer, costume);
};


void CGamePlayerContainer::Remove(IGamePlayer* pGameplayer)
{
    ;
};


CGamePlayer* CGamePlayerContainer::getGamePlayer(int32 no) const
{
    ASSERT(no >= 0);
    ASSERT(no < GAMETYPES::PLAYERS_MAX);
    ASSERT(m_pGamePlayerTable != nullptr);

    return &m_pGamePlayerTable[no];
};