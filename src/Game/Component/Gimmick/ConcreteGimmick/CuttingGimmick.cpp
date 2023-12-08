#include "CuttingGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/CuttingGimmickMove.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "System/Common/RenderState.hpp"


CCuttingGimmickModel::CCuttingGimmickModel(void)
: m_state(STATE_NORMAL)
{
    ;
};


CCuttingGimmickModel::~CCuttingGimmickModel(void)
{
    ;
};


void CCuttingGimmickModel::Draw(void) const
{
    switch (m_state)
    {
    case STATE_NORMAL:
        m_apModel[CUTMODELKIND_NORMAL]->Draw();
        m_apModel[CUTMODELKIND_ATARI]->UpdateFrame();
        m_apModel[CUTMODELKIND_NORMAL_UP]->UpdateFrame();
        m_apModel[CUTMODELKIND_NORMAL_DOWN]->UpdateFrame();
        break;

    case STATE_DESTROY:
        m_apModel[CUTMODELKIND_NORMAL]->UpdateFrame();
        m_apModel[CUTMODELKIND_ATARI]->UpdateFrame();
        m_apModel[CUTMODELKIND_NORMAL_UP]->Draw();
        m_apModel[CUTMODELKIND_NORMAL_DOWN]->Draw();
        break;

    case STATE_END:
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CCuttingGimmickModel::SetState(STATE state)
{
    m_state = state;
};


/*static*/ const int32 CCuttingGimmick::NUM_ATTACK_TO_BREAK = 10;


/*static*/ const CCuttingGimmick::KINDINFO CCuttingGimmick::m_aCuttingGimmickKindInfoList[] =
{
    { "bamboo",		"bamboo_a",		"bamboo_b1",	"bamboo_b2",	0.3f, 0.4f, 4.0f, true, 0x1044 },
    { "irongate",	"irongate_a",	"irongate_b1",	"irongate_b2",	1.0f, 0.8f, 5.0f, true, 0x1041 },
    { "roadsign",	"roadsign_a",	"roadsign_b1",	"roadsign_b2",	0.3f, 0.8f, 4.0f, true, 0x1042 },
    { "icepillar",	"icepillar_a",	"icepillar_b1", "icepillar_b2", 1.0f, 0.5f, 4.0f, true, 0x1040 },
    { "tree",		"tree_a",		"tree_b1",		"tree_b2",		0.4f, 0.4f, 4.0f, true, 0x1043 },
};


CCuttingGimmick::CCuttingGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pCuttingMove(nullptr)
, m_fTimer(0.0f)
, m_state(STATE_NONE)
, m_subid(0)
, m_hAtari(0)
, m_nNumAttack(NUM_ATTACK_TO_BREAK)
, m_fInvinsibleTimer(0.0f)
{
    init(pParam);
};


CCuttingGimmick::~CCuttingGimmick(void)
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
};


void CCuttingGimmick::Draw(void) const
{
    CRenderStateManager::SetForDrawBeginning();
    
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    m_model.Draw();
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
};


GIMMICKTYPES::FEATURE CCuttingGimmick::GetFeatures(void) const
{
    return GIMMICKTYPES::FEATURE_SLEEP;
};


bool CCuttingGimmick::Query(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);

    if (pQuery->GetType() == GIMMICKTYPES::QUERY_EVE_BROKEN)
    {
        CStateGimmickQuery* pStateQuery = (CStateGimmickQuery*)pQuery;
        ++pStateQuery->m_nTarget;

        if (m_state == STATE_END)
        {
            ++pStateQuery->m_nState;
            return true;
        };
    };

    return CGimmick::Query(pQuery);
};


void CCuttingGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_NONE:
        m_state = STATE_WAIT;
        break;

    case STATE_WAIT:
        waiting();
        break;

    case STATE_CUT:
        cutting();
        break;

    case STATE_BREAK:
        breakdown();
        break;

    case STATE_DISAPPEAR:
        disappear();
        break;

    case STATE_END:
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CCuttingGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_state == STATE_WAIT)
            onCut();
    };
};


void CCuttingGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_state == STATE_WAIT)
    {
        if (pAttack->IsFlagSlice() || (--m_nNumAttack <= 0))
        {
            onCut();            
            CGimmickUtils::PlayHitEffect(pAttack);
        }
        else
        {
            m_fInvinsibleTimer = 1.0f;
        };
    };
};


void CCuttingGimmick::init(void* pParam)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    m_subid = pInitParam->m_subid;

    CModel* pModel = CModelManager::CreateModel(kindinfo().m_pszModelName);
    ASSERT(pModel);
    pModel->SetLightingEnable(kindinfo().m_bLighting);

    CModel* pAtari = CModelManager::CreateModel(kindinfo().m_pszAtariModelName);
    ASSERT(pAtari);
    pAtari->SetLightingEnable(kindinfo().m_bLighting);

    CModel* pModelU = CModelManager::CreateModel(kindinfo().m_pszBreakUModelName);
    ASSERT(pModelU);
    pModelU->SetLightingEnable(kindinfo().m_bLighting);

    CModel* pModelD = CModelManager::CreateModel(kindinfo().m_pszBreakDModelName);
    ASSERT(pModelD);
    pModelD->SetLightingEnable(kindinfo().m_bLighting);

    m_model.SetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL, pModel);
    m_model.SetModel(CCuttingGimmickModel::CUTMODELKIND_ATARI, pAtari);
    m_model.SetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL_UP, pModelU);
    m_model.SetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL_DOWN, pModelD);
    m_model.SetPosition(&pInitParam->m_vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    m_model.SetRotation(&vRotation);
    m_model.SetState(CCuttingGimmickModel::STATE_NORMAL);
    m_model.UpdateFrame();

    m_pCuttingMove = new CCuttingGimmickMove;
    ASSERT(m_pCuttingMove);
    m_pCuttingMove->SetPosition(&pInitParam->m_vPosition);
    m_pCuttingMove->SetRotation(&vRotation);
    SetMoveStrategy(m_pCuttingMove);
    m_pCuttingMove->Start();

    RpClump* pAtariClump = pAtari->GetClump();
    if (pAtariClump)
    {
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pAtariClump);
        ASSERT(m_hAtari);
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, kindinfo().m_fCollisionRadius);
    };
};


void CCuttingGimmick::waiting(void)
{
    if (m_fInvinsibleTimer <= 0.0f)
    {        
        RwSphere sphere = { 0 };
        sphere.radius = kindinfo().m_fHitRadius;
        
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
        m_fInvinsibleTimer -= CGameProperty::GetElapsedTime();
    };
};


void CCuttingGimmick::cutting(void)
{
    CModel* pModel = m_model.GetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL_UP);
    ASSERT(pModel);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetPosition(&vPosition);
    pModel->SetPosition(&vPosition);
    pModel->UpdateFrame();

    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer > kindinfo().m_fSlideTime)
    {
        m_fTimer = 0.0f;
        m_pCuttingMove->SetVelocity(&Math::VECTOR3_ZERO);
        onBreakDown();
    };
};


void CCuttingGimmick::breakdown(void)
{
    CModel* pModel = m_model.GetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL_UP);
    ASSERT(pModel);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetRotation(&vRotation);
    pModel->SetRotation(&vRotation);
    pModel->UpdateFrame();

    RwV3d vRotVelocity = Math::VECTOR3_ZERO;
    m_pCuttingMove->GetRotVelocity(&vRotVelocity);
    vRotVelocity.z += (CGameProperty::GetElapsedTime() * Math::PI05);
    m_pCuttingMove->SetRotVelocity(&vRotVelocity);

    RwRGBA Color = { 0xFF, 0xFF, 0xFF, 0xFF };
    Color.alpha = uint8((1.0f - m_fTimer * 0.83) * 255.0f);
    pModel->SetColor(Color);
    
    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer > 1.2f)
    {
        m_fTimer = 0.0f;
        m_pCuttingMove->SetRotVelocity(&Math::VECTOR3_ZERO);
        onDisappear();
    };
};


void CCuttingGimmick::disappear(void)
{
    CModel* pModel = m_model.GetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL_DOWN);
    ASSERT(pModel);
    
    RwRGBA Color = { 0xFF, 0xFF, 0xFF, 0xFF };
    Color.alpha = uint8((1.0f - m_fTimer * 0.83) * 255.0f);
    pModel->SetColor(Color);

    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer > 1.0f)
    {
        m_fTimer = 0.0f;
        m_model.SetState(CCuttingGimmickModel::STATE_END);
        m_state = STATE_END;
    };
};


void CCuttingGimmick::onCut(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;        
    };

    m_model.SetState(CCuttingGimmickModel::STATE_DESTROY);
    
    CModel* pModel = m_model.GetModel(CCuttingGimmickModel::CUTMODELKIND_NORMAL_UP);
    ASSERT(pModel);
    
    RpClump* pClump = pModel->GetClump();
    ASSERT(pClump);
    
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    ASSERT(pFrame);
    
    RwMatrix* pModeling = RwFrameGetMatrixMacro(pFrame);
    ASSERT(pModeling);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    vVelocity.x = -pModeling->right.x;
    vVelocity.y = -0.5f;
    vVelocity.z = -pModeling->right.z;
    Math::Vec3_Normalize(&vVelocity, &vVelocity);
    Math::Vec3_Scale(&vVelocity, &vVelocity, 0.8f);

    m_pCuttingMove->SetVelocity(&vVelocity);

    if (kindinfo().m_nSE)
        CGameSound::PlayObjectSE(this, kindinfo().m_nSE);

    m_state = STATE_CUT;
};


void CCuttingGimmick::onBreakDown(void)
{
    m_state = STATE_BREAK;
};


void CCuttingGimmick::onDisappear(void)
{
    m_state = STATE_DISAPPEAR;
};


const CCuttingGimmick::KINDINFO& CCuttingGimmick::kindinfo(void) const
{
    ASSERT(m_subid >= 0 && m_subid < COUNT_OF(m_aCuttingGimmickKindInfoList));
    return m_aCuttingGimmickKindInfoList[m_subid];
};