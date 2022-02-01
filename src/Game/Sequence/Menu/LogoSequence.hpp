#pragma once

#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Sprite.hpp"


class CLogoSequence final : public CSequence
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
    
public:
    static CProcess* Instance(void);

    CLogoSequence(void);
    virtual ~CLogoSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;

private:
    float m_fTime;
    int32 m_iLogoIndex;
    PHASE m_phase;
    CSprite m_Sprite;
};