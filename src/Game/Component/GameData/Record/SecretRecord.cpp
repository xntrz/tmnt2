#include "SecretRecord.hpp"
#include "DatabaseRecord.hpp"
#include "NexusRecord.hpp"

#include "Game/Component/GameData/GameData.hpp"


static inline int32 calcCrystalLevel(GAMETYPES::CRYSTALTYPE crytype)
{
	int32 iCryNum = CGameData::Record().Item().GetCrystalNum(crytype);
	int32 iLevel = iCryNum / 10;
	return Math::Clamp(iLevel, 0, 3);
};


/*static*/ bool CSecretRecord::m_bDebugNotifyUnlocked = true;


CSecretRecord::CSecretRecord(void)
{
    ;
};


CSecretRecord::~CSecretRecord(void)
{
	;
};


void CSecretRecord::SetDefault(void)
{
	for (int32 i = 0; i < COUNT_OF(m_auUnlockedFlag); ++i)
		m_auUnlockedFlag[i] = 0;

	for (int32 i = 0; i < COUNT_OF(m_auUnnotifiedFlag); ++i)
		m_auUnnotifiedFlag[i] = 0;

	UnlockSecret(SECRETID::ID_HOME_SHOP);
	UnlockSecret(SECRETID::ID_HOME_TOURNAMENT_KITTY);

	m_fAttackEnchanceValue = 1.0f;
	m_fDefenceEnchanceValue = 1.0f;
};


bool CSecretRecord::IsValid(void) const
{
	if (m_fAttackEnchanceValue < 1.0f ||
		m_fAttackEnchanceValue > 2.0f)
	{
		OUTPUT("[GAME] %s is failed: attack value\n", __FUNCTION__);
		return false;
	};

	if (m_fDefenceEnchanceValue < 1.0f ||
		m_fDefenceEnchanceValue > 2.0f)
	{
		OUTPUT("[GAME] %s is failed: defence value\n", __FUNCTION__);
		return false;
	};

	OUTPUT("[GAME] %s ...OK\n", __FUNCTION__);
	return true;
};


void CSecretRecord::SetNotSavedFlagOff(void)
{
	for (int32 i = SECRETID::SECRETSTART; i < SECRETID::SECRETMAX; i++)
		setFlag(FLAGTYPE_UNLOCKED, SECRETID::VALUE(i), false);
};


void CSecretRecord::Snapshot(RAWDATA& rRawData) const
{
	std::memcpy(rRawData.m_auUnlockedFlag, m_auUnlockedFlag, sizeof(rRawData.m_auUnlockedFlag));
	std::memcpy(rRawData.m_auUnnotifiedFlag, m_auUnnotifiedFlag, sizeof(rRawData.m_auUnnotifiedFlag));
	rRawData.m_fAttackEnchanceValue = m_fAttackEnchanceValue;
	rRawData.m_fDefenceEnchanceValue = m_fDefenceEnchanceValue;
};


void CSecretRecord::Restore(const RAWDATA& rRawData)
{
	std::memcpy(m_auUnlockedFlag, rRawData.m_auUnlockedFlag, sizeof(m_auUnlockedFlag));
	std::memcpy(m_auUnnotifiedFlag, rRawData.m_auUnnotifiedFlag, sizeof(m_auUnnotifiedFlag));
	m_fAttackEnchanceValue = rRawData.m_fAttackEnchanceValue;
	m_fDefenceEnchanceValue = rRawData.m_fDefenceEnchanceValue;
};


bool CSecretRecord::IsUnlockedSecret(SECRETID::VALUE idSecret) const
{
	if (CGameData::Attribute().IsNexusMode())
		return false;
	else
		return getFlag(FLAGTYPE_UNLOCKED, idSecret);
};


void CSecretRecord::UnlockSecret(SECRETID::VALUE idSecret)
{
	if (getFlag(FLAGTYPE_UNLOCKED, idSecret))
		return;		
			
	if (idSecret >= SECRETID::CHEATCODESTART && idSecret < SECRETID::CHEATCODEMAX)
		setOffGroupA();
	else if (idSecret >= SECRETID::CHALLENGESTART && idSecret < SECRETID::CHALLENGEMAX)
		setOffGroupB();

	if (isDatabase(idSecret))
	{
		switch (idSecret)
		{
		case SECRETID::ID_DATABASE_PLAYMATES:
			{
				if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_A) &&
					!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_B) &&
					!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_C) &&
					!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_D))
				{
					CGameData::Record().Database().SetItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_A);
					CGameData::Record().Database().SetItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_B);
					CGameData::Record().Database().SetItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_C);
					CGameData::Record().Database().SetItemUnlocked(DBITEMID::ID_ETC_PLAYMATES_D);
				};
			}
			break;

		case SECRETID::ID_DATABASE_PROMOTION_ARTS:
			{
				if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::ID_ETC_GALLERY))
					CGameData::Record().Database().SetItemUnlocked(DBITEMID::ID_ETC_GALLERY);
			}
			break;

		default:
			{
				DBITEMID::VALUE idDBItem = DBITEMID::VALUE(idSecret - (SECRETID::DATABASESTART - 1));
				if (!CGameData::Record().Database().IsItemUnlocked(idDBItem))
					CGameData::Record().Database().SetItemUnlocked(idDBItem);
			}
			break;
		};
	};

	switch (idSecret)
	{
	case SECRETID::ID_HOME_SHOP:
		CGameData::Record().Antique().SetShopState(CAntiqueRecord::SHOPSTATE_OPEN);
		break;
		
	case SECRETID::ID_HOME_BONUSMATERIAL:
		CGameData::Record().Database().SetDatabaseState(CDatabaseRecord::DBSTATE_OPEN);
		break;

	case SECRETID::ID_HOME_TOURNAMENT_KITTY:
		CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN, CNexusRecord::STATE_OPEN);
		break;

	case SECRETID::ID_HOME_TOURNAMENT_MONSTER:
		CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN, CNexusRecord::STATE_OPEN);
		break;

	case SECRETID::ID_HOME_TOURNAMENT_FOOT:
		CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT, CNexusRecord::STATE_OPEN);
		break;

	case SECRETID::ID_HOME_TOURNAMENT_BATTLENEXUS:
		CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS, CNexusRecord::STATE_OPEN);
		break;
	};

	setFlag(FLAGTYPE_UNLOCKED, idSecret, true);

#ifdef _DEBUG
	if (m_bDebugNotifyUnlocked)
		setFlag(FLAGTYPE_UNNOTIFIED, idSecret, true);
	else
		setFlag(FLAGTYPE_UNNOTIFIED, idSecret, false);
#else
	setFlag(FLAGTYPE_UNNOTIFIED, idSecret, true);
#endif
};


void CSecretRecord::NotifySecret(SECRETID::VALUE idSecret)
{
	setFlag(FLAGTYPE_UNNOTIFIED, idSecret, false);
};


int32 CSecretRecord::GetAttackLevel(void) const
{
	return calcCrystalLevel(GAMETYPES::CRYSTALTYPE_RED);	
};


int32 CSecretRecord::GetDefenceLevel(void) const
{
	return calcCrystalLevel(GAMETYPES::CRYSTALTYPE_GREEN);
};


int32 CSecretRecord::GetAerialLevel(void) const
{	
	return calcCrystalLevel(GAMETYPES::CRYSTALTYPE_WHITE);
};


int32 CSecretRecord::GetChargeLevel(void) const
{
	return calcCrystalLevel(GAMETYPES::CRYSTALTYPE_ORANGE);
};


bool CSecretRecord::IsDonLazerEnabled(void) const
{
	return CGameData::Record().Item().IsItemTakenYet(ITEMID::ID_DON_LASER);
};


float CSecretRecord::GetAttackEnchanceValue(void) const
{
	if (IsUnlockedSecret(SECRETID::ID_CHEATCODE_SUPERATTACK))
		return 2.0f;
	else
		return m_fAttackEnchanceValue;
};


float CSecretRecord::GetDefenceEnchanceValue(void) const
{
	if (IsUnlockedSecret(SECRETID::ID_CHEATCODE_SUPERDEFENCE))
		return 2.0f;
	else
		return m_fAttackEnchanceValue;
};


SECRETID::VALUE CSecretRecord::GetUnnotifiedSecret(void)
{
	for (int32 i = SECRETID::ID_FIRST; i < SECRETID::ID_MAX; ++i)
	{
		SECRETID::VALUE SecretID = SECRETID::VALUE(i);

		//
		//	If secret is unlocked AND ready for notify
		//
		if (getFlag(FLAGTYPE_UNNOTIFIED, SecretID) &&
			getFlag(FLAGTYPE_UNLOCKED, SecretID))
		{
			//
			//	Return result and remove ready for notify flag
			//
			setFlag(FLAGTYPE_UNNOTIFIED, SecretID, false);
			return SecretID;
		};
	};

	return SECRETID::ID_NONE;
};


void CSecretRecord::setOffGroupA(void)
{
	for (int32 i = SECRETID::CHEATCODESTART; i < SECRETID::CHEATCODEMAX; ++i)
		setFlag(FLAGTYPE_UNLOCKED, SECRETID::VALUE(i), false);
};


void CSecretRecord::setOffGroupB(void)
{
	for (int32 i = SECRETID::CHALLENGESTART; i < SECRETID::CHALLENGEMAX; ++i)
		setFlag(FLAGTYPE_UNLOCKED, SECRETID::VALUE(i), false);
};


bool CSecretRecord::isValidSecretID(SECRETID::VALUE idSecret) const
{
	return (idSecret >= SECRETID::ID_NONE && idSecret < SECRETID::NORMALMAX);
};


bool CSecretRecord::isDatabase(SECRETID::VALUE idSecret) const
{
	return (idSecret >= SECRETID::DATABASESTART && idSecret < SECRETID::DATABASEMAX);
};


void CSecretRecord::setFlag(FLAGTYPE flagtype, SECRETID::VALUE idSecret, bool bSet)
{
	ASSERT(idSecret >= SECRETID::ID_FIRST && idSecret < SECRETID::ID_MAX);
	
	int32 FlagNo = (idSecret / 32);
	int32 SecretNo = (idSecret % 32);

	ASSERT(FlagNo >= 0 && FlagNo < FLAGNUM);
	ASSERT(SecretNo >= 0 && SecretNo < BITSOF(uint32));

	uint32* puFlag = (flagtype == FLAGTYPE_UNLOCKED ? m_auUnlockedFlag : m_auUnnotifiedFlag);

	FLAG_CHANGE(puFlag[FlagNo], BIT(SecretNo), bSet);
};


bool CSecretRecord::getFlag(FLAGTYPE flagtype, SECRETID::VALUE idSecret) const
{
	ASSERT(idSecret >= SECRETID::ID_FIRST && idSecret < SECRETID::ID_MAX);
	
	int32 FlagNo = (idSecret / 32);
	int32 SecretNo = (idSecret % 32);

	ASSERT(FlagNo >= 0 && FlagNo < FLAGNUM);
	ASSERT(SecretNo >= 0 && SecretNo < BITSOF(uint32));

	const uint32* puFlag = (flagtype == FLAGTYPE_UNLOCKED ? m_auUnlockedFlag : m_auUnnotifiedFlag);

	return IS_FLAG_SET(puFlag[FlagNo], BIT(SecretNo));
};