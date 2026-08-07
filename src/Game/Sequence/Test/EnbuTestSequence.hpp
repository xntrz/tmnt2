#pragma once

#ifdef _DEBUG

#include "System/Common/Process/Sequence.hpp"
#include "DebugUtils.hpp"


class CEnbuTestSequence final : public CSequence
{
private:
    enum MENU
    {
        MENU_CHR = 0,
        MENU_RANK,

        MENUNUM,
    };

public:
    static CProcess* Instance(void);

    CEnbuTestSequence(void);
    virtual ~CEnbuTestSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void InitMenuChr(CDebugMenuCtrl& menu);
    void InitMenuRank(CDebugMenuCtrl& menu);
    void InitMenuBase(CDebugMenuCtrl& menu);

private:
    int32 m_iActiveMenu;
    CDebugMenuCtrl m_aMenu[MENUNUM];
};

#endif /* _DEBUG */