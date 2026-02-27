#include "RideRockGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Gimmick/MoveStrategy/RideRockGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Sequence/Ride/RideStage.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"


/*static*/ RwRGBA CRideRockGimmick::m_damageColor = { 0xFF, 0x00, 0x00, 0xFF };


CRideRockGimmick::CRideRockGimmick(const char* pszName, void* pParam)
: CRideGimmick(pszName, pParam)
, m_iLife(4)
, m_defaultColor({ 0xFF, 0xFF, 0xFF, 0xFF })
, m_fBlendRate(0.0f)
{
    m_fRadius = 1.0f;
    m_fOffset = 0.0f;
    m_iPower = 10;
    m_bCatchHit = true;    

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    //
    //  Init model
    //
    CModel* pModel = CModelManager::CreateModel("asteroid");
    ASSERT(pModel);
    pModel->SetClippingEnable(false);

    m_defaultColor = pModel->GetColor();

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    SetModelStrategy(&m_model);

    //
    //  Init movement
    //
    m_pRideMove = new CRideRockGimmickMove;
    m_pRideMove->SetPosition(&pInitParam->m_vPosition);
    m_pRideMove->Start();

    SetMoveStrategy(m_pRideMove);

    //
    //  Init module man
    //
    if (CRideStage::GetShadowFlag())
    {
        m_pModuleManager = new CModuleManager;
        m_pModuleManager->Include(CCircleShadowModule::New(this, m_fRadius * 3.0f, m_fRadius * 3.0f, false));
        m_pModuleManager->Run();
    };
};


CRideRockGimmick::~CRideRockGimmick(void)
{
    ;
};


void CRideRockGimmick::PostMove(void)
{
    CRideGimmick::PostMove();

    if (m_iLife < 1)
        m_fBlendRate = 1.0f;
    else
        m_fBlendRate -= (CGameProperty::GetElapsedTime() * 8.0f);

    if (m_fBlendRate < 0.0f)
        m_fBlendRate = 0.0f;

    RwRGBA materialColor = {};
    materialColor.red   = static_cast<RwUInt8>(m_defaultColor.red   * (1.0f - m_fBlendRate) + m_damageColor.red     * m_fBlendRate);
    materialColor.green = static_cast<RwUInt8>(m_defaultColor.green * (1.0f - m_fBlendRate) + m_damageColor.green   * m_fBlendRate);
    materialColor.blue  = static_cast<RwUInt8>(m_defaultColor.blue  * (1.0f - m_fBlendRate) + m_damageColor.blue    * m_fBlendRate);
    materialColor.alpha = static_cast<RwUInt8>(m_defaultColor.alpha * (1.0f - m_fBlendRate) + m_damageColor.alpha   * m_fBlendRate);

    m_model.SetColor(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, materialColor);
};


void CRideRockGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    m_fBlendRate = 1.0f;

    if (--m_iLife <= 0)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pRideMove->GetPosition(&vPosition);

        CEffectManager::Play(EFFECTID::ID_METEORITE, &vPosition);
        
        CRideStage::NotifyGimmickDestroyed(pAttack);

        Release();
    };
};


void CRideRockGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    ;
};