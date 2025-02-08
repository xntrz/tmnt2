#pragma once

#include "MapTypes.hpp"


class CWorldMap
{
public:
    static const float INVALID_MAP_HEIGHT;

    enum DRAWTYPE
    {
        DRAWTYPE_NORMAL = 0,
        DRAWTYPE_AFTER,
    };
    
    enum WEATHER
    {
        WEATHER_FINE = 0,
        WEATHER_SNOW,
        WEATHER_RAIN,
        WEATHER_SANDER,
    };

    enum CHECKFLAG : uint32
    {
        CHECKFLAG_NONE      = 0,
        CHECKFLAG_WALLJUMP  = (1 << 0),
        CHECKFLAG_ONEWAY    = (1 << 1),
        CHECKFLAG_MAPONLY   = (1 << 2),
    };

    DEFINE_ENUM_FLAG_OPS(CHECKFLAG, friend);

    enum COLLISIONTYPE
    {
        COLLISIONTYPE_NONE = 0,
        COLLISIONTYPE_MAPCAM,
        COLLISIONTYPE_MAPCOL,
    };

    struct FOGINFO
    {
        uint32  m_bEnable;
        RwRGBA  m_Color;
        float   m_fDist;
        uint32  m_dummy;
    };

    struct WATERINFO
    {
        uint32  m_bOffset;
        float   m_fWaterSurfaceHeight;
        uint32  m_dummy0;
        uint32  m_dummy1;
    };

    struct MAPINFO
    {
        char        m_szName[16];
        float       m_fFarClipDist;
        float       m_fNearClipDist;
        float       m_fDeathHeight;
        RwV3d       m_vLightDirection;
        WATERINFO   m_waterinfo;
        WEATHER     m_weather;
        FOGINFO     m_foginfo;
        uint32      m_bBackFaceCulling;
    };

    CHECK_SIZE(MAPINFO, 80);

    struct COLLISIONPARAM_MAPGIMMICK
    {
        RwV3d                 m_vVelocity;
        RwV3d                 m_vRotate;
        RwMatrix*             m_pLTM;
        char                  m_szGimmickObjName[16];
        MAPTYPES::GIMMICKTYPE m_type;
    };

    struct COLLISIONPARAM_MAPWALL
    {
        RwV3d   m_vClosestPt;
        RwV3d   m_vNormal;
        float   m_fDistance;
        bool    m_bHit;
    };

    struct COLLISIONPARAM_BASE
    {
        RwV3d                     m_vClosestPt;
        RwV3d                     m_vNormal;
        MAPTYPES::ATTRIBUTE       m_attribute;
        RwRGBA                    m_Color;
        MAPTYPES::HITTYPE         m_type;
        COLLISIONPARAM_MAPGIMMICK m_mapobj;
        COLLISIONPARAM_MAPWALL    m_mapwall;
    };

    struct COLLISIONPARAM_MODEL : public COLLISIONPARAM_BASE
    {
        float       m_fDistance;
        char        m_szAttributeName[32];
        RwV3d*      m_pPrimitiveVelocity;
        RpGeometry* m_pGeometry;
        bool        m_bUpdate;
    };

    struct COLLISIONRESULT : public COLLISIONPARAM_BASE {};
    
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Period(void);
    static void Draw(DRAWTYPE drawtype);
    static void OnLoaded(void);
    static void ReadBsp(const char* pszName, void* pBuffer, uint32 uBufferSize);
    static void ReadMapInfo(void* pBuffer, uint32 uBufferSize);
    static const MAPINFO* GetMapInfo(void);
    static void GetWaterSurfacePosition(RwV3d* pPos);
    static float GetCharacterDeathHeight(void);
    static RpWorld* GetWorld(void);
    static void SetAmbientLightColor(const RwRGBAReal& rColor);
    static void SetDirectionLightColor(const RwRGBAReal& rColor);
    static void SetDirectionLightMatrix(RwMatrix* pMatrix);
    static bool CheckCollisionLine(const RwV3d* pPtStart, const RwV3d* pPtEnd);
    static bool CheckCollisionSphere(RwV3d* pPos, float fRadius);
    static bool CheckCollisionSphereMove(RwV3d* pPos, float fRadius, RwV3d* pVelocity, CHECKFLAG checkflag);
    static bool CheckCollisionMoveSphere(RwV3d* pPos, float fRadius, RwV3d* pVelocity, CHECKFLAG checkflag);
    static bool GetCameraAreaName(RwV3d* pPos, char* pszCameraAreaName);
    static bool CheckCollisionHeight(RwV3d* pPos, float fRadius, float fHeight);
    static float GetMapHeight(const RwV3d* pPos);
    static float GetMapHeight(const RwV3d* pPos, float fRadius);
    static const COLLISIONRESULT* GetCollisionResult(void);
    static const RwV3d* GetCollisionResultClosestPoint(void);
    static const RwV3d* GetCollisionResultNormal(void);
    static MAPTYPES::ATTRIBUTE GetCollisionResultAttribute(void);    
    static bool CheckCollisionCharacterMove(RwV3d* pPos, RwV3d* pNewPos, RwV3d* pVel, uint32* puHitFlag, float fRadius, float fHeight);
    static bool CheckCollisionCharacterHeight(RwV3d* pPos, RwV3d* pNewPos, float* pfRotY, float fRadius, float fHeight);
};