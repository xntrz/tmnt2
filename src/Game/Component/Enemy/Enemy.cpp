#include "Enemy.hpp"
#include "EnemyCharacter.hpp"
#ifdef _DEBUG
#include "EnemyDebug.hpp"
#endif /* _DEBUG */

#include "ConcreteEnemyCharacter/000Dummy.hpp"
#include "ConcreteEnemyCharacter/001PurpleDragonGang.hpp"
#include "ConcreteEnemyCharacter/002FootNinja.hpp"
#include "ConcreteEnemyCharacter/003FootNinja.hpp"
#include "ConcreteEnemyCharacter/004FeudalFootNinja.hpp"
#include "ConcreteEnemyCharacter/005FeudalFootNinja.hpp"
#include "ConcreteEnemyCharacter/006UtromsSecurityPatrol.hpp"
#include "ConcreteEnemyCharacter/007FederationSoldier.hpp"
#include "ConcreteEnemyCharacter/010Mobster.hpp"
#include "ConcreteEnemyCharacter/011FootArcherNinja.hpp"
#include "ConcreteEnemyCharacter/012FeudalFootArcherNinja.hpp"
#include "ConcreteEnemyCharacter/013FederationShooter.hpp"
#include "ConcreteEnemyCharacter/014ExoSuitedUtroms.hpp"
#include "ConcreteEnemyCharacter/015CaptainOfFederation.hpp"
#include "ConcreteEnemyCharacter/020LargeMobster.hpp"
#include "ConcreteEnemyCharacter/021LargeFootNinja.hpp"
#include "ConcreteEnemyCharacter/022FeudalLargeFootNinja.hpp"
#include "ConcreteEnemyCharacter/023LargeFederationSoldier.hpp"
#include "ConcreteEnemyCharacter/024LargeExoSuitedUtroms.hpp"
#include "ConcreteEnemyCharacter/030FootTechNinja.hpp"
#include "ConcreteEnemyCharacter/031FootSumoNinja.hpp"
#include "ConcreteEnemyCharacter/032GunnerFootMech.hpp"
#include "ConcreteEnemyCharacter/033CaptainOfFeudalFootNinja.hpp"
#include "ConcreteEnemyCharacter/035TriceratonSoldier.hpp"
#include "ConcreteEnemyCharacter/036TriceratonShooter.hpp"
#include "ConcreteEnemyCharacter/037TriceratonFlyingHarness.hpp"
#include "ConcreteEnemyCharacter/040StoneBiter.hpp"
#include "ConcreteEnemyCharacter/041Bergox.hpp"
#include "ConcreteEnemyCharacter/042SpiderMonster.hpp"
#include "ConcreteEnemyCharacter/043Redurian.hpp"
#include "ConcreteEnemyCharacter/044KingNail.hpp"
#include "ConcreteEnemyCharacter/045Greenpus.hpp"
#include "ConcreteEnemyCharacter/046Razorfist.hpp"
#include "ConcreteEnemyCharacter/047MadMuncher.hpp"
#include "ConcreteEnemyCharacter/048Rynokk.hpp"
#include "ConcreteEnemyCharacter/050BloodSucker.hpp"
#include "ConcreteEnemyCharacter/051PoisonBat.hpp"
#include "ConcreteEnemyCharacter/052HypnosisBat.hpp"
#include "ConcreteEnemyCharacter/053BombBatMech.hpp"
#include "ConcreteEnemyCharacter/055Kurage.hpp"
#include "ConcreteEnemyCharacter/056Kabuto.hpp"
#include "ConcreteEnemyCharacter/057UtromSaucer.hpp"
#include "ConcreteEnemyCharacter/060MouserRobot.hpp"
#include "ConcreteEnemyCharacter/061MouserRobotB.hpp"
#include "ConcreteEnemyCharacter/062KrokodilMouser.hpp"
#include "ConcreteEnemyCharacter/063Raptor.hpp"
#include "ConcreteEnemyCharacter/065CarnivorousPlant.hpp"
#include "ConcreteEnemyCharacter/066MuralCarnivorousPlant.hpp"
#include "ConcreteEnemyCharacter/084Karai.hpp"
#include "ConcreteEnemyCharacter/098Fugitoid.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/System/Misc/DebugShape.hpp"


static CEnemyCharacter* CreateEnemyCharacter(ENEMYID::VALUE id)
{
    if (!ENEMYID::GetImplementProgress(id))
        return nullptr;

    switch (id)
    {
    case ENEMYID::ID_DUMMY:
        return nullptr;

    case ENEMYID::ID_PURPLE_DRAGON_GANG:
        return new C001PurpleDragonGang;

    case ENEMYID::ID_FOOT_NINJA_SWORD:
        return new C002FootNinja;

    case ENEMYID::ID_FOOT_NINJA_STAFF:
        return new C003FootNinja;

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD:
        return new C004FeudalFootNinja;

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF:
        return new C005FeudalFootNinja;

    case ENEMYID::ID_UTROMS_SECURITY_PATROL:
        return new C006UtromsSecurityPatrol;

    case ENEMYID::ID_FEDERATION_SOLDIER:
        return new C007FederationSoldier;

    case ENEMYID::ID_MOBSTER:
        return new C010Mobster;

    case ENEMYID::ID_FOOT_NINJA_ARCHER:
        return new C011FootArcherNinja;

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER:
        return new C012FeudalFootArcherNinja;

    case ENEMYID::ID_FEDERATION_SHOOTER:
        return new C013FederationShooter;

    case ENEMYID::ID_EXO_SUITED_UTROMS:
        return new C014ExoSuitedUtroms;

    case ENEMYID::ID_CAPTAIN_OF_FEDERATION:
        return new C015CaptainOfFederation;

    case ENEMYID::ID_LARGE_MOBSTER:
        return new C020LargeMobster;

    case ENEMYID::ID_LARGE_FOOT_NINJA:
        return new C021LargeFootNinja;

    case ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA:
        return new C022FeudalLargeFootNinja;

    case ENEMYID::ID_LARGE_FEDERATION_SOLDIET:
        return new C023LargeFederationSoldier;

    case ENEMYID::ID_LARGE_EXO_SUITED_UTROMS:
        return new C024LargeExoSuitedUtroms;

    case ENEMYID::ID_FOOT_TECH_NINJA:
        return new C030FootTechNinja;

    case ENEMYID::ID_FOOT_SUMO_NINJA:
        return new C031FootSumoNinja;

    case ENEMYID::ID_FOOT_GUNNER_FOOT_MECH:
        return new C032GunnerFootMech;

    case ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA:
        return new C033CaptainOfFeudalFootNinja;

    case ENEMYID::ID_TRICERATION_SOLDIER:
        return new C035TriceratonSoldier;

    case ENEMYID::ID_TRICERATION_SHOOTER:
        return new C036TriceratonShooter;

    case ENEMYID::ID_TRICERATION_FLYING_HARNESS:
        return new C037TriceratonFlyingHarness;

    case ENEMYID::ID_STONE_BITER:
        return new C040StoneBiter;

    case ENEMYID::ID_BERGOX:
        return new C041Bergox;

    case ENEMYID::ID_SPIDER_MONSTER:
        return new C042SpiderMonster;

    case ENEMYID::ID_REDURION:
        return new C043Redurian;

    case ENEMYID::ID_KING_NAIL:
        return new C044KingNail;

    case ENEMYID::ID_GREENPUS:
        return new C045Greenpus;

    case ENEMYID::ID_RAZORFIST:
        return new C046Razorfist;

    case ENEMYID::ID_MAD_MUNCHER:
        return new C047MadMuncher;

    case ENEMYID::ID_RYNOKK:
        return new C048Rynokk;

    case ENEMYID::ID_BLOOD_SUCKER:
        return new C050BloodSucker;

    case ENEMYID::ID_POISON_BAT:
        return new C051PoisonBat;

    case ENEMYID::ID_HYPNOSIS_BAT:
        return new C052HypnosisBat;

    case ENEMYID::ID_BOMB_BAT_MECH:
        return new C053BombBatMech;

    case ENEMYID::ID_KURAGE:
        return new C055Kurage;

    case ENEMYID::ID_KABUTO:
        return new C056Kabuto;

    case ENEMYID::ID_UTROM_SAUCER:
        return new C057UtromSaucer;

    case ENEMYID::ID_MOUSER_ROBOT:
        return new C060MouserRobot;

    case ENEMYID::ID_MOUSER_ROBOT_B:
        return new C061MouserRobotB;

    case ENEMYID::ID_KROKODIL_MOUSER:
        return new C062KrokodilMouser;

    case ENEMYID::ID_RAPTOR:
        return new C063Raptor;

    case ENEMYID::ID_CARNIVOROUS_PLANT:
        return new C065CarnivorousPlant;

    case ENEMYID::ID_MURAL_CARNIVOROUS_PLANT:
        return new C066MuralCarnivorousPlant;

    case ENEMYID::ID_KARAI:
        return new C084Karai;

    case ENEMYID::ID_FUGITOID:
        return new C098Fugitoid;

    default:
        break;
    };

    return nullptr;
};


static EFFECTID::VALUE GetEnemyCharacterNeededEffect(ENEMYID::VALUE enemyId, int32 no)
{
    switch (enemyId)
    {
    case ENEMYID::ID_DUMMY:
        return EFFECTID::ID_UNKNOWN;

    case ENEMYID::ID_PURPLE_DRAGON_GANG:
        return C001PurpleDragonGang::GetNeededEffect(no);

    case ENEMYID::ID_FOOT_NINJA_SWORD:
        return C002FootNinja::GetNeededEffect(no);

    case ENEMYID::ID_FOOT_NINJA_STAFF:
        return C003FootNinja::GetNeededEffect(no);

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD:
        return C004FeudalFootNinja::GetNeededEffect(no);

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF:
        return C005FeudalFootNinja::GetNeededEffect(no);

    case ENEMYID::ID_UTROMS_SECURITY_PATROL:
        return C006UtromsSecurityPatrol::GetNeededEffect(no);

    case ENEMYID::ID_FEDERATION_SOLDIER:
        return C007FederationSoldier::GetNeededEffect(no);

    case ENEMYID::ID_MOBSTER:
        return C010Mobster::GetNeededEffect(no);

    case ENEMYID::ID_FOOT_NINJA_ARCHER:
        return C011FootArcherNinja::GetNeededEffect(no);

    case ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER:
        return C012FeudalFootArcherNinja::GetNeededEffect(no);

    case ENEMYID::ID_FEDERATION_SHOOTER:
        return C013FederationShooter::GetNeededEffect(no);

    case ENEMYID::ID_EXO_SUITED_UTROMS:
        return C014ExoSuitedUtroms::GetNeededEffect(no);

    case ENEMYID::ID_CAPTAIN_OF_FEDERATION:
        return C015CaptainOfFederation::GetNeededEffect(no);

    case ENEMYID::ID_LARGE_MOBSTER:
        return C020LargeMobster::GetNeededEffect(no);

    case ENEMYID::ID_LARGE_FOOT_NINJA:
        return C021LargeFootNinja::GetNeededEffect(no);

    case ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA:
        return C022FeudalLargeFootNinja::GetNeededEffect(no);

    case ENEMYID::ID_LARGE_FEDERATION_SOLDIET:
        return C023LargeFederationSoldier::GetNeededEffect(no);

    case ENEMYID::ID_LARGE_EXO_SUITED_UTROMS:
        return C024LargeExoSuitedUtroms::GetNeededEffect(no);

    case ENEMYID::ID_FOOT_TECH_NINJA:
        return C030FootTechNinja::GetNeededEffect(no);

    case ENEMYID::ID_FOOT_SUMO_NINJA:
        return C031FootSumoNinja::GetNeededEffect(no);

    case ENEMYID::ID_FOOT_GUNNER_FOOT_MECH:
        return C032GunnerFootMech::GetNeededEffect(no);

    case ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA:
        return C033CaptainOfFeudalFootNinja::GetNeededEffect(no);

    case ENEMYID::ID_TRICERATION_SOLDIER:
        return C035TriceratonSoldier::GetNeededEffect(no);

    case ENEMYID::ID_TRICERATION_SHOOTER:
        return C036TriceratonShooter::GetNeededEffect(no);

    case ENEMYID::ID_TRICERATION_FLYING_HARNESS:
        return C037TriceratonFlyingHarness::GetNeededEffect(no);

    case ENEMYID::ID_STONE_BITER:
        return C040StoneBiter::GetNeededEffect(no);

    case ENEMYID::ID_BERGOX:
        return C041Bergox::GetNeededEffect(no);

    case ENEMYID::ID_SPIDER_MONSTER:
        return C042SpiderMonster::GetNeededEffect(no);

    case ENEMYID::ID_REDURION:
        return C043Redurian::GetNeededEffect(no);

    case ENEMYID::ID_KING_NAIL:
        return C044KingNail::GetNeededEffect(no);

    case ENEMYID::ID_GREENPUS:
        return C045Greenpus::GetNeededEffect(no);

    case ENEMYID::ID_RAZORFIST:
        return C046Razorfist::GetNeededEffect(no);

    case ENEMYID::ID_MAD_MUNCHER:
        return C047MadMuncher::GetNeededEffect(no);

    case ENEMYID::ID_RYNOKK:
        return C048Rynokk::GetNeededEffect(no);

    case ENEMYID::ID_BLOOD_SUCKER:
        return C050BloodSucker::GetNeededEffect(no);

    case ENEMYID::ID_POISON_BAT:
        return C051PoisonBat::GetNeededEffect(no);

    case ENEMYID::ID_HYPNOSIS_BAT:
        return C052HypnosisBat::GetNeededEffect(no);

    case ENEMYID::ID_BOMB_BAT_MECH:
        return C053BombBatMech::GetNeededEffect(no);

    case ENEMYID::ID_KURAGE:
        return C055Kurage::GetNeededEffect(no);

    case ENEMYID::ID_KABUTO:
        return C056Kabuto::GetNeededEffect(no);

    case ENEMYID::ID_UTROM_SAUCER:
        return C057UtromSaucer::GetNeededEffect(no);

    case ENEMYID::ID_MOUSER_ROBOT:
        return C060MouserRobot::GetNeededEffect(no);

    case ENEMYID::ID_MOUSER_ROBOT_B:
        return C061MouserRobotB::GetNeededEffect(no);

    case ENEMYID::ID_KROKODIL_MOUSER:
        return C062KrokodilMouser::GetNeededEffect(no);

    case ENEMYID::ID_RAPTOR:
        return C063Raptor::GetNeededEffect(no);

    case ENEMYID::ID_CARNIVOROUS_PLANT:
        return C065CarnivorousPlant::GetNeededEffect(no);

    case ENEMYID::ID_MURAL_CARNIVOROUS_PLANT:
        return C066MuralCarnivorousPlant::GetNeededEffect(no);

    case ENEMYID::ID_KARAI:
        return C084Karai::GetNeededEffect(no);

    case ENEMYID::ID_FUGITOID:
        return C098Fugitoid::GetNeededEffect(no);

    default:
        break;
    };

    return EFFECTID::ID_UNKNOWN;
};


static STAGEID::VALUE GetEnemyCharacterStageId(ENEMYID::VALUE enemyId)
{
    switch (enemyId)
    {
    case ENEMYID::ID_KARAI:
        return STAGEID::ID_ST57NB;

    default:
        break;
    };

    return STAGEID::ID_NONE;
};


/*static*/ int32 CEnemy::m_iRefCount = 0;
/*static*/ int32 CEnemy::m_iUniqueCount = 0;


/*static*/ CEnemy* CEnemy::New(const ENEMYTYPES::CREATEINFO* pCreateInfo)
{
    char szEnemyName[GAMEOBJECTTYPES::NAME_MAX];
    szEnemyName[0] = '\0';
    
    std::sprintf(szEnemyName, "e%03d_%04d", pCreateInfo->m_idEnemy, m_iUniqueCount);

#ifdef _DEBUG
    if (CEnemyDebug::SHOW_ENEMY_SPAWN_INFO)
    {
        OUTPUT(
            "Creating \"%s\" (\"%s\") enemy at %03f | %03f | %03f\n",
            ENEMYID::GetName(pCreateInfo->m_idEnemy),
            szEnemyName,
            pCreateInfo->m_vPosition.x,
            pCreateInfo->m_vPosition.y,
            pCreateInfo->m_vPosition.z
        );

        RwSphere sphere = { 0 };
        sphere.center = pCreateInfo->m_vPosition;
        sphere.radius = 0.5f;

        CDebugShape::m_fDuration = 3.0f;
        CDebugShape::ShowSphere(&sphere, { 0xFF, 0xFF, 0x00, 0xFF });
        CDebugShape::ShowLabel(&pCreateInfo->m_vPosition, ENEMYID::GetName(pCreateInfo->m_idEnemy));
        CDebugShape::m_fDuration = 0.0f;
    };
#endif /* _DEBUG */

    CEnemyCharacter::PreInitialize(pCreateInfo);
    CEnemyCharacter* pEnemyCharacter = CreateEnemyCharacter(pCreateInfo->m_idEnemy);

    if (!pEnemyCharacter)
        return nullptr;

    CEnemy* pEnemy = new CEnemy(szEnemyName, pCreateInfo->m_idEnemy, pEnemyCharacter);
    if (pEnemy)
    {
        pEnemyCharacter->SetOwner(pEnemy->GetHandle());
        
        ++m_iRefCount;
        ++m_iUniqueCount;

        CGameEvent::SetEnemyCreated(pEnemy);
    }
    else
    {
        delete pEnemyCharacter;
        pEnemyCharacter = nullptr;
    };

    return pEnemy;
};


/*static*/ EFFECTID::VALUE CEnemy::GetNeededEffect(ENEMYID::VALUE enemyId, int32 no)
{
    return GetEnemyCharacterNeededEffect(enemyId, no);
};


/*static*/ STAGEID::VALUE CEnemy::GetNeededStage(ENEMYID::VALUE enemyId)
{
    return GetEnemyCharacterStageId(enemyId);
};


CEnemy::CEnemy(const char* pszName, ENEMYID::VALUE idEnemy, CEnemyCharacter* pEnemyChr)
: CGameObject(pszName, GAMEOBJECTTYPE::ENEMY)
, m_pEnemyCharacter(pEnemyChr)
{
    ASSERT(m_pEnemyCharacter);
};


CEnemy::~CEnemy(void)
{
    CGameEvent::SetEnemyDestroyed(this);

    if (m_pEnemyCharacter)
    {
        delete m_pEnemyCharacter;
        m_pEnemyCharacter = nullptr;
    };

    ASSERT(m_iRefCount > 0);
    if (!--m_iRefCount)
        m_iUniqueCount = 0;
};


void CEnemy::Kill(void)
{
    ENEMYTYPES::STATUS status = Character().GetStatus();

    if ((status == ENEMYTYPES::STATUS_CAUGHT) ||
        (status == ENEMYTYPES::STATUS_LIFTED) ||
        (status == ENEMYTYPES::STATUS_LIFTED_WALK))
        return;

    delete this;
};


void CEnemy::Start(void)
{
    Character().Start();
};


void CEnemy::StartAI(void)
{
    Character().StartAI();
};


void CEnemy::StopAI(void)
{
    Character().StopAI();
};


void CEnemy::GetPosition(RwV3d* pvPosition) const
{
    Character().GetBodyPosition(pvPosition);
};


int32 CEnemy::GetHPMax(void) const
{
    return Character().CharacterParameter().m_feature.m_iHPMax;
};


int32 CEnemy::GetHP(void)
{
    return Character().CharacterParameter().m_feature.m_iHP;
};


CEnemyCharacter& CEnemy::Character(void)
{
    ASSERT(m_pEnemyCharacter);
    return *m_pEnemyCharacter;
};


const CEnemyCharacter& CEnemy::Character(void) const
{
    ASSERT(m_pEnemyCharacter);
    return *m_pEnemyCharacter;
};