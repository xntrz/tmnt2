#include "PCMovie.hpp"
#include "PCSpecific.hpp"

#include "System/Common/RenderState.hpp"
#include "System/Common/System2D.hpp"


static inline void
cnvFrmBGRAtoRGBA(uint8* pData, int32 width, int32 height)
{
    int32 pixelCount = width * height;
    uint8* pSrc = pData;

    for (int32 i = 0; i < pixelCount; ++i)
    {
        uint8 b = pSrc[0];
        uint8 g = pSrc[1];
        uint8 r = pSrc[2];
        uint8 a = pSrc[3];

        pSrc[0] = r;
        pSrc[1] = g;
        pSrc[2] = b;
        pSrc[3] = a;

        pSrc += 4;
    };
};


CPCMovie::CPCMovie(int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode)
: CMovie(iWidth, iHeight, iMaxBps, bUsePalMode)
, m_pRaster(nullptr)
{
    CPCSpecific::FrameSkipEnable(false);
    
    m_pRaster = CreateRaster(MV_WIDTH, MV_HEIGHT);
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
            mwPlyFxSetOutBufSize(m_mwply, (MV_WIDTH * 4), MV_HEIGHT);
            mwPlyFxCnvFrmARGB8888(m_mwply, &m_frameObj, reinterpret_cast<CriUint8*>(pData));
            
#if defined(TMNT2_RWDRV_OPENGL)
            /* frame convert to RGBA8888 is not exist in current
               cri mw lib - so lets do it by ourselfs for opengl */
            cnvFrmBGRAtoRGBA(reinterpret_cast<uint8*>(pData), MV_WIDTH, MV_HEIGHT);
#endif /* defined(TMNT2_RWDRV_OPENGL) */

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