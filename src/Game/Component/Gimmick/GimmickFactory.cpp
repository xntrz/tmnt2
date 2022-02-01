#include "GimmickFactory.hpp"
#include "GimmickParam.hpp"

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
#include "ConcreteGimmick/StopperGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"


/*static*/ CGimmick* CGimmickFactory::Create(GIMMICKID::VALUE idGimmick, int32 subid, void* pParam)
{
    CGimmick* pGimmick = nullptr;
    
    ASSERT(pParam);
    GIMMICKPARAM::GIMMICK_COMMON_HEAD* pHeader = (GIMMICKPARAM::GIMMICK_COMMON_HEAD*)pParam;
    const char* pszName = pHeader->m_szName;
    
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
        if (CGameData::Option().Play().IsHelpEnabled())
            pGimmick = new CHelpGimmick(pszName, pParam);        
        break;
        
    case GIMMICKID::ID_S_TUTO:
        if (CGameData::Option().Play().IsHelpEnabled())
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
        break;
        
    case GIMMICKID::ID_G_GROUND:
    case GIMMICKID::ID_G_AIR:
        break;
        
    case GIMMICKID::ID_G_EGG:
        break;
        
    case GIMMICKID::ID_G_CAVE:
        break;
        
    case GIMMICKID::ID_G_DOOR:
        break;
        
    case GIMMICKID::ID_G_CAR:
        break;
        
    case GIMMICKID::ID_G_SSHIPS:
        break;


    //
    //  Normal
    //
    case GIMMICKID::ID_N_THROW:
    case GIMMICKID::ID_N_ITEMBX:
        pGimmick = new CThrowingGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_BRKCAR:
        break;
        
    case GIMMICKID::ID_N_BRKOBJ:
        break;
        
    case GIMMICKID::ID_N_ROLOBJ:
        break;
        
    case GIMMICKID::ID_N_MVFLLI:
        pGimmick = new CLinearTransferFloorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_MVFLPA:
        break;
        
    case GIMMICKID::ID_N_MVFLRO:
        break;
        
    case GIMMICKID::ID_N_CONSOL:
        pGimmick = new CConsoleGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_SWITCH:
        pGimmick = new CSwitchGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_SWIPIL:
        break;
        
    case GIMMICKID::ID_N_BRKPIL:
    case GIMMICKID::ID_N_SHKPIL:
    case GIMMICKID::ID_N_NORPIL:
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
        break;
        
    case GIMMICKID::ID_N_GRASS:
        pGimmick = new CGrassGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_CRYSTL:
        break;
        
    case GIMMICKID::ID_N_GUILLO:
        break;
        
    case GIMMICKID::ID_N_SEESAW:
        break;
        
    case GIMMICKID::ID_N_FALOBJ:
        break;
        
    case GIMMICKID::ID_N_METEO:
        break;
        
    case GIMMICKID::ID_N_DOOR:
        pGimmick = new CNormalDoorGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_UTROM:
        break;
        
    case GIMMICKID::ID_N_LASER:
        pGimmick = new CLaserGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_N_MINE:
        break;
        
    case GIMMICKID::ID_N_BELCON:
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
        break;
        
    case GIMMICKID::ID_R_ASTRID:
        pGimmick = new CRideRockGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_R_SHUTTR:
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
        break;
        
    case GIMMICKID::ID_K_M02N:
    case GIMMICKID::ID_K_M05N:
        break;
        
    case GIMMICKID::ID_K_M38B:
        break;
        
    case GIMMICKID::ID_K_M38BMAN:
        break;
        
    case GIMMICKID::ID_K_M17N:
        break;
        
    case GIMMICKID::ID_K_M43N:
        break;
        
    case GIMMICKID::ID_K_M28NGR:
        break;
        
    case GIMMICKID::ID_K_M28NPT:
        break;
        
    case GIMMICKID::ID_K_M28NPR:
        break;
        
    case GIMMICKID::ID_K_M28OB:
        break;
        
    case GIMMICKID::ID_K_M28OBR:
        break;
        
    case GIMMICKID::ID_K_M28OBRM:
        break;
        
    case GIMMICKID::ID_K_M44N:
        break;
        
    case GIMMICKID::ID_K_M44NM:
        break;
        
    case GIMMICKID::ID_K_M50NB:
        pGimmick = new CDescentCityGimmick(pszName, pParam);
        break;
        
    case GIMMICKID::ID_K_M23N:
        break;
        
    case GIMMICKID::ID_K_M04N:
        break;
        
    case GIMMICKID::ID_K_M20F:
        break;
        
    case GIMMICKID::ID_K_M53F:
        break;

    default:
        ASSERT(false);
        break;
    };

    if (pGimmick)
        pGimmick->Init(idGimmick);
    else
        OUTPUT("[GAME] Missing gimmick %s\n", pszName);

    return pGimmick;
};