#pragma once

#include "Game/System/2d/Anim2DBaseSequence.hpp"
#include "Game/Component/Menu/Dialog.hpp"


class CTitleSequence final : public CAnim2DSequence
{
public:
    enum NEXT_SEQUENCE
    {
        NEXT_SEQUENCE_NONE = 0,
        NEXT_SEQUENCE_GAME_NEW,
        NEXT_SEQUENCE_GAME_CONTINUE,
        NEXT_SEQUENCE_OPTIONS,
        NEXT_SEQUENCE_DEMO,
        NEXT_SEQUENCE_ARCADE,        
        NEXT_SEQUENCE_QUIT,
    };

private:    
    enum PHASE
    {
        PHASE_NONE = 0,
        PHASE_START,
        PHASE_CHOICE,
        PHASE_CHOICE_WARNING,
    };

    struct MENUITEMINFO
    {
        int32 m_iIndex;
        int32 m_iStringID;
        NEXT_SEQUENCE m_nextseq;
        bool m_bVisible;
        bool m_bEnabled;
    };

public:
    static CProcess* Instance(void);

    CTitleSequence(void);
    virtual ~CTitleSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
    virtual bool OnRet(void) override;
    void CheckPressStart(void);
    void UpdateTimers(void);
    void OpenNewGameWarning(void);
    void UpdateNewGameWarning(void);
    void UpdateMenu(void);
    void UpdateDemo(void);
    int32 GetSelectByItemIndex(int32 iItemIndex) const;

private:
    static MENUITEMINFO m_aMenuItemInfoTable[];

    NEXT_SEQUENCE m_NextSequence;
    PHASE m_phase;
    int32 m_iCurrentSelect;
    float m_fTimer;
    CDialog m_Dialog;
};