#pragma once


class CModelToon;


class CToonManager
{
public:
    enum PATTERN
    {
        PATTERN_START = 0,

        PATTERN_1 = PATTERN_START,
        PATTERN_2,
        PATTERN_3,
        PATTERN_4,
        PATTERN_5,

        PATTERN_MAX,
    };
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void RegistModel(CModelToon* pModel);
    static void RemoveModel(CModelToon* pModel);
    static void SetTextureSet(const char* pszName);

private:
    static const char* m_apszPatternTextureName[];
};