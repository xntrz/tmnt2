#include "RidePlayer.hpp"
#include "RideCharacter.hpp"
#include "RideManipulator.hpp"


static const char* s_apszRidePlayerClassName[] =
{
    "ride_player_1",
    "ride_player_2",
    "ride_player_3",
    "ride_player_4",
};


/*static*/ CRidePlayer* CRidePlayer::New(int32 nNo, int32 nControllerNo)
{
    ASSERT(nNo >= 0 && nNo < COUNT_OF(s_apszRidePlayerClassName));
    
    CRidePlayer* pRidePlayer = new CRidePlayer(s_apszRidePlayerClassName[nNo], nNo, nControllerNo);
    ASSERT(pRidePlayer);

    return pRidePlayer;
};


/*static*/ void CRidePlayer::Delete(CRidePlayer* pPlayer)
{
    ASSERT(pPlayer);
    delete pPlayer;
};


CRidePlayer::CRidePlayer(const char* pszName, int32 nNo, int32 nControllerNo)
: CPlayer(pszName, nNo, nControllerNo)
{
    ;
};


CRidePlayer::~CRidePlayer(void)
{
    for (int32 i = 0; i < m_nNumCharacter; ++i)
    {
        CHARACTERINFO* pCharacterInfo = &m_aCharacterInfo[i];

        ASSERT(pCharacterInfo->m_pManipulator);
        ASSERT(pCharacterInfo->m_pCharacter);

        CRideManipulator::Delete((CRideManipulator*)pCharacterInfo->m_pManipulator);
        pCharacterInfo->m_pManipulator = nullptr;

        CRideCharacter::Delete((CRideCharacter*)pCharacterInfo->m_pCharacter);
        pCharacterInfo->m_pCharacter = nullptr;
    };

    m_nNumCharacter = 0;
};


void CRidePlayer::AddCharacter(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume)
{
    ASSERT(m_nNumCharacter >= 0 && m_nNumCharacter < COUNT_OF(m_aCharacterInfo));
    ASSERT(idChr >= PLAYERID::ID_START && idChr < PLAYERID::ID_MAX);

    CHARACTERINFO* pCharacterInfo = &m_aCharacterInfo[m_nNumCharacter];

    pCharacterInfo->m_pCharacter = CRideCharacter::New(idChr, this, costume);
    ASSERT(pCharacterInfo->m_pCharacter);
    pCharacterInfo->m_pCharacter->SetPlayerNo(m_nNo);
    pCharacterInfo->m_pCharacter->SetPadID(m_nControllerNo);

    pCharacterInfo->m_pManipulator = CRideManipulator::New((CRideCharacter*)pCharacterInfo->m_pCharacter, m_nControllerNo);
    ASSERT(pCharacterInfo->m_pManipulator);

    if (m_pCurrentCharacterInfo)
    {
        SetSleepCharacter(pCharacterInfo);
    }
    else
    {
        SetAwakeCharacter(pCharacterInfo);
        m_pCurrentCharacterInfo = pCharacterInfo;
    };

    ++m_nNumCharacter;
};


void CRidePlayer::SetCurrentCharacter(PLAYERID::VALUE idChr, bool bPlayChangeEffect)
{
    CHARACTERINFO* pCharacterInfo = SearchCharacter(idChr);

    ASSERT(pCharacterInfo);
    ASSERT(m_pCurrentCharacterInfo);

    CRideCharacter* pBeforeCharacter = (CRideCharacter*)m_pCurrentCharacterInfo->m_pCharacter;
    CRideCharacter* pAfterCharacter = (CRideCharacter*)pCharacterInfo->m_pCharacter;

    SetSleepCharacter(m_pCurrentCharacterInfo);
    SetAwakeCharacter(pCharacterInfo);

    pBeforeCharacter->OnDetach(pAfterCharacter);
    pAfterCharacter->OnAttach(pBeforeCharacter, bPlayChangeEffect);

    m_pCurrentCharacterInfo = pCharacterInfo;
};


int32 CRidePlayer::GetScore(RIDETYPES::SCOREKIND scorekind)
{
    int32 nScore = 0;
    
    for (int32 i = 0; i < GetCharacterNum(); ++i)
    {
        CRideCharacter* pRideChr = GetRideCharacter(i);
        ASSERT(pRideChr);

        nScore += pRideChr->GetScore(scorekind);
    };

    return nScore;
};


CRideCharacter* CRidePlayer::GetRideCharacter(int32 no) const
{
    ASSERT(no >= 0 && no < GetCharacterNum());
    return (CRideCharacter*)GetCharacter(no);
};