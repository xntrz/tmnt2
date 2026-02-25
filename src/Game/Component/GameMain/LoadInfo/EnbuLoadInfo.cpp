#include "EnbuLoadInfo.hpp"


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


FNAME CEnbuLoadInfo::GetFileID(void) const
{
    FNAME fileID = FNAME_INVALID;

    switch (m_idPlayer)
    {
    case PLAYERID::ID_LEO:
        {
            switch (m_costume)
            {
            case GAMETYPES::COSTUME_NONE:
                fileID = FPATH("Common/Enbu/Leo/Enbu_Leo.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Enbu/Leo/Enbu_LeoExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Enbu/Leo/Enbu_LeoExB.lpac");
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
                fileID = FPATH("Common/Enbu/Rap/Enbu_Rap.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Enbu/Rap/Enbu_RapExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Enbu/Rap/Enbu_RapExB.lpac");
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
                fileID = FPATH("Common/Enbu/Mic/Enbu_Mic.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Enbu/Mic/Enbu_MicExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Enbu/Mic/Enbu_MicExB.lpac");
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
                fileID = FPATH("Common/Enbu/Don/Enbu_Don.lpac");
                break;

            case GAMETYPES::COSTUME_SAMURAI:
                fileID = FPATH("Common/Enbu/Don/Enbu_DonExA.lpac");
                break;

            case GAMETYPES::COSTUME_NEXUS:
                fileID = FPATH("Common/Enbu/Don/Enbu_DonExB.lpac");
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case PLAYERID::ID_SLA:
        fileID = FPATH("Common/Enbu/Sls/Enbu_Sls.lpac");
        break;

    case PLAYERID::ID_CAS:
        fileID = FPATH("Common/Enbu/Cas/Enbu_Cas.lpac");
        break;

    case PLAYERID::ID_KAR:
        fileID = FPATH("Common/Enbu/Kri/Enbu_Kri.lpac");
        break;

    case PLAYERID::ID_SPL:
        fileID = FPATH("Common/Enbu/Spl/Enbu_Spl.lpac");
        break;

    default:
        ASSERT(false);
        break;
    };

    return fileID;
};


bool CEnbuLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    ASSERT(false);
    return false;
};