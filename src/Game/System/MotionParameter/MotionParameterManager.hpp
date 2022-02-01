#pragma once


class CCharacterParameter;


class CMotionParameterManager
{
public:
    static const int32 MOTION_NAME_MAX = 16;
    static const int32 MODEL_NAME_MAX = 16;
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const char* pszName, const void* pBuffer, uint32 uBufferSize);
    static CCharacterParameter* GetCharacterParameter(const char* pszName, int32 nPartsNum);
};