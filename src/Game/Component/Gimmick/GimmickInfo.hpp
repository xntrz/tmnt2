#pragma once

#include "GimmickID.hpp"


class CGimmickInfo
{
public:
    enum DRAWPRI
    {
        DRAWPRI_PRECHARA = 0,
        DRAWPRI_POSTCHARA,
        DRAWPRI_POSTALPHAMAP,
        
        DRAWPRIMAX,
    };

    enum PARAMTYPE
    {
        PARAMTYPE_NONE = 0,
        PARAMTYPE_BASIC,
        PARAMTYPE_ITEMBOX,
        PARAMTYPE_MOVIE,
        PARAMTYPE_CAMERA,
        PARAMTYPE_SOUND,
        PARAMTYPE_HELP,
        PARAMTYPE_TUTO,
        PARAMTYPE_FLOOR_ROUND,
        PARAMTYPE_FLOOR_LINER,
        PARAMTYPE_FLOOR_PATH,
        PARAMTYPE_AREA_BOX,
        PARAMTYPE_SWITCH,
        PARAMTYPE_SWITCH_AND,
        PARAMTYPE_SWITCH_TIME,
        PARAMTYPE_SWITCH_GENERATOR,
        PARAMTYPE_SWITCH_BOX,
        PARAMTYPE_SWITCH_SPHERE,
        PARAMTYPE_SWITCH_TARGET,
        PARAMTYPE_ENEMY_PLACE,
        PARAMTYPE_ENEMY_SINGLE,
        PARAMTYPE_ENEMY_MULTIPLE,
        PARAMTYPE_GROUP,
        PARAMTYPE_GUILLOTINE,
        
        PARAMTYPEMAX,
    };

public:
    static void MakeName(char* pszGimmickName, GIMMICKID::VALUE idGimmick, int32 subid, int32 no);
    static const char* GetBaseName(GIMMICKID::VALUE idGimmick);
    static DRAWPRI GetDrawPriority(GIMMICKID::VALUE idGimmick);
};