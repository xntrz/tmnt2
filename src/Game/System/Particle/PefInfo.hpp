#pragma once


namespace PEFINFO
{
    typedef enum tag_PATTERNFLAG : uint32
    {
        PATTERNFLAG_NONE            = 0x0,
        PATTERNFLAG_FALL            = 0x1,
        PATTERNFLAG_RISE            = 0x2,
        PATTERNFLAG_ROT_X           = 0x4,
        PATTERNFLAG_ROT_Y           = 0x8,
        PATTERNFLAG_ROT_Z           = 0x10,
        PATTERNFLAG_BILLBOARD       = 0x20,
        // TODO unknown pattern flag 0x40 (not used in code)
        // TODO unknown pattern flag 0x80 (not used in code)
        PATTERNFLAG_ACCELSIZE2D_X   = 0x100,
        PATTERNFLAG_ACCELSIZE2D_Y   = 0x200,        
        PATTERNFLAG_ACCELROT3D_X    = 0x400,
        PATTERNFLAG_ACCELROT3D_Y    = 0x1000,
        PATTERNFLAG_ACCELROT3D_Z    = 0x2000,
        PATTERNFLAG_ACCELROT2D      = 0x4000,
    } PATTERNFLAG;
    
    typedef enum tag_MOVEFLAG
    {
        MOVEFLAG_NONE           = 0x0,
        MOVEFLAG_CIRCLE         = 0x1,        
    } MOVEFLAG;

    typedef enum tag_RANDFLAG : uint32
    {
        RANDFLAG_NONE           = 0x0,
        RANDFLAG_STARTPOS_X     = 0x1,
        RANDFLAG_STARTPOS_Y     = 0x2,
        RANDFLAG_STARTPOS_Z     = 0x4,
        RANDFLAG_STARTSIZE_X    = 0x8,
        RANDFLAG_STARTSIZE_Y    = 0x10,
        // TODO unknown rand flag 0x20 (not used in code)
        RANDFLAG_LIVECOUNT      = 0x40,
        RANDFLAG_FIRSTVEC_X     = 0x80,
        RANDFLAG_FIRSTVEC_Y     = 0x100,
        RANDFLAG_FIRSTVEC_Z     = 0x200,
        RANDFLAG_FIRSTSPEED     = 0x400,
        RANDFLAG_ACCELVEC_X     = 0x800,
        RANDFLAG_ACCELVEC_Y     = 0x1000,
        RANDFLAG_ACCELVEC_Z     = 0x2000,
        // TODO unknown rand flag 0x4000 (not used in code)
        RANDFLAG_SCALESIZE2D    = 0x8000,
        RANDFLAG_SCALESIZE3D_X  = 0x10000,
        RANDFLAG_SCALESIZE3D_Y  = 0x20000,
        RANDFLAG_SCALESIZE3D_Z  = 0x40000,
        RANDFLAG_RANDANGLE      = 0x80000,
    } RANDFLAG;

    typedef enum tag_BLENDTYPE
    {
        BLENDTYPE_ADD = 0,
        BLENDTYPE_SUB,
        BLENDTYPE_ALPHA,

        BLENDTYPEMAX,
    } BLENDTYPE;

    typedef enum tag_CLIPPINGTYPE
    {
        CLIPPINGTYPE_NOCLIPPING = 0,
        CLIPPINGTYPE_NEARPLAYER,
        CLIPPINGTYPE_CAMERADISTANCE,
        CLIPPINGTYPE_INCLUDEDCAMERA,

        CLIPPINGTYPEMAX,
    } CLIPPINGTYPE;

    typedef enum tag_PARTICLEKIND
    {
        PARTICLEKIND_NORMAL = 0,
        PARTICLEKIND_STRING,
        
        PARTICLEKINDMAX,
    } PARTICLEKIND;

    typedef enum tag_PARTICLETYPE
    {
        PARTICLETYPE_BILLBOARD = 0,
        PARTICLETYPE_CYLINDER,
        PARTICLETYPE_NORMALBOARD,
        PARTICLETYPE_SPHERE,
        
        PARTICLETYPEMAX,
    } PARTICLETYPE;

    typedef struct tag_SETINFO
    {
        char m_magic[4];
        int8 m_nParticleNum;
    } SETINFO;

    CHECK_SIZE(SETINFO, 5);

    typedef struct tag_EFFECTINFO
    {
        char m_szEffectName[32];
        float m_fEffectPlayTime;
        uint32 m_bLoop;
        uint32 m_bTrace;
        char m_szTexDictName[32];
        int32 m_nClippingType;
        float m_fDistance;
    } EFFECTINFO;

    CHECK_SIZE(EFFECTINFO, 0x54);

    typedef struct tag_PARTICLEINFO
    {
        char m_szParticleName[32];
        int8 m_nType;
        int8 m_nBlend;
        int8 m_nKind;
        int8 pad;
        RwV3d m_vPosition;
        RwRGBA m_Color;
        RwV2d m_vSize;
        RwV3d m_vScale;
        RwV3d m_vDirection;
        RwV3d m_vVecFirst;
        float m_fSpeedFirst;
        RwV3d m_vAcceleration;
        float m_fAccelSpeed;
        float m_fLiveCount;
        int32 m_nOccurNum;
        float m_fOccurTiming;
        float m_fRotSpeedBillboard;
        float m_fRotationBillboard;
        RwV3d m_vRotationSpeed;
        RwV3d m_vRotation;
        uint32 m_uPatternFlag;
        uint32 m_uLinearFlag;
        float m_fAppearStart;
        float m_fAppearEnd;
        float m_fAppearTiming;
        float m_fEndTime;
        char m_szTextureName[32];
        float m_u0;
        float m_v0;
        float m_u1;
        float m_v1;
        int32 m_nRepetitionNum;
        float m_fColorDecayR;
        float m_fColorDecayG;
        float m_fColorDecayB;
        float m_fColorDecayA;
        float m_fSizeDecayWidth;
        float m_fSizeDecayHeight;
        RwV3d m_vScaleDecay;
        RwV3d m_vScaleDecayBuffer;
        float m_fTopRadius;
        float m_fBottomRadius;
        float m_fHeight;
        int32 m_nPointNum;
        int32 m_bRandCheckFlag;
        RwV3d m_vRandPosition;
        RwV2d m_vRandSize;
        RwV3d m_vRandVecFirst;
        float m_fRandSpeedFirst;
        RwV3d m_vRandAcceleration;
        float m_fRandAccel;
        float m_fRandLiveCount;
        float m_fRandOccurTiming;
        float m_fRandScaleMin;
        float m_fRandScaleMax;
        RwV3d m_vRandScale3D;
        float m_fRandAngle;
        float m_fCircleRadius;
        float m_fCircleSpeed;
        float m_fAccelSizeDecayWidth;
        float m_fAccelSizeDecayHeight;
        float m_fAccelRotationBillboard;
        RwV3d m_vAccelRotation;
    } PARTICLEINFO;

    CHECK_SIZE(PARTICLEINFO, 0x1A4);
};