#include "TransferFloorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/TransferFloorGimmickMove.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CLinearTransferFloorGimmick::CLinearTransferFloorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_hAtari(0)
, m_pLinearMove(nullptr)
, m_vPrePos(Math::VECTOR3_ZERO)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_MOVEFLOOR_LINEAR* pInitParam = (GIMMICKPARAM::GIMMICK_MOVEFLOOR_LINEAR*)pParam;
    
    const char* apszMdlName[] =
    {
        "mvf_s",
        "mvf_l",
    };

    ASSERT(pInitParam->m_subid >= 0 && pInitParam->m_subid < COUNT_OF(apszMdlName));    
    CModel* pModel = CModelManager::CreateModel(apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);
    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
    CGimmickUtils::MatrixToRotation(&matrix, &vRotation);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    m_pLinearMove = new CLinearTransferFloorGimmickMove;
    ASSERT(m_pLinearMove);
    m_pLinearMove->SetPosition(&pInitParam->m_vPosition);
    m_pLinearMove->SetMoveTime(pInitParam->m_fMoveTime);
    m_pLinearMove->SetStopTime(pInitParam->m_fStopTime);
    m_pLinearMove->SetKind(pInitParam->m_kind);
    m_pLinearMove->SetStartPosition(&pInitParam->m_vPosStart);
    m_pLinearMove->SetGoalPosition(&pInitParam->m_vPosGoal);
    m_pLinearMove->Start();
    SetMoveStrategy(m_pLinearMove);

    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();
        
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
        ASSERT(m_hAtari);

        if (m_hAtari)
        {
            if (pInitParam->m_subid)
            {
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 3.0f);
            }
            else
            {
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 2.0f);
            };
        };
    };
};


CLinearTransferFloorGimmick::~CLinearTransferFloorGimmick(void)
{
    if (m_pLinearMove)
    {
        delete m_pLinearMove;
        m_pLinearMove = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CLinearTransferFloorGimmick::PreMove(void)
{
    ASSERT(m_pLinearMove);
    if (m_pLinearMove)
        m_pLinearMove->GetPosition(&m_vPrePos);
};


void CLinearTransferFloorGimmick::PostMove(void)
{
    ASSERT(m_pLinearMove);
    if (m_pLinearMove)
    {
        RwV3d vPostPos = Math::VECTOR3_ZERO;
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        
        m_pLinearMove->GetPosition(&vPostPos);
        Math::Vec3_Sub(&vVelocity, &vPostPos, &m_vPrePos);

        ASSERT(m_hAtari);
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, &vVelocity);
    };
};


void CLinearTransferFloorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        ASSERT(m_pLinearMove);
        
        if (m_pLinearMove)
        {
            if (m_pLinearMove->GetKind() == 2)
                m_pLinearMove->StartTransfer();
        };
    };
};