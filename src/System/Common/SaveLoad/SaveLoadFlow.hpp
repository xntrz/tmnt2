#pragma once


class CSaveLoadDataBase;
class CSaveLoadFrameBase;


class CSaveLoadFlow
{
public:
    enum MODE
    {
        MODE_CHECK,
        MODE_SAVE,
        MODE_LOAD,
        MODE_AUTOSAVE,
    };

public:
    static bool m_bNewSave;
    
    static CSaveLoadFlow* Create(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData);
    static void Destroy(CSaveLoadFlow* pFlow);

    virtual void Initialize(void) = 0;
    virtual void Terminate(void) = 0;
    virtual bool Run(void) = 0;

protected:
    CSaveLoadFlow(MODE mode, CSaveLoadFrameBase* pFrame, CSaveLoadDataBase* pData);
    virtual ~CSaveLoadFlow(void);

protected:    
    MODE m_mode;
    CSaveLoadFrameBase* m_pFrame;
    CSaveLoadDataBase* m_pData;
};