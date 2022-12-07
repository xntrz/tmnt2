#include "PlayerUtil.hpp"
#include "PlayerCharacter.hpp"

#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/System/Sound/GameSound.hpp"


static void PlayCharaSE(const CPlayerCharacter* pPlayerChara, int32* aiVoiceIDList)
{
    ASSERT(pPlayerChara);
    ASSERT(aiVoiceIDList);

    PLAYERID::VALUE idPlayer = pPlayerChara->GetID();
    ASSERT(idPlayer >= PLAYERID::ID_START && idPlayer < PLAYERID::ID_MAX);

    if (aiVoiceIDList[idPlayer] != -1)
        CGameSound::PlayObjectSE(pPlayerChara, aiVoiceIDList[idPlayer]);
};


namespace PlayerUtil
{
    bool IsPowerfullCharacter(const CPlayerCharacter* pPlayerChr)
    {
        ASSERT(pPlayerChr);

        return (pPlayerChr->GetID() == PLAYERID::ID_RAP ||
                pPlayerChr->GetID() == PLAYERID::ID_CAS);
    };


    void CallSEOfTouchdown(const CPlayerCharacter* pPlayerChr)
    {
        const CPlayerCharacter::COLLISIONGROUNDINFO* pGndInfo = pPlayerChr->GetCollisionGround();
        ASSERT(pGndInfo);
        
        if (EFFECT_GENERIC::IsNeedWaterEffect(pGndInfo->m_attribute))
        {
            CGameSound::PlayObjectSE(pPlayerChr, SDCODE_SE(4117), 0);
        }
        else if (!FLAG_TEST(pGndInfo->m_attribute, MAPTYPES::ATTRIBUTE_DEATH))
        {
            CGameSound::PlayObjectSE(pPlayerChr, SDCODE_SE(4116), 0);
        };
    };


    void CallVoiceOfChargeAttack(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8241),
            SDCODE_VOICE(8266),
            SDCODE_VOICE(8291),
            SDCODE_VOICE(8316),
            SDCODE_VOICE(8342),
            SDCODE_VOICE(8368),
            SDCODE_VOICE(8396),
            SDCODE_VOICE(8424),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfPassive(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8242),
            SDCODE_VOICE(8267),
            SDCODE_VOICE(8292),
            SDCODE_VOICE(8317),
            SDCODE_VOICE(8343),
            SDCODE_VOICE(8369),
            SDCODE_VOICE(8397),
            SDCODE_VOICE(8425),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfLiftUp(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8218),
            SDCODE_VOICE(8243),
            SDCODE_VOICE(8268),
            SDCODE_VOICE(8293),
            SDCODE_VOICE(8318),
            SDCODE_VOICE(8344),
            SDCODE_VOICE(8370),
            SDCODE_VOICE(8398),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfBackThrow(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(-1),
            SDCODE_VOICE(-1),
            SDCODE_VOICE(-1),
            SDCODE_VOICE(-1),
            SDCODE_VOICE(8341),
            SDCODE_VOICE(8367),
            SDCODE_VOICE(8395),
            SDCODE_VOICE(8423),
        };
        
        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfJumpWall(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList [] =
        {
            SDCODE_VOICE(8235),
            SDCODE_VOICE(8260),
            SDCODE_VOICE(8285),
            SDCODE_VOICE(8310),
            SDCODE_VOICE(8335),
            SDCODE_VOICE(8361),
            SDCODE_VOICE(8388),
            SDCODE_VOICE(8417),
            SDCODE_VOICE(8236),
            SDCODE_VOICE(8261),

            SDCODE_VOICE(8286),
            SDCODE_VOICE(8311),
            SDCODE_VOICE(8336),
            SDCODE_VOICE(8362),
            SDCODE_VOICE(8389),
            SDCODE_VOICE(8416),
            SDCODE_VOICE(8237),
            SDCODE_VOICE(8262),
            SDCODE_VOICE(8287),
            SDCODE_VOICE(8312),

            SDCODE_VOICE(8337),
            SDCODE_VOICE(8363),
            SDCODE_VOICE(8390),
            SDCODE_VOICE(8418),
            SDCODE_VOICE(8238),
            SDCODE_VOICE(8263),
            SDCODE_VOICE(8288),
            SDCODE_VOICE(8313),
            SDCODE_VOICE(8338),
            SDCODE_VOICE(8364),

            SDCODE_VOICE(8391),
            SDCODE_VOICE(8419),
            SDCODE_VOICE(8239),
            SDCODE_VOICE(8264),
            SDCODE_VOICE(8289),
            SDCODE_VOICE(8314),
            SDCODE_VOICE(8339),
            SDCODE_VOICE(8365),
            SDCODE_VOICE(8392),
            SDCODE_VOICE(8420),
        };

        int32 nNumWallJump = pPlayerChr->GetNumWallJump();
        if (nNumWallJump >= 1)
        {
            if (nNumWallJump < 5)
                nNumWallJump = nNumWallJump - 1;
            else
                nNumWallJump = 4;
        }
        else
        {
            nNumWallJump = 0;
        };

        PlayCharaSE(pPlayerChr, &aiVoiceList[nNumWallJump * PLAYERID::ID_MAX]);
    };


    void CallVoiceOfAABBB(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8222),
            SDCODE_VOICE(8247),
            SDCODE_VOICE(8272),
            SDCODE_VOICE(8297),
            SDCODE_VOICE(8322),
            SDCODE_VOICE(8348),
            SDCODE_VOICE(8374),
            SDCODE_VOICE(8402),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfAABBC(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8223),
            SDCODE_VOICE(8248),
            SDCODE_VOICE(8273),
            SDCODE_VOICE(8298),
            SDCODE_VOICE(8323),
            SDCODE_VOICE(8349),
            SDCODE_VOICE(8375),
            SDCODE_VOICE(8403),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfComboHit(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8230),
            SDCODE_VOICE(8255),
            SDCODE_VOICE(8280),
            SDCODE_VOICE(8305),
            SDCODE_VOICE(8330),
            SDCODE_VOICE(8356),
            SDCODE_VOICE(8382),
            SDCODE_VOICE(8410),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };


    void CallVoiceOfGuardImpact(const CPlayerCharacter* pPlayerChr)
    {
        int32 aiVoiceList[] =
        {
            SDCODE_VOICE(8234),
            SDCODE_VOICE(8259),
            SDCODE_VOICE(8284),
            SDCODE_VOICE(8309),
            SDCODE_VOICE(8334),
            SDCODE_VOICE(8360),
            SDCODE_VOICE(8386),
            SDCODE_VOICE(8415),
        };

        PlayCharaSE(pPlayerChr, aiVoiceList);
    };
};