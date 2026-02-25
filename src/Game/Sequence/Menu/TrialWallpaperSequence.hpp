#pragma once

#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Sprite.hpp"


class CTrialWallpaperSequence : public CSequence
{
protected:
    enum STEP
    {
        STEP_READFILE = 0,
        STEP_FADEIN,
        STEP_FADEIN_WAIT,
        STEP_DRAW,
        STEP_FADEOUT,
        STEP_FADEOUT_WAIT,
        STEP_END,
    };

    enum FADETYPE
    {
        FADETYPE_BLACK = 0,
        FADETYPE_WHITE,
    };

    enum TYPE
    {
        TYPE_UNKNOWN = 0,
        TYPE_CONTROL_INFO,
        TYPE_COMING_SOON,

        TYPENUM,
    };

public:
    CTrialWallpaperSequence(void) {};
    virtual ~CTrialWallpaperSequence(void) {};
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

protected:
    RwTexture*  m_pTexture;
    float       m_fTimer;
    STEP        m_eStep;
    CSprite     m_sprite;
    TYPE        m_type;
};


class CTrialComingSoon : public CTrialWallpaperSequence
{
public:
    static CProcess* Instance(void);

    CTrialComingSoon(void) {};
    virtual ~CTrialComingSoon(void) {};
    virtual bool OnAttach(const void* pParam) override;
};


class CTrialControlInformation : public CTrialWallpaperSequence
{
public:
    static CProcess* Instance(void);

    CTrialControlInformation(void) {};
    virtual ~CTrialControlInformation(void) {};
    virtual bool OnAttach(const void* pParam) override;
};