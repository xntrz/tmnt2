#pragma once

#include "AreaTypes.hpp"


class CAreaMenu
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void AreaMenuInit(void);
    static AREATYPES::NEXTSEQUENCE AreaMenuSelect(void);
    static void AreaMenuStationWarp(void);
    static void AreaMenuPeriod(void);
    static void AreaMenuDraw(void);
    static void AreaInfoMenuDraw(void);
};