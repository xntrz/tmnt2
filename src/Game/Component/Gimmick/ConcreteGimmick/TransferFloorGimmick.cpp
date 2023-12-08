#include "TransferFloorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/TransferFloorGimmickMove.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CTransferFloorGimmick::CTransferFloorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_hAtari(0)
, m_model()
, m_pTransferMove(nullptr)
, m_vPreMovePosition(Math::VECTOR3_ZERO)
{
    ;
};


CTransferFloorGimmick::~CTransferFloorGimmick(void)
{
    if (m_pTransferMove)
    {
        delete m_pTransferMove;
        m_pTransferMove = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CTransferFloorGimmick::PreMove(void)
{
    m_pTransferMove->GetPosition(&m_vPreMovePosition);
};


void CTransferFloorGimmick::PostMove(void)
{
    RwV3d vPostMovePosition = Math::VECTOR3_ZERO;
    m_pTransferMove->GetPosition(&vPostMovePosition);

    RwV3d vFrameVel = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vFrameVel, &vPostMovePosition, &m_vPreMovePosition);

    ASSERT(m_hAtari);
    CMapCollisionModel::UpdateCollisionModel(m_hAtari, &vFrameVel);
};


void CTransferFloorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_pTransferMove->GetKind() == 2)
            m_pTransferMove->StartTransfer();
    };
};


CLinearTransferFloorGimmick::CLinearTransferFloorGimmick(const char* pszName, void* pParam)
: CTransferFloorGimmick(pszName, pParam)
{
    GIMMICKPARAM::GIMMICK_MOVEFLOOR_LINEAR* pInitParam = (GIMMICKPARAM::GIMMICK_MOVEFLOOR_LINEAR*)pParam;
    ASSERT(pInitParam);

    static const char* s_apszMdlName[] =
    {
        "mvf_s",
        "mvf_l",
    };

    ASSERT(pInitParam->m_subid >= 0);
    ASSERT(pInitParam->m_subid < COUNT_OF(s_apszMdlName));

    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel(s_apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    
    //
    //  init pos & rot
    //
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);    
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
    
    RwV3d vRotation = Math::VECTOR3_ZERO;
    CGimmickUtils::MatrixToRotation(&matrix, &vRotation);
    
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    //
    //  init movement
    //
    CLinearTransferFloorGimmickMove* pLinearMove = new CLinearTransferFloorGimmickMove;
    pLinearMove->SetPosition(&pInitParam->m_vPosition);
    pLinearMove->SetMoveTime(pInitParam->m_fMoveTime);
    pLinearMove->SetStopTime(pInitParam->m_fStopTime);
    pLinearMove->SetKind(pInitParam->m_kind);
    pLinearMove->SetStartPosition(&pInitParam->m_vPosStart);
    pLinearMove->SetGoalPosition(&pInitParam->m_vPosGoal);
    pLinearMove->Start();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();
        
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, (pInitParam->m_subid ? 3.0f : 2.0f));
    };

    //
    //  setup model & move strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(pLinearMove);
    m_pTransferMove = pLinearMove;
};


CPathTransferFloorGimmick::CPathTransferFloorGimmick(const char* pszName, void* pParam)
: CTransferFloorGimmick(pszName, pParam)
{
    GIMMICKPARAM::GIMMICK_MOVEFLOOR_PATH* pInitParam = (GIMMICKPARAM::GIMMICK_MOVEFLOOR_PATH*)pParam;
    ASSERT(pInitParam);

    static const char* s_apszMdlName[] =
    {
        "mvf_s",
        "mvf_l",
    };
    
    ASSERT(pInitParam->m_subid >= 0);
    ASSERT(pInitParam->m_subid < COUNT_OF(s_apszMdlName));

    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel(s_apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    
    //
    //  init pos & rot
    //
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
    
    RwV3d vRotation = Math::VECTOR3_ZERO;
    CGimmickUtils::MatrixToRotation(&matrix, &vRotation);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    //
    //  init movement
    //
    CPathTransferFloorGimmickMove* pPathMove = new CPathTransferFloorGimmickMove;
    pPathMove->SetPosition(&pInitParam->m_vPosition);
    pPathMove->SetMoveTime(pInitParam->m_fMoveTime);
    pPathMove->SetStopTime(pInitParam->m_fStopTime);
    pPathMove->SetKind(pInitParam->m_kind);
    pPathMove->SetPathName(pInitParam->m_szPathName);
    pPathMove->Start();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();
        
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 5.0f);
    };

    //
    //  setup model & move strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(pPathMove);
    m_pTransferMove = pPathMove;
};


CRotateTransferFloorGimmick::CRotateTransferFloorGimmick(const char* pszName, void* pParam)
: CTransferFloorGimmick(pszName, pParam)
, m_pRotateMove(nullptr)
, m_vPreMoveRotation(Math::VECTOR3_ZERO)
, m_bSE(false)
{
    GIMMICKPARAM::GIMMICK_MOVEFLOOR_ROUND* pInitParam = (GIMMICKPARAM::GIMMICK_MOVEFLOOR_ROUND*)pParam;
    ASSERT(pInitParam);

    static const char* s_apszMdlName[] =
    {
        "mvf_bar_l",
        "mvf_bar_s",
        "mvf_disk_l",
        "mvf_disk_s",
        "mvf_disk_s",
    };

    ASSERT(pInitParam->m_subid >= 0);
    ASSERT(pInitParam->m_subid < COUNT_OF(s_apszMdlName));

    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel(s_apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);
    
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();

    //
    //  init movement
    //
    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    CGimmickUtils::QuaternionToRotationMatrix(&matRot, &pInitParam->m_quat);

    m_pRotateMove = new CRotateTransferFloorGimmickMove;
    m_pRotateMove->SetPosition(&pInitParam->m_vPosition);
    m_pRotateMove->SetKind(3);
    m_pRotateMove->SetRotAxis(&matRot);
    m_pRotateMove->SetRoundTime(pInitParam->m_fRoundTime);
    m_pRotateMove->Start();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 10.0f);
    };

    //
    //  setup model & move strategy
    //
    SetModelStrategy(&m_model);
	SetMoveStrategy(m_pRotateMove);
    m_pTransferMove = m_pRotateMove;
};


void CRotateTransferFloorGimmick::PreMove(void)
{
    m_pRotateMove->GetRotation(&m_vPreMoveRotation);
};


void CRotateTransferFloorGimmick::PostMove(void)
{
    RwV3d vPostMoveRotation = Math::VECTOR3_ZERO;
    m_pRotateMove->GetRotation(&vPostMoveRotation);
    
    m_model.SetRotation(&vPostMoveRotation);

    RwV3d vFrameVel = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vFrameVel, &vPostMoveRotation, &m_vPreMoveRotation);

    ASSERT(m_hAtari);
    CMapCollisionModel::UpdateCollisionModel(m_hAtari, nullptr, &vFrameVel);
};


void CRotateTransferFloorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    ;
};