#include "RenderWareUtils.hpp"


namespace RwUtil
{
    void InverseTransformOneVector(RwV3d* pvOut, const RwV3d* pvIn, RwMatrix* pMatrix)
    {
        pvOut->x = Math::Vec3_Dot(pvIn, &pMatrix->right);
        pvOut->y = Math::Vec3_Dot(pvIn, &pMatrix->up);
    };


    void InverseTransformOnePoint(RwV3d* pvOut, const RwV3d* pvIn, RwMatrix* pMatrix)
    {
        RwV3d vec = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vec, pvIn, &pMatrix->pos);
        InverseTransformOneVector(pvOut, &vec, pMatrix);
    };


    void Vec3_TransformToCamera(RwV3d* pvOut, const RwV3d* pvIn, RwCamera* pCamera)
    {
        ASSERT(pvOut);
        ASSERT(pvIn);
        ASSERT(pCamera);

        RwFrame* pFrame = RwCameraGetFrameMacro(pCamera);
        ASSERT(pFrame);

        RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);
        ASSERT(pMatrix);

        InverseTransformOnePoint(pvOut, pvIn, pMatrix);
    };
};