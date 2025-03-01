#include "PlayerWatcher.hpp"
#include "AIUtils.hpp"
#include "EnemyUtils.hpp"
#include "EnemyCharacter.hpp"
#include "CharacterCompositor.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"


CPlayerStateWatcher::CPlayerStateWatcher(void)
{
    static_assert(COUNT_OF(m_aCurrentStateFlag) == GAMETYPES::PLAYERS_MAX, "update me");
    static_assert(COUNT_OF(m_aPrevStateFlag) == GAMETYPES::PLAYERS_MAX, "update me");
    
    for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
    {
        m_aCurrentStateFlag[i] = 0;
        m_aPrevStateFlag[i] = 0;
    };
};


CPlayerStateWatcher::~CPlayerStateWatcher(void)
{
    ;
};


void CPlayerStateWatcher::Reset(void)
{
    Update();
    Update();
};


void CPlayerStateWatcher::Update(void)
{
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        m_aPrevStateFlag[i] = m_aCurrentStateFlag[i];
        m_aCurrentStateFlag[i] = 0;

        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (pPlayerChr)
        {
            PLAYERTYPES::STATUS ePlayerStatus = pPlayerChr->GetStatus();
            m_aCurrentStateFlag[i] = GetWatchFlag(ePlayerStatus, pPlayerChr->GetID());
        };
    };
};


bool CPlayerStateWatcher::IsAttack(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_ATTACK_ALL);
};


bool CPlayerStateWatcher::IsAttackTrigger(int32 iPlayerIndex) const
{
    bool bResult = false;
    
    if (IsValidIndex(iPlayerIndex))
    {
        uint32 uAttackFlagCurr = (m_aCurrentStateFlag[iPlayerIndex] & PLAYER_WATCH_ATTACK_ALL);
        uint32 uAttackFlagPrev = (m_aPrevStateFlag[iPlayerIndex] & PLAYER_WATCH_ATTACK_ALL);

        if (IsAttack(iPlayerIndex) && (uAttackFlagCurr != uAttackFlagPrev))
            bResult = true;
    };

    return bResult;
};


bool CPlayerStateWatcher::IsAttackLeoDash(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_ATTACK_LEO_DASH);
};


bool CPlayerStateWatcher::IsShootThrowKnife(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_SHOOT_KNIFE);
};


bool CPlayerStateWatcher::IsShootChargeAttack(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_SHOOT_CHARGE);
};


bool CPlayerStateWatcher::IsGuardState(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_STATE_GUARD);
};


bool CPlayerStateWatcher::IsDownState(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_STATE_DOWN);
};


bool CPlayerStateWatcher::IsDamagedFlyawayState(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_STATE_FLYAWAY);
};


bool CPlayerStateWatcher::IsStunState(int32 iPlayerIndex) const
{
    return IsEnableFlag(iPlayerIndex, PLAYER_WATCH_STATE_STUN);
};


bool CPlayerStateWatcher::IsAllPlayerDownState(void) const
{
    int32 nCnt = 0;
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        if (IsDownState(i))
            ++nCnt;
    };

    return (nCnt == nPlayerCnt);
};


bool CPlayerStateWatcher::IsAllPlayerFlyawayOrDownState(void) const
{
    int32 nCnt = 0;
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();

    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        if (IsDownState(i) || IsDamagedFlyawayState(i))
            ++nCnt;
    };

    return (nCnt == nPlayerCnt);
};


bool CPlayerStateWatcher::IsEnableFlag(int32 iPlayerIndex, uint32 uFlag) const
{
    if (IsValidIndex(iPlayerIndex))
        return ((m_aCurrentStateFlag[iPlayerIndex] & uFlag) != 0);
    
    return (false);
};


bool CPlayerStateWatcher::IsValidIndex(int32 iPlayerIndex) const
{
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    return ((iPlayerIndex >= 0) && (iPlayerIndex < nPlayerCnt));
};


uint32 CPlayerStateWatcher::GetWatchFlag(PLAYERTYPES::STATUS eStatus, PLAYERID::VALUE idPlayer) const
{
    uint32 uWatchFlag = 0;
    uWatchFlag |= Sub_GetAttackFlag(eStatus, idPlayer);
    uWatchFlag |= Sub_GetShootFlag(eStatus);
    uWatchFlag |= Sub_GetStateFlag(eStatus);
    return uWatchFlag;
};


uint32 CPlayerStateWatcher::Sub_GetAttackFlag(PLAYERTYPES::STATUS eStatus, PLAYERID::VALUE idPlayer) const
{
    uint32 uFlag = 0;

    if (((eStatus >= PLAYERTYPES::STATUS_ATTACK_A) && (eStatus < PLAYERTYPES::STATUS_ATTACK_JUMP))
        && (eStatus != PLAYERTYPES::STATUS_ATTACK_B_CHARGE)
        || (eStatus == PLAYERTYPES::STATUS_THROW_COMBINATION))
    {
        uFlag |= PLAYER_WATCH_ATTACK;
    };

    if (eStatus == PLAYERTYPES::STATUS_ATTACK_JUMP)
    {
        uFlag |= PLAYER_WATCH_ATTACK_AERIAL;
    };
    
    if (eStatus == PLAYERTYPES::STATUS_LIFT_CHALLENGE)
    {
        uFlag |= PLAYER_WATCH_ATTACK_COMBINATION;
    };

    if (((eStatus >= PLAYERTYPES::STATUS_DASH_CANCEL) && (eStatus <= PLAYERTYPES::STATUS_DASH_FINISH))
        && (idPlayer == PLAYERID::ID_LEO))
    {
        uFlag |= PLAYER_WATCH_ATTACK_LEO_DASH;
    };
    
    return uFlag;
};


uint32 CPlayerStateWatcher::Sub_GetShootFlag(PLAYERTYPES::STATUS eStatus) const
{
    uint32 uFlag = 0;
    
    if ((eStatus >= PLAYERTYPES::STATUS_ATTACK_KNIFE) &&
        (eStatus < PLAYERTYPES::STATUS_LIFT_CHALLENGE))
    {
        uFlag |= PLAYER_WATCH_SHOOT_KNIFE;
    };

    if ((eStatus == PLAYERTYPES::STATUS_ATTACK_B) ||
        (eStatus == PLAYERTYPES::STATUS_ATTACK_AABBC))
    {
        uFlag |= PLAYER_WATCH_SHOOT_CHARGE;
    };

    return uFlag;
};


uint32 CPlayerStateWatcher::Sub_GetStateFlag(PLAYERTYPES::STATUS eStatus) const
{
    uint32 uFlag = 0;

    if (eStatus == PLAYERTYPES::STATUS_ATTACK_B_CHARGE)
    {
        uFlag |= PLAYER_WATCH_STATE_CHARGE;
    };

    if (((eStatus >= PLAYERTYPES::STATUS_JUMP) && (eStatus <= PLAYERTYPES::STATUS_AERIAL_MOVE))
        || (eStatus == PLAYERTYPES::STATUS_ATTACK_JUMP))
    {
        uFlag |= PLAYER_WATCH_STATE_AERIAL;
    };

    if ((eStatus >= PLAYERTYPES::STATUS_GUARD_READY) && (eStatus <= PLAYERTYPES::STATUS_GUARD_FINISH))
    {
        uFlag |= PLAYER_WATCH_STATE_GUARD;
    };

    if (((eStatus >= PLAYERTYPES::STATUS_DOWN_FRONT) && (eStatus <= PLAYERTYPES::STATUS_TUMBLER_BACK)) ||
        ((eStatus >= PLAYERTYPES::STATUS_CRASH_WALL_FRONT) && (eStatus <= PLAYERTYPES::STATUS_CRASH_WALL_TOUCHDOWN_BACK)))
    {
        uFlag |= PLAYER_WATCH_STATE_DOWN;
    };

    if (((eStatus >= PLAYERTYPES::STATUS_FLYAWAY_FRONT) && (eStatus <= PLAYERTYPES::STATUS_FLYAWAY_BOUND_BACK)) ||
        (eStatus == PLAYERTYPES::STATUS_THROWN_MISS))
    {
        uFlag |= PLAYER_WATCH_STATE_FLYAWAY;
    };

    if ((eStatus >= PLAYERTYPES::STATUS_DINDLE) && (eStatus <= PLAYERTYPES::STATUS_FREEZE))
    {
        uFlag |= PLAYER_WATCH_STATE_STUN;
    };    

    return uFlag;
};


//
// *********************************************************************************
//


/*static*/ const float CPlayerWatcher::DIST_MAX = (256.0f * 256.0f);


/*static*/ float CPlayerWatcher::GetDistanceFromPlayer(int32 iPlayerIndex, RwV3d* pvPosition)
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerIndex);
    if (!pPlayerChr)
        return DIST_MAX;

    RwV3d vecPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecPlayer);

    RwV3d vecDistance = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vecDistance, &vecPlayer, pvPosition);
    vecDistance.y = 0.0f;

    float fDist = Math::Vec3_Length(&vecDistance);
    return fDist;
};


/*static*/ float CPlayerWatcher::GetDirection(const RwV3d* pvecPos, const RwV3d* pvecAt)
{
    return std::atan2((pvecAt->x - pvecPos->x),
                      (pvecAt->z - pvecPos->z));
};


CPlayerWatcher::CPlayerWatcher(void)
: m_LastAttackType(0)
, m_vMyPos(Math::VECTOR3_ZERO)
, m_vMyPosOld(Math::VECTOR3_ZERO)
, m_eTargetType(TARGET_TYPE_TEMP)
, m_pEnemyChara(nullptr)    
{
    std::memset(m_aPlayerJump, 0x00, sizeof(m_aPlayerJump));
    std::memset(m_aPlayerData, 0x00, sizeof(m_aPlayerData));
    std::memset(m_aPlayerStatusCurr, 0x00, sizeof(m_aPlayerStatusCurr));
    std::memset(m_aPlayerStatusPrev, 0x00, sizeof(m_aPlayerStatusPrev));
    ClearTargetType();
};


CPlayerWatcher::~CPlayerWatcher(void)
{
    ;
};


void CPlayerWatcher::Initialize(CEnemyCharacter* pEnemyChara)
{
    m_pEnemyChara = pEnemyChara;
    CPlayerStateWatcher::Reset();
    std::memset(m_aPlayerJump, 0x00, sizeof(m_aPlayerJump));
    std::memset(m_aPlayerData, 0x00, sizeof(m_aPlayerData));
    std::memset(m_aPlayerStatusCurr, 0x00, sizeof(m_aPlayerStatusCurr));
    std::memset(m_aPlayerStatusPrev, 0x00, sizeof(m_aPlayerStatusPrev));
    ClearTargetType();
};


void CPlayerWatcher::Update(void)
{
    CPlayerStateWatcher::Update();

    m_vMyPosOld = m_vMyPos;
    m_fMyAngle = EnemyChara().Compositor().GetDirection();
    EnemyChara().Compositor().GetFootPosition(&m_vMyPos);

    IsViewArea(CAIUtils::PLAYER_STATE_ENABLE);
    
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (pPlayerChr)
        {
            m_aPlayerStatusPrev[i] = m_aPlayerStatusCurr[i];
            m_aPlayerStatusCurr[i] = pPlayerChr->GetStatus();
        };
    };
    
    ClearTargetType();
};


bool CPlayerWatcher::IsAttackWithinRange(float fLength, float fFrontAngle)
{
    bool bResult = false;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        float fDist = GetDistanceFromPlayer(i, &m_vMyPos);
        if (fDist <= fLength)
        {
            if (IsPlayerInTheFront(i, fFrontAngle))
            {
                if (IsPlayerLookAtMe(i, Math::PI / 2))
                {
                    if (IsPlayerAttack(i))
                    {
                        SetPlayerData(i, PLAYER_DATA_TEMP);
                        bResult = true;
                        break;
                    };
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsAttackTriggerWithinRange(float fLength, float fFrontAngle)
{
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        float fDist = GetDistanceFromPlayer(i, &m_vMyPos);
		if (fDist > fLength)
			continue;

		if (!IsPlayerInTheFront(i, fFrontAngle))
			continue;

		if (IsPlayerLookAtMe(i, MATH_DEG2RAD(90.0f)) || (fDist < 2.0f))
		{
			if (IsAttack(i) && (m_aPlayerStatusPrev[i] != m_aPlayerStatusCurr[i]))
			{
				SetPlayerData(i, PLAYER_DATA_TEMP);
				return true;
			};
		};
    };

	return false;
};


bool CPlayerWatcher::IsShootShuriken(float fLength)
{
    bool bResult = false;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        if (IsPlayerWithinRange(i, fLength))
        {
            if (IsPlayerLookAtMe(i, Math::PI / 6))
            {
                if (IsShootThrowKnife(i))
                {
                    SetPlayerData(i, PLAYER_DATA_TEMP);
                    bResult = true;
                    break;
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsShootShurikenTrigger(float fLength)
{
    bool bResult = false;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        if (IsPlayerWithinRange(i, fLength))
        {
            if (IsPlayerLookAtMe(i, Math::PI / 6))
            {
                m_aPlayerKnife[0] -= CGameProperty::GetElapsedTime();
                
                if (IsShootThrowKnife(i))
                {
                    if (m_aPlayerKnife[0] <= 0.0f)
                    {
                        m_aPlayerKnife[0] = 0.01f;
                        SetPlayerData(i, PLAYER_DATA_TEMP);
                        bResult = true;
                        break;
                    };
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsChargeAttack(float fLength)
{
    bool bResult = false;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        if (IsPlayerWithinRange(i, fLength))
        {
            if (IsPlayerLookAtMe(i, Math::PI / 6))
            {
                if (IsShootChargeAttack(i))
                {
                    m_aPlayerKnife[0] = 0.01f;
                    SetPlayerData(i, PLAYER_DATA_TEMP);
                    bResult = true;
                    break;
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsHighAttack(float fLength, float fLowSide, float fHighSide)
{
    bool bResult = false;

    float fDistanceOfSuitable = EnemyChara().AICharacteristic().m_fDistanceOfSuitable;
    if (fLength >= 0.0f)
        fDistanceOfSuitable = fLength;
    
    int32 numPlayer = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < numPlayer; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        RwV3d vPlayerPos = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vPlayerPos);

        RwV3d vToPlayer = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vToPlayer, &vPlayerPos, &m_vMyPos);

        if ((vToPlayer.y <= fLowSide) ||
            (vToPlayer.y >= fHighSide))
        {
            m_aPlayerJump[i] = false;
            continue;
        };

        float fD = ((vToPlayer.y - fLowSide) / (fHighSide - fLowSide)) * fDistanceOfSuitable;

        vToPlayer.y = 0.0f;

        float fLengthXZ = Math::Vec3_Length(&vToPlayer);
        if (fD <= fLengthXZ)
        {
            m_aPlayerJump[i] = false;
        }
        else if (!m_aPlayerJump[i])
        {
            SetPlayerData(i, PLAYER_DATA_TEMP);
            m_aPlayerJump[i] = true;
            bResult = true;
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsSuitableArea(float fViewAngle, float fLength /*= -1.0f*/)
{
    bool bResult = false;
    float fDistOfSuit = EnemyChara().AICharacteristic().m_fDistanceOfSuitable;
    float fDistNearest = DIST_MAX;

    if (fLength >= 0.0f)
        fDistOfSuit = fLength;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
        CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);
        if (playerStateFlag & CAIUtils::PLAYER_STATE_ENABLE)
        {
            if (IsPlayerViewArea(i)                 &&
                IsPlayerWithinRange(i, fDistOfSuit) &&
                IsPlayerInTheFront(i, fViewAngle))
            {
                float fDist = GetDistanceFromPlayer(i, &m_vMyPos);
                if (fDist < fDistNearest)
                {
                    fDistNearest = fDist;
                    SetPlayerData(i, PLAYER_DATA_SUIT);
                    bResult = true;
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsViewArea(CAIUtils::PLAYER_STATE_FLAG state)
{
    bool bResult = false;
    float fDistNearest = DIST_MAX;
    
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
        CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);

        if (playerStateFlag & state)
        {
            if (IsPlayerViewArea(i))
            {
                float fDist = GetDistanceFromPlayer(i, &m_vMyPos);
                if (fDist < fDistNearest)
                {
                    fDistNearest = fDist;
                    SetPlayerData(i, PLAYER_DATA_NEAR);
                    bResult = true;
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsPlayerAttack(int32 iPlayerIndex)
{
    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);    
    if (iPlayerIndex != -1)
        return IsAttack(iPlayerIndex);
    else
        return false;
};


bool CPlayerWatcher::IsPlayerAttackTrigger(int32 iPlayerIndex)
{
    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
	if (iPlayerIndex != -1)
		return IsAttackTrigger(iPlayerIndex);

	return false;
};


bool CPlayerWatcher::IsPlayerAttackTriggerWithinRange(int32 iPlayerIndex, float fLength, float fFrontAngle)
{
	float fDist = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
	if (fDist > fLength)
		return false;

	if (!IsPlayerInTheFront(iPlayerIndex, fFrontAngle))
		return false;

	if (!IsPlayerLookAtMe(iPlayerIndex, MATH_DEG2RAD(90.0f)))
	{
		if (fDist >= 2.0f)
			return false;
	};

	if (!IsAttack(iPlayerIndex))
		return false;

	if (m_aPlayerStatusPrev[iPlayerIndex] == m_aPlayerStatusCurr[iPlayerIndex])
		return false;

	SetPlayerData(iPlayerIndex, PLAYER_DATA_TEMP);
	return true;
};


bool CPlayerWatcher::IsPlayerShootShurikenTrigger(int32 iPlayerIndex, float fLength)
{
    bool bResult = false;

    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
    if (iPlayerIndex != -1)
    {
        float fDist = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
        if (fDist <= fLength)
        {
            if (IsPlayerLookAtMe(iPlayerIndex, Math::PI / 6))
            {
                m_aPlayerKnife[iPlayerIndex] -= CGameProperty::GetElapsedTime();
                if (IsShootThrowKnife(iPlayerIndex) && (m_aPlayerKnife[iPlayerIndex] <= 0.0f))
                {
                    m_aPlayerKnife[iPlayerIndex] = 0.01f;
                    SetPlayerData(iPlayerIndex, PLAYER_DATA_TEMP);
                    bResult = true;
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsPlayerViewArea(int32 iPlayerIndex)
{
    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
	if (iPlayerIndex == -1)
		return false;

	if (!IsPlayerActionRange(iPlayerIndex, 1.0f))
		return false;

	CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerIndex);
	if (!pPlayerChr)
		return false;

	RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
	pPlayerChr->GetFootPosition(&vecFootPosPlayer);

	float fHeight = (m_vMyPos.y - vecFootPosPlayer.y);
	fHeight = std::fabs(fHeight);

	if (fHeight > 3.0f)
		return false;

	return true;
};


bool CPlayerWatcher::IsPlayerLookAtMe(int32 iPlayerIndex, float fAngle)
{
    bool bResult = false;
    
    if (iPlayerIndex == -1)
        iPlayerIndex = m_aPlayerData[PLAYER_DATA_TEMP].No;

    if (iPlayerIndex != -1)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerIndex);
        if (pPlayerChr)
        {
            RwV3d pos = Math::VECTOR3_ZERO;
            pPlayerChr->GetFootPosition(&pos);
            
            float fDirToEnemy = (GetDirection(&pos, &m_vMyPos) - pPlayerChr->GetDirection());
            fDirToEnemy = Math::RadianCorrect(fDirToEnemy);
            if (fDirToEnemy < 0.0f)
                fDirToEnemy = -fDirToEnemy;
            
            bResult = (fDirToEnemy <= fAngle);
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsPlayerInTheFront(int32 iPlayerIndex, float fFrontAngle)
{
    if (fFrontAngle >= MATH_PI2)
        return true;

    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
	if (iPlayerIndex == -1)
		return false;

    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerIndex);
    if (!pPlayerChr)
        return false;

    RwV3d vecFootPosPlayer = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vecFootPosPlayer);
    
	float fDir = GetDirection(&vecFootPosPlayer, &m_vMyPos) - m_fMyAngle;
    fDir = Math::RadianCorrect(fDir);
	fDir = std::fabs(fDir);
    
	return (fDir >= (MATH_PI - fFrontAngle));
};


bool CPlayerWatcher::IsPlayerWithinRange(int32 iPlayerIndex, float fLength)
{
    if (iPlayerIndex == -1)
        iPlayerIndex = GetPlayerData(PLAYER_DATA_TEMP)->No;

    if (iPlayerIndex != -1)
        return (GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos) <= fLength);

	return false;
};


bool CPlayerWatcher::IsPlayerSuitRange(int32 iPlayerIndex, float fRate)
{
    ASSERT(fRate < 3.0f);
    ASSERT(m_pEnemyChara);

    bool bResult = false;

    fRate *= EnemyChara().AICharacteristic().m_fDistanceOfSuitable;

    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
    if (iPlayerIndex != -1)
    {
        float fDist = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
        if (fDist <= fRate)
            bResult = true;
    };

    return bResult;
};


bool CPlayerWatcher::IsPlayerActionRange(int32 iPlayerIndex, float fRate)
{
    ASSERT(fRate < 3.0f);
    ASSERT(m_pEnemyChara);

    bool bResult = false;

    fRate *= EnemyChara().AICharacteristic().m_fRadiusOfAction;
    
    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
    if (iPlayerIndex != -1)
    {
        float fDist = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
        if (fDist <= fRate)
            bResult = true;
    };

    return bResult;
};


bool CPlayerWatcher::IsPlayerStop(int32 iPlayerIndex)
{
    bool bResult = false;
    
    iPlayerIndex = PlayerIndexCorrection(iPlayerIndex);
    if (iPlayerIndex != -1)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerIndex);
        if (pPlayerChr)
        {
            RwV3d vecVel = Math::VECTOR3_ZERO;
            pPlayerChr->GetVelocity(&vecVel);

            if (Math::Vec3_Length(&vecVel) <= 2.0f)
            {
                PLAYERTYPES::STATUS eStatusCurr = m_aPlayerStatusCurr[iPlayerIndex];
                if ((eStatusCurr < PLAYERTYPES::STATUS_DASH_CANCEL) || (eStatusCurr >= PLAYERTYPES::STATUS_JUMP_WALL))
                {
                    if (eStatusCurr != PLAYERTYPES::STATUS_ATTACK_JUMP)
                        bResult = true;
                };
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsDonBarrier(float fLength)
{
    bool bResult = false;
    
    const PLAYERID::VALUE aPlayerID[] =
    {
        PLAYERID::ID_DON,
        PLAYERID::ID_SPL
    };

    for (int32 i = 0; i < COUNT_OF(aPlayerID); ++i)
    {
        int32 iPlayerIndex = GetPlayerNoFromID(aPlayerID[i]);
        if (iPlayerIndex != -1)
        {
            float fDist = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
            if (fDist < fLength)
            {
                bResult = IsShootChargeAttack(iPlayerIndex);
                if (bResult)
                    break;
            };
        };
    };

    return bResult;
};


bool CPlayerWatcher::IsLeoDash(float fLength)
{
    bool bResult = false;
    
    int32 iPlayerIndex = GetPlayerNoFromID(PLAYERID::ID_LEO);
    if (iPlayerIndex != -1)
    {
        float fDist = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
        if (fDist < fLength)
        {
            if (IsAttackLeoDash(iPlayerIndex))
                bResult = IsPlayerLookAtMe(iPlayerIndex, Math::PI05);
        };
    };

    return bResult;
};


int32 CPlayerWatcher::GetPlayerNumThere(RwV3d* pvCenter, float fRadius)
{
    int32 iResult = 0;
    RwV3d vCenter = m_vMyPos;
    
    if (pvCenter)
        vCenter = *pvCenter;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
        CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);

        if (playerStateFlag & CAIUtils::PLAYER_STATE_ENABLE)
        {
            RwV3d vecPos = Math::VECTOR3_ZERO;
            pPlayerChr->GetFootPosition(&vecPos);

            RwV3d vecDist = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vecDist, &vecPos, &vCenter);

            float fDist = Math::Vec3_Length(&vecDist);
            if (fDist < fRadius)
                ++iResult;
        };
    };
    
    return iResult;
};


bool CPlayerWatcher::IsTogether(int32 iPlayerNum, float fRadius)
{
    bool bResult = false;    
    float fDistNearest = DIST_MAX;

    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (pPlayerChr)
        {
            RwV3d vecPos = Math::VECTOR3_ZERO;
            pPlayerChr->GetFootPosition(&vecPos);

            if (GetPlayerNumThere(&vecPos, fRadius) >= iPlayerNum)
            {
                float fDist = GetDistanceFromPlayer(i, &m_vMyPos);
                if (fDist < fDistNearest)
                {
                    fDistNearest = fDist;
                    SetPlayerData(i, PLAYER_DATA_TEMP);
                    bResult = true;
                };
            };
        };        
    };

    return bResult;
};


int32 CPlayerWatcher::GetPlayerNoFromID(PLAYERID::VALUE id)
{
    int32 nPlayerCnt = CAIUtils::GetPlayerNum();
    for (int32 i = 0; i < nPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (pPlayerChr)
        {
            if (pPlayerChr->GetID() == id)
                return i;
        };
    };

    return -1;
};


int32 CPlayerWatcher::GetNearPlayer(RwV3d* pvCenter, float* pfDistNearest, RwV3d* pvPosNearest, CAIUtils::PLAYER_STATE_FLAG state)
{
    int32 iPlayerIndex = -1;
    int32 iPlayerCnt = CAIUtils::GetPlayerNum();
    float fDistNearest = DIST_MAX;
    float dt = CGameProperty::GetElapsedTime();

    for (int32 i = 0; i < iPlayerCnt; ++i)
    {
        CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(i);
        if (!pPlayerChr)
            continue;

        PLAYERTYPES::STATUS playerStatus = pPlayerChr->GetStatus();
        CAIUtils::PLAYER_STATE_FLAG playerStateFlag = CAIUtils::GetPlayerStateFlag(playerStatus);

        if (!(playerStateFlag & state))
            continue;

        RwV3d vecPos = Math::VECTOR3_ZERO;
        pPlayerChr->GetFootPosition(&vecPos);

        RwV3d vecVel = Math::VECTOR3_ZERO;
        pPlayerChr->GetVelocity(&vecVel);

        Math::Vec3_Scale(&vecVel, &vecVel, dt);
        Math::Vec3_Add(&vecPos, &vecPos, &vecVel);

        RwV3d vecDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vecDist, &vecPos, pvCenter);
        vecDist.y = 0.0f;

        float fDist = Math::Vec3_Length(&vecDist);
        if (fDist < fDistNearest)
        {
            fDistNearest = fDist;
            iPlayerIndex = i;
            
            if (pvPosNearest)
                *pvPosNearest = vecPos;
            
            if (pfDistNearest)
                *pfDistNearest = fDistNearest;
        };
    };

    return iPlayerIndex;
};


void CPlayerWatcher::SetTargetType(TARGET_TYPE type)
{
    if (m_eTargetType != TARGET_TYPE_ATTACKLAST)
    {
        ASSERT(GetPlayerData(PLAYER_DATA_TYPE(type))->No >= 0);
        m_eTargetType = type;
    };
};


void CPlayerWatcher::ClearTargetType(void)
{
    m_eTargetType = TARGET_TYPE_NEAR;
};


void CPlayerWatcher::SetPlayerData(int32 iPlayerIndex, PLAYER_DATA_TYPE type)
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(iPlayerIndex);
    if (pPlayerChr)
    {
        m_aPlayerData[type].No = iPlayerIndex;
        m_aPlayerData[type].Length = GetDistanceFromPlayer(iPlayerIndex, &m_vMyPos);
        pPlayerChr->GetFootPosition(&m_aPlayerData[type].Position);

        if (type)
            SetPlayerData(iPlayerIndex, PLAYER_DATA_TEMP);    
    };
};


CPlayerWatcher::PLAYERDATA* CPlayerWatcher::GetPlayerData(PLAYER_DATA_TYPE type)
{
    ASSERT(type >= 0);
    ASSERT(type < COUNT_OF(m_aPlayerData));
    
    return &m_aPlayerData[type];
};


CEnemyCharacter& CPlayerWatcher::EnemyChara(void)
{
    ASSERT(m_pEnemyChara);
    return *m_pEnemyChara;
};


int32 CPlayerWatcher::PlayerIndexCorrection(int32 iPlayerIndex)
{
    if (iPlayerIndex == -1)
        return GetPlayerData(PLAYER_DATA_TEMP)->No;
    
    return iPlayerIndex;
};


void CPlayerWatcher::GetTargetTypePosition(RwV3d* pvPos) const
{
    ASSERT(pvPos != nullptr);
    ASSERT(m_eTargetType <= TARGET_TYPE_ATTACKLAST);

    *pvPos = m_aPlayerData[m_eTargetType].Position;
};


int32 CPlayerWatcher::GetTargetTypeNo(void) const
{
    ASSERT(m_eTargetType <= TARGET_TYPE_ATTACKLAST);

    return m_aPlayerData[m_eTargetType].No;
};