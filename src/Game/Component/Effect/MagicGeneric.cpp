#include "MagicGeneric.hpp"
#include "MagicManager.hpp"

#include "Game/System/Character/Character.hpp"
#include "Game/Component/GameData/GameData.hpp"


static inline float GetPower(void)
{
    float fPower = CGameData::Record().Secret().GetAttackEnhanceValue();
    
    int32 nLevel = CGameData::Record().Secret().GetAttackLevel();
    if (nLevel >= 3)
        fPower *= 2.0f;

    return fPower;
};


namespace MAGIC_GENERIC
{
    void ChargeAttackLeonardo(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step)
    {
        CMagicManager::CParameter param;
        param.SetPositon(pvPosition);
        param.SetDirection(fDirection);
        param.SetObject(pCharacter);

        uint32 hMagic = CMagicManager::Play(MAGICID::ID_LEO_SP, &param);

        switch (step)
        {
        case STEP_ONE:
            CMagicManager::SetSpeed(hMagic, 3.0f);
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 10.0f));
            break;

        case STEP_TWO:
            CMagicManager::SetSpeed(hMagic, 6.0f);
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 20.0f));
            break;

        case STEP_THREE:
            CMagicManager::SetSpeed(hMagic, 9.0f);
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 30.0f));
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void ChargeAttackRaphael(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step)
    {
        CMagicManager::CParameter param;
        param.SetPositon(pvPosition);
        param.SetDirection(fDirection);
        param.SetObject(pCharacter);

        uint32 hMagic = CMagicManager::Play(MAGICID::ID_RAP_SP, &param);

        switch (step)
        {
        case STEP_ONE:
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 30.0f));
            break;

        case STEP_TWO:
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 40.0f));
            break;

        case STEP_THREE:
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 50.0f));
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void ChargeAttackMichelangero(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step)
    {
        CMagicManager::CParameter param;
        param.SetPositon(pvPosition);
        param.SetObject(pCharacter);

        float fPower = GetPower();
        const float fAngleStep = MATH_DEG2RAD(30.0f);
        
        switch (step)
        {
        case STEP_ONE:
            {
                param.SetDirection(fDirection);
                
                uint32 hEffect = CMagicManager::Play(MAGICID::ID_MIC_SP, &param);
                CMagicManager::SetDamage(hEffect, static_cast<int32>(fPower * 10.0f));
            }
            break;

        case STEP_TWO:
            {
                for (int32 i = 0; i < 3; ++i)
                {
                    switch (i)
                    {
                    case 0:
                        param.SetDirection(fDirection);
                        break;

                    case 1:
                        param.SetDirection(fDirection - fAngleStep);
                        break;

                    case 2:
                        param.SetDirection(fDirection + fAngleStep);
                        break;

                    default:
                        break;
                    };

                    uint32 hEffect = CMagicManager::Play(MAGICID::ID_MIC_SP, &param);
                    CMagicManager::SetDamage(hEffect, static_cast<int32>(fPower * 20.0f));
                };
            }
            break;

        case STEP_THREE:
            {
                for (int32 i = 0; i < 5; ++i)
                {
                    switch (i)
                    {
                    case 0:
                        param.SetDirection(fDirection);
                        break;

                    case 1:
                        param.SetDirection(fDirection - fAngleStep);
                        break;

                    case 2:
                        param.SetDirection(fDirection + fAngleStep);
                        break;

                    case 3:
                        param.SetDirection(fDirection - (fAngleStep * 2.0f));
                        break;

                    case 4:
                        param.SetDirection(fDirection + (fAngleStep * 2.0f));
                        break;

                    default:
                        break;
                    };

                    uint32 hEffect = CMagicManager::Play(MAGICID::ID_MIC_SP, &param);                    
                    CMagicManager::SetDamage(hEffect, static_cast<int32>(fPower * 30.0f));
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
    

    void ChargeAttackSlashuur(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step)
    {
        CMagicManager::CParameter param;
        param.SetPositon(pvPosition);
        param.SetDirection(fDirection);
        param.SetObject(pCharacter);

        uint32 hMagic = CMagicManager::Play(MAGICID::ID_SLA_SP, &param);

        switch (step)
        {
        case STEP_ONE:
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 20.0f));
            break;

        case STEP_TWO:
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 30.0f));
            break;

        case STEP_THREE:
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 40.0f));
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void ChargeAttackCasey(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step)
    {
        CMagicManager::CParameter param;
        param.SetPositon(pvPosition);
        param.SetObject(pCharacter);

        float fPower = GetPower();
        const float fAngleStep = MATH_DEG2RAD(30.0f);

        switch (step)
        {
        case STEP_ONE:
            {
                param.SetDirection(fDirection);

                uint32 hEffect = CMagicManager::Play(MAGICID::ID_CAS_SP, &param);
                CMagicManager::SetDamage(hEffect, static_cast<int32>(fPower * 10.0f));
            }
            break;

        case STEP_TWO:
            {
                for (int32 i = 0; i < 3; ++i)
                {
                    switch (i)
                    {
                    case 0:
                        param.SetDirection(fDirection);
                        break;

                    case 1:
                        param.SetDirection(fDirection - fAngleStep);
                        break;

                    case 2:
                        param.SetDirection(fDirection + fAngleStep);
                        break;

                    default:
                        break;
                    };

                    uint32 hEffect = CMagicManager::Play(MAGICID::ID_CAS_SP, &param);
                    CMagicManager::SetDamage(hEffect, static_cast<int32>(fPower * 20.0f));
                };
            }
            break;

        case STEP_THREE:
            {
                for (int32 i = 0; i < 5; ++i)
                {
                    switch (i)
                    {
                    case 0:
                        param.SetDirection(fDirection);
                        break;

                    case 1:
                        param.SetDirection(fDirection - fAngleStep);
                        break;

                    case 2:
                        param.SetDirection(fDirection + fAngleStep);
                        break;

                    case 3:
                        param.SetDirection(fDirection - (fAngleStep * 2.0f));
                        break;

                    case 4:
                        param.SetDirection(fDirection + (fAngleStep * 2.0f));
                        break;
                    };

                    uint32 hEffect = CMagicManager::Play(MAGICID::ID_CAS_SP, &param);                    
                    CMagicManager::SetDamage(hEffect, static_cast<int32>(fPower * 30.0f));
                };
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };


    void ChargeAttackKarai(const RwV3d* pvPosition, float fDirection, CCharacter* pCharacter, STEP step)
    {
        CMagicManager::CParameter param;
        param.SetPositon(pvPosition);
        param.SetDirection(fDirection);
        param.SetObject(pCharacter);

        uint32 hMagic = CMagicManager::Play(MAGICID::ID_KAR_SP, &param);

        switch (step)
        {
        case STEP_ONE:
            CMagicManager::SetSpeed(hMagic, 6.0f);
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 10.0f));
            break;

        case STEP_TWO:
            CMagicManager::SetSpeed(hMagic, 12.0f);
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 20.0f));
            break;

        case STEP_THREE:
            CMagicManager::SetSpeed(hMagic, 18.0f);
            CMagicManager::SetDamage(hMagic, static_cast<int32>(GetPower() * 30.0f));
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};