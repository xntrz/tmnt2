#pragma once


class CFileAccess;


class CFileManager
{
protected:
    struct REQUEST
    {
        enum TYPE
        {
            TYPE_ID = 0,
            TYPE_NAME,
        };

        CFileAccess* m_pAccessData;
        int32 m_nLabel;
        TYPE m_type;
        union
        {
            char* name;
            int32 id;
        } m_data;

        void SetName(const char* pszName);
        void ClearName(void);
    };

    enum STATE
    {
        STATE_IDLE,
        STATE_LOADING,
    };

public:
    static CFileManager& Instance(void);
    
    CFileManager(void);
    virtual ~CFileManager(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Sync(void);
    virtual void Error(const char* pszDescription);
    virtual void Reset(void) = 0;
    virtual CFileAccess* AllocRequest(int32 nID, int32 nLabel);
    virtual CFileAccess* AllocRequest(const char* pszName, int32 nLabel);

protected:
    void RegistRequest(const REQUEST& rRequest);

private:
    static CFileManager* m_pInstance;
    STATE m_state;
    CQueue<REQUEST> m_RequestQueue;
    REQUEST* m_pCurrentRequest;
};