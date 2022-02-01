#pragma once

#include "System/Common/SaveLoad/SaveLoadFrame.hpp"


class CMessageWindow;


class CSaveLoadFrame : public CSaveLoadFrameBase
{
public:
    CSaveLoadFrame(void);
    virtual ~CSaveLoadFrame(void);
    virtual void Start(const wchar* pwszMessage, const wchar* pwszTitle) override;
    virtual void Stop(void) override;
    virtual void SetModeMessage(void) override;
    virtual void SetModeSelect(SELECTITEM defselect) override;
    virtual void SetModeError(PADBTN ConfirmPadbtn, float fConfirmtimeLimit) override;
    virtual bool IsErrorConfirmed(void) const override;
    virtual bool IsSelected(void) const override;
    virtual SELECTITEM GetSelectItem(void) const override;
    virtual void SetTitle(const wchar* pwszTitle) override;
    virtual void RemoveTitle(void) override;
    virtual void SetMessage(const wchar* pwszMsg) override;
    virtual void RemoveMessage(void) override;

protected:
    void CreateMessageWindow(void);
    void DestroyMessageWindow(void);
    
protected:
    CMessageWindow* m_pWindow;
    float m_fTimer;
};