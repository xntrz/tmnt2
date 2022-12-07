#pragma once


class CFilename
{
public:
    static int32 ID(const char* Filename);
    static void ConvPathTable(char* Path);
    static void ConvPathPlatform(char* Path);
    static void Filename(char* Buff, int32 FileId);
};