#include "DisplayOptionData.hpp"
#include "System/Common/System2D.hpp"

#ifdef TARGET_PC
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

#ifdef TARGET_PC
    m_iVideomodeNum = CPCSpecific::GetVideomodeNum();
    m_iVideomodeCur = CPCSpecific::GetVideomodeCur();    
    m_pVideomode = new VIDEOMODE[m_iVideomodeNum];

    for (int32 i = 0; i < m_iVideomodeNum; ++i)
    {
		PC::VIDEOMODE vm;

		CPCSpecific::GetVideomode(i, vm);
        
		std::sprintf(m_pVideomode[i].m_szName, "%d x %d x %d", vm.w, vm.h, vm.d);
    };
#endif
};


void CDisplayOptionData::Terminate(void)
{
#ifdef TARGET_PC
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
    m_bHelpFlag = true;
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
    rRawData.m_bFontEffectFlag      = m_bFontEffectFlag;
    rRawData.m_bPlayerMarkerFlag    = m_bPlayerMarkerFlag;
    rRawData.m_bHelpFlag            = m_bHelpFlag;

#ifdef TARGET_PC    
    rRawData.m_iVideomodeNo         = m_iVideomodeCur;
#endif        
};


void CDisplayOptionData::Restore(const RAWDATA& rRawData)
{
    m_bFontEffectFlag   = rRawData.m_bFontEffectFlag;
    m_bPlayerMarkerFlag = rRawData.m_bPlayerMarkerFlag;
    m_bHelpFlag         = rRawData.m_bHelpFlag;

#ifdef TARGET_PC    
    m_iVideomodeCur     = rRawData.m_iVideomodeNo;
#endif
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


#ifdef TARGET_PC


void CDisplayOptionData::SetVideomode(int32 No)
{
    ASSERT(No >= 0 && No < m_iVideomodeNum);
    
    m_iVideomodeCur = No;
};


bool CDisplayOptionData::ApplyVideomode(void) const
{
    ASSERT(m_iVideomodeCur >= 0 && m_iVideomodeCur < m_iVideomodeNum);
    
    if (CPCSpecific::SetVideomode(m_iVideomodeCur))
    {
        CSystem2D::Reset();
        return true;
    };

    return false;
};


int32 CDisplayOptionData::GetVideomodeNum(void) const
{
    return m_iVideomodeNum;
};


int32 CDisplayOptionData::GetVideomodeCur(void) const
{
    return m_iVideomodeCur;
};


const char* CDisplayOptionData::GetVideomodeName(int32 No) const
{
    ASSERT(No >= 0 && No < m_iVideomodeNum);
    
    return m_pVideomode[No].m_szName;
};

#endif /* TARGET_PC */