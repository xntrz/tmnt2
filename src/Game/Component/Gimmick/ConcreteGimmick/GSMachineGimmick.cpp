#include "GSMachineGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/GSMachineGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickMotion.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


static float s_fHitRadius = 54.0f;
static int32 s_nPower = 70;
static RwV3d s_vOffset = { 0.0f, 0.0f, 49.5f };


CGSMachineGimmick::CGSMachineGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pGSMachineGimmickMove(nullptr)
, m_pBodyHitData(nullptr)
, m_model()
, m_pGimmickMotion(nullptr)
, m_szPathName()
, m_vNearestPlayerPosition(Math::VECTOR3_ZERO)
, m_fDistance(0.0f)
, m_fNearestPathT(0.0f)
, m_fAimPlayrate(0.0f)
, m_fAimPathTimerate(0.0f)
, m_fPlayrate(0.0f)
, m_fPathTimerate(0.0f)
, m_hEffectWaterSplash(0)
, m_eState(STATE_WAIT)
, m_fStepTimer(0.0f)
, m_subid(0)
, m_bIsVib(true)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    m_subid = pBasicParam->m_subid;    

    //
    //  init model
    //
    CModel* pTempModel = CModelManager::CreateModel("m17n_sweep");
    ASSERT(pTempModel);
    
    pTempModel->SetLightingEnable(false);
    pTempModel->SetClippingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);
    
    //
    //  init motion
    //
    CMotionManager::SetCurrentMotionSet("sweepmachine");
    
    m_pGimmickMotion = new CGimmickMotion(pTempModel);
    m_pGimmickMotion->AddMotion("Idle");
    m_pGimmickMotion->SetMotion("Idle", 0.0f, 0.0f, 0.0f, false);
    m_pGimmickMotion->StartRepeat(1.0f);

    //
    //  init movement
    //
    static const char* s_apszPathName[] =
    {
        "sweep0",
        "sweep1",
        "sweep2",
        "sweep3",
    };

    ASSERT(m_subid >= 0);
    ASSERT(m_subid < COUNT_OF(s_apszPathName));
    ASSERT(std::strlen(s_apszPathName[m_subid]) < COUNT_OF(m_szPathName));

    m_szPathName[0] = '\0';
    std::strcpy(m_szPathName, s_apszPathName[m_subid]);

    m_pGSMachineGimmickMove = new CGSMachineGimmickMove;
    m_pGSMachineGimmickMove->InitPath(m_szPathName, 0.0f);
    m_pGSMachineGimmickMove->SetPathLength(m_pGSMachineGimmickMove->GetPathLength(m_szPathName));
    
    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);

    m_pGSMachineGimmickMove->SetRotation(&vRotation);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGSMachineGimmickMove->GetPosition(&vPosition);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&vPosition);
    m_model.UpdateFrame();
    
    //
    //  init body hit
    //
    if (m_subid == 3)
    {
        m_pBodyHitData = nullptr;
    }
    else
    {
        RwV3d vPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vPos, &s_vOffset, m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL));

        m_pBodyHitData = CBodyHitManager::AllocData();
        ASSERT(m_pBodyHitData);

        m_pBodyHitData->InitData(&vPos, s_fHitRadius);
        m_pBodyHitData->SetState(CBodyHitData::STATE_SELF_TO_OTHER, false);
    };

    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pGSMachineGimmickMove);
};


CGSMachineGimmick::~CGSMachineGimmick(void)
{
    EffectStop();

    if (m_pBodyHitData)
    {
        CBodyHitManager::FreeData(m_pBodyHitData);
        m_pBodyHitData = nullptr;
    };

    if (m_pGSMachineGimmickMove)
    {
        delete m_pGSMachineGimmickMove;
        m_pGSMachineGimmickMove = nullptr;
    };

    if (m_pGimmickMotion)
    {
        delete m_pGimmickMotion;
        m_pGimmickMotion = nullptr;
    };
};


void CGSMachineGimmick::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT:
        OnAttackResult(static_cast<CHitCatchData*>(pParam));
        break;

    case GAMEOBJECTTYPES::MESSAGEID_CATCHATTACK:
        OnCatchAttack(static_cast<CHitAttackData*>(pParam));
        break;

    case GIMMICKTYPES::MESSAGEID_RECVEVENT:
        OnReceiveEvent(
            static_cast<GIMMICKTYPES::EVENT_ARGS*>(pParam)->m_szSender,
            static_cast<GIMMICKTYPES::EVENT_ARGS*>(pParam)->m_type
        );
        break;

    default:
        break;
    };
};


void CGSMachineGimmick::PostMove(void)
{
    float dt = CGameProperty::GetElapsedTime();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGSMachineGimmickMove->GetPosition(&vPosition);

    m_fDistance = CGimmickUtils::CalcNearestPlayerDistance(&vPosition, &m_vNearestPlayerPosition);
    m_fStepTimer += dt;

    switch (m_eState)
    {
    case STATE_WAIT:
        Wait();
        break;
        
    case STATE_CRUISE:
        Cruise();
        break;
        
    case STATE_PRECHARGE:
        PreCharge();
        break;
        
    case STATE_CHARGE:
        Charge();
        break;
        
    case STATE_POSTCHARGE:
        PostCharge();
        break;
        
    case STATE_SLEEP:
        Sleep();
        break;
        
    case STATE_STOP:
        Stop();
        break;

    default:
        ASSERT(false);
        break;
    };

    m_fPlayrate = RateControl(m_fPlayrate, m_fAimPlayrate, dt + dt);
    m_pGimmickMotion->SetPlayrate(m_fPlayrate);
    
    m_fPathTimerate = RateControl(m_fPathTimerate, m_fAimPathTimerate, dt * 0.1f);
    m_pGSMachineGimmickMove->SetPathTimeRate(m_fPathTimerate);

    if (m_eState > STATE_WAIT)
    {
        CommonUpdate();
        EffectUpdate();
    };

    if (m_pGSMachineGimmickMove->IsPathEnd())
    {
        if (m_bIsVib)
        {
            CGameProperty::SetCameraVibration(0.5f, 0.5f, 5);
            CGameSound::PlayObjectSE(this, SDCODE_SE(4384));
            if (m_subid == 3)
            {
                CGameSound::PlayObjectSE(this, SDCODE_SE(4139));
                CGameSound::PlayObjectSE(this, SDCODE_SE(4385));
                CGameSound::StopSE(SDCODE_SE(4385));
            };

            m_bIsVib = false;
        };

        if (m_fDistance > 15.0f)
            SetGSState(STATE_STOP);
    }
    else
    {
        m_bIsVib = true;
    };
};


void CGSMachineGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    m_pGSMachineGimmickMove->Start();
    
    if (m_pBodyHitData)
        m_pBodyHitData->SetState(CBodyHitData::STATE_SELF_TO_OTHER, true);
    
    SetGSState(STATE_CRUISE);
    EffectStart();

    CGameProperty::SetCameraVibration(0.05f, 2.5f, 10);
};


void CGSMachineGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    CGameObject* pGameObj = CGameObjectManager::GetObject(pCatch->GetObjectHandle());
    ASSERT(pGameObj);

    if (pGameObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    if (static_cast<CCharacter*>(pGameObj)->GetCharacterType() != CCharacter::TYPE_PLAYER)
        return;

    SetGSState(STATE_SLEEP);
    
    CGameProperty::SetCameraVibration(0.5f, 0.5f, 5);
};


void CGSMachineGimmick::Wait(void)
{
    m_fAimPlayrate = 0.0f;
    m_fAimPathTimerate = 0.0f;
};


void CGSMachineGimmick::Cruise(void)
{
    m_fAimPlayrate = 1.0f;
    m_fAimPathTimerate = (m_fPlayrate <= 0.5f ? 0.0f : GetPathTimerateByMPS(float(m_subid) * 0.8f + 5.0f));

    if (m_fDistance > 15.0f)
        SetGSState(STATE_PRECHARGE);

    if (m_fStepTimer > 10.0f)
        SetGSState(STATE_PRECHARGE);
};


void CGSMachineGimmick::PreCharge(void)
{
    m_fAimPlayrate = 0.0f;
    m_fAimPathTimerate = 0.0f;

    if (m_fStepTimer > 1.0f)
        SetGSState(STATE_CHARGE);
};


void CGSMachineGimmick::Charge(void)
{
    m_fAimPlayrate = 2.0f;
    m_fAimPathTimerate = GetPathTimerateByMPS(float(m_subid) * 3.0f + 14.0f);    

    if (m_fNearestPathT < m_pGSMachineGimmickMove->GetPathT())
        SetGSState(STATE_POSTCHARGE);
};


void CGSMachineGimmick::PostCharge(void)
{
    m_fAimPlayrate = 0.5f;
    m_fAimPathTimerate = 0.0f;

    if (m_fStepTimer > 3.0f)
        SetGSState(STATE_CRUISE);
};


void CGSMachineGimmick::Sleep(void)
{
    m_fAimPlayrate = 0.1f;
    m_fAimPathTimerate = (m_fDistance >= 8.0f ? 0.0f : GetPathTimerateByMPS(float(m_subid) + -2.5f));

    if (m_fStepTimer > 3.0f)
        SetGSState(STATE_CRUISE);
};


void CGSMachineGimmick::Stop(void)
{
    m_fAimPlayrate = 0.0f;
    m_fAimPathTimerate = 0.0f;

    Release();
};


void CGSMachineGimmick::CommonUpdate(void)
{
    RwV3d vHitPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vHitPos, &s_vOffset, m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL));

    //
    //  update catch
    //
    {
        RwSphere HitSphere = { vHitPos, s_fHitRadius };
        
        CHitCatchData CatchData;
        CatchData.Cleanup();
        CatchData.SetObject(GetHandle());
        CatchData.SetObjectType(GetType());
        CatchData.SetShape(CHitCatchData::SHAPE_SPHERE);
        CatchData.SetSphere(&HitSphere);

        CHitAttackManager::RegistCatch(&CatchData);
    }

    //
    //  update attack
    //
    {
        RwSphere HitSphere =
        {
            vHitPos,
            s_fHitRadius * (m_pGSMachineGimmickMove->IsPathEnd() ? 0.95f : 1.0f)
        };

        RwV3d vObjPos = Math::VECTOR3_ZERO;
        m_pGSMachineGimmickMove->GetPosition(&vObjPos);

        CHitAttackData AttackData;
        AttackData.Cleanup();
        AttackData.SetObject(GetHandle());
        AttackData.SetShape(CHitAttackData::SHAPE_SPHERE);
        AttackData.SetSphere(&HitSphere);
        AttackData.SetObjectPos(&vObjPos);
        AttackData.SetPower(s_nPower);
        AttackData.SetTarget(CHitAttackData::TARGET_ALL_EXCEPT_SELF);
        AttackData.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        AttackData.SetStatus(CHitAttackData::STATUS_FLYAWAY);

        CHitAttackManager::RegistAttack(&AttackData);
    }

    //
    //  update model
    //
    RwV3d vPos = Math::VECTOR3_ZERO;
    m_pGSMachineGimmickMove->GetPosition(&vPos);

    RwV3d vRot = Math::VECTOR3_ZERO;
    m_pGSMachineGimmickMove->GetRotation(&vRot);

    m_model.SetPosition(&vPos);
    m_model.SetRotation(&vRot);
    m_model.UpdateFrame();
    
    //
    //  update motion
    //
    m_pGimmickMotion->Update();

    //
    //  update body hit
    //
    if (m_pBodyHitData)
        m_pBodyHitData->SetCurrentPos(&vHitPos);
};


void CGSMachineGimmick::EffectStart(void)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pGSMachineGimmickMove->GetRotation(&vRotation);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGSMachineGimmickMove->GetPosition(&vPosition);

    m_hEffectWaterSplash = CEffectManager::Play(EFFECTID::ID_SWEEP_SPLASH, &vPosition, vRotation.y + Math::PI);
    ASSERT(m_hEffectWaterSplash);
};


void CGSMachineGimmick::EffectStop(void)
{
    if (m_hEffectWaterSplash)
    {
        CEffectManager::Finish(m_hEffectWaterSplash);
        m_hEffectWaterSplash = 0;
    };
};


void CGSMachineGimmick::EffectUpdate(void)
{
    static const RwV3d s_vLocalPos = { 0.0f, 0.3f, -1.0f };
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vPosition, &s_vLocalPos, m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL));

    if (m_hEffectWaterSplash)
        CEffectManager::SetPosition(m_hEffectWaterSplash, &vPosition);
};


void CGSMachineGimmick::SetGSState(STATE eState)
{
    switch (eState)
    {
    case STATE_CRUISE:
        {
            CGameSound::PlayObjectSE(this, SDCODE_SE(4386));
        }
        break;

    case STATE_PRECHARGE:
    case STATE_POSTCHARGE:
        {
            CGameSound::PlayObjectSE(this, SDCODE_SE(4385));
        }
        break;

    case STATE_CHARGE:
        {
            m_fNearestPathT = m_pGSMachineGimmickMove->GetNearestPathT(&m_vNearestPlayerPosition);
            CGameSound::PlayObjectSE(this, SDCODE_SE(4386));
        }
        break;

    case STATE_SLEEP:
        {
            m_fPathTimerate = 0.0f;
            CGameSound::PlayObjectSE(this, SDCODE_SE(4385));
        }
        break;

    default:
        break;
    };

    m_eState = eState;
    m_fStepTimer = 0.0f;
};


float CGSMachineGimmick::RateControl(float fCur, float fAim, float dt) const
{
    float fDlt = std::fabs(fCur - fAim);

    if (fDlt >= dt)
        return (fCur >= fAim ? (fCur - dt) : (fCur + dt));
    else
        return fAim;
};


float CGSMachineGimmick::GetPathTimerateByMPS(float fMPS) const
{
    return (m_pGSMachineGimmickMove->GetPathTByMeterPerSecond(fMPS) - m_pGSMachineGimmickMove->GetPathT()) / CGameProperty::GetElapsedTime();
};