#pragma once


class CCharacter;


namespace MAGIC_GENERIC
{
    enum STEP
    {
        STEP_ONE = 0,
        STEP_TWO,
        STEP_THREE,
    };

    void ChargeAttackLeonardo(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step);
    void ChargeAttackRaphael(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step);
    void ChargeAttackMichelangero(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step);
    void ChargeAttackSlashuur(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step);
    void ChargeAttackCasey(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step);
    void ChargeAttackKarai(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step);
};