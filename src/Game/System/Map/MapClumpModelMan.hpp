#pragma once


class CMapClumpModelManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(const char* pszName, int32 mode);
    static void Draw(void);
    static void CreateModel(void);
    static void SetDrawEnable(const char* pszName, bool bEnable);
};