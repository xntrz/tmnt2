#pragma once

#include "OptionTypes.hpp"


class CDisplayOptionData
{
public:
    struct RAWDATA
    {
#ifdef _TARGET_PC        
        int32 m_iDisplaymode;
        int32 m_iVideomodeNo;
#endif        
        bool m_bFontEffectFlag;
        bool m_bPlayerMarkerFlag;
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

private:
#ifdef _TARGET_PC
    int32 m_iDisplaymode;
    int32 m_iVideomodeNo;
#endif
    bool m_bFontEffectFlag;
    bool m_bPlayerMarkerFlag;    
};