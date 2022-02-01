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

public:
    static VIDEOMODE m_videomode;
    static int32 m_iDispMode;
    static int32 m_iMonitorNo;
    static bool m_bFlagClassicPad;
    static bool m_bFlagFontEffect;
    static bool m_bFlagAutosave;
    static bool m_bFlagHelp;
    static int32 m_iDifficulty;
};