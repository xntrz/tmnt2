#pragma once


class CChainMotionParameter
{
public:
    enum
    {
        CHAIN_MAX = 3,        
    };

    struct INIT_PARAMETER
    {
        float m_fStart;
        float m_fEnd;
    };

    struct ATTACH_PARAMETER
    {
        int32 m_nButton;
        char m_szMotionName[16];
    };

public:
    CChainMotionParameter(INIT_PARAMETER* pParam);
    virtual ~CChainMotionParameter(void);
    void Initialize(INIT_PARAMETER* pParam);
    void Attach(ATTACH_PARAMETER* pParam);
    float GetNextChainMotionConnectTime(const char* pszNextMotion);

private:
    float m_fStart;
    float m_fEnd;
    char m_aszMotionName[CHAIN_MAX][16];
};