#include "rweval.hpp"
#include "Configure.hpp"

#include "rwcore.h"


static RwCameraEndUpdateFunc cameraEndUpdate = NULL;


static RwCamera*
rwNoEvalCameraEndUpdate(RwCamera* camera)
{
    extern void* RwEngineInstance;

    RwStandardFunc CameraEndUpdateFunc = RWSRCGLOBAL(stdFunc[rwSTANDARDCAMERAENDUPDATE]);
    ASSERT(CameraEndUpdateFunc != NULL);

    if (CameraEndUpdateFunc(NULL, camera, 0))
    {
        RWSRCGLOBAL(curCamera) = NULL;
        return camera;
    };

    return NULL;
};


void rwevalInitialize(void)
{
    if (CConfigure::CheckArg("noeval"))
    {
        cameraEndUpdate = &rwNoEvalCameraEndUpdate;
    }
    else
    {
        RwCamera* camera = RwCameraCreate();
        if (camera != NULL)
        {
            cameraEndUpdate = camera->endUpdate;
            RwCameraDestroy(camera);
        };

        if (cameraEndUpdate == NULL)
            cameraEndUpdate = &rwNoEvalCameraEndUpdate;
    };
};


void rwevalTerminate(void)
{
    ;
};


RwCameraEndUpdateFunc rwevalGetCameraEndUpdateFunc(void)
{
    return cameraEndUpdate;
};