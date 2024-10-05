#pragma once

#include "RwFileSystemManager.hpp"

#include <queue>


class CFileAccess;


class CFileManager
{
protected:
    class CRequest
    {
    public:
        enum TYPE
        {
            TYPE_ID = 0,
            TYPE_NAME,
        };
        
    public:
        CRequest(const char* pszName, CFileAccess* pAccess);
        CRequest(int32 nID, CFileAccess* pAccess);
        void Cleanup(void);
        
        inline TYPE type(void) const { return m_type; };
        inline int32 id(void) const { return m_data.id; };
        inline const char* name(void) const { return m_data.name; };
        inline CFileAccess* access(void) { return m_pAccess; };
        
    private:
        CFileAccess* m_pAccess;
        TYPE m_type;
        union
        {
            char* name;
            int32 id;
        } m_data;
    };

    enum STATE
    {
        STATE_IDLE = 0,
        STATE_LOADING,
    };

public:
    static CFileManager& Instance(void);
    
    CFileManager(void);
    virtual ~CFileManager(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Sync(void);
    virtual void Error(const char* pszDescription) = 0;
    virtual void Reset(void) = 0;
    virtual CFileAccess* AllocRequest(int32 nType, void* pTypeData) = 0;

protected:
    void RegistRequest(CRequest& Request);

private:
    static CFileManager* m_pInstance;
    STATE m_state;
    std::queue<CRequest> m_ReqQueue;
    CRequest* m_pReqCurrent;
    CRwFileSystemManager m_rwFileSystem;
};