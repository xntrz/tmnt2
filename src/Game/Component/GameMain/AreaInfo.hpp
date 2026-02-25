#pragma once

#include "AreaID.hpp"
#include "WorldID.hpp"
#include "StageID.hpp"
#include "AntiqueID.hpp"
#include "GameTypes.hpp"


class CAreaInfo
{
public:
    static const wchar* GetDispName(AREAID::VALUE idArea);
    static const wchar* GetDispWorldName(AREAID::VALUE idArea);
    static const char* GetEpisode(AREAID::VALUE idArea);
    static int32 GetWindowNo(AREAID::VALUE idArea);
    static STAGEID::VALUE GetStageID(AREAID::VALUE idArea, int32 index);
    static int32 IndexOfStage(AREAID::VALUE idArea, STAGEID::VALUE idStage);
    static int32 GetStageNum(AREAID::VALUE idArea);
    static ANTIQUEID::VALUE GetAntiqueID(AREAID::VALUE idArea);
    static AREAID::VALUE FindAntiqueArea(ANTIQUEID::VALUE idAntique);
    static MVNAME GetAreaOutMovieID(AREAID::VALUE idArea);
    static bool IsPlayMovieOfAreaOut(AREAID::VALUE idArea);
    static GAMETYPES::NEXUSID GetNexusID(AREAID::VALUE idArea);
    static uint32 GetBestRankClearTime(AREAID::VALUE idArea);
    static uint32 GetWorstRankClearTime(AREAID::VALUE idArea);
    static WORLDID::VALUE GetWorldNo(AREAID::VALUE idArea);
    static int32 GetDiscNo(AREAID::VALUE idArea);
};