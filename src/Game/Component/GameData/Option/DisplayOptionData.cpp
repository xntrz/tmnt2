#include "DisplayOptionData.hpp"
#include "System/Common/System2D.hpp"

#ifdef _TARGET_PC
#include "System/PC/PCSpecific.hpp"
#endif


CDisplayOptionData::CDisplayOptionData(void)
{
    ;
};


CDisplayOptionData::~CDisplayOptionData(void)
{
    ;
};


void CDisplayOptionData::Initialize(void)
{
    SetDefault();

#ifdef _TARGET_PC
    int32 iVideomodeNum = CPCSpecific::GetVideomodeNum();
    int32 iVideomodeCur = CPCSpecific::GetVideomodeCur();
    
    m_pVideomode = new VIDEOMODE[iVideomodeNum];
    ASSERT(m_pVideomode);

	m_iVideomodeNum = 0;
    for (int32 i = 0, no = 0; i < iVideomodeNum; ++i)
    {
        if (CPCSpecific::IsVideomodeWindow(i))
            continue;
        
        int32 Width = 0;
        int32 Height = 0;
        int32 Depth = 0;
        CPCSpecific::GetVideomode(i, Width, Height, Depth);
        
        std::sprintf(m_pVideomode[no].m_szName, "%d x %d x %d", Width, Height, Depth);
        m_pVideomode[no].m_iVideomodeNo = i;

		if (i == iVideomodeCur)
			m_iVideomodeCur = no;

        ++no;
        ++m_iVideomodeNum;
    };
#endif
};


void CDisplayOptionData::Terminate(void)
{
#ifdef _TARGET_PC
    if (m_pVideomode)
    {
        delete[] m_pVideomode;
        m_pVideomode = nullptr;

        m_iVideomodeNum = 0;
        m_iVideomodeCur = -1;
    };
#endif
};


void CDisplayOptionData::SetDefault(void)
{
    m_bFontEffectFlag = true;
    m_bPlayerMarkerFlag = true;
};


void CDisplayOptionData::Apply(void)
{
    ;
};


bool CDisplayOptionData::IsValid(void) const
{
    return true;
};


void CDisplayOptionData::Snapshot(RAWDATA& rRawData) const
{
    rRawData.m_bFontEffectFlag = m_bFontEffectFlag;
    rRawData.m_bPlayerMarkerFlag = m_bPlayerMarkerFlag;
    rRawData.m_bHelpFlag = m_bHelpFlag;
};


void CDisplayOptionData::Restore(const RAWDATA& rRawData)
{
    m_bFontEffectFlag = rRawData.m_bFontEffectFlag;
    m_bPlayerMarkerFlag = rRawData.m_bPlayerMarkerFlag;
    m_bHelpFlag = rRawData.m_bHelpFlag;
};


void CDisplayOptionData::SetEnableFontEffect(bool bSet)
{
    m_bFontEffectFlag = bSet;
};


void CDisplayOptionData::SetEnablePlayerMarker(bool bSet)
{
    m_bPlayerMarkerFlag = bSet;
};


bool CDisplayOptionData::IsEnabledFontEffect(void) const
{
    return m_bFontEffectFlag;
};


bool CDisplayOptionData::IsEnabledPlayerMarker(void) const
{
    return m_bPlayerMarkerFlag;
};


void CDisplayOptionData::SetEnableHelp(bool bSet)
{
    m_bHelpFlag = bSet;
};


bool CDisplayOptionData::IsHelpEnabled(void) const
{
    return m_bHelpFlag;
};


int32 CDisplayOptionData::VideomodeNum(void)
{
#ifdef _TARGET_PC    
    return m_iVideomodeNum;
#else
    return -1;
#endif  
};


int32 CDisplayOptionData::VideomodeCur(void)
{
#ifdef _TARGET_PC    
    return m_iVideomodeCur;
#else
    return -1;
#endif  
};


const char* CDisplayOptionData::VideomodeReso(int32 no)
{
#ifdef _TARGET_PC    
    ASSERT((no >= 0) && (no < m_iVideomodeNum));
    return m_pVideomode[no].m_szName;
#else
    return nullptr;
#endif    
};


void CDisplayOptionData::VideomodeAssign(int32 no)
{
#ifdef _TARGET_PC  
    m_iVideomodeCur = no;
#endif    
};


bool CDisplayOptionData::VideomodeApply(void)
{
#ifdef _TARGET_PC  
    ASSERT((m_iVideomodeCur >= 0) && (m_iVideomodeCur < m_iVideomodeNum));
    if (CPCSpecific::SetVideomode(m_pVideomode[m_iVideomodeCur].m_iVideomodeNo))
    {
        CSystem2D::ScreenChanged();
        return true;
    };

    return false;
#else
    return false;
#endif    
};