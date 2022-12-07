#include "BarrierModule.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Effect/Tracer.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Particle/Sphere.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "System/Common/RenderState.hpp"


/*static*/ void CBarrierModule::DrawAll(void)
{
    for (CBarrierModule& it : m_listDraw)
        it.DrawBarrier();
};


/*static*/ CList<CBarrierModule> CBarrierModule::m_listDraw;


CBarrierModule::CBarrierModule(CTracer* pTracer, CGameObject* pObject, float fRadius)
: IModule(MODULETYPE::BARRIER)
, m_pBarrierSphere(nullptr)
, m_pDummy(nullptr)
, m_pTracer(nullptr)
, m_pObject(nullptr)
, m_pTexture(nullptr)
, m_state(STATE_APPEAR)
, m_fRadius(0.0f)
, m_fScale(0.0f)
, m_bDisplay(false)
, m_target(CHitAttackData::TARGET_NONE)
, m_nPower(0)
, m_fTime(0.0f)
, m_fLivetime(0.0f)
{
    ASSERT(pTracer);
    ASSERT(pObject);

    m_fRadius = fRadius;
    m_pTracer = pTracer;
    m_pObject = pObject;

    m_pBarrierSphere = new CSphere(m_fRadius, 16, 16, 4, 6, { 0xFF, 0xFF, 0xFF, 0x80 });
    ASSERT(m_pBarrierSphere);

    m_state = STATE_SLEEP;

    CTextureManager::SetCurrentTextureSet("common_ef");
    m_pTexture = CTextureManager::GetRwTexture("barrier");
    ASSERT(m_pTexture);
    m_pBarrierSphere->SetTexture(m_pTexture);

    m_pDummy = new CGameObject("Dummy", GAMEOBJECTTYPE::EFFECT);
    ASSERT(m_pDummy);

    m_fTime = 0.0f;
    m_nPower = 5;
    m_fLivetime = 2.0f;
    m_target = CHitAttackData::TARGET_ENEMY_GIMMICK;

    m_nodeDraw.data = this;
    m_listDraw.push_back(&m_nodeDraw);
};


CBarrierModule::~CBarrierModule(void)
{
    ASSERT(!m_listDraw.empty());
    m_listDraw.erase(&m_nodeDraw);

    if (m_pBarrierSphere)
    {
        delete m_pBarrierSphere;
        m_pBarrierSphere = nullptr;
    };

    if (m_pDummy)
    {
        delete m_pDummy;
        m_pDummy = nullptr;
    };

    if (m_pTracer)
    {
        delete m_pTracer;
        m_pTracer = nullptr;
    };
};


void CBarrierModule::Run(void)
{
    m_fTime += CGameProperty::GetElapsedTime();

    switch (m_state)
    {
    case STATE_APPEAR:
        {
            m_fScale += (CGameProperty::GetElapsedTime() * 3.0f);
            if (m_fScale >= 1.0f)
            {
                m_fScale = 1.0f;
                m_state = STATE_IDLE;
                m_fTime = 0.0f;
            };
        }
        break;

    case STATE_IDLE:
        {
            if (m_fTime > m_fLivetime)
            {
                m_state = STATE_DISAPPEAR;
                m_fScale = 1.0f;
                m_bDisplay = true;
            };
        }
        break;

    case STATE_DISAPPEAR:
        {
            m_fScale -= CGameProperty::GetElapsedTime() * 3.0f;
            if (m_fScale <= 0.0f)
            {
                m_fTime = 0.0f;
                m_fScale = 0.0f;
                m_state = STATE_SLEEP;
                m_bDisplay = false;
            };
        }
        break;

    case STATE_SLEEP:
        {
            ;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_bDisplay)
    {
        RwV3d vScale = { m_fScale, m_fScale, m_fScale };
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pTracer->GetPosition(&vPosition);
        
        m_pBarrierSphere->SetScale(&vScale);
        m_pBarrierSphere->SetPosition(&vPosition);

        RwSphere sphere = { 0 };
        sphere.center = vPosition;
        sphere.radius = (m_fRadius * m_fScale);

        {
            ASSERT(m_pObject);
            
            CHitAttackData Attack;
            Attack.SetObject(m_pObject->GetHandle());
            Attack.SetShape(CHitAttackData::SHAPE_SPHERE);
            Attack.SetSphere(&sphere);
            Attack.SetObjectPos(&vPosition);
            Attack.SetPower(m_nPower);
            Attack.SetTarget(m_target);
            Attack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
            Attack.SetMotion("Barrier");
            Attack.SetStatus(CHitAttackData::STATUS_FLYAWAY);

            if (m_pObject->GetType() == GAMEOBJECTTYPE::CHARACTER)
            {
                CCharacter* pCharacter = (CCharacter*)m_pObject;
                if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
                {
                    CPlayerCharacter* pPlayerCharacter = (CPlayerCharacter*)pCharacter;
                    if (pPlayerCharacter->GetID() == PLAYERID::ID_SPL)
                        Attack.SetFlyawayParameter(0.0f, 8.0f);
                };
            };

            CHitAttackManager::RegistAttack(&Attack);
        }

        {
            ASSERT(m_pDummy);
            
            CHitCatchData Catch;
            Catch.SetObject(m_pDummy->GetHandle());
            Catch.SetObjectType(m_pDummy->GetType());
            Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
            Catch.SetSphere(&sphere);
            
            CHitAttackManager::RegistCatch(&Catch);
        }

        m_pBarrierSphere->Run(CGameProperty::GetElapsedTime());
    };
};


void CBarrierModule::Draw(void)
{
    ;
};


void CBarrierModule::DrawBarrier(void)
{
    if (!m_bDisplay)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    
    ASSERT(m_pBarrierSphere);
    m_pBarrierSphere->Draw();
    
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


void CBarrierModule::SetLive(float fLivetime)
{
    m_fLivetime = fLivetime;
};


void CBarrierModule::SetHitTarget(CHitAttackData::TARGET target)
{
    m_target = target;
};


void CBarrierModule::SetStepOne(void)
{
    m_nPower = 10;
};


void CBarrierModule::SetStepTwo(void)
{
    m_nPower = 20;
};


void CBarrierModule::SetStepThree(void)
{
    m_nPower = 30;
};


void CBarrierModule::Appear(void)
{
    ASSERT(m_pBarrierSphere);
    ASSERT(m_pTracer);
    
    m_state = STATE_APPEAR;
    m_fScale = 0.0f;
    m_bDisplay = true;
    m_pBarrierSphere->ResetUV();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pTracer->GetPosition(&vPosition);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_BARRIER_START, &vPosition);
    ASSERT(hEffect);
};


void CBarrierModule::Disappear(void)
{
    if (m_state != STATE_SLEEP)
    {
        m_state = STATE_SLEEP;
        m_fScale = 0.0f;
        m_bDisplay = true;
    };
};


CBarrierModule::STATE CBarrierModule::GetState(void) const
{
    return m_state;
};