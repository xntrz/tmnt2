#pragma once


namespace Math
{
    static const float EPSILON = 1e-06f;// 1.1920929e-7f;
    static const float PI = 3.1415927f;
    static const float PI05 = 1.57079f;
    static const float PI2 = PI * 2.0f;
    static const RwV3d VECTOR3_AXIS_X = { 1.0f, 0.0f, 0.0f };
    static const RwV3d VECTOR3_AXIS_Y = { 0.0f, 1.0f, 0.0f };
    static const RwV3d VECTOR3_AXIS_Z = { 0.0f, 0.0f, 1.0f };
    static const RwV3d VECTOR3_ZERO = { 0.0f, 0.0f, 0.0f };
    static const RwV2d VECTOR2_ZERO = { 0.0f, 0.0f };

    float RadianClamp(float x);
    float FNegate(float x);
    float Floor(float x);
    float Round(float x);
    float RadianCorrect(float fRadian);
    float Sqrt(float x);
    int32 Gcd(int32 x, int32 y);
    float ToRadian(float fDegree);
    float ToDegree(float fRadian);
    float Fmod(float x, float y);
    bool FEqual(float a, float b);
    float ACos(float x);
    float ASin(float x);
    float Sin(float x);
    float Cos(float x);
    float ATan2(float y, float x);
    float Tan(float x);
    void SRand(uint32 seed);
    uint32 Rand(void);
    float RandFloat(void);
    uint32 RandRange(uint32 nLow, uint32 nHigh);
    float RandFloatRange(float fLow, float fHigh);
    float Vec2_Length(const RwV2d* pvIn);
    void Vec2_Normalize(RwV2d* pvOut, const RwV2d* pvIn);
    void Vec3_Multiply(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    void Vec3_Multiply(RwV3d* pvOut, const RwV3d* pvInA, float s);
    void Vec3_Add(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    void Vec3_Sub(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    void Vec3_Scale(RwV3d* pvOut, const RwV3d* pvIn, float s);
    float Vec3_Length(const RwV3d* pvIn);
    float Vec3_LengthXZ(const RwV3d* pvIn);
    void Vec3_Negate(RwV3d* pvOut, const RwV3d* pvIn);
    void Vec3_Abs(RwV3d* pvOut, const RwV3d* pvIn);
    void Vec3_Normalize(RwV3d* pvOut, const RwV3d* pvIn);
    void Vec3_Cross(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    float Vec3_Dot(const RwV3d* pvA, const RwV3d* pvB);
    void Vec3_Lerp(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB, float t);
    void Vec3_Reflect(RwV3d* pvOut, const RwV3d* pvIn, const RwV3d* pvNormal);
    bool Vec3_IsEqual(RwV3d const* vecA, RwV3d const* vecB);
    bool Vec3_IsEmpty(const RwV3d* pv);
    void Matrix_Update(RwMatrix* matrix, const RwV3d* right, const RwV3d* up, const RwV3d* at, const RwV3d* pos = nullptr);
    void Matrix_Multiply(RwMatrix* matrix, const RwMatrix* m1, const RwMatrix* m2);
    void Matrix_RotateX(RwMatrix* matrix, float rad);
    void Matrix_RotateY(RwMatrix* matrix, float rad);
    void Matrix_RotateZ(RwMatrix* matrix, float rad);
    void Matrix_RotationYawPitchRoll(RwMatrix* matrix, float y, float p, float r);
    void Matrix_Scale(RwMatrix* matrix, float x, float y, float z);
    void Matrix_LookAt(RwMatrix* matrix, const RwV3d* pvEye, const RwV3d* pvAt, const RwV3d* pvUp);
    void Matrix_Rotate(RwMatrix* matrix, const RwV3d* rotation);
    void Matrix_AffineTransformation(RwMatrix* matrix, const RwV3d* scale, const RwV3d* rotation, const RwV3d* position);
    void Matrix_Translate(RwMatrix* matrix, const RwV3d* pvTranslation);
    void Matrix_Invert(RwMatrix* out, RwMatrix* in);
    
    inline int32 Abs(int32 iValue)
    {
        if (iValue < 0)
            iValue = -iValue;

        return iValue;
    };
    

    inline float FAbs(float fValue)
    {
        if (fValue < 0.0f)
            fValue = -fValue;
        return fValue;
    };


    inline float RadianCorrectPrecise(float rad)
    {
        if (rad > PI)
            rad -= PI2;
        if (rad < -PI)
            rad += PI2;
        return rad;
    };


    template<class T>
    inline T Lerp(T start, T end, float t)
    {
        return ((end - start) * t) + start;
    };

    
    inline float LinearTween(float startValue, float changeValue, float time, float duration)
    {
        return (startValue + time / duration * changeValue);
    };


    template<class T>
    inline T Clamp(T val, T min, T max)
    {
        return (
            val > max ?
            max :
            (val < min ? min : val)
        );
    };

    
    template<class T>
    inline T InvClamp(T val, T min, T max)
    {
        return (
            val > max ?
            min :
            (val < min ? max : val)
        );
    };
    

    template<class T>
    inline T Max(T a, T b)
    {
        return (a > b ? a : b);
    };


    template<class T>
    inline T Min(T a, T b)
    {
        return (a < b ? a : b);
    };
};