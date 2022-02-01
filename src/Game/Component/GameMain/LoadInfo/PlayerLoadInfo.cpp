#include "PlayerLoadInfo.hpp"

#include "System/Common/File/AfsFileID.hpp"


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
    int32 FileID = AFSFILEID::ID_INVALID;
    
    switch (m_idPlayer)
    {
    case PLAYERID::ID_LEO:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = AFSFILEID::ID_LEONARDO;
                break;
                
            case GAMETYPES::COSTUME_SAMURAI:
                FileID = AFSFILEID::ID_LEONARDO_EXA;
                break;
                
            case GAMETYPES::COSTUME_NEXUS:
                FileID = AFSFILEID::ID_LEONARDO_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_RAPH:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = AFSFILEID::ID_RAPHAEL;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = AFSFILEID::ID_RAPHAEL_EXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = AFSFILEID::ID_RAPHAEL_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_MIC:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = AFSFILEID::ID_MICHELANGERO;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = AFSFILEID::ID_MICHELANGERO_EXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = AFSFILEID::ID_MICHELANGERO_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_DON:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = AFSFILEID::ID_DONATELLO;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = AFSFILEID::ID_DONATELLO_EXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = AFSFILEID::ID_DONATELLO_EXB;
                break;
            };
        }
        break;
        
    case PLAYERID::ID_SLASHUUR:
        FileID = AFSFILEID::ID_SLASHUUR;
        break;

    case PLAYERID::ID_CASEY:
        FileID = AFSFILEID::ID_CASEY;
        break;
        
    case PLAYERID::ID_KARAI:
        FileID = AFSFILEID::ID_KARAI;
        break;
        
    case PLAYERID::ID_SPLINTER:
        FileID = AFSFILEID::ID_SPLINTER;
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