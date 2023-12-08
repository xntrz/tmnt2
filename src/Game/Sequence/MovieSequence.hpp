#pragma once

#include "System/Common/Process/Sequence.hpp"


class CMovie;


class CMovieSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CMovieSequence(void);
    virtual ~CMovieSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;

private:
    CMovie* m_pMovie;
    bool m_bOwner;
};