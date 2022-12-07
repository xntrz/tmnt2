#pragma once

#include "System/Common/Movie.hpp"


class CPCMovie final : public CMovie
{
public:
    CPCMovie(int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode);
    virtual ~CPCMovie(void);
    virtual void Update(void) override;
    virtual void Draw(void) override;
    virtual uint32 GetRwRasterLockFlag(void) const override;

private:
    RwRaster* m_pRaster;
};