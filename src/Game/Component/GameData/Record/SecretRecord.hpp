#pragma once

#include "Game/Component/GameMain/SecretID.hpp"


class CSecretRecord
{
public:
    static const int32 FLAGNUM = ((SECRETID::ID_MAX / BITSOF(uint32)) + 1);

    struct RAWDATA
    {
        uint32 m_auUnlockedFlag[FLAGNUM];
        uint32 m_auUnnotifiedFlag[FLAGNUM];
        float  m_fAttackEnchanceValue;
        float  m_fDefenceEnchanceValue;
    };

    enum FLAGTYPE
    {
        FLAGTYPE_UNLOCKED = 0,
        FLAGTYPE_UNNOTIFIED,

        FLAGTYPEMAX,
    };

public:
    static bool m_bDebugNotifyUnlocked;
    
    CSecretRecord(void);
    ~CSecretRecord(void);
    void SetDefault(void);
    bool IsValid(void) const;
    void SetNotSavedFlagOff(void);
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);    
    bool IsUnlockedSecret(SECRETID::VALUE idSecret) const;
    void UnlockSecret(SECRETID::VALUE idSecret);
    void NotifySecret(SECRETID::VALUE idSecret);
    int32 GetAttackLevel(void) const;
    int32 GetDefenceLevel(void) const;
    int32 GetAerialLevel(void) const;
    int32 GetChargeLevel(void) const;
    bool IsDonLazerEnabled(void) const;
    float GetAttackEnchanceValue(void) const;
    float GetDefenceEnchanceValue(void) const;
    SECRETID::VALUE GetUnnotifiedSecret(void);

private:
    void setOffGroupA(void);
    void setOffGroupB(void);
    bool isValidSecretID(SECRETID::VALUE idSecret) const;
    bool isDatabase(SECRETID::VALUE idSecret) const;
    void setFlag(FLAGTYPE flagtype, SECRETID::VALUE idSecret, bool bSet);
    bool getFlag(FLAGTYPE flagtype, SECRETID::VALUE idSecret) const;

private:
    uint32 m_auUnlockedFlag[FLAGNUM];
    uint32 m_auUnnotifiedFlag[FLAGNUM];
    float m_fAttackEnchanceValue;
    float m_fDefenceEnchanceValue;
};