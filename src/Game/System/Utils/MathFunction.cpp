#include "MathFunction.hpp"

#include "rprandom.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace Math
{
    void SRand(uint32 seed)
    {
        RpRandomSeed(seed);
    };

    
    uint32 Rand(void)
    {
        return RpRandom();
    };


    float RandFloat(void)
    {
        return static_cast<float>(Rand() % 0x10000) / static_cast<float>(0x10000);
    };


    uint32 RandRange(uint32 nLow, uint32 nHigh)
    {
		uint32 uRand = (Rand() % 0x10000);
        
        uint32 nWidth = (nHigh - nLow);
        ASSERT(nWidth <= 0x8000);

        return ((uRand * nWidth) / 0x10000) + nLow;
    };


    float RandFloatRange(float fLow, float fHigh)
    {
        ASSERT(fLow <= fHigh);

        float fResult = RandFloat() * (fHigh - fLow) + fLow;
        if (fResult > fHigh)
            fResult = fHigh;

        return fResult;
    };


    float Fmod(float x, float y)
    {
        if (y == 0.0f)
            return 0.0f;

        return std::fmod(x, y);
    };


    float RadianCorrection(float fRadian)
    {
        if (fRadian > MATH_PI)
            fRadian -= MATH_PI2;

        if (fRadian < -MATH_PI)
            fRadian += MATH_PI2;

        return fRadian;
    };
    

    float RadianNormalize(float fRadian)
    {
        while (fRadian > MATH_PI)
            fRadian -= MATH_PI2;
        
        while (fRadian < -MATH_PI)
            fRadian += MATH_PI2;
        
        return fRadian;
    };

    
    float Sqrt(float x)
    {
        return std::sqrt(x);
    };


    float ACos(float x)
    {
        return std::acos(Clamp(x, -1.0f, 1.0f));
    };


    float ASin(float x)
    {
        return std::asin(Clamp(x, -1.0f, 1.0f));
    };


    float Sin(float x)
    {
        return std::sin(x);
    };


    float Cos(float x)
    {
        return std::cos(x);
    };


    float ATan2(float y, float x)
    {
        return std::atan2(y, x);
    };


    float Tan(float x)
    {
        return std::tan(x);
    };

    
    float Vec2_Length(const RwV2d* pvIn)
    {
        float fResult = 0.0f;

        if (pvIn->x != 0.0f || pvIn->y != 0.0f)
            fResult = RwV2dLength(pvIn);

        return fResult;
    };


    void Vec2_Normalize(RwV2d* pvOut, const RwV2d* pvIn)
    {
        float len = RwV2dLength(pvIn);

        if (((pvIn->x == 0.0f) && (pvIn->y == 0.0f)) || (len <= 0.000001f))
        {
            pvOut->y = pvOut->x = 0.0f;
        }
        else
        {
            float lenInv = 1.0f / len;
            pvOut->x = lenInv * pvIn->x;
            pvOut->y = lenInv * pvIn->y;
        };
    };


    void Vec3_Multiply(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB)
    {
        pvOut->x = pvInA->x * pvInB->x;
        pvOut->y = pvInA->y * pvInB->y;
        pvOut->z = pvInA->z * pvInB->z;
    };


    void Vec3_Multiply(RwV3d* pvOut, const RwV3d* pvInA, float s)
    {
        pvOut->x = pvInA->x * s;
        pvOut->y = pvInA->y * s;
        pvOut->z = pvInA->z * s;
    };


    void Vec3_Add(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB)
    {
        pvOut->x = pvInA->x + pvInB->x;
        pvOut->y = pvInA->y + pvInB->y;
        pvOut->z = pvInA->z + pvInB->z;
    };


    void Vec3_Sub(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB)
    {
        pvOut->x = pvInA->x - pvInB->x;
        pvOut->y = pvInA->y - pvInB->y;
        pvOut->z = pvInA->z - pvInB->z;
    };


    void Vec3_Scale(RwV3d* pvOut, const RwV3d* pvIn, float s)
    {
        pvOut->x = s * pvIn->x;
        pvOut->y = s * pvIn->y;
        pvOut->z = s * pvIn->z;
    };


    float Vec3_Length(const RwV3d* pvIn)
    {
        if ((pvIn->x == 0.0f) &&
            (pvIn->y == 0.0f) &&
            (pvIn->z == 0.0f))
        {
            return 0.0f;
        };
        
        return RwV3dLength(pvIn);
    };


    void Vec3_Negate(RwV3d* pvOut, const RwV3d* pvIn)
    {
        pvOut->x = -pvIn->x;
        pvOut->y = -pvIn->y;
        pvOut->z = -pvIn->z;
    };


    void Vec3_Abs(RwV3d* pvOut, const RwV3d* pvIn)
    {
        pvOut->x = std::fabs(pvIn->x);
        pvOut->y = std::fabs(pvIn->y);
        pvOut->z = std::fabs(pvIn->z);
    };
    

    void Vec3_Normalize(RwV3d* pvOut, const RwV3d* pvIn)
    {
        float len = RwV3dLength(pvIn);

        if (((pvIn->x == 0.0f) && (pvIn->y == 0.0f) && (pvIn->z == 0.0f)) || (len <= 0.000001f))
        {
            pvOut->x = pvOut->y = pvOut->z = 0.0f;
        }
        else
        {
            float lenInv = 1.0f / len;
            pvOut->x = lenInv * pvIn->x;
            pvOut->y = lenInv * pvIn->y;
            pvOut->z = lenInv * pvIn->z;
        };
    };


    void Vec3_Cross(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB)
    {
        pvOut->x = pvInB->z * pvInA->y - pvInB->y * pvInA->z;
        pvOut->y = pvInA->z * pvInB->x - pvInB->z * pvInA->x;
        pvOut->z = pvInB->y * pvInA->x - pvInB->x * pvInA->y;
    };


    float Vec3_Dot(const RwV3d* pvA, const RwV3d* pvB)
    {
        return (pvA->x * pvB->x +
                pvA->y * pvB->y +
                pvA->z * pvB->z);
    };


    void Vec3_Lerp(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB, float t)
    {
        pvOut->x = ((pvInB->x - pvInA->x) * t) + pvInA->x;
        pvOut->y = ((pvInB->y - pvInA->y) * t) + pvInA->y;
        pvOut->z = ((pvInB->z - pvInA->z) * t) + pvInA->z;
    };


    void Vec3_Reflect(RwV3d* pvOut, const RwV3d* pvIn, const RwV3d* pvNormal)
    {
        RwV3d normal = *pvNormal;
        RwV3d in = *pvIn;
        float inLen = Vec3_Length(&in);

        Vec3_Normalize(&in, &in);
        Vec3_Multiply(&in, &in, -1.0f);
        float d = Vec3_Dot(&in, &normal);
        d += d;
        Vec3_Multiply(&normal, &normal, d);
        Vec3_Sub(pvOut, &normal, &in);
        Vec3_Normalize(pvOut, pvOut);
        Vec3_Multiply(pvOut, pvOut, inLen);
    };

    
    bool Vec3_IsEqual(RwV3d const* vecA, RwV3d const* vecB)
    {
        return (FEqual(vecA->x, vecB->x) &&
                FEqual(vecA->y, vecB->y) &&
                FEqual(vecA->z, vecB->z));
    };
    

    void Matrix_Update(RwMatrix* matrix, const RwV3d* right, const RwV3d* up, const RwV3d* at, const RwV3d* pos)
    {
        matrix->right   = *right;
        matrix->up      = *up;
        matrix->at      = *at;

        if (pos)
            matrix->pos = *pos;
        
        RwMatrixUpdate(matrix);
    };


    void Matrix_Multiply(RwMatrix* matrix, const RwMatrix* m1, const RwMatrix* m2)
    {
        RwMatrix m;
        RwMatrixMultiply(&m, m1, m2);
        std::memcpy(matrix, &m, sizeof(*matrix));
    };
    

    void Matrix_RotateX(RwMatrix* matrix, float rad)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_X, MATH_RAD2DEG(rad), rwCOMBINEREPLACE);
    };


    void Matrix_RotateY(RwMatrix* matrix, float rad)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Y, MATH_RAD2DEG(rad), rwCOMBINEREPLACE);
    };


    void Matrix_RotateZ(RwMatrix* matrix, float rad)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Z, MATH_RAD2DEG(rad), rwCOMBINEREPLACE);
    };


    void Matrix_RotationYawPitchRoll(RwMatrix* matrix, float y, float p, float r)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Z, MATH_RAD2DEG(r), rwCOMBINEREPLACE);
        RwMatrixRotate(matrix, &VECTOR3_AXIS_X, MATH_RAD2DEG(p), rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Y, MATH_RAD2DEG(y), rwCOMBINEPOSTCONCAT);
    };


    void Matrix_Scale(RwMatrix* matrix, float x, float y, float z)
    {
        RwV3d scale = { x, y, z };
        RwMatrixScale(matrix, &scale, RwOpCombineType::rwCOMBINEREPLACE);
    };


    void Matrix_LookAt(RwMatrix* matrix, const RwV3d* pvEye, const RwV3d* pvAt, const RwV3d* pvUp)
    {
        RwV3d vZAxis = VECTOR3_ZERO;
        Vec3_Sub(&vZAxis, pvAt, pvEye);
        Vec3_Normalize(&vZAxis, &vZAxis);

        RwV3d vXAxis = VECTOR3_ZERO;
        Vec3_Cross(&vXAxis, pvUp, &vZAxis);
        Vec3_Normalize(&vXAxis, &vXAxis);

        RwV3d vYAxis = VECTOR3_ZERO;
        Vec3_Cross(&vYAxis, &vZAxis, &vXAxis);
        
        matrix->flags = 0;
        RwMatrixSetIdentityMacro(matrix);

        Matrix_Update(matrix, &vXAxis, &vYAxis, &vZAxis, pvEye);
    };


    void Matrix_Rotate(RwMatrix* matrix, const RwV3d* rotation)
    {
        Matrix_RotationYawPitchRoll(matrix, rotation->y, rotation->x, rotation->z);
    };


    void Matrix_AffineTransformation(RwMatrix* matrix, const RwV3d* scale, const RwV3d* rotation, const RwV3d* position)
    {
        if (scale)
            RwMatrixScale(matrix, scale, rwCOMBINEREPLACE);
        else
            RwMatrixSetIdentityMacro(matrix);

        if (rotation)
        {
            RwMatrix matRotation;
            RwMatrixSetIdentityMacro(&matRotation);
			matRotation = *matrix;

            Math::Matrix_RotationYawPitchRoll(&matRotation, rotation->y, rotation->x, rotation->z);
            Math::Matrix_Multiply(matrix, matrix, &matRotation);
        };

        if (position)
        {
            RwMatrix matTranslation;
            RwMatrixSetIdentityMacro(&matTranslation);

            RwMatrixTranslate(&matTranslation, position, rwCOMBINEREPLACE);
            Math::Matrix_Multiply(matrix, matrix, &matTranslation);
        };
    };


    void Matrix_Translate(RwMatrix* matrix, const RwV3d* pvTranslation)
    {
        RwMatrixTranslate(matrix, pvTranslation, rwCOMBINEREPLACE);
    };

    
    void Matrix_Invert(RwMatrix* out, RwMatrix* in)
    {
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        
        RwMatrixInvert(&matrix, in);
        std::memcpy(out, &matrix, sizeof(*out));
    };
};
