#include "CrumbleFloorGimmick.hpp"

#include "Game/Component/Gimmick/MoveStrategy/CrumbleFloorGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"


CCrumbleFloorGimmick::CCrumbleFloorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pCrumbleFloorMove(nullptr)
, m_model()
, m_hAtari(0)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    //
    //  init display model
    //
    CModel* pTempModel = CModelManager::CreateModel("kuzure");
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);

    //
    //  init atari model
    //
    pTempModel = CModelManager::CreateModel("kuzure_a");
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pTempModel);

    //
    //  init movement & pos & rot
    //    
    m_pCrumbleFloorMove = new CCrumbleFloorGimmickMove;
    m_pCrumbleFloorMove->SetPosition(&pBasicParam->m_vPosition);
    m_pCrumbleFloorMove->Start();
    
    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.SetRotation(&Math::VECTOR3_ZERO);
    m_model.UpdateFrame();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_SEESAW);
        ASSERT(m_hAtari);
    };
        
    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pCrumbleFloorMove);
};


CCrumbleFloorGimmick::~CCrumbleFloorGimmick(void)
{    
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    if (m_pCrumbleFloorMove)
    {
        delete m_pCrumbleFloorMove;
        m_pCrumbleFloorMove = nullptr;
    };
};


void CCrumbleFloorGimmick::PreMove(void)
{
    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    if (pModel)
        m_pCrumbleFloorMove->SetMatrixFromClump(pModel->GetClump());
};


void CCrumbleFloorGimmick::PostMove(void)
{
    if (m_model.GetCollisionModelClump())
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pCrumbleFloorMove->GetPosition(&vPosition);

        RwV3d vRotation = Math::VECTOR3_ZERO;
        m_pCrumbleFloorMove->GetTheta(&vRotation);

        m_model.SetPosition(&vPosition);
        m_model.SetRotation(&vRotation);

        RwV3d vRotationVel = Math::VECTOR3_ZERO;
        m_pCrumbleFloorMove->GetOmega(&vRotationVel);

        ASSERT(m_hAtari);
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, nullptr, &vRotationVel);
    };
};


void CCrumbleFloorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        m_pCrumbleFloorMove->SetCharacterPositionFromName(pszSender);
};