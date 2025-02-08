#include "MakeFileSequence.hpp"

#ifdef _DEBUG

#include "Game/ProcessList.hpp"
#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/File/Filename.hpp"
#include "System/Common/File/FileTypes.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Process/ProcessMail.hpp"


/* copy from DataLoaderFromBinary.cpp */
#define CHUNK_ALIGNMENT_SIZE    (64)


/* copy from DataLoaderFromBinary.cpp */
namespace READERTYPE
{
    enum VALUE
    {
        MODEL = 0,
        MODEL_TOON,
        MOTION_SET,
        MOTION,
        MOTION_PARAMETER,

        TEXTURE_DICTIONARY = 7,
        MODEL_BSP,
        CAMERA,
        EFFECT,
        ANIM2D,
        UVANIM,
        FONT,
        MAP_MODEL,
        MAP_INFO,
        ADD_TEXTURE_DICTIONARY,
        GIMMICK,
        STRINGS,
        ENEMY_PARAMETER,
        ICONS_PS2,
        ICONS_NGC,

        TYPE_MAX,
    };
};


class CMakeFileSequence final : public CSequence
{
public:
    enum MODE
    {
        MODE_EXTRACT = 0,
        MODE_PRINT,
        MODE_ENUM,

        MODE_DEFAULT = MODE_PRINT,
    };

    struct REQUEST
    {
        MODE         mode;
        int32        fileId;
        const char*  filename;
        LPAC_ENUM_CB cb;
        void*        cbParam;
    };

private:
    /* copy from DataLoaderFromBinary.cpp */
    class CFileHeader
    {
    public:
        uint32 m_uMagic;
        uint32 m_uChunkNum;
    };

    /* copy from DataLoaderFromBinary.cpp */
    class CChunkHeader
    {
    public:
        struct HEADER
        {
            uint32 m_uType;
            uint32 m_uSize;
            uint32 m_uReserved[2];
            char m_szName[16];
        };

    public:
        HEADER* m_pHeader;
        void* m_pExtraHeader;
    };

    struct FILENODE : public CListNode<FILENODE>
    {
        enum STEP
        {
            STEP_TEXTURE_DICT = 0,
            STEP_MODEL,
            STEP_MOTION_SET,
            STEP_MOTION,
            STEP_MOTION_PARAM,
            STEP_ADD_TEXTURE_DICT,
            STEP_EFFECT,
        };

        int32  step;
        char   name[16];
        FILE*  file;
        uint32 chunkNum;
    };

    struct LAST_TEXDICT
    {
        CChunkHeader::HEADER*   header;
		void*					buffer;
    };

public:
    static void MakeRequest(CSequence* pCaller, MODE mode, int32 fileId, const char* filename, LPAC_ENUM_CB cb, void* cbParam);
    static void ExtractRequest(CSequence* pCaller, int32 fileId, const char* filename);
    static void PrintRequest(CSequence* pCaller, int32 fileId);
    static void EnumRequest(CSequence* pCaller, int32 fileId, LPAC_ENUM_CB cb, void* cbParam);
    static void LoadInterceptCallback(void* pBufferOrg, uint32 uBufferSize, void* pParam);
    
    CMakeFileSequence(void);
    virtual ~CMakeFileSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void RegistFile(int32 fileId);
    void PreEval(void);
    void PostEval(void);
    void Eval(void* pBufferOrg, uint32 sizeBuffer);
    void ReadChunk(CChunkHeader* pChunkHdr, void* pBuffer);
    bool IsTarget(const char* pszFilename) const;
    FILENODE* AllocFileNode(const char* pszName);
    FILENODE* FindFileNode(const char* pszName);
    void WriteFileNodeHeader(FILENODE* pNode, bool bReserve);
    void WriteFileNodeChunkHeader(FILENODE* pNode, CChunkHeader::HEADER* pHeader);
    void WriteFileNodeChunkTrailer(FILENODE* pNode, CChunkHeader::HEADER* pHeader);
    void WriteFileNode(FILENODE* pNode, const void* pBuffer, uint32 uBufferSize);
    uint32 MakeReaderMask(int32 readerType) const;

private:
    static REQUEST  m_request;
    static bool     m_bBusy;
    FILENODE        m_aFileNode[64];
    CList<FILENODE> m_listFileNodeFree;
    CList<FILENODE> m_listFileNodeAlloc;
    FILENODE*       m_pFileNodeCurrent;
    bool            m_bComplete;
    uint32          m_readerCurr;
    uint32          m_nextReaderMask;
    LAST_TEXDICT    m_lastTexDict;
};


/*static*/ bool CMakeFileSequence::m_bBusy = false;
/*static*/ CMakeFileSequence::REQUEST CMakeFileSequence::m_request;


/*static*/ void CMakeFileSequence::MakeRequest(CSequence* pCaller, MODE mode, int32 fileId, const char* filename, LPAC_ENUM_CB cb, void* cbParam)
{
    ASSERT(pCaller);

    ASSERT(m_bBusy == false, "makefile is busy");
    m_bBusy = true;

    m_request.mode      = mode;
    m_request.fileId    = fileId;
    m_request.filename  = filename;
    m_request.cb        = cb;
    m_request.cbParam   = cbParam;

    bool bContinueCallerDraw = true;
    pCaller->Call(PROCLABEL_SEQ_MAKEFILE, &m_request, bContinueCallerDraw);
};


/*static*/ void CMakeFileSequence::ExtractRequest(CSequence* pCaller, int32 fileId, const char* filename)
{
    MakeRequest(pCaller, MODE_EXTRACT, fileId, filename, nullptr, nullptr);
};


/*static*/ void CMakeFileSequence::PrintRequest(CSequence* pCaller, int32 fileId)
{
    MakeRequest(pCaller, MODE_PRINT, fileId, nullptr, nullptr, nullptr);
};


/*static*/ void CMakeFileSequence::EnumRequest(CSequence* pCaller, int32 fileId, LPAC_ENUM_CB cb, void* cbParam)
{
    MakeRequest(pCaller, MODE_ENUM, fileId, nullptr, cb, cbParam);
};


/*static*/ void CMakeFileSequence::LoadInterceptCallback(void* pBufferOrg, uint32 uBufferSize, void* pParam)
{
    ASSERT(pBufferOrg);
    ASSERT(uBufferSize > 0);
    ASSERT(pParam);

    CMakeFileSequence* pSequence = static_cast<CMakeFileSequence*>(pParam);

    pSequence->PreEval();
    pSequence->Eval(pBufferOrg, uBufferSize);
    pSequence->PostEval();
};


CMakeFileSequence::CMakeFileSequence(void)
: m_aFileNode()
, m_listFileNodeFree()
, m_listFileNodeAlloc()
, m_pFileNodeCurrent(nullptr)
, m_bComplete(false)
, m_readerCurr(TYPEDEF::UINT32_MAX)
, m_nextReaderMask(0)
, m_lastTexDict()
{
    m_lastTexDict.header = nullptr;
    m_lastTexDict.buffer = nullptr;

    for (int32 i = 0; i < COUNT_OF(m_aFileNode); ++i)
        m_listFileNodeFree.push_back(&m_aFileNode[i]);
};


CMakeFileSequence::~CMakeFileSequence(void)
{
    ;
};


bool CMakeFileSequence::OnAttach(const void* pParam)
{
    CDataLoader::RegistIntercept(&LoadInterceptCallback, this);
    
	if (m_request.fileId != -1)
	{
		char szFilename[FILETYPES::FILE_NAME_MAX];
		szFilename[0] = '\0';

		CFilename::Filename(szFilename, m_request.fileId);

		if (std::strstr(szFilename, ".lpac"))
			CDataLoader::Regist(m_request.fileId);
		else
			m_bComplete = true;
	}
    else
    {
        CDataLoader::Regist(m_request.filename);
    };
    
    return true;
};


void CMakeFileSequence::OnDetach(void)
{
    CDataLoader::RemoveIntercept(&LoadInterceptCallback);
};


void CMakeFileSequence::OnMove(bool bRet, const void* pReturnValue)
{
	if (m_bComplete)
	{
		m_bBusy = false;
        Ret();
	};

    if (!CDataLoader::IsLoadEnd())
        CDataLoader::Period();
};


void CMakeFileSequence::OnDraw(void) const
{
    ;
};


void CMakeFileSequence::PreEval(void)
{
    ;
};


void CMakeFileSequence::PostEval(void)
{
    auto it = m_listFileNodeAlloc.begin();
    auto itEnd = m_listFileNodeAlloc.end();
    while (it != itEnd)
    {
        FILENODE* pNode = &(*it);

        /* finalize file header and close file */
        bool bReserve = false;
        WriteFileNodeHeader(pNode, bReserve);        

        fclose(pNode->file);
        pNode->file = nullptr;

        /* move node to free list */
        it = m_listFileNodeAlloc.erase(it);
		m_listFileNodeFree.push_back(pNode);
    };

    m_bComplete = true;
};


void CMakeFileSequence::Eval(void* pBufferOrg, uint32 sizeBuffer)
{
    /* copy from DataLoaderFromBinary.cpp */

    uint8* pBuffer = static_cast<uint8*>(pBufferOrg)
                     + sizeof(CFileHeader)
                     + (CHUNK_ALIGNMENT_SIZE - (sizeof(CFileHeader) % CHUNK_ALIGNMENT_SIZE));

    uint32 uChunkNum = reinterpret_cast<CFileHeader*>(pBufferOrg)->m_uChunkNum;
    for (uint32 i = 0; i < uChunkNum; ++i)
    {
        CChunkHeader chunkHeader;
        chunkHeader.m_pHeader = reinterpret_cast<CChunkHeader::HEADER*>(pBuffer);
        chunkHeader.m_pExtraHeader = reinterpret_cast<CChunkHeader::HEADER*>(pBuffer + sizeof(CChunkHeader::HEADER));

        pBuffer += (sizeof(CChunkHeader::HEADER) * 2);

        switch (m_request.mode)
        {
        case MODE_EXTRACT:
            ReadChunk(&chunkHeader, pBuffer);
            break;

        case MODE_PRINT:
            OUTPUT("%02" PRIi32 " -- %s\n", chunkHeader.m_pHeader->m_uType, chunkHeader.m_pHeader->m_szName);
            break;

        case MODE_ENUM:
            m_request.cb(m_request.fileId, chunkHeader.m_pHeader->m_uSize, chunkHeader.m_pHeader->m_szName, m_request.cbParam);
            break;

        default:
            ASSERT(false);
            break;
        };

        uint32 uChunkSize = chunkHeader.m_pHeader->m_uSize;

        if (uChunkSize % CHUNK_ALIGNMENT_SIZE)
            uChunkSize += (CHUNK_ALIGNMENT_SIZE - (uChunkSize % CHUNK_ALIGNMENT_SIZE));

        pBuffer += uChunkSize;

        ASSERT((uChunkSize % CHUNK_ALIGNMENT_SIZE) == 0);
        ASSERT((reinterpret_cast<uint32>(pBuffer) - reinterpret_cast<uint32>(pBufferOrg)) <= sizeBuffer);
        ASSERT(((reinterpret_cast<uint32>(pBuffer) - reinterpret_cast<uint32>(pBufferOrg)) % CHUNK_ALIGNMENT_SIZE) == 0);
    };
};


void CMakeFileSequence::ReadChunk(CChunkHeader* pChunkHdr, void* pBuffer)
{
    m_readerCurr = pChunkHdr->m_pHeader->m_uType;

    if (m_readerCurr == READERTYPE::ADD_TEXTURE_DICTIONARY)
    {
        m_lastTexDict.header = pChunkHdr->m_pHeader;
        m_lastTexDict.buffer = pBuffer;
    };

    bool bIsTarget = IsTarget(pChunkHdr->m_pHeader->m_szName);
    if (!bIsTarget)
    {
        if (!(m_nextReaderMask & MakeReaderMask(m_readerCurr)))
        {
            m_nextReaderMask = 0;
            return;
        };
    };

    switch (pChunkHdr->m_pHeader->m_uType)
    {
    case READERTYPE::TEXTURE_DICTIONARY:
        {
            /* character texture dict is indicator to start sequence but not continue */
            FILENODE* pNode = AllocFileNode(pChunkHdr->m_pHeader->m_szName);
            ASSERT(pNode);

            bool bReserve = true;
            WriteFileNodeHeader(pNode, bReserve);

            WriteFileNodeChunkHeader(pNode, pChunkHdr->m_pHeader);
            WriteFileNode(pNode, pBuffer, pChunkHdr->m_pHeader->m_uSize);
            WriteFileNodeChunkTrailer(pNode, pChunkHdr->m_pHeader);

            pNode->step = FILENODE::STEP_TEXTURE_DICT;
            ++pNode->chunkNum;

            m_pFileNodeCurrent = nullptr;
        }
        break;

    case READERTYPE::MODEL:
    case READERTYPE::MODEL_TOON:
        {
            /* model toon or simple mode is indicator to character sequence continue */
            FILENODE* pNode = FindFileNode(pChunkHdr->m_pHeader->m_szName);
            ASSERT(pNode);
            ASSERT(pNode->step == FILENODE::STEP_TEXTURE_DICT);
            ASSERT(m_nextReaderMask == 0);

            WriteFileNodeChunkHeader(pNode, pChunkHdr->m_pHeader);
            WriteFileNode(pNode, pBuffer, pChunkHdr->m_pHeader->m_uSize);
            WriteFileNodeChunkTrailer(pNode, pChunkHdr->m_pHeader);

            pNode->step = FILENODE::STEP_MODEL;
            ++pNode->chunkNum;

            m_pFileNodeCurrent = pNode;

            /* next reader may be motion set */
            m_nextReaderMask = MakeReaderMask(READERTYPE::MOTION_SET);
        }
        break;

    case READERTYPE::MOTION_SET:
        {
            /* motion set is indicator to character motion sequence start and continue */
            FILENODE* pNode = m_pFileNodeCurrent;
            ASSERT(pNode);
            ASSERT(pNode->step == FILENODE::STEP_MODEL);
            ASSERT(m_nextReaderMask & MakeReaderMask(READERTYPE::MOTION_SET));

            WriteFileNodeChunkHeader(pNode, pChunkHdr->m_pHeader);
            WriteFileNode(pNode, pBuffer, pChunkHdr->m_pHeader->m_uSize);
            WriteFileNodeChunkTrailer(pNode, pChunkHdr->m_pHeader);

            pNode->step = FILENODE::STEP_MOTION;
            ++pNode->chunkNum;

            /* next reader should be only motion */
            m_nextReaderMask = MakeReaderMask(READERTYPE::MOTION);
        }
        break;

    case READERTYPE::MOTION:
        {
            FILENODE* pNode = m_pFileNodeCurrent;
            ASSERT(pNode);
            ASSERT((pNode->step == FILENODE::STEP_MODEL) ||
                   (pNode->step == FILENODE::STEP_MOTION));
            ASSERT(m_nextReaderMask & MakeReaderMask(READERTYPE::MOTION));

            WriteFileNodeChunkHeader(pNode, pChunkHdr->m_pHeader);
            WriteFileNode(pNode, pBuffer, pChunkHdr->m_pHeader->m_uSize);
            WriteFileNodeChunkTrailer(pNode, pChunkHdr->m_pHeader);

            pNode->step = FILENODE::STEP_MOTION;
            ++pNode->chunkNum;

            /* next reader may be motion (continue) or motion parameter (end) */
            m_nextReaderMask = MakeReaderMask(READERTYPE::MOTION)
                             | MakeReaderMask(READERTYPE::MOTION_PARAMETER);
        }
        break;

    case READERTYPE::MOTION_PARAMETER:
        {
            /* motion parameter is indicator to character motion sequence end */
            FILENODE* pNode = m_pFileNodeCurrent;
            ASSERT(pNode);
            ASSERT(pNode->step == FILENODE::STEP_MOTION);
            ASSERT(m_nextReaderMask & MakeReaderMask(READERTYPE::MOTION_PARAMETER));

            WriteFileNodeChunkHeader(pNode, pChunkHdr->m_pHeader);
            WriteFileNode(pNode, pBuffer, pChunkHdr->m_pHeader->m_uSize);
            WriteFileNodeChunkTrailer(pNode, pChunkHdr->m_pHeader);

            pNode->step = FILENODE::STEP_MOTION_PARAM;
            ++pNode->chunkNum;

            m_nextReaderMask = 0;
        }
        break;
        
    case READERTYPE::EFFECT:
        {
            FILENODE* pNode = AllocFileNode(pChunkHdr->m_pHeader->m_szName);
            ASSERT(pNode);

            /* reserve lpac header */
            bool bReserve = true;
            WriteFileNodeHeader(pNode, bReserve);

            /* write add effect tex dict */
			if (m_lastTexDict.header)
			{
				WriteFileNodeChunkHeader(pNode, m_lastTexDict.header);
				WriteFileNode(pNode, m_lastTexDict.buffer, m_lastTexDict.header->m_uSize);
				WriteFileNodeChunkTrailer(pNode, m_lastTexDict.header);

				++pNode->chunkNum;
			};

            /* write effect */
            WriteFileNodeChunkHeader(pNode, pChunkHdr->m_pHeader);
            WriteFileNode(pNode, pBuffer, pChunkHdr->m_pHeader->m_uSize);
            WriteFileNodeChunkTrailer(pNode, pChunkHdr->m_pHeader);

            ++pNode->chunkNum;

            m_nextReaderMask = 0;
        }
        break;

    default:
        break;
    };
};


bool CMakeFileSequence::IsTarget(const char* pszFilename) const
{
	ASSERT(m_request.filename != nullptr);

    return (!std::strcmp(m_request.filename, pszFilename));
};


CMakeFileSequence::FILENODE* CMakeFileSequence::AllocFileNode(const char* pszName)
{
    if (m_listFileNodeFree.empty())
        return nullptr;

    FILENODE* pNode = m_listFileNodeFree.front();
    m_listFileNodeFree.erase(pNode);
    m_listFileNodeAlloc.push_back(pNode);

    pNode->step = FILENODE::STEP_TEXTURE_DICT;
    pNode->chunkNum = 0;

    ASSERT(std::strlen(pszName) < sizeof(pNode->name));
    std::strcpy(pNode->name, pszName);

    char szFilepath[256];
    szFilepath[0] = '\0';

    //std::strcpy(szFilepath, "extract/");
    //std::strcat(szFilepath, pNode->name);
	std::strcpy(szFilepath, pNode->name);

    pNode->file = fopen(szFilepath, "wb");
    ASSERT(pNode->file != nullptr);

    return pNode;
};


CMakeFileSequence::FILENODE* CMakeFileSequence::FindFileNode(const char* pszName)
{
    for (FILENODE& it : m_listFileNodeAlloc)
    {
        if (!std::strcmp(it.name, pszName))
            return &it;
    };

    return nullptr;
};


void CMakeFileSequence::WriteFileNodeHeader(FILENODE* pNode, bool bReserve)
{
    ASSERT(pNode->file != nullptr);

    if (bReserve)
    {
        /* reserve space at beginning of the file for header - it will be REwritten at the end */
        static char reserve[sizeof(CFileHeader) + (CHUNK_ALIGNMENT_SIZE - (sizeof(CFileHeader) % CHUNK_ALIGNMENT_SIZE))];

        long filePos = ftell(pNode->file);
        ASSERT(filePos == 0, "reserving file header should be performed only at beginning of the file!!");

        WriteFileNode(pNode, reserve, sizeof(reserve));

        return;
    };

    /* seek to beginning of the file and REwrite header */
    fseek(pNode->file, SEEK_SET, 0);

    char magic[] = { 'L', 'P', 'A', 'C' };
    static_assert(sizeof(magic) == sizeof(CFileHeader::m_uMagic), "size should match");

    CFileHeader header;
    header.m_uMagic     = *reinterpret_cast<uint32*>(magic);
    header.m_uChunkNum  = pNode->chunkNum;

    WriteFileNode(pNode, &header, sizeof(header));
};


void CMakeFileSequence::WriteFileNodeChunkHeader(FILENODE* pNode, CChunkHeader::HEADER* pHeader)
{
    WriteFileNode(pNode, pHeader, sizeof(CChunkHeader::HEADER) * 2); // write main and extra header
};


void CMakeFileSequence::WriteFileNodeChunkTrailer(FILENODE* pNode, CChunkHeader::HEADER* pHeader)
{
    /* trailer is empty space with size to fit chunk alignment size */
    static char trailer[CHUNK_ALIGNMENT_SIZE];

    uint32 uChunkSize = pHeader->m_uSize;

    if (uChunkSize % CHUNK_ALIGNMENT_SIZE)
        uChunkSize += (CHUNK_ALIGNMENT_SIZE - (uChunkSize % CHUNK_ALIGNMENT_SIZE));

    uint32 cbWrite = uChunkSize - pHeader->m_uSize;
    ASSERT(cbWrite >= 0);
    ASSERT(cbWrite < sizeof(trailer));

    WriteFileNode(pNode, trailer, cbWrite);
};


void CMakeFileSequence::WriteFileNode(FILENODE* pNode, const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pNode->file != nullptr);

    fwrite(pBuffer, static_cast<size_t>(sizeof(uint8)), static_cast<size_t>(uBufferSize), pNode->file);
};


uint32 CMakeFileSequence::MakeReaderMask(int32 readerType) const
{
    ASSERT(readerType >= 0);
    ASSERT(readerType < (sizeof(uint32) << 3));

    uint32 readerMask = (1 << readerType);
    return readerMask;
};


void ExtractLPACFile(class CSequence* pCaller, int32 fileId, const char* pszFilename)
{
    CMakeFileSequence::ExtractRequest(pCaller, fileId, pszFilename);
};


void PrintLPACFile(class CSequence* pCaller, int32 fileId)
{
    CMakeFileSequence::PrintRequest(pCaller, fileId);
};


void EnumLPACFile(class CSequence* pCaller, int32 fileId, LPAC_ENUM_CB cb, void* pParam /*= nullptr*/)
{
    CMakeFileSequence::EnumRequest(pCaller, fileId, cb, pParam);
};


class CProcess* MakeFileSequenceInstance(void)
{
    return new CMakeFileSequence;
};


#endif /* _DEBUG */