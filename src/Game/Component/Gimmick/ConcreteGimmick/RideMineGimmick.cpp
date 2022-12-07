#include "RideMineGimmick.hpp"

#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
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

    ASSERT(pParam);
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;

    CModel* pModel = CModelManager::CreateModel("mine");
    ASSERT(pModel);
    pModel->SetClippingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    m_pRideMove = new CRideMineGimmickMove(pInitParam->m_subid);
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

    CMagicManager::CParameter MagicParam;
    MagicParam.SetPositon(&vPosition);
    MagicParam.SetObject(this);
    MagicParam.SetSoundPlay(false);
    CMagicManager::Play(MAGICID::ID_EXPL_B1, &MagicParam);
    
    CGameSound::PlaySE(SDCODE_SE(0x102B));

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

    float fDist = vNearestPos.x - vPosition.x;
    if (fDist < 0.0f)
        fDist = -fDist;

    if (fDist >= 3.0f)
        return false;

    return true;
};