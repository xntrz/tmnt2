#include "Shot.hpp"
#include "ShotTracer.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Effect/EffectID.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicID.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Sequence/Ride/RideTypes.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Particle/Locus.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "System/Common/RenderState.hpp"


namespace
{
    struct LOCUSINFO
    {
        RwRGBA  m_Color;
        uint8   m_uAlphaBasis;
        int32   m_nPointNum;
    };

    struct SHOTINFO
    {
        const char*             m_pszModelName;
        int32                   m_nPower;
        float                   m_fSpeed;
        float                   m_fHitRadius;
        float                   m_fRadius;
        float                   m_fGravity;
        CHitAttackData::TARGET  m_target;
        bool                    m_bReflection;
        EFFECTID::VALUE         m_idEffectTrace;
        EFFECTID::VALUE         m_idEffectHit;
        EFFECTID::VALUE         m_idEffectVanish;
        MAGICID::VALUE          m_idMagicHit;
        LOCUSINFO               m_locusinfo;
    };

    
    void GetRotateFromVector(RwV3d* pvRot, RwV3d* pv)
    {
        pvRot->x = -Math::ASin(pv->y);
        pvRot->z = 0.0f;

        float fCosArg = pv->z / Math::Cos(pvRot->x);
        pvRot->y = Math::ACos(fCosArg);

        if (pv->x <= 0.0f)
            pvRot->y *= -1.0f;
        else
            pvRot->y *= 1.0f;
    };
};


static const SHOTINFO s_aShotInfo[] =
{
    /* model name |  power | speed | hit radius | radius | gravity | target                                  |  reflection    | effect trace                  | effect hit                | effect vanish           | hit magic             | locus */
    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    {  "",           0,      0.0f,      0.0f,      0.0f,    0.0f,    CHitAttackData::TARGET_NONE,               false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { 0 } },
    {  "arrow",      5,      72.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     true,           EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0xFF, 0xFF, 0xFF, 0xFF }, 0x40, 4 } },
    {  "arrow",      5,      72.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     true,           EFFECTID::ID_FIRE_ARROWSHOT,    EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_KO_EXPL,    { 0 } },
    {  "arrow",      5,      72.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     true,           EFFECTID::ID_SLEEP_ARROWSHOT,   EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_SLEEPGAS_2, { 0 } },
    {  "grenade",    20,     36.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_EXPL_B1,    { 0 } },
    {  "bomb",       20,     0.0f,      0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_EXPL_B1,    { 0 } },
    {  "shuriken",   5,      54.0f,     0.2f,      0.2f,    0.1f,    CHitAttackData::TARGET_ENEMY_GIMMICK,      true,           EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0x80, 0x80, 0xFF, 0xFF }, 0x40, 4 } },
    {  "shuriken",   5,      54.0f,     0.2f,      0.2f,    0.1f,    CHitAttackData::TARGET_PLAYER,             true,           EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0x80, 0x80, 0xFF, 0xFF }, 0x40, 4 } },
    {  "kunai",      5,      54.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_ENEMY_GIMMICK,      true,           EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0x80, 0x80, 0xFF, 0xFF }, 0x40, 4 } },
    {  "kunai",      5,      54.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_PLAYER,             true,           EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0x80, 0x80, 0xFF, 0xFF }, 0x40, 4 } },
    {  "pack",       5,      54.0f,     0.1f,      0.1f,    0.1f,    CHitAttackData::TARGET_ENEMY_GIMMICK,      false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0x80, 0x80, 0xFF, 0xFF }, 0x40, 4 } },
    {  "shuriken",   5,      54.0f,     0.6f,      0.2f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     true,           EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { { 0x80, 0x80, 0xFF, 0xFF }, 0x40, 4 } },
    {  "fundou",     5,      54.0f,     0.3f,      0.3f,    0.1f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { 0 } },
    {  "scyth",      10,     0.0f,      0.6f,      0.4f,    0.0f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     false,          EFFECTID::ID_SICKLE_WARP,       EFFECTID::ID_UNKNOWN,       EFFECTID::ID_SICKLE_WARP, MAGICID::ID_UNKNOWN,    { 0 } },
    {  "spear",      5,      0.0f,      0.4f,      0.2f,    0.0f,    CHitAttackData::TARGET_PLAYER_GIMMICK,     false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_UNKNOWN,       EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { 0 } },
    {  "ride_shot",  5,      120.0f,    0.4f,      0.2f,    0.0f,    CHitAttackData::TARGET_ENEMY_GIMMICK,      false,          EFFECTID::ID_UNKNOWN,           EFFECTID::ID_LASER_HIT_ALL, EFFECTID::ID_UNKNOWN,     MAGICID::ID_UNKNOWN,    { 0 } },
};


static_assert(COUNT_OF(s_aShotInfo) == SHOTID::ID_MAX, "update me");


static inline const SHOTINFO& ShotInfo(SHOTID::VALUE idShot)
{
    ASSERT(idShot >= 0);
    ASSERT(idShot < SHOTID::ID_MAX);

    return s_aShotInfo[idShot];
};


class CShotArrow : public CShot
{
public:
    CShotArrow(const PARAMETER* pParameter);
    virtual ~CShotArrow(void);
    virtual void SetModel(void) override;
};


class CShotGrenade : public CShot
{
public:
    CShotGrenade(const PARAMETER* pParameter);
    virtual ~CShotGrenade(void);
    virtual void SetModel(void) override;
};


class CShotKnife : public CShot
{
public:
    CShotKnife(const PARAMETER* pParameter);
    virtual ~CShotKnife(void);
    virtual void SetModel(void) override;
};


class CShotScatterKnife : public CShot
{
public:
    CShotScatterKnife(const PARAMETER* pParameter);
    virtual ~CShotScatterKnife(void);
    virtual void SetModel(void) override;
    virtual void Move(void) override;
};


class CShotSpinningWeight : public CShot
{
public:
    CShotSpinningWeight(const PARAMETER* pParameter);
    virtual ~CShotSpinningWeight(void);
    virtual void SetModel(void) override;
};


class CShotScythe : public CShot
{
public:
    CShotScythe(const PARAMETER* pParameter);
    virtual ~CShotScythe(void);
    virtual void SetModel(void) override;
    virtual void Move(void) override;

private:
    float m_fAlpha;
};


class CShotSpear : public CShot
{
public:
    CShotSpear(const PARAMETER* pParameter);
    virtual ~CShotSpear(void);
    virtual void SetModel(void) override;
    virtual void Move(void) override;
};


class CShotRide : public CShot
{
public:
    CShotRide(const PARAMETER* pParameter);
    virtual ~CShotRide(void);
    virtual void Draw(void) override;
};


CShotArrow::CShotArrow(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotArrow::~CShotArrow(void)
{
    ;
};


void CShotArrow::SetModel(void)
{
    RwV3d vMove = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vMove, &m_vVelocity);

    RwV3d vRot = Math::VECTOR3_ZERO;    
    GetRotateFromVector(&vRot, &vMove);
    
    m_pModel->SetRotation(&vRot);
    m_pModel->UpdateFrame();
};


//
// *********************************************************************************
//


CShotGrenade::CShotGrenade(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotGrenade::~CShotGrenade(void)
{
    ;
};


void CShotGrenade::SetModel(void)
{
    RwV3d vMove = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vMove, &m_vVelocity);

    RwV3d vRot = Math::VECTOR3_ZERO;
    GetRotateFromVector(&vRot, &vMove);

    vRot.x += (m_fTimer * (MATH_PI * 6.0f));

    m_pModel->SetRotation(&vRot);
    m_pModel->UpdateFrame();
};


//
// *********************************************************************************
//


CShotKnife::CShotKnife(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotKnife::~CShotKnife(void)
{
    ;
};


void CShotKnife::SetModel(void)
{
    RotateModel(MATH_PI * 8.0f);
};


//
// *********************************************************************************
//


CShotScatterKnife::CShotScatterKnife(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotScatterKnife::~CShotScatterKnife(void)
{
    ;
};


void CShotScatterKnife::SetModel(void)
{
    switch (m_step)
    {
    case STEP_FLY:
        RotateModel(MATH_PI * 8.0f);
        break;

    default:
        break;
    };
};


void CShotScatterKnife::Move(void)
{
    switch (m_step)
    {
    case STEP_FLY:
        {
            if (CheckHit(&m_vPosition))
            {
                float fNormalY = CWorldMap::GetCollisionResultNormal()->y;
                fNormalY = std::fabs(fNormalY);

                if (fNormalY >= 0.1f)
                {
                    m_vVelocity = Math::VECTOR3_ZERO;
                    m_vAcceleration = Math::VECTOR3_ZERO;

                    m_fTimer = 0.0f;
                    m_step = STEP_STOP;
                }
                else
                {
                    m_vVelocity.x = 0.0f;
                    m_vVelocity.z = 0.0f;

                    m_fTimer = 0.0f;
                    m_step = STEP_FALL;
                };
            };
        }
        break;

    case STEP_STOP:
        {
            if (m_fTimer >= m_fLife)
                Finish();
        }
        break;

    case STEP_FALL:
        {
            if (CheckHit(&m_vPosition))
            {
                float fNormalY = CWorldMap::GetCollisionResultNormal()->y;
                fNormalY = std::fabs(fNormalY);

                if (fNormalY >= 0.1f)
                {
                    m_vVelocity = Math::VECTOR3_ZERO;
                    m_vAcceleration = Math::VECTOR3_ZERO;

                    m_fTimer = 0.0f;
                    m_step = STEP_STOP;
                };
            };            
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


//
// *********************************************************************************
//


CShotSpinningWeight::CShotSpinningWeight(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotSpinningWeight::~CShotSpinningWeight(void)
{
    ;
};


void CShotSpinningWeight::SetModel(void)
{
    RotateModel(MATH_PI * 8.0f);
};


//
// *********************************************************************************
//


CShotScythe::CShotScythe(const PARAMETER* pParameter)
: CShot(pParameter)
, m_fAlpha(0.0f)
{
    ;
};


CShotScythe::~CShotScythe(void)
{
    ;
};


void CShotScythe::SetModel(void)
{
    RotateModel(MATH_PI * 8.0f);

    RwRGBA ModelColor = m_pModel->GetColor();
    ModelColor.alpha = static_cast<uint8>(m_fAlpha * 255.0f);
    m_pModel->SetColor(ModelColor);
};


void CShotScythe::Move(void)
{
    float fTimeNow = m_fTimer * 0.2f;

    if (fTimeNow <= m_fLife)
    {
        if ((m_fTimer - fTimeNow) >= m_fLife)
            m_fAlpha = 1.0f;
        else
            m_fAlpha = (m_fTimer - m_fLife) / fTimeNow;
    }
    else
    {
        m_fAlpha = m_fLife / fTimeNow;
    };
};


//
// *********************************************************************************
//


CShotSpear::CShotSpear(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotSpear::~CShotSpear(void)
{
    ;
};


void CShotSpear::SetModel(void)
{
    RwV3d vRot = { -Math::PI05, 0.0f, 0.0f };
    
    m_pModel->SetRotation(&vRot);
    m_pModel->UpdateFrame();
};


void CShotSpear::Move(void)
{
    float dt = CGameProperty::GetElapsedTime();
    
    switch (m_step)
    {
    case STEP_FLY:
        {
            m_vPosition.y -= -1.5f;

            m_vVelocity.x = 0.0f;
            m_vVelocity.y = (dt * 8.33f);
            m_vVelocity.z = 0.0f;

            m_step = STEP_UP;
        }
        break;

    case STEP_STOP:
        {
            if (m_fTimer >= 0.2f)
            {
                m_vVelocity.x = 0.0f;
                m_vVelocity.y = (dt * -8.33f);
                m_vVelocity.z = 0.0f;
                
                m_fTimer = 0.0f;
                m_step = STEP_DOWN;
            };
        }
        break;
        
    case STEP_UP:
        {
            if (m_fTimer >= 0.3f)
            {
                m_vVelocity = Math::VECTOR3_ZERO;
                
                m_fTimer = 0.0f;
                m_step = STEP_STOP;
            };
        }
        break;
        
    case STEP_DOWN:
        {
            if (m_fTimer >= 0.3f)
                Finish();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


//
// *********************************************************************************
//


CShotRide::CShotRide(const PARAMETER* pParameter)
: CShot(pParameter)
{
    ;
};


CShotRide::~CShotRide(void)
{
    ;
};


void CShotRide::Draw(void)
{
    float dt = CGameProperty::GetElapsedTime();

    if (m_fTimer > dt)
    {
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        
        m_pModel->Draw();

        RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
        RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
        RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    };
};


//
// *********************************************************************************
//


/*static*/ CShot* CShot::New(const PARAMETER* pParameter)
{
    ASSERT(pParameter);

    switch (pParameter->m_idShot)
    {
    case SHOTID::ID_ARROW:          return new CShotArrow(pParameter);
    case SHOTID::ID_ARROW_FIRE:     return new CShotArrow(pParameter);
    case SHOTID::ID_ARROW_SLEEP:    return new CShotArrow(pParameter);
    case SHOTID::ID_KUNAI_PLAYER:   return new CShotArrow(pParameter);
    case SHOTID::ID_KUNAI_ENEMY:    return new CShotArrow(pParameter);
    case SHOTID::ID_GRENADE:        return new CShotGrenade(pParameter);
    case SHOTID::ID_KNIFE_PLAYER:   return new CShotKnife(pParameter);
    case SHOTID::ID_KNIFE_ENEMY:    return new CShotKnife(pParameter);
    case SHOTID::ID_SCATTER_KNIFE:  return new CShotScatterKnife(pParameter);
    case SHOTID::ID_SPINNING_WEIGHT:return new CShotSpinningWeight(pParameter);
    case SHOTID::ID_SCYTHE:         return new CShotScythe(pParameter);
    case SHOTID::ID_SPEAR:          return new CShotSpear(pParameter);
    case SHOTID::ID_RIDE:           return new CShotRide(pParameter);
    default:                        return new CShot(pParameter);
    };
};


CShot::CShot(const PARAMETER* pParameter)
: CGameObject(ShotInfo(pParameter->m_idShot).m_pszModelName, GAMEOBJECTTYPE::SHOT)
, m_hParentObj(0)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vVelocity(Math::VECTOR3_ZERO)
, m_vAcceleration(Math::VECTOR3_ZERO)
, m_fTimer(0.0f)
, m_fLife(0.0f)
, m_pModel(nullptr)
, m_apLocus()
, m_fRadius(0.0f)
, m_nPower(0)
, m_uTarget(CHitAttackData::TARGET_NONE)
, m_id(SHOTID::ID_UNKNOWN)
, m_step(STEP_FLY)
, m_hEffect(0)
, m_bReflection(false)
, m_bFinish(false)
{
    m_apLocus[LOCUSKIND_HOR] = nullptr;
    m_apLocus[LOCUSKIND_VER] = nullptr;
    
    ASSERT(pParameter);
    ASSERT(pParameter->m_pObject);

    m_id = pParameter->m_idShot;
    m_fLife = pParameter->m_fLifetime;
    m_vPosition = pParameter->m_vPosition;
    m_hParentObj = pParameter->m_pObject->GetHandle();

    const SHOTINFO& shotInfo = ShotInfo(m_id);

    float dt = CGameProperty::GetElapsedTime();
    Math::Vec3_Normalize(&m_vVelocity, &pParameter->m_vDirection);
    Math::Vec3_Scale(&m_vVelocity, &m_vVelocity, (dt * shotInfo.m_fSpeed) / 4.0f);

    float fGravity = (CGameProperty::GetGravity() * dt);
    m_vAcceleration = { 0.0f, ((dt * fGravity) * shotInfo.m_fGravity), 0.0f };

    m_pModel = CModelManager::CreateModel(shotInfo.m_pszModelName);
    ASSERT(m_pModel);
    m_pModel->SetPosition(&m_vPosition);
    m_pModel->UpdateFrame();

    if (shotInfo.m_locusinfo.m_nPointNum)
    {
		for (int32 i = 0; i < COUNT_OF(m_apLocus); ++i)
		{
			CLocus* pLocus = new CLocus(shotInfo.m_locusinfo.m_nPointNum,
				                        shotInfo.m_locusinfo.m_Color);

			pLocus->SetAlphaBasis(shotInfo.m_locusinfo.m_uAlphaBasis);
			pLocus->SetDrawEnable(true);

			m_apLocus[i] = pLocus;
		};
    };

    if (shotInfo.m_idEffectTrace)
        m_hEffect = CEffectManager::PlayTrace(shotInfo.m_idEffectTrace, new CShotTracer(this), &Math::VECTOR3_ZERO);

    m_fRadius       = shotInfo.m_fRadius;
    m_nPower        = shotInfo.m_nPower;
    m_uTarget       = shotInfo.m_target;
    m_bReflection   = shotInfo.m_bReflection;
};


CShot::~CShot(void)
{
    if (m_hEffect && !CEffectManager::IsEnd(m_hEffect))
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };

    for (int32 i = 0; i < COUNT_OF(m_apLocus); ++i)
    {
        if (m_apLocus[i])
        {
            delete m_apLocus[i];
            m_apLocus[i] = nullptr;
        };
    };

    if (m_pModel)
    {
        CModelManager::DestroyModel(m_pModel);
        m_pModel = nullptr;
    };

    if (m_id == SHOTID::ID_RIDE)
    {
        CGameObject* pParent = CGameObjectManager::GetObject(m_hParentObj);
        if (pParent)
            CGameObjectManager::SendMessage(pParent, RIDETYPES::MESSAGEID_SHOT_EOL);
    };
};


void CShot::Run(void)
{
    if (m_bFinish)
        return;

    Math::Vec3_Add(&m_vVelocity, &m_vVelocity, &m_vAcceleration);
    Math::Vec3_Add(&m_vPosition, &m_vPosition, &m_vVelocity);

    Move();

    CheckAttack();

    m_pModel->SetPosition(&m_vPosition);
    m_pModel->UpdateFrame();

    SetModel();
    SetLocus();

    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer >= m_fLife)
        Finish();
};


void CShot::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT:
        {
            CHitCatchData* pCatch = static_cast<CHitCatchData*>(pParam);
            ASSERT(pCatch);

            if (CheckFinishByHit())
                Finish();
        }
        break;

    case GAMEOBJECTTYPES::MESSAGEID_CATCHATTACK:
        {
            CHitAttackData* pAttack = static_cast<CHitAttackData*>(pParam);
            ASSERT(pAttack);

            if (pAttack->IsFlagReflectShot())
            {
                CGameObject* pParent = CGameObjectManager::GetObject(m_hParentObj);
                if (pParent->GetType() == GAMEOBJECTTYPE::CHARACTER)
                {
                    CCharacter* pCharacter = static_cast<CCharacter*>(pParent);

                    uint32 uTarget = CHitAttackData::TARGET_NONE;

                    if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
                        uTarget = CHitAttackData::TARGET_ENEMY;
                    else
                        uTarget = CHitAttackData::TARGET_PLAYER;

                    if (m_uTarget & uTarget)
                    {
                        m_vVelocity.x *= -1.0f;
                        m_vVelocity.y = 0.0f;
                        m_vVelocity.z *= -1.0f;

                        if (m_uTarget == CHitAttackData::TARGET_ENEMY_GIMMICK)
                            m_uTarget = CHitAttackData::TARGET_PLAYER_GIMMICK;
                        else if (m_uTarget == CHitAttackData::TARGET_PLAYER_GIMMICK)
                            m_uTarget = CHitAttackData::TARGET_ENEMY_GIMMICK;

                        CEffectManager::Play(EFFECTID::ID_MIC_REFLECT, &m_vPosition);
                    };
                };
            };            
        }
        break;

    default:
        CGameObject::MessageProc(nMessageID, pParam);
        Finish();
        break;
    };
};


void CShot::Draw(void)
{
    ASSERT(m_pModel);
    m_pModel->Draw();

    for (int32 i = 0; i < COUNT_OF(m_apLocus); ++i)
    {
        if (m_apLocus[i])
            m_apLocus[i]->Draw();
    };
};


void CShot::SetModel(void)
{
    ;
};


void CShot::Move(void)
{
    if (CheckHit(&m_vPosition))
    {
        m_vVelocity = Math::VECTOR3_ZERO;
        m_vAcceleration = Math::VECTOR3_ZERO;
        Finish();
    };
};


void CShot::Kill(void)
{
    Finish();
};


void CShot::Finish(void)
{
    if (ShotInfo(m_id).m_idEffectVanish)
        CEffectManager::Play(ShotInfo(m_id).m_idEffectVanish, &m_vPosition);

    if (ShotInfo(m_id).m_idMagicHit)
    {
        CMagicManager::CParameter param;
        param.SetPositon(&m_vPosition);
        param.SetDirection(&m_vVelocity);
        param.SetObject(this);
        
        uint32 hMagic = CMagicManager::Play(ShotInfo(m_id).m_idMagicHit, &param);
        ASSERT(hMagic);

        if (m_id == SHOTID::ID_ARROW_FIRE)
            CMagicManager::SetScale(hMagic, 0.4f);        
    };

    m_bFinish = true;
};


bool CShot::IsEnd(void) const
{
    return m_bFinish;
};


void CShot::CheckAttack(void)
{
    {
        RwV3d vObjPos = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vObjPos, &m_vPosition, &m_vVelocity);

        CHitAttackData Attack;
        Attack.SetObject(GetHandle());
        Attack.SetObjectPos(&vObjPos);

        if (m_id == SHOTID::ID_RIDE)
        {
            RwLine line = { 0 };
            line.start = m_vPosition;
            line.end = vObjPos;

            Attack.SetShape(CHitAttackData::SHAPE_LINE);
            Attack.SetLine(&line);
        }
        else
        {
            RwSphere sphere = { 0 };
            SetHitSphere(&sphere);

            Attack.SetShape(CHitAttackData::SHAPE_SPHERE);
            Attack.SetSphere(&sphere);
        };

        Attack.SetPower(m_nPower);
        Attack.SetTarget(CHitAttackData::TARGET(m_uTarget));
        Attack.SetAntiguard(CHitAttackData::ANTIGUARD_ENABLE);

        switch (m_id)
        {
        case SHOTID::ID_SCATTER_KNIFE:
            Attack.SetStatus(CHitAttackData::STATUS_DINDLE);
            Attack.SetTroubleParameter(2.0f);
            break;

        case SHOTID::ID_SCYTHE:
            Attack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
            break;

        default:
            Attack.SetStatus(CHitAttackData::STATUS_KNOCK);
            break;
        };

        CHitAttackManager::RegistAttack(&Attack);
    };

    if (m_bReflection)
    {
        RwSphere sphere = { 0 };
        SetHitSphere(&sphere);

        CHitCatchData Catch;
        Catch.SetObject(GetHandle());
        Catch.SetObjectType(GetType());
        Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
        Catch.SetSphere(&sphere);

        CHitAttackManager::RegistCatch(&Catch);
    };
};


bool CShot::CheckFinishByHit(void)
{
    if ((m_id == SHOTID::ID_SCYTHE) ||
        (m_id == SHOTID::ID_SPEAR))
    {
        return false;
    };

    return true;
};


bool CShot::CheckHit(RwV3d* pvCollisionPoint)
{
    float fSpeed = Math::Vec3_Length(&m_vVelocity);
    
    if (fSpeed == 0.0f)
        return false;

    RwV3d vPtStart = m_vPosition;
    RwV3d vPtEnd = m_vPosition;
    
    Math::Vec3_Sub(&vPtStart, &vPtStart, &m_vVelocity);

    if (CWorldMap::CheckCollisionLine(&vPtStart, &vPtEnd))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        if (pResult->m_attribute == MAPTYPES::ATTRIBUTE_ONEWAY)
            return false;

        RwV3d vPosition = pResult->m_vClosestPt;
        
        if (pvCollisionPoint)
            *pvCollisionPoint = vPosition;

        if (ShotInfo(m_id).m_idEffectHit)
            CEffectManager::Play(ShotInfo(m_id).m_idEffectHit, &vPosition);

        return true;
    };

    return false;
};


void CShot::SetHitSphere(RwSphere* pSphere)
{
    ASSERT(pSphere);
    ASSERT(m_pModel);
    
    if (m_id == SHOTID::ID_SCYTHE)
    {
        RwV3d vRotation = Math::VECTOR3_ZERO;
        m_pModel->GetRotation(&vRotation);
        
        pSphere->center = m_vPosition;

        pSphere->center.x += Math::Sin(vRotation.y) * 1.2f;
        pSphere->center.z += Math::Cos(vRotation.y) * 1.2f;
    }
    else
    {
        pSphere->center = m_vPosition;
    };

    pSphere->radius = ShotInfo(m_id).m_fHitRadius;
};


void CShot::SetLocus(void)
{
    if (m_apLocus[LOCUSKIND_HOR] &&
        m_apLocus[LOCUSKIND_VER])
    {
        RwV3d vMove = Math::VECTOR3_ZERO;
        Math::Vec3_Normalize(&vMove, &m_vVelocity);

        RwV3d vVer = Math::VECTOR3_ZERO;
        RwV3d vHor = Math::VECTOR3_ZERO;
        Math::Vec3_Cross(&vHor, &Math::VECTOR3_AXIS_Y, &vMove);
        Math::Vec3_Cross(&vVer, &vHor, &vMove);
        Math::Vec3_Scale(&vHor, &vHor, m_fRadius);
        Math::Vec3_Scale(&vVer, &vVer, m_fRadius);
        
        RwV3d vPtHigh = Math::VECTOR3_ZERO;
        RwV3d vPtLow = Math::VECTOR3_ZERO;

        Math::Vec3_Add(&vPtHigh, &m_vPosition, &vHor);
        Math::Vec3_Sub(&vPtLow, &m_vPosition, &vHor);
        m_apLocus[LOCUSKIND_HOR]->SetPosition(&vPtHigh, &vPtLow);

        Math::Vec3_Add(&vPtHigh, &m_vPosition, &vVer);
        Math::Vec3_Sub(&vPtLow, &m_vPosition, &vVer);
        m_apLocus[LOCUSKIND_VER]->SetPosition(&vPtHigh, &vPtLow);
    };
};


void CShot::RotateModel(float fSpeed)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pModel->GetRotation(&vRotation);

	vRotation.y += (CGameProperty::GetElapsedTime() * fSpeed);
    
    m_pModel->SetRotation(&vRotation);
    m_pModel->UpdateFrame();
};


float CShot::GetDirection(void) const
{    
    RwV3d vDirection = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vDirection, &m_vVelocity);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    GetRotateFromVector(&vRotation, &vDirection);
    
    return vRotation.y;
};


void CShot::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);
    *pvPosition = m_vPosition;
};


void CShot::GetVelocity(RwV3d* pvVelocity) const
{
    ASSERT(pvVelocity);
    *pvVelocity = m_vVelocity;
};


void CShot::GetAcceleration(RwV3d* pvAcceleration) const
{
    ASSERT(pvAcceleration);
    *pvAcceleration = m_vAcceleration;
};


uint32 CShot::GetParentHandle(void) const
{
    return m_hParentObj;
};


SHOTID::VALUE CShot::GetID(void) const
{
    return m_id;
};