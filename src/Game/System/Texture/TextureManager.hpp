#pragma once


class CTextureManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void GenerationInc(void);
    static void GenerationDec(void);
    static void AddDict(const char* pszName, RwTexDictionary* pTexDict);
    static void AddSet(const char* pszName, void* pBuffer, uint32 uBufferSize);
    static void ReadSet(const char* pszName, void* pBuffer, uint32 uBufferSize);
    static RwTexture* GetRwTexture(const char* pszName);
    static RwTexDictionary* GetRwTextureDictionary(const char* pszName);
    static void SetCurrentTextureSet(const char* pszName);    
};