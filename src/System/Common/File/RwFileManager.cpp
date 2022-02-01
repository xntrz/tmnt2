#include "RwFileManager.hpp"
#include "RwFileSystem.hpp"


static const int32 FILESYSTEM_MAX = 8;
static int32 s_nNumFS = 0;


/*static*/ bool CRwFileManager::Initialize(void)
{
    if (!RtFSManagerOpen(FILESYSTEM_MAX))
        return false;
    
    s_nNumFS = 0;

    return true;
};


/*static*/ void CRwFileManager::Terminate(void)
{
    ASSERT(s_nNumFS == 0);
    
    RtFSManagerClose();
};


/*static*/ void CRwFileManager::Regist(CRwFileSystem* pRwFileSystem, bool bSetAsDefault)
{
    ASSERT(pRwFileSystem);
    
    RtFSManagerRegister(pRwFileSystem);
    
    if (bSetAsDefault)
        RtFSManagerSetDefaultFileSystem(pRwFileSystem);

    ++s_nNumFS;
};


/*static*/ void CRwFileManager::Remove(CRwFileSystem* pRwFileSystem)
{
    ASSERT(pRwFileSystem);
    
    ASSERT(s_nNumFS > 0);
    --s_nNumFS;
    
    RtFSManagerUnregister(pRwFileSystem);
};