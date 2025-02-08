#include "PillarGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/PillarGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CPillarGimmick::CPillarGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pPillarMove(nullptr)
, m_model()
, m_vPrePosition(Math::VECTOR3_ZERO)
, m_vOrgPosition(Math::VECTOR3_ZERO)
, m_fTimer(0.0f)
, m_fRad(0.0f)
, m_fEffectTime(0.0f)
, m_step(0)
, m_hAtari(0)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    m_vOrgPosition = pBasicParam->m_vPosition;
    m_step = (GetID() == GIMMICKID::ID_N_SHKPIL ? SHKSTEP_IDLE : BRKSTEP_IDLE);
    
    //
    //  init normal disp model
    //
    CModel* pModel = CModelManager::CreateModel("capital");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init normal atari model
    //
    CModel* pAtariModel = CModelManager::CreateModel("capital_a");
    ASSERT(pAtariModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);

    //
    //  init movement
    //
    m_pPillarMove = new CPillarGimmickMove;
    
    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);

    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.SetRotation(&vRotation);

    m_pPillarMove->SetPosition(&pBasicParam->m_vPosition);
    m_pPillarMove->SetRotation(&vRotation);
    m_pPillarMove->Start();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_PILLAR);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 5.0f);
    };

    //
    //  setup model & move strat
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pPillarMove);
};


CPillarGimmick::~CPillarGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    if (m_pPillarMove)
    {
        delete m_pPillarMove;
        m_pPillarMove = nullptr;
    };
};


void CPillarGimmick::PreMove(void)
{
    m_pPillarMove->GetPosition(&m_vPrePosition);

    float dt = CGameProperty::GetElapsedTime();

    switch (GetID())
    {
    case GIMMICKID::ID_N_BRKPIL:
        crumble(dt);
        break;

    case GIMMICKID::ID_N_SHKPIL:
        shake(dt);
        break;

    default:
        break;
    };
};


void CPillarGimmick::PostMove(void)
{
    if (!m_hAtari)
        return;

    RwV3d vPosNow = Math::VECTOR3_ZERO;
    m_pPillarMove->GetPosition(&vPosNow);

    RwV3d vPosDlt = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vPosDlt, &vPosNow, &m_vPrePosition);

    float fDltMove = Math::Vec3_Length(&vPosDlt);
    
    if (fDltMove > 0.0f)
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, &vPosDlt);
};


void CPillarGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    switch (GetID())
    {
    case GIMMICKID::ID_N_BRKPIL:
        onCrumble();
        break;

    case GIMMICKID::ID_N_SHKPIL:
        onShake();
        break;

    default:
        break;
    };
};


void CPillarGimmick::onShake(void)
{
    if (m_step == SHKSTEP_IDLE)
        m_step = SHKSTEP_MOVE;
};


void CPillarGimmick::onCrumble(void)
{
    if (m_step == BRKSTEP_IDLE)
    {
        m_step = BRKSTEP_STEADY;
        
        RwV3d vVelocity = { 0.0f, -5.0f, 0.0f };
        m_pPillarMove->SetVelocity(&vVelocity);

        bool bSoundCall = true;        
        RwV3d vPosition = m_vPrePosition;
        vPosition.y -= 0.8f;

        uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vPosition, bSoundCall);
        ASSERT(hEffect);

        if (hEffect)
            CEffectManager::SetScale(hEffect, 2.5f);

        CGameProperty::SetCameraVibration(0.1f, 0.5f, 10);
        CGameSound::PlayObjectSE(this, SDCODE_SE(4253));
    };
};


void CPillarGimmick::shake(float dt)
{
    if (m_step != SHKSTEP_MOVE)
        return;
    
    float fRadianPerFrame = dt * 30.0f;

    m_fEffectTime += dt;
    
    if (m_fEffectTime >= 0.25f)
    {
        bool bSoundCall = true;
        
        RwV3d vPosEff = m_vPrePosition;
        vPosEff.y -= 1.0f;

        uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vPosEff, bSoundCall);
        ASSERT(hEffect);

        if (hEffect)
            CEffectManager::SetScale(hEffect, 2.0f);

        m_fEffectTime -= 0.25f;
    };
    
    RwV3d vShakeDir = Math::VECTOR3_ZERO;
    m_pPillarMove->GetLook(&vShakeDir);
    vShakeDir.y = 0.0f;

    Math::Vec3_Normalize(&vShakeDir, &vShakeDir);

    RwV3d vPosPre = Math::VECTOR3_ZERO;
    vPosPre.x = (vShakeDir.x * (Math::Sin(m_fRad) * 8.0f)) + (vShakeDir.z * (Math::Sin(m_fRad * 2.0f) * 2.0f));
    vPosPre.y = (vShakeDir.y * (Math::Sin(m_fRad) * 8.0f)) + (vShakeDir.y * (Math::Sin(m_fRad * 2.0f) * 2.0f));
    vPosPre.z = (vShakeDir.z * (Math::Sin(m_fRad) * 8.0f)) * (-vShakeDir.x * (Math::Sin(m_fRad * 2.0f) * 2.0f));

    bool bShakeEnd = false;

    m_fRad += fRadianPerFrame;
    if (m_fRad > Math::PI2)
    {
        m_fRad -= Math::PI2;
        
        if (m_fTimer > 1.0f)
            bShakeEnd = true;
    };

    if (!bShakeEnd)
    {
        RwV3d vPosPost = Math::VECTOR3_ZERO;
        vPosPost.x = (vShakeDir.x * (Math::Sin(m_fRad) * 8.0f)) + (vShakeDir.z * (Math::Sin(m_fRad * 2.0f) * 2.0f));
        vPosPost.y = (vShakeDir.y * (Math::Sin(m_fRad) * 8.0f)) + (vShakeDir.y * (Math::Sin(m_fRad * 2.0f) * 2.0f));
        vPosPost.z = (vShakeDir.z * (Math::Sin(m_fRad) * 8.0f)) * (-vShakeDir.x * (Math::Sin(m_fRad * 2.0f) * 2.0f));

        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vVelocity, &vPosPost, &vPosPre);

        m_pPillarMove->SetVelocity(&vVelocity);
    }
    else
    {
        m_pPillarMove->SetVelocity(&Math::VECTOR3_ZERO);
        m_pPillarMove->SetPosition(&m_vOrgPosition);

        m_fRad = 0.0f;
        m_fTimer = 0.0f;
        m_fEffectTime = 0.0f;
        m_step = 0;
    };

    m_fTimer += dt;
};


void CPillarGimmick::crumble(float dt)
{
    if (m_step > BRKSTEP_IDLE)
        m_fTimer += dt;

    switch (m_step)
    {
    case BRKSTEP_IDLE:
        {
            ;
        }
        break;

    case BRKSTEP_STEADY:
        {
            if (m_fTimer > 0.1f)
            {
                RwV3d vVelocity = { 0.0f, -1.0f, 0.0f };
                m_pPillarMove->SetVelocity(&vVelocity);

				m_step = BRKSTEP_MOVE;
            };
        }
        break;

    case BRKSTEP_MOVE:
        {
            if (m_fEffectTime > 0.4f)
            {
                bool bSoundCall = true;

                RwV3d vEffPos = m_vPrePosition;
                vEffPos.y -= 1.0f;

                uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &vEffPos, bSoundCall);
                ASSERT(hEffect);

                if (hEffect)
                    CEffectManager::SetScale(hEffect, 2.0f);

                m_fEffectTime -= 0.4f;
            };

            m_fEffectTime += dt;

            if (m_fTimer > 15.0f)
            {
                m_pPillarMove->SetVelocity(&Math::VECTOR3_ZERO);
                CGameSound::FadeOutSE(SDCODE_SE(4253), CGameSound::FADESPEED_NORMAL);
                m_step = BRKSTEP_END;
            };
        }
        break;

    case BRKSTEP_END:
        {
            Release();
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


CSwitchPillarGimmick::CSwitchPillarGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_hAtari(0)
, m_vOrgPosition(Math::VECTOR3_ZERO)
, m_eState(STATE_OFF)
, m_nRidePlayer(0)
, m_aszOnPlayerName()
, m_aszPreOnPlayerName()
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    m_vOrgPosition = pBasicParam->m_vPosition;

    //
    //  init OFF model
    //
    CModel* pModel = CModelManager::CreateModel("capital_off");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    
    //
    //  init ON model
    //
    pModel = CModelManager::CreateModel("capital_on");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pModel);
    
    //
    //  init ATARI model
    //
    CModel* pAtariModel = CModelManager::CreateModel("capital_a");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);

    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.UpdateFrame();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_SWITCH_PILLAR);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 5.0f);
    };

    //
    //  setup model & move strategy
    //
    SetModelStrategy(&m_model);
};


CSwitchPillarGimmick::~CSwitchPillarGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CSwitchPillarGimmick::GetPosition(RwV3d* pvPosition) const
{
    *pvPosition = m_vOrgPosition;
};


bool CSwitchPillarGimmick::Query(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);

    if (pQuery->GetType() != GIMMICKTYPES::QUERY_SYS_SWITCH)
        return CGimmick::Query(pQuery);

    CStateGimmickQuery* pSwitchStateQuery = static_cast<CStateGimmickQuery*>(pQuery);
    ++pSwitchStateQuery->m_nTarget;

    if (m_eState != STATE_ON)
        return CGimmick::Query(pQuery);

    ++pSwitchStateQuery->m_nState;
    return true;
};


void CSwitchPillarGimmick::PreMove(void)
{
    ;
};


void CSwitchPillarGimmick::PostMove(void)
{
    static_assert(COUNT_OF(m_aszOnPlayerName) == COUNT_OF(m_aszPreOnPlayerName), "checkout");
    static_assert(COUNT_OF(m_aszOnPlayerName[0]) == COUNT_OF(m_aszPreOnPlayerName[0]), "checkout");

    for (int32 i = 0; i < COUNT_OF(m_aszOnPlayerName); ++i)
    {
        std::strcpy(m_aszPreOnPlayerName[i], m_aszOnPlayerName[i]);
        m_aszOnPlayerName[i][0] = '\0';
    };
};


void CSwitchPillarGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    CGameObject* pGameObj = CGameObjectManager::GetObject(pszSender);
    ASSERT(pGameObj);

    if (pGameObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    if (!isPlayerPreOnSwitchPillar(pszSender))
    {
        m_eState = STATE((int32(m_eState) + 1) % STATENUM);
        
        m_model.SetDrawType(
            m_eState == STATE_OFF ? CNormalGimmickModel::MODELTYPE_DRAW_NORMAL : CNormalGimmickModel::MODELTYPE_DRAW_BREAK
        );

        CGameSound::PlayObjectSE(this, SDCODE_SE(4177));
    };

    ASSERT(std::strlen(pszSender) < COUNT_OF(m_aszOnPlayerName[0]));    
    std::strcpy(m_aszOnPlayerName[m_nRidePlayer++], pszSender);
};


bool CSwitchPillarGimmick::isPlayerPreOnSwitchPillar(const char* pszName) const
{
    for (int32 i = 0; i < COUNT_OF(m_aszPreOnPlayerName); ++i)
    {
        if (!std::strcmp(m_aszPreOnPlayerName[i], pszName))
            return true;
    };

    return false;
};