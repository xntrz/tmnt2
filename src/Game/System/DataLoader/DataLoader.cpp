#include "DataLoader.hpp"
#include "DataLoaderImpl.hpp"

#include "System/Common/File/File.hpp"
#include "System/Common/File/FileQueue.hpp"
#include "System/Common/File/FileTypes.hpp"


extern IDataLoaderImpl* CreateDataLoaderImplement(void);
extern IDataLoaderImpl* CreateDataLoaderList(void);


class COpenRequest : public CListNode<COpenRequest>
{
public:
    enum TYPE
    {
        TYPE_ID = 0,
        TYPE_ISO,
    };

    union NAME
    {
        char  path[FILETYPES::FILE_NAME_MAX];
        int32 id;
    };

public:
    COpenRequest(void);
    ~COpenRequest(void);
    void SetFname(const char* pszFilename, bool bList);
    void SetFid(int32 id);
    bool IsList(void) const;
    TYPE GetType(void) const;
    int32 GetFid(void) const;
    const char* GetFname(void) const;

private:
    TYPE m_type;
    NAME m_name;
    bool m_bList;
};


class CDataLoaderContainer
{
public:
#ifdef _DEBUG    
    static const int32 REQUEST_POOL = 128;
#else /* _DEBUG */
    static const int32 REQUEST_POOL = 16;
#endif /* _DEBUG */
    
public:
    CDataLoaderContainer(void);
    ~CDataLoaderContainer(void);
    void Period(void);
    void Regist(int32 fid);
    void Regist(const char* fname, bool bList);
    bool IsLoadEnd(void) const;
    void PushRequest(COpenRequest* pRequest);
    void OpenRequest(COpenRequest* pRequest);
    COpenRequest* AllocOpenRequest(void);
    void FreeOpenRequest(COpenRequest* pRequest);

private:
    CFileQueue<COpenRequest*> m_openQueue;
    COpenRequest              m_aOpenRequest[REQUEST_POOL];
    CList<COpenRequest>       m_listOpenRequestFree;
    IDataLoaderImpl*          m_pDataLoaderImpl;
    IDataLoaderImpl*          m_pDataLoaderList;
    CFile*                    m_pCurrentFile;
    COpenRequest*             m_pCurrentRequest;
};


COpenRequest::COpenRequest(void)
: m_type(TYPE_ID)
, m_name()
, m_bList(false)
{
    m_name.id = -1;
};


COpenRequest::~COpenRequest(void)
{
    ;
};


void COpenRequest::SetFname(const char* pszFilename, bool bList)
{
    m_type = TYPE_ISO;
    std::strcpy(m_name.path, pszFilename);
    m_bList = bList;
};


void COpenRequest::SetFid(int32 fid)
{
    m_type = TYPE_ID;
    m_name.id = fid;
    m_bList = false;
};


bool COpenRequest::IsList(void) const
{
    return m_bList;
};


COpenRequest::TYPE COpenRequest::GetType(void) const
{
    return m_type;
};


int32 COpenRequest::GetFid(void) const
{
    return m_name.id;
};


const char* COpenRequest::GetFname(void) const
{
    return m_name.path;
};


//
// *********************************************************************************
//


CDataLoaderContainer::CDataLoaderContainer(void)
: m_openQueue(COUNT_OF(m_aOpenRequest))
, m_aOpenRequest()
, m_listOpenRequestFree()
, m_pDataLoaderImpl(CreateDataLoaderImplement())
, m_pDataLoaderList(CreateDataLoaderList())
, m_pCurrentFile(nullptr)
, m_pCurrentRequest(nullptr)
{
    ASSERT(m_pDataLoaderImpl);

    for (int32 i = 0; i < COUNT_OF(m_aOpenRequest); ++i)
        m_listOpenRequestFree.push_back(&m_aOpenRequest[i]);
};


CDataLoaderContainer::~CDataLoaderContainer(void) 
{
    if (m_pCurrentFile)
    {
        delete m_pCurrentFile;
        m_pCurrentFile = nullptr;
    };

    if (m_pDataLoaderList)
    {
        delete m_pDataLoaderList;
        m_pDataLoaderList = nullptr;
    };

    if (m_pDataLoaderImpl)
    {
        delete m_pDataLoaderImpl;
        m_pDataLoaderImpl = nullptr;
    };
};


void CDataLoaderContainer::Period(void) 
{
    if (!m_pCurrentFile)
        return;
    
    if (m_pCurrentFile->GetStat() != FILE_STAT_READEND)
        return;
    
    uint8* fdata = m_pCurrentFile->GetData();
    uint32 fsize = m_pCurrentFile->GetSize();

    if (m_pCurrentRequest->IsList())
        m_pDataLoaderList->Eval(fdata, fsize, m_pCurrentRequest->GetFname());
    else
        m_pDataLoaderImpl->Eval(fdata, fsize, reinterpret_cast<const void*>(m_pCurrentRequest->GetFid()));

    delete m_pCurrentFile;
    m_pCurrentFile = nullptr;

    FreeOpenRequest(m_pCurrentRequest);
    m_pCurrentRequest = nullptr;

    if (!m_openQueue.is_empty())
    {
        COpenRequest* pRequest = m_openQueue.front();
        m_openQueue.pop();

        OpenRequest(pRequest);
    };
};


void CDataLoaderContainer::Regist(int32 fid) 
{
    COpenRequest* pRequest = AllocOpenRequest();
    ASSERT(pRequest);

    pRequest->SetFid(fid);

    PushRequest(pRequest);
};


void CDataLoaderContainer::Regist(const char* fname, bool bList)
{
    COpenRequest* pRequest = AllocOpenRequest();
    ASSERT(pRequest);

    pRequest->SetFname(fname, bList);

    PushRequest(pRequest);
};


bool CDataLoaderContainer::IsLoadEnd(void) const
{
    return (m_openQueue.is_empty() && !m_pCurrentRequest);
};


void CDataLoaderContainer::PushRequest(COpenRequest* pRequest)
{
    if (m_pCurrentRequest)
        m_openQueue.push(pRequest);
    else
        OpenRequest(pRequest);
};


void CDataLoaderContainer::OpenRequest(COpenRequest* pRequest)
{
    CFile* pFile = new CFile;

    switch (pRequest->GetType())
    {
    case COpenRequest::TYPE_ID:
        pFile->OpenRequest(pRequest->GetFid());
        break;

    case COpenRequest::TYPE_ISO:
        if (pRequest->IsList())
            pFile->OpenRequest(pRequest->GetFname(), false);
        else
            pFile->OpenRequest(pRequest->GetFname());
        break;

    default:
        ASSERT(false);
        break;
    };

    m_pCurrentRequest = pRequest;
    m_pCurrentFile = pFile;
};


COpenRequest* CDataLoaderContainer::AllocOpenRequest(void)
{
    if (m_listOpenRequestFree.empty())
        return nullptr;

    COpenRequest* pNode = m_listOpenRequestFree.front();
    m_listOpenRequestFree.erase(pNode);

    return pNode;
};


void CDataLoaderContainer::FreeOpenRequest(COpenRequest* pRequest)
{
    m_listOpenRequestFree.push_front(pRequest);
};


static CDataLoaderContainer* s_pDataLoaderContainer = nullptr;


static inline CDataLoaderContainer& DataLoaderContainer(void)
{
    ASSERT(s_pDataLoaderContainer);
    return *s_pDataLoaderContainer;
};


/*static*/ void CDataLoader::Initialize(void)
{
    if (!s_pDataLoaderContainer)
        s_pDataLoaderContainer = new CDataLoaderContainer;
};


/*static*/ void CDataLoader::Terminate(void)
{
    if (s_pDataLoaderContainer)
    {
        delete s_pDataLoaderContainer;
        s_pDataLoaderContainer = nullptr;
    };
};


/*static*/ void CDataLoader::Period(void)
{
    DataLoaderContainer().Period();
};


/*static*/ bool CDataLoader::IsLoadEnd(void)
{
    return DataLoaderContainer().IsLoadEnd();
};


/*static*/ void CDataLoader::Regist(int32 fid)
{
    DataLoaderContainer().Regist(fid);
};


/*static*/ void CDataLoader::Regist(const char* fname, bool bList /*= false*/)
{
    DataLoaderContainer().Regist(fname, bList);
};