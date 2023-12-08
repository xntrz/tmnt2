#include "PCSaveLoadManager.hpp"
#include "PCSaveLoadFlow.hpp"

#include "../PCTypedefs.hpp"

#include "System/Common/SaveLoad/SaveLoadData.hpp"
#include "System/Common/SaveLoad/SaveLoadFrame.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/Screen.hpp"


static CSaveLoadFrameBase* s_pSaveloadFrame = nullptr;
static CSaveLoadDataBase* s_pSaveloadData = nullptr;


/*static*/ void CPCSaveLoadManager::Initialize(CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
{
    ASSERT(!s_pSaveloadFrame);
    ASSERT(!s_pSaveloadData);

    s_pSaveloadFrame = pFrame;
    s_pSaveloadData = pData;

    ASSERT(s_pSaveloadFrame);
    ASSERT(s_pSaveloadData);
};


/*static*/ void CPCSaveLoadManager::Terminate(void)
{
    ASSERT(s_pSaveloadFrame);
    ASSERT(s_pSaveloadData);

    s_pSaveloadFrame = nullptr;
    s_pSaveloadData = nullptr;
};


/*static*/ void CPCSaveLoadManager::SetMsg(MESSAGEID id)
{    
    Frame().SetMessage(GetMsg(id));
};


/*static*/ void CPCSaveLoadManager::SetTitle(TITLEID id)
{
    Frame().SetTitle(GetTitle(id));
};


/*static*/ const wchar* CPCSaveLoadManager::GetMsg(MESSAGEID id)
{
    static const int32 s_aSystemMessageTable[] =
    {
        13,
        24,
        12,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
    };

    static_assert(COUNT_OF(s_aSystemMessageTable) == MESSAGEIDMAX, "update me");

    ASSERT(id >= 0 && id < MESSAGEIDMAX);
    ASSERT(id >= 0 && id < COUNT_OF(s_aSystemMessageTable));

    return CSystemText::GetText(SYSTEXT(s_aSystemMessageTable[id]));
};


/*static*/ const wchar* CPCSaveLoadManager::GetTitle(TITLEID id)
{
    static const int32 s_aSystemTitleTable[] =
    {
        9,
        10,
        11,
    };

    static_assert(COUNT_OF(s_aSystemTitleTable) == TITLEIDMAX, "update me");

    ASSERT(id >= 0 && id < TITLEIDMAX);
    ASSERT(id >= 0 && id < COUNT_OF(s_aSystemTitleTable));

    return CSystemText::GetText(SYSTEXT(s_aSystemTitleTable[id]));
};


/*static*/ void CPCSaveLoadManager::InitializeFrame(MESSAGEID msgid, TITLEID titleid)
{
    Frame().Start(GetMsg(msgid), GetTitle(titleid));
    Frame().SetModeMessage();
};


/*static*/ void CPCSaveLoadManager::TerminateFrame(void)
{
    Frame().Stop();
};


/*static*/ CSaveLoadDataBase& CPCSaveLoadManager::Data(void)
{
    ASSERT(s_pSaveloadData);

    return *s_pSaveloadData;
};


/*static*/ CSaveLoadFrameBase& CPCSaveLoadManager::Frame(void)
{
    ASSERT(s_pSaveloadFrame);
    
    return *s_pSaveloadFrame;
};


CPCQuestManager::CPCQuestManager(CPCSaveLoadManager::MESSAGEID msgid)
{
    CPCSaveLoadManager::SetMsg(msgid);
    CPCSaveLoadManager::Frame().SetModeSelect(CSaveLoadFrameBase::SELECTITEM_YES);
};


CPCQuestManager::~CPCQuestManager(void)
{
    CPCSaveLoadManager::Frame().SetModeMessage();
};


CPCQuestManager::RESULT CPCQuestManager::Proc(void)
{
    if (!CPCSaveLoadManager::Frame().IsSelected())
        return RESULT_NONE;

    switch (CPCSaveLoadManager::Frame().GetSelectItem())
    {
    case CSaveLoadFrameBase::SELECTITEM_YES:
        return RESULT_YES;

    case CSaveLoadFrameBase::SELECTITEM_NO:
        return RESULT_NO;

    default:
        ASSERT(false);
        return RESULT_NONE;
    };
};


CPCWarningManager::CPCWarningManager(CPCSaveLoadManager::MESSAGEID msgid)
{
    CPCSaveLoadManager::SetMsg(msgid);
    CPCSaveLoadManager::Frame().SetModeError(CSaveLoadFrameBase::PADBTN_OK, 2.0f);
};


CPCWarningManager::~CPCWarningManager(void)
{
    CPCSaveLoadManager::Frame().SetModeMessage();
};


bool CPCWarningManager::Proc(void)
{
    return CPCSaveLoadManager::Frame().IsErrorConfirmed();
};


/*static*/ const char* CPCSaveLoadManagerBase::FILENAME = "tmntsave.dat";


CPCSaveLoadManagerBase::CPCSaveLoadManagerBase(void)
: m_pWarning(nullptr)
, m_pQuest(nullptr)
, m_msgid(CPCSaveLoadManager::MESSAGEIDMAX)
, m_step(-1)
, m_substep(-1)
, m_fTimer(0.0f)
{
    ;
};


CPCSaveLoadManagerBase::~CPCSaveLoadManagerBase(void)
{
    ASSERT(!m_pWarning);
    ASSERT(!m_pQuest);

    if (m_pWarning)
    {
        delete m_pWarning;
        m_pWarning = nullptr;
    };

    if (m_pQuest)
    {
        delete m_pQuest;
        m_pQuest = nullptr;
    };
};


void CPCSaveLoadManagerBase::SetStep(int32 step)
{
    m_step = step;
};


void CPCSaveLoadManagerBase::SetSubStep(int32 substep)
{
    m_substep = substep;
};


void CPCSaveLoadManagerBase::SetMessage(CPCSaveLoadManager::MESSAGEID msgid)
{
    m_msgid = msgid;
};


void CPCSaveLoadManagerBase::SyncTime(void)
{
    m_fTimer = CScreen::TimeElapsed();
};


void CPCSaveLoadManagerBase::MakeFilePath(char* pszFilepathBuff) const
{
    char szMyPath[MAX_PATH];
    szMyPath[0] = '\0';

    GetModulePath(szMyPath);

    std::sprintf(pszFilepathBuff, "%s%s", szMyPath, FILENAME);
};


bool CPCSaveLoadManagerBase::CheckFileExist(void) const
{
    char szFilepath[MAX_PATH];
    szFilepath[0] = '\0';

    MakeFilePath(szFilepath);

    return (RwFexist(szFilepath) > 0);
};


CPCLoadManager::CPCLoadManager(void)
: m_status(STATUS_OK)
{
    m_step = STEP_INTRO;
    CPCSaveLoadManager::SetMsg(CPCSaveLoadManager::MESSAGEID_LOAD_CHECK);
};


bool CPCLoadManager::Proc(void)
{
    bool bResult = false;
    
    switch (m_step)
    {
    case STEP_INTRO:
        if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
            return false;
        
        m_step = STEP_READ;
        SyncTime();
        break;
        
    case STEP_READ:
        m_step = STEP_DISP_CHECK;        
        if (CheckFileExist())
        {
            CSaveLoadFlow::m_bNewSave = false;
            if (FileLoad())
            {
                if (CPCSaveLoadManager::Data().IsValid())
                    m_status = STATUS_OK;
                else
                    m_status = STATUS_DATA_INVALID;
            }
            else
            {
                m_status = STATUS_READ_FAIL;
            };
        }
        else
        {
            m_status = STATUS_OPEN_FAIL;
            CSaveLoadFlow::m_bNewSave = true;
        };
        break;
        
    case STEP_DISP_CHECK:
        if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
            return false;
        
        SyncTime();

        if (m_status == STATUS_OPEN_FAIL)
        {
            m_step = STEP_DISP_WARN;
            SetMessage(CPCSaveLoadManager::MESSAGEID_LOAD_EMPTY);
			m_pWarning = new CPCWarningManager(m_msgid);
			ASSERT(m_pWarning);
        }
        else
        {
            m_step = STEP_DISP_LOAD;
            CPCSaveLoadManager::SetTitle(CPCSaveLoadManager::TITLEID_LOAD);
            CPCSaveLoadManager::SetMsg(CPCSaveLoadManager::MESSAGEID_LOAD_NOW);
        };
        break;
        
    case STEP_DISP_LOAD:
        if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
            return false;

        SyncTime();
        m_step = STEP_DISP_WARN;

        switch (m_status)
        {
        case STATUS_OK:
            CPCSaveLoadManager::Data().Update();            
            m_msgid = CPCSaveLoadManager::MESSAGEID_LOAD_OK;
            break;
            
        case STATUS_READ_FAIL:
            m_msgid = CPCSaveLoadManager::MESSAGEID_LOAD_FAIL;
            break;

        case STATUS_DATA_INVALID:
            m_msgid = CPCSaveLoadManager::MESSAGEID_LOAD_INVALID;
            break;

        default:
            ASSERT(false);
            break;
        };

        m_pWarning = new CPCWarningManager(m_msgid);
        ASSERT(m_pWarning);
        break;
        
    case STEP_DISP_WARN:
        ASSERT(m_pWarning);
        if (m_pWarning->Proc())
        {
            m_step = STEP_EOL;
            
            delete m_pWarning;
            m_pWarning = nullptr;
        };        
        break;

    case STEP_EOL:
		CPCSaveLoadManager::Data().Terminate();
        bResult = true;
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


bool CPCLoadManager::FileLoad(void) const
{
	bool bResult = false;

    char szFilepath[MAX_PATH];
    szFilepath[0] = '\0';

    MakeFilePath(szFilepath);

    void* hFile = RwFopen(szFilepath, "rb");
	if (!hFile)
		return bResult;

    uint32 uReaded = 0;
    uint32 uFileSize = 0;
    void* pFileData = nullptr;

    RwFseek(hFile, 0, SEEK_END);
	uFileSize = uint32(RwFtell(hFile));
    RwFseek(hFile, 0, SEEK_SET);

    if (uFileSize)
    {
        pFileData = new char[uFileSize];    
        if (pFileData)
        {
            uReaded = RwFread(pFileData, sizeof(uint8), uFileSize, hFile);
			if (uReaded == uFileSize)
			{
                CPCSaveLoadManager::Data().Initialize(pFileData, uFileSize);
				bResult = true;
			};
        };

        delete[] pFileData;
        pFileData = nullptr;
    };

    RwFclose(hFile);
    hFile = nullptr;

	return bResult;
};


CPCSaveManager::CPCSaveManager(void)
{
    m_step = STEP_INTRO;
    CPCSaveLoadManager::SetMsg(CPCSaveLoadManager::MESSAGEID_SAVE_NOW);
};


bool CPCSaveManager::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_INTRO:
        if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
            return false;

        m_step = STEP_WRITE;
        SyncTime();
        break;
        
    case STEP_WRITE:
        m_step = STEP_DISP_WRITE;
        if (FileSave())
            m_msgid = CPCSaveLoadManager::MESSAGEID_SAVE_OK;
        else
            m_msgid = CPCSaveLoadManager::MESSAGEID_SAVE_FAIL;      
        break;
        
    case STEP_DISP_WRITE:
        if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
            return false;

        SyncTime();
        m_step = STEP_DISP_WARN;
        ASSERT(!m_pWarning);
        m_pWarning = new CPCWarningManager(m_msgid);
        ASSERT(m_pWarning);
        break;
        
    case STEP_DISP_WARN:
        ASSERT(m_pWarning);
        if (m_pWarning->Proc())
        {
            m_step = STEP_EOL;
            
            delete m_pWarning;
            m_pWarning = nullptr;
        };
        break;
        
    case STEP_EOL:
        CPCSaveLoadManager::Data().Terminate();
        bResult = true;
        break;
        
    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


bool CPCSaveManager::FileSave(void) const
{
    char szFilepath[MAX_PATH];
    szFilepath[0] = '\0';

    MakeFilePath(szFilepath);

    void* hFile = RwFopen(szFilepath, "wb");
    if (!hFile)
        return false;

    CPCSaveLoadManager::Data().Initialize();

    uint32 uWritten = RwFwrite(
        CPCSaveLoadManager::Data().GetData(),
        CPCSaveLoadManager::Data().GetSize(),
        sizeof(uint8),
        hFile
    );

    RwFclose(hFile);
    hFile = nullptr;

    return (uWritten > 0);
};