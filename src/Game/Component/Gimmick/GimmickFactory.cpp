#include "GimmickFactory.hpp"
#include "GimmickParam.hpp"
#ifdef _DEBUG
#include "GimmickDebug.hpp"
#endif

#include "ConcreteGimmick/SystemGimmick.hpp"
#include "ConcreteGimmick/EventCheckGimmick.hpp"
#include "ConcreteGimmick/DescentCityGimmick.hpp"
#include "ConcreteGimmick/MapEffectGimmick.hpp"
#include "ConcreteGimmick/ItemGimmick.hpp"
#include "ConcreteGimmick/ThrowingGimmick.hpp"
#include "ConcreteGimmick/CuttingGimmick.hpp"
#include "ConcreteGimmick/GrassGimmick.hpp"
#include "ConcreteGimmick/PipeGimmick.hpp"
#include "ConcreteGimmick/SwitchGimmick.hpp"
#include "ConcreteGimmick/ConsoleGimmick.hpp"
#include "ConcreteGimmick/NormalDoorGimmick.hpp"
#include "ConcreteGimmick/PushingGimmick.hpp"
#include "ConcreteGimmick/TransferFloorGimmick.hpp"
#include "ConcreteGimmick/LaserGimmick.hpp"
#include "ConcreteGimmick/RideFlagGimmick.hpp"
#include "ConcreteGimmick/RideRockGimmick.hpp"
#include "ConcreteGimmick/RideMineGimmick.hpp"
#include "ConcreteGimmick/RideShutterGimmick.hpp"
#include "ConcreteGimmick/StopperGimmick.hpp"
#include "ConcreteGimmick/MapModelCtrlGimmick.hpp"
#include "ConcreteGimmick/UniqueTransferFloorGimmick.hpp"
#include "ConcreteGimmick/BridgePillarGimmick.hpp"
#include "ConcreteGimmick/MadCarGimmick.hpp"
#include "ConcreteGimmick/MadCarGimmickManager.hpp"
#include "ConcreteGimmick/FallGimmick.hpp"
#include "ConcreteGimmick/BreakableCarGimmick.hpp"
#include "ConcreteGimmick/BreakableGimmick.hpp"
#include "ConcreteGimmick/FallRubbleGimmick.hpp"
#include "ConcreteGimmick/FallRubbleGimmickManager.hpp"
#include "ConcreteGimmick/RollingRockGimmick.hpp"
#include "ConcreteGimmick/UtromGimmick.hpp"
#include "ConcreteGimmick/FenceGimmick.hpp"
#include "ConcreteGimmick/FenceGimmickManager.hpp"
#include "ConcreteGimmick/CrumbleFloorGimmick.hpp"
#include "ConcreteGimmick/GSMachineGimmick.hpp"
#include "ConcreteGimmick/PillarGimmick.hpp"
#include "ConcreteGimmick/MineGimmick.hpp"
#include "ConcreteGimmick/BeltConveyorGimmick.hpp"
#include "ConcreteGimmick/SeesawGimmick.hpp"
#include "ConcreteGimmick/GuillotineGimmick.hpp"
#include "ConcreteGimmick/DoorGimmick.hpp"
#include "ConcreteGimmick/GeneratorGimmick.hpp"
#include "ConcreteGimmick/EggGimmick.hpp"
#include "ConcreteGimmick/CaveGimmick.hpp"
#include "ConcreteGimmick/SpaceShipGimmick.hpp"
#include "ConcreteGimmick/CrystalGimmick.hpp"
#include "ConcreteGimmick/ElevatorGimmick.hpp"
#include "ConcreteGimmick/GearGimmick.hpp"
#include "ConcreteGimmick/PistonGimmick.hpp"
#include "ConcreteGimmick/PropellerGimmick.hpp"
#include "ConcreteGimmick/HatchGimmick.hpp"
#include "ConcreteGimmick/GateGimmick.hpp"
#include "ConcreteGimmick/ApproachingTrailerGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"


/*static*/ CGimmick* CGimmickFactory::Create(GIMMICKID::VALUE idGimmick, int32 subid, void* pParam)
{    
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_COMMON_HEAD* pHeader = static_cast<GIMMICKPARAM::GIMMICK_COMMON_HEAD*>(pParam);
    const char* pszName = pHeader->m_szName;

    CGimmick* pGimmick = nullptr;

#ifdef _DEBUG
    if (CGimmickDebug::DISABLE_GENERERATOR && (idGimmick >= GIMMICKID::GENERATORSTART) && (idGimmick < GIMMICKID::GENERATOREND))
        return pGimmick;
#endif    

    switch (idGimmick)
    {
    //
    //  System
    //
    case GIMMICKID::ID_S_ANDSWI:
        pGimmick = new CAndSwitchGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_GROUP:
        pGimmick = new CGroupGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_GCLEAR:
    case GIMMICKID::ID_S_GOVER:
        pGimmick = new CGameEndGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_MOVIE:
        //
        //  Not exists
        //
        break;
        
    case GIMMICKID::ID_S_CAMERA:
        pGimmick = new CCameraChangeGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_EVCAM:
        //
        //  Not exists
        //
        break;
        
    case GIMMICKID::ID_S_SOUND:
        pGimmick = new CSEGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_WALL:
        pGimmick = new CInvisibleWallGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_FALLPT:
        //
        //  Not exists
        //
        break;
        
    case GIMMICKID::ID_S_RESET:
        pGimmick = new CReplaceGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_S_HELP:
        if (CGameData::Option().Display().IsHelpEnabled())
            pGimmick = new CHelpGimmick(pszName, pParam);        
        break;
        
    case GIMMICKID::ID_S_TUTO:
        if (CGameData::Option().Display().IsHelpEnabled())
           pGimmick = new CTutorialGimmick(pszName, pParam);        
        break;
        
    case GIMMICKID::ID_S_EKILL:
        pGimmick = new CEnemyKillGimmick(pszName, pParam);
        break;


    //
    //  Event
    //
    case GIMMICKID::ID_E_PDEAD:
        //
        //  Not exists
        //
        break;
        
    case GIMMICKID::ID_E_TIME:
        pGimmick = new CTimeCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_EBRK:
    case GIMMICKID::ID_E_EBRKAL:
        pGimmick = new CEnemyCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_PWALL:
    case GIMMICKID::ID_E_PSPHER:
    case GIMMICKID::ID_E_PVOLUM:
        pGimmick = new CPlayerAreaCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_GBRK:
        pGimmick = new CBrokenCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_EWALL:
        pGimmick = new CEnemyAreaCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_UTRVOL:
        pGimmick = new CUtromAreaCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_DONLSR:
        pGimmick = new CDonLaserGetCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_RRSPHR:
        pGimmick = new CGimmickObjAreaCheckGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_E_FTWALL:
        pGimmick = new CFugitoidAreaCheckGimmick(pszName, pParam);
        break;


    //
    //   Generator
    //
    case GIMMICKID::ID_G_SET:
        pGimmick = new CEnemySettingGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_G_GROUND:
    case GIMMICKID::ID_G_AIR:
        pGimmick = new CEnemyGeneratePointGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_G_EGG:
        pGimmick = new CEggGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_G_CAVE:
        pGimmick = new CCaveGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_G_DOOR:
        pGimmick = new CDoorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_G_CAR:
        //
        //  Not exist
        //
        break;
        
    case GIMMICKID::ID_G_SSHIPS:
        pGimmick = new CSpaceShipGimmick(pszName, pParam);
        break;


    //
    //  Normal
    //
    case GIMMICKID::ID_N_THROW:
    case GIMMICKID::ID_N_ITEMBX:
        pGimmick = new CThrowingGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_BRKCAR:
        pGimmick = new CBreakableCarGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_BRKOBJ:
        pGimmick = new CBreakableGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_ROLOBJ:
        pGimmick = new CRollingRockGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_MVFLLI:
        pGimmick = new CLinearTransferFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_MVFLPA:
        pGimmick = new CPathTransferFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_MVFLRO:
        pGimmick = new CRotateTransferFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_CONSOL:
        pGimmick = new CConsoleGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_SWITCH:
        pGimmick = new CSwitchGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_SWIPIL:
        pGimmick = new CSwitchPillarGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_BRKPIL:
    case GIMMICKID::ID_N_SHKPIL:
    case GIMMICKID::ID_N_NORPIL:
        pGimmick = new CPillarGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_RAPROC:
        pGimmick = new CPushingGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_LEOS:
        pGimmick = new CPipeGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_LEOM:
        pGimmick = new CCuttingGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_LEOL:
        pGimmick = new CBridgePillarGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_GRASS:
        pGimmick = new CGrassGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_CRYSTL:
        pGimmick = new CCrystalGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_GUILLO:
        pGimmick = new CGuillotineGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_SEESAW:
        pGimmick = new CSeesawGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_FALOBJ:
        pGimmick = new CFallGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_METEO:
        //
        //  Not exist
        //
        break;
        
    case GIMMICKID::ID_N_DOOR:
        pGimmick = new CNormalDoorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_UTROM:
        pGimmick = new CUtromGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_LASER:
        pGimmick = new CLaserGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_MINE:
        pGimmick = new CMineGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_BELCON:
        pGimmick = new CBeltConveyorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_STOPER:
        pGimmick = new CStopperGimmick(pszName, pParam);
        break;


    //
    //  Ride
    //
    case GIMMICKID::ID_R_FLAG:
        pGimmick = new CRideFlagGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_R_THNDER:
        pGimmick = new CRideMineGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_R_ASTRID:
        pGimmick = new CRideRockGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_R_SHUTTR:
        pGimmick = new CRideShutterGimmick(pszName, pParam);
        break;


    //
    //  Other
    //
    case GIMMICKID::ID_N_ITEM:
        pGimmick = new CItemGimmick(pszName, pParam);
        break;


    //
    //  Map
    //
    case GIMMICKID::ID_N_LIGHT:
        pGimmick = new CMapEffectLightGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_WEATHER:
    case GIMMICKID::ID_N_SMOKE:
    case GIMMICKID::ID_N_FLAME:
    case GIMMICKID::ID_N_STEAM:
    case GIMMICKID::ID_N_SPARK:
    case GIMMICKID::ID_N_MOTH:
    case GIMMICKID::ID_N_FISH:
    case GIMMICKID::ID_N_BUBBLE:
    case GIMMICKID::ID_N_MIST:
    case GIMMICKID::ID_N_SPLASH:
    case GIMMICKID::ID_N_EXP:
        pGimmick = new CMapEffectGimmick(pszName, pParam);
        break;

    //
    //  Koyuu
    //
    case GIMMICKID::ID_K_M12N:
        pGimmick = new CCrumbleFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M02N:
    case GIMMICKID::ID_K_M05N:
        pGimmick = new CElevatorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M38B:
        pGimmick = new CFenceGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M38BMAN:
        pGimmick = new CFenceGimmickManager(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M17N:
        {
            GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
            
            if (!std::strcmp(pInitParam->m_szName, "GMKM17N_A_0"))
            {
                std::strcpy(pInitParam->m_szName, "gsmachine00");
                pInitParam->m_subid = 0;
            };

            if (!std::strcmp(pInitParam->m_szName, "GMKM17N_A_1"))
            {
                std::strcpy(pInitParam->m_szName, "gsmachine01");
                pInitParam->m_subid = 1;
            };

            if (!std::strcmp(pInitParam->m_szName, "GMKM17N_A_2"))
            {
                std::strcpy(pInitParam->m_szName, "gsmachine02");
                pInitParam->m_subid = 2;
            };

            if (!std::strcmp(pInitParam->m_szName, "GMKM17N_A_3"))
            {
                std::strcpy(pInitParam->m_szName, "gsmachine03");
                pInitParam->m_subid = 3;
            };

            pGimmick = new CGSMachineGimmick(pszName, pParam);
        }
        break;
        
    case GIMMICKID::ID_K_M43N:
        {
            GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

            if (!std::strcmp(pInitParam->m_szName, "GMKM43N_A_0"))
            {
                std::strcpy(pInitParam->m_szName, "AppTrailer");
                pInitParam->m_subid = 0;
            };

            pGimmick = new CApproachingTrailerGimmick(pszName, pParam);
        }
        break;
        
    case GIMMICKID::ID_K_M28NGR:
        pGimmick = new CGearGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M28NPT:
        pGimmick = new CPistonGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M28NPR:
        pGimmick = new CPropellerGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M28OB:
        pGimmick = new CHatchGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M28OBR:
        pGimmick = new CFallRubbleGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M28OBRM:
        pGimmick = new CFallRubbleGimmickManager(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M44N:
        pGimmick = new CMadCarGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M44NM:
        pGimmick = new CMadCarGimmickManager(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M50NB:
        pGimmick = new CDescentCityGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M23N:
        pGimmick = new CGateGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M04N:
        pGimmick = new CUniquePathTransferFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M20F:
        pGimmick = new CUniqueLinearTransferFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M53F:
        pGimmick = new CMapModelCtrlGimmick(pszName, pParam);
        break;

    default:
        ASSERT(false);
        break;
    };

    if (pGimmick)
        pGimmick->Init(idGimmick);
    else
        OUTPUT("Missing gimmick %s\n", pszName);

    return pGimmick;
};