#pragma once

#include "Game/Component/GameMain/GameObjLoadInfo.hpp"
#include "Game/Component/GameMain/MapID.hpp"


class CMapLoadInfo final : public CGameObjLoadInfo
{
public:
    CMapLoadInfo(MAPID::VALUE mapID);
    virtual ~CMapLoadInfo(void);
    virtual bool MakeFileName(char* pszFilenameBuff) const override;

private:
    MAPID::VALUE m_mapID;
};