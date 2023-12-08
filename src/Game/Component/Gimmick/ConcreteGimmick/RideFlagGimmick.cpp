#include "RideFlagGimmick.hpp"

#include "Game/Component/Gimmick/MoveStrategy/RideFlagGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Sequence/Ride/RideStage.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"


/*static*/ int32 CRideFlagGimmick::m_iCounter[FLAGTYPENUM] = { 0 };
/*static*/ int32 CRideFlagGimmick::m_iRestCounter[FLAGTYPENUM] = { 0 };


/*static*/ void CRideFlagGimmick::Initialize(void)
{
    std::memset(m_iCounter, 0x00, sizeof(m_iCounter));
    std::memset(m_iRestCounter, 0x00, sizeof(m_iRestCounter));
};


CRideFlagGimmick::CRideFlagGimmick(const char* pszName, void* pParam)
: CRideGimmick(pszName, pParam)
{
    struct INFO
    {
        int32 m_iScore;
        float m_fOffset;
        const char* m_pszModelName;
    };

    static const INFO s_aInfo[FLAGTYPENUM] =
    {
        { 0, 0.0f, "flag" },
        { 1, 0.0f, "gflag" },
        { 1, 0.0f, "gflag" },
        { 1, 0.0f, "gflag" },
        { 1, 1.0f, "gflag" },
        { 1, 1.0f, "gflag" },
    };

    ASSERT(pParam);
    
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;

    ASSERT(pInitParam->m_subid >= 0 && pInitParam->m_subid < COUNT_OF(s_aInfo));
    const INFO* pInfo = &s_aInfo[pInitParam->m_subid];

    RwV3d vPosition = pInitParam->m_vPosition;
    vPosition.y += pInfo->m_fOffset;

    m_iScore = pInfo->m_iScore;
    m_flagtype = FLAGTYPE(pInitParam->m_subid);

    CModel* pModel = CModelManager::CreateModel(pInfo->m_pszModelName);
    ASSERT(pModel);
    pModel->SetClippingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetPosition(&vPosition);
    m_model.UpdateFrame();

    m_pRideMove = new CRideFlagGimmickMove(m_flagtype);
    ASSERT(m_pRideMove);
    m_pRideMove->SetPosition(&vPosition);
    m_pRideMove->Start();

    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pRideMove);

    ++m_iCounter[m_flagtype];
    ++m_iRestCounter[m_flagtype];

    if (CRideStage::GetShadowFlag())
    {
        m_pModuleManager = new CModuleManager;
        ASSERT(m_pModuleManager);
        m_pModuleManager->Include(CCircleShadowModule::New(this, m_fRadius * 3.0f, m_fRadius * 3.0f, false));
        m_pModuleManager->Run();
    };

    m_bAdd = true;
};


CRideFlagGimmick::~CRideFlagGimmick(void)
{
    ;
};


void CRideFlagGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    if (m_bAdd)
    {
        CRideStage::AddScore(RIDETYPES::SCOREKIND(m_iScore), pCatch);
        --m_iRestCounter[m_flagtype];
        Release();
        m_bAdd = false;
    };
};