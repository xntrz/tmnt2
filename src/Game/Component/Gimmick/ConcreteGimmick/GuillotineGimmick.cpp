#include "GuillotineGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/GuillotineGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"


static const CGuillotineGimmick::PENDULUMPARAM s_aPendulumParam[] =
{
    { "Guillotine", 1.5f, { 0.0f, 1.3f,  0.0f } },
    { "crane",      1.0f, { 0.0f, 1.35f, 0.0f } }
};


static_assert(COUNT_OF(s_aPendulumParam) == CGuillotineGimmick::KINDNUM, "update me");


CGuillotineGimmick::CGuillotineGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_pModuleMan(nullptr)
, m_pGuillotineMove(nullptr)
, m_vFulcrumVector(Math::VECTOR3_ZERO)
, m_vPrePos(Math::VECTOR3_ZERO)
, m_vHitSpherePos(Math::VECTOR3_ZERO)
, m_fHitRadius(0.0f)
, m_kind(KIND_GUILLOTINE)
{
    GIMMICKPARAM::GIMMICK_GUILLOTINE* pGuilloParam = static_cast<GIMMICKPARAM::GIMMICK_GUILLOTINE*>(pParam);
    ASSERT(pGuilloParam);

    ASSERT(pGuilloParam->m_subid >= 0);
    ASSERT(pGuilloParam->m_subid < KINDNUM);

    const PENDULUMPARAM* pPendulumParam = &s_aPendulumParam[pGuilloParam->m_subid];

    m_kind          = static_cast<KIND>(pGuilloParam->m_subid);
    m_vHitSpherePos = pPendulumParam->m_vHitPos;
    m_fHitRadius    = pPendulumParam->m_fHitRadius;
    m_vFulcrumVector= { 0.0f, pGuilloParam->m_fRadius, 0.0f };

    //
    //  init disp model
    //
    CModel* pModel = CModelManager::CreateModel(pPendulumParam->m_szName);
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init movement
    //
    CGuillotineGimmickMove::INITPARAM moveParam = { 0 };
    moveParam.m_fTheta          = 0.0f;
    moveParam.m_fOnceTheta      = MATH_DEG2RAD(pGuilloParam->m_fAmplitude);
    moveParam.m_fMaxMove        = pGuilloParam->m_fRoundTime;
    moveParam.m_fHitRadius      = pPendulumParam->m_fHitRadius;
    moveParam.m_vFulcrumVector  = { 0.0f, pGuilloParam->m_fRadius, 0.0f };
    moveParam.m_vHitSpherePos   = pPendulumParam->m_vHitPos;
    
    m_pGuillotineMove = new CGuillotineGimmickMove(&moveParam);
    
    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pGuilloParam->m_quat);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pGuilloParam->m_vPosition);
    m_model.UpdateFrame();
    
    m_pGuillotineMove->SetRotation(&vRotation);
    m_pGuillotineMove->SetPosition(&pGuilloParam->m_vPosition);
    m_pGuillotineMove->InitFulcrumMatrix();
    m_pGuillotineMove->Start();

    //
    //  init module man
    //
    const float fShadowWH = 5.0f;
    
    m_pModuleMan = new CModuleManager;
    m_pModuleMan->Include(CCircleShadowModule::New(this, fShadowWH, fShadowWH, true));
    
    //
    //  setup model & movement strat
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pGuillotineMove);
};


/*virtual*/ CGuillotineGimmick::~CGuillotineGimmick(void)
{
    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };

    if (m_pGuillotineMove)
    {
        delete m_pGuillotineMove;
        m_pGuillotineMove = nullptr;
    };
};


/*virtual*/ void CGuillotineGimmick::Run(void) /*override*/
{
    PreMove();

    if (m_pMoveStrategy)
    {
        float dt = CGameProperty::GetElapsedTime();
        CGimmickMove::MOVESTATE moveState = m_pMoveStrategy->Move(dt);
        
        if ((moveState == CGimmickMove::MOVESTATE_TOUCHDOWN) ||
            (moveState == CGimmickMove::MOVESTATE_ON_GROUND))
            OnTouchedDown();

        if (m_pModelStrategy)
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_pMoveStrategy->GetPosition(&vPosition);

            m_pModelStrategy->SetPosition(&vPosition);
        };
    };

    PostMove();
};


/*virtual*/ void CGuillotineGimmick::Draw(void) const /*override*/
{
    CGimmick::Draw();

    if (m_pModuleMan)
        m_pModuleMan->Draw();
};


/*virtual*/ void CGuillotineGimmick::PreMove(void) /*override*/
{
    m_pGuillotineMove->GetPosition(&m_vPrePos);

    if (m_pModuleMan)
        m_pModuleMan->Run();
};


/*virtual*/ void CGuillotineGimmick::PostMove(void) /*override*/
{
    setHitAttack();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGuillotineMove->GetPosition(&vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pGuillotineMove->GetRotation(&vRotation);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&vPosition);
};


/*virtual*/ void CGuillotineGimmick::OnTouchedDown(void) /*override*/
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pGuillotineMove->GetRotation(&vRotation);
    
    RwV3d vPosition = m_pGuillotineMove->GetCollisionResult().m_vClosestPt;

    float fDirection = Math::RandFloatRange(-1.0f, 1.0f) * MATH_DEG2RAD(45.0f) + (vRotation.y + MATH_DEG2RAD(90.0f));

    if ((m_pGuillotineMove->GetPendulumState() == CGuillotineGimmickMove::PENDULUMSTATE_RIGHT) ||
        (m_pGuillotineMove->GetPendulumState() == CGuillotineGimmickMove::PENDULUMSTATE_CENTER_TO_RIGHT))
        fDirection += MATH_PI;

    fDirection = Math::RadianCorrection(fDirection);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_SPARKS, &vPosition, fDirection);
	if (hEffect && (m_kind == KIND_CRANE))
		CEffectManager::SetScale(hEffect, 0.5f);
};


void CGuillotineGimmick::setHitAttack(void)
{
    setHitAttackChain();

    if (m_kind == KIND_GUILLOTINE)
        setHitAttackGuillotine();
    else
        setHitAttackCrane();
};


void CGuillotineGimmick::setHitAttackChain(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGuillotineMove->GetPosition(&vPosition);

    if (CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition) > 5.0f)
        return;

    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RpClump* pClump = pModel->GetClump();
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    RwV3d vHitPos = m_vHitSpherePos;
    vHitPos.y += (m_fHitRadius + 0.5f);

    int32 numAttack = static_cast<int32>((m_vFulcrumVector.y * 0.5f) - m_fHitRadius);
    for (int32 i = 0; i < numAttack; ++i)
    {
        RwSphere hitSphere = { 0 };
        RwV3dTransformPoint(&hitSphere.center, &vHitPos, pMatrix);
        hitSphere.radius = 0.5f;

        registHitAttack(&hitSphere, 3 + i);

        vHitPos.y += 1.0f;
    };
};


void CGuillotineGimmick::setHitAttackCrane(void)
{
    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RpClump* pClump = pModel->GetClump();
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    RwSphere hitSphere = { Math::VECTOR3_ZERO, m_fHitRadius };
    RwV3dTransformPoint(&hitSphere.center, &m_vHitSpherePos, pMatrix);

    registHitAttack(&hitSphere, 0);
};


void CGuillotineGimmick::setHitAttackGuillotine(void)
{
    static const RwSphere s_aHitSphere[] =
    {
        { { m_vHitSpherePos     }, m_fHitRadius },
        { { -2.0f,  1.8f, 0.0f  }, 1.0f         },
        { {  2.0f,  1.8f, 0.0f  }, 1.0f         },
    };

    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RpClump* pClump = pModel->GetClump();
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    for (int32 i = 0; i < COUNT_OF(s_aHitSphere); ++i)
    {
        RwSphere hitSphere = { 0 };
        RwV3dTransformPoint(&hitSphere.center, &s_aHitSphere[i].center, pMatrix);
        hitSphere.radius = s_aHitSphere[i].radius;

        registHitAttack(&hitSphere, 0);
    };
};


void CGuillotineGimmick::registHitAttack(const RwSphere* pHitSphere, int32 No)
{
    if (!pHitSphere)
        return;

    if (pHitSphere->radius <= 0.0f)
        return;

    CHitAttackData hitAttack;
    hitAttack.Cleanup();
    hitAttack.SetObject(GetHandle());
    hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);
    hitAttack.SetSphere(pHitSphere);
    hitAttack.SetObjectPos(&m_vPrePos);
    hitAttack.SetPower(30);
    hitAttack.SetTarget(CHitAttackData::TARGET_CHARACTER_GIMMICK);
    hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
    hitAttack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
    hitAttack.SetAttackNo(No);

    CHitAttackManager::RegistAttack(&hitAttack);
};