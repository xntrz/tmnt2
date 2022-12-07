#include "UniqueTransferFloorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/TransferFloorGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CUniqueTransferFloorGimmickModel::CUniqueTransferFloorGimmickModel(void)
: CGimmickModel(MODELTYPE_NUM)
{
    ;
};


CUniqueTransferFloorGimmickModel::~CUniqueTransferFloorGimmickModel(void)
{
    ;
};


void CUniqueTransferFloorGimmickModel::SetModel(MODELTYPE Modeltype, CModel* pModel)
{
    ASSERT((Modeltype >= 0) && (Modeltype < m_nNumGimmickModel));
    ASSERT(!m_apModel[Modeltype]);
    m_apModel[Modeltype] = pModel;
};


RpClump* CUniqueTransferFloorGimmickModel::GetModelClump(MODELTYPE Modeltype)
{
    ASSERT((Modeltype >= 0) && (Modeltype < m_nNumGimmickModel));
    return m_apModel[Modeltype]->GetClump();
};


CUniqueTransferFloorGimmick::CUniqueTransferFloorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pTransferMove(nullptr)
, m_model()
, m_hAtari0(0)
, m_hAtari1(0)
, m_vPreMovePosition(Math::VECTOR3_ZERO)
, m_fMoveTime(0.0f)
, m_fOneWayMoveTime(0.0f)
, m_eState(STATE_WAIT)
{
    ;
};


CUniqueTransferFloorGimmick::~CUniqueTransferFloorGimmick(void)
{
    if (m_pTransferMove)
    {
        delete m_pTransferMove;
        m_pTransferMove = nullptr;
    };
};


CUniqueLinearTransferFloorGimmick::CUniqueLinearTransferFloorGimmick(const char* pszName, void* pParam)
: CUniqueTransferFloorGimmick(pszName, pParam)
{
	m_fOneWayMoveTime = 10.0f;

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    ASSERT(pInitParam);

    CModel* pModel = CModelManager::CreateModel("m20f_mfloor");
    CModel* pAtariModel0 = CModelManager::CreateModel("m20f_mfloor");
    CModel* pAtariModel1 = CModelManager::CreateModel("m20f_mfloor_a");
    ASSERT(pModel);
    ASSERT(pAtariModel0);
    ASSERT(pAtariModel1);    
    m_model.SetModel(CUniqueTransferFloorGimmickModel::MODELTYPE_DISP, pModel);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI0, pAtariModel0);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI1, pAtariModel1);
    m_model.SetPosition(&pInitParam->m_vPosition);
    RwV3d vRot = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };
    m_model.SetRotation(&vRot);
    
    SetModelStrategy(&m_model);

    RwV3d vPosStart = pInitParam->m_vPosition;
    RwV3d vPosGoal = { vPosStart.x, vPosStart.y + 10.0f, vPosStart.z };
    CLinearTransferFloorGimmickMove* pLinearMove = new CLinearTransferFloorGimmickMove();
    ASSERT(pLinearMove);
    pLinearMove->SetPosition(&vPosStart);
    pLinearMove->SetStartPosition(&vPosStart);
    pLinearMove->SetGoalPosition(&vPosGoal);
    pLinearMove->SetMoveTime(m_fOneWayMoveTime);
    pLinearMove->SetStopTime(0.0f);
    pLinearMove->SetKind(2);
	pLinearMove->Start();

    m_pTransferMove = pLinearMove;
    SetMoveStrategy(m_pTransferMove);

    ASSERT(m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI0));
    m_hAtari0 = CMapCollisionModel::RegistCollisionModel(
        m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI0),
        GetName(),
        MAPTYPES::GIMMICKTYPE_NORMAL
    );
    if (m_hAtari0)
        CMapCollisionModel::SetBoundingSphereRadius(m_hAtari0, 5.0f);
};


CUniqueLinearTransferFloorGimmick::~CUniqueLinearTransferFloorGimmick(void)
{
    ;
};


void CUniqueLinearTransferFloorGimmick::PreMove(void)
{
    m_pTransferMove->GetPosition(&m_vPreMovePosition);
};


void CUniqueLinearTransferFloorGimmick::PostMove(void)
{
    if (m_eState == STATE_RUN)
    {
        RwV3d vPostMovePosition = Math::VECTOR3_ZERO;
        m_pTransferMove->GetPosition(&vPostMovePosition);

        RwV3d vFrameVel = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vFrameVel, &vPostMovePosition, &m_vPreMovePosition);

        if (m_hAtari0)
            CMapCollisionModel::UpdateCollisionModel(m_hAtari0, &vFrameVel);

        if (m_hAtari1)
            CMapCollisionModel::UpdateCollisionModel(m_hAtari1, &vFrameVel);

        m_fMoveTime += CGameProperty::GetElapsedTime();
        if (m_fMoveTime > m_fOneWayMoveTime)
        {
            CGameSound::StopSE(SDCODE_SE(0x1073));
            m_eState = STATE_STOP;
        };
    };
};


void CUniqueLinearTransferFloorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_eState == STATE_WAIT)
        {
            m_pTransferMove->StartTransfer();
            
            ASSERT(m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI1));
            m_hAtari1 = CMapCollisionModel::RegistCollisionModel(
                m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI1),
                GetName(),
                MAPTYPES::GIMMICKTYPE_NORMAL
            );
            if (m_hAtari1)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari1, 5.0f);

            CGameSound::PlayObjectSE(this, SDCODE_SE(0x1073));

            m_eState = STATE_RUN;
        };
    };
};


CUniquePathTransferFloorGimmick::CUniquePathTransferFloorGimmick(const char* pszName, void* pParam)
: CUniqueTransferFloorGimmick(pszName, pParam)
{
    m_fOneWayMoveTime = 60.0f;

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    ASSERT(pInitParam);

    CModel* pModel = CModelManager::CreateModel("m04n_mfloor");
    CModel* pAtariModel0 = CModelManager::CreateModel("m04n_mfloor");
    CModel* pAtariModel1 = CModelManager::CreateModel("m04n_mfloor_a");
    ASSERT(pModel);
    ASSERT(pAtariModel0);
    ASSERT(pAtariModel1);
	pModel->SetLightingEnable(false);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::MODELTYPE_DISP, pModel);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI0, pAtariModel0);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI1, pAtariModel1);
    m_model.SetPosition(&pInitParam->m_vPosition);
    RwV3d vRot = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };
    m_model.SetRotation(&vRot);

    SetModelStrategy(&m_model);

    CPathTransferFloorGimmickMove* pPathMove = new CPathTransferFloorGimmickMove();
    ASSERT(pPathMove);
    pPathMove->SetPathName("elevator");
    pPathMove->SetMoveTime(m_fOneWayMoveTime);
    pPathMove->SetStopTime(0.0f);
    pPathMove->SetKind(2);
    pPathMove->Start();

    m_pTransferMove = pPathMove;
    SetMoveStrategy(m_pTransferMove);

    ASSERT(m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI0));
    m_hAtari0 = CMapCollisionModel::RegistCollisionModel(
        m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI0),
        GetName(),
        MAPTYPES::GIMMICKTYPE_NORMAL
    );
    if (m_hAtari0)
        CMapCollisionModel::SetBoundingSphereRadius(m_hAtari0, 5.0f);
};


CUniquePathTransferFloorGimmick::~CUniquePathTransferFloorGimmick(void)
{
    ;
};


void CUniquePathTransferFloorGimmick::PreMove(void)
{
    m_pTransferMove->GetPosition(&m_vPreMovePosition);
};


void CUniquePathTransferFloorGimmick::PostMove(void)
{
    if (m_eState == STATE_RUN)
    {
        RwV3d vPostMovePosition = Math::VECTOR3_ZERO;
        m_pTransferMove->GetPosition(&vPostMovePosition);

        RwV3d vFrameVel = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vFrameVel, &vPostMovePosition, &m_vPreMovePosition);

        if (vFrameVel.y < 0.0f)
        {
			vFrameVel.x = vFrameVel.x + vFrameVel.x;
            vFrameVel.y = 0.0f;
			vFrameVel.z = vFrameVel.z + vFrameVel.z;
        };

        if (m_hAtari0)
            CMapCollisionModel::UpdateCollisionModel(m_hAtari0, &vFrameVel);

        if (m_hAtari1)
            CMapCollisionModel::UpdateCollisionModel(m_hAtari1, &vFrameVel);

        m_fMoveTime += CGameProperty::GetElapsedTime();
        if (m_fMoveTime > m_fOneWayMoveTime)
        {
            if (m_hAtari1)
            {
                CMapCollisionModel::RemoveCollisionModel(m_hAtari1);
                m_hAtari1 = 0;
            };
            
            CGameSound::StopSE(SDCODE_SE(0x1073));
            m_eState = STATE_STOP;
        };
    };
};


void CUniquePathTransferFloorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_eState == STATE_WAIT)
        {
            m_pTransferMove->StartTransfer();

            ASSERT(m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI1));
            m_hAtari1 = CMapCollisionModel::RegistCollisionModel(
                m_model.GetModelClump(CUniqueTransferFloorGimmickModel::MODELTYPE_ATARI1),
                GetName(),
                MAPTYPES::GIMMICKTYPE_NORMAL
            );
            if (m_hAtari1)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari1, 10.0f);

            CGameSound::PlayObjectSE(this, SDCODE_SE(0x1073));

            m_eState = STATE_RUN;
        };
    };
};