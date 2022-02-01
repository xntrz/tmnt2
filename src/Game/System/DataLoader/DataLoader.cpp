#include "DataLoader.hpp"
#include "DataLoaderImpl.hpp"

#include "System/Common/File/AfsFile.hpp"
#include "System/Common/File/File.hpp"
#include "System/Common/File/FileTypes.hpp"


IDataLoaderImpl* CreateDataLoaderImplement(void);


template<class T>
class CRequestQueue
{
private:
    struct NODE : public CListNode<NODE>
    {
        T m_payload;
    };
    
public:
    CRequestQueue(int32 nNumNode)
    : m_paNode(new NODE[nNumNode])
    , m_nNumNode(nNumNode)
    {
        for (int32 i = 0; i < m_nNumNode; ++i)
        {
            std::memset(&m_paNode[i].m_payload, 0x00, sizeof(m_paNode[i].m_payload));
            m_listNodeFree.push_back(&m_paNode[i]);
        };
    };


    ~CRequestQueue(void)
    {
        if (m_paNode)
        {
            delete [] m_paNode;
            m_paNode = nullptr;
        };
    };


    void push(T* data)
    {
        if (m_listNodeFree.empty())
        {
            ASSERT(false);
            return;
        };
        
        NODE* pNode = m_listNodeFree.front();
        m_listNodeFree.erase(pNode);
        m_listNodeAlloc.push_front(pNode);

        pNode->m_payload = *data;
    };


    void pop(void)
    {
        if (m_listNodeAlloc.empty())
        {
            ASSERT(false);
            return;
        };

        NODE* pNode = m_listNodeAlloc.back();
        m_listNodeAlloc.erase(pNode);
        m_listNodeFree.push_back(pNode);
    };


    T* peek(void)
    {
        ASSERT(!empty());

        NODE* pNode = m_listNodeAlloc.back();
        return &pNode->m_payload;
    };


    bool empty(void) const
    {
        return m_listNodeAlloc.empty();
    };

private:
    NODE* m_paNode;
    int32 m_nNumNode;
    CList<NODE> m_listNodeAlloc;
    CList<NODE> m_listNodeFree;
};


class CDataLoaderContainer
{
private:
    struct REQUEST
    {
        enum TYPE
        {
            TYPE_ID,
            TYPE_ISO,
        };

        TYPE m_type;
        union
        {
            int32 m_iFileID;
            char m_szFilename[FILETYPES::FILE_NAME_MAX];
        };
    };
    
public:
    CDataLoaderContainer(void);
    virtual ~CDataLoaderContainer(void);
    void Period(void);
    void Regist(int32 iFileID);
    void Regist(const char* pszFilename);
    void CreateFile(int32 iFileID);
    void CreateFile(const char* pszFilename);
    void DestroyFile(void);
    bool IsLoadEnd(void) const;

private:
    CRequestQueue<REQUEST> m_RequestQueue;
    CFile* m_pCurrentFile;
    IDataLoaderImpl* m_pDataLoaderImpl;
};


CDataLoaderContainer::CDataLoaderContainer(void)
: m_RequestQueue(32)
, m_pCurrentFile(nullptr)
, m_pDataLoaderImpl(CreateDataLoaderImplement())
{
    ASSERT(m_pDataLoaderImpl);
};


CDataLoaderContainer::~CDataLoaderContainer(void) 
{
    if (m_pDataLoaderImpl)
    {
        delete m_pDataLoaderImpl;
        m_pDataLoaderImpl = nullptr;
    };

    if (m_pCurrentFile)
    {
        m_pCurrentFile->Close();
        delete m_pCurrentFile;
        m_pCurrentFile = nullptr;
    };
};


void CDataLoaderContainer::Period(void) 
{
    if (m_pCurrentFile)
    {
        if (m_pCurrentFile->Status() == CFile::STATUS_READEND)
        {
            const void* pFileData = m_pCurrentFile->Data();
            uint32 uFileSize = m_pCurrentFile->Size();

            ASSERT(m_pDataLoaderImpl);
            m_pDataLoaderImpl->Eval(pFileData, uFileSize);
            
            DestroyFile();
			m_RequestQueue.pop();
        };
    }
    else
    {
        if (!m_RequestQueue.empty())
        {
            REQUEST* pRequest = m_RequestQueue.peek();

            switch (pRequest->m_type)
            {
            case REQUEST::TYPE_ID:
                CreateFile(pRequest->m_iFileID);
                break;

            case REQUEST::TYPE_ISO:
                CreateFile(pRequest->m_szFilename);
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CDataLoaderContainer::Regist(int32 iFileID) 
{
    REQUEST Request;
    Request.m_type = REQUEST::TYPE_ID;
    Request.m_iFileID = iFileID;

    m_RequestQueue.push(&Request);
};


void CDataLoaderContainer::Regist(const char* pszFilename) 
{
    ASSERT(strlen(pszFilename) < COUNT_OF(REQUEST::m_szFilename));
    
    REQUEST Request;
    Request.m_type = REQUEST::TYPE_ID;
    std::strcpy(Request.m_szFilename, pszFilename);

    m_RequestQueue.push(&Request);
};


void CDataLoaderContainer::CreateFile(int32 iFileID)
{
    CAfsFileID* pFile = new CAfsFileID;
    if (pFile)
    {
        pFile->Open(iFileID);
        m_pCurrentFile = pFile;
    };
};


void CDataLoaderContainer::CreateFile(const char* pszFilename)
{
    ASSERT(!m_pCurrentFile);

    CAfsFileISO* pFile = new CAfsFileISO;
    if (pFile)
    {
        pFile->Open(pszFilename);
        m_pCurrentFile = pFile;
    };
};


void CDataLoaderContainer::DestroyFile(void)
{
    ASSERT(m_pCurrentFile);
    
    m_pCurrentFile->Close();
    delete m_pCurrentFile;
    m_pCurrentFile = nullptr;
};


bool CDataLoaderContainer::IsLoadEnd(void) const
{
    return m_RequestQueue.empty();
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


/*static*/ void CDataLoader::Regist(int32 iFileID)
{
    DataLoaderContainer().Regist(iFileID);
};


/*static*/ void CDataLoader::Regist(const char* pszFilename)
{
    DataLoaderContainer().Regist(pszFilename);
};