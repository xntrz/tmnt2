#include "EffectGeneric.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Map/WorldMap.hpp"


static void CallHitEffectCommon(const RwV3d* pvPosition, int32 nPower)
{
    if (nPower >= GAMETYPES::ATTACKPOWER_MAX)
        CEffectManager::Play(EFFECTID::ID_ALL_HIT_B, pvPosition);
    else
        CEffectManager::Play(EFFECTID::ID_ALL_HIT_S, pvPosition);
};


static void CallCharacterHitEffect(const CCharacter* pCharacter, const RwV3d* pvPosition, int32 nPower)
{
    ASSERT(pCharacter);
    ASSERT(pvPosition);
    
    switch (pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_ENEMY:
        {
            CallHitEffectCommon(pvPosition, nPower);
        }
        break;

    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerCharacter = (CPlayerCharacter*)pCharacter;
            switch (pPlayerCharacter->GetID())
            {
            case PLAYERID::ID_LEO:
            case PLAYERID::ID_SLASHUUR:
                {
                    if (nPower >= GAMETYPES::ATTACKPOWER_MAX)
                        CEffectManager::Play(EFFECTID::ID_LEO_HIT_B, pvPosition);
                    else
                        CEffectManager::Play(EFFECTID::ID_LEO_HIT_S, pvPosition);
                }
                break;
                
            case PLAYERID::ID_RAPH:
            case PLAYERID::ID_CASEY:
                {
                    if (nPower >= GAMETYPES::ATTACKPOWER_MAX)
                        CEffectManager::Play(EFFECTID::ID_RAP_HIT_B, pvPosition);
                    else
                        CEffectManager::Play(EFFECTID::ID_RAP_HIT_S, pvPosition);
                }
                break;
                
            case PLAYERID::ID_MIC:
            case PLAYERID::ID_KARAI:
                {
                    if (nPower >= GAMETYPES::ATTACKPOWER_MAX)
                        CEffectManager::Play(EFFECTID::ID_MIC_HIT_B, pvPosition);
                    else
                        CEffectManager::Play(EFFECTID::ID_MIC_HIT_S, pvPosition);
                }
                break;
                
            case PLAYERID::ID_DON:
            case PLAYERID::ID_SPLINTER:
                {
                    if (nPower >= GAMETYPES::ATTACKPOWER_MAX)
                        CEffectManager::Play(EFFECTID::ID_DON_HIT_B, pvPosition);
                    else
                        CEffectManager::Play(EFFECTID::ID_DON_HIT_S, pvPosition);
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


static void CallObjectHitEffect(const CGameObject* pGameObject, const RwV3d* pvPosition, int32 nPower)
{
    ASSERT(pGameObject);
    
    switch (pGameObject->GetType())
    {
    case GAMEOBJECTTYPE::GIMMICK:
        CallHitEffectCommon(pvPosition, nPower);
        break;
        
    case GAMEOBJECTTYPE::CHARACTER:
        {
            CCharacter* pCharacter = (CCharacter*)pGameObject;
            
            CallCharacterHitEffect(pCharacter, pvPosition, nPower);
        }
        break;
        
    case GAMEOBJECTTYPE::EFFECT:
        {
            CEffect* pEffect = (CEffect*)pGameObject;

            switch (pEffect->GetEffectType())
            {
            case CEffect::TYPE_WITHHIT:
                {
                    CMagic* pMagic = (CMagic*)pEffect;
                    
                    uint32 hParent = pMagic->GetParent();
                    if (CGameObjectManager::GetObject(hParent))
                        CallHitEffectCommon(pvPosition, nPower);                    
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;
        
    case GAMEOBJECTTYPE::SHOT:
        CallHitEffectCommon(pvPosition, nPower);
        break;
    };
};


namespace EFFECT_GENERIC
{
    void CallHitEffect(CHitAttackData* pAttack, const RwV3d* pvPosition)
    {
        CallObjectHitEffect(pAttack->GetObject(), pvPosition, pAttack->GetPower());
    };


    void CallDeathEffect(DEATHTYPE type, const RwV3d* pvPosition, float fScale, bool bPlaySound)
    {
        switch (type)
        {
        case DEATHTYPE_NORMAL:
            {
                uint32 hEffect = CEffectManager::Play(EFFECTID::ID_KO_FLASH, pvPosition);
                ASSERT(hEffect);
                if (hEffect)
                    CEffectManager::SetScale(hEffect, fScale);
            }
            break;
            
        case DEATHTYPE_MONSTER:
            {
                CMagicManager::CParameter param;
                param.SetPositon(pvPosition);
                param.SetSoundPlay(bPlaySound);
                
                uint32 hMagic = CMagicManager::Play(MAGICID::ID_KO_EXPL, &param);
                ASSERT(hMagic);
                if (hMagic)
                    CMagicManager::SetScale(hMagic, fScale);
            }
            break;
            
        case DEATHTYPE_MACHINE:
            {
                uint32 hEffect = CEffectManager::Play(EFFECTID::ID_KO_SMOKE, pvPosition);
                ASSERT(hEffect);
                if (hEffect)
                    CEffectManager::SetScale(hEffect, fScale);
            }
            break;
            
        default:
            ASSERT(false);
            break;
        };
    };


    void CallTouchDownEffect(MAPTYPES::ATTRIBUTE attribute, const RwV3d* pvPosition, float fScale, bool bPlaySound)
    {
        ASSERT(pvPosition);
        
        if (attribute == MAPTYPES::ATTRIBUTE_DOBON ||
            attribute == MAPTYPES::ATTRIBUTE_POISON ||
            attribute == MAPTYPES::ATTRIBUTE_MAGMA ||
            attribute == MAPTYPES::ATTRIBUTE_WATER)
        {
            RwV3d vPosition = *pvPosition;
            CWorldMap::GetWaterSurfacePosition(&vPosition);
            
            uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_W_DOBON, &vPosition, bPlaySound);
            ASSERT(hEffect);
            CEffectManager::SetScale(hEffect, fScale);
        }
        else if (attribute == MAPTYPES::ATTRIBUTE_UNKNOWN ||
                attribute == MAPTYPES::ATTRIBUTE_SNOW ||
                attribute == MAPTYPES::ATTRIBUTE_SLIP ||
                attribute == MAPTYPES::ATTRIBUTE_SAND ||
                attribute == MAPTYPES::ATTRIBUTE_MESH)
        {
            RwV3d vPosition = *pvPosition;
            vPosition.y += 0.2f;
            
            uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vPosition, bPlaySound);
            ASSERT(hEffect);
            CEffectManager::SetScale(hEffect, fScale);
        };
    };


    bool IsNeedWaterEffect(MAPTYPES::ATTRIBUTE attribute)
    {
        return (IS_FLAG_SET(attribute, MAPTYPES::ATTRIBUTE_WATER) ||
                IS_FLAG_SET(attribute, MAPTYPES::ATTRIBUTE_POISON));
    };
};