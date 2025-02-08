#include "RidePlayer.hpp"
#include "RideCharacter.hpp"


/*static*/ CRidePlayer* CRidePlayer::New(int32 nNo, int32 nControllerNo)
{
    static const char* s_apszRidePlayerClassName[] =
    {
        "ride_player_1",
        "ride_player_2",
        "ride_player_3",
        "ride_player_4",
    };

    ASSERT(nNo >= 0);
    ASSERT(nNo < COUNT_OF(s_apszRidePlayerClassName));

    CRidePlayer* pRidePlayer = new CRidePlayer(s_apszRidePlayerClassName[nNo], nNo, nControllerNo);
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

        ASSERT(pCharacterInfo->m_pCharacter);
        CRideCharacter::Delete(static_cast<CRideCharacter*>(pCharacterInfo->m_pCharacter));
        pCharacterInfo->m_pCharacter = nullptr;
    };

    m_nNumCharacter = 0;
};


void CRidePlayer::AddCharacter(PLAYERID::VALUE idChr, GAMETYPES::COSTUME costume)
{
    ASSERT(idChr >= PLAYERID::ID_START);
    ASSERT(idChr <  PLAYERID::ID_MAX);

    CRideCharacter* pRideChr = CRideCharacter::New(idChr, this, costume);
    ASSERT(pRideChr);
    pRideChr->SetPlayerNo(m_nNo);
    pRideChr->SetPadID(m_nControllerNo);

    ASSERT(m_nNumCharacter >= 0);
    ASSERT(m_nNumCharacter < COUNT_OF(m_aCharacterInfo));

    CHARACTERINFO* pCharacterInfo = &m_aCharacterInfo[m_nNumCharacter];
    pCharacterInfo->m_pCharacter = pRideChr;

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

    CRideCharacter* pBeforeCharacter = static_cast<CRideCharacter*>(m_pCurrentCharacterInfo->m_pCharacter);
    CRideCharacter* pAfterCharacter = static_cast<CRideCharacter*>(pCharacterInfo->m_pCharacter);

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
#ifdef _DEBUG    
    int32 characterNum = GetCharacterNum();
    
    ASSERT(no >= 0);
    ASSERT(no < characterNum);
#endif /* _DEBUG */   
    
    return static_cast<CRideCharacter*>(GetCharacter(no));
};