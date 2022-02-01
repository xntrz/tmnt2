#include "RidePlayerLoadInfo.hpp"

#include "Game/Sequence/Ride/RideStage.hpp"
#include "System/Common/File/AfsFileID.hpp"


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
    int32 nFileID = AFSFILEID::ID_INVALID;
    
    if (GetSpaceFlag())
    {
        switch (m_idPlayer)
        {
        case PLAYERID::ID_LEO:
            nFileID = AFSFILEID::ID_LEONARDO_SHIP;
            break;
            
        case PLAYERID::ID_RAPH:
            nFileID = AFSFILEID::ID_RAPHAEL_SHIP;
            break;
            
        case PLAYERID::ID_MIC:
            nFileID = AFSFILEID::ID_MICHELANGERO_SHIP;
            break;
            
        case PLAYERID::ID_DON:
            nFileID = AFSFILEID::ID_DONATELLO_SHIP;
            break;
            
        case PLAYERID::ID_SLASHUUR:
            nFileID = AFSFILEID::ID_SLASHUUR_SHIP;
            break;
            
        case PLAYERID::ID_CASEY:
            nFileID = AFSFILEID::ID_CASEY_SHIP;
            break;
            
        case PLAYERID::ID_KARAI:
            nFileID = AFSFILEID::ID_KARAI_SHIP;
            break;
            
        case PLAYERID::ID_SPLINTER:
            nFileID = AFSFILEID::ID_SPLINTER_SHIP;
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
                    nFileID = AFSFILEID::ID_LEONARDO_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = AFSFILEID::ID_LEONARDO_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = AFSFILEID::ID_LEONARDO_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_RAPH:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    nFileID = AFSFILEID::ID_RAPHAEL_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = AFSFILEID::ID_RAPHAEL_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = AFSFILEID::ID_RAPHAEL_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_MIC:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    nFileID = AFSFILEID::ID_MICHELANGERO_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = AFSFILEID::ID_MICHELANGERO_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = AFSFILEID::ID_MICHELANGERO_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_DON:
            {
                switch (m_costume)
                {
                case GAMETYPES::COSTUME_NONE:
                    nFileID = AFSFILEID::ID_DONATELLO_BOARD;
                    break;

                case GAMETYPES::COSTUME_SAMURAI:
                    nFileID = AFSFILEID::ID_DONATELLO_BOARD_EXA;
                    break;

                case GAMETYPES::COSTUME_NEXUS:
                    nFileID = AFSFILEID::ID_DONATELLO_BOARD_EXB;
                    break;
                };
            }
            break;

        case PLAYERID::ID_SLASHUUR:
            nFileID = AFSFILEID::ID_SLASHUUR_BOARD;
            break;

        case PLAYERID::ID_CASEY:
            nFileID = AFSFILEID::ID_CASEY_BOARD;
            break;

        case PLAYERID::ID_KARAI:
            nFileID = AFSFILEID::ID_KARAI_BOARD;
            break;

        case PLAYERID::ID_SPLINTER:
            nFileID = AFSFILEID::ID_SPLINTER_BOARD;
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
    bool bResult = false;
    const char* pszPlayerName = PLAYERID::GetName(m_idPlayer);

    if (GetSpaceFlag())
    {
        bResult = true;
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
            bResult = true;
            std::sprintf(
                pszFilenameBuff,
                "%s%s/%s.list",
                LISTFILE_PATH,
                pszPlayerName,
                pszPlayerName
            );
            break;

        case GAMETYPES::COSTUME_SAMURAI:
            bResult = true;
            std::sprintf(
                pszFilenameBuff,
                "%s%s/%s_EXa.list",
                LISTFILE_PATH,
                pszPlayerName,
                pszPlayerName
            );
            break;

        case GAMETYPES::COSTUME_NEXUS:
            bResult = true;
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

    return bResult;
};


bool CRidePlayerLoadInfo::GetSpaceFlag(void) const
{
    return CRideStage::m_bSpace;
};