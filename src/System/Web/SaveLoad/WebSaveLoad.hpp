#pragma once


class CWebSaveManager;
class CWebLoadManager;
class CWebQuestManager;
class CWebWarningManager;


class CWebSaveLoadBaseFlow
{
public:
    CWebSaveLoadBaseFlow(void);
    virtual ~CWebSaveLoadBaseFlow(void);
    virtual bool Proc(void) = 0;

protected:
    int32 m_step;
    static CWebSaveManager* m_pSave;
    static CWebLoadManager* m_pLoad;
    static CWebQuestManager* m_pQuest;
    static CWebWarningManager* m_pWarning;
};


class CWebStartCheckFlow : public CWebSaveLoadBaseFlow
{
protected:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_RUN,
        STEP_EOL,
    };
    
public:
    CWebStartCheckFlow(void);
    virtual ~CWebStartCheckFlow(void);
    virtual bool Proc(void) override;
};


class CWebMenuSaveFlow : public CWebSaveLoadBaseFlow
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
    CWebMenuSaveFlow(void);
    virtual ~CWebMenuSaveFlow(void);
    virtual bool Proc(void) override;
};


class CWebMenuLoadFlow : public CWebSaveLoadBaseFlow
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
    CWebMenuLoadFlow(void);
    virtual ~CWebMenuLoadFlow(void);
    virtual bool Proc(void) override;
};


class CWebAutosaveFlow : public CWebSaveLoadBaseFlow
{
protected:
    enum STEP
    {
        STEP_INIT = 0,
        STEP_RUN,
        STEP_EOL,
    };
    
public:
    CWebAutosaveFlow(void);
    virtual ~CWebAutosaveFlow(void);
    virtual bool Proc(void) override;
};