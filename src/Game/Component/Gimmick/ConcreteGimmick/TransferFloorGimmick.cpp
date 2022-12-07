#include "TransferFloorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
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

    const char* apszMdlName[] = { "mvf_s", "mvf_l", };
    ASSERT( (pInitParam->m_subid) >= 0 && (pInitParam->m_subid < COUNT_OF(apszMdlName)) );
    
    CModel* pModel = CModelManager::CreateModel(apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
    CGimmickUtils::MatrixToRotation(&matrix, &vRotation);
    
    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    CLinearTransferFloorGimmickMove* pLinearMove = new CLinearTransferFloorGimmickMove;
    ASSERT(pLinearMove);
    pLinearMove->SetPosition(&pInitParam->m_vPosition);
    pLinearMove->SetMoveTime(pInitParam->m_fMoveTime);
    pLinearMove->SetStopTime(pInitParam->m_fStopTime);
    pLinearMove->SetKind(pInitParam->m_kind);
    pLinearMove->SetStartPosition(&pInitParam->m_vPosStart);
    pLinearMove->SetGoalPosition(&pInitParam->m_vPosGoal);
    pLinearMove->Start();
    m_pTransferMove = pLinearMove;
    SetMoveStrategy(m_pTransferMove);

    if (m_model.GetCollisionModelClump())
    {
        m_hAtari = CMapCollisionModel::RegistCollisionModel(
            m_model.GetCollisionModelClump(),
            GetName(),
            MAPTYPES::GIMMICKTYPE_NORMAL
        );
        
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, (pInitParam->m_subid ? 3.0f : 2.0f));
    };
};


CLinearTransferFloorGimmick::~CLinearTransferFloorGimmick(void)
{
    ;
};


CPathTransferFloorGimmick::CPathTransferFloorGimmick(const char* pszName, void* pParam)
: CTransferFloorGimmick(pszName, pParam)
{
    GIMMICKPARAM::GIMMICK_MOVEFLOOR_PATH* pInitParam = (GIMMICKPARAM::GIMMICK_MOVEFLOOR_PATH*)pParam;
    ASSERT(pInitParam);

    const char* apszMdlName[] = { "mvf_s", "mvf_l", };
    ASSERT((pInitParam->m_subid) >= 0 && (pInitParam->m_subid < COUNT_OF(apszMdlName)));

    CModel* pModel = CModelManager::CreateModel(apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
    CGimmickUtils::MatrixToRotation(&matrix, &vRotation);

    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    CPathTransferFloorGimmickMove* pPathMove = new CPathTransferFloorGimmickMove;
    ASSERT(pPathMove);
    pPathMove->SetPosition(&pInitParam->m_vPosition);
    pPathMove->SetMoveTime(pInitParam->m_fMoveTime);
    pPathMove->SetStopTime(pInitParam->m_fStopTime);
    pPathMove->SetKind(pInitParam->m_kind);
    pPathMove->SetPathName(pInitParam->m_szPathName);
    pPathMove->Start();
    m_pTransferMove = pPathMove;
    SetMoveStrategy(m_pTransferMove);

    if (m_model.GetCollisionModelClump())
    {
        m_hAtari = CMapCollisionModel::RegistCollisionModel(
            m_model.GetCollisionModelClump(),
            GetName(),
            MAPTYPES::GIMMICKTYPE_NORMAL
        );

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 5.0f);
    };
};


CPathTransferFloorGimmick::~CPathTransferFloorGimmick(void)
{
    ;
};


CRotateTransferFloorGimmick::CRotateTransferFloorGimmick(const char* pszName, void* pParam)
: CTransferFloorGimmick(pszName, pParam)
, m_pRotateMove(nullptr)
, m_vPreMoveRotation(Math::VECTOR3_ZERO)
, m_bSE(false)
{
    GIMMICKPARAM::GIMMICK_MOVEFLOOR_ROUND* pInitParam = (GIMMICKPARAM::GIMMICK_MOVEFLOOR_ROUND*)pParam;
    ASSERT(pInitParam);

    const char* apszMdlName[] =
    {
        "mvf_bar_l",
        "mvf_bar_s",
        "mvf_disk_l",
        "mvf_disk_s",
        "mvf_disk_s",
    };
    ASSERT((pInitParam->m_subid) >= 0 && (pInitParam->m_subid < COUNT_OF(apszMdlName)));

    CModel* pModel = CModelManager::CreateModel(apszMdlName[pInitParam->m_subid]);
    ASSERT(pModel);
    RwV3d vRotation = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };    
    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    CGimmickUtils::QuaternionToRotationMatrix(&matRot, &pInitParam->m_quat);

    CRotateTransferFloorGimmickMove* pRotateMove = new CRotateTransferFloorGimmickMove;
    ASSERT(pRotateMove);
    pRotateMove->SetPosition(&pInitParam->m_vPosition);
    pRotateMove->SetKind(3);
    pRotateMove->SetRotAxis(&matRot);
    pRotateMove->SetRoundTime(pInitParam->m_fRoundTime);
    pRotateMove->Start();
    m_pTransferMove = pRotateMove;
    SetMoveStrategy(m_pTransferMove);
    m_pRotateMove = pRotateMove;

    if (m_model.GetCollisionModelClump())
    {
        m_hAtari = CMapCollisionModel::RegistCollisionModel(
            m_model.GetCollisionModelClump(),
            GetName(),
            MAPTYPES::GIMMICKTYPE_NORMAL
        );

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 10.0f);
    };
};


CRotateTransferFloorGimmick::~CRotateTransferFloorGimmick(void)
{
    ;
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