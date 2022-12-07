#pragma once


class CPCSetting
{
public:
    struct VIDEOMODE
    {
        int32 m_iWidth;
        int32 m_iHeight;
        int32 m_iDepth;
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Load(void);
    static void Save(void);
    static void GetIniPath(std::string& Path);

public:
    static VIDEOMODE m_videomode;
    static int32 m_iDispMode;
    static int32 m_iMonitorNo;
};