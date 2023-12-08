#include "HatchGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"


CHatchGimmick::CHatchGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_move()
, m_model()
, m_eState(STATE_IDLE)
, m_hAtari(0)
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);

    //
    //  init disp model
    //
    CModel* pModelTemp = CModelManager::CreateModel("hatch");
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelTemp);

    //
    //  init atari model
    //
    pModelTemp = CModelManager::CreateModel("hatch_a");
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelTemp);
    
    //
    //  init model
    //
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    //
    //  init movement
    //
    m_move.SetRotation(&vRotation);
    m_move.SetPosition(&pInitParam->m_vPosition);
    
    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);
    };
    
    //
    //  setup model & move strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(&m_move);
};


CHatchGimmick::~CHatchGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CHatchGimmick::PreMove(void)
{
    ;
};


void CHatchGimmick::PostMove(void)
{
    ;
};


void CHatchGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    Release();
};