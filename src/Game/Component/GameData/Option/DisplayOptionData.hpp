#pragma once

#include "OptionTypes.hpp"


class CDisplayOptionData
{
public:
#ifdef TARGET_PC    
    struct VIDEOMODE
    {
        char m_szName[64];
    };
#endif    

    struct RAWDATA
    {
        bool m_bPlayerMarkerFlag;
        bool m_bFontEffectFlag;
        bool m_bHelpFlag;
#ifdef TARGET_PC
        int32 m_iVideomodeNo;
#endif
    };

public:
    CDisplayOptionData(void);
    ~CDisplayOptionData(void);
    void Initialize(void);
    void Terminate(void);
    void SetDefault(void);
    void Apply(void);
    bool IsValid(void) const;
    void Snapshot(RAWDATA& rRawData) const;
    void Restore(const RAWDATA& rRawData);
    void SetEnableHelp(bool bSet);
    void SetEnableFontEffect(bool bSet);
    void SetEnablePlayerMarker(bool bSet);
    bool IsHelpEnabled(void) const;
    bool IsEnabledFontEffect(void) const;
    bool IsEnabledPlayerMarker(void) const;
#ifdef TARGET_PC    
    void SetVideomode(int32 No);
    bool ApplyVideomode(void) const;
    int32 GetVideomodeNum(void) const;
    int32 GetVideomodeCur(void) const;
    const char* GetVideomodeName(int32 No) const;
#endif

private:
    bool m_bHelpFlag;
    bool m_bFontEffectFlag;
    bool m_bPlayerMarkerFlag;    
#ifdef TARGET_PC
    VIDEOMODE* m_pVideomode;
    int32 m_iVideomodeNum;
    int32 m_iVideomodeCur;
#endif
};