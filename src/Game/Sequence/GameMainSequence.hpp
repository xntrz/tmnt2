#pragma once

#include "System/Common/Process/Sequence.hpp"


class CGameMainSequence final : public CSequence
{
private:
    enum STEP
    {
        STEP_LOAD_TEXTURE,
        STEP_LOAD_SOUND,
        STEP_RUN,
    };
    
public:
    static CProcess* Instance(void);
    
    CGameMainSequence(void);
    virtual ~CGameMainSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
    void OnLoadSound(void);
    void OnLoadTexture(void);
    void OnRun(bool bResume, int32 iLabel);
    int32 Branch(int32 iLabel);
    
private:
    STEP m_step;
    int32 m_iLabelNext;
    int32 m_iLabelPrev;
    int32 m_iLabelCurrent;
    void* m_param;
    float m_fTime;
    int32 m_iIntroMovieID;    
};