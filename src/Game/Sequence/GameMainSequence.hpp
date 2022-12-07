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
    virtual void OnMove(bool bRet, const void* param) override;
    virtual void OnDraw(void) const override;
    int32 Branch(int32 iLabel);
    void PreMovie(void);
    void PostMovie(void);
    int32 BranchTest(int32 iDefaultLabel);

private:
    STEP m_step;
    int32 m_iLabelNext;
    int32 m_iLabelPrev;
    int32 m_iLabelCurrent;
    const void* m_param;
    float m_fTime;
    int32 m_iMovieID;    
};