#pragma once


namespace OPTIONTYPES
{
    enum DISPLAYMODE
    {
        DISPLAYMODE_FULLSCREEN = 0,
        DISPLAYMODE_FULLWINDOW,
        DISPLAYMODE_WINDOW,

        DISPLAYMODEMAX,
    };

    enum VOLUMETYPE
    {
        VOLUMETYPE_SE = 0,
        VOLUMETYPE_BGM,
        VOLUMETYPE_VOICE,
        
        VOLUMETYPEMAX,
    };

    enum BTNFUNC
    {
        BTNFUNC_ATTACK_A = 0,
        BTNFUNC_ATTACK_B,
        BTNFUNC_SHURIKEN,
        BTNFUNC_JUMP,
        BTNFUNC_DASH,
        BTNFUNC_GUARD,
        BTNFUNC_CHANGE_CHARA,
        
        BTNFUNCMAX,
    };

    enum KEYFUNC
    {
        KEYFUNC_ATTACK_A = 0,
        KEYFUNC_ATTACK_B,
        KEYFUNC_SHURIKEN,
        KEYFUNC_JUMP,
        KEYFUNC_DASH,
        KEYFUNC_GUARD,
        KEYFUNC_CHANGE_CHARA,
        KEYFUNC_CHANGE_CAMERA,
        KEYFUNC_CHANGE_GAUGE,
        KEYFUNC_UP,
        KEYFUNC_DOWN,
        KEYFUNC_LEFT,
        KEYFUNC_RIGHT,
        
        KEYFUNCMAX,
    };
};