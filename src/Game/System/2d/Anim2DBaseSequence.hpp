#pragma once

#include "System/Common/Process/Sequence.hpp"


class CAnimation2D;


class CAnim2DSequence : public CSequence
{
protected:
    static const int32 ANIMNAME_MAX = 16;
    
    enum STEP
    {
        STEP_READFILE = 0,
        STEP_FADE_OUT,
        STEP_DRAW,
        STEP_FADE_IN,
        STEP_END,
        STEP_CUSTOM_RET,
    };
    
public:
    CAnim2DSequence(void);
    CAnim2DSequence(const char* pszAnimName);
    virtual ~CAnim2DSequence(void);
    virtual bool Call(int32 iLabel, bool bKeepDraw = false, const void* param = nullptr) override;
    virtual bool Ret(const void* param = nullptr) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
    virtual bool OnAttach(int32 iFileID);
    virtual bool OnAttach(const char* pszFilename);
    virtual void BeginFadeOut(void);
    virtual void BeginFadeIn(void);
    virtual bool OnRet(void);
    bool IsAnim2DMessageList(const char** pTable, int32 max, int32* index) const;
    bool IsDrawing(void) const;
    void SetAnimationName(const char* pszAnimName);

protected:
    char m_szAnimName[ANIMNAME_MAX];
    STEP m_step;
    CAnimation2D* m_pAnimation2D;
    bool m_bDisplayLoading;
    bool m_bResumed;
};