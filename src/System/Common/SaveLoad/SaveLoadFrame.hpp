#pragma once


class CSaveLoadFrameBase
{
public:
    enum PADBTN
    {
        PADBTN_OK = 0,
        PADBTN_CANCEL,

        PADBTNMAX,
    };
    
    enum MODE
    {
        MODE_MESSAGE = 0,
        MODE_SELECT,
        MODE_ERROR,
        
        MODEMAX,
    };

    enum SELECTITEM
    {
        SELECTITEM_YES = 0,
        SELECTITEM_NO,
        SELECTITEM_ERROR,
        
        SELECTITEMMAX,
    };
    
public:
    CSaveLoadFrameBase(void);
    virtual ~CSaveLoadFrameBase(void);
    virtual void Start(const wchar* pwszMessage, const wchar* pwszTitle);
    virtual void Stop(void);
    virtual void SetModeMessage(void);
    virtual void SetModeSelect(SELECTITEM defselect);
    virtual void SetModeError(PADBTN ConfirmPadbtn, float fConfirmtimeLimit);
    virtual bool IsErrorConfirmed(void) const = 0;
    virtual bool IsSelected(void) const = 0;
    virtual SELECTITEM GetSelectItem(void) const = 0;
    virtual void SetTitle(const wchar* pwszTitle);
    virtual void RemoveTitle(void);
    virtual void SetMessage(const wchar* pwszMsg);
    virtual void RemoveMessage(void);

protected:
    MODE m_mode;
    SELECTITEM m_defaultSelect;
    wchar* m_pwszTitle;
    wchar* m_pwszMessage;
    float m_fConfirmtimeStart;
    float m_fConfirmtimeLimit;
    PADBTN m_ConfirmPadbtn;
};