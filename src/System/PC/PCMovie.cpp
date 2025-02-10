#include "PCMovie.hpp"
#include "PCSpecific.hpp"

#include "System/Common/RenderState.hpp"
#include "System/Common/System2D.hpp"


CPCMovie::CPCMovie(int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode)
: CMovie(iWidth, iHeight, iMaxBps, bUsePalMode)
, m_pRaster(nullptr)
{
    CPCSpecific::FrameSkipEnable(false);
    
    m_pRaster = CreateRaster(1024, 512);
    ASSERT(m_pRaster);
};


CPCMovie::~CPCMovie(void)
{
    if (m_pRaster)
    {
        RwRasterDestroy(m_pRaster);
        m_pRaster = nullptr;
    };

    CPCSpecific::FrameSkipEnable(true);
};


void CPCMovie::Update(void)
{
    mwPlySwitchToIdle();
    
    if (LoadMovieFrame())
    {
        void* pData = RwRasterLock(m_pRaster, 0, GetRwRasterLockFlag());
        if (pData)
        {
            mwPlyFxSetOutBufSize(m_mwply, 4096, 512);
            mwPlyFxCnvFrmARGB8888(m_mwply, &m_frameObj, reinterpret_cast<CriUint8*>(pData));
            RwRasterUnlock(m_pRaster);
        };
        
        ReleaseMovieFrame();
    };
};


void CPCMovie::Draw(void)
{
    PushRenderState();

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, m_pRaster);
    
    m_pRenderSprite->Draw();

    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);

    PopRenderState();
};


uint32 CPCMovie::GetRwRasterLockFlag(void) const
{
    return rwRASTERLOCKWRITE;
};