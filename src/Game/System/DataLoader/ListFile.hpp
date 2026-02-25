#pragma once



class CListFile
{
public:
    static const int32 PATH_MAX_LEN = 256;

    class CParam
    {
    public:
        enum TYPE
        {
            TYPE_INT = 0,
            TYPE_STR,
        };

        union VALUE
        {
            int32 integer;
            char  string[16];
        };

    public:
        static CParam& Dummy(void);
        
        CParam(void);
        ~CParam(void);
        void Parse(const char* pszParam, uint32 paramLen);
        TYPE GetType(void) const;
        int32 AsInt(void) const;
        const char* AsStr(void) const;

    private:
        bool isNumber(const char* pszStr) const;
        void removeSpaces(char* pszStr, uint32 strLen) const;

    private:
        TYPE  m_type;
        VALUE m_value;
    };

    class CPath
    {
    public:
        static const int32 MAX_PARAM = 4;

    public:
        static CPath& Dummy(void);

        CPath(void);
        ~CPath(void);
        void Parse(const char* pszPath, uint32 pathLen);
        bool IsValid(void) const;
        bool IsList(void) const;
        int32 GetParamNum(void) const;
        const CParam& GetParam(int32 no) const;
        const char* GetStr(void) const;
        const char* GetFileExt(void) const;
        const char* GetFileName(void) const;

    private:
        char   m_szPath[PATH_MAX_LEN];
        char   m_szFExt[32];
        char   m_szFName[64];
        CParam m_aParam[MAX_PARAM];
        int32  m_nParamNum;
    };

public:
    CListFile(void);
    ~CListFile(void);
    void Parse(const char* pBuff, uint32 uBuffSize);
    int32 GetPathNum(void) const;
    const CPath& SearchPath(const char* pszPath) const;
    const CPath& GetPath(int32 idx) const;

private:    
    char* createTmpBuf(const char* pBuff, uint32 uBuffSize, uint32* len);
    void  destroyTmpBuf(char* tmp);
    int32 parseBuf(const char* buf, uint32 len, bool bCountLines);
    void  parseLine(char* line, uint32 len, int32 no);
    void  removeSpaces(char* line, uint32 len);

private:
    CPath* m_aPaths;
    int32  m_nPathNum;
};