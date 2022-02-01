#pragma once


class CModel;


class CModelManager
{
public:
    enum MODELTYPE
    {
        MODELTYPE_INVALID = 0,
        MODELTYPE_NORMAL,
        MODELTYPE_TOON,
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static CModel* CreateModel(const char* pszName);
    static CModel* CreateModelToon(const char* pszName);
    static void DestroyModel(CModel* pModel);

    static void Read(
        const char* pszName,
        MODELTYPE modeltype,
        void* pBuffer,
        uint32 uBufferSize,
        const char* pszTextureSetName,
        int32 pattern
    );

    static void ReadNormal(
        const char* pszName,
        void* pBuffer,
        uint32 uBufferSize,
        const char* pszTextureSetName
    );

    static void ReadToon(
        const char* pszName,
        void* pBuffer,
        uint32 uBufferSize,
        const char* pszTextureSetName,
        int32 pattern
    );
};