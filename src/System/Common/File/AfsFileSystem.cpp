#include "AfsFileSystem.hpp"
#include "AfsFilename.hpp"

#include "afs.hpp"


static const int32 AFS_PARTITION_MAX = 64;
static const int32 AFS_SCT_ALIGN = 2048;


static const char* s_pszRootPath = nullptr;
static void* s_aAfsPartitionHandle[AFS_PARTITION_MAX];
static uint32 s_uRef = 0;
static uint32 s_uNumMounted = 0;


static void AfsMsgEndpoint(afs_note_type type, const char* msg)
{
    switch (type)
    {
    case afs_note_error:
		OUTPUT("[AFS] %s\n", msg);
        break;

    case afs_note_msg:
        OUTPUT("[AFS] %s\n", msg);
        break;

    default:
        OUTPUT("[AFS] Unknown msg type\n");
        DBGBREAK;
        break;
    };    
};


/*static*/ void CAfsFileSystem::Initialize(const char* pszPath)
{
    if (!s_uRef++)
    {
        s_pszRootPath = pszPath;
        afs_opt_note(AfsMsgEndpoint);
        afs_opt_align(AFS_SCT_ALIGN);
    };    
};


/*static*/ void CAfsFileSystem::Terminate(void)
{
	if (s_uRef)
	{
		--s_uRef;
		ASSERT(!s_uNumMounted);
	};    
};


/*static*/ int32 CAfsFileSystem::PartitionAllocID(void)
{
    for (int32 i = 0; i < COUNT_OF(s_aAfsPartitionHandle); ++i)
    {
        if (!s_aAfsPartitionHandle[i])
            return i;
    };

    return -1;
};


/*static*/ bool CAfsFileSystem::PartitionMount(int32 iPartitionID, const char* pszArchive)
{
    ASSERT(iPartitionID >= 0 && iPartitionID < COUNT_OF(s_aAfsPartitionHandle));
    ASSERT(!s_aAfsPartitionHandle[iPartitionID]);
    
    void* PtHandle = s_aAfsPartitionHandle[iPartitionID];
    if (PtHandle)
        return false;

    std::string ArchivePath;
    ArchivePath.reserve(128);
	if (strlen(s_pszRootPath))
	{
		ArchivePath += s_pszRootPath;
		ArchivePath += '/';
	};
    ArchivePath += pszArchive;

    CAfsFilename::ConvPathPlatform(&ArchivePath[0]);

    PtHandle = afs_open(ArchivePath.c_str());
    if (!PtHandle)
        return false;

    s_aAfsPartitionHandle[iPartitionID] = PtHandle;
    ++s_uNumMounted;
    
    return true;
};


/*static*/ void CAfsFileSystem::PartitionUnmount(int32 iPartitionID)
{
    ASSERT(iPartitionID >= 0 && iPartitionID < COUNT_OF(s_aAfsPartitionHandle));
    ASSERT(s_aAfsPartitionHandle[iPartitionID]);

    void* PtHandle = s_aAfsPartitionHandle[iPartitionID];
    if (!PtHandle)
        return;    

    ASSERT(s_uNumMounted > 0);
    --s_uNumMounted;
    afs_close(PtHandle);
    s_aAfsPartitionHandle[iPartitionID] = nullptr;
};


/*static*/ int32 CAfsFileSystem::FileSize(int32 iPartitionID, int32 iFileID)
{
    ASSERT(iPartitionID >= 0 && iPartitionID < COUNT_OF(s_aAfsPartitionHandle));
    ASSERT(s_aAfsPartitionHandle[iPartitionID]);

    void* PtHandle = s_aAfsPartitionHandle[iPartitionID];
    if (!PtHandle)
        return 0;

    return afs_fsize(PtHandle, iFileID);
};


/*static*/ bool CAfsFileSystem::FileRead(int32 iPartitionID, int32 iFileID, void* buffer, int32 bufferSize)
{
    ASSERT(iPartitionID >= 0 && iPartitionID < COUNT_OF(s_aAfsPartitionHandle));
    ASSERT(s_aAfsPartitionHandle[iPartitionID]);

    void* PtHandle = s_aAfsPartitionHandle[iPartitionID];
    if (!PtHandle)
        return false;

    return afs_read(PtHandle, iFileID, buffer, bufferSize);
};

