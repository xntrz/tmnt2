#include "EventCheckGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GameEvent.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/Component/Gimmick/GimmickInfo.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/SeGroupID.hpp"
#include "Game/System/Hit/Intersection.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/DebugShape.hpp"


CSimpleEventCheckGimmick::CSimpleEventCheckGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_privatestate(PRIVATESTATE_CHECK)
{
    m_szTargetName[0] = '\0';
};


CSimpleEventCheckGimmick::~CSimpleEventCheckGimmick(void)
{
    ;
};


void CSimpleEventCheckGimmick::PostMove(void)
{
    switch (m_privatestate)
    {
    case PRIVATESTATE_CHECK:
        {
            if (CheckEvent())
            {
                CGimmickManager::PostEvent(
                    m_szTargetName,
                    GetName(),
                    GIMMICKTYPES::EVENTTYPE_ACTIVATE
                );
                
                m_privatestate = PRIVATESTATE_END;
            };
        }
        break;
    };
};


void CSimpleEventCheckGimmick::Init(const char* pszTarget)
{
    ASSERT(pszTarget);
    ASSERT(std::strlen(pszTarget) < COUNT_OF(m_szTargetName));
    
    std::strcpy(m_szTargetName, pszTarget);
};


//
// *********************************************************************************
//


CTimeCheckGimmick::CTimeCheckGimmick(const char* pszName, void* pParam)
: CSimpleEventCheckGimmick(pszName, pParam)
, m_fTime(0.0f)
, m_fTargetTime(0.0f)
, m_bIsCounting(false)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_TERMS_TIME* pTimeParam = (GIMMICKPARAM::GIMMICK_TERMS_TIME*)pParam;

    Init(pTimeParam->m_szTargetGimmick);
    m_fTargetTime = pTimeParam->m_fTime;
};


CTimeCheckGimmick::~CTimeCheckGimmick(void)
{
    ;
};


void CTimeCheckGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    switch (eventtype)
    {
    case GIMMICKTYPES::EVENTTYPE_ACTIVATE:
        m_bIsCounting = true;
        break;

    case GIMMICKTYPES::EVENTTYPE_INACTIVATE:
        m_bIsCounting = false;
        break;
    };
};


bool CTimeCheckGimmick::CheckEvent(void)
{
    if (m_bIsCounting)
        m_fTime += CGameProperty::GetElapsedTime();

    return (m_fTime >= m_fTargetTime);
};


float CTimeCheckGimmick::GetRemainTime(void) const
{
    float fResult = (m_fTargetTime - m_fTime);
    if (fResult < 0.0f)
        fResult = 0.0f;
    
    return fResult;
};


float CTimeCheckGimmick::GetTargetTime(void) const
{
    return m_fTargetTime;
};


//
// *********************************************************************************
//


CEnemyCheckGimmick::CEnemyCheckGimmick(const char* pszName, void* pParam)
: CSimpleEventCheckGimmick(pszName, pParam)
, m_mode(MODE_DEFEAT_ALL)
, m_nTargetDefeatNum(0)
, m_bCheckCompleteFlag(false)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_TERMS_GENERATOR* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS_GENERATOR*)pParam;

    Init(pInitParam->m_szTargetGimmick);
    std::strcpy(m_szGeneratorName, pInitParam->m_szObserveGeneratorName);

    if (pInitParam->m_id == GIMMICKID::ID_E_EBRKAL)
    {
        m_mode = MODE_DEFEAT_ALL;
        m_nTargetDefeatNum = 0;
    }
    else
    {
        m_mode = MODE_DEFEAT_COUNT;
        m_nTargetDefeatNum = pInitParam->m_nNumTermsDeathEnemy;
    };
};


CEnemyCheckGimmick::~CEnemyCheckGimmick(void)
{
    ;
};


bool CEnemyCheckGimmick::Query(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);

    if (pQuery->GetType() != GIMMICKTYPES::QUERY_SYS_SWITCH)
        return CGimmick::Query(pQuery);

    CStateGimmickQuery* pStateQuery = (CStateGimmickQuery*)pQuery;
    ++pStateQuery->m_nTarget;

    if(!m_bCheckCompleteFlag)
        return CGimmick::Query(pQuery);

    ++pStateQuery->m_nState;
    return true;
};


bool CEnemyCheckGimmick::CheckEvent(void)
{
    CGimmick* pWatchTarget = CGimmickManager::Find(m_szGeneratorName);
    if (!pWatchTarget)
        return false;

    int32 nTargetNum = m_nTargetDefeatNum;

    if (m_mode == MODE_DEFEAT_ALL)
    {
        CCounterGimmickQuery queryTotalNum(GIMMICKTYPES::QUERY_GEN_ENEMY_MAX);
        pWatchTarget->Query(&queryTotalNum);
        nTargetNum = queryTotalNum.m_nCounter;
    };

    CCounterGimmickQuery queryDefeatNum(GIMMICKTYPES::QUERY_GEN_DEAD_ENEMY_NUM);
	pWatchTarget->Query(&queryDefeatNum);
    if (queryDefeatNum.m_nCounter < nTargetNum)
        return false;

    m_bCheckCompleteFlag = true;
    return true;
};


//
// *********************************************************************************
//


/*static*/ void CAreaCheckGimmick::getPlaneNormal(RwV3d* pvV0, RwV3d* pvV1, RwV3d* pvV2, RwV3d* pvNormal)
{
    RwV3d vVec0 = Math::VECTOR3_ZERO;
    RwV3d vVec1 = Math::VECTOR3_ZERO;
    
    Math::Vec3_Sub(&vVec0, pvV1, pvV0);
    Math::Vec3_Sub(&vVec1, pvV2, pvV0);
    Math::Vec3_Cross(pvNormal, &vVec0, &vVec1);
    Math::Vec3_Normalize(pvNormal, pvNormal);
};


/*static*/ bool CAreaCheckGimmick::isPointReverseSidePlane(RwV3d* pvPos, RwV3d* pvPlanePos, RwV3d* pvNormal)
{
    RwV3d vTmp = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vTmp, pvPlanePos, pvPos);
    return Math::Vec3_Dot(pvNormal, &vTmp) <= 0.0f;
};


CAreaCheckGimmick::CAreaCheckGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_areatype(AREATYPE_WALL)
, m_numbertype(NUMBERTYPE_SOMEONE)
, m_bInsideAreaSomeone(false)
, m_bInsideAreaAll(false)
, m_sphere({ 0 })    
{
    std::memset(m_aWallVertex, 0x00, sizeof(m_aWallVertex));
    std::memset(m_aBoxVertex, 0x00, sizeof(m_aBoxVertex));

    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS*)pParam;
    
    ASSERT(std::strlen(pInitParam->m_szTargetGimmick) < COUNT_OF(m_szTargetGimmickName));
    std::strcpy(m_szTargetGimmickName, pInitParam->m_szTargetGimmick);
};


CAreaCheckGimmick::~CAreaCheckGimmick(void)
{
    ;
};


void CAreaCheckGimmick::Draw(void) const
{
#ifdef _DEBUG
    if (CGimmickDebug::SHOW_AREA)
    {
        switch (m_areatype)
        {
        case AREATYPE_SPHERE:
            {
                CDebugShape::ShowSphere(&m_sphere, CGimmickDebug::COLOR_AREA_SPHERE);
            }
            break;

        case AREATYPE_BOX:
            {
                CDebugShape::ShowBox(m_aBoxVertex, CGimmickDebug::COLOR_AREA_BOX);
            }
            break;

        case AREATYPE_WALL:
            {
                CDebugShape::ShowPlane(m_aWallVertex, CGimmickDebug::COLOR_AREA_WALL);

                RwV3d vNormal = Math::VECTOR3_ZERO;
                
                RwV3d aVertexList[3] =
                {
                    m_aWallVertex[0],
                    m_aWallVertex[2],
                    m_aWallVertex[1],
                };
                
                getPlaneNormal(
                    &aVertexList[0],
                    &aVertexList[2],
                    &aVertexList[1],
                    &vNormal
                );

                RwLine line = { 0 };
                GetPosition(&line.start);

                line.start.y =
                    m_aWallVertex[0].y + ((m_aWallVertex[1].y - m_aWallVertex[0].y) * 0.5f);
                line.end = vNormal;

                Math::Vec3_Scale(&line.end, &line.end, 3.0f);
                Math::Vec3_Add(&line.end, &line.end, &line.start);

                CDebugShape::ShowLine(
                    &line,
                    CGimmickDebug::COLOR_AREA_WALL,
                    CGimmickDebug::COLOR_AREA_WALL
                );
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
#endif
	CGimmick::Draw();
};


void CAreaCheckGimmick::PostMove(void)
{
    switch (m_numbertype)
    {
    case NUMBERTYPE_ALL:
        {
            if (!CheckArea())
            {
                if (m_bInsideAreaAll)
                {
                    if (std::strcmp(m_szTargetGimmickName, ""))
                    {
                        CGimmickManager::PostEvent(
                            m_szTargetGimmickName,
                            GetName(),
                            GIMMICKTYPES::EVENTTYPE_ACTIVATE
                        );
                    };
                };

                m_bInsideAreaAll = false;
            }
            else
            {
                if (!m_bInsideAreaAll)
                {
                    if (std::strcmp(m_szTargetGimmickName, ""))
                    {
                        CGimmickManager::PostEvent(
                            m_szTargetGimmickName,
                            GetName(),
                            GIMMICKTYPES::EVENTTYPE_ACTIVATE
                        );
                    };
                };

                m_bInsideAreaAll = true;
            };
        }
        break;

    case NUMBERTYPE_SOMEONE:
        {
            if (!m_bInsideAreaSomeone)
            {
                if (CheckArea())
                {
                    if (std::strcmp(m_szTargetGimmickName, ""))
                    {
                        CGimmickManager::PostEvent(
                            m_szTargetGimmickName,
                            GetName(),
                            GIMMICKTYPES::EVENTTYPE_ACTIVATE
                        );
                    };
                    
                    m_bInsideAreaSomeone = true;
                };
            };
        }
        break;
    };
};


bool CAreaCheckGimmick::Query(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);
    
    if (pQuery->GetType() != GIMMICKTYPES::QUERY_SYS_SWITCH)
        return CGimmick::Query(pQuery);

    CStateGimmickQuery* pStateQuery = (CStateGimmickQuery*)pQuery;
    bool bStatus = false;
    
    switch (m_numbertype)
    {
    case NUMBERTYPE_ALL:
        ++pStateQuery->m_nTarget;
        bStatus = m_bInsideAreaAll;
        break;

    case NUMBERTYPE_SOMEONE:
        ++pStateQuery->m_nTarget;
        bStatus = m_bInsideAreaSomeone;
        break;

    default:
        ASSERT(false);
        break;
    };

    if (!bStatus)
        return CGimmick::Query(pQuery);

    ++pStateQuery->m_nState;
    
    return true;
};


bool CAreaCheckGimmick::IsInsideArea(RwV3d* pvPosition)
{
    bool bResult = false;
    
    switch (m_areatype)
    {
    case AREATYPE_SPHERE:
        bResult = isInsideSphere(pvPosition);
        break;

    case AREATYPE_WALL:
        bResult = isReverseSideWall(pvPosition);
        break;

    case AREATYPE_BOX:
        bResult = isInsideBox(pvPosition);
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};


void CAreaCheckGimmick::SetAreaParameter(AREATYPE areatype, void* pParam)
{
    ASSERT(pParam);
    
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;

    RwMatrix matrixRotation;
    RwMatrixSetIdentityMacro(&matrixRotation);
    CGimmickUtils::QuaternionToRotationMatrix(&matrixRotation, &pBasicParam->m_quat);

    m_areatype = areatype;

    switch (m_areatype)
    {
    case AREATYPE_SPHERE:
        {
            GIMMICKPARAM::GIMMICK_TERMS_AREA_SPHERE* pSphereParam = (GIMMICKPARAM::GIMMICK_TERMS_AREA_SPHERE*)pParam;
            m_sphere.center = pSphereParam->m_vPosition;
            m_sphere.radius = pSphereParam->m_fRadius;
        }
        break;

    case AREATYPE_WALL:
        {
            GIMMICKPARAM::GIMMICK_TERMS_AREA_WALL* pWallParam = (GIMMICKPARAM::GIMMICK_TERMS_AREA_WALL*)pParam;
            RwV3d aWallVertex[4] = { 0 };

            static_assert(COUNT_OF(aWallVertex) == COUNT_OF(m_aWallVertex), "update me");

            aWallVertex[0].x = pWallParam->m_wall.x * -0.5f;
            aWallVertex[0].y = pWallParam->m_wall.y;
            aWallVertex[0].z = 0.0f;

            aWallVertex[1].x = pWallParam->m_wall.x * -0.5f;
            aWallVertex[1].y = 0.0f;
            aWallVertex[1].z = 0.0f;

            aWallVertex[2].x = pWallParam->m_wall.x * 0.5f;
            aWallVertex[2].y = pWallParam->m_wall.y;
            aWallVertex[2].z = 0.0f;

            aWallVertex[3].x = pWallParam->m_wall.x * 0.5f;
            aWallVertex[3].y = 0.0f;
            aWallVertex[3].z = 0.0f;

            RwMatrix matrix;
            RwMatrix matrixTranslate;
            RwMatrixSetIdentityMacro(&matrix);
            RwMatrixSetIdentityMacro(&matrixTranslate);
            
            RwMatrixTranslate(
                &matrixTranslate,
                &pWallParam->m_vPosition,
                rwCOMBINEREPLACE
            );
            
            Math::Matrix_Multiply(
                &matrix,
                &matrixRotation,
                &matrixTranslate
            );
            
            RwV3dTransformPoints(m_aWallVertex, aWallVertex, COUNT_OF(aWallVertex), &matrix);
        }
        break;

    case AREATYPE_BOX:
        {
            GIMMICKPARAM::GIMMICK_TERMS_AREA_BOX* pBoxParam = (GIMMICKPARAM::GIMMICK_TERMS_AREA_BOX*)pParam;
            RwV3d aBoxVertex[8] = { 0 };

            static_assert(COUNT_OF(aBoxVertex) == COUNT_OF(m_aBoxVertex), "update me");

            aBoxVertex[0].x = pBoxParam->m_box.x * -0.5f;
            aBoxVertex[0].y = pBoxParam->m_box.y;
            aBoxVertex[0].z = pBoxParam->m_box.z * -0.5f;
            
            aBoxVertex[1].x = pBoxParam->m_box.x * -0.5f;
            aBoxVertex[1].y = 0.0f;
            aBoxVertex[1].z = pBoxParam->m_box.z * -0.5f;
            
            aBoxVertex[2].x = pBoxParam->m_box.x * 0.5f;
            aBoxVertex[2].y = pBoxParam->m_box.y;
            aBoxVertex[2].z = pBoxParam->m_box.z * -0.5f;
            
            aBoxVertex[3].x = pBoxParam->m_box.x * 0.5f;
            aBoxVertex[3].y = 0.0f;
            aBoxVertex[3].z = pBoxParam->m_box.z * -0.5f;
            
            aBoxVertex[4].x = pBoxParam->m_box.x * -0.5f;
            aBoxVertex[4].y = pBoxParam->m_box.y;
            aBoxVertex[4].z = pBoxParam->m_box.z * 0.5f;
            
            aBoxVertex[5].x = pBoxParam->m_box.x * -0.5f;
            aBoxVertex[5].y = 0.0f;
            aBoxVertex[5].z = pBoxParam->m_box.z * 0.5f;
            
            aBoxVertex[6].x = pBoxParam->m_box.x * 0.5f;
            aBoxVertex[6].y = pBoxParam->m_box.y;
            aBoxVertex[6].z = pBoxParam->m_box.z * 0.5f;
            
            aBoxVertex[7].x = pBoxParam->m_box.x * 0.5f;
            aBoxVertex[7].y = 0.0f;
            aBoxVertex[7].z = pBoxParam->m_box.z * 0.5f;

            RwMatrix matrix;
            RwMatrix matrixTranslate;
            RwMatrixSetIdentityMacro(&matrix);
            RwMatrixSetIdentityMacro(&matrixTranslate);

            RwMatrixTranslate(
                &matrixTranslate,
                &pBoxParam->m_vPosition,
                rwCOMBINEREPLACE
            );

            Math::Matrix_Multiply(
                &matrix,
                &matrixRotation,
                &matrixTranslate
            );

            RwV3dTransformPoints(m_aBoxVertex, aBoxVertex, COUNT_OF(aBoxVertex), &matrix);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


bool CAreaCheckGimmick::isInsideSphere(RwV3d* pvPosition)
{
    RwV3d vDistance = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vDistance, pvPosition, &m_sphere.center);
    
    float fDist = Math::Vec3_Length(&vDistance);
    
    return Math::FAbs(fDist) <= m_sphere.radius;
};


bool CAreaCheckGimmick::isInsideBox(RwV3d* pvPosition)
{
    static const int32 s_aBoxVertexIndex[] =
    {        
        4, 0, 2,
        1, 5, 3,
        6, 2, 3,
        0, 4, 1,
        5, 4, 6,
        2, 0, 1,
    };

    for (int32 i = 0; i < COUNT_OF(s_aBoxVertexIndex); i += 3)
    {
        ASSERT(i < COUNT_OF(s_aBoxVertexIndex));

        RwV3d vNormal = Math::VECTOR3_ZERO;
        
        getPlaneNormal(
            &m_aBoxVertex[s_aBoxVertexIndex[i + 0]],
            &m_aBoxVertex[s_aBoxVertexIndex[i + 1]],
            &m_aBoxVertex[s_aBoxVertexIndex[i + 2]],
            &vNormal
        );

        if (!isPointReverseSidePlane(pvPosition, &m_aBoxVertex[s_aBoxVertexIndex[i + 0]], &vNormal))
            return false;
    };

    return true;
};


bool CAreaCheckGimmick::isReverseSideWall(RwV3d* pvPosition)
{
    RwV3d vNormal = Math::VECTOR3_ZERO;

    getPlaneNormal(
        &m_aWallVertex[0],
        &m_aWallVertex[2],
        &m_aWallVertex[1],
        &vNormal
    );

    if (!isPointReverseSidePlane(pvPosition, &m_aWallVertex[0], &vNormal))
        return false;

    RwV3d* aTriVertex[3] = { 0 };
    aTriVertex[0] = &m_aWallVertex[0];
    aTriVertex[1] = &m_aWallVertex[1];
    aTriVertex[2] = &m_aWallVertex[2];

    float fDistPlane =
        Math::Vec3_Dot(&vNormal, &m_aWallVertex[0]) -
        Math::Vec3_Dot(&vNormal, pvPosition);

    RwV3d vPt = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vPt, &vNormal, fDistPlane);
    Math::Vec3_Add(&vPt, &vPt, pvPosition);

    if (Intersection::PointWithinTriangle(&vPt, aTriVertex, &vNormal))
        return true;

    aTriVertex[0] = &m_aWallVertex[2];
    aTriVertex[1] = &m_aWallVertex[1];
    aTriVertex[2] = &m_aWallVertex[3];

    return Intersection::PointWithinTriangle(&vPt, aTriVertex, &vNormal);
};


//
// *********************************************************************************
//


CPlayerAreaCheckGimmick::CPlayerAreaCheckGimmick(const char* pszName, void* pParam)
: CAreaCheckGimmick(pszName, pParam)
, m_bCheckAllFlag(false)
{
    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS*)pParam;

    AREATYPE areatype = AREATYPE_WALL;
    switch (pInitParam->m_id)
    {
    case GIMMICKID::ID_E_PWALL:
        areatype = AREATYPE_WALL;
        break;

    case GIMMICKID::ID_E_PSPHER:
        areatype = AREATYPE_SPHERE;
        break;

    case GIMMICKID::ID_E_PVOLUM:
        areatype = AREATYPE_BOX;
        break;

    default:
        ASSERT(false);
        break;
    };

    SetAreaParameter(areatype, pParam);

    switch (pInitParam->m_subid)
    {
    case 0:
        m_bCheckAllFlag = false;
        m_numbertype = NUMBERTYPE_SOMEONE;
        break;

    case 1:
        m_bCheckAllFlag = true;
        m_numbertype = NUMBERTYPE_SOMEONE;
        break;

    case 2:
        m_bCheckAllFlag = false;
        m_numbertype = NUMBERTYPE_ALL;
        break;

    case 3:
        m_bCheckAllFlag = true;
        m_numbertype = NUMBERTYPE_ALL;
        break;

    default:
        ASSERT(false);
        break;
    };
};


CPlayerAreaCheckGimmick::~CPlayerAreaCheckGimmick(void)
{
    ;
};


bool CPlayerAreaCheckGimmick::CheckArea(void)
{
    int32 nNumSuccessCheck = 0;
	int32 nPlayerNum = CGameProperty::GetPlayerNum();

    for (int32 i = 0; i < nPlayerNum; i++)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        
		IGamePlayer* pGamePlayer = CGameProperty::GetGamePlayer(i);
		pGamePlayer->GetPosition(&vPosition);
		pGamePlayer->Release();

        bool bResult = IsInsideArea(&vPosition);
        if (bResult)
        {
            ++nNumSuccessCheck;
            if (!m_bCheckAllFlag)
                return true;
        }
        else
        {
            if (m_bCheckAllFlag)
                return false;
        };
    };

    return (nNumSuccessCheck > 0);
};


//
// *********************************************************************************
//


CEnemyAreaCheckGimmick::CEnemyAreaCheckGimmick(const char* pszName, void* pParam)
: CAreaCheckGimmick(pszName, pParam)
{
    ASSERT(pParam);
    
    SetAreaParameter(AREATYPE_WALL, pParam);
    m_numbertype = NUMBERTYPE_SOMEONE;
};


CEnemyAreaCheckGimmick::~CEnemyAreaCheckGimmick(void)
{
    ;
};


bool CEnemyAreaCheckGimmick::CheckArea(void)
{
    bool bResult = false;
    
    CGameObject* pGameObject = CGameObjectManager::GetNext();
    while (pGameObject)
    {
        if (pGameObject->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CCharacter* pCharacter = (CCharacter*)pGameObject;
            if (pCharacter->GetCharacterType() == CCharacter::TYPE_ENEMY)
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                pCharacter->GetPosition(&vPosition);

                bResult = IsInsideArea(&vPosition);
                if (bResult)
                    break;
            };
        };

        pGameObject = CGameObjectManager::GetNext(pGameObject);
    };

    return bResult;
};


//
// *********************************************************************************
//


CGimmickObjAreaCheckGimmick::CGimmickObjAreaCheckGimmick(const char* pszName, void* pParam)
: CAreaCheckGimmick(pszName, pParam)
, m_idTarget(GIMMICKID::ID_INVALID)
{
    SetAreaParameter(AREATYPE_SPHERE, pParam);

    ASSERT(pParam);
    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS*)pParam;

    m_idTarget = (pInitParam->m_id != GIMMICKID::ID_E_RRSPHR ? GIMMICKID::ID_INVALID : GIMMICKID::ID_N_RAPROC);
    m_numbertype = NUMBERTYPE_SOMEONE;
};


CGimmickObjAreaCheckGimmick::~CGimmickObjAreaCheckGimmick(void)
{
    ;
};


bool CGimmickObjAreaCheckGimmick::CheckArea(void)
{
    bool bResult = false;

    CGameObject* pGameObject = CGameObjectManager::GetNext();
    while (pGameObject)
    {
        if (pGameObject->GetType() == GAMEOBJECTTYPE::GIMMICK)
        {
            CGimmick* pGimmick = (CGimmick*)pGameObject;
            if(pGimmick->GetID() == m_idTarget)
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                pGimmick->GetPosition(&vPosition);

                bResult = IsInsideArea(&vPosition);
                if (bResult)
                    break;
            };
        };

        pGameObject = CGameObjectManager::GetNext(pGameObject);
    };

    return bResult;
};


//
// *********************************************************************************
//


CFugitoidAreaCheckGimmick::CFugitoidAreaCheckGimmick(const char* pszName, void* pParam)
: CAreaCheckGimmick(pszName, pParam)
{
    SetAreaParameter(AREATYPE_WALL, pParam);
    
    m_numbertype = NUMBERTYPE_SOMEONE;
};


CFugitoidAreaCheckGimmick::~CFugitoidAreaCheckGimmick(void)
{
    ;
};


bool CFugitoidAreaCheckGimmick::CheckArea(void)
{
    bool bResult = false;

    CGameObject* pGameObject = CGameObjectManager::GetNext();
    while (pGameObject)
    {
        if (pGameObject->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CCharacter* pCharacter = (CCharacter*)pGameObject;
            if (pCharacter->GetCharacterType() == CCharacter::TYPE_ENEMY)
            {
                CCharacterCompositor* pEnemyCharacter = (CCharacterCompositor*)pCharacter;
                if (pEnemyCharacter->GetID() == ENEMYID::ID_FUGITOID)
                {
                    RwV3d vPosition = Math::VECTOR3_ZERO;
                    pEnemyCharacter->GetPosition(&vPosition);

                    bResult = IsInsideArea(&vPosition);
                    if (bResult)
                        break;
                };                
            };
        };

        pGameObject = CGameObjectManager::GetNext(pGameObject);
    };

    return bResult;
};


//
// *********************************************************************************
//


CBrokenCheckGimmick::CBrokenCheckGimmick(const char* pszName, void* pParam)
: CSimpleEventCheckGimmick(pszName, pParam)
{
    m_szObserveTarget[0] = '\0';

    ASSERT(pParam);
    GIMMICKPARAM::GIMMICK_TERMS_TARGET* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS_TARGET*)pParam;

    Init(pInitParam->m_szTargetGimmick);

    ASSERT(std::strlen(pInitParam->m_szObserveTargetName) < COUNT_OF(m_szObserveTarget));
    std::strcpy(m_szObserveTarget, pInitParam->m_szObserveTargetName);
};


CBrokenCheckGimmick::~CBrokenCheckGimmick(void)
{
    ;
};


bool CBrokenCheckGimmick::CheckEvent(void)
{
    CGimmick* pWatchTarget = CGimmickManager::Find(m_szObserveTarget);
    if (pWatchTarget)
    {
        CStateGimmickQuery queryBroken(GIMMICKTYPES::QUERY_EVE_BROKEN);
        pWatchTarget->Query(&queryBroken);
        
        if (queryBroken.m_nTarget == queryBroken.m_nState && queryBroken.m_nTarget > 0)
            return true;
    };

    return false;
};


//
// *********************************************************************************
//


CDonLaserGetCheckGimmick::CDonLaserGetCheckGimmick(const char* pszName, void* pParam)
: CSimpleEventCheckGimmick(pszName, pParam)
{
    ASSERT(pParam);
    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS*)pParam;

    Init(pInitParam->m_szTargetGimmick);
};


CDonLaserGetCheckGimmick::~CDonLaserGetCheckGimmick(void)
{
    ;
};


bool CDonLaserGetCheckGimmick::CheckEvent(void)
{
    return CGameData::Record().Secret().IsDonLazerEnabled();
};