#include "PCSaveLoadManager.hpp"
#include "PCSaveLoadFlow.hpp"

#include "System/Common/SaveLoad/SaveLoadData.hpp"
#include "System/Common/SaveLoad/SaveLoadFrame.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/Screen.hpp"
#include "System/PC/File/PCFileStd.hpp"


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


/*static*/ void CPCSaveLoadManager::SetMessage(MESSAGEID id)
{    
    Frame().SetMessage(GetMessage(id));
};


/*static*/ void CPCSaveLoadManager::SetTitle(TITLEID id)
{
    Frame().SetTitle(GetTitle(id));
};


/*static*/ const wchar* CPCSaveLoadManager::GetMessage(MESSAGEID id)
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

    return CSystemText::GetText(SYSTEXT::VALUE(s_aSystemMessageTable[id]));
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

    return CSystemText::GetText(SYSTEXT::VALUE(s_aSystemTitleTable[id]));
};


/*static*/ void CPCSaveLoadManager::InitializeFrame(MESSAGEID msgid, TITLEID titleid)
{
    Frame().Start(GetMessage(msgid), GetTitle(titleid));
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
    CPCSaveLoadManager::SetMessage(msgid);
    CPCSaveLoadManager::Frame().SetModeSelect(
        CSaveLoadFrameBase::SELECTITEM_YES
    );
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
    CPCSaveLoadManager::SetMessage(msgid);
    CPCSaveLoadManager::Frame().SetModeError(
        CSaveLoadFrameBase::PADBTN_OK,
        2.0f
    );
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


bool CPCSaveLoadManagerBase::LoadFile(void)
{
    bool bResult = false;

    void* hFile = stdf_open(FILENAME, "rb");
    if (hFile)
    {
        uint32 uFileSize = 0;
        void* pFileData = nullptr;

        stdf_seek(hFile, 0, stdf_seek_end);
        uFileSize = stdf_tell(hFile);
        stdf_seek(hFile, 0, stdf_seek_begin);

        if (uFileSize)
        {
            pFileData = new char[uFileSize];
            ASSERT(pFileData);

            if (pFileData)
            {
                uint32 uReaded = stdf_read(hFile, pFileData, uFileSize);
                ASSERT(uReaded > 0);

                CPCSaveLoadManager::Data().Initialize(pFileData, uFileSize);
                
                bResult = true;
            };

            delete [] pFileData;
            pFileData = nullptr;
        };

        stdf_close(hFile);
        hFile = nullptr;
    };

    return bResult;
};


bool CPCSaveLoadManagerBase::SaveFile(void)
{
    void* hFile = stdf_open(FILENAME, "wb");
    if (hFile)
    {
        CPCSaveLoadManager::Data().Initialize();
        
        uint32 uWritten = stdf_write(
            hFile,
            CPCSaveLoadManager::Data().GetData(),
            CPCSaveLoadManager::Data().GetSize()
        );

        ASSERT(uWritten > 0);

        stdf_close(hFile);
        return true;
    };

    return false;
};


bool CPCSaveLoadManagerBase::IsExistFile(void)
{
    void* hFile = stdf_open(FILENAME, "rb");
    if (hFile)
    {
        stdf_close(hFile);
        return true;
    };

    return false;
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


CPCLoadManager::CPCLoadManager(void)
: m_status(STATUS_OK)
{
    m_step = STEP_INTRO;
    CPCSaveLoadManager::SetMessage(
        CPCSaveLoadManager::MESSAGEID_LOAD_CHECK
    );
};


CPCLoadManager::~CPCLoadManager(void)
{
    ;
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
        if (IsExistFile())
        {
            CSaveLoadFlow::m_bNewSave = false;
            if (LoadFile())
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
            CPCSaveLoadManager::SetMessage(CPCSaveLoadManager::MESSAGEID_LOAD_NOW);
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


CPCSaveManager::CPCSaveManager(void)
{
    m_step = STEP_INTRO;
    CPCSaveLoadManager::SetMessage(
        CPCSaveLoadManager::MESSAGEID_SAVE_NOW
    );
};


CPCSaveManager::~CPCSaveManager(void)
{
    ;
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
        if (SaveFile())
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