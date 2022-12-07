#include "PlayerLoadInfo.hpp"

#include "System/Common/File/FileID.hpp"


/*static*/ const char* CPlayerLoadInfo::LISTFILE_PATH = "Common/Player/";


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


int32 CPlayerLoadInfo::GetFileID(void) const
{
    int32 FileID = FILEID::ID_INVALID;
    
    switch (m_idPlayer)
    {
    case PLAYERID::ID_LEO:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_LEONARDO;
                break;
                
            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_LEONARDO_EXA;
                break;
                
            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_LEONARDO_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_RAP:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_RAPHAEL;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_RAPHAEL_EXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_RAPHAEL_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_MIC:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_MICHELANGERO;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_MICHELANGERO_EXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_MICHELANGERO_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_DON:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_DONATELLO;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_DONATELLO_EXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_DONATELLO_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_SLA:
        FileID = FILEID::ID_SLASHUUR;
        break;

    case PLAYERID::ID_CAS:
        FileID = FILEID::ID_CASEY;
        break;
        
    case PLAYERID::ID_KAR:
        FileID = FILEID::ID_KARAI;
        break;
        
    case PLAYERID::ID_SPL:
        FileID = FILEID::ID_SPLINTER;
        break;

    default:
        ASSERT(false);
        break;        
    };

    return FileID;
};


bool CPlayerLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    bool bResult = false;
    const char* pszPlayerName = PLAYERID::GetName(m_idPlayer);

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

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};