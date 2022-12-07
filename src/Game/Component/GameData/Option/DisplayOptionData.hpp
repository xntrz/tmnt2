#pragma once

#include "OptionTypes.hpp"


class CDisplayOptionData
{
public:
    struct VIDEOMODE
    {
        int32 m_iVideomodeNo;
        char m_szName[64];
    };
    
    struct RAWDATA
    {
#ifdef _TARGET_PC
        int32 m_iVideomodeNo;
#endif        
        bool m_bFontEffectFlag;
        bool m_bPlayerMarkerFlag;
        bool m_bHelpFlag;
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
    void SetEnableFontEffect(bool bSet);
    void SetEnablePlayerMarker(bool bSet);
    bool IsEnabledFontEffect(void) const;
    bool IsEnabledPlayerMarker(void) const;
    void SetEnableHelp(bool bSet);
    bool IsHelpEnabled(void) const;
    int32 VideomodeNum(void);
    int32 VideomodeCur(void);
    const char* VideomodeReso(int32 no);
    void VideomodeAssign(int32 no);
    bool VideomodeApply(void);

private:
#ifdef _TARGET_PC
    VIDEOMODE* m_pVideomode;
    int32 m_iVideomodeNum;
    int32 m_iVideomodeCur;
#endif
    bool m_bHelpFlag;
    bool m_bFontEffectFlag;
    bool m_bPlayerMarkerFlag;    
};