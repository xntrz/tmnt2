#include "GrassGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
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
#include "System/Common/RenderState.hpp"


/*static*/ const CGrassGimmick::KINDINFO CGrassGimmick::m_aGrassGimmickKindInfoList[] =
{
    { "glass_l_2d", "glass_l_2d_b", 0.5f },
    { "glass_s_2d", "glass_s_2d_b", 0.5f },
    { "glass_l_3d", "glass_l_3d_b", 0.5f },
    { "glass_s_3d", "glass_s_3d_b", 0.5f },
};


CGrassGimmick::CGrassGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pGrassMove(nullptr)
, m_model()
, m_fTimer(0.0f)
, m_state(STATE_NONE)
, m_subid(0)
, m_bPlayerNearMe(false)
{
    init(pParam);
};


CGrassGimmick::~CGrassGimmick(void)
{
    if (m_pGrassMove)
    {
        delete m_pGrassMove;
        m_pGrassMove = nullptr;
    };
};


#ifdef TARGET_PC 

void CGrassGimmick::Draw(void) const
{
    CRenderStateManager::SetForDrawBeginning();

    RENDERSTATE_PUSH(rwRENDERSTATEALPHATESTFUNCTIONREF, 32);
    CGimmick::Draw();
    RENDERSTATE_POP(rwRENDERSTATEALPHATESTFUNCTIONREF);
};

#endif /* TARGET_PC */


GIMMICKTYPES::FEATURE CGrassGimmick::GetFeatures(void) const
{
    return GIMMICKTYPES::FEATURE_ABLE_SLEEP;
};


void CGrassGimmick::PostMove(void)
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
        Release();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CGrassGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_state == STATE_WAIT)
            onCut();
    };
};


void CGrassGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);

    if (m_state == STATE_WAIT)
    {
        if (pAttack->IsFlagSlice())
        {
            onCut();
            CGimmickUtils::PlayHitEffect(pAttack);
        };
    };
};


void CGrassGimmick::init(void* pParam)
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam != nullptr);

    m_subid = pInitParam->m_subid;

    //
    //  Init model
    //
    CModel* pModel = CModelManager::CreateModel(kindinfo().m_pszModelName);
    ASSERT(pModel);

    CModel* pBreakModel = CModelManager::CreateModel(kindinfo().m_pszBreakModelName);
    ASSERT(pBreakModel);        

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pBreakModel);

    m_model.SetPosition(&pInitParam->m_vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();

    SetModelStrategy(&m_model);

    //
    //  Init movement
    //
    m_pGrassMove = new CCuttingGimmickMove;
    m_pGrassMove->SetPosition(&pInitParam->m_vPosition);
    m_pGrassMove->SetRotation(&vRotation);
    m_pGrassMove->Start();

    SetMoveStrategy(m_pGrassMove);
};


void CGrassGimmick::waiting(void)
{
    //
    //  Update attack
    //
    RwSphere hitSphere = {};
    hitSphere.radius = kindinfo().m_fHitRadius;
    m_pGrassMove->GetPosition(&hitSphere.center);
    hitSphere.center.y += 0.8f;

    CHitCatchData hitCatch;
    hitCatch.SetObject(GetHandle());
    hitCatch.SetObjectType(GetType());
    hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
    hitCatch.SetSphere(&hitSphere);

    CHitAttackManager::RegistCatch(&hitCatch);

    //
    //  Update sound
    //
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGrassMove->GetPosition(&vPosition);

    bool bPlayerWasNearMe = m_bPlayerNearMe;
    float fDistance = CGimmickUtils::CalcNearestPlayerDistance(&vPosition);
    m_bPlayerNearMe = (fDistance < 1.0f);
    if (!bPlayerWasNearMe && m_bPlayerNearMe)
        CGameSound::PlayObjectSE(this, SDCODE_SE(4155));    
};


void CGrassGimmick::cutting(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer > 2.0f)
    {
        m_fTimer = 0.0f;
        onBreakDown();
    };
};


void CGrassGimmick::breakdown(void)
{
    onDisappear();
};


void CGrassGimmick::disappear(void)
{
    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK);
    ASSERT(pModel);

    RwUInt8 alphaBasis = static_cast<RwUInt8>((1.0f - m_fTimer) * 255.0f);
    pModel->SetColor({ 255, 255, 255, alphaBasis });

    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer > 1.0f)
    {
        m_fTimer = 0.0f;
        m_state = STATE_END;
    };
};


void CGrassGimmick::onCut(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pGrassMove->GetPosition(&vPosition);

    vPosition.y += 0.6f;
    CEffectManager::Play(EFFECTID::ID_GRASS, &vPosition);

    CGameSound::PlayObjectSE(this, SDCODE_SE(4156));

    m_model.SetVisualBreak();
    m_state = STATE_CUT;
};


void CGrassGimmick::onBreakDown(void)
{
    m_state = STATE_BREAK;
};


void CGrassGimmick::onDisappear(void)
{
    m_state = STATE_DISAPPEAR;
};


const CGrassGimmick::KINDINFO& CGrassGimmick::kindinfo(void) const
{
    ASSERT(m_subid >= 0);
    ASSERT(m_subid < COUNT_OF(m_aGrassGimmickKindInfoList));

    return m_aGrassGimmickKindInfoList[m_subid];
};