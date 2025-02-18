#pragma once

#include "PCTypedefs.hpp"


class CPCFramework;


class CPCSystem
{
public:
    static const char* WNDNAME;
    
public:
    static CPCSystem& Instance(void);
    
    CPCSystem(CPCFramework* pFramework);
    virtual ~CPCSystem(void);
    virtual bool Initialize(void);
    virtual void Terminate(void);
    virtual bool Run(void);
    virtual bool CheckOS(void);
#ifdef TMNT2_BUILD_EU
    virtual void SetLanguage(void);
#endif /* TMNT2_BUILD_EU */
    virtual bool WindowCreate(void);
    virtual void WindowDestroy(void);

    inline void SetFocused(bool bSet) { m_bFocused = bSet; };
    inline bool IsFocused(void) const { return m_bFocused; };
    inline CPCFramework& Framework(void) { return *m_pFramework; };

private:
    static CPCSystem* m_pInstance;
    CPCFramework*     m_pFramework;
    char              m_szOsName[64];
    STICKYKEYS        m_stickyKeys;
    TOGGLEKEYS        m_toggleKeys;
    FILTERKEYS        m_filterKeys;
    BOOL              m_bScreenSavingEnabled;
    bool              m_bFocused;
};