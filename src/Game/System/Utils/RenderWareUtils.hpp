#pragma once


namespace RwUtil
{
    void InverseTransformOneVector(RwV3d* pvOut, const RwV3d* pvIn, RwMatrix* pMatrix);
    void InverseTransformOnePoint(RwV3d* pvOut, const RwV3d* pvIn, RwMatrix* pMatrix);
    void Vec3_TransformToCamera(RwV3d* pvOut, const RwV3d* pvIn, RwCamera* pCamera);
};