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



/*static*/ const char* CBarrierModule::BARRIER_MOTIONNAME = "Barrier";
/*static*/ CList<CBarrierModule> CBarrierModule::m_listDraw;


/*static*/ void CBarrierModule::DrawAll(void)
{
    for (CBarrierModule& it : m_listDraw)
        it.DrawBarrier();
};


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
    m_pBarrierSphere = new CSphere(m_fRadius, 16, 16, 4, 16, { 0xFF, 0xFF, 0xFF, 0x80 });
    m_state = STATE_SLEEP;

    CTextureManager::SetCurrentTextureSet("common_ef");
    m_pTexture = CTextureManager::GetRwTexture("barrier");
    ASSERT(m_pTexture);
    m_pBarrierSphere->SetTexture(m_pTexture);

    m_pDummy = new CGameObject("Dummy", GAMEOBJECTTYPE::EFFECT);
    
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
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_bDisplay)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pTracer->GetPosition(&vPosition);
        
        RwV3d vScale = { m_fScale, m_fScale, m_fScale };
        m_pBarrierSphere->SetScale(&vScale);
        m_pBarrierSphere->SetPosition(&vPosition);

        RwSphere sphere = {};
        sphere.center = vPosition;
        sphere.radius = (m_fRadius * m_fScale);

        {
            CHitAttackData hitAttack;
            hitAttack.SetObject(m_pObject->GetHandle());
            hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);
            hitAttack.SetSphere(&sphere);
            hitAttack.SetObjectPos(&vPosition);
            hitAttack.SetPower(m_nPower);
            hitAttack.SetTarget(m_target);
            hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
            hitAttack.SetMotion(BARRIER_MOTIONNAME);
            hitAttack.SetStatus(CHitAttackData::STATUS_FLYAWAY);

            if (m_pObject->GetType() == GAMEOBJECTTYPE::CHARACTER)
            {
                CCharacter* pCharacter = static_cast<CCharacter*>(m_pObject);

                if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
                {
                    CPlayerCharacter* pPlayerCharacter = static_cast<CPlayerCharacter*>(pCharacter);
                    
                    if (pPlayerCharacter->GetID() == PLAYERID::ID_SPL)
                        hitAttack.SetFlyawayParameter(0.0f, 8.0f);
                };
            };

            CHitAttackManager::RegistAttack(&hitAttack);
        }

        {
            CHitCatchData hitCatch;
            hitCatch.SetObject(m_pDummy->GetHandle());
            hitCatch.SetObjectType(m_pDummy->GetType());
            hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
            hitCatch.SetSphere(&sphere);
            
            CHitAttackManager::RegistCatch(&hitCatch);
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
    if (m_bDisplay)
    {
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);

        m_pBarrierSphere->Draw();

        RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    };
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

    CEffectManager::Play(EFFECTID::ID_BARRIER_START, &vPosition);
};


void CBarrierModule::Disappear(void)
{
    if (m_state != STATE_SLEEP)
    {
        m_state = STATE_DISAPPEAR;
        m_fScale = 0.0f;
        m_bDisplay = true;
    };
};


CBarrierModule::STATE CBarrierModule::GetState(void) const
{
    return m_state;
};