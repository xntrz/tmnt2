#pragma once


class CAfsFileSystem
{
public:
    static const int32 FNAME_MAX = 256;
    
public:
    static void Initialize(const char* pszPath);
    static void Terminate(void);
    static int32 PartitionAllocID(void);
    static bool PartitionMount(int32 iPartitionID, const char* pszArchive);
    static void PartitionUnmount(int32 iPartitionID);
    static int32 FileSize(int32 iPartitionID, int32 iFileID);
    static bool FileRead(int32 iPartitionID, int32 iFileID, void* buffer, int32 bufferSize);
};