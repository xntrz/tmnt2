#pragma once


class CAfsFilename
{
public:
    static void MapReadInfo(const char* pszFilename, int32* piFileID, int32* piPartitionID);
    static void MapReadInfo(int32 iFileID, int32* piFileID, int32* piPartitionID);
    static void Filename(char* pszBuffer, int32 iFileID);
    static int32 FileID(const char* pszFilename);
    static void ConvPathTable(char* pszPath);
    static void ConvPathPlatform(char* pszPath);
};