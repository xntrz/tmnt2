#include "PlayerUtil.hpp"
#include "PlayerCharacter.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/System/Sound/GameSound.hpp"


static void PlayCharaSE(const CPlayerCharacter* pPlayerChara, int32* aiVoiceIDList)
{
    ASSERT(pPlayerChara);
    ASSERT(aiVoiceIDList);

    PLAYERID::VALUE idPlayer = pPlayerChara->GetID();
    ASSERT(idPlayer >= PLAYERID::ID_START);
    ASSERT(idPlayer <  PLAYERID::ID_MAX);

    if (aiVoiceIDList[idPlayer] != -1)
        CGameSound::PlayObjectSE(pPlayerChara, aiVoiceIDList[idPlayer]);
};


namespace PlayerUtil
{
    bool IsPowerfullCharacter(const CPlayerCharacter* pPlayerChr)
    {
        ASSERT(pPlayerChr);

        return ((pPlayerChr->GetID() == PLAYERID::ID_RAP) ||
                (pPlayerChr->GetID() == PLAYERID::ID_CAS));
    };


    void CallSEOfTouchdown(const CPlayerCharacter* pPlayerChr)
    {
        const CPlayerCharacter::COLLISIONGROUNDINFO* pGndInfo = pPlayerChr->GetCollisionGround();
        ASSERT(pGndInfo);
        
        if (EFFECT_GENERIC::IsNeedWaterEffect(pGndInfo->m_attribute))
        {
            CGameSound::PlayObjectSE(pPlayerChr, SDCODE_SE(4117), 0);
        }
        else if (pGndInfo->m_attribute != MAPTYPES::ATTRIBUTE_DEATH)
        {
            CGameSound::PlayObjectSE(pPlayerChr, SDCODE_SE(4116), 0);
        };
    };


    void CallVoiceOfChargeAttack(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
			SDCODE_SE(0x2031), // leo
			SDCODE_SE(0x204A), // rap
			SDCODE_SE(0x2063), // mic
			SDCODE_SE(0x207C), // don
			SDCODE_SE(0x20E8), // sla
			SDCODE_SE(0x20B0), // cas
			SDCODE_SE(0x20CC), // kar
			SDCODE_SE(0x2096), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfPassive(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
			SDCODE_SE(0x2032), // leo
			SDCODE_SE(0x204B), // rap
			SDCODE_SE(0x2064), // mic
			SDCODE_SE(0x207D), // don
			SDCODE_SE(0x20E9), // sla
			SDCODE_SE(0x20B1), // cas
			SDCODE_SE(0x20CD), // kar
			SDCODE_SE(0x2097), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfLiftUp(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
			SDCODE_SE(0x201A), // leo
			SDCODE_SE(0x2033), // rap
			SDCODE_SE(0x204C), // mic
			SDCODE_SE(0x2065), // don
			SDCODE_SE(0x20CE), // sla
			SDCODE_SE(0x2098), // cas
			SDCODE_SE(0x20B2), // kar
			SDCODE_SE(0x207E), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfBackThrow(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
		{
			SDCODE_SE(-1), 		// leo
			SDCODE_SE(-1), 		// rap
			SDCODE_SE(-1), 		// mic
			SDCODE_SE(-1), 		// don
			SDCODE_SE(0x20E7), 	// sla
			SDCODE_SE(0x20AF), 	// cas
			SDCODE_SE(0x20CB), 	// kar
			SDCODE_SE(0x2095), 	// spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");
        
        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfJumpWall(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
		{
			// jump №1
			SDCODE_SE(0x202B), // leo
  			SDCODE_SE(0x2044), // rap
  			SDCODE_SE(0x205D), // mic
  			SDCODE_SE(0x2076), // don
			SDCODE_SE(0x20E1), // sla
  			SDCODE_SE(0x20A9), // cas
  			SDCODE_SE(0x20C4), // kar
  			SDCODE_SE(0x208F), // spl

			// jump №2
			SDCODE_SE(0x202C), // leo
  			SDCODE_SE(0x2045), // rap
  			SDCODE_SE(0x205E), // mic
  			SDCODE_SE(0x2077), // don
			SDCODE_SE(0x20E0), // sla
  			SDCODE_SE(0x20AA), // cas
  			SDCODE_SE(0x20C5), // kar
  			SDCODE_SE(0x2090), // spl

			// jump №3
			SDCODE_SE(0x202D), // leo
  			SDCODE_SE(0x2046), // rap
  			SDCODE_SE(0x205F), // mic
  			SDCODE_SE(0x2078), // don
			SDCODE_SE(0x20E2), // sla
  			SDCODE_SE(0x20AB), // cas
  			SDCODE_SE(0x20C6), // kar
  			SDCODE_SE(0x2091), // spl

			// jump №4
			SDCODE_SE(0x202E), // leo
  			SDCODE_SE(0x2047), // rap
  			SDCODE_SE(0x2060), // mic
  			SDCODE_SE(0x2079), // don
			SDCODE_SE(0x20E3), // sla
  			SDCODE_SE(0x20AC), // cas
  			SDCODE_SE(0x20C7), // kar
  			SDCODE_SE(0x2092), // spl

			// jump №5
			SDCODE_SE(0x202F), // leo
  			SDCODE_SE(0x2048), // rap
  			SDCODE_SE(0x2061), // mic
  			SDCODE_SE(0x207A), // don
  			SDCODE_SE(0x20E4), // sla
  			SDCODE_SE(0x20AD), // cas
  			SDCODE_SE(0x20C8), // kar
  			SDCODE_SE(0x2093), // spl
        };

        static_assert((COUNT_OF(aiVoiceList) / 5) == PLAYERID::ID_MAX, "update me");

		const int32 nNumWallJumpMax = (COUNT_OF(aiVoiceList) / PLAYERID::ID_MAX) - 1;

		int32 nNumWallJump = pPlayerChr->GetNumWallJump();
		nNumWallJump = Clamp(nNumWallJump, 0, nNumWallJumpMax);

        PlayCharaSE(pPlayerChr, &aiVoiceList[nNumWallJump * PLAYERID::ID_MAX]);
    };


    void CallVoiceOfAABBB(const CPlayerCharacter* pPlayerChr)
    {
		int32 aiVoiceList[] =
		{
			SDCODE_SE(0x201E), // leo
			SDCODE_SE(0x2037), // rap
			SDCODE_SE(0x2050), // mic
			SDCODE_SE(0x2069), // don
			SDCODE_SE(0x20D2), // sla
			SDCODE_SE(0x209C), // cas
			SDCODE_SE(0x20B6), // kar
			SDCODE_SE(0x2082), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfAABBC(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
			SDCODE_SE(0x201F), // leo
			SDCODE_SE(0x2038), // rap
			SDCODE_SE(0x2051), // mic
			SDCODE_SE(0x206A), // don
			SDCODE_SE(0x20D3), // sla
			SDCODE_SE(0x209D), // cas
			SDCODE_SE(0x20B7), // kar
			SDCODE_SE(0x2083), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfComboHit(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
  			SDCODE_SE(0x2026), // leo
  			SDCODE_SE(0x203F), // rap
  			SDCODE_SE(0x2058), // mic
  			SDCODE_SE(0x2071), // don
  			SDCODE_SE(0x20DA), // sla
  			SDCODE_SE(0x20A4), // cas
  			SDCODE_SE(0x20BE), // kar
  			SDCODE_SE(0x208A), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfGuardImpact(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
  			SDCODE_SE(0x202A), // leo
  			SDCODE_SE(0x2043), // rap
  			SDCODE_SE(0x205C), // mic
  			SDCODE_SE(0x2075), // don
  			SDCODE_SE(0x20DF), // sla
  			SDCODE_SE(0x20A8), // cas
  			SDCODE_SE(0x20C2), // kar
  			SDCODE_SE(0x208E), // spl
        };

        static_assert(COUNT_OF(aiVoiceList) == PLAYERID::ID_MAX, "update me");

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };
};