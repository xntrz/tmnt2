#include "RideShutterGimmick.hpp"

#include "Game/Component/Gimmick/GimmickMotion.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"


CRideShutterGimmick::CRideShutterGimmick(const char* pszName, void* pParam)
: CRideGimmick(pszName, pParam)
, m_pMotionController(nullptr)
, m_pMotionControllerAtari(nullptr)
, m_hAtari(0)
, m_bRunFlag(false)
{
    m_fRadius = 0.5f;
    m_fOffset = 0.5f;
    m_iPower = 0;
    m_bCatchHit = false;

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    //
    //  Init model
    //
    CModel* pModel = CModelManager::CreateModel("shutter");
    ASSERT(pModel);
    pModel->SetClippingEnable(false);

    CModel* pModelAtari = CModelManager::CreateModel("shutter_a");
    ASSERT(pModelAtari);

    RwMatrix mat;
    RwMatrixSetIdentity(&mat);
    CGimmickUtils::QuaternionToRotationMatrix(&mat, &pInitParam->m_quat);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    CGimmickUtils::MatrixToRotation(&mat, &vRotation);
    
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();

    SetModelStrategy(&m_model);

    //
    //  Init motion
    //
    CMotionManager::SetCurrentMotionSet("shutter");
    m_pMotionController = new CGimmickMotion(pModel);
    m_pMotionController->AddMotion("close");
    m_pMotionController->SetMotion("close", 0.0f, 0.0f, 0.0f, false);
    m_pMotionController->StartOne(1.0f);

    CMotionManager::SetCurrentMotionSet("shutter");
    m_pMotionControllerAtari = new CGimmickMotion(pModelAtari);
    m_pMotionControllerAtari->AddMotion("close");
    m_pMotionControllerAtari->SetMotion("close", 0.0f, 0.0f, 0.0f, false);
    m_pMotionControllerAtari->StartOne(1.0f);

    //
    //  Init atari
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
    };

    m_bRunFlag = false;
};


CRideShutterGimmick::~CRideShutterGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    if (m_pMotionController)
    {
        delete m_pMotionController;
        m_pMotionController = nullptr;
    };

    if (m_pMotionControllerAtari)
    {
        delete m_pMotionControllerAtari;
        m_pMotionControllerAtari = nullptr;
    };
};


void CRideShutterGimmick::PostMove(void)
{
    if (m_bRunFlag)
    {
        m_pMotionController->Update();
        m_pMotionControllerAtari->Update();
    };

    if (m_hAtari)
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, nullptr, nullptr);
};


void CRideShutterGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        m_bRunFlag = true;
};