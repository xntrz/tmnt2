#pragma once

#include "System/Common/Process/Sequence.hpp"


class CAnimation2D;


class CAnim2DSequence : public CSequence
{
protected:
    static const int32 ANIMNAME_MAX = 16;
    
    enum ANIMSTEP
    {
        ANIMSTEP_READFILE = 0,
        ANIMSTEP_FADEIN,
        ANIMSTEP_DRAW,
        ANIMSTEP_FADEOUT,
        ANIMSTEP_END,
    };
    
public:
    CAnim2DSequence(void);
    CAnim2DSequence(const char* pszAnimName);
    virtual ~CAnim2DSequence(void);
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual bool OnAttach(int32 iFileID);
    virtual bool OnAttach(const char* pszFilename);
    virtual void BeginFadeIn(void);
    virtual void BeginFadeOut(void);
    bool IsAnim2DMessageList(const char** pTable, int32 max, int32* index) const;
    bool IsDrawing(void) const;
    void SetAnimationName(const char* pszAnimName);
    CAnimation2D& Animation2D(void);
    CAnimation2D& Animation2D(void) const;
    ANIMSTEP AnimStep(void) const;
    void EnableLoadingDisplay(bool bEnable);

private:
    char                    m_szAnimName[ANIMNAME_MAX];
    ANIMSTEP                m_animStep;
    mutable CAnimation2D*   m_pAnimation2D;
    bool                    m_bDisplayLoading;
    bool                    m_bRet;
};