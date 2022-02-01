#include "MagicSetup.hpp"
#include "Magic.hpp"
#include "MagicManager.hpp"
#include "EffectManager.hpp"
#include "MagicParameter.hpp"

#include "Game/System/Hit/HitAttackData.hpp"


static void SetLeonardoAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE      |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ    |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY     |
        MAGICTYPES::FEATURE_PARENT              |
        MAGICTYPES::FEATURE_STRAIGHT            |
        MAGICTYPES::FEATURE_LIVETIME            |
        MAGICTYPES::FEATURE_ATTACK
    );

    RwV3d vVelocity = { 9.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_LEO_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(nullptr, 1.0f);
    param.SetCollisionAttack(
        nullptr,
        1.0f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_KNOCK,
        0.0f,
        0.0f,
        0.0f
    );
    param.SetMovement(&vVelocity, nullptr);
    param.SetLive(0.5f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_LEO_SP),
        &param
    );
};


static void SetRaphaelAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE      |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ    |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY     |
        MAGICTYPES::FEATURE_PARENT              |
        MAGICTYPES::FEATURE_STRAIGHT            |
        MAGICTYPES::FEATURE_LIVETIME            |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_RAP_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(nullptr, 1.7f);
    param.SetCollisionAttack(
        nullptr,
        1.7f,
        20,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );
    param.SetLive(1.1f);
    param.SetHitTiming(0.0f, 0.5f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_RAP_SP),
        &param
    );
};


static void SetDonatelloLaser(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE          |
        MAGICTYPES::FEATURE_LOCUS_HOR               |
        MAGICTYPES::FEATURE_LOCUS_VER               |
        MAGICTYPES::FEATURE_COLLISION_REFLECTION    |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_HIT              |
        MAGICTYPES::FEATURE_COLLISION_MAPOBJECT     |
        MAGICTYPES::FEATURE_COLLISION_MAP           |
        MAGICTYPES::FEATURE_PARENT                  |
        MAGICTYPES::FEATURE_STRAIGHT                |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_COLLISION_HIT_NUM       |
        MAGICTYPES::FEATURE_ATTACK                  |
        MAGICTYPES::FEATURE_BODY
    );

    MAGICTYPES::LOCUSINFO locusinfo = { 0 };
    locusinfo.m_Color = { 0x99, 0x33, 0xFF, 0xFF };
    locusinfo.m_uAlphaBasis = 0x80;
    locusinfo.m_nNumPoint = 20;
    locusinfo.m_fRadius = 0.15f;

    RwV3d vVelocity = { 25.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_BEAM));
    param.SetFeature(feature);
    param.SetLocusInfo(locusinfo);
    param.SetReflectNum(5);
    param.SetCollisionBody(nullptr, 0.3f);
    param.SetCollisionAttack(
        nullptr,
        0.3f,
        5,
        CHitAttackData::ANTIGUARD_ENABLE,
        CHitAttackData::STATUS_DINDLE,
        2.0f
    );
    param.SetMovement(&vVelocity, nullptr);
    param.SetLive(5.0f);
    param.SetAppearEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_FIRE));
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_HIT));
    param.SetReflectEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_HIT));

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_DON_LASER_BEAM),
        &param
    );
};


static void SetMichelangeroAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE          |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_HIT              |
        MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT |
        MAGICTYPES::FEATURE_COLLISION_MAP_HIT       |
        MAGICTYPES::FEATURE_COLLISION_MAPOBJECT     |
        MAGICTYPES::FEATURE_COLLISION_MAP           |
        MAGICTYPES::FEATURE_PARENT                  |
        MAGICTYPES::FEATURE_STRAIGHT                |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK                  |
        MAGICTYPES::FEATURE_BODY
    );

    RwV3d vVelocity = { 20.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_MIC_SP_BALL));
    param.SetFeature(feature);
    param.SetCollisionBody(nullptr, 0.34f);
    param.SetCollisionAttack(
        nullptr,
        0.34f,
        5,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );
    param.SetMovement(&vVelocity, nullptr);
    param.SetLive(0.25f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_MIC_SP_END));

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_MIC_SP_BALL),
        &param
    );
};


static void SetSlashuurAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE      |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ    |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY     |
        MAGICTYPES::FEATURE_PARENT              |
        MAGICTYPES::FEATURE_STRAIGHT            |
        MAGICTYPES::FEATURE_LIVETIME            |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SLA_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(nullptr, 2.4f);
    param.SetCollisionAttack(
        nullptr,
        2.4f,
        20,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );
    param.SetLive(1.0f);
    param.SetHitTiming(0.0f, 0.5f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_SLA_SP),
        &param
    );
};


static void SetKaraiAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE      |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ    |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY     |
        MAGICTYPES::FEATURE_PARENT              |
        MAGICTYPES::FEATURE_STRAIGHT            |
        MAGICTYPES::FEATURE_LIVETIME            |
        MAGICTYPES::FEATURE_ATTACK
    );

    RwV3d vVelocity = { 18.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_KAR_SP));
    param.SetFeature(feature);
    param.SetCollisionBody(nullptr, 1.0f);
    param.SetCollisionAttack(
        nullptr,
        1.0f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_KNOCK
    );
    param.SetMovement(&vVelocity, nullptr);
    param.SetLive(0.25f);

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_KAR_SP),
        &param
    );
};


static void SetCaseyAccumulateAttack(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_LOST_IMMEDIATE          |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_HIT              |
        MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT |
        MAGICTYPES::FEATURE_COLLISION_MAP_HIT       |
        MAGICTYPES::FEATURE_COLLISION_MAPOBJECT     |
        MAGICTYPES::FEATURE_COLLISION_MAP           |
        MAGICTYPES::FEATURE_PARENT                  |
        MAGICTYPES::FEATURE_STRAIGHT                |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK                  |
        MAGICTYPES::FEATURE_BODY
    );

    RwV3d vVelocity = { 20.0f, 0.0f, 0.0f };

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_CAS_SP_BALL));
    param.SetFeature(feature);
    param.SetCollisionBody(nullptr, 0.34f);
    param.SetCollisionAttack(
        nullptr,
        0.34f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );
    param.SetMovement(&vVelocity, nullptr);
    param.SetLive(0.25f);
    param.SetVanishEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_CAS_SP_END));

    CMagicManager::ChangeToCommon();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_CAS_SP_BALL),
        &param
    );
};


static void SetDeathExplosion(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_KO_EXPL));
    param.SetFeature(feature);
    param.SetCollisionAttack(
        nullptr,
        0.70f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );
    param.SetLive(0.5f);
    param.SetHitTiming(0.0f, 0.3f);
    param.SetChangeSize(3.0f);

    CMagicManager::ChangeToCommon();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_KO_EXPL),
        &param
    );
};


static void SetExplosion(void)
{
    MAGICTYPES::FEATURE featureAll = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    MAGICTYPES::FEATURE featurePlayer = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    MAGICTYPES::FEATURE featureChara = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1));
    param.SetLive(1.5f);
    param.SetHitTiming(0.0f, 0.3f);
    param.SetChangeSize(0.35f);
    
    //
    //  All
    //
    param.SetFeature(featureAll);
    param.SetCollisionAttack(
        nullptr,
        1.12f,
        20,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );

    CMagicManager::ChangeToCommon();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1),
        &param
    );

    //
    //  Player
    //
    param.SetFeature(featurePlayer);
    param.SetCollisionAttack(
        nullptr,
        1.12f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1_PLAYER),
        &param
    );

    //
    //  Chara
    //
    param.SetFeature(featureChara);
    param.SetCollisionAttack(
        nullptr,
        1.12f,
        20,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_EXPL_B1_CHARA),
        &param
    );
};


static void SetSleepgas(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_SLEEPGAS));
    param.SetFeature(feature);
    param.SetCollisionAttack(
        nullptr,
        1.1f,
        0,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_SLEEP,
        2.0f
    );
    param.SetLive(2.33f);
    param.SetHitTiming(0.0f, 1.6f);
    param.SetChangeSize(0.4f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_SLEEPGAS),
        &param
    );
};


static void SetFreezegas(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_FREEZEGAS));
    param.SetFeature(feature);
    param.SetCollisionAttack(
        nullptr,
        1.1f,
        0,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FREEZE,
        2.0f
    );
    param.SetLive(2.33f);
    param.SetHitTiming(0.0f, 0.9f);
    param.SetChangeSize(0.4f);

    CMagicManager::ChangeToAttached();
    CEffectManager::ConvertWithDestroy(
        EFFECTID::GetNameFromID(EFFECTID::ID_FREEZEGAS),
        &param
    );
};


static void SetFireTorch(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_FIRE_TORCH));
    param.SetFeature(feature);

    RwV3d vOffset = { 0.0f, 0.15f,0.0f };
    param.SetCollisionAttack(
        &vOffset,
        0.15f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_KNOCK
    );
    param.SetLive(0.26f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_FIRE_TORCH),
        &param
    );
};


static void SetPipeSteam(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_STEAM));
    param.SetFeature(feature);

    RwV3d vOffset = { 0.0f, 0.0f, 1.7f };
    param.SetCollisionAttack(
        &vOffset,
        1.0f,
        20,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FLYAWAY
    );
    param.SetLive(7.0f);
    param.SetHitTiming(2.0f, 6.0f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_STEAM),
        &param
    );
};


static void SetPipeFreezegas(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ        |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_LIVETIME                |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_FREEZEGAS));
    param.SetFeature(feature);

    RwV3d vOffset = { 0.0f, 0.0f, 1.7f };
    param.SetCollisionAttack(
        &vOffset,
        1.0f,
        20,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_FREEZE,
        2.0f
    );
    param.SetLive(7.0f);
    param.SetHitTiming(2.0f, 6.0f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_PIPE_FREEZEGAS),
        &param
    );
};


static void SetFireHydrantWater(void)
{
    ;
};


static void SetStickynessSputum(void)
{
    ;
};


static void SetTenguSword(void)
{
    ;
};


static void SetThunder(void)
{
    ;
};


static void SetShredderEnergyBomb(void)
{
    ;
};


static void SetDorakoForceField(void)
{
    ;
};


static void SetPillarOfLight(void)
{
    ;
};


static void SetRodBeam(void)
{
    ;
};


static void SetSmokescreenAttack(void)
{
    ;
};


static void SetAbsorptionEnergy(void)
{
    ;
};


static void SetPurpleEnergyBall(void)
{
    ;
};


static void SetWindPress(void)
{
    ;
};


static void SetLaserBlue(void)
{
    ;
};


static void SetLaserRed(void)
{
    ;
};


static void SetEnergyBallGun(void)
{
    ;
};


static void SetSpiderNet(void)
{
    ;
};


static void SetMonsterSaliva(void)
{
    ;
};


static void SetMonsterDrool(void)
{
    ;
};


static void SetLaserCommon(void)
{
    ;
};


static void SetFlameMap(void)
{
    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE(
        MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE   |
        MAGICTYPES::FEATURE_ATTACK_TO_ENEMY         |
        MAGICTYPES::FEATURE_ATTACK_TO_PLAYER        |
        MAGICTYPES::FEATURE_ATTACK
    );

    CMagicParameter param;
    param.SetBaseEffectName(EFFECTID::GetNameFromID(EFFECTID::ID_FLAME_MAP));
    param.SetFeature(feature);

    RwV3d vOffset = { 0.0f, 0.6f, 0.0f };
    param.SetCollisionAttack(
        &vOffset,
        0.75f,
        10,
        CHitAttackData::ANTIGUARD_INVALID,
        CHitAttackData::STATUS_KNOCK
    );
    param.SetLive(0.4f);

    CMagicManager::ChangeToAttached();
    CEffectManager::Convert(
        EFFECTID::GetNameFromID(EFFECTID::ID_FLAME_MAP),
        &param
    );
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
            //ASSERT(false);
            break;
        };
    };
};