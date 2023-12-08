#include "RidePlayerLoadInfo.hpp"

#include "Game/Sequence/Ride/RideStage.hpp"
#include "System/Common/File/FileID.hpp"


/*static*/ const char* CRidePlayerLoadInfo::LISTFILE_PATH = "Common/Ride/";


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


int32 CRidePlayerLoadInfo::GetFileID(void) const
{
    int32 nFileID = FILEID::ID_INVALID;
    
    if (GetSpaceFlag())
    {
        switch (m_idPlayer)
        {
        case PLAYERID::ID_LEO:
            nFileID = FILEID::ID_LEONARDO_SHIP;
            break;
            
        case PLAYERID::ID_RAP:
            nFileID = FILEID::ID_RAPHAEL_SHIP;
            break;
            
        case PLAYERID::ID_MIC:
            nFileID = FILEID::ID_MICHELANGERO_SHIP;
            break;
            
        case PLAYERID::ID_DON:
            nFileID = FILEID::ID_DONATELLO_SHIP;
            break;
            
        case PLAYERID::ID_SLA:
            nFileID = FILEID::ID_SLASHUUR_SHIP;
            break;
            
        case PLAYERID::ID_CAS:
            nFileID = FILEID::ID_CASEY_SHIP;
            break;
            
        case PLAYERID::ID_KAR:
            nFileID = FILEID::ID_KARAI_SHIP;
            break;
            
        case PLAYERID::ID_SPL:
            nFileID = FILEID::ID_SPLINTER_SHIP;
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
                    nFileID = FILEID::ID_LEONARDO_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = FILEID::ID_LEONARDO_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = FILEID::ID_LEONARDO_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_RAP:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    nFileID = FILEID::ID_RAPHAEL_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = FILEID::ID_RAPHAEL_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = FILEID::ID_RAPHAEL_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_MIC:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    nFileID = FILEID::ID_MICHELANGERO_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = FILEID::ID_MICHELANGERO_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = FILEID::ID_MICHELANGERO_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_DON:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    nFileID = FILEID::ID_DONATELLO_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = FILEID::ID_DONATELLO_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = FILEID::ID_DONATELLO_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_SLA:
            nFileID = FILEID::ID_SLASHUUR_BOARD;
            break;

        case PLAYERID::ID_CAS:
            nFileID = FILEID::ID_CASEY_BOARD;
            break;

        case PLAYERID::ID_KAR:
            nFileID = FILEID::ID_KARAI_BOARD;
            break;

        case PLAYERID::ID_SPL:
            nFileID = FILEID::ID_SPLINTER_BOARD;
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    return nFileID;
};


bool CRidePlayerLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    pszFilenameBuff[0] = '\0';
    
    const char* pszPlayerName = PLAYERID::GetName(m_idPlayer);

    if (GetSpaceFlag())
    {
        std::sprintf(
            pszFilenameBuff,
            "%s%s/%s_ship.list",
            LISTFILE_PATH,
            pszPlayerName,
            pszPlayerName
        );
    }
    else
    {
        switch (m_costume)
        {
        case GAMETYPES::COSTUME_NONE:
            std::sprintf(
                pszFilenameBuff,
                "%s%s/%s.list",
                LISTFILE_PATH,
                pszPlayerName,
                pszPlayerName
            );
            break;

        case GAMETYPES::COSTUME_SAMURAI:
            std::sprintf(
                pszFilenameBuff,
                "%s%s/%s_EXa.list",
                LISTFILE_PATH,
                pszPlayerName,
                pszPlayerName
            );
            break;

        case GAMETYPES::COSTUME_NEXUS:
            std::sprintf(
                pszFilenameBuff,
                "%s%s/%s_EXb.list",
                LISTFILE_PATH,
                pszPlayerName,
                pszPlayerName
            );
            break;
        };
    };

    return (pszFilenameBuff[0] != '\0');
};


bool CRidePlayerLoadInfo::GetSpaceFlag(void) const
{
    return CRideStage::m_bSpace;
};