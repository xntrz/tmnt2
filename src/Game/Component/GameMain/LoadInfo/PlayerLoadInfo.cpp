#include "PlayerLoadInfo.hpp"


bool CPlayerCommonLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    std::sprintf(pszFilenameBuff, "Common/Player/PlayerCommon.list");
    return true;
};


//
// *********************************************************************************
//


CPlayerLoadInfo::CPlayerLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
: m_idPlayer(idPlayer)
, m_costume(costume)
{
    ;
};


CPlayerLoadInfo::~CPlayerLoadInfo(void)
{
    ;
};


FNAME CPlayerLoadInfo::GetFileID(void) const
{
    FNAME fileID = FNAME_INVALID;
    
    switch (m_idPlayer)
    {
    case PLAYERID::ID_LEO:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                fileID = FPATH("Common/Player/Leonardo/Leonardo.lpac");
                break;
                
            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Player/Leonardo/Leonardo_ExA.lpac");
                break;
                
            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Player/Leonardo/Leonardo_ExB.lpac");
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
                fileID = FPATH("Common/Player/Raphael/Raphael.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Player/Raphael/Raphael_ExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Player/Raphael/Raphael_ExB.lpac");
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
                fileID = FPATH("Common/Player/Michelangero/Michelangero.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Player/Michelangero/Michelangero_ExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Player/Michelangero/Michelangero_ExB.lpac");
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
                fileID = FPATH("Common/Player/Donatello/Donatello.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Player/Donatello/Donatello_ExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Player/Donatello/Donatello_ExB.lpac");
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;
        
    case PLAYERID::ID_SLA:
        fileID = FPATH("Common/Player/Slashuur/Slashuur.lpac");
        break;

    case PLAYERID::ID_CAS:
        fileID = FPATH("Common/Player/Casey/Casey.lpac");
        break;

    case PLAYERID::ID_KAR:
        fileID = FPATH("Common/Player/Karai/Karai.lpac");
        break;

    case PLAYERID::ID_SPL:
        fileID = FPATH("Common/Player/Splinter/Splinter.lpac");
        break;

    default:
        ASSERT(false);
        break;        
    };

    return fileID;
};


bool CPlayerLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    pszFilenameBuff[0] = '\0';

    const char* pszListFilePath = "Common/Player/";
    const char* pszPlayerName = PLAYERID::GetName(m_idPlayer);
    
    switch (m_costume)
    {
    case GAMETYPES::COSTUME_NONE:
        std::sprintf(pszFilenameBuff, "%s%s/%s.list", pszListFilePath, pszPlayerName, pszPlayerName);
        break;

    case GAMETYPES::COSTUME_SAMURAI:
        std::sprintf(pszFilenameBuff, "%s%s/%s_EXa.list", pszListFilePath, pszPlayerName, pszPlayerName);
        break;

    case GAMETYPES::COSTUME_NEXUS:
        std::sprintf(pszFilenameBuff, "%s%s/%s_EXb.list", pszListFilePath, pszPlayerName, pszPlayerName);
        break;

    default:
        ASSERT(false);
        break;
    };

    return (pszFilenameBuff[0] != '\0');
};