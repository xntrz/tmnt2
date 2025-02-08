#include "GameObjLoadInfo.hpp"

#include "System/Common/File/FileTypes.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/File/Filename.hpp"


CGameObjLoadInfo::CGameObjLoadInfo(void)
: m_nDependObjLoadNum(0)
{
    initDependObjInfo();
};


CGameObjLoadInfo::~CGameObjLoadInfo(void)
{
    freeDependObjInfo();
};


int32 CGameObjLoadInfo::GetFileID(void) const
{
    const char* pszFilename = GetFileName();
    if (pszFilename)
        return CFilename::ID(pszFilename);

    return FILEID::ID_INVALID;
};


const char* CGameObjLoadInfo::GetFileName(void) const
{
    static char s_szFnameWork[FILETYPES::FILE_NAME_MAX - 1];

    return (MakeFileName(s_szFnameWork) ? s_szFnameWork : nullptr);    
};


int32 CGameObjLoadInfo::GetDependObjInfoNum(void) const
{
    return m_nDependObjLoadNum;
};


CGameObjLoadInfo& CGameObjLoadInfo::GetDependObjInfo(int32 nIndex)
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nDependObjLoadNum);

    return *m_apDependObjInfo[nIndex];
};


void CGameObjLoadInfo::initDependObjInfo(void)
{
    m_nDependObjLoadNum = 0;
    
    for (int32 i = 0; i < COUNT_OF(m_apDependObjInfo); ++i)
        m_apDependObjInfo[i] = nullptr;
};


void CGameObjLoadInfo::freeDependObjInfo(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apDependObjInfo); ++i)
    {
        if (m_apDependObjInfo[i])
        {
            delete m_apDependObjInfo[i];
            m_apDependObjInfo[i] = nullptr;
        };
    };

    m_nDependObjLoadNum = 0;
};

