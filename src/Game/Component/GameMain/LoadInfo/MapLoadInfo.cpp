#include "MapLoadInfo.hpp"

#include "Game/Component/GameMain/MapInfo.hpp"


CMapLoadInfo::CMapLoadInfo(MAPID::VALUE mapID)
: m_mapID(mapID)
{
    ;
};


CMapLoadInfo::~CMapLoadInfo(void)
{
    ;
};


bool CMapLoadInfo::MakeFileName(char* pszFilenameBuff) const
{
    const char* pszPath = "Common/Map/";
    const char* pszMapName = CMapInfo::GetName(m_mapID);

    std::sprintf(pszFilenameBuff, "%s%s/%s.list", pszPath, pszMapName, pszMapName);

    return true;
};