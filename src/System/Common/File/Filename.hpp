#pragma once


class CFilename
{
public:
    static int32 ID(const char* pszFilename);
    static void ConvPathTable(char* pszPath);
    static void ConvPathPlatform(char* pszPath);
    static void Filename(char* Buff, int32 FileId);
};