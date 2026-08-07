#pragma once


class CSaveLoadFrameBase;
class CSaveLoadDataBase;


class CWebSaveLoadManager
{
public:
    enum MESSAGEID
    {
        MESSAGEID_OVERWRITE_SURE = 0,
        MESSAGEID_WAIT,
        MESSAGEID_SAVE_SURE,
        MESSAGEID_SAVE_NOW,
        MESSAGEID_SAVE_OK,
        MESSAGEID_SAVE_FAIL,
        MESSAGEID_LOAD_SURE,
        MESSAGEID_LOAD_CHECK,
        MESSAGEID_LOAD_EMPTY,
        MESSAGEID_LOAD_NOW,
        MESSAGEID_LOAD_OK,
        MESSAGEID_LOAD_FAIL,
        MESSAGEID_LOAD_INVALID,

        MESSAGEIDMAX,
    };

    enum TITLEID
    {
        TITLEID_CHECK = 0,
        TITLEID_SAVE,
        TITLEID_LOAD,

        TITLEIDMAX,
    };
    
public:
    static void Initialize(CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData);
    static void Terminate(void);
    static void SetMsg(MESSAGEID id);
    static void SetTitle(TITLEID id);
    static const wchar* GetMsg(MESSAGEID id);
    static const wchar* GetTitle(TITLEID id);
    static void InitializeFrame(MESSAGEID msgid, TITLEID titleid);
    static void TerminateFrame(void);
    static CSaveLoadDataBase& Data(void);
    static CSaveLoadFrameBase& Frame(void);
};


class CWebQuestManager
{
public:
    enum RESULT
    {
        RESULT_NONE = 0,
        RESULT_NO,
        RESULT_YES,
    };

public:
    CWebQuestManager(CWebSaveLoadManager::MESSAGEID msgid);
    virtual ~CWebQuestManager(void);
    virtual RESULT Proc(void);
};


class CWebWarningManager
{
public:
    CWebWarningManager(CWebSaveLoadManager::MESSAGEID msgid);
    virtual ~CWebWarningManager(void);
    virtual bool Proc(void);
};


class CWebSaveLoadManagerBase
{
protected:
    static const char* FILENAME;
    
public:
    CWebSaveLoadManagerBase(void);
    virtual ~CWebSaveLoadManagerBase(void);
    virtual bool Proc(void) = 0;
    void SetStep(int32 step);
    void SetSubStep(int32 substep);
    void SetMessage(CWebSaveLoadManager::MESSAGEID msgid);
    void SyncTime(void);
    void MakeFilePath(char* pszFilepathBuff) const;
    bool CheckFileExist(void) const;
    
protected:
    CWebWarningManager* m_pWarning;
    CWebQuestManager* m_pQuest;
    CWebSaveLoadManager::MESSAGEID m_msgid;
    int32 m_step;
    int32 m_substep;
    float m_fTimer;
};


class CWebLoadManager : public CWebSaveLoadManagerBase
{
protected:
    enum STEP
    {
        STEP_INTRO = 0,
        STEP_READ,
        STEP_DISP_CHECK,
        STEP_DISP_LOAD,
        STEP_DISP_WARN,
        STEP_EOL,
    };

    enum STATUS
    {
        STATUS_OK = 0,
        STATUS_OPEN_FAIL,
        STATUS_READ_FAIL,
        STATUS_DATA_INVALID,
    };

public:
    CWebLoadManager(void);
    virtual ~CWebLoadManager(void) {};
    virtual bool Proc(void);
    bool FileLoad(void) const;

protected:
    STATUS m_status;
};


class CWebSaveManager : public CWebSaveLoadManagerBase
{
protected:
    enum STEP
    {
        STEP_INTRO = 0,
        STEP_WRITE,
        STEP_DISP_WRITE,
        STEP_DISP_WARN,
        STEP_EOL,
    };

public:
    CWebSaveManager(void);
    virtual ~CWebSaveManager(void) {};
    virtual bool Proc(void);
    bool FileSave(void) const;
};