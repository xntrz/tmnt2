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


void CGrassGimmick::Draw(void) const
{
    CRenderStateManager::SetForDrawBeginning();

    RENDERSTATE_PUSH(rwRENDERSTATEALPHATESTFUNCTIONREF, 32);
    CGimmick::Draw();
    RENDERSTATE_POP(rwRENDERSTATEALPHATESTFUNCTIONREF);
};


GIMMICKTYPES::FEATURE CGrassGimmick::GetFeatures(void) const
{
    return GIMMICKTYPES::FEATURE_SLEEP;
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
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    m_subid = pInitParam->m_subid;

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

    m_pGrassMove = new CCuttingGimmickMove;
    ASSERT(m_pGrassMove);
    m_pGrassMove->SetPosition(&pInitParam->m_vPosition);
    m_pGrassMove->SetRotation(&vRotation);
    m_pGrassMove->Start();
    
    SetMoveStrategy(m_pGrassMove);
    SetModelStrategy(&m_model);
};


void CGrassGimmick::waiting(void)
{
    {
        RwSphere sphere = { 0 };
        sphere.radius = kindinfo().m_fHitRadius;

        ASSERT(m_pGrassMove);
        m_pGrassMove->GetPosition(&sphere.center);
        sphere.center.y += 0.8f;

        CHitCatchData Catch;
        Catch.SetObject(GetHandle());
        Catch.SetObjectType(GetType());
        Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
        Catch.SetSphere(&sphere);

        CHitAttackManager::RegistCatch(&Catch);
    }

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

    RwRGBA Color = { 0xFF, 0xFF, 0xFF, 0xFF };
    Color.alpha = uint8((1.0f - m_fTimer) * 255.0f);
    pModel->SetColor(Color);

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
    ASSERT(m_subid >= 0 && m_subid < COUNT_OF(m_aGrassGimmickKindInfoList));
    return m_aGrassGimmickKindInfoList[m_subid];
};