#pragma once


class CSaveLoadFrameBase;
class CSaveLoadDataBase;


class CPCSaveLoadManager
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
    static void SetMessage(MESSAGEID id);
    static void SetTitle(TITLEID id);
    static const wchar* GetMessage(MESSAGEID id);
    static const wchar* GetTitle(TITLEID id);
    static void InitializeFrame(MESSAGEID msgid, TITLEID titleid);
    static void TerminateFrame(void);
    static CSaveLoadDataBase& Data(void);
    static CSaveLoadFrameBase& Frame(void);
};


class CPCQuestManager
{
public:
    enum RESULT
    {
        RESULT_NONE = 0,
        RESULT_NO,
        RESULT_YES,
    };

public:
    CPCQuestManager(CPCSaveLoadManager::MESSAGEID msgid);
    virtual ~CPCQuestManager(void);
    virtual RESULT Proc(void);
};


class CPCWarningManager
{
public:
    CPCWarningManager(CPCSaveLoadManager::MESSAGEID msgid);
    virtual ~CPCWarningManager(void);
    virtual bool Proc(void);
};


class CPCSaveLoadManagerBase
{
protected:
    static const char* FILENAME;
    
public:
    CPCSaveLoadManagerBase(void);
    virtual ~CPCSaveLoadManagerBase(void);
    virtual bool Proc(void) = 0;
    virtual bool LoadFile(void);
    virtual bool SaveFile(void);
    virtual bool IsExistFile(void);
    void SetStep(int32 step);
    void SetSubStep(int32 substep);
    void SetMessage(CPCSaveLoadManager::MESSAGEID msgid);
    void SyncTime(void);

protected:
    CPCWarningManager* m_pWarning;
    CPCQuestManager* m_pQuest;
    CPCSaveLoadManager::MESSAGEID m_msgid;
    int32 m_step;
    int32 m_substep;
    float m_fTimer;
};


class CPCLoadManager : public CPCSaveLoadManagerBase
{
protected:
    enum STEP
    {
        STEP_INTRO,
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
    CPCLoadManager(void);
    virtual ~CPCLoadManager(void);
    virtual bool Proc(void);

protected:
    STATUS m_status;
};


class CPCSaveManager : public CPCSaveLoadManagerBase
{
protected:
    enum STEP
    {
        STEP_INTRO,
        STEP_WRITE,
        STEP_DISP_WRITE,
        STEP_DISP_WARN,
        STEP_EOL,
    };

public:
    CPCSaveManager(void);
    virtual ~CPCSaveManager(void);
    virtual bool Proc(void);
};