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
    else
        return FILEID::ID_INVALID;
};


int32 CGameObjLoadInfo::GetDependObjInfoNum(void) const
{
    return m_nDependObjLoadNum;
};


CGameObjLoadInfo* CGameObjLoadInfo::GetDependObjInfo(int32 nIndex)
{
    ASSERT(nIndex >= 0 && nIndex < m_nDependObjLoadNum);
    
    return m_apDependObjInfo[nIndex];
};


const char* CGameObjLoadInfo::GetFileName(void) const
{
    static char s_szFnameWork[FILETYPES::FILE_NAME_MAX - 1];

    if (MakeFileName(s_szFnameWork))
    {
        ASSERT(std::strlen(s_szFnameWork) < COUNT_OF(s_szFnameWork));
        return s_szFnameWork;
    }
    else
    {
        return nullptr;
    };
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
            ASSERT(i < m_nDependObjLoadNum);
            
            delete m_apDependObjInfo[i];
            m_apDependObjInfo[i] = nullptr;
        };
    };

    m_nDependObjLoadNum = 0;
};

