#include "WebSaveLoadManager.hpp"
#include "WebSaveLoadFlow.hpp"

#include "System/Common/SaveLoad/SaveLoadData.hpp"
#include "System/Common/SaveLoad/SaveLoadFrame.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/SystemText.hpp"
#include "System/Common/TextData.hpp"

#include "rtfsyst.h"


static CSaveLoadFrameBase* s_pSaveloadFrame = nullptr;
static CSaveLoadDataBase* s_pSaveloadData = nullptr;


/*static*/ void CWebSaveLoadManager::Initialize(CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData)
{
    ASSERT(!s_pSaveloadFrame);
    ASSERT(!s_pSaveloadData);

    s_pSaveloadFrame = pFrame;
    s_pSaveloadData = pData;

    ASSERT(s_pSaveloadFrame);
    ASSERT(s_pSaveloadData);
};


/*static*/ void CWebSaveLoadManager::Terminate(void)
{
    ASSERT(s_pSaveloadFrame);
    ASSERT(s_pSaveloadData);

    s_pSaveloadFrame = nullptr;
    s_pSaveloadData = nullptr;
};


/*static*/ void CWebSaveLoadManager::SetMsg(MESSAGEID id)
{    
    Frame().SetMessage(GetMsg(id));
};


/*static*/ void CWebSaveLoadManager::SetTitle(TITLEID id)
{
    Frame().SetTitle(GetTitle(id));
};


/*static*/ const wchar* CWebSaveLoadManager::GetMsg(MESSAGEID id)
{
    static const SYSTEXT s_aSystextMsgTable[] =
    {
        SYSTEXT_SAVE_SURE_OVERW,    //  MESSAGEID_OVERWRITE_SURE
        SYSTEXT_WAIT,               //  MESSAGEID_WAIT
        SYSTEXT_SAVE_SURE,          //  MESSAGEID_SAVE_SURE
        SYSTEXT_SAVE_NOW,           //  MESSAGEID_SAVE_NOW
        SYSTEXT_SAVE_OK,            //  MESSAGEID_SAVE_OK
        SYSTEXT_SAVE_FAIL,          //  MESSAGEID_SAVE_FAIL
        SYSTEXT_LOAD_SURE,          //  MESSAGEID_LOAD_SURE
        SYSTEXT_LOAD_CHECK,         //  MESSAGEID_LOAD_CHECK
        SYSTEXT_LOAD_EMPTY,         //  MESSAGEID_LOAD_EMPTY
        SYSTEXT_LOAD_NOW,           //  MESSAGEID_LOAD_NOW
        SYSTEXT_LOAD_OK,            //  MESSAGEID_LOAD_OK
        SYSTEXT_LOAD_FAIL,          //  MESSAGEID_LOAD_FAIL
        SYSTEXT_LOAD_INVALID,       //  MESSAGEID_LOAD_INVALID
    };

    static_assert(COUNT_OF(s_aSystextMsgTable) == MESSAGEIDMAX, "update id table");

    ASSERT(id >= 0);
    ASSERT(id < MESSAGEIDMAX);

    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(s_aSystextMsgTable));

    static wchar s_wszTextBuff[1024];
    s_wszTextBuff[0] = UCHAR('\0');

    const wchar* pwszString = CSystemText::GetText(s_aSystextMsgTable[id]);
    CTextData::StrCpy(s_wszTextBuff, pwszString);

    if ((id == MESSAGEID_SAVE_NOW) ||
        (id == MESSAGEID_LOAD_NOW))
    {
        const wchar* pwszPleaseWait = CSystemText::GetText(s_aSystextMsgTable[MESSAGEID_WAIT]);
        CTextData::StrCat(s_wszTextBuff, pwszPleaseWait);
    };

    return s_wszTextBuff;
};


/*static*/ const wchar* CWebSaveLoadManager::GetTitle(TITLEID id)
{
    static const SYSTEXT s_aSystextTitleTable[] =
    {
        SYSTEXT_TIT_CHECK,  //  TITLEID_CHECK
        SYSTEXT_TIT_SAVE,   //  TITLEID_SAVE
        SYSTEXT_TIT_LOAD,   //  TITLEID_LOAD
    };

    static_assert(COUNT_OF(s_aSystextTitleTable) == TITLEIDMAX, "update id table");

    ASSERT(id >= 0);
    ASSERT(id < TITLEIDMAX);
    
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(s_aSystextTitleTable));

    return CSystemText::GetText(s_aSystextTitleTable[id]);
};


/*static*/ void CWebSaveLoadManager::InitializeFrame(MESSAGEID msgid, TITLEID titleid)
{
    Frame().Start(GetMsg(msgid), GetTitle(titleid));
    Frame().SetModeMessage();
};


/*static*/ void CWebSaveLoadManager::TerminateFrame(void)
{
    Frame().Stop();
};


/*static*/ CSaveLoadDataBase& CWebSaveLoadManager::Data(void)
{
    ASSERT(s_pSaveloadData);

    return *s_pSaveloadData;
};


/*static*/ CSaveLoadFrameBase& CWebSaveLoadManager::Frame(void)
{
    ASSERT(s_pSaveloadFrame);
    
    return *s_pSaveloadFrame;
};


//
// *********************************************************************************
//


CWebQuestManager::CWebQuestManager(CWebSaveLoadManager::MESSAGEID msgid)
{
    CWebSaveLoadManager::SetMsg(msgid);
    CWebSaveLoadManager::Frame().SetModeSelect(CSaveLoadFrameBase::SELECTITEM_YES);
};


CWebQuestManager::~CWebQuestManager(void)
{
    CWebSaveLoadManager::Frame().SetModeMessage();
};


CWebQuestManager::RESULT CWebQuestManager::Proc(void)
{
    if (!CWebSaveLoadManager::Frame().IsSelected())
        return RESULT_NONE;

    switch (CWebSaveLoadManager::Frame().GetSelectItem())
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


//
// *********************************************************************************
//


CWebWarningManager::CWebWarningManager(CWebSaveLoadManager::MESSAGEID msgid)
{
    CWebSaveLoadManager::SetMsg(msgid);
    CWebSaveLoadManager::Frame().SetModeError(CSaveLoadFrameBase::PADBTN_OK, 2.0f);
};


CWebWarningManager::~CWebWarningManager(void)
{
    CWebSaveLoadManager::Frame().SetModeMessage();
};


bool CWebWarningManager::Proc(void)
{
    return CWebSaveLoadManager::Frame().IsErrorConfirmed();
};


//
// *********************************************************************************
//


/*static*/ const char* CWebSaveLoadManagerBase::FILENAME = "tmntsave.dat";


CWebSaveLoadManagerBase::CWebSaveLoadManagerBase(void)
: m_pWarning(nullptr)
, m_pQuest(nullptr)
, m_msgid(CWebSaveLoadManager::MESSAGEIDMAX)
, m_step(-1)
, m_substep(-1)
, m_fTimer(0.0f)
{
    ;
};


CWebSaveLoadManagerBase::~CWebSaveLoadManagerBase(void)
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


void CWebSaveLoadManagerBase::SetStep(int32 step)
{
    m_step = step;
};


void CWebSaveLoadManagerBase::SetSubStep(int32 substep)
{
    m_substep = substep;
};


void CWebSaveLoadManagerBase::SetMessage(CWebSaveLoadManager::MESSAGEID msgid)
{
    m_msgid = msgid;
};


void CWebSaveLoadManagerBase::SyncTime(void)
{
    m_fTimer = CScreen::TimeElapsed();
};


void CWebSaveLoadManagerBase::MakeFilePath(char* pszFilepathBuff) const
{
    std::strcpy(pszFilepathBuff, "ifs:");
    std::strcat(pszFilepathBuff, FILENAME);
};


bool CWebSaveLoadManagerBase::CheckFileExist(void) const
{
    char szFilepath[256];
    szFilepath[0] = '\0';

    MakeFilePath(szFilepath);

    return RwFexist(szFilepath);
};


//
// *********************************************************************************
//


CWebLoadManager::CWebLoadManager(void)
: m_status(STATUS_OK)
{
    m_step = STEP_INTRO;
    CWebSaveLoadManager::SetMsg(CWebSaveLoadManager::MESSAGEID_LOAD_CHECK);
};


bool CWebLoadManager::Proc(void)
{
    bool bResult = false;
    
    switch (m_step)
    {
    case STEP_INTRO:
        {
            if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
                return false;

            m_step = STEP_READ;
            SyncTime();
        }
        break;
        
    case STEP_READ:
        {
            m_step = STEP_DISP_CHECK;
            if (CheckFileExist())
            {
                CSaveLoadFlow::m_bNewSave = false;
                if (FileLoad())
                {
                    if (CWebSaveLoadManager::Data().IsValid())
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
        }
        break;
        
    case STEP_DISP_CHECK:
        {
            if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
                return false;

            SyncTime();

            if (m_status == STATUS_OPEN_FAIL)
            {
                m_step = STEP_DISP_WARN;

                SetMessage(CWebSaveLoadManager::MESSAGEID_LOAD_EMPTY);

                m_pWarning = new CWebWarningManager(m_msgid);
            }
            else
            {
                m_step = STEP_DISP_LOAD;
                CWebSaveLoadManager::SetTitle(CWebSaveLoadManager::TITLEID_LOAD);
                CWebSaveLoadManager::SetMsg(CWebSaveLoadManager::MESSAGEID_LOAD_NOW);
            };
        }
        break;
        
    case STEP_DISP_LOAD:
        {
            if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
                return false;

            SyncTime();
            m_step = STEP_DISP_WARN;

            switch (m_status)
            {
            case STATUS_OK:
                CWebSaveLoadManager::Data().Update();
                m_msgid = CWebSaveLoadManager::MESSAGEID_LOAD_OK;
                break;

            case STATUS_READ_FAIL:
                m_msgid = CWebSaveLoadManager::MESSAGEID_LOAD_FAIL;
                break;

            case STATUS_DATA_INVALID:
                m_msgid = CWebSaveLoadManager::MESSAGEID_LOAD_INVALID;
                break;

            default:
                ASSERT(false);
                break;
            };

            m_pWarning = new CWebWarningManager(m_msgid);
        }
        break;
        
    case STEP_DISP_WARN:
        {
            ASSERT(m_pWarning);
            if (m_pWarning->Proc())
            {
                m_step = STEP_EOL;

                delete m_pWarning;
                m_pWarning = nullptr;
            };
        }
        break;

    case STEP_EOL:
        {
            CWebSaveLoadManager::Data().Terminate();
            bResult = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


bool CWebLoadManager::FileLoad(void) const
{
    bool bResult = false;

    char szFilepath[256];
    szFilepath[0] = '\0';

    MakeFilePath(szFilepath);

    void* fptr = RwFopen(szFilepath, "r");
    if (fptr)
    {
        RwFseek(fptr, 0, SEEK_END);
        uint32 fsize = static_cast<uint32>(RwFtell(fptr));
        RwFseek(fptr, 0, SEEK_SET);

        if (fsize)
        {
            uint8* fdata = new uint8[fsize];
            if (fdata)
            {
                uint32 cbread = static_cast<uint32>(RwFread(fdata, static_cast<size_t>(fsize), 1, fptr));
                if (cbread == 1)
                {
                    CWebSaveLoadManager::Data().Initialize(fdata, fsize);
                    bResult = true;
                };
            };

            delete[] fdata;
        };

        RwFclose(fptr);
    };

    return bResult;
};


//
// *********************************************************************************
//


CWebSaveManager::CWebSaveManager(void)
{
    m_step = STEP_INTRO;
    CWebSaveLoadManager::SetMsg(CWebSaveLoadManager::MESSAGEID_SAVE_NOW);
};


bool CWebSaveManager::Proc(void)
{
    bool bResult = false;

    switch (m_step)
    {
    case STEP_INTRO:
        {
            if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
                return false;

            m_step = STEP_WRITE;
            SyncTime();
        }
        break;

    case STEP_WRITE:
        {
            m_step = STEP_DISP_WRITE;
            if (FileSave())
                m_msgid = CWebSaveLoadManager::MESSAGEID_SAVE_OK;
            else
                m_msgid = CWebSaveLoadManager::MESSAGEID_SAVE_FAIL;
        }
        break;

    case STEP_DISP_WRITE:
        {
            if (CScreen::TimeElapsed() - m_fTimer <= 1.0f)
                return false;

            SyncTime();
            m_step = STEP_DISP_WARN;

            ASSERT(!m_pWarning);
            m_pWarning = new CWebWarningManager(m_msgid);
        }
        break;

    case STEP_DISP_WARN:
        {
            ASSERT(m_pWarning);
            if (m_pWarning->Proc())
            {
                m_step = STEP_EOL;

                delete m_pWarning;
                m_pWarning = nullptr;
            };
        }
        break;

    case STEP_EOL:
        {
            CWebSaveLoadManager::Data().Terminate();
            bResult = true;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


bool CWebSaveManager::FileSave(void) const
{
    char szFilepath[256];
    szFilepath[0] = '\0';

    MakeFilePath(szFilepath);

    void* fptr = RwFopen(szFilepath, "w+");
    if (fptr)
    {
        CWebSaveLoadManager::Data().Initialize();

        void* data = CWebSaveLoadManager::Data().GetData();
        uint32 len = CWebSaveLoadManager::Data().GetSize();

        size_t written = RwFwrite(data, static_cast<size_t>(len), 1, fptr);

        RwFclose(fptr);

        return (written == 1);
    };

    return false;
};