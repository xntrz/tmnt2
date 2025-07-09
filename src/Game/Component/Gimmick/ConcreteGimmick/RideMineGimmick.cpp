#include "RideMineGimmick.hpp"

#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/RideMineGimmickMove.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Sequence/Ride/RideStage.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"


CRideMineGimmick::CRideMineGimmick(const char* pszName, void* pParam)
: CRideGimmick(pszName, pParam)
{
    m_fRadius = 0.75f;
    m_fOffset = 1.0f;
    m_iPower = 10;
    m_bCatchHit = true;

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    //
    //  Init model
    //
    CModel* pModel = CModelManager::CreateModel("mine");
    ASSERT(pModel);
    pModel->SetClippingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    SetModelStrategy(&m_model);

    //
    //  Init movement
    //
    m_pRideMove = new CRideMineGimmickMove(pInitParam->m_subid);
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


CRideMineGimmick::~CRideMineGimmick(void)
{
    ;
};


void CRideMineGimmick::PostMove(void)
{
    CRideGimmick::PostMove();
    
    if (IsNearByPlayer())
        Explose();
};


void CRideMineGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    CRideStage::NotifyGimmickDestroyed(pAttack);
    Explose();
};


void CRideMineGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    Explose();
};


void CRideMineGimmick::Explose(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pRideMove->GetPosition(&vPosition);

    CMagicManager::CParameter param;
    param.SetPositon(&vPosition);
    param.SetObject(this);
    param.SetSoundPlay(false);
    CMagicManager::Play(MAGICID::ID_EXPL_B1, &param);
    
    CGameSound::PlayObjectSE(this, SDCODE_SE(0x102B));

    Release();
};


bool CRideMineGimmick::IsNearByPlayer(void)
{
    if (!IsProcess(PROCESSTYPE_ATTACK))
        return false;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pRideMove->GetPosition(&vPosition);
    
    RwV3d vNearestPos = Math::VECTOR3_ZERO;
    if (CGimmickUtils::CalcNearestPlayerDistance(&vPosition, &vNearestPos) >= 5.0f)
        return false;

    float fDist = std::fabs(vNearestPos.x - vPosition.x);
    if (fDist >= 3.0f)
        return false;

    return true;
};