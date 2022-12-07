#include "Math.hpp"
#include "rprandom.h"
#include <cmath>


namespace Math
{
    float RadianClamp(float x)
    {
        return x;
    };
    
    float FNegate(float x)
    {
        return -x;
    };
    
    float Floor(float x)
    {
        return std::floor(x);
    };

    
    float Round(float x)
    {
        return std::round(x);
    };
    

    float RadianCorrect(float fRadian)
    {
        while (fRadian > PI)
        {
            fRadian -= PI2;
        };
        
        while (fRadian < -PI)
        {
            fRadian += PI2;
        };
        
        return fRadian;
    };

    
    float Sqrt(float x)
    {
        return std::sqrt(x);
    };

    
    int32 Gcd(int32 x, int32 y)
    {
        while (x > 0 && y > 0)
        {
            if (x > y)
                x %= y;
            else
                y %= x;
        };

        return (x + y);
    };
    

    float ToRadian(float fDegree)
    {
        return (fDegree * PI / 180.0f);
    };

    
    float ToDegree(float fRadian)
    {
        return (fRadian * 180.0f / PI);
    };

    
    float Fmod(float x, float y)
    {
        if (y == 0.0f)
            return 0.0f;
        else
            return std::fmod(x, y);
    };
    

    bool FEqual(float a, float b)
    {
        return FAbs(a - b) <= EPSILON;
    };


    float ACos(float x)
    {
        x = Clamp(x, -1.0f, 1.0f);
        return std::acosf(x);
    };


    float ASin(float x)
    {
        x = Clamp(x, -1.0f, 1.0f);
        return std::asinf(x);
    };


    float Sin(float x)
    {
        return std::sinf(x);
    };


    float Cos(float x)
    {
        return std::cos(x);
    };


    float ATan2(float y, float x)
    {
        return std::atan2f(y, x);
    };


    float Tan(float x)
    {
        return std::tan(x);
    };


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
		uint16 uValue = uint16(Rand() & 0x0000FFFF);
		return float(uValue) / 65536.0f;
    };


    uint32 RandRange(uint32 nLow, uint32 nHigh)
    {
        uint16 uValue = uint16(Rand() & 0x0000FFFF);
        
        uint32 nWidth = nHigh - nLow;
        ASSERT(nWidth <= 0x8000);

        return nLow + ((uValue * nWidth) / 65536);
    };


    float RandFloatRange(float fLow, float fHigh)
    {
        float fResult = RandFloat() * (fHigh - fLow) + fLow;
        if (fResult > fHigh)
            fResult = fHigh;

        return fResult;
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
        if (pvIn->x == 0.0f && pvIn->y == 0.0f)
        {
            pvOut->y = pvOut->x = 0.0f;
        }
        else
        {
            float len = RwV2dLength(pvIn);
            if (len > 0.000001)
            {
                float n = 1.0f / len;
                pvOut->x = n * pvIn->x;
                pvOut->y = n * pvIn->y;
            };
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
		return RwV3dLength(pvIn);
    };


    float Vec3_LengthXZ(const RwV3d* pvIn)
    {
        RwV3d vIn = { pvIn->x, 0.0f, pvIn->z };
        
        return Vec3_Length(&vIn);
    };


    void Vec3_Negate(RwV3d* pvOut, const RwV3d* pvIn)
    {
        pvOut->x = -pvIn->x;
        pvOut->y = -pvIn->y;
        pvOut->z = -pvIn->z;
    };


    void Vec3_Abs(RwV3d* pvOut, const RwV3d* pvIn)
    {
        pvOut->x = FAbs(pvIn->x);
        pvOut->y = FAbs(pvIn->y);
        pvOut->z = FAbs(pvIn->z);
    };
    

    void Vec3_Normalize(RwV3d* pvOut, const RwV3d* pvIn)
    {
        if (pvIn->x == 0.0f && pvIn->y == 0.0f && pvIn->z == 0.0f)
        {
            pvOut->x = pvOut->y = pvOut->z = 0.0f;
        }
        else
        {
            float len = RwV3dLength(pvIn);
            if (len > 0.000001)
            {
                float invlen = 1.0f / len;
                pvOut->x = invlen * pvIn->x;
                pvOut->y = invlen * pvIn->y;
                pvOut->z = invlen * pvIn->z;
            }
            else
            {
                pvOut->x = pvOut->y = pvOut->z = 0.0f;
            };
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


    bool Vec3_IsEmpty(const RwV3d* pv)
    {
        return (FEqual(pv->x, 0.0f) &&
                FEqual(pv->y, 0.0f) &&
                FEqual(pv->z, 0.0f));
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
        RwMatrixRotate(matrix, &VECTOR3_AXIS_X, ToDegree(rad), rwCOMBINEREPLACE);
    };


    void Matrix_RotateY(RwMatrix* matrix, float rad)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Y, ToDegree(rad), rwCOMBINEREPLACE);
    };


    void Matrix_RotateZ(RwMatrix* matrix, float rad)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Z, ToDegree(rad), rwCOMBINEREPLACE);
    };


    void Matrix_RotationYawPitchRoll(RwMatrix* matrix, float y, float p, float r)
    {
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Z, ToDegree(r), rwCOMBINEREPLACE);
        RwMatrixRotate(matrix, &VECTOR3_AXIS_X, ToDegree(p), rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(matrix, &VECTOR3_AXIS_Y, ToDegree(y), rwCOMBINEPOSTCONCAT);
    };


    void Matrix_Scale(RwMatrix* matrix, float x, float y, float z)
    {
        RwV3d scale = { x, y, z };
        RwMatrixScale(matrix, &scale, RwOpCombineType::rwCOMBINEREPLACE);
    };


    void Matrix_LookAt(RwMatrix* matrix, const RwV3d* pvEye, const RwV3d* pvAt, const RwV3d* pvUp)
    {
        RwV3d vXAxis = VECTOR3_ZERO;
        RwV3d vYAxis = VECTOR3_ZERO;
        RwV3d vZAxis = VECTOR3_ZERO;

        Vec3_Sub(&vZAxis, pvAt, pvEye);
        Vec3_Normalize(&vZAxis, &vZAxis);
        Vec3_Cross(&vXAxis, pvUp, &vZAxis);
        Vec3_Normalize(&vXAxis, &vXAxis);
        Vec3_Cross(&vYAxis, &vZAxis, &vXAxis);
        
        RwMatrixSetIdentity(matrix);
        matrix->right = vXAxis;
        matrix->up = vYAxis;
        matrix->at = vZAxis;
        matrix->pos = *pvEye;
        RwMatrixUpdate(matrix);
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
