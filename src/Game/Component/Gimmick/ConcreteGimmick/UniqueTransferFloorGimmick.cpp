#include "UniqueTransferFloorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/TransferFloorGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CUniqueTransferFloorGimmickModel::CUniqueTransferFloorGimmickModel(void)
: CGimmickModel(TYPENUM)
{
    ;
};


void CUniqueTransferFloorGimmickModel::SetModel(TYPE type, CModel* pModel)
{
    ASSERT(type >= 0);
    ASSERT(type < m_nNumGimmickModel);
    ASSERT(!m_apModel[type]);

    m_apModel[type] = pModel;
};


RpClump* CUniqueTransferFloorGimmickModel::GetModelClump(TYPE type)
{
    ASSERT(type >= 0);
    ASSERT(type < m_nNumGimmickModel);

    return m_apModel[type]->GetClump();
};


//
// *********************************************************************************
//


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


/*virtual*/ CUniqueTransferFloorGimmick::~CUniqueTransferFloorGimmick(void)
{
    if (m_hAtari0)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari0);
        m_hAtari0 = 0;
    };

    if (m_hAtari1)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari1);
        m_hAtari1 = 0;
    };

    if (m_pTransferMove)
    {
        delete m_pTransferMove;
        m_pTransferMove = nullptr;
    };
};


uint32 CUniqueTransferFloorGimmick::CreateCollisionModel(CUniqueTransferFloorGimmickModel::TYPE modelType, float fInitialRadius)
{
    RpClump* pClump = m_model.GetModelClump(modelType);
    ASSERT(pClump != nullptr);

    uint32 hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
    if (hAtari)
        CMapCollisionModel::SetBoundingSphereRadius(hAtari, fInitialRadius);

    return hAtari;
};


//
// *********************************************************************************
//


CUniqueLinearTransferFloorGimmick::CUniqueLinearTransferFloorGimmick(const char* pszName, void* pParam)
: CUniqueTransferFloorGimmick(pszName, pParam)
{
	m_fOneWayMoveTime = 10.0f;

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    //
    //  Init model
    //
    CModel* pModel = CModelManager::CreateModel("m20f_mfloor");
    ASSERT(pModel);

    CModel* pAtariModel0 = CModelManager::CreateModel("m20f_mfloor");
    ASSERT(pAtariModel0);

    CModel* pAtariModel1 = CModelManager::CreateModel("m20f_mfloor_a");
    ASSERT(pAtariModel1);
    
    m_model.SetModel(CUniqueTransferFloorGimmickModel::TYPE_DISP, pModel);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI0, pAtariModel0);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI1, pAtariModel1);

    m_model.SetPosition(&pInitParam->m_vPosition);

    RwV3d vRot = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };
    m_model.SetRotation(&vRot);
    
    SetModelStrategy(&m_model);

    //
    //  Init movement
    //
    RwV3d vPosStart = pInitParam->m_vPosition;
    RwV3d vPosGoal = { vPosStart.x, vPosStart.y + 10.0f, vPosStart.z };

    CLinearTransferFloorGimmickMove* pLinearMove = new CLinearTransferFloorGimmickMove;
    pLinearMove->SetPosition(&vPosStart);
    pLinearMove->SetStartPosition(&vPosStart);
    pLinearMove->SetGoalPosition(&vPosGoal);
    pLinearMove->SetMoveTime(m_fOneWayMoveTime);
    pLinearMove->SetStopTime(0.0f);
    pLinearMove->SetKind(2);
	pLinearMove->Start();

    m_pTransferMove = pLinearMove;
    SetMoveStrategy(m_pTransferMove);

    //
    // Init atari
    //
    m_hAtari0 = CreateCollisionModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI0, 5.0f);    
};


/*virtual*/ CUniqueLinearTransferFloorGimmick::~CUniqueLinearTransferFloorGimmick(void)
{
    if (m_eState == STATE_RUN)
        CGameSound::StopSE(SDCODE_SE(0x1073));
};


/*virtual*/ void CUniqueLinearTransferFloorGimmick::PreMove(void) /*override*/
{
    m_pTransferMove->GetPosition(&m_vPreMovePosition);
};


/*virtual*/ void CUniqueLinearTransferFloorGimmick::PostMove(void) /*override*/
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


/*virtual*/ void CUniqueLinearTransferFloorGimmick::OnReceiveEvent(const char* pszSender,
                                                                   GIMMICKTYPES::EVENTTYPE eventtype) /*override*/
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_eState == STATE_WAIT)
        {
            m_pTransferMove->StartTransfer();
            m_hAtari1 = CreateCollisionModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI1, 5.0f);
            m_eState = STATE_RUN;

            CGameSound::PlayObjectSE(this, SDCODE_SE(0x1073));
        };
    };
};


//
// *********************************************************************************
//


CUniquePathTransferFloorGimmick::CUniquePathTransferFloorGimmick(const char* pszName, void* pParam)
: CUniqueTransferFloorGimmick(pszName, pParam)
{
    m_fOneWayMoveTime = 60.0f;

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    //
    //  Init model
    //
    CModel* pModel = CModelManager::CreateModel("m04n_mfloor");
    ASSERT(pModel);
    pModel->SetLightingEnable(false);

    CModel* pAtariModel0 = CModelManager::CreateModel("m04n_mfloor");
    ASSERT(pAtariModel0);

    CModel* pAtariModel1 = CModelManager::CreateModel("m04n_mfloor_a");
    ASSERT(pAtariModel1);
    
    m_model.SetModel(CUniqueTransferFloorGimmickModel::TYPE_DISP, pModel);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI0, pAtariModel0);
    m_model.SetModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI1, pAtariModel1);
    
    m_model.SetPosition(&pInitParam->m_vPosition);

    RwV3d vRot = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };
    m_model.SetRotation(&vRot);

    SetModelStrategy(&m_model);

    //
    //  Init movement
    //
    CPathTransferFloorGimmickMove* pPathMove = new CPathTransferFloorGimmickMove;
    pPathMove->SetPathName("elevator");
    pPathMove->SetMoveTime(m_fOneWayMoveTime);
    pPathMove->SetStopTime(0.0f);
    pPathMove->SetKind(2);
    pPathMove->Start();

    m_pTransferMove = pPathMove;
    SetMoveStrategy(m_pTransferMove);

    //
    //  Init atari
    //    
    m_hAtari0 = CreateCollisionModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI0, 5.0f);    
};


/*virtual*/ CUniquePathTransferFloorGimmick::~CUniquePathTransferFloorGimmick(void)
{
    if (m_eState == STATE_RUN)
        CGameSound::StopSE(SDCODE_SE(0x1073));
};


/*virtual*/ void CUniquePathTransferFloorGimmick::PreMove(void) /*override*/
{
    m_pTransferMove->GetPosition(&m_vPreMovePosition);
};


/*virtual*/ void CUniquePathTransferFloorGimmick::PostMove(void) /*override*/
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


/*virtual*/ void CUniquePathTransferFloorGimmick::OnReceiveEvent(const char* pszSender,
                                                                 GIMMICKTYPES::EVENTTYPE eventtype) /*override*/
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_eState == STATE_WAIT)
        {
            m_pTransferMove->StartTransfer();
            m_hAtari1 = CreateCollisionModel(CUniqueTransferFloorGimmickModel::TYPE_ATARI1, 10.0f);
            m_eState = STATE_RUN;

            CGameSound::PlayObjectSE(this, SDCODE_SE(0x1073));
        };
    };
};