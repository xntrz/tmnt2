#pragma once

#include "GameMainSequence.hpp"


class CTrialGameMainSequence : public CGameMainSequence
{
public:
    static CProcess* Instance(void);

    CTrialGameMainSequence(void);
    virtual ~CTrialGameMainSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    int32 Branch(int32 iLabel);
};