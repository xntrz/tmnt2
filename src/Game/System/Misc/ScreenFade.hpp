#pragma once

#include "System/Common/Process/Process.hpp"


class CScreenFade
{
private:
    class CScreenFadeController;
    
public:
    static float DEFAULT_FADE_TIME;

public:    
    static bool Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(void);
    static bool IsFading(void);
    static bool IsDrawing(void);
    static void BlackIn(float fTime = DEFAULT_FADE_TIME);
    static void BlackOut(float fTime = DEFAULT_FADE_TIME);
    static void WhiteIn(float fTime = DEFAULT_FADE_TIME);
    static void WhiteOut(float fTime = DEFAULT_FADE_TIME);

private:
    static CScreenFadeController* m_pScreenFade;
};


class CScreenFadeProcess final : public CProcess
{
public:    
    static CProcess* Instance(void);
    static bool Initialize(CProcess* pCurrent);
    static void Terminate(CProcess* pCurrent);
    
    CScreenFadeProcess(void);
    virtual ~CScreenFadeProcess(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
};