#pragma once

#include "System/Common/Process/Process.hpp"


class CScreenFade : public CProcess
{
private:
    class CHandler;
    
public:
    static float DEFAULT_FADE_TIME;
    
    static CProcess* Instance(void);
    static void Initialize(void);
    static void Terminate(void);
    static void StartOut(float fFadeTime = DEFAULT_FADE_TIME);
    static void StartIn(float fFadeTime = DEFAULT_FADE_TIME);
    static void SetColor(uint8 r, uint8 g, uint8 b);
    static void ResetColor(void);
    static bool IsFading(void);

    CScreenFade(void);
    virtual ~CScreenFade(void);
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;

private:
    static CHandler& Handler(void);

private:
    static CScreenFade* m_pScreenFade;
    CHandler* m_pHandler;
};