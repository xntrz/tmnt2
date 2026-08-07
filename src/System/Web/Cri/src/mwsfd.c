#include "crimwsfd.h"
#include "adxt.h"


static CriSint32 mwsfd_init_cnt = 0;


void CRIAPI
mwPlyInitSfdFx(MwsfdInitPrm* iprm)
{
    if (!mwsfd_init_cnt++)
    {
        ADXT_Init();
    };
};


void CRIAPI
mwPlyFinishSfdFx(void)
{
    if (mwsfd_init_cnt == 0)
        return;

    if (!--mwsfd_init_cnt)
    {
        ADXT_Finish();
    };
};


CriSint32 CRIAPI
mwPlyCalcWorkCprmSfd(MwsfdCrePrm* cprm)
{
    return 0;
};


MWPLY CRIAPI
mwPlyCreateSofdec(MwsfdCrePrm* cprm)
{
    return CRI_NULL;
};


void CRIAPI
mwPlyDestroy(MWPLY mwply)
{

};


void CRIAPI
mwPlyStartFname(MWPLY mwply, const CriChar8* fname)
{

};


void CRIAPI
mwPlyStop(MWPLY mwply)
{

};


MwsfdStat CRIAPI
mwPlyGetStat(MWPLY mwply)
{
    return MWSFD_STAT_PLAYEND;
};


void CRIAPI
mwPlyGetCurFrm(MWPLY mwply, MwsfdFrmObj* frm)
{

};


void CRIAPI
mwPlyRelCurFrm(MWPLY mwply)
{

};


void CRIAPI
mwPlyStartAfs(MWPLY mwply, CriSint32 patid, CriSint32 fid)
{

};


void CRIAPI
mwPlySetOutVol(MWPLY mwply, CriSint32 vol)
{

};


void CRIAPI
mwPlyFxSetOutBufSize(MWPLY mwply, CriSint32 width, CriSint32 height)
{

};


void CRIAPI
mwPlySwitchToIdle(void)
{

};


void CRIAPI
mwPlyFxCnvFrmARGB8888(MWPLY mwply, MwsfdFrmObj* frm, CriUint8* out)
{

};