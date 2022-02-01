#pragma once

#include "System/Common/Process/Sequence.hpp"


class CMovieSequence final : public CSequence
{
public:
    static CProcess* Instance(void);

    CMovieSequence(void);
    virtual ~CMovieSequence(void);
    virtual bool OnAttach(const void* param) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bResume, const void* param) override;
    virtual void OnDraw(void) const override;
};