#include "RwFileSystemManager.hpp"
#include "RwFileSystem.hpp"


bool CRwFileSystemManager::Initialize(void)
{
    if (!RtFSManagerOpen(2))
        return false;

    m_pFileSystem = static_cast<CRwFileSystem*>(RwMalloc(sizeof(CRwFileSystem), rwMEMHINTDUR_GLOBAL));
    
    if (!m_pFileSystem)
        return false;

    if (!m_pFileSystem->Initialize())
        return false;

    if (!RtFSManagerRegister(m_pFileSystem))
        return false;

    RtFSManagerSetDefaultFileSystem(m_pFileSystem);

    return true;
};


void CRwFileSystemManager::Terminate(void)
{
    if (m_pFileSystem)
    {
        m_pFileSystem->Terminate();
        RtFSManagerUnregister(m_pFileSystem);
        
        m_pFileSystem = nullptr;
    };

    RtFSManagerClose();
};