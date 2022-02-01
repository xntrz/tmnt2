#pragma once


class CUVAnim
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void ReadUVAnim(const char* pszName, const void* pBuffer, uint32 uBufferSize);
    static void UpdateUVAnim(void* pAnimatedMaterials, float dt);
    static void InitUVAnim(void* hList);
    static void* CreateAnimatedMaterialsList(RpClump* pClump);
    static void* CreateAnimatedMaterialsList(RpWorld* pWorld);
    static void DestroyAnimatedMaterialsList(void* hList);
    static void SetUVAnimDict(const char* pszName);
    static bool IsExistsUVAnimDict(const char* pszName);
};