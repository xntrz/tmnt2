#pragma once


class CPCSaveManager;
class CPCLoadManager;
class CPCQuestManager;
class CPCWarningManager;


class CPCSaveLoadBaseFlow
{
public:
    CPCSaveLoadBaseFlow(void);
    virtual ~CPCSaveLoadBaseFlow(void);
    virtual bool Proc(void) = 0;

protected:
    int32 m_step;
    static CPCSaveManager* m_pSave;
    static CPCLoadManager* m_pLoad;
    static CPCQuestManager* m_pQuest;
    static CPCWarningManager* m_pWarning;
};


class CPCStartCheckFlow : public CPCSaveLoadBaseFlow
{
protected:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_RUN,
        STEP_EOL,
    };
    
public:
    CPCStartCheckFlow(void);
    virtual ~CPCStartCheckFlow(void);
    virtual bool Proc(void) override;
};


class CPCMenuSaveFlow : public CPCSaveLoadBaseFlow
{
protected:
    enum STEP
    {
        STEP_QUEST_INIT = 0,
        STEP_QUEST,
        STEP_RUN_INIT,
        STEP_RUN,
        STEP_EOL,
    };
    
public:
    CPCMenuSaveFlow(void);
    virtual ~CPCMenuSaveFlow(void);
    virtual bool Proc(void) override;
};


class CPCMenuLoadFlow : public CPCSaveLoadBaseFlow
{
protected:
    enum STEP
    {
        STEP_QUEST_INIT = 0,
        STEP_QUEST,
        STEP_RUN_INIT,
        STEP_RUN,
        STEP_EOL,
    };
    
public:
    CPCMenuLoadFlow(void);
    virtual ~CPCMenuLoadFlow(void);
    virtual bool Proc(void) override;
};


class CPCAutosaveFlow : public CPCSaveLoadBaseFlow
{
protected:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_RUN,
        STEP_EOL,
    };
    
public:
    CPCAutosaveFlow(void);
    virtual ~CPCAutosaveFlow(void);
    virtual bool Proc(void) override;
};