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
    virtual void BeginFadein(void);
    virtual void BeginFadeout(void);
    bool IsAnim2DMessageList(const char** pTable, int32 max, int32* index) const;
    bool IsDrawing(void) const;
    void SetAnimationName(const char* pszAnimName);

protected:
    char m_szAnimName[ANIMNAME_MAX];
    ANIMSTEP m_animstep;
    CAnimation2D* m_pAnimation2D;
    bool m_bDisplayLoading;
    bool m_bResumed;
};