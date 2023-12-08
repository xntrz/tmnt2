#include "GamePlayerContainer.hpp"
#include "GamePlayer.hpp"


CGamePlayerContainer::CGamePlayerContainer(void)
: m_pGamePlayerTable(nullptr)
, m_nNumPlayer(0)
{
    CGamePlayer::AttachContainer(this);
    CGamePlayer::ClearDamageNegation();
    
    m_pGamePlayerTable = new CGamePlayer[GAMETYPES::PLAYERS_MAX];
    ASSERT(m_pGamePlayerTable);
};


CGamePlayerContainer::~CGamePlayerContainer(void)
{
    if (m_pGamePlayerTable)
    {
        delete[] m_pGamePlayerTable;
        m_pGamePlayerTable = nullptr;
    };

    m_nNumPlayer = 0;

    CGamePlayer::DetachContainer();
};


CGamePlayer* CGamePlayerContainer::GamePlayer(int32 no) const
{
    return ((no >= 0 && no < m_nNumPlayer) ? getGamePlayer(no) : CGamePlayer::Dummy());
};


int32 CGamePlayerContainer::GetPlayerNum(void) const
{
    return m_nNumPlayer;
};


void CGamePlayerContainer::AddPlayer(int32 nPlayerNo, PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
{
    ASSERT(nPlayerNo >= 0 && nPlayerNo < GAMETYPES::PLAYERS_MAX);

    CGamePlayer* pGamePlayer = getGamePlayer(nPlayerNo);
    if(!pGamePlayer->IsAlive())
    {
        pGamePlayer->CreatePlayer(nPlayerNo);
        ++m_nNumPlayer;
    };

    pGamePlayer->AddCharacter(idPlayer, costume);
};


void CGamePlayerContainer::Remove(CGamePlayer* pGamePlayer)
{
    (void)pGamePlayer;    
};


CGamePlayer* CGamePlayerContainer::getGamePlayer(int32 no) const
{
    ASSERT(no >= 0 && no < GAMETYPES::PLAYERS_MAX);
    return &m_pGamePlayerTable[no];
};