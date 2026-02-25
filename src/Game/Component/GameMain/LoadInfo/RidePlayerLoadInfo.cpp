#include "RidePlayerLoadInfo.hpp"

#include "Game/Sequence/Ride/RideStage.hpp"


CRidePlayerLoadInfo::CRidePlayerLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
: m_idPlayer(idPlayer)
, m_costume(costume)
{
    ;
};


CRidePlayerLoadInfo::~CRidePlayerLoadInfo(void)
{
    ;
};


FNAME CRidePlayerLoadInfo::GetFileID(void) const
{
    FNAME fileID = FNAME_INVALID;
    
    if (IsSpaceStage())
    {
        switch (m_idPlayer)
        {
        case PLAYERID::ID_LEO:
            fileID = FPATH("Common/Ride/Leonardo/Leonardo_Ship.lpac");
            break;
            
        case PLAYERID::ID_RAP:
            fileID = FPATH("Common/Ride/Raphael/Raphael_Ship.lpac");
            break;
            
        case PLAYERID::ID_MIC:
            fileID = FPATH("Common/Ride/Michelangero/Michelangero_Ship.lpac");
            break;
            
        case PLAYERID::ID_DON:
            fileID = FPATH("Common/Ride/Donatello/Donatello_Ship.lpac");
            break;
            
        case PLAYERID::ID_SLA:
            fileID = FPATH("Common/Ride/Slashuur/Slashuur_Ship.lpac");
            break;
            
        case PLAYERID::ID_CAS:
            fileID = FPATH("Common/Ride/Casey/Casey_Ship.lpac");
            break;
            
        case PLAYERID::ID_KAR:
            fileID = FPATH("Common/Ride/Karai/Karai_Ship.lpac");
            break;
            
        case PLAYERID::ID_SPL:
            fileID = FPATH("Common/Ride/Splinter/Splinter_Ship.lpac");
            break;
            
        default:
            ASSERT(false);
            break;
        };
    }
    else
    {
        switch (m_idPlayer)
        {
        case PLAYERID::ID_LEO:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    fileID = FPATH("Common/Ride/Leonardo/Leonardo.lpac");
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    fileID = FPATH("Common/Ride/Leonardo/Leonardo_ExA.lpac");
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    fileID = FPATH("Common/Ride/Leonardo/Leonardo_ExB.lpac");
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            }
            break;

        case PLAYERID::ID_RAP:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    fileID = FPATH("Common/Ride/Raphael/Raphael.lpac");
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    fileID = FPATH("Common/Ride/Raphael/Raphael_ExA.lpac");
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    fileID = FPATH("Common/Ride/Raphael/Raphael_ExB.lpac");
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            }
            break;

        case PLAYERID::ID_MIC:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    fileID = FPATH("Common/Ride/Michelangero/Michelangero.lpac");
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    fileID = FPATH("Common/Ride/Michelangero/Michelangero_ExA.lpac");
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    fileID = FPATH("Common/Ride/Michelangero/Michelangero_ExB.lpac");
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            }
            break;

        case PLAYERID::ID_DON:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    fileID = FPATH("Common/Ride/Donatello/Donatello.lpac");
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    fileID = FPATH("Common/Ride/Donatello/Donatello_ExA.lpac");
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    fileID = FPATH("Common/Ride/Donatello/Donatello_ExB.lpac");
                    break;

                default:
                    ASSERT(false);
                    break;
                };
            }
            break;

        case PLAYERID::ID_SLA:
            fileID = FPATH("Common/Ride/Slashuur/Slashuur.lpac");
            break;

        case PLAYERID::ID_CAS:
            fileID = FPATH("Common/Ride/Casey/Casey.lpac");
            break;

        case PLAYERID::ID_KAR:
            fileID = FPATH("Common/Ride/Karai/Karai.lpac");
            break;

        case PLAYERID::ID_SPL:
            fileID = FPATH("Common/Ride/Splinter/Splinter.lpac");
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    return fileID;
};


bool CRidePlayerLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    const char* pszListFilePath = "Common/Ride/";
    const char* pszPlayerName = PLAYERID::GetName(m_idPlayer);

    if (IsSpaceStage())
    {
        std::sprintf(pszFilenameBuff, "%s%s/%s_ship.list",
                     pszListFilePath, pszPlayerName, pszPlayerName);
    }
    else
    {
        switch (m_costume)
        {
        case GAMETYPES::COSTUME_NONE:
            std::sprintf(pszFilenameBuff, "%s%s/%s.list",
                         pszListFilePath, pszPlayerName, pszPlayerName);
            break;

        case GAMETYPES::COSTUME_SAMURAI:
            std::sprintf(pszFilenameBuff, "%s%s/%s_EXa.list",
                         pszListFilePath, pszPlayerName, pszPlayerName);
            break;

        case GAMETYPES::COSTUME_NEXUS:
            std::sprintf(pszFilenameBuff, "%s%s/%s_EXb.list",
                         pszListFilePath, pszPlayerName, pszPlayerName);
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    return true;
};


bool CRidePlayerLoadInfo::IsSpaceStage(void) const
{
    return CRideStage::m_bSpace;
};