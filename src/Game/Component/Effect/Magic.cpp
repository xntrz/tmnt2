#include "Magic.hpp"
#include "Tracer.hpp"
#include "MagicManager.hpp"
#include "EffectManager.hpp"
#include "MagicParameter.hpp"

#include "Game/Component/Shot/Shot.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Particle/ParticleSet.hpp"
#include "Game/System/Particle/Locus.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"


CMagic::CMagic(const char* pszName)
: CEffect(pszName)
, m_feature(MAGICTYPES::FEATURE_NONE)
, m_collisionBody({ 0 })
, m_collisionAttack({ 0 })
, m_movement({ 0 })
, m_locusinfo({ 0 })
, m_fLivetime(0.0f)
, m_fNowTime(0.0f)
, m_fHitTimingStart(0.0f)
, m_fHitTimingEnd(0.0f)
, m_nReflectNumMax(0)
, m_nMapHitNum(0)
, m_nCharaHitNum(0)
, m_nMapObjectHitNum(0)
, m_pParent(nullptr)
, m_hParentObj(0)
, m_pTarget(nullptr)
, m_fChangeSize(0.0f)
, m_fInitBodyRadius(0.0f)
, m_fInitAttackRadius(0.0f)
, m_bFinish(false)
, m_bFinishStart(false)
{
    m_apLocus[LOCUSKIND_HOR] = nullptr;
    m_apLocus[LOCUSKIND_VER] = nullptr;

    m_szBaseEffectName[0] = '\0';
    m_szAppearEffectName[0] = '\0';
    m_szVanishEffectName[0] = '\0';
    m_szReflectEffectName[0] = '\0';
    m_szVanishMagicName[0] = '\0';

    m_type = TYPE_WITHHIT;
};


CMagic::~CMagic(void)
{
    if (m_pTarget)
    {
        delete m_pTarget;
        m_pTarget = nullptr;
    };

    for (int32 i = 0; i < COUNT_OF(m_apLocus); ++i)
    {
        if (m_apLocus[i])
        {
            delete m_apLocus[i];
            m_apLocus[i] = nullptr;
        };
    };
};


void CMagic::Run(void)
{
    if (!m_bPlay)
        return;

    RunFinish();

    if (m_bFinish)
        return;

    float dt = CGameProperty::GetElapsedTime();
    
    RwV3d vMyPosition = Math::VECTOR3_ZERO;
    GetPosition(&vMyPosition);

    if (m_fNowTime == 0.0f)
        Appearing(&vMyPosition);

    if (m_pParticleSet)
        m_pParticleSet->Run(dt);

    if (m_apLocus[LOCUSKIND_HOR])
    {
        CLocus* pLocus = m_apLocus[LOCUSKIND_HOR];

        RwV3d vHigh = vMyPosition;
        vHigh.y += m_locusinfo.m_fRadius;

        RwV3d vLow = vMyPosition;
        vLow.y -= m_locusinfo.m_fRadius;

        pLocus->SetPosition(&vHigh, &vLow);
    };

    if (m_apLocus[LOCUSKIND_VER])
    {
        CLocus* pLocus = m_apLocus[LOCUSKIND_VER];
        
        RwV3d vMove = Math::VECTOR3_ZERO;
        Math::Vec3_Normalize(&vMove, &m_movement.m_vVelocity);
        
        RwV3d vVerVec = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vVerVec, &Math::VECTOR3_AXIS_Y, &vMove);
        Math::Vec3_Scale(&vVerVec, &vVerVec, m_locusinfo.m_fRadius);

        RwV3d vHigh = vMyPosition;
        Math::Vec3_Add(&vHigh, &vHigh, &vVerVec);

        Math::Vec3_Scale(&vVerVec, &vVerVec, -1.0f);

        RwV3d vLow = vMyPosition;
        Math::Vec3_Add(&vLow, &vLow, &vVerVec);

        pLocus->SetPosition(&vHigh, &vLow);
    };

    Transition();
    m_fNowTime += dt;
};


void CMagic::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case GAMEOBJECTTYPES::MESSAGEID_CATCHATTACK:
        {
            CHitAttackData* pAttack = static_cast<CHitAttackData*>(pParam);
            ASSERT(pAttack);
            
            if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_ATTACKED_HIT))
            {
                if (IsRivalAttack(pAttack, m_pParent))
                {
                    pAttack->GetCatch()->SetResult(CHitCatchData::RESULT_HIT);
                    
                    RwV3d vMyPosition = Math::VECTOR3_ZERO;
                    GetPosition(&vMyPosition);
                    
                    Vanishing(&vMyPosition);
                    Finish();
                };
            };
        }
        break;
        
    case GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT:
        {
            CHitCatchData* pCatch = static_cast<CHitCatchData*>(pParam);
            ASSERT(pCatch);

            if (CheckFeature(MAGICTYPES::FEATURE_ATTACK_HIT))
            {
                RwV3d vMyPosition = Math::VECTOR3_ZERO;
                GetPosition(&vMyPosition);
                
                if (!CheckFeature(MAGICTYPES::FEATURE_APPEAR_HORIZON))
                    Vanishing(&vMyPosition);

                Finish();         
            };

            if (CheckFeature(MAGICTYPES::FEATURE_PARENT))
            {
                ASSERT(m_pParent);
                CGameObjectManager::SendMessage(m_pParent, GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT, pParam);
            };
        }
        break;

    default:
        CEffect::MessageProc(nMessageID, pParam);
        break;
    };
};


void CMagic::CreateSubstance(const char* pszName)
{
    ASSERT(pszName);

    CEffect* pEffect = CEffectManager::GetEffectFromPool(pszName);
    ASSERT(pEffect);
    
    ASSERT(!m_pParticleSet);
    m_pParticleSet = pEffect->CloneParticle();

    RwV3d vEffectPos = Math::VECTOR3_ZERO;
    pEffect->GetPosition(&vEffectPos);

    m_vPosition = vEffectPos;
    m_pTarget   = nullptr;
    m_bPlay     = false;    
};


CMagic* CMagic::Clone(void)
{
    CMagic* pMagic = new CMagic(GetName());

    pMagic->m_vPosition         = m_vPosition;
	pMagic->m_pParticleSet      = m_pParticleSet->Clone();
    pMagic->m_feature           = m_feature;
    pMagic->m_collisionBody     = m_collisionBody;
    pMagic->m_collisionAttack   = m_collisionAttack;
    pMagic->m_movement          = m_movement;
    pMagic->m_locusinfo         = m_locusinfo;
    pMagic->m_fLivetime         = m_fLivetime;
    pMagic->m_fHitTimingStart   = m_fHitTimingStart;
    pMagic->m_fHitTimingEnd     = m_fHitTimingEnd;
    pMagic->m_nReflectNumMax    = m_nReflectNumMax;
    pMagic->m_pParent           = m_pParent;
    pMagic->m_fChangeSize       = m_fChangeSize;
    pMagic->m_bFinish           = m_bFinish;
    pMagic->m_bFinishStart      = m_bFinishStart;
    pMagic->m_fInitBodyRadius   = m_fInitBodyRadius;
    pMagic->m_fInitAttackRadius = m_fInitAttackRadius;
	pMagic->m_fNowTime          = 0.0f;
	pMagic->m_nMapHitNum        = 0;
	pMagic->m_nCharaHitNum      = 0;
	pMagic->m_nMapObjectHitNum  = 0;

    std::strcpy(pMagic->m_szBaseEffectName,     m_szBaseEffectName);
    std::strcpy(pMagic->m_szAppearEffectName,   m_szAppearEffectName);
    std::strcpy(pMagic->m_szVanishEffectName,   m_szVanishEffectName);
    std::strcpy(pMagic->m_szReflectEffectName,  m_szReflectEffectName);
    std::strcpy(pMagic->m_szVanishMagicName,    m_szVanishMagicName);

    if (CheckFeature(MAGICTYPES::FEATURE_LOCUS_VER))
    {
        CLocus* pLocus = new CLocus(m_locusinfo.m_nNumPoint, m_locusinfo.m_Color);
        pLocus->SetDrawEnable(true);
		pLocus->SetAlphaBasis(uint8(m_locusinfo.m_uAlphaBasis));

        pMagic->m_apLocus[LOCUSKIND_VER] = pLocus;
    };

    if (CheckFeature(MAGICTYPES::FEATURE_LOCUS_HOR))
    {
        CLocus* pLocus = new CLocus(m_locusinfo.m_nNumPoint, m_locusinfo.m_Color);
        pLocus->SetDrawEnable(true);
		pLocus->SetAlphaBasis(uint8(m_locusinfo.m_uAlphaBasis));

        pMagic->m_apLocus[LOCUSKIND_HOR] = pLocus;
    };

    return pMagic;
};


void CMagic::Draw(RwCamera* pCamera)
{
    for (int32 i = 0; i < COUNT_OF(m_apLocus); ++i)
    {
        if (m_apLocus[i])
            m_apLocus[i]->Draw();
    };

    CEffect::Draw(pCamera);
};


void CMagic::StartObject(void)
{
    CEffect::Start();
};


void CMagic::End(void)
{
    Finish();
};


bool CMagic::IsEnd(void) const
{
    if (!m_pParticleSet || !m_bPlay)
        return false;

    if (m_bFinish)
        return true;

    return m_pParticleSet->IsEnd();
};


void CMagic::Finish(void)
{
    m_bFinishStart = true;
    m_pParticleSet->SetEmitter(false);

    if (CheckFeature(MAGICTYPES::FEATURE_LOST_IMMEDIATE))
        m_bFinish = true;
};


void CMagic::RunFinish(void)
{
    if (m_bFinishStart && !m_pParticleSet->IsDrawing())
        m_bFinish = true;
};


void CMagic::SetParameter(CMagicParameter* pMagicParameter)
{
    ASSERT(pMagicParameter->m_pszBaseEffectName);

    std::strcpy(m_szBaseEffectName, pMagicParameter->m_pszBaseEffectName);

    if (pMagicParameter->m_pszAppearEffectName)
        std::strcpy(m_szAppearEffectName, pMagicParameter->m_pszAppearEffectName);

    if (pMagicParameter->m_pszVanishEffectName)
        std::strcpy(m_szVanishEffectName, pMagicParameter->m_pszVanishEffectName);

    if (pMagicParameter->m_pszReflectEffectName)
        std::strcpy(m_szReflectEffectName, pMagicParameter->m_pszReflectEffectName);

    if (pMagicParameter->m_pszVanishMagicName)
        std::strcpy(m_szVanishMagicName, pMagicParameter->m_pszVanishMagicName);

	if (pMagicParameter->m_fScale != 1.0f)
		SetScale(pMagicParameter->m_fScale);

    m_feature           = pMagicParameter->m_feature;
    m_collisionBody     = pMagicParameter->m_collisionBody;
    m_collisionAttack   = pMagicParameter->m_collisionAttack;
	m_movement          = pMagicParameter->m_movement;
    m_locusinfo         = pMagicParameter->m_locusinfo;
    m_fLivetime         = pMagicParameter->m_fLivetime;
    m_nReflectNumMax    = pMagicParameter->m_nReflectNumMax;
    m_fChangeSize       = pMagicParameter->m_fChangeSize;
    m_fHitTimingStart   = pMagicParameter->m_fHitTimingStart;
    m_fHitTimingEnd     = pMagicParameter->m_fHitTimingEnd;   

    m_fInitBodyRadius   = m_collisionBody.m_fRadius;
    m_fInitAttackRadius = m_collisionAttack.m_fRadius;
    m_fNowTime          = 0.0f;
    m_nMapHitNum        = 0;
    m_nCharaHitNum      = 0;
    m_nMapObjectHitNum  = 0;

    if (CheckFeature(MAGICTYPES::FEATURE_LOCUS_VER))
    {
        CLocus* pLocus = new CLocus(m_locusinfo.m_nNumPoint, m_locusinfo.m_Color);
        pLocus->SetDrawEnable(true);
		pLocus->SetAlphaBasis(uint8(m_locusinfo.m_uAlphaBasis));

        m_apLocus[LOCUSKIND_VER] = pLocus;
    };

    if (CheckFeature(MAGICTYPES::FEATURE_LOCUS_HOR))
    {
        CLocus* pLocus = new CLocus(m_locusinfo.m_nNumPoint, m_locusinfo.m_Color);
        pLocus->SetDrawEnable(true);
		pLocus->SetAlphaBasis(uint8(m_locusinfo.m_uAlphaBasis));

        m_apLocus[LOCUSKIND_HOR] = pLocus;
    };
};


void CMagic::SetDamage(int32 nPower)
{
    m_collisionAttack.m_nDamage = nPower;
};


void CMagic::SetStatusTime(float fTime)
{
    m_collisionAttack.m_fStatusTime = fTime;
};


void CMagic::SetChangeStatus(int32 nStatus)
{
    m_collisionAttack.m_nStatus = nStatus;
};


void CMagic::SetFlyawayXY(float x, float y)
{
    m_collisionAttack.m_fFlyawayX = x;
    m_collisionAttack.m_fFlyawayY = y;
};


void CMagic::SetMovement(MAGICTYPES::MOVEMENT* pMovement)
{
    ASSERT(pMovement);
    m_movement = *pMovement;
};


void CMagic::SetLive(float fLivetime)
{
    m_fLivetime = fLivetime;
};


void CMagic::SetParent(CGameObject* pParent)
{
    m_pParent = pParent;
    if (m_pParent)
        m_hParentObj = m_pParent->GetHandle();
};


void CMagic::SetTarget(CTracer* pTarget)
{
    m_pTarget = pTarget;
};


void CMagic::SetDirectionBase(const RwV3d* pvDirection)
{
    CEffect::SetDirection(pvDirection);
};


void CMagic::SetDirection(const RwV3d* pvDirection)
{
    ASSERT(pvDirection);
    ASSERT(m_pParticleSet);

    //
    //  Update movememnt
    //
    RwV3d vDirection = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vDirection, pvDirection);
    
    float fLength = Math::Vec3_Length(&m_movement.m_vVelocity);
    float fLengthAccel = Math::Vec3_Length(&m_movement.m_vAcceleration);
    
    m_movement.m_vVelocity = vDirection;
    m_movement.m_vAcceleration = vDirection;

    Math::Vec3_Scale(&m_movement.m_vVelocity, &m_movement.m_vVelocity, fLength);
    Math::Vec3_Scale(&m_movement.m_vAcceleration, &m_movement.m_vAcceleration, fLengthAccel);

    //
    //  Update particle set and collision
    //
    RwV3d vBuffer = Math::VECTOR3_ZERO;
    RwV3d vAxisX = Math::VECTOR3_ZERO;
    RwV3d vAxisY = Math::VECTOR3_ZERO;
    RwV3d vAxisZ = Math::VECTOR3_ZERO;

    Math::Vec3_Normalize(&vBuffer, pvDirection);
    vBuffer.y = std::fabs(vBuffer.y);

    if (vBuffer.y > 0.3f)
    {
        vBuffer.z = std::fabs(vBuffer.z);
        if (vBuffer.z > 0.3f)
            Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_X, pvDirection);
        else
            Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Z, pvDirection);

        Math::Vec3_Cross(&vAxisY, pvDirection, &vAxisX);
    }
    else
    {
        Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Y, pvDirection);
        Math::Vec3_Cross(&vAxisY, pvDirection, &vAxisX);
    };

    Math::Vec3_Normalize(&vAxisX, &vAxisX);
    Math::Vec3_Normalize(&vAxisY, &vAxisY);
    Math::Vec3_Normalize(&vAxisZ, pvDirection);

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    Math::Matrix_Update(&matrix,
                        &vAxisX,
                        &vAxisY,
                        &vAxisZ,
                        &Math::VECTOR3_ZERO);

    RwV3dTransformVector(&m_collisionBody.m_vPosition,
                         &m_collisionBody.m_vPosition,
                         &matrix);

    RwV3dTransformVector(&m_collisionAttack.m_vPosition,
                         &m_collisionAttack.m_vPosition,
                         &matrix);

    m_pParticleSet->SetVector(&vAxisX, &vAxisY, &vAxisZ);
};


void CMagic::SetDirection(const RwMatrix* pMatrix)
{
    CEffect::SetDirection(pMatrix);
};


void CMagic::SetPosition(const RwV3d* pvPosition)
{
    CEffect::SetPosition(pvPosition);
};


void CMagic::GetPosition(RwV3d* pvPosition) const
{
    CEffect::GetPosition(pvPosition);
};


void CMagic::SetSpeed(float fSpeed)
{
    Math::Vec3_Normalize(&m_movement.m_vVelocity, &m_movement.m_vVelocity);
    Math::Vec3_Scale(&m_movement.m_vVelocity, &m_movement.m_vVelocity, fSpeed);
};


void CMagic::SetScale(float fScale)
{
    Math::Vec3_Scale(&m_collisionBody.m_vPosition, &m_collisionBody.m_vPosition, fScale);
    m_collisionBody.m_fRadius *= fScale;
    
    Math::Vec3_Scale(&m_collisionAttack.m_vPosition, &m_collisionAttack.m_vPosition, fScale);
    m_collisionAttack.m_fRadius *= fScale;
    
	m_locusinfo.m_fRadius *= fScale;

    CEffect::SetScale(fScale);
};


void CMagic::Transition(void)
{
    if (m_feature == MAGICTYPES::FEATURE_NONE)
        return;

    if (CheckFeature(MAGICTYPES::FEATURE_LIVETIME))
    {
        if ((m_fHitTimingStart <= m_fNowTime) &&
            (m_fHitTimingEnd >= m_fNowTime) &&
            (CheckBody() || CheckAttack()))
            return;
    }
    else if (CheckBody() || CheckAttack())
    {
        return;
    };

    if (CheckFeature(MAGICTYPES::FEATURE_LIVETIME))
    {
        if (m_fLivetime < m_fNowTime)
        {
            RwV3d vMyPosition = Math::VECTOR3_ZERO;
            GetPosition(&vMyPosition);

            Vanishing(&vMyPosition);
            Finish();
            return;
        };
    };
    
    if (CheckFeature(MAGICTYPES::FEATURE_GRAVITY))
    {
        float dt = CGameProperty::GetElapsedTime();
        float fGravity = CGameProperty::GetGravity();
        
        m_movement.m_vVelocity.y += (dt * fGravity);
    };

    if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_HIT_NUM))
    {
        int32 nReflectNum = 0;
        
        if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_MAP))
            nReflectNum += m_nMapHitNum;

        if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_CHARA))
            nReflectNum += m_nCharaHitNum;

        if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_MAPOBJECT))
            nReflectNum += m_nMapObjectHitNum;

        if (nReflectNum > m_nReflectNumMax)
        {
            RwV3d vMyPosition = Math::VECTOR3_ZERO;
            GetPosition(&vMyPosition);

            Vanishing(&vMyPosition);
            Finish();
            return;
        };
    };

    if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_HIT) &&
        (m_nMapHitNum || m_nCharaHitNum || m_nMapObjectHitNum))
    {
        RwV3d vMyPosition = Math::VECTOR3_ZERO;
        GetPosition(&vMyPosition);

        Vanishing(&vMyPosition);
        Finish();
        return;
    };

    if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_CHANGE_SIZE))
    {
        float dt = CGameProperty::GetElapsedTime();
        
        m_collisionBody.m_fRadius += (dt * m_fInitBodyRadius * m_fChangeSize);
        m_collisionAttack.m_fRadius += (dt * m_fInitAttackRadius * m_fChangeSize);
    };

    if (CheckFeature(MAGICTYPES::FEATURE_HOMING))
    {
        if (m_pTarget)
        {
            RwV3d vTargetPosition = Math::VECTOR3_ZERO;
            m_pTarget->GetPosition(&vTargetPosition);
            
            RwV3d vMyPosition = Math::VECTOR3_ZERO;
            GetPosition(&vMyPosition);

            RwV3d vDistance = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vDistance, &vTargetPosition, &vMyPosition);

            if (Math::Vec3_Length(&vDistance) < 0.1f)
            {
                Vanishing(&vMyPosition);
                Finish();
                return;
            };

            if (Math::Vec3_Length(&m_movement.m_vVelocity) > 0.0f)
                Homing();
        };
    };

    if (CheckFeature(MAGICTYPES::FEATURE_STRAIGHT))
    {
        float dt = CGameProperty::GetElapsedTime();        
        RwV3d vFrameVelocity = Math::VECTOR3_ZERO;
        
        Math::Vec3_Scale(&vFrameVelocity, &m_movement.m_vVelocity, dt);
        Math::Vec3_Add(&m_vPosition, &m_vPosition, &vFrameVelocity);
        
        m_pParticleSet->SetBasisPositionForAll(&m_vPosition);
    }
    else if (CheckFeature(MAGICTYPES::FEATURE_ACCELERATE))
    {
        float dt = CGameProperty::GetElapsedTime();
        RwV3d vFrameVelocity = Math::VECTOR3_ZERO;
        RwV3d vFrameAcceleration = Math::VECTOR3_ZERO;

        Math::Vec3_Scale(&vFrameAcceleration, &m_movement.m_vAcceleration, dt);
        Math::Vec3_Add(&m_movement.m_vVelocity, &m_movement.m_vVelocity, &vFrameAcceleration);
        
        Math::Vec3_Scale(&vFrameVelocity, &m_movement.m_vVelocity, dt);
        Math::Vec3_Add(&m_vPosition, &m_vPosition, &vFrameVelocity);
        
        m_pParticleSet->SetBasisPositionForAll(&m_vPosition);
    };

    RwV3d vMyPosition = Math::VECTOR3_ZERO;
    GetPosition(&vMyPosition);

    if (m_pParticleSet->IsTrace())
        m_pParticleSet->SetBasisPosition(&vMyPosition);
};


bool CMagic::CheckBody(void)
{
    if (!CheckFeature(MAGICTYPES::FEATURE_BODY))
        return false;

    MAGICTYPES::FEATURE feature = MAGICTYPES::FEATURE_COLLISION_MAP
                                | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT;

    if (CheckFeature(feature) && (m_fNowTime != 0.0f))
    {
        float dt = CGameProperty::GetElapsedTime();

        RwV3d vMyPosition = Math::VECTOR3_ZERO;
        GetPosition(&vMyPosition);

        RwV3d vFrameVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vFrameVelocity, &m_movement.m_vVelocity, dt);

        RwV3d vMyPrevPosition = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vMyPrevPosition, &vMyPosition, &vFrameVelocity);

        RwLine line = { 0 };
        line.end = vMyPosition;
        line.start = vMyPrevPosition;

        if (CWorldMap::CheckCollisionLine(&line.start, &line.end))
        {
            const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
            ASSERT(pResult);

            ++m_nMapHitNum;

            if (pResult->m_attribute != MAPTYPES::ATTRIBUTE_ONEWAY)
            {
                feature = MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                        | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT;

                RwV3d vNormal = pResult->m_vNormal;
                RwV3d vPosition = pResult->m_vClosestPt;

                if (CheckFeature(feature))
                {
                    VanishingHorizontal(&vNormal, &vPosition);
                    Finish();

                    return true;
                }
                else
                {
                    if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_REFLECTION))
                    {
                        Reflect(&vNormal);
                        Reflecting(&vPosition);
                    };
                };
            };
        }
        else
        {
			float fMyRadius = m_collisionBody.m_fRadius;
            Math::Vec3_Add(&vMyPosition, &vMyPosition, &m_collisionBody.m_vPosition);
            
            if (CWorldMap::CheckCollisionSphere(&vMyPosition, fMyRadius))
            {
                const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
                ASSERT(pResult);

                ++m_nMapHitNum;

                if (pResult->m_attribute != MAPTYPES::ATTRIBUTE_ONEWAY)
                {
                    feature = MAGICTYPES::FEATURE_COLLISION_MAP_HIT
                            | MAGICTYPES::FEATURE_COLLISION_MAPOBJECT_HIT;

                    RwV3d vNormal = pResult->m_vNormal;
                    RwV3d vPosition = pResult->m_vClosestPt;

                    if (CheckFeature(feature))
                    {
                        VanishingHorizontal(&vNormal, &vPosition);
                        Finish();

                        return true;
                    }
                    else
                    {
                        if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_REFLECTION))
                        {
                            Reflect(&vNormal);
                            Reflecting(&vPosition);
                        };
                    };                    
                };
            };
        };
    };

    if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_CHARA))
    {
        RwV3d vMyPosition = Math::VECTOR3_ZERO;
        GetPosition(&vMyPosition);
        
        Math::Vec3_Add(&vMyPosition, &vMyPosition, &m_collisionBody.m_vPosition);
        
        CBodyHitData BodyHit;
        BodyHit.InitData(&vMyPosition, m_collisionBody.m_fRadius);
        
        RwV3d vResult = Math::VECTOR3_ZERO;
        if (CBodyHitManager::CheckHit(&BodyHit, &m_movement.m_vVelocity, &vResult))
        {
            ++m_nCharaHitNum;

            if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_CHARA_HIT))
            {
                GetPosition(&vMyPosition);
                
                Vanishing(&vMyPosition);
                Finish();
                return true;
            };

            if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_REFLECTION))
                Math::Vec3_Scale(&m_movement.m_vVelocity, &m_movement.m_vVelocity, -1.0f);
        };
    };

    return false;
};


bool CMagic::CheckAttack(void)
{
    if (CheckFeature(MAGICTYPES::FEATURE_ATTACK))
    {
        RwV3d vMyPosition = Math::VECTOR3_ZERO;
        GetPosition(&vMyPosition);

        Math::Vec3_Add(&vMyPosition, &vMyPosition, &m_collisionAttack.m_vPosition);

        RwV3d vMyPrevPosition = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vMyPrevPosition, &vMyPosition, &m_movement.m_vVelocity);

        RwSphere hitSphere;
        hitSphere.center = vMyPosition;
        hitSphere.radius = m_collisionAttack.m_fRadius;

        CHitAttackData hitAttack;
        hitAttack.SetObject(GetHandle());
        hitAttack.SetObjectPos(&vMyPrevPosition);
        hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);
        hitAttack.SetSphere(&hitSphere);
        hitAttack.SetPower(m_collisionAttack.m_nDamage);
        hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD(m_collisionAttack.m_nAntiguard));
        hitAttack.SetFlagConfusion(m_collisionAttack.m_bConfusion);
        hitAttack.SetStatus(CHitAttackData::STATUS(m_collisionAttack.m_nStatus));
		if (hitAttack.GetStatus() != CHitAttackData::STATUS_KNOCK)
		{
			if (hitAttack.IsTroubleAttack())
				hitAttack.SetTroubleParameter(m_collisionAttack.m_fStatusTime);
			else
				hitAttack.SetFlyawayParameter(m_collisionAttack.m_fFlyawayX, m_collisionAttack.m_fFlyawayY);
		};

        MAGICTYPES::FEATURE featureAttack = (m_feature & MAGICTYPES::FEATURE_MASK_ATTACK_TO);
        switch (featureAttack)
        {
        case MAGICTYPES::FEATURE_ATTACK_TO_PLAYER:
            hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER);
            break;

        case MAGICTYPES::FEATURE_ATTACK_TO_ENEMY:
            hitAttack.SetTarget(CHitAttackData::TARGET_ENEMY);
            break;

        case MAGICTYPES::FEATURE_ATTACK_TO_MAPOBJ:
            hitAttack.SetTarget(CHitAttackData::TARGET_GIMMICK);
            break;

        case MAGICTYPES::FEATURE_ATTACK_TO_PLAYER_MAPOBJ:                    
            hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER_GIMMICK);
            break;

        case MAGICTYPES::FEATURE_ATTACK_TO_ENEMY_MAPOBJ:
            hitAttack.SetTarget(CHitAttackData::TARGET_ENEMY_GIMMICK);
            break;

        case MAGICTYPES::FEATURE_ATTACK_TO_PLAYER_ENEMY:
            hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER | CHitAttackData::TARGET_ENEMY);
            break;

        default:
            hitAttack.SetTarget(CHitAttackData::TARGET_ALL_EXCEPT_SELF);
            break;
        };

        CHitAttackManager::RegistAttack(&hitAttack);
    };

    if (CheckFeature(MAGICTYPES::FEATURE_COLLISION_ATTACKED))
    {
        RwV3d vMyPosition = Math::VECTOR3_ZERO;
        GetPosition(&vMyPosition);

        Math::Vec3_Add(&vMyPosition, &vMyPosition, &m_collisionBody.m_vPosition);

        RwSphere hitSphere;
        hitSphere.center = vMyPosition;
        hitSphere.radius = m_collisionBody.m_fRadius;

        CHitCatchData hitCatch;
        hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
        hitCatch.SetSphere(&hitSphere);
        hitCatch.SetObject(GetHandle());
        hitCatch.SetObjectType(GetType());

        CHitAttackManager::RegistCatch(&hitCatch);
    };

    return false;
};


void CMagic::Homing(void)
{    
    RwV3d vTargetPosition = Math::VECTOR3_ZERO;
    m_pTarget->GetPosition(&vTargetPosition);

    RwV3d vMyPosition = Math::VECTOR3_ZERO;
    GetPosition(&vMyPosition);
    
    RwV3d vHomingVector = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vHomingVector, &vTargetPosition, &vMyPosition);
    Math::Vec3_Normalize(&vHomingVector, &vHomingVector);

    float fVel = Math::Vec3_Length(&m_movement.m_vVelocity);
    Math::Vec3_Scale(&m_movement.m_vVelocity, &vHomingVector, fVel);

    float fAccel = Math::Vec3_Length(&m_movement.m_vAcceleration);
    Math::Vec3_Scale(&m_movement.m_vAcceleration, &vHomingVector, fAccel);
};


void CMagic::Reflect(RwV3d* pvNormal)
{
    RwV3d vResult = Math::VECTOR3_ZERO;
    Math::Vec3_Reflect(&vResult, &m_movement.m_vVelocity, pvNormal);

    SetDirection(&vResult);
};


void CMagic::Appearing(RwV3d* pvPosition)
{
    if (std::strlen(m_szAppearEffectName) > 0)
    {
        RwV3d vDirection = Math::VECTOR3_ZERO;

        vDirection.x = m_movement.m_vVelocity.x;
        vDirection.y = 0.0f;
        vDirection.z = m_movement.m_vVelocity.z;

        Math::Vec3_Normalize(&vDirection, &vDirection);
        float fDirection = Math::ATan2(vDirection.x, vDirection.z);

        CEffectManager::Play(m_szAppearEffectName, pvPosition, fDirection);
    };
};


void CMagic::Reflecting(RwV3d* pvPosition)
{
    if (std::strlen(m_szReflectEffectName) > 0)
        CEffectManager::Play(m_szReflectEffectName, pvPosition);
};


void CMagic::Vanishing(RwV3d* pvPosition)
{
    if (!m_bFinish && !m_bFinishStart)
    {
        if (std::strlen(m_szVanishEffectName) > 0)
            CEffectManager::Play(m_szVanishEffectName, pvPosition);

        if (std::strlen(m_szVanishMagicName) > 0)
        {
            CMagicManager::Play(m_szVanishMagicName, pvPosition, &m_movement.m_vVelocity, nullptr, true);
            VanishingSE();
        };
    };
};


void CMagic::VanishingHorizontal(RwV3d* pvNormal, RwV3d* pvPosition)
{
    if (m_bFinish || m_bFinishStart)
        return;

    if (std::strlen(m_szVanishEffectName) > 0)
    {
        if (CheckFeature(MAGICTYPES::FEATURE_APPEAR_HORIZON))
        {
            RwV3d vAxisX = Math::VECTOR3_ZERO;
            RwV3d vAxisY = Math::VECTOR3_ZERO;
            RwV3d vAxisZ = Math::VECTOR3_ZERO;

            Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Z, pvNormal);
            Math::Vec3_Cross(&vAxisZ, pvNormal, &Math::VECTOR3_AXIS_X);
            Math::Vec3_Normalize(&vAxisX, &vAxisX);
            Math::Vec3_Normalize(&vAxisY, pvNormal);
            Math::Vec3_Normalize(&vAxisZ, &vAxisZ);

            RwMatrix matrix;
            RwMatrixSetIdentityMacro(&matrix);

            Math::Matrix_Update(
                &matrix,
                &vAxisX,
                &vAxisY,
                &vAxisZ,
                &Math::VECTOR3_ZERO
            );

            uint32 hEffect = CEffectManager::Play(m_szVanishEffectName, pvPosition);
            CEffectManager::SetDirection(hEffect, &matrix);
        }
        else
        {
            CEffectManager::Play(m_szVanishEffectName, pvPosition);
        };
    };

    if (std::strlen(m_szVanishMagicName) > 0)
    {
        RwV3d vDirection = Math::VECTOR3_ZERO;
        
        if (CheckFeature(MAGICTYPES::FEATURE_APPEAR_HORIZON))
        {
            RwV3d vAxisX = Math::VECTOR3_ZERO;
            RwV3d vAxisY = Math::VECTOR3_ZERO;
            RwV3d vAxisZ = Math::VECTOR3_ZERO;

            Math::Vec3_Cross(&vAxisX, &Math::VECTOR3_AXIS_Z, pvNormal);
            Math::Vec3_Cross(&vAxisZ, pvNormal, &Math::VECTOR3_AXIS_X);
            Math::Vec3_Normalize(&vAxisX, &vAxisX);
            Math::Vec3_Normalize(&vAxisY, pvNormal);
            Math::Vec3_Normalize(&vAxisZ, &vAxisZ);

            RwMatrix matrix;
            RwMatrixSetIdentityMacro(&matrix);

            Math::Matrix_Update(
                &matrix,
                &vAxisX,
                &vAxisY,
                &vAxisZ,
                &Math::VECTOR3_ZERO
            );

            uint32 hMagic = CMagicManager::Play(m_szVanishMagicName, pvPosition, &vDirection, nullptr, true);
            CMagicManager::SetDirection(hMagic, &matrix);
        }
        else
        {
            CMagicManager::Play(m_szVanishMagicName, pvPosition, &vDirection, nullptr, true);
            VanishingSE();
        };
    };
};


void CMagic::VanishingSE(void)
{
    switch (GetID())
    {
    case MAGICID::ID_TENGSWORD:
        CGameSound::PlayObjectSE(this, SDCODE_SE(4359));
        break;

    case MAGICID::ID_SLABALL:
        CGameSound::PlayObjectSE(this, SDCODE_SE(4400));
        break;

    case MAGICID::ID_GUNBALL:
        CGameSound::PlayObjectSE(this, SDCODE_SE(4139));
        break;

    default:
        break;
    };
};


MAGICID::VALUE CMagic::GetID(void) const
{
    return MAGICID::GetIDFromName(GetName());
};


bool CMagic::IsRivalAttack(CHitAttackData* pAttack, const CGameObject* pObject)
{
    ASSERT(pAttack);

    bool bResult = true;

    if (!pObject)
        return bResult;

    switch (pObject->GetType())
    {
    case GAMEOBJECTTYPE::GIMMICK:
        bResult = false;
        break;

    case GAMEOBJECTTYPE::PLAYER:
        bResult = false;
        break;

    case GAMEOBJECTTYPE::CHARACTER:
        {
            const CCharacter* pCharacter = static_cast<const CCharacter*>(pObject);
            switch (pCharacter->GetCharacterType())
            {
            case CCharacter::TYPE_ENEMY:
                bResult = false;
                break;

            case CCharacter::TYPE_PLAYER:
                bResult = false;
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case GAMEOBJECTTYPE::ENEMY:
        bResult = false;
        break;

    case GAMEOBJECTTYPE::EFFECT:
        {
            const CMagic* pMagic = static_cast<const CMagic*>(pObject);

            CGameObject* pParentObj = CGameObjectManager::GetObject(pMagic->m_hParentObj);
            if (pParentObj)
                bResult = IsRivalAttack(pAttack, pParentObj);
        }
        break;

    case GAMEOBJECTTYPE::SHOT:
        {
            const CShot* pShot = static_cast<const CShot*>(pObject);

            CGameObject* pParentObj = CGameObjectManager::GetObject(pShot->GetParentHandle());
            if (pParentObj)
                bResult = IsRivalAttack(pAttack, pParentObj);
        }
        break;

    default:
        break;
    };

    return bResult;
};


uint32 CMagic::GetParent(void) const
{
    return m_hParentObj;
};


void CMagic::SetFeature(MAGICTYPES::FEATURE feature)
{
    m_feature = feature;
};


void CMagic::AddFeature(MAGICTYPES::FEATURE feature)
{
    m_feature |= feature;
};


bool CMagic::CheckFeature(MAGICTYPES::FEATURE feature) const
{
    return ((m_feature & feature) != 0);
};