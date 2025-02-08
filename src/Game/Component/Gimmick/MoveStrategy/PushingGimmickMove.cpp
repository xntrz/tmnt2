#include "PushingGimmickMove.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/Map/WorldMap.hpp"


CPushingGimmickMove::CPushingGimmickMove(void)
: m_vVelocity(Math::VECTOR3_ZERO)
, m_fRadius(0.0f)
, m_fSpeed(0.0f)
, m_vRotation(Math::VECTOR3_ZERO)
, m_vRotVelocity(Math::VECTOR3_ZERO)
, m_bRequestPush(false)
, m_pushstate(PUSHSTATE_PUSH)
, m_fallaxis(FALL_ROT_AXIS_X_P)
{
    m_szName[0] = '\0';
};


CPushingGimmickMove::~CPushingGimmickMove(void)
{
    ;
};


CPushingGimmickMove::RESULT CPushingGimmickMove::OnMove(float dt)
{
    RESULT Result = RESULT_NONE;
    
    switch (m_pushstate)
	{
    case PUSHSTATE_PUSH:
        {
            if (m_bRequestPush)
            {
                m_bRequestPush = false;

                RwV3d vSaveVelocity = m_vVelocity;
                RwV3d vDltPos = Math::VECTOR3_ZERO;
                RwV3d vNewPos = Math::VECTOR3_ZERO;

                vDltPos.x = (m_vVelocity.x * dt);
                vDltPos.y = (m_vVelocity.y * dt);
                vDltPos.z = (m_vVelocity.z * dt);

                vNewPos.x = m_vPosition.x + vDltPos.x;
                vNewPos.y = m_vPosition.y + vDltPos.y;
                vNewPos.z = m_vPosition.z + vDltPos.z;

                float fSphereRadius = m_fRadius - (m_fRadius / 3.0f);

                RwV3d vGimmickAt = Math::VECTOR3_ZERO;
                RwV3d vGimmickRight = Math::VECTOR3_ZERO;

                Math::Vec3_Normalize(&vGimmickAt, &m_vVelocity);

                RwMatrix matrix;
                RwMatrixSetIdentityMacro(&matrix);
                Math::Matrix_RotateY(&matrix, Math::PI05);
                RwV3dTransformPoint(&vGimmickRight, &vGimmickAt, &matrix);

                for (int32 i = 0; i < 3; ++i)
                {
                    RwV3d vPosition = Math::VECTOR3_ZERO;

                    vPosition.x = vNewPos.x + (vGimmickAt.x * fSphereRadius);
                    vPosition.y = vNewPos.y + (vGimmickAt.y * fSphereRadius);
                    vPosition.z = vNewPos.z + (vGimmickAt.z * fSphereRadius);

                    switch (i)
                    {
                    case 0:
                        {
                            ;
                        }
                        break;

                    case 1:
                        {
                            vPosition.x += (vGimmickRight.x * fSphereRadius);
                            vPosition.y += (vGimmickRight.y * fSphereRadius);
                            vPosition.z += (vGimmickRight.z * fSphereRadius);
                        }
                        break;

                    case 2:
                        {
                            vPosition.x += ((vGimmickRight.x * -1.0f) * fSphereRadius);
                            vPosition.y += ((vGimmickRight.y * -1.0f) * fSphereRadius);
                            vPosition.z += ((vGimmickRight.z * -1.0f) * fSphereRadius);
                        }
                        break;
                    };

                    vPosition.y += ((m_fRadius / 3.0f) + 0.01f);

                    if (isExistsCharacterNear(&vPosition, m_fRadius / 3.0f))
                    {
                        vNewPos = m_vPosition;
                        Result = RESULT_COLLIDED;
                        break;
                    };

                    if (CWorldMap::CheckCollisionMoveSphere(&vPosition, m_fRadius / 3.0f, &vDltPos, CWorldMap::CHECKFLAG_NONE))
                    {
                        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
                        ASSERT(pResult);

                        if (std::strcmp(m_szName, pResult->m_mapobj.m_szGimmickObjName))
                        {
                            m_vVelocity = Math::VECTOR3_ZERO;
                            vNewPos = m_vPosition;
                            Result = RESULT_COLLIDED;
                            break;
                        };
                    };
                };

                m_vPosition = vNewPos;

                if (isFall(&vGimmickAt, &vGimmickRight))
                {
					m_pushstate = PUSHSTATE_FALL;

                    Math::Vec3_Normalize(&vSaveVelocity, &vSaveVelocity);
                    Math::Vec3_Scale(&m_vVelocity, &vSaveVelocity, 3.0f);

                    switch (m_fallaxis)
                    {
                    case FALL_ROT_AXIS_X_P:
                        m_vRotVelocity.x = Math::PI05;
                        break;

                    case FALL_ROT_AXIS_X_N:
                        m_vRotVelocity.x = -Math::PI05;
                        break;

                    case FALL_ROT_AXIS_Z_P:
                        m_vRotVelocity.z = Math::PI05;
                        break;

                    case FALL_ROT_AXIS_Z_N:
                        m_vRotVelocity.z = -Math::PI05;
                        break;

                    default:
                        ASSERT(false);
                        break;
                    };
                };
            }
            else
            {
                m_vVelocity = Math::VECTOR3_ZERO;                
            };
        }
        break;

    case PUSHSTATE_ROTATION:
        {
            m_bRequestPush = false;

            m_vRotation.x += (m_vRotVelocity.x * dt);
            m_vRotation.y += (m_vRotVelocity.y * dt);
            m_vRotation.z += (m_vRotVelocity.z * dt);
            
            float fGravity = CGameProperty::GetGravity();

            m_vVelocity.y += (fGravity * dt);

            m_vPosition.x += (m_vVelocity.x * dt);
            m_vPosition.y += (m_vVelocity.y * dt);
            m_vPosition.z += (m_vVelocity.z * dt);

            if (( (m_fallaxis == FALL_ROT_AXIS_X_P) && (m_vVelocity.x >  MATH_DEG2RAD(60.0f)) ) ||
                ( (m_fallaxis == FALL_ROT_AXIS_X_N) && (m_vVelocity.x < -MATH_DEG2RAD(60.0f)) ) ||
                ( (m_fallaxis == FALL_ROT_AXIS_Z_P) && (m_vVelocity.z <  MATH_DEG2RAD(60.0f)) ) ||
                ( (m_fallaxis == FALL_ROT_AXIS_Z_N) && (m_vVelocity.z < -MATH_DEG2RAD(60.0f)) ))
            {
                m_vVelocity.x = 0.0f;
                m_vVelocity.z = 0.0f;
                m_pushstate = PUSHSTATE_FALL;
            };
        }
        break;
        
    case PUSHSTATE_FALL:
        {
            m_bRequestPush = false;

            float fGravity = CGameProperty::GetGravity();
            
            m_vVelocity.y += (fGravity * dt);
            
            m_vPosition.x += (m_vVelocity.x * dt);
            m_vPosition.y += (m_vVelocity.y * dt);
            m_vPosition.z += (m_vVelocity.z * dt);

            if (m_vPosition.y < -100.0f)
            {
                m_vVelocity = Math::VECTOR3_ZERO;
                m_pushstate = PUSHSTATE_END;
            };
        }
        break;

    case PUSHSTATE_END:
        {
            m_bRequestPush = false;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return Result;
};


bool CPushingGimmickMove::RequestPlayerPushGimmick(CPlayerCharacter* pPlayerCharacter)
{
    bool bResult = false;
    
    if (m_pushstate == PUSHSTATE_PUSH && !m_bRequestPush)
    {
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        Math::Matrix_RotateY(&matrix, pPlayerCharacter->GetDirection());
        
        RwV3d vPlayerDir = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vPlayerDir, &Math::VECTOR3_AXIS_Z, &matrix);

        RwV3d vGimmickFrontDir = Math::VECTOR3_ZERO;
        RwV3d vGimmickRightDir = Math::VECTOR3_ZERO;
        RwV3d vGimmickBackDir = Math::VECTOR3_ZERO;

        RwMatrixSetIdentityMacro(&matrix);
        Math::Matrix_Rotate(&matrix, &m_vRotation);
        RwV3dTransformPoint(&vGimmickFrontDir, &Math::VECTOR3_AXIS_Z, &matrix);
        Math::Vec3_Scale(&vGimmickBackDir, &vGimmickFrontDir, -1.0f);
        RwV3dTransformPoint(&vGimmickRightDir, &Math::VECTOR3_AXIS_X, &matrix);

        const CPlayerCharacter::COLLISIONWALLINFO* pWallInfo = pPlayerCharacter->GetCollisionWall();
        ASSERT(pWallInfo);
        
        RwV3d vWallNormal = pWallInfo->m_vNormal;

        if (isSameDirectionVectors(&vGimmickFrontDir, &vPlayerDir, Math::Cos(MATH_DEG2RAD(15.0f))) &&
            isSameDirectionVectors(&vGimmickBackDir, &vWallNormal, 0.99f))
        {
            RwV3d vVelocity = Math::VECTOR3_ZERO;
            
            Math::Vec3_Scale(&vVelocity, &vGimmickFrontDir, m_fSpeed);
            SetVelocity(&vVelocity);

            m_bRequestPush = true;
            bResult = true;
        }
        else if (isSameDirectionVectors(&vGimmickBackDir, &vPlayerDir, Math::Cos(MATH_DEG2RAD(15.0f))) &&
                 isSameDirectionVectors(&vGimmickFrontDir, &vWallNormal, 0.99f))
        {
            RwV3d vVelocity = Math::VECTOR3_ZERO;
            
            Math::Vec3_Scale(&vVelocity, &vGimmickBackDir, m_fSpeed);
            SetVelocity(&vVelocity);
            
            m_bRequestPush = true;            
            bResult = true;
        };
    };

    return bResult;
};


void CPushingGimmickMove::SetRadius(float fRadius)
{
    m_fRadius = fRadius;
};


void CPushingGimmickMove::SetVelocity(const RwV3d* pvVelocity)
{
    ASSERT(pvVelocity);
    m_vVelocity = *pvVelocity;
};


void CPushingGimmickMove::SetRotation(const RwV3d* pvRotation)
{
    ASSERT(pvRotation);
    m_vRotation = *pvRotation;
};


void CPushingGimmickMove::SetSpeed(float fSpeed)
{
    m_fSpeed = fSpeed;
};


void CPushingGimmickMove::SetName(const char* pszName)
{
    ASSERT(pszName);
    ASSERT(std::strlen(pszName) < COUNT_OF(m_szName));
    
    std::strcpy(m_szName, pszName);
};


CPushingGimmickMove::PUSHSTATE CPushingGimmickMove::GetPushState(void) const
{
    return m_pushstate;
};


bool CPushingGimmickMove::isSameDirectionVectors(const RwV3d* pvVec0, const RwV3d* pvVec1, float fDotRange)
{
    return Math::Vec3_Dot(pvVec0, pvVec1) > fDotRange;
};


bool CPushingGimmickMove::isFall(const RwV3d* pvAt, const RwV3d* pvRight)
{
    RwV3d aChkPos[3] = { m_vPosition };

    aChkPos[0].x += (pvAt->x * m_fRadius);
    aChkPos[0].y += (pvAt->y * m_fRadius);
    aChkPos[0].z += (pvAt->z * m_fRadius);
    aChkPos[0].x += (pvRight->x * -m_fRadius);
    aChkPos[0].y += (pvRight->y * -m_fRadius);
    aChkPos[0].z += (pvRight->z * -m_fRadius);

    aChkPos[1].x += (pvAt->x * m_fRadius);
    aChkPos[1].y += (pvAt->y * m_fRadius);
    aChkPos[1].z += (pvAt->z * m_fRadius);
    aChkPos[1].x += (pvRight->x * m_fRadius);
    aChkPos[1].y += (pvRight->y * m_fRadius);
    aChkPos[1].z += (pvRight->z * m_fRadius);

    for (int32 i = 0; i < COUNT_OF(aChkPos); ++i)
    {
        float fMapHeight = CWorldMap::GetMapHeight(&aChkPos[i]);
        if (fMapHeight > (m_vPosition.y - 5.0f))
            return false;        
    };

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    Math::Matrix_Rotate(&matrix, &m_vRotation);

    RwV3d vGimmickAt = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vGimmickAt, &Math::VECTOR3_AXIS_Z, &matrix);

    if (isSameDirectionVectors(&vGimmickAt, &m_vVelocity, Math::Cos(MATH_DEG2RAD(45.0f))))
        m_fallaxis = FALL_ROT_AXIS_X_P;
    else
        m_fallaxis = FALL_ROT_AXIS_X_N;

    return true;
};


bool CPushingGimmickMove::isExistsCharacterNear(const RwV3d* pvPosition, float fRadius)
{
    float fNearestPlayerDist = CGimmickUtils::CalcNearestPlayerDistance(pvPosition);
    return (fNearestPlayerDist < (fRadius + 1.0f));
};