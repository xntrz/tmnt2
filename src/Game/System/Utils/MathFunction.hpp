#pragma once


#define MATH_PI         (3.1415927f)
#define MATH_PI2        (MATH_PI * 2.0f)

#define MATH_INV_PI     (1.0f / MATH_PI)

#define MATH_DEG2RAD(x) ((x) * ((MATH_PI) / (180.0f)))
#define MATH_RAD2DEG(x) ((x) * ((180.0f) / (MATH_PI)))


namespace Math
{
    static const float EPSILON = 1e-06f;
    static const float PI = MATH_PI;
    static const float PI05 = 1.57079f;
    static const float PI2 = PI * 2.0f;
    static const RwV3d VECTOR3_AXIS_X = { 1.0f, 0.0f, 0.0f };
    static const RwV3d VECTOR3_AXIS_Y = { 0.0f, 1.0f, 0.0f };
    static const RwV3d VECTOR3_AXIS_Z = { 0.0f, 0.0f, 1.0f };
    static const RwV3d VECTOR3_ZERO = { 0.0f, 0.0f, 0.0f };
    static const RwV2d VECTOR2_ZERO = { 0.0f, 0.0f };

    void   SRand(uint32 seed);
    uint32 Rand(void);
    float  RandFloat(void); // within [0.0f..1.0f] range
    uint32 RandRange(uint32 nLow, uint32 nHigh);
    float  RandFloatRange(float fLow, float fHigh);

    float RadianInvClamp(float x);
    float RadianCorrect(float fRadian);
    float Sqrt(float x);
    float ACos(float x);
    float ASin(float x);
    float Sin(float x);
    float Cos(float x);
    float ATan2(float y, float x);
    float Tan(float x);
    
    float Vec2_Length(const RwV2d* pvIn);
    void  Vec2_Normalize(RwV2d* pvOut, const RwV2d* pvIn);

    void  Vec3_Multiply(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    void  Vec3_Multiply(RwV3d* pvOut, const RwV3d* pvInA, float s);
    void  Vec3_Add(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    void  Vec3_Sub(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    void  Vec3_Scale(RwV3d* pvOut, const RwV3d* pvIn, float s);
    float Vec3_Length(const RwV3d* pvIn);
    void  Vec3_Negate(RwV3d* pvOut, const RwV3d* pvIn);
    void  Vec3_Abs(RwV3d* pvOut, const RwV3d* pvIn);
    void  Vec3_Normalize(RwV3d* pvOut, const RwV3d* pvIn);
    void  Vec3_Cross(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB);
    float Vec3_Dot(const RwV3d* pvA, const RwV3d* pvB);
    void  Vec3_Lerp(RwV3d* pvOut, const RwV3d* pvInA, const RwV3d* pvInB, float t);
    void  Vec3_Reflect(RwV3d* pvOut, const RwV3d* pvIn, const RwV3d* pvNormal);
    bool  Vec3_IsEqual(RwV3d const* vecA, RwV3d const* vecB);

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

    inline bool FEqual(float a, float b)
    {
        float c = (a - b);
        if (c < 0.0f)
            c = -c;
        return (c <= EPSILON);
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
};