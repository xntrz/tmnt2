#pragma once


class CConfigure
{
public:
    enum LAUNCHMODE
    {
        LAUNCHMODE_NORMAL = 0,
        LAUNCHMODE_ARCADE,
        LAUNCHMODE_TVCHANGE,
        LAUNCHMODE_DASHBOARD,
    };
    
public:
    static LAUNCHMODE m_launchmode;
};