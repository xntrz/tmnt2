#include "WorldMap.hpp"
#include "MapCollision.hpp"
#include "MapCollisionModel.hpp"
#include "MapClumpModelMan.hpp"
#include "CameraDataMan.hpp"

#include "Game/Component/Effect/WaterSplash.hpp"
#include "Game/Component/Effect/ScreenEffectRain.hpp"
#include "Game/Component/Effect/Snow.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Camera.hpp"

#include "rppvs.h"


class CWorldMapManager
{
public:
    CWorldMapManager(void);
    ~CWorldMapManager(void);
    void OnLoaded(void);
    void ReadWorldBsp(void* pBuffer, uint32 uBufferSize);
    void ReadMapInfo(void* pBuffer, uint32 uBufferSize);
    void ReadCollisionBsp(void* pBuffer, uint32 uBufferSize);
    void ReadCameraAreaBsp(void* pBuffer, uint32 uBufferSize);
    RpWorld* ReadBsp(void* pBuffer, uint32 uBufferSize);
    void SetWorldMapEnvironment(void);
    void DestroyWorld(void);
    void CreateLight(void);
    void DestroyLight(void);
    void Period(void);
    void Draw(CWorldMap::DRAWTYPE drawtype);
    void SetAmbientLightColor(const RwRGBAReal& rColor);
    void SetDirectionLightColor(const RwRGBAReal& rColor);
    void SetDirectionLightMatrix(RwMatrix* pMatrix);
    const CWorldMap::MAPINFO* GetMapInfo(void) const;
    RpWorld* GetWorld(void) const;
    const CWorldMap::COLLISIONRESULT* GetCollisionResult(void) const;
    bool CheckWorldObjCollision(RpIntersection* pIntersection, CMapCollision::COLLISIONPARAM* pCollisionParam, CWorldMap::COLLISIONTYPE collisiontype);
    bool CheckWorldMapCollision(RpIntersection* pIntersection, CMapCollision::COLLISIONPARAM* pCollisionParam, CWorldMap::COLLISIONTYPE collisiontype);
    void CheckWorldCollision(RpIntersection* pIntersection, CMapCollision::COLLISIONPARAM* pCollisionParam, CWorldMap::COLLISIONTYPE collisiontype);
    bool CheckCollision(RpIntersection* pIntersection, RwV3d* pVel = nullptr, CWorldMap::CHECKFLAG checkflag = CWorldMap::CHECKFLAG_NONE);
    bool CheckCollisionCharacterMove(RwV3d* pPos, RwV3d* pNewPos, RwV3d* pVel, CWorldMap::MOVE_HIT_FLAG* hitFlag, float fRadius, float fHeight);
    bool CheckCollisionCharacterHeight(RwV3d* pPos, RwV3d* pNewPos, float* pfRotY, float fRadius, float fHeight, bool bLimitNormalY);
    bool CheckCollisionHeight(RwV3d* pPos, float fRadius, float fHeight);
    bool CheckCollisionLine(const RwV3d* pPtStart, const RwV3d* pPtEnd);
    bool CheckCollisionSphere(RwV3d* pPos, float fRadius);
    bool CheckCollisionSphereMove(RwV3d* pPos, float fRadius, RwV3d* pVel, CWorldMap::CHECKFLAG checkflag);
    bool CheckCollisionMoveSphere(RwV3d* pPos, float fRadius, RwV3d* pVel = nullptr, CWorldMap::CHECKFLAG checkflag = CWorldMap::CHECKFLAG_NONE);
    bool GetCameraAreaName(RwV3d* pPos, char* pszCameraAreaName);
    float GetMapHeight(const RwV3d* pPos);
    float GetMapHeight(const RwV3d* pPos, float fRadius);
    void SetCharacterMoveSphereOfPos(RwV3d aOfsPos[4], float fRadius, float fHeight, RwMatrix* pMatrix = nullptr);
    
private:
    RpWorld* m_pWorld;
    RpWorld* m_pWorldCollision;
    RpWorld* m_pWorldCameraArea;    
    RwTexDictionary* m_pTexDict;
    RpLight* m_pLightAmbient;
    RpLight* m_pLightDirection;
    void* m_hUVAnim;
    CWorldMap::MAPINFO* m_pMapInfo;
    CWorldMap::COLLISIONRESULT m_CollisionResult;
};


CWorldMapManager::CWorldMapManager(void)
: m_pWorld(nullptr)
, m_pWorldCollision(nullptr)
, m_pWorldCameraArea(nullptr)
, m_pTexDict(nullptr)
, m_pLightAmbient(nullptr)
, m_pLightDirection(nullptr)
, m_hUVAnim(nullptr)
, m_pMapInfo(nullptr)
{
    ;
};


CWorldMapManager::~CWorldMapManager(void)
{
    if (m_pMapInfo)
    {
        delete m_pMapInfo;
        m_pMapInfo = nullptr;
    };

    DestroyLight();
    DestroyWorld();
};


void CWorldMapManager::OnLoaded(void)
{
    if (m_pMapInfo)
    {
        RwV3d vRotation =
        {
            MATH_DEG2RAD(m_pMapInfo->m_vLightDirection.x) + MATH_PI05,
            MATH_DEG2RAD(m_pMapInfo->m_vLightDirection.y),
            MATH_DEG2RAD(m_pMapInfo->m_vLightDirection.z),
        };
        
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        Math::Matrix_Rotate(&matrix, &vRotation);

        SetDirectionLightMatrix(&matrix);

        CRenderStateManager::SetFogParam(bool(m_pMapInfo->m_foginfo.m_bEnable > 0),  m_pMapInfo->m_foginfo.m_Color);
        CScreen::SetClearColor(m_pMapInfo->m_foginfo.m_Color);

        switch (m_pMapInfo->m_weather)
        {
        case CWorldMap::WEATHER_RAIN:
            CScreenEffectRain::SetRain();
            CWaterSplashManager::SetDefaultTexture();            
            break;

        case CWorldMap::WEATHER_SNOW:
            CSnowManager::SetNormalSnow();            
            break;

        default:
            break;
        };
    }
    else
    {
        CScreen::SetClearColor({ 0x00, 0x00, 0x00, 0x00 });
    };

    SetDirectionLightColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    SetAmbientLightColor({ 0.6f, 0.6f, 0.6f, 1.0f });

    if (m_hUVAnim)
        CUVAnim::InitUVAnim(m_hUVAnim);    
};


void CWorldMapManager::ReadWorldBsp(void* pBuffer, uint32 uBufferSize)
{
    bool bUVExists = CUVAnim::IsExistsUVAnimDict("mapanim");    
    if (bUVExists)
        CUVAnim::SetUVAnimDict("mapanim");

    m_pWorld = ReadBsp(pBuffer, uBufferSize);
    ASSERT(m_pWorld);

    SetWorldMapEnvironment();

    if (bUVExists)
        m_hUVAnim = CUVAnim::CreateAnimatedMaterialsList(m_pWorld);
};


void CWorldMapManager::ReadMapInfo(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(!m_pMapInfo);
    ASSERT(uBufferSize == sizeof(CWorldMap::MAPINFO));
    
    m_pMapInfo = new CWorldMap::MAPINFO;

    std::memcpy(m_pMapInfo, pBuffer, uBufferSize);
};


void CWorldMapManager::ReadCollisionBsp(void* pBuffer, uint32 uBufferSize)
{
    m_pWorldCollision = ReadBsp(pBuffer, uBufferSize);
    ASSERT(m_pWorldCollision);
};


void CWorldMapManager::ReadCameraAreaBsp(void* pBuffer, uint32 uBufferSize)
{
    m_pWorldCameraArea = ReadBsp(pBuffer, uBufferSize);
    ASSERT(m_pWorldCameraArea);
};


RpWorld* CWorldMapManager::ReadBsp(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer != nullptr);
    ASSERT(uBufferSize > 0);

    RpWorld* pWorld = nullptr;
    
    RwMemory memStream;
    memStream.start  = static_cast<RwUInt8*>(pBuffer);
    memStream.length = uBufferSize;

    RwStream* pRwStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &memStream);
    ASSERT(pRwStream);

    if (pRwStream)
    {
        if (RwStreamFindChunk(pRwStream, rwID_WORLD, nullptr, nullptr))
            pWorld = RpWorldStreamRead(pRwStream);

        RwStreamClose(pRwStream, nullptr);
    };

    return pWorld;
};


void CWorldMapManager::SetWorldMapEnvironment(void)
{
    CreateLight();

    RwUInt32 worldFlags = RpWorldGetFlagsMacro(m_pWorld);

    worldFlags &= (~rpWORLDLIGHT);
    worldFlags |= rpWORLDPRELIT;

    RpWorldSetFlagsMacro(m_pWorld, worldFlags);
};


void CWorldMapManager::DestroyWorld(void)
{
    if (m_hUVAnim)
    {
        CUVAnim::DestroyAnimatedMaterialsList(m_hUVAnim);
        m_hUVAnim = nullptr;
    };

    if (m_pTexDict)
    {
        RwTexDictionaryDestroy(m_pTexDict);
        m_pTexDict = nullptr;
    };

    if (m_pWorld)
    {
        RpWorldDestroy(m_pWorld);
        m_pWorld = nullptr;
    };

    if (m_pWorldCollision)
    {
        RpWorldDestroy(m_pWorldCollision);
        m_pWorldCollision = nullptr;
    };

    if (m_pWorldCameraArea)
    {
        RpWorldDestroy(m_pWorldCameraArea);
        m_pWorldCameraArea = nullptr;
    };
};


void CWorldMapManager::CreateLight(void)
{
    if (!m_pWorld)
        return;

    ASSERT(!m_pLightAmbient);
    ASSERT(!m_pLightDirection);

    m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
    ASSERT(m_pLightAmbient);

    if (m_pLightAmbient)
    {
        RwRGBAReal lightColor = { 0.6f, 0.6f, 0.6f, 1.0f };
        RpLightSetColor(m_pLightAmbient, &lightColor);

        RpWorldAddLight(m_pWorld, m_pLightAmbient);

        RwUInt8 lightFlags = RpLightGetFlagsMacro(m_pLightAmbient);;
        lightFlags &= (~rpLIGHTLIGHTWORLD);
        RpLightSetFlagsMacro(m_pLightAmbient, lightFlags);
    };

    m_pLightDirection = RpLightCreate(rpLIGHTDIRECTIONAL);
    ASSERT(m_pLightDirection);

    if (m_pLightDirection)
    {
        RwFrame* pFrame = RwFrameCreate();
        if (pFrame)
        {
            RwRGBAReal lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
            RwFrameRotate(pFrame, &Math::VECTOR3_AXIS_X, 45.0f, rwCOMBINEREPLACE);
            RwFrameRotate(pFrame, &Math::VECTOR3_AXIS_Y, 45.0f, rwCOMBINEPOSTCONCAT);
            RpLightSetFrame(m_pLightDirection, pFrame);
            RpLightSetColor(m_pLightDirection, &lightColor);

            RpWorldAddLight(m_pWorld, m_pLightDirection);

            RwUInt8 lightFlags = RpLightGetFlagsMacro(m_pLightDirection);
            lightFlags &= (~rpLIGHTLIGHTWORLD);
            RpLightSetFlagsMacro(m_pLightDirection, lightFlags);            
        };
    };

#ifdef TARGET_PC
    //D3DRS_AMBIENT
    RwD3D9SetRenderState(139, 0xFFFFFFFF);
#else
#error Not implemented for current target
#endif
};


void CWorldMapManager::DestroyLight(void)
{
    if (m_pLightDirection)
    {        
        RpWorldRemoveLight(m_pWorld, m_pLightDirection);

        RwFrame* pFrame = RpLightGetFrame(m_pLightDirection);
        if (pFrame)
            RwFrameDestroy(pFrame);
        
        RpLightDestroy(m_pLightDirection);
        m_pLightDirection = nullptr;
    };

    if (m_pLightAmbient)
    {
        RpWorldRemoveLight(m_pWorld, m_pLightAmbient);

        RpLightDestroy(m_pLightAmbient);
        m_pLightAmbient = nullptr;
    };
};


void CWorldMapManager::Period(void)
{
    if (m_hUVAnim)
        CUVAnim::UpdateUVAnim(m_hUVAnim, CGameProperty::GetElapsedTime());
};


void CWorldMapManager::Draw(CWorldMap::DRAWTYPE drawtype)
{
    if (m_pMapInfo)
    {
        if (m_pMapInfo->m_foginfo.m_bEnable)
        {
            RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, true);
            RENDERSTATE_PUSH(
                rwRENDERSTATEFOGCOLOR,
                RWRGBALONGEX(m_pMapInfo->m_foginfo.m_Color)
            );
            RENDERSTATE_PUSH(rwRENDERSTATEFOGTYPE, rwFOGTYPELINEAR);
        }
        else
        {
            RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
        };

        if (m_pMapInfo->m_bBackFaceCulling)
        {
            RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLBACK);            
        }
        else
        {
            RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
        };
    };

    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);

    switch (drawtype)
    {
    case CWorldMap::DRAWTYPE_NORMAL:
        {
            bool bPVSHookStatus = false;

            if (RpPVSQuery(m_pWorld))
            {
                if (RpPVSHook(m_pWorld))
                {
                    bPVSHookStatus = true;

                    RwCamera* pCameraCurrent = CCamera::CameraCurrent();
                    if (pCameraCurrent)
                    {
                        RwFrame* pFrame = RwCameraGetFrameMacro(pCameraCurrent);
                        RwMatrix* pLTM = RwFrameGetLTM(pFrame);

                        RpPVSSetViewPosition(m_pWorld, &pLTM->pos);
                    };
                };                
            };

            RpWorldRender(m_pWorld);

            if (bPVSHookStatus)
                RpPVSUnhook(m_pWorld);
        }
        break;

    case CWorldMap::DRAWTYPE_AFTER:
        {
            CMapClumpModelManager::Draw();
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
};


void CWorldMapManager::SetAmbientLightColor(const RwRGBAReal& rColor)
{
    if (m_pLightAmbient)
        RpLightSetColor(m_pLightAmbient, &rColor);
};


void CWorldMapManager::SetDirectionLightColor(const RwRGBAReal& rColor)
{
    if (m_pLightDirection)
        RpLightSetColor(m_pLightDirection, &rColor);
};


void CWorldMapManager::SetDirectionLightMatrix(RwMatrix* pMatrix)
{
    if (!m_pLightDirection)
        return;
    
    RwFrame* pFrame = RpLightGetFrameMacro(m_pLightDirection);
    if (!pFrame)
        return;

    RwFrameTransform(pFrame, pMatrix, rwCOMBINEREPLACE);
};


const CWorldMap::MAPINFO* CWorldMapManager::GetMapInfo(void) const
{
    return m_pMapInfo;
};


RpWorld* CWorldMapManager::GetWorld(void) const
{
    return m_pWorld;
};


const CWorldMap::COLLISIONRESULT* CWorldMapManager::GetCollisionResult(void) const
{
    return &m_CollisionResult;
};


bool CWorldMapManager::CheckWorldObjCollision(RpIntersection* pIntersection, CMapCollision::COLLISIONPARAM* pCollisionParam, CWorldMap::COLLISIONTYPE collisiontype)
{
    if ((collisiontype == CWorldMap::COLLISIONTYPE_MAPCAM) ||
        (collisiontype == CWorldMap::COLLISIONTYPE_MAPCOL))
        return false;

    RpIntersection intersection;
    std::memcpy(&intersection, pIntersection, sizeof(intersection));
    float fDistance = pCollisionParam->m_fDistance;

    if (!CMapCollisionModel::CheckModelCollision(&intersection, pCollisionParam->m_pvVelocity))
        return false;

    const CMapCollisionModel::COLLISIONPARAM_MODEL* pModelCollisionResult = CMapCollisionModel::GetModelCollisionResult();
    
    std::memcpy(
        pCollisionParam,
        pModelCollisionResult,
        sizeof(CWorldMap::COLLISIONPARAM_BASE)
    );

    std::strcpy(
        pCollisionParam->m_szAttributeName,
        pModelCollisionResult->m_szAttributeName
    );

    pCollisionParam->m_fDistance = pModelCollisionResult->m_fDistance;
    
    if (pModelCollisionResult->m_fDistance < fDistance)
        pCollisionParam->m_type = MAPTYPES::HITTYPE_GIMMICK;

    return true;
};


bool CWorldMapManager::CheckWorldMapCollision(RpIntersection* pIntersection, CMapCollision::COLLISIONPARAM* pCollisionParam, CWorldMap::COLLISIONTYPE collisiontype)
{
    if (!m_pWorldCollision ||
        !m_pWorldCameraArea)
        return false;

    float fDistance = pCollisionParam->m_fDistance;
    
    pCollisionParam->m_pWorldCollision =
        (collisiontype == CWorldMap::COLLISIONTYPE_MAPCAM ? m_pWorldCameraArea : m_pWorldCollision);        

    CMapCollision::CheckMapCollision(pIntersection, pCollisionParam);
    
    if (pCollisionParam->m_fDistance < fDistance)
        pCollisionParam->m_type = MAPTYPES::HITTYPE_MAP;

    return (pCollisionParam->m_fDistance < TYPEDEF::FLOAT_MAX);
};


void CWorldMapManager::CheckWorldCollision(RpIntersection* pIntersection, CMapCollision::COLLISIONPARAM* pCollisionParam, CWorldMap::COLLISIONTYPE collisiontype)
{
    ASSERT(pIntersection);
    ASSERT(pCollisionParam);

    if (pCollisionParam->m_checkflag & CWorldMap::CHECKFLAG_MAPONLY)
    {
        CheckWorldMapCollision(pIntersection, pCollisionParam, collisiontype);
    }
    else
    {
        CheckWorldObjCollision(pIntersection, pCollisionParam, collisiontype);
        CheckWorldMapCollision(pIntersection, pCollisionParam, collisiontype);
    };
};


bool CWorldMapManager::CheckCollision(RpIntersection* pIntersection, RwV3d* pVel, CWorldMap::CHECKFLAG checkflag)
{
    ASSERT(pIntersection);
    
    CMapCollision::COLLISIONPARAM CollisionParam;    
    std::memset(&CollisionParam, 0x00, sizeof(CollisionParam));

    CollisionParam.m_type               = MAPTYPES::HITTYPE_NONE;
    CollisionParam.m_checkflag          = checkflag;
    CollisionParam.m_fDistance          = TYPEDEF::FLOAT_MAX;
    CollisionParam.m_mapwall.m_fDistance= TYPEDEF::FLOAT_MAX;

    if (pVel)
        CollisionParam.m_pvVelocity = pVel;

    CheckWorldCollision(pIntersection, &CollisionParam, CWorldMap::COLLISIONTYPE_NONE);

    if (CollisionParam.m_fDistance >= TYPEDEF::FLOAT_MAX)
        return false;

    std::memcpy(&m_CollisionResult, &CollisionParam, sizeof(CWorldMap::COLLISIONPARAM_BASE));
    
    return true;
};


bool CWorldMapManager::CheckCollisionCharacterMove(RwV3d* pPos,
                                                   RwV3d* pNewPos,
                                                   RwV3d* pVel,
                                                   CWorldMap::MOVE_HIT_FLAG* hitFlag,
                                                   float fRadius,
                                                   float fHeight)
{
    ASSERT(pPos);
    ASSERT(pNewPos);
    ASSERT(pVel);
    ASSERT(hitFlag);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3d vOldPosition = *pPos;

    CWorldMap::COLLISIONPARAM_MAPWALL CollisionParamWall;
    std::memset(&CollisionParamWall, 0x00, sizeof(CollisionParamWall));

    RwV3d aOfsPos[4] = { Math::VECTOR3_ZERO };
    SetCharacterMoveSphereOfPos(aOfsPos, fRadius, fHeight);

    bool bFound = false;
    for (int32 i = 0; i < 4; ++i)
    {
        RwV3d vVelocity = *pVel;

        vPosition.x = (pPos->x + vVelocity.x + aOfsPos[i].x);
        vPosition.y = (pPos->y + vVelocity.y + aOfsPos[i].y);
        vPosition.z = (pPos->z + vVelocity.z + aOfsPos[i].z);

        if (CheckCollisionMoveSphere(&vPosition, fRadius, &vVelocity, CWorldMap::CHECKFLAG_WALLJUMP))
        {
            bFound = true;
            break;
        };
    };

    if (!bFound)
    {
        Math::Vec3_Add(pNewPos, pPos, pVel);
        *hitFlag = CWorldMap::MOVE_HIT_FLAG_NONE;
        return false;
    };

    const CWorldMap::COLLISIONRESULT* pCollisionResult = GetCollisionResult();
    ASSERT(pCollisionResult != nullptr);

    if (pCollisionResult->m_attribute != MAPTYPES::ATTRIBUTE_JUMP)
        std::memcpy(&CollisionParamWall, &pCollisionResult->m_mapwall, sizeof(CollisionParamWall));

    if (pCollisionResult->m_vNormal.y >= -0.8f)
    {
        *hitFlag = CWorldMap::MOVE_HIT_FLAG_XZ;

        pNewPos->x = vPosition.x;
        pNewPos->y = vOldPosition.y + pVel->y;
        pNewPos->z = vPosition.z;
    }
    else
    {
        *hitFlag = CWorldMap::MOVE_HIT_FLAG_Y;

        *pNewPos = vOldPosition;

        if (pVel->y < 0.0f)
            pNewPos->y += pVel->y;
    };

    vPosition.x = pNewPos->x;
    vPosition.y = pNewPos->y + (fRadius + 0.1f);
    vPosition.z = pNewPos->z;

    if (CheckCollisionMoveSphere(&vPosition, fRadius))
    {
        pCollisionResult = GetCollisionResult();

        pNewPos->x = vPosition.x;
        pNewPos->z = vPosition.z;

        if (!(*hitFlag == CWorldMap::MOVE_HIT_FLAG_Y) || (pVel->y < 0.0f))
            pNewPos->y = vOldPosition.y + pVel->y;
        else
            pNewPos->y = vOldPosition.y;

        RwV3d vNormalBefore = pCollisionResult->m_vNormal;

        vPosition.x = pNewPos->x;
        vPosition.y = pNewPos->y + (fRadius + 0.1f);
        vPosition.z = pNewPos->z;

        if (CheckCollisionMoveSphere(&vPosition, fRadius))
        {
            pCollisionResult = GetCollisionResult();
            ASSERT(pCollisionResult != nullptr);

            pNewPos->x = vOldPosition.x;
            pNewPos->z = vOldPosition.z;

            RwV3d vNormalAfter = pCollisionResult->m_vNormal;

            vPosition.x = pNewPos->x;
            vPosition.y = pNewPos->y;
            vPosition.z = pNewPos->z;

            if (CheckCollisionMoveSphere(&vPosition, fRadius))
            {
                pCollisionResult = GetCollisionResult();
                ASSERT(pCollisionResult != nullptr);

                RwV3d vNormal = Math::VECTOR3_ZERO;

                vNormal.x = vNormalAfter.x + vNormalBefore.x;
                vNormal.y = vNormalAfter.y + vNormalBefore.y;
                vNormal.z = vNormalAfter.z + vNormalBefore.z;

                Math::Vec3_Normalize(&vNormal, &vNormal);

                vNormal.x *= (fRadius * 0.25f);
                vNormal.y *= (fRadius * 0.25f);
                vNormal.z *= (fRadius * 0.25f);

                pNewPos->x += vNormal.x;
                pNewPos->z += vNormal.z;

                vPosition.x = pNewPos->x;
                vPosition.y = pNewPos->y;
                vPosition.z = pNewPos->z;

                if (CheckCollisionMoveSphere(&vPosition, fRadius))
                {
                    pNewPos->x += vNormal.x;
                    pNewPos->z += vNormal.z;
                };
            };
        };
    };

    ASSERT(*hitFlag != CWorldMap::MOVE_HIT_FLAG_NONE);

    if (CollisionParamWall.m_bHit)
        std::memcpy(&m_CollisionResult.m_mapwall, &CollisionParamWall, sizeof(m_CollisionResult.m_mapwall));

    return true;
};


bool CWorldMapManager::CheckCollisionCharacterHeight(RwV3d* pPos, RwV3d* pNewPos, float* pfRotY, float fRadius, float fHeight, bool bLimitNormalY)
{
    ASSERT(pPos);
    ASSERT(pNewPos);
    ASSERT(pfRotY);

    bool bResult = false;

    float fCheckHeight = fHeight * 0.8f;
    float fCheckRadius = fRadius;

    RwV3d vPos = Math::VECTOR3_ZERO;
    vPos.x = pPos->x;
	vPos.y = pPos->y + fCheckHeight;
    vPos.z = pPos->z;

    if (!CheckCollisionHeight(&vPos, fCheckRadius, -fCheckHeight))
        return bResult;

    const CWorldMap::COLLISIONRESULT* pCollisionResult = GetCollisionResult();
    ASSERT(pCollisionResult != nullptr);

    while (pPos->y <= pCollisionResult->m_vClosestPt.y)
    {
        if ((pCollisionResult->m_vNormal.y >= 0.5f) || !bLimitNormalY)
        {
            pNewPos->x = pPos->x;
            pNewPos->y = pCollisionResult->m_vClosestPt.y;
            pNewPos->z = pPos->z;

            if (pCollisionResult->m_type == MAPTYPES::HITTYPE_GIMMICK)
            {
                if (pCollisionResult->m_mapobj.m_pLTM && !Math::Vec3_IsEqual(&pCollisionResult->m_mapobj.m_vRotate, &Math::VECTOR3_ZERO))
                {
                    RwMatrix invLTM;
                    RwMatrixSetIdentityMacro(&invLTM);
                    RwMatrixInvert(&invLTM, pCollisionResult->m_mapobj.m_pLTM);

                    RwV3d vLocalPos = Math::VECTOR3_ZERO;
                    RwV3dTransformPoint(&vLocalPos, pNewPos, &invLTM);

                    RwMatrix matrix;
                    RwMatrixSetIdentityMacro(&matrix);
                    Math::Matrix_Rotate(&matrix, &pCollisionResult->m_mapobj.m_vRotate);
                    
					RwV3d vAfterPos = Math::VECTOR3_ZERO;
                    RwV3dTransformPoint(&vAfterPos, &vLocalPos, &matrix);
                    RwV3dTransformPoint(&vAfterPos, &vAfterPos, pCollisionResult->m_mapobj.m_pLTM);

                    *pNewPos = vAfterPos;

                    if (pfRotY)
                        *pfRotY += pCollisionResult->m_mapobj.m_vRotate.y;
                };

                Math::Vec3_Add(pNewPos, pNewPos, &pCollisionResult->m_mapobj.m_vVelocity);
            };

            if (pCollisionResult->m_attribute == MAPTYPES::ATTRIBUTE_SLIP)
            {
				pNewPos->x += ( pCollisionResult->m_vNormal.x * 0.07f);
				pNewPos->y += (-pCollisionResult->m_vNormal.y * 0.07f);
				pNewPos->z += ( pCollisionResult->m_vNormal.z * 0.07f);
            };

            bResult = true;
            break;
        };

        bLimitNormalY = false;

        fCheckHeight *= 0.8f;
        fCheckRadius *= 0.5f;

        vPos.x = pPos->x;
        vPos.y = pPos->y + fCheckHeight;
        vPos.z = pPos->z;

        if (!CheckCollisionHeight(&vPos, fCheckRadius, -fCheckHeight))
            break;
    };

    return bResult;
};


bool CWorldMapManager::CheckCollisionHeight(RwV3d* pPos, float fRadius, float fHeight)
{
    bool bFoundGround = false;

    CWorldMap::COLLISIONRESULT CollisionResult;
    std::memset(&CollisionResult, 0x00, sizeof(CollisionResult));

    CWorldMap::COLLISIONRESULT CollisionResultBest;
    std::memset(&CollisionResultBest, 0x00, sizeof(CollisionResultBest));
    CollisionResultBest.m_vClosestPt.y = -1000.0f;

    RwV3d vPtStart = Math::VECTOR3_ZERO;
    vPtStart = { pPos->x, pPos->y, pPos->z };

    RwV3d vPtEnd = Math::VECTOR3_ZERO;
    vPtEnd   = { pPos->x, pPos->y + fHeight, pPos->z };

    if (CheckCollisionLine(&vPtStart, &vPtEnd))
    {
        std::memcpy(&CollisionResult, GetCollisionResult(), sizeof(CollisionResult));

        if (CollisionResult.m_vClosestPt.y > -1000.0f)
        {
            std::memcpy(&CollisionResultBest, &CollisionResult, sizeof(CollisionResultBest));
            bFoundGround = true;
        };
    };

    if (fRadius != 0.0f)
    {
        int32 nNumDivRadius = (fRadius > 0.4f) ? (1 + static_cast<int32>(fRadius / 0.4f)) : (1);
        float fCheckRadius = fRadius;

        for (int32 i = 0; i < nNumDivRadius; ++i)
        {
            int32 nNumAngle  = static_cast<int32>(MATH_PI2);
            float fAngleStep = (MATH_PI2 / 6.0f);

            ASSERT(nNumAngle == 6);
            ASSERT(Math::FEqual(fAngleStep, 1.0471976f));

            for (int32 j = 0; j < nNumAngle; ++j)
            {
                float fAngle = (static_cast<float>(j) * fAngleStep);

                vPtStart.x = Math::Cos(fAngle) * fCheckRadius + pPos->x;
                vPtStart.y = pPos->y;
                vPtStart.z = Math::Sin(fAngle) * fCheckRadius + pPos->z;

                vPtEnd.x = vPtStart.x;
                vPtEnd.y = pPos->y + fHeight;
                vPtEnd.z = vPtStart.z;

                if (CheckCollisionLine(&vPtStart, &vPtEnd))
                {
                    std::memcpy(&CollisionResult, GetCollisionResult(), sizeof(CollisionResult));

                    if (CollisionResultBest.m_vClosestPt.y < CollisionResult.m_vClosestPt.y)
                    {
                        std::memcpy(&CollisionResultBest, &CollisionResult, sizeof(CollisionResultBest));
                        bFoundGround = true;
                    };
                };
            };

            fCheckRadius -= 0.4f;
        };
    };

    if (bFoundGround)
        std::memcpy(&m_CollisionResult, &CollisionResultBest, sizeof(m_CollisionResult));

    return bFoundGround;
};


bool CWorldMapManager::CheckCollisionLine(const RwV3d* pPtStart, const RwV3d* pPtEnd)
{
    ASSERT(pPtStart);
    ASSERT(pPtEnd);

    RpIntersection intersection;
    intersection.type = rpINTERSECTLINE;
    intersection.t.line.start = *pPtStart;
    intersection.t.line.end = *pPtEnd;

    return CheckCollision(&intersection);
};


bool CWorldMapManager::CheckCollisionSphere(RwV3d* pPos, float fRadius)
{
    ASSERT(pPos);

    RpIntersection intersection;
    intersection.type = rpINTERSECTSPHERE;
    intersection.t.sphere = { { pPos->x, pPos->y, pPos->z }, fRadius };

    return CheckCollision(&intersection);
};


bool CWorldMapManager::CheckCollisionSphereMove(RwV3d* pPos, float fRadius, RwV3d* pVel, CWorldMap::CHECKFLAG checkflag)
{
    ASSERT(pPos);

    RpIntersection intersection;
    intersection.type     = rpINTERSECTSPHERE;
    intersection.t.sphere = { *pPos, fRadius };

    return CheckCollision(&intersection, pVel, checkflag);
};


bool CWorldMapManager::CheckCollisionMoveSphere(RwV3d* pPos, float fRadius, RwV3d* pVel, CWorldMap::CHECKFLAG checkflag)
{
    ASSERT(pPos);

    RpIntersection intersection;
    intersection.type     = rpINTERSECTSPHERE;
    intersection.t.sphere = { *pPos, fRadius };

    if (!CheckCollision(&intersection, pVel, checkflag))
        return false;

    const CWorldMap::COLLISIONRESULT* pCollisionResult = GetCollisionResult();
    ASSERT(pCollisionResult != nullptr);

    RwV3d vNormalXZ = Math::VECTOR3_ZERO;
    vNormalXZ.x = pCollisionResult->m_vNormal.x;
    vNormalXZ.z = pCollisionResult->m_vNormal.z;

    float fVecLen = Math::Vec3_Length(&vNormalXZ);
    if (fVecLen != 0.0f)
        Math::Vec3_Normalize(&vNormalXZ, &vNormalXZ);

    Math::Vec3_Scale(&vNormalXZ, &vNormalXZ, fRadius + 0.01f);
    Math::Vec3_Add(pPos, &pCollisionResult->m_vClosestPt, &vNormalXZ);
    
    if ((pCollisionResult->m_type == MAPTYPES::HITTYPE_GIMMICK) && pVel)
    {
        float fD = Math::Vec3_Dot(&pCollisionResult->m_mapobj.m_vVelocity, pVel);
        float fL = Math::Vec3_Length(&pCollisionResult->m_mapobj.m_vVelocity);

        if ((fL > 0.0f) && (fD <= 0.0f))
        {
            RwV3d vTmp = Math::VECTOR3_ZERO;
            Math::Vec3_Scale(&vTmp, &pCollisionResult->m_mapobj.m_vVelocity, 2.0f);
            Math::Vec3_Add(pPos, pPos, &vTmp);
        };
    };

    return true;
};


bool CWorldMapManager::GetCameraAreaName(RwV3d* pPos, char* pszCameraAreaName)
{
    ASSERT(pPos);
    ASSERT(pszCameraAreaName);

    CMapCollision::COLLISIONPARAM CollisionParam;
    std::memset(&CollisionParam, 0x00, sizeof(CollisionParam));

    CollisionParam.m_fDistance = TYPEDEF::FLOAT_MAX;

    RpIntersection intersection;
    intersection.type = rpINTERSECTLINE;
    intersection.t.line.start = { pPos->x, pPos->y + 20.0f, pPos->z };
    intersection.t.line.end   = { pPos->x, pPos->y - 20.0f, pPos->z };

    CheckWorldCollision(&intersection, &CollisionParam, CWorldMap::COLLISIONTYPE_MAPCAM);

    if (CollisionParam.m_fDistance >= TYPEDEF::FLOAT_MAX)
        return false;

    std::strcpy(pszCameraAreaName, CollisionParam.m_szAttributeName);

    return true;
};


float CWorldMapManager::GetMapHeight(const RwV3d* pPos)
{
    ASSERT(pPos);

    RwV3d vPtStart = *pPos;
    vPtStart.y += 1.0f;

    RwV3d vPtEnd = *pPos;
    vPtEnd.y += CWorldMap::INVALID_MAP_HEIGHT;

    if (!CheckCollisionLine(&vPtStart, &vPtEnd))
        return CWorldMap::INVALID_MAP_HEIGHT;

    return GetCollisionResult()->m_vClosestPt.y;
};


float CWorldMapManager::GetMapHeight(const RwV3d* pPos, float fRadius)
{
    ASSERT(pPos);

    RwV3d vPtStart = { pPos->x, pPos->y + 1.0f, pPos->z };
    
    if (!CheckCollisionHeight(&vPtStart, fRadius, CWorldMap::INVALID_MAP_HEIGHT * 2.0f))
        return CWorldMap::INVALID_MAP_HEIGHT;

    return GetCollisionResult()->m_vClosestPt.y;
};


void CWorldMapManager::SetCharacterMoveSphereOfPos(RwV3d aOfsPos[4], float fRadius, float fHeight, RwMatrix* pMatrix)
{
    float fStart = fHeight - fRadius;
    float fEnd = fRadius + 0.1f;

    if ((fRadius + fRadius + 0.1f) > fHeight)
        fStart = fEnd;

    for (int32 i = 0; i < 4; ++i)
    {
        aOfsPos[i] = Math::VECTOR3_ZERO;

        float fDelta = (fEnd - fStart) * 0.25f;
        aOfsPos[i].y = fStart + (static_cast<float>(i) * fDelta);

        if (pMatrix)
            RwV3dTransformPoint(&aOfsPos[i], &aOfsPos[i], pMatrix);
    };
};


static CWorldMapManager* s_pWorldMapManager = nullptr;


static inline CWorldMapManager& WorldMapManager(void)
{
    ASSERT(s_pWorldMapManager);
    return *s_pWorldMapManager;
};


/*static*/ const float CWorldMap::INVALID_MAP_HEIGHT = -100.0f;


/*static*/ void CWorldMap::Initialize(void)
{
    if (!s_pWorldMapManager)
    {
        CCameraDataManager::Initialize();
        CMapCollisionModel::Initialize();
        CUVAnim::Initialize();
        CMapClumpModelManager::Initialize();
        
        s_pWorldMapManager = new CWorldMapManager;
    };
};


/*static*/ void CWorldMap::Terminate(void)
{
    if (s_pWorldMapManager)
    {
        delete s_pWorldMapManager;
        s_pWorldMapManager = nullptr;

        CMapClumpModelManager::Terminate();
        CUVAnim::Terminate();
        CMapCollisionModel::Terminate();
        CCameraDataManager::Terminate();

        CRenderStateManager::ClearFogParam();
        CScreen::ResetClearColor();
    };
};


/*static*/ void CWorldMap::Period(void)
{
    WorldMapManager().Period();
    CMapCollisionModel::Period();
};


/*static*/ void CWorldMap::Draw(DRAWTYPE drawtype)
{
    WorldMapManager().Draw(drawtype);
};


/*static*/ void CWorldMap::OnLoaded(void)
{
    CMapClumpModelManager::CreateModel();
    WorldMapManager().OnLoaded();
};


/*static*/ void CWorldMap::ReadBsp(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    CTextureManager::SetCurrentTextureSet("maptex");

    if (!std::strcmp(pszName, "world"))
    {
        WorldMapManager().ReadWorldBsp(pBuffer, uBufferSize);
    }
    else if (!std::strcmp(pszName, "collision"))
    {
        WorldMapManager().ReadCollisionBsp(pBuffer, uBufferSize);
    }
    else if (!std::strcmp(pszName, "cameraarea"))
    {
        WorldMapManager().ReadCameraAreaBsp(pBuffer, uBufferSize);
    }
    else
    {
        ASSERT(false);
    };
};


/*static*/ void CWorldMap::ReadMapInfo(void* pBuffer, uint32 uBufferSize)
{
    WorldMapManager().ReadMapInfo(pBuffer, uBufferSize);
};


/*static*/ const CWorldMap::MAPINFO* CWorldMap::GetMapInfo(void)
{
    return WorldMapManager().GetMapInfo();
};


/*static*/ void CWorldMap::GetWaterSurfacePosition(RwV3d* pPos)
{
    const MAPINFO* pMapInfo = WorldMapManager().GetMapInfo();
    if (pMapInfo)
    {
        if (pMapInfo->m_waterinfo.m_bOffset)
        {
            pPos->y += pMapInfo->m_waterinfo.m_fWaterSurfaceHeight;
        }
        else
        {
            pPos->y = pMapInfo->m_waterinfo.m_fWaterSurfaceHeight;
        };
    };
};


/*static*/ float CWorldMap::GetCharacterDeathHeight(void)
{
	if (s_pWorldMapManager && s_pWorldMapManager->GetMapInfo())
		return WorldMapManager().GetMapInfo()->m_fDeathHeight;
	
    return INVALID_MAP_HEIGHT;
};


/*static*/ RpWorld* CWorldMap::GetWorld(void)
{
    return WorldMapManager().GetWorld();
};


/*static*/ void CWorldMap::SetAmbientLightColor(const RwRGBAReal& rColor)
{
    WorldMapManager().SetAmbientLightColor(rColor);
};


/*static*/ void CWorldMap::SetDirectionLightColor(const RwRGBAReal& rColor)
{
    WorldMapManager().SetDirectionLightColor(rColor);
};


/*static*/ void CWorldMap::SetDirectionLightMatrix(RwMatrix* pMatrix)
{
    WorldMapManager().SetDirectionLightMatrix(pMatrix);
};


/*static*/ bool CWorldMap::CheckCollisionLine(const RwV3d* pPtStart, const RwV3d* pPtEnd)
{
    return WorldMapManager().CheckCollisionLine(pPtStart, pPtEnd);
};


/*static*/ bool CWorldMap::CheckCollisionSphere(RwV3d* pPos, float fRadius)
{
    return WorldMapManager().CheckCollisionSphere(pPos, fRadius);
};


/*static*/ bool CWorldMap::CheckCollisionSphereMove(RwV3d* pPos, float fRadius, RwV3d* pVel, CHECKFLAG checkflag)
{
    return WorldMapManager().CheckCollisionSphereMove(pPos, fRadius, pVel, checkflag);
};


/*static*/ bool CWorldMap::CheckCollisionMoveSphere(RwV3d* pPos, float fRadius, RwV3d* pVel, CHECKFLAG checkflag)
{
    return WorldMapManager().CheckCollisionMoveSphere(pPos, fRadius, pVel, checkflag);
};


/*static*/ bool CWorldMap::GetCameraAreaName(RwV3d* pPos, char* pszCameraAreaName)
{
    return WorldMapManager().GetCameraAreaName(pPos, pszCameraAreaName);
};


/*static*/ bool CWorldMap::CheckCollisionHeight(RwV3d* pPos, float fRadius, float fHeight)
{
    return WorldMapManager().CheckCollisionHeight(pPos, fRadius, fHeight);    
};


/*static*/ float CWorldMap::GetMapHeight(const RwV3d* pPos)
{
    return WorldMapManager().GetMapHeight(pPos);
};


/*static*/ float CWorldMap::GetMapHeight(const RwV3d* pPos, float fRadius)
{
    return WorldMapManager().GetMapHeight(pPos, fRadius);
};


/*static*/ const CWorldMap::COLLISIONRESULT* CWorldMap::GetCollisionResult(void)
{
    return WorldMapManager().GetCollisionResult();
};


/*static*/ const RwV3d* CWorldMap::GetCollisionResultClosestPoint(void)
{
    return &WorldMapManager().GetCollisionResult()->m_vClosestPt;
};


/*static*/ const RwV3d* CWorldMap::GetCollisionResultNormal(void)
{
    return &WorldMapManager().GetCollisionResult()->m_vNormal;
};


/*static*/ MAPTYPES::ATTRIBUTE CWorldMap::GetCollisionResultAttribute(void)
{
    return WorldMapManager().GetCollisionResult()->m_attribute;
};


/*static*/ bool CWorldMap::CheckCollisionCharacterMove(RwV3d* pPos,
                                                       RwV3d* pNewPos,
                                                       RwV3d* pVel,
                                                       MOVE_HIT_FLAG* hitFlag,
                                                       float fRadius,
                                                       float fHeight)
{
    ASSERT(pPos);
    ASSERT(pNewPos);
    ASSERT(pVel);
    ASSERT(fRadius > 0.0f);

    float fVecLen = Math::Vec3_Length(pVel);
    if (fVecLen < fRadius)
        return WorldMapManager().CheckCollisionCharacterMove(pPos, pNewPos, pVel, hitFlag, fRadius, fHeight);

    float fDivVelocityScale = (fVecLen / fRadius);
    int32 nCheck = (static_cast<int32>(fDivVelocityScale) + 1);

    if (nCheck <= 0)
        return false;

    RwV3d vVelocityStep = Math::VECTOR3_ZERO;
    vVelocityStep.x = pVel->x * (1.0f / static_cast<float>(nCheck));
    vVelocityStep.y = pVel->y * (1.0f / static_cast<float>(nCheck));
    vVelocityStep.z = pVel->z * (1.0f / static_cast<float>(nCheck));

	RwV3d vPosition = *pPos;

    for (int32 i = nCheck; i > 0; --i)
    {
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        if (i == 1)
        {
            vVelocity.x = pVel->x - (vVelocityStep.x * fDivVelocityScale);
            vVelocity.y = pVel->y - (vVelocityStep.y * fDivVelocityScale);
            vVelocity.z = pVel->z - (vVelocityStep.z * fDivVelocityScale);
        }
        else
        {
            vVelocity = vVelocityStep;
        };

        if (WorldMapManager().CheckCollisionCharacterMove(&vPosition, pNewPos, &vVelocity, hitFlag, fRadius, fHeight))
            return true;

        Math::Vec3_Add(&vPosition, &vPosition, &vVelocity);
    };

    return false;
};


/*static*/ bool CWorldMap::CheckCollisionCharacterHeight(RwV3d* pPos, RwV3d* pNewPos, float* pfRotY, float fRadius, float fHeight)
{
    return WorldMapManager().CheckCollisionCharacterHeight(pPos, pNewPos, pfRotY, fRadius, fHeight, true);
};