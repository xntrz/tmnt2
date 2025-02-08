#include "BridgePillarGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickMotion.hpp"
#include "Game/Component/Gimmick/MoveStrategy/CuttingGimmickMove.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Motion/MotionManager.hpp"


CBridgePillarGimmick::CBridgePillarGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pCuttingMove(nullptr)
, m_model()
, m_fTimer(0.0f)
, m_fEffectTimer(0.0f)
, m_fInvincibleTimer(0.0f)
, m_eState(STATE_NONE)
, m_step(0)
, m_iLife(10)
, m_hAtari(0)
, m_pGimmickMotion(nullptr)
{
    init(pParam);
};


CBridgePillarGimmick::~CBridgePillarGimmick(void)
{
    if (m_pCuttingMove)
    {
        delete m_pCuttingMove;
        m_pCuttingMove = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);        
        m_hAtari = 0;
    };

    if (m_pGimmickMotion)
    {
        delete m_pGimmickMotion;
        m_pGimmickMotion = nullptr;
    };
};


bool CBridgePillarGimmick::Query(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);

    if (pQuery->GetType() == GIMMICKTYPES::QUERY_EVE_BROKEN)
    {
        CStateGimmickQuery* pStateQuery = (CStateGimmickQuery*)pQuery;
        ++pStateQuery->m_nTarget;

        if (m_eState == STATE_END)
        {
            ++pStateQuery->m_nState;
            return true;
        };
    };

    return CGimmick::Query(pQuery);
};


void CBridgePillarGimmick::PostMove(void)
{
    switch (m_eState)
    {
    case STATE_NONE:
        {
            m_eState = STATE_WAIT;
        }
        break;
        
    case STATE_WAIT:
        {
            waiting();
        }
        break;

    case STATE_CUT:
        {
            cutting();
        }
        break;

    case STATE_DISAPPEAR:
        {
            breakdown();
        }
        break;
    };
};


void CBridgePillarGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_eState == STATE_WAIT)
            onCut();
    };
};


void CBridgePillarGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_eState == STATE_WAIT)
    {
        if (pAttack->IsFlagSlice() || (--m_iLife <= 0))
        {
            onCut();
            CGimmickUtils::PlayHitEffect(pAttack);
        }
        else
        {
            m_fInvincibleTimer = 1.0f;
        };
    };
};


void CBridgePillarGimmick::init(void* pParam)
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    ASSERT(pInitParam);

    CModel* pModel = CModelManager::CreateModel("bpillar");
    ASSERT(pModel);
    pModel->SetBoundingSphereRadius(20.0f);

    CModel* pBreakModel = CModelManager::CreateModel("bpillar_b");
    ASSERT(pBreakModel);
    pBreakModel->SetClippingEnable(false);
    
    CModel* pAtariModel = CModelManager::CreateModel("bpillar_a");
    ASSERT(pAtariModel);

    CModel* pBreakAtariModel = CModelManager::CreateModel("bpillar_b_a");
    ASSERT(pBreakAtariModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pBreakModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_BREAK, pBreakAtariModel);
    m_model.SetPosition(&pInitParam->m_vPosition);
    RwV3d vRot = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };
    m_model.SetRotation(&vRot);
    SetModelStrategy(&m_model);
    
    m_pCuttingMove = new CCuttingGimmickMove();
    ASSERT(m_pCuttingMove);
    m_pCuttingMove->SetPosition(&pInitParam->m_vPosition);
    m_pCuttingMove->SetRotation(&vRot);
    m_pCuttingMove->Start();
    SetMoveStrategy(m_pCuttingMove);

    if (m_model.GetCollisionModelClump())
    {
        m_hAtari = CMapCollisionModel::RegistCollisionModel(
            m_model.GetCollisionModelClump(),
            GetName(),
            MAPTYPES::GIMMICKTYPE_NORMAL
        );
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 20.0f);
    };

    m_pGimmickMotion = new CGimmickMotion(pBreakModel);
    ASSERT(m_pGimmickMotion);
    CMotionManager::SetCurrentMotionSet("bridgepillar");
    m_pGimmickMotion->AddMotion("breakdown");
    m_pGimmickMotion->SetMotion("breakdown", 0.0f, 0.0f, 0.0f, false);
};


void CBridgePillarGimmick::onCut(void)
{
    m_eState = STATE_CUT;
};


void CBridgePillarGimmick::onBreakdown(void)
{
    m_eState = STATE_DISAPPEAR;
    m_model.SetVisualBreak();
    m_pGimmickMotion->StartOne(1.0f);
    CGameSound::PlayObjectSE(this, SDCODE_SE(0x103F));
};


void CBridgePillarGimmick::waiting(void)
{
    if (m_fInvincibleTimer <= 0.0f)
    {
        RwSphere sphere = { 0 };
        sphere.radius = 1.0f;

        ASSERT(m_pCuttingMove);
        m_pCuttingMove->GetPosition(&sphere.center);
        sphere.center.y += 1.0f;

        CHitCatchData Catch;
        Catch.SetObject(GetHandle());
        Catch.SetObjectType(GetType());
        Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
        Catch.SetSphere(&sphere);

        CHitAttackManager::RegistCatch(&Catch);
    }
    else
    {
        m_fInvincibleTimer -= CGameProperty::GetElapsedTime();
    };
};


void CBridgePillarGimmick::cutting(void)
{
    onBreakdown();
};


void CBridgePillarGimmick::breakdown(void)
{
    switch (m_step)
    {
    case 0:
        {
            m_fEffectTimer += CGameProperty::GetElapsedTime();
            if (m_fEffectTimer > 0.08f)
            {
                RwV3d vPos = Math::VECTOR3_ZERO;
                m_pCuttingMove->GetPosition(&vPos);
                vPos.y += 0.4f;
                
                CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vPos);
                
                m_fEffectTimer -= 0.08f;
            };

            if (m_fTimer > 2.2f)
            {
                setBreakdownSmokeEffect(m_step);
                setBreakdownHitAttack(m_step);
                CGameProperty::SetCameraVibration(0.1f, 0.2f, 10);
                CGameSound::PlayObjectSE(this, SDCODE_SE(0x103E));
                m_fEffectTimer = 0.0f;
                ++m_step;
            };
        }
        break;

    case 1:
        {
            if (m_fTimer > 2.8f)
            {
                setBreakdownSmokeEffect(m_step);
                setBreakdownHitAttack(m_step);
                CGameSound::PlayObjectSE(this, SDCODE_SE(0x103D));

                if (m_hAtari)
                {
                    CMapCollisionModel::RemoveCollisionModel(m_hAtari);
                    m_hAtari = 0;
                };

                m_model.SetCollisionBreak();
                if (m_model.GetCollisionModelClump())
                {
                    m_hAtari = CMapCollisionModel::RegistCollisionModel(
                        m_model.GetCollisionModelClump(),
                        GetName(),
                        MAPTYPES::GIMMICKTYPE_NORMAL
                    );
                    if (m_hAtari)
                        CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 20.0f);
                };

                CGameProperty::SetCameraVibration(0.2f, 0.3f, 10);

                ++m_step;
            };
        }
        break;
    };

    m_pGimmickMotion->Update();
    m_fTimer += CGameProperty::GetElapsedTime();

    if (m_pGimmickMotion->GetEndTime() < m_fTimer)
    {
        m_step = 0;
        m_fTimer = 0.0f;
        m_eState = STATE_END;
    };
};


void CBridgePillarGimmick::setBreakdownHitAttack(int32 Step)
{
    const int32 aAttackNum[] = { 1, 15 };

    ASSERT((Step >= 0) && (Step < COUNT_OF(aAttackNum)));

    int32 AttackNum = aAttackNum[Step];

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetRotation(&vRotation);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetPosition(&vPosition);

    vRotation.x = Math::Sin(vRotation.y);
    vRotation.z = Math::Cos(vRotation.y);
    vRotation.y = 0.0f;

    RwV3d vStep = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vStep, &vRotation);
    Math::Vec3_Add(&vPosition, &vPosition, &vStep);
    Math::Vec3_Add(&vPosition, &vPosition, &vStep);

    for (int32 i = 0; i < AttackNum; ++i)
    {
        RwSphere Sphere = { vPosition, 1.0f };

        CHitAttackData Attack;
        Attack.SetObject(GetHandle());
        Attack.SetShape(CHitAttackData::SHAPE_SPHERE);
        Attack.SetSphere(&Sphere);
        Attack.SetObjectPos(&vPosition);
        Attack.SetPower(50);
        Attack.SetAttackNo(i);
        Attack.SetTarget(CHitAttackData::TARGET_PLAYER | CHitAttackData::TARGET_ENEMY);
        Attack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        Attack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        
        CHitAttackManager::RegistAttack(&Attack);

        Math::Vec3_Add(&vPosition, &vPosition, &vStep);
    };
};


void CBridgePillarGimmick::setBreakdownSmokeEffect(int32 Step)
{
    const int32 aSmokeNum[] = { 2, 10 };
    const float aSmokeScale[] = { 2.0f, 4.0f };

    ASSERT((Step >= 0) && (Step < COUNT_OF(aSmokeNum)));
    ASSERT((Step >= 0) && (Step < COUNT_OF(aSmokeScale)));

    int32 SmokeNum = aSmokeNum[Step];
    float EffectScale = aSmokeScale[Step];

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetRotation(&vRotation);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetPosition(&vPosition);

    vRotation.x = Math::Sin(vRotation.y);
    vRotation.z = Math::Cos(vRotation.y);
    vRotation.y = 0.0f;

    RwV3d vStep = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vStep, &vRotation);
    Math::Vec3_Scale(&vStep, &vStep, 1.5f);

    for (int32 i = 0; i < SmokeNum; ++i)
    {
        CWorldMap::GetMapHeight(&vPosition);
        
        const CWorldMap::COLLISIONRESULT* pCollisonResult = CWorldMap::GetCollisionResult();
        ASSERT(pCollisonResult);

        if (pCollisonResult->m_attribute != MAPTYPES::ATTRIBUTE_DEATH)
        {
            uint32 hFx = CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vPosition);
            if (hFx)
                CEffectManager::SetScale(hFx, EffectScale);
        };

        Math::Vec3_Add(&vPosition, &vPosition, &vStep);
    };
};