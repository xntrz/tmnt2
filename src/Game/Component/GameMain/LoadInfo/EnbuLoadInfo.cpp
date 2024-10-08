#include "EnbuLoadInfo.hpp"

#include "System/Common/File/FileID.hpp"


CEnbuLoadInfo::CEnbuLoadInfo(PLAYERID::VALUE idPlayer, GAMETYPES::COSTUME costume)
: m_idPlayer(idPlayer)
, m_costume(costume)
{
    ;
};


CEnbuLoadInfo::~CEnbuLoadInfo(void)
{
    ;
};


int32 CEnbuLoadInfo::GetFileID(void) const
{
    int32 FileID = FILEID::ID_INVALID;

    switch (m_idPlayer)
    {
    case PLAYERID::ID_LEO:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_ENBU_LEO;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_ENBU_LEOEXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_ENBU_LEOEXB;
                break;
            };
        }
        break;

    case PLAYERID::ID_RAP:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_ENBU_RAP;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_ENBU_RAPEXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_ENBU_RAPEXB;
                break;
            };
        }
        break;

    case PLAYERID::ID_MIC:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_ENBU_MIC;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_ENBU_MICEXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_ENBU_MICEXB;
                break;
            };
        }
        break;

    case PLAYERID::ID_DON:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                FileID = FILEID::ID_ENBU_DON;
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                FileID = FILEID::ID_ENBU_DONEXA;
                break;

            case GAMETYPES::COSTUME_NEXUS:
                FileID = FILEID::ID_ENBU_DONEXB;
                break;
            };
        }
        break;

    case PLAYERID::ID_SLA:
        FileID = FILEID::ID_ENBU_SLS;
        break;

    case PLAYERID::ID_CAS:
        FileID = FILEID::ID_ENBU_CAS;
        break;

    case PLAYERID::ID_KAR:
        FileID = FILEID::ID_ENBU_KRI;
        break;

    case PLAYERID::ID_SPL:
        FileID = FILEID::ID_ENBU_SPL;
        break;

    default:
        ASSERT(false);
        break;
    };

    return FileID;
};


bool CEnbuLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    ASSERT(false);
    return false;
};