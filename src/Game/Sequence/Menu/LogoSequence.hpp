#pragma once

#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Sprite.hpp"


class CLogoDisplaySequence final : public CSequence
{
private:
    enum PHASE
    {
        PHASE_LOAD,
        PHASE_BEGIN,
        PHASE_FADEIN,
        PHASE_DISPLAY,
        PHASE_FADEOUT,
        PHASE_END,
        PHASE_RET,
    };

    enum FADETYPE
    {
        FADETYPE_BLACK = 0,
        FADETYPE_WHITE,
    };

    struct LOGOPROPERTY
    {
        const char* m_pszTextureName;
        float       m_fDisplayTime;
        float       m_fStressDisplayTime;
        FADETYPE    m_eFadeinType;
        float       m_fFadeinTime;
        FADETYPE    m_eFadeoutType;
        float       m_fFadeoutTime;
        int32       m_iSound;
        void*       m_pLoadData;
    };

public:
    static CProcess* Instance(void);

    CLogoDisplaySequence(void);
    virtual ~CLogoDisplaySequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    void FadeinScreen(int32 index);
    void FadeinWait(void);
    void FadeoutScreen(int32 index);
    void FadeoutWait(void);

private:
    static const LOGOPROPERTY m_aLogoProperties[];
    float m_fTime;
    int32 m_iLogoIndex;
    PHASE m_phase;
    CSprite m_Sprite;
};