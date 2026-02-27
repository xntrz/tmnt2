#include "MagicSetup.hpp"
#include "Magic.hpp"
#include "MagicManager.hpp"
#include "EffectManager.hpp"
#include "MagicParameter.hpp"

#include "Game/System/Hit/HitAttackData.hpp"


static void SetLeonardoAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vVelocity = { 9.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LEO_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(1.0f);
    param.SetCollisionAttack(1.0f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetMovement(&vVelocity);
    param.SetLive(0.5f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_LEO_SP), &param);
};


static void SetRaphaelAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_RAP_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(1.7f);
    param.SetCollisionAttack(1.7f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(1.1f);
    param.SetHitTiming(0.0f, 0.5f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_RAP_SP), &param);
};


static void SetDonatelloLaser(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_LOCUS_HOR
                                | MAGICTYPES::FEATURE_LOCUS_VER
                                | MAGICTYPES::FEATURE_COLLISION_REFLECTION
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_COLLISION_HIT_NUM
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    MAGICTYPES::LOCUSINFO locusinfo = {};
    locusinfo.m_Color       = { 0x99, 0x33, 0xFF, 0xFF };
    locusinfo.m_uAlphaBasis = 0x80;
    locusinfo.m_nNumPoint   = 20;
    locusinfo.m_fRadius     = 0.15f;

    RwV3d vVelocity = { 25.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_BEAM));
    param.SetFeature(feature);
    param.SetLocusInfo(locusinfo);
    param.SetReflectNum(5);
    param.SetCollisionBody(0.3f);
    param.SetCollisionAttack(0.3f, 5, CHitAttackData::ANTIGUARD_ENABLE, CHitAttackData::STATUS_DINDLE, 2.0f);
    param.SetMovement(&vVelocity);
    param.SetLive(5.0f);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_FIRE));
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_HIT));
    param.SetReflectEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_HIT));

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_BEAM), &param);
};


static void SetMichelangeroAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vVelocity = { 20.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_MIC_SP_BALL));
    param.SetFeature(feature);
    param.SetCollisionBody(0.35f);
    param.SetCollisionAttack(0.35f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetMovement(&vVelocity);
    param.SetLive(0.25f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_MIC_SP_END));

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_MIC_SP_BALL), &param);
};


static void SetSlashuurAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SLA_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(2.4f);
    param.SetCollisionAttack(2.4f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(1.0f);
    param.SetHitTiming(0.0f, 0.5f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_SLA_SP), &param);
};


static void SetKaraiAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vVelocity = { 18.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_KAR_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(1.0f);
    param.SetCollisionAttack(1.0f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetMovement(&vVelocity);
    param.SetLive(0.25f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_KAR_SP), &param);
};


static void SetCaseyAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_PARENT
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vVelocity = { 20.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_CAS_SP_BALL));
    param.SetFeature(feature);
    param.SetCollisionBody(0.35f);
    param.SetCollisionAttack(0.35f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetMovement(&vVelocity);
    param.SetLive(0.25f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_CAS_SP_END));

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_CAS_SP_BALL), &param);
};


static void SetDeathExplosion(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        
                                | MAGICTYPES::FEATURE_LIVETIME                
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_KO_EXPL));
    param.SetFeature(feature);
    param.SetCollisionAttack(0.7f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(0.5f);
    param.SetHitTiming(0.0f, 0.3f);
    param.SetChangeSize(3.0f);

    CMagicManager::ChangeToCommon();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_KO_EXPL), &param);
};


static void SetExplosion(void)
{
    //
    //  Base feature & param
    //
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1));
    param.SetLive(1.5f);
    param.SetHitTiming(0.0f, 0.5f);
    param.SetChangeSize(0.35f);
    
    //
    //  All
    //
    MAGICTYPES::FEATURE featureAll = feature
                                   | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                   | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                   | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER;

    param.SetFeature(featureAll);
    param.SetCollisionAttack(1.12f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);

    CMagicManager::ChangeToCommon();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1), &param);

    //
    //  Player
    //
    MAGICTYPES::FEATURE featurePlayer = feature
                                      | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER;

    param.SetFeature(featurePlayer);
    param.SetCollisionAttack(1.12f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1_PLAYER), &param);

    //
    //  Chara
    //
    MAGICTYPES::FEATURE featureChara = feature
                                     | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                     | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER;

    param.SetFeature(featureChara);
    param.SetCollisionAttack(1.12f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1_CHARA), &param);
};


static void SetSleepgas(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;
    
    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SLEEPGAS));
    param.SetFeature(feature);
    param.SetCollisionAttack(1.1f, 0, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_SLEEP, 2.0f);
    param.SetLive(2.0f + (1.0f / 3.0f));
    param.SetHitTiming(0.0f, 1.6f);
    param.SetChangeSize(0.4f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_SLEEPGAS), &param);
};


static void SetFreezegas(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_FREEZEGAS));
    param.SetFeature(feature);
    param.SetCollisionAttack(1.1f, 0, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FREEZE, 2.0f);
    param.SetLive(2.0f + (1.0f / 3.0f));
    param.SetHitTiming(0.0f, 0.9f);
    param.SetChangeSize(0.4f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_FREEZEGAS), &param);
};


static void SetFireTorch(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, 0.15f, 0.0f };
    
    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_FIRE_TORCH));
    param.SetFeature(feature);
    param.SetCollisionAttack(&vOffset, 0.15f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetLive(0.2f + (0.1f / 1.5f));

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_FIRE_TORCH), &param);
};


static void SetPipeSteam(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, 0.0f, 1.7f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_STEAM));
    param.SetFeature(feature);
    param.SetCollisionAttack(&vOffset, 1.0f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(7.0f);
    param.SetHitTiming(2.0f - 0.0001f, 6.0f - 0.00051f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_STEAM), &param);
};


static void SetPipeFreezegas(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, 0.0f, 1.7f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_FREEZEGAS));
    param.SetFeature(feature);
    param.SetCollisionAttack(&vOffset, 1.0f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FREEZE, 2.0f);
    param.SetLive(7.0f);
    param.SetHitTiming(2.0f - 0.0001f, 6.0f - 0.00051f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_FREEZEGAS), &param);
};


static void SetFireHydrantWater(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_GRAVITY
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, -0.3f, 1.8f };
    RwV3d vMovement = { 19.0f, 0.0f, 0.0f };
    
    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SPRAYWTER));
    param.SetFeature(feature);
    param.SetCollisionBody(&vOffset, 0.5f);
    param.SetCollisionAttack(&vOffset, 0.5f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetMovement(&vMovement);
    param.SetLive(0.5f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_SPRAYWTER), &param);
};


static void SetStickynessSputum(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_APPEAR_HORIZON
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_GRAVITY
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vMovement = { 10.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SPSM_TBSHOT));
    param.SetFeature(feature);
    param.SetCollisionBody(0.46f);
    param.SetCollisionAttack(0.46f, 5, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_BIND, 2.0f);
    param.SetMovement(&vMovement);
    param.SetReflectNum(1);
    param.SetLive(2.0f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_SPSM_TBSHOT), &param);
};


static void SetTenguSword(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vOffset = { 0.0f, 0.0f, 0.7f };
    RwV3d vMovement = { 10.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_TENGSWORD));
    param.SetFeature(feature);
    param.SetCollisionBody(0.3f);
    param.SetCollisionAttack(&vOffset, 0.3f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetMovement(&vMovement);
    param.SetReflectNum(1);
    param.SetLive(4.0f);
    param.SetScale(1.3f);
    param.SetVanishMagicName(MAGICID::GetNameFromID(MAGICID::ID_EXPL_B1_PLAYER));

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_TENGSWORD), &param);
};


static void SetThunder(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_THUNDER));
    param.SetFeature(feature);
    param.SetCollisionAttack(1.0f, 30, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(0.7f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_THUNDER), &param);
};


static void SetShredderEnergyBomb(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_HOMING
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vMovement = { 5.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SHRD_ELBALL));
    param.SetFeature(feature);
    param.SetCollisionBody(0.3f);
    param.SetCollisionAttack(0.3f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetMovement(&vMovement);
    param.SetReflectNum(1);
    param.SetLive(4.0f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SHRD_ELEND));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_SHRD_ELBALL), &param);
};


static void SetDorakoForceField(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DRKBALL));
    param.SetFeature(feature);
    param.SetCollisionAttack(1.0f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetLive(2.0f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DRKBALL_END));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_DRKBALL), &param);
};


static void SetPillarOfLight(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, 2.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DRK_PILLAR));
    param.SetFeature(feature);
    param.SetCollisionAttack(&vOffset, 2.0f, 30, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(1.5f);
    param.SetHitTiming(0.3f, 1.1f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_DRK_PILLAR), &param);
};


static void SetRodBeam(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_LOCUS_HOR
                                | MAGICTYPES::FEATURE_LOCUS_VER
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    MAGICTYPES::LOCUSINFO locusInfo = {};
    locusInfo.m_nNumPoint   = 20;
    locusInfo.m_Color       = { 0x67, 0x4D, 0xFF, 0xFF };
    locusInfo.m_uAlphaBasis = 0x80;
    locusInfo.m_fRadius     = 0.17f;

    RwV3d vMovement = { 25.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_ROD_BEAM));
    param.SetFeature(feature);
    param.SetCollisionBody(0.25f);
    param.SetCollisionAttack(0.25f, 20, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetMovement(&vMovement);
    param.SetLocusInfo(locusInfo);
    param.SetReflectNum(5);
    param.SetLive(5.0f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_ROD_BEAM_HIT));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_ROD_BEAM), &param);
};


static void SetSmokescreenAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_ULNIN_ENMAKU));
    param.SetFeature(feature);
    param.SetCollisionAttack(&vOffset, 1.5f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_SLEEP, 2.0f);
    param.SetLive(1.8f);
    param.SetHitTiming(0.0f, 1.3f);
    param.SetChangeSize(0.0f);    

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_ULNIN_ENMAKU), &param);
};


static void SetAbsorptionEnergy(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_HOMING
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME;

    RwV3d vMovement = { 5.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_HPSTEAL));
    param.SetFeature(feature);
    param.SetCollisionBody(0.3f);
    param.SetCollisionAttack(0.3f, 0, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetMovement(&vMovement);
    param.SetLive(94608000.0f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_HPSTEAL_LIGHT));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_HPSTEAL), &param);
};


static void SetPurpleEnergyBall(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vMovement = { 15.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SLABALL));
    param.SetFeature(feature);
    param.SetCollisionBody(0.3f);
    param.SetCollisionAttack(0.3f, 15, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetMovement(&vMovement);
    param.SetLive(4.0f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SLABALL_END));
    param.SetVanishMagicName(MAGICID::GetNameFromID(MAGICID::ID_EXPL_B1_PLAYER));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_SLABALL), &param);
};


static void SetWindPress(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_WIND_PRESS));
    param.SetFeature(feature);
    param.SetCollisionAttack(1.0f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_FLYAWAY);
    param.SetLive(0.8f);
    param.SetHitTiming(0.0f, 0.5f);
    param.SetChangeSize(2.0f);

	CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_WIND_PRESS), &param);
};


static void SetLaserBlue(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_LOCUS_HOR
                                | MAGICTYPES::FEATURE_LOCUS_VER
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;


    MAGICTYPES::LOCUSINFO locusinfo = {};
    locusinfo.m_nNumPoint   = 12;
    locusinfo.m_Color       = { 0x00, 0x20, 0xFF, 0xFF };
    locusinfo.m_uAlphaBasis = 0x80;
    locusinfo.m_fRadius     = 0.1f;

    RwV3d vVelocity = { 25.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_BEAM_FED));
    param.SetFeature(feature);
    param.SetLocusInfo(locusinfo);
    param.SetCollisionBody(0.15f);
    param.SetCollisionAttack(0.15f, 5, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_DINDLE, 2.0f);
    param.SetMovement(&vVelocity);
    param.SetReflectNum(5); // NOTE: reflect count is set but feature not present 
    param.SetLive(5.0f);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_FIRE_FED));
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_HIT_FED));
    param.SetReflectEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_HIT_FED));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_BEAM_FED), &param);
};


static void SetLaserRed(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_LOCUS_HOR
                                | MAGICTYPES::FEATURE_LOCUS_VER
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    MAGICTYPES::LOCUSINFO locusinfo = {};
    locusinfo.m_nNumPoint   = 12;
    locusinfo.m_Color       = { 0xFF, 0x20, 0xFF, 0xFF };   // SetLaserRed ???
    locusinfo.m_uAlphaBasis = 0x80;
    locusinfo.m_fRadius     = 0.1f;

    RwV3d vVelocity = { 25.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_BEAM_TRI));
    param.SetFeature(feature);
    param.SetLocusInfo(locusinfo);
    param.SetCollisionBody(0.15f);
    param.SetCollisionAttack(0.15f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_DINDLE, 2.0f);
    param.SetMovement(&vVelocity);
    param.SetReflectNum(5); // NOTE: reflect count is set but feature not present 
    param.SetLive(5.0f);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_FIRE_TRI));
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_HIT_TRI));
    param.SetReflectEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_HIT_TRI));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_BEAM_TRI), &param);
};


static void SetEnergyBallGun(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vMovement = { 15.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_GUNBALL));
    param.SetFeature(feature);
    param.SetCollisionBody(0.2f);
    param.SetCollisionAttack(0.2f, 15, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetMovement(&vMovement);
    param.SetLive(3.0f);
    param.SetReflectNum(1);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_GUNBALL_FIRE));
    param.SetVanishMagicName(MAGICID::GetNameFromID(MAGICID::ID_EXPL_B1_PLAYER));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_GUNBALL), &param);
};


static void SetSpiderNet(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_GRAVITY
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE;

    RwV3d vOffset = { 0.0f, 0.0f, -0.5f };
    RwV3d vMovement = { 19.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SPIDERNET));
    param.SetFeature(feature);
    param.SetCollisionBody(&vOffset, 0.5f);
    param.SetCollisionAttack(&vOffset, 0.5f, 0, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_BIND, 2.0f);
    param.SetMovement(&vMovement);
    param.SetLive(3.0f);
    param.SetChangeSize(4.0f);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SPIDERNET));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_SPIDERNET), &param);
};


static void SetMonsterSaliva(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_APPEAR_HORIZON
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_GRAVITY
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    RwV3d vMovement = { 10.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_TBSHOT));
    param.SetFeature(feature);
    param.SetCollisionBody(0.4f);
    param.SetCollisionAttack(0.4f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK, 2.0f);
    param.SetConfusionFlag(true);
    param.SetMovement(&vMovement);
    param.SetLive(3.0f);
    param.SetVanishMagicName(MAGICID::GetNameFromID(MAGICID::ID_TBSHOT_MARK));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_TBSHOT), &param);
};


static void SetMonsterDrool(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK;

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_TBSHOT_MARK));
    param.SetFeature(feature);
    param.SetCollisionAttack(0.8f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK, 2.0f);
    param.SetConfusionFlag(true);
    param.SetLive(0.9f);
    param.SetHitTiming(0.0f, 0.6f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_TBSHOT_MARK), &param);
};


static void SetLaserCommon(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_LOST_IMMEDIATE
                                | MAGICTYPES::FEATURE_LOCUS_HOR
                                | MAGICTYPES::FEATURE_LOCUS_VER
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                                | MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_STRAIGHT
                                | MAGICTYPES::FEATURE_LIVETIME
                                | MAGICTYPES::FEATURE_ATTACK
                                | MAGICTYPES::FEATURE_BODY;

    MAGICTYPES::LOCUSINFO locusinfo = {};
    locusinfo.m_nNumPoint   = 12;
    locusinfo.m_Color       = { 0xFF, 0xFF, 0x0, 0xFF };
    locusinfo.m_uAlphaBasis = 0x80;
    locusinfo.m_fRadius     = 0.1f;

    RwV3d vVelocity = { 25.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_BEAM_ALL));
    param.SetFeature(feature);
    param.SetLocusInfo(locusinfo);
    param.SetCollisionBody(0.15f);
    param.SetCollisionAttack(0.15f, 5, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetMovement(&vVelocity);
    param.SetReflectNum(5);
    param.SetLive(5.0f);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_FIRE_ALL));
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_HIT_ALL));
    param.SetReflectEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_HIT_ALL));

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(EFFECTID::GetNameFromID(EFFECTID::ID_LASER_BEAM_ALL), &param);
};


static void SetFlameMap(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE
                                | MAGICTYPES::FEATURE_ATTACK_TO_ENEMY
                                | MAGICTYPES::FEATURE_ATTACK_TO_PLAYER
                                | MAGICTYPES::FEATURE_ATTACK;

    RwV3d vOffset = { 0.0f, 0.6f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_FLAME_MAP));
    param.SetFeature(feature);
    param.SetCollisionAttack(&vOffset, 0.75f, 10, CHitAttackData::ANTIGUARD_INVALID, CHitAttackData::STATUS_KNOCK);
    param.SetLive(0.4f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(EFFECTID::GetNameFromID(EFFECTID::ID_FLAME_MAP), &param);
};


namespace MAGIC_SETUP
{
    void CreateMagic(const char* pszName)
    {
        CreateMagic(EFFECTID::GetIDFromName(pszName));
    };


    void CreateMagic(EFFECTID::VALUE idEffect)
    {
        switch (idEffect)
        {
        case EFFECTID::ID_LEO_SP:
            SetLeonardoAccumulateAttack();
            break;

        case EFFECTID::ID_RAP_SP:
            SetRaphaelAccumulateAttack();
            break;

        case EFFECTID::ID_DON_LASER_BEAM:
            SetDonatelloLaser();
            break;

        case EFFECTID::ID_MIC_SP_BALL:
            SetMichelangeroAccumulateAttack();
            break;

        case EFFECTID::ID_SLA_SP:
            SetSlashuurAccumulateAttack();
            break;

        case EFFECTID::ID_KAR_SP:
            SetKaraiAccumulateAttack();
            break;

        case EFFECTID::ID_CAS_SP_BALL:
            SetCaseyAccumulateAttack();
            break;

        case EFFECTID::ID_KO_EXPL:
            SetDeathExplosion();
            break;

        case EFFECTID::ID_EXPL_B1:
            SetExplosion();
            break;

        case EFFECTID::ID_SLEEPGAS:
            SetSleepgas();
            break;

        case EFFECTID::ID_FREEZEGAS:
            SetFreezegas();
            break;

        case EFFECTID::ID_FIRE_TORCH:
            SetFireTorch();
            break;

        case EFFECTID::ID_PIPE_STEAM:
            SetPipeSteam();
            break;

        case EFFECTID::ID_PIPE_FREEZEGAS:
            SetPipeFreezegas();
            break;

        case EFFECTID::ID_SPRAYWTER:
            SetFireHydrantWater();
            break;

        case EFFECTID::ID_SPSM_TBSHOT:
            SetStickynessSputum();
            break;

        case EFFECTID::ID_TENGSWORD:
            SetTenguSword();
            break;

        case EFFECTID::ID_THUNDER:
            SetThunder();
            break;

        case EFFECTID::ID_SHRD_ELBALL:
            SetShredderEnergyBomb();
            break;

        case EFFECTID::ID_DRKBALL:
            SetDorakoForceField();
            break;

        case EFFECTID::ID_DRK_PILLAR:
            SetPillarOfLight();
            break;

        case EFFECTID::ID_ROD_BEAM:
            SetRodBeam();
            break;

        case EFFECTID::ID_ULNIN_ENMAKU:
            SetSmokescreenAttack();
            break;

        case EFFECTID::ID_HPSTEAL:
            SetAbsorptionEnergy();
            break;

        case EFFECTID::ID_SLABALL:
            SetPurpleEnergyBall();
            break;

        case EFFECTID::ID_WIND_PRESS:
            SetWindPress();
            break;

        case EFFECTID::ID_LASER_BEAM_FED:
            SetLaserBlue();
            break;

        case EFFECTID::ID_LASER_BEAM_TRI:
            SetLaserRed();
            break;

        case EFFECTID::ID_GUNBALL:
            SetEnergyBallGun();
            break;

        case EFFECTID::ID_SPIDERNET:
            SetSpiderNet();
            break;

        case EFFECTID::ID_TBSHOT:
            SetMonsterSaliva();
            break;

        case EFFECTID::ID_TBSHOT_MARK:
            SetMonsterDrool();
            break;

        case EFFECTID::ID_LASER_BEAM_ALL:
            SetLaserCommon();
            break;

        case EFFECTID::ID_FLAME_MAP:
            SetFlameMap();
            break;

        default:
            break;
        };
    };
};