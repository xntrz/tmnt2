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


/*static*/ RwRGBA CRideRockGimmick::m_DamageColor = { 0xFF, 0x00, 0x00, 0xFF };


CRideRockGimmick::CRideRockGimmick(const char* pszName, void* pParam)
: CRideGimmick(pszName, pParam)
, m_iLife(4)
, m_DefaultColor({ 0xFF, 0xFF,0xFF,0xFF })
, m_fBlendRate(0.0f)
{
    m_fRadius = 1.0f;
    m_fOffset = 0.0f;
    m_iPower = 10;
    m_bCatchHit = true;    

    ASSERT(pParam);    
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;

    CModel* pModel = CModelManager::CreateModel("asteroid");
    ASSERT(pModel);
    pModel->SetClippingEnable(false);

    m_DefaultColor = pModel->GetColor();

    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    m_pRideMove = new CRideRockGimmickMove;
    ASSERT(m_pRideMove);
    m_pRideMove->SetPosition(&pInitParam->m_vPosition);
    m_pRideMove->Start();

    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pRideMove);

    if (CRideStage::GetShadowFlag())
    {
        m_pModuleManager = new CModuleManager;
        ASSERT(m_pModuleManager);
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

    RwRGBA Color = { 0x00 ,0x00, 0x00, 0x00 };
    Color.red   = uint8(m_DefaultColor.red   * (1.0f - m_fBlendRate) + m_DamageColor.red     * m_fBlendRate);
    Color.green = uint8(m_DefaultColor.green * (1.0f - m_fBlendRate) + m_DamageColor.green   * m_fBlendRate);
    Color.blue  = uint8(m_DefaultColor.blue  * (1.0f - m_fBlendRate) + m_DamageColor.blue    * m_fBlendRate);
    Color.alpha = uint8(m_DefaultColor.alpha * (1.0f - m_fBlendRate) + m_DamageColor.alpha   * m_fBlendRate);

    m_model.SetColor(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, Color);
};


void CRideRockGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    m_fBlendRate = 1.0f;
    --m_iLife;

    if (m_iLife <= 0)
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