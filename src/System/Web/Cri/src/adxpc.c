#include "adxpc.h"
#include "cvfs.h"
#include "rwci.h"


static CriBool adxpc_fs_init_flag = 0;
static CriBool adxpc_sd_init_flag = 0;


void CRIAPI
ADXPC_SetupFileSystem(AdxpcSprmFs* sprmd)
{
    if (!adxpc_fs_init_flag++)
    {
        cvFsInit();
        rwCiInit();

        cvFsAddDev("RW", &rwCiGetInterface);
        cvFsSetDefDev("RW");

        rwCiSetRootDir(sprmd->rtdir);
    };
};


void CRIAPI
ADXPC_ShutdownFileSystem(void)
{
    if (adxpc_fs_init_flag == 0)
        return;

    if (!--adxpc_fs_init_flag)
    {
        cvFsSetDefDev(CRI_NULL);
        cvFsDelDev("RW");

        rwCiFinish();
        cvFsFinish();
    };    
};


ADXFIC CRIAPI
ADXFIC_Create(const CriChar8* dname, CriSint32 mode, void* work, CriSint32 wksize)
{
    return (ADXFIC)1; // return something but not zero
};


void CRIAPI
ADXFIC_Destroy(ADXFIC fic)
{
    ;
};


CriSint32 CRIAPI
ADXPC_SetupSoundDirectSound8(LPDIRECTSOUND8 ds)
{
    return 0;
};


CriSint32 CRIAPI
ADXPC_ShutdownSound(void)
{
    return 0;
};


void CRIAPI
ADXPC_SetDsbCapsGlobalFocus(CriBool flag)
{
    ;
};


void CRIAPI
ADXPC_SetDsbCapsCtrl3D(CriBool flag)
{
    ;
};


void CRIAPI
ADXPC_SetDsbCapsCtrlFx(CriBool flag)
{
    ;
};


void CRIAPI
ADXPC_SetDsbNumChannels(CriSint32 num)
{
    ;
};