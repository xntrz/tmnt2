#include "DataLoader.hpp"
#include "DataLoaderImpl.hpp"

#include "System/Common/File/AdxFile.hpp"
#include "System/Common/File/File.hpp"
#include "System/Common/File/FileTypes.hpp"

#ifdef _DEBUG
#ifdef TARGET_PC
#include "System/PC/File/PCFile.hpp"
#endif /* TARGET_PC */
#endif /* _DEBUG */

#include <queue>
#include <vector>
#include <utility> // std::pair


extern IDataLoaderImpl* CreateDataLoaderImplement(void);


class CDataLoaderContainer
{
private:
    struct REQUEST
    {
        enum TYPE { TYPE_ID, TYPE_ISO, };

        TYPE m_type;
        int32 m_iFileID;
        char m_szFilename[FILETYPES::FILE_NAME_MAX];

        /* ctor for id file type */
        inline REQUEST(int32 fileId)
            : m_type(TYPE_ID), m_iFileID(fileId), m_szFilename() { m_szFilename[0] = '\0'; };

        /* ctor for iso file type */
        inline REQUEST(const char* filename)
            : m_type(TYPE_ISO), m_iFileID(-1), m_szFilename() { std::strcpy(m_szFilename, filename); };

        /* default ctor for prealloc */
        inline REQUEST()
            : m_type(TYPE_ID), m_iFileID(-1), m_szFilename() { m_szFilename[0] = '\0'; };
    };

public:
    CDataLoaderContainer(void);
    ~CDataLoaderContainer(void);
    void Period(void);
    void Regist(int32 iFileID);
    void Regist(const char* pszFilename);
    void CreateFile(int32 iFileID);
    void CreateFile(const char* pszFilename);
    void DestroyFile(void);
    bool IsLoadEnd(void) const;
#ifdef _DEBUG
    void RegistIntercept(CDataLoader::COMPLETE_CB cb, void* pParam);
    void RemoveIntercept(CDataLoader::COMPLETE_CB cb);
#endif /* _DEBUG */

private:
    std::queue<REQUEST> m_queueRequest;
    CFile* m_pCurrentFile;
    IDataLoaderImpl* m_pDataLoaderImpl;
#ifdef _DEBUG
    std::pair<CDataLoader::COMPLETE_CB, void*> m_cbIntercept;
#endif /* _DEBUG */
};


CDataLoaderContainer::CDataLoaderContainer(void)
: m_queueRequest()
, m_pCurrentFile(nullptr)
, m_pDataLoaderImpl(CreateDataLoaderImplement())
{
    ASSERT(m_pDataLoaderImpl);

    // preallocate space for queue
    std::deque<REQUEST> container;
    container.resize(64);
    container.resize(0);

    m_queueRequest = std::queue<REQUEST>(std::move(container));
};


CDataLoaderContainer::~CDataLoaderContainer(void) 
{
    if (m_pCurrentFile)
        DestroyFile();

    if (m_pDataLoaderImpl)
    {
        delete m_pDataLoaderImpl;
        m_pDataLoaderImpl = nullptr;
    };
};


void CDataLoaderContainer::Period(void) 
{
    if (m_pCurrentFile)
    {
        if (m_pCurrentFile->Stat() == CFile::STAT_READEND)
        {
            void* pFileData = m_pCurrentFile->Data();
            uint32 uFileSize = m_pCurrentFile->Size();

            ASSERT(m_pDataLoaderImpl);
#ifdef _DEBUG
            if (m_cbIntercept.first)
                m_cbIntercept.first(pFileData, uFileSize, m_cbIntercept.second);
            else
                m_pDataLoaderImpl->Eval(pFileData, uFileSize);
#else /* _DEBUG */
            m_pDataLoaderImpl->Eval(pFileData, uFileSize);
#endif /* _DEBUG */
    

            DestroyFile();
			m_queueRequest.pop();
        };
    }
    else
    {
        if (!m_queueRequest.empty())
        {
            REQUEST& request = m_queueRequest.front();

            switch (request.m_type)
            {
            case REQUEST::TYPE_ID:
                CreateFile(request.m_iFileID);
                break;

            case REQUEST::TYPE_ISO:
                CreateFile(request.m_szFilename);
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
    m_queueRequest.push(REQUEST(iFileID));
};


void CDataLoaderContainer::Regist(const char* pszFilename) 
{
    m_queueRequest.push(REQUEST(pszFilename));
};


void CDataLoaderContainer::CreateFile(int32 iFileID)
{
    CAdxFileID* pFile = new CAdxFileID;
    if (pFile)
    {
        pFile->Open(iFileID);
        m_pCurrentFile = pFile;
    };
};


void CDataLoaderContainer::CreateFile(const char* pszFilename)
{
#ifdef _DEBUG
#ifdef TARGET_PC
    CPCFile* pFile = new CPCFile;
    if (pFile)
    {
        pFile->Open(pszFilename);
        m_pCurrentFile = pFile;
    };
#endif /* TARGET_PC */
#else /* _DEBUG */
    CAdxFileISO* pFile = new CAdxFileISO;
    if (pFile)
    {
        pFile->Open(pszFilename);
        m_pCurrentFile = pFile;
    };
#endif /* _DEBUG */
};


void CDataLoaderContainer::DestroyFile(void)
{
    m_pCurrentFile->Close();

    delete m_pCurrentFile;
    m_pCurrentFile = nullptr;
};


bool CDataLoaderContainer::IsLoadEnd(void) const
{
    return m_queueRequest.empty();
};

#ifdef _DEBUG

void CDataLoaderContainer::RegistIntercept(CDataLoader::COMPLETE_CB cb, void* pParam)
{
    m_cbIntercept = { cb, pParam };
};


void CDataLoaderContainer::RemoveIntercept(CDataLoader::COMPLETE_CB cb)
{
    ASSERT(m_cbIntercept.first == cb);
    m_cbIntercept = {};
};

#endif /* _DEBUG */


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

#ifdef _DEBUG

/*static*/ void CDataLoader::RegistIntercept(COMPLETE_CB cb, void* pParam /*= nullptr*/)
{
    DataLoaderContainer().RegistIntercept(cb, pParam);
};


/*static*/ void CDataLoader::RemoveIntercept(COMPLETE_CB cb)
{
    DataLoaderContainer().RemoveIntercept(cb);
};

#endif /* _DEBUG */