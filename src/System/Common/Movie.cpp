#include "Movie.hpp"
#include "RenderState.hpp"


CMovie::CMovie(int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode)
: m_mwply(0)
, m_paWorkArea(nullptr)
, m_sprite()
, m_pRenderSprite(&m_sprite)
, m_fSpritePosX(0.0f)
, m_fSpritePosY(0.0f)
, m_fSpriteW(CSprite::VIRTUALSCREEN_DEFAULT_W)
, m_fSpriteH(CSprite::VIRTUALSCREEN_DEFAULT_H)
, m_iMovieW(iWidth)
, m_iMovieH(iHeight)
, m_bIsCprmHandleOK(false)
{
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetPositionAndSize(m_fSpritePosX, m_fSpritePosY, m_fSpriteW, m_fSpriteH);
    m_sprite.SetUV(0.0f, 0.0f, 0.625f, 0.875f);

    m_mwply = CreateHandle(&m_paWorkArea, m_iMovieW, m_iMovieH, iMaxBps, bUsePalMode);
    if (m_mwply)
    {
        m_bIsCprmHandleOK = true;
    }
    else
    {
        if (m_paWorkArea)
        {
            delete[] m_paWorkArea;
            m_bIsCprmHandleOK = false;
        };
    };
};


CMovie::~CMovie(void)
{
    if (m_mwply)
    {
        mwPlySetOutVol(m_mwply, -960);
        mwPlyStop(m_mwply);
        mwPlyDestroy(m_mwply);
    };

    if (m_paWorkArea)
    {
        delete[] m_paWorkArea;
        m_paWorkArea = nullptr;        
    };

    mwPlyFinishSfdFx();
};


bool CMovie::IsEnded(void)
{
    if (!m_mwply)
        return false;

	MwsfdStat Stat = mwPlyGetStat(m_mwply);
	switch (Stat)
	{
	case MWSFD_STAT_PLAYEND:
		return true;

	default:
		return false;
	};    
};


bool CMovie::IsCreateSuccess(void)
{
    return m_bIsCprmHandleOK;
};


void CMovie::PushRenderState(void)
{
    RENDERSTATE_PUSH(rwRENDERSTATEBORDERCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREPERSPECTIVE, true);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESHADEMODE, rwSHADEMODEFLAT);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
};


void CMovie::PopRenderState(void)
{
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATESHADEMODE);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESS);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREFILTER);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREPERSPECTIVE);
    RENDERSTATE_POP(rwRENDERSTATEBORDERCOLOR);
};


RwRaster* CMovie::CreateRaster(int32 iWidth, int32 iHeight)
{
	RwRaster* pRaster = RwRasterCreate(iWidth, iHeight, 32, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888);
    if (pRaster)
    {
        void* pData = RwRasterLock(pRaster, 0, GetRwRasterLockFlag());
        std::memset(pData, 0x00, 4 * iHeight * iWidth);
        RwRasterUnlock(pRaster);        
    };

    return pRaster;
};


MWPLY CMovie::CreateHandle(char** ppWorkArea, int32 iWidth, int32 iHeight, int32 iMaxBps, bool bUsePalMode)
{
    MwsfdInitPrm iprm;
    std::memset(&iprm, 0x00, sizeof(iprm));    
    iprm.vhz = MWSFD_VHZ_50_00;
    if (!bUsePalMode)
        iprm.vhz = MWSFD_VHZ_59_94;
	iprm.disp_cycle = 1;
	iprm.disp_latency = 1;
    mwPlyInitSfdFx(&iprm);

    MwsfdCrePrm cprm;
    std::memset(&cprm, 0x00, sizeof(cprm));
    cprm.max_bps = iMaxBps;
    cprm.ftype = MWSFD_FTYPE_SFD;
    cprm.compo_mode = 0;
    cprm.nfrm_pool_wk = 1;
    cprm.max_stm = 1;
    cprm.max_width = iWidth;
    cprm.max_height = iHeight;
    cprm.wksize = mwPlyCalcWorkCprmSfd(&cprm);
    cprm.work = new CriSint8[cprm.wksize];
    ASSERT(cprm.work);
    *ppWorkArea = (char*)cprm.work;    
    return mwPlyCreateSofdec(&cprm);
};


void CMovie::ReleaseMovieFrame(void)
{
    mwPlyRelCurFrm(m_mwply);
};


bool CMovie::LoadMovieFrame(void)
{
    if (!m_mwply)
        return false;

	MwsfdStat Stat = mwPlyGetStat(m_mwply);
    if (Stat != MWSFD_STAT_PLAYING)
        return false;

    mwPlyGetCurFrm(m_mwply, &m_frameobj);
    return (m_frameobj.bufadr != nullptr);
};


void CMovie::StartAfs(int32 PtId, int32 FileId)
{
    mwPlyStartAfs(m_mwply, PtId, FileId);
};