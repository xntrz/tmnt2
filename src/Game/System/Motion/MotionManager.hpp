#pragma once


class CMotion;


class CMotionManager
{
public:
    static const char* GLOBAL_SET;
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void CreateMotionSet(const char* pszSetName, const char* pszParentSetName);
    static void SetCurrentMotionSet(const char* pszSetName);
    static void Read(const char* pszName, void* pBuffer, uint32 uBufferSize);
    static CMotion* CreateMotion(const char* pszName);
};