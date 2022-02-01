#pragma once


namespace HOMETYPES
{
    enum DOORKIND
    {
        DOORKIND_EXIT = 0,
        DOORKIND_APRIL,
        DOORKIND_COMPUTER,
        DOORKIND_TOURNAMENT,

        DOORKINDNUM,
    };
    
    enum DOORPOSITION
    {
        DOORPOSITION_FRONT = 0,
        DOORPOSITION_LEFT,
        DOORPOSITION_BACK,
        DOORPOSITION_RIGHT,
        
        DOORPOSITIONNUM,
    };

    enum DOORSTATUS
    {
        DOORSTATUS_CLOSE = 0,
        DOORSTATUS_OPEN,
        DOORSTATUS_NEW,
        DOORSTATUS_UPDATE,
    };

    enum CAMERAKIND
    {
        CAMERAKIND_OVERLOOK = 0,
        CAMERAKIND_DOOR,
        CAMERAKIND_ENTER,
        
        CAMERAKINDNUM,
    };

    enum CAMERA_POS_INFO_KIND
    {
        CAMERA_POS_INFO_KIND_EYE = 0,
        CAMERA_POS_INFO_KIND_AT,
        
        CAMERA_POS_INFO_KIND_NUM,
    };

    enum PADINPUT
    {
        PADINPUT_NONE = 0,
        PADINPUT_OK,
        PADINPUT_UP,
        PADINPUT_LEFT,
        PADINPUT_DOWN,
        PADINPUT_RIGHT,
    };
};