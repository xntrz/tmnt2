#pragma once


class CPlayerCharacter;


namespace PlayerUtil
{
    bool IsPowerfullCharacter(const CPlayerCharacter* pPlayerChr);
    void CallSEOfTouchdown(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfChargeAttack(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfPassive(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfLiftUp(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfBackThrow(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfJumpWall(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfAABBB(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfAABBC(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfComboHit(const CPlayerCharacter* pPlayerChr);
    void CallVoiceOfGuardImpact(const CPlayerCharacter* pPlayerChr);
};