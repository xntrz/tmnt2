#include "NormalDoorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickMotion.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ const CNormalDoorGimmick::KINDINFO CNormalDoorGimmick::m_aDoorKindInfoList []
{
    { "door1", SDCODE_SE(0x1087), },
    { "door2", SDCODE_SE(0x1084), },
    { "door3", SDCODE_SE(0x1084), },
    { "door4", SDCODE_SE(0x1084), },
    { "door5", SDCODE_SE(0x1086), },
    { "door6", SDCODE_SE(0x1081), },
    { "door7", SDCODE_SE(0x1080), },
    { "door8", SDCODE_SE(0x1081), },
};


CNormalDoorGimmick::CNormalDoorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pMotion(nullptr)
, m_hAtari(0)
, m_state(STATE_WAIT)
, m_subid(0)
, m_fMotionRemainTime(0.0f)
, m_bStart(false)
{
    m_szMotionName[0] = '\0';
    
    m_apModel[MODELTYPE_DISP] = nullptr;
    m_apModel[MODELTYPE_COLL] = nullptr;

    static_assert(COUNT_OF(m_aDoorKindInfoList) == CNormalDoorGimmick::SUBIDNUM, "update me");

    ASSERT(pParam);
    
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    m_subid = pInitParam->m_subid;

    std::strcpy(m_szMotionName, kindinfo().m_pszModelName);

    CModel* pModelDisp = CModelManager::CreateModel(kindinfo().m_pszModelName);

    char szModelNameAtariBuff[16];
    szModelNameAtariBuff[0] = '\0';
    sprintf(szModelNameAtariBuff, "%s_a", kindinfo().m_pszModelName);

    CModel* pModelColl = CModelManager::CreateModel(szModelNameAtariBuff);

    ASSERT(pModelDisp);
    ASSERT(pModelColl);

    m_apModel[MODELTYPE_DISP] = pModelDisp;
    m_apModel[MODELTYPE_COLL] = pModelColl;

    for (int32 i = 0; i < COUNT_OF(m_apModel); ++i)
    {
        RwV3d vRotation = Math::VECTOR3_ZERO;
        RwV3d vPosition = pInitParam->m_vPosition;
        
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
        CGimmickUtils::MatrixToRotation(&matrix, &vRotation);

        m_apModel[i]->SetPosition(&vPosition);
        m_apModel[i]->SetRotation(&vRotation);
        m_apModel[i]->UpdateFrame();
    };

    if (pModelColl->GetClump())
    {
        RpClump* pClump = pModelColl->GetClump();
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);
    };

    m_model.SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModelDisp);
    m_model.SetModel(CNormalGimmickModel::MODELKIND_ATARI_NORMAL, pModelColl);
    SetModelStrategy(&m_model);

    m_pMotion = new CGimmickMotion(pModelDisp);
    ASSERT(m_pMotion);
    CMotionManager::SetCurrentMotionSet(m_szMotionName);
    m_pMotion->AddMotion(m_szMotionName);
    m_pMotion->SetMotion(m_szMotionName, 0.0f, 0.0f, 0.0f, false);    
};


CNormalDoorGimmick::~CNormalDoorGimmick(void)
{
    if (m_pMotion)
    {
        delete m_pMotion;
        m_pMotion = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CNormalDoorGimmick::PreMove(void)
{
	m_pMotion->Update();

    switch (m_state)
    {
    case STATE_WAIT:
        {
            if (m_bStart)
            {
                m_pMotion->Start(1.0f);
                m_fMotionRemainTime = m_pMotion->GetEndTime();
                m_state = STATE_OPENING;
                m_bStart = false;

                if (kindinfo().m_nSE)
                    CGameSound::PlayObjectSE(this, kindinfo().m_nSE);
            };
        }
        break;

    case STATE_OPENING:
        {
            m_fMotionRemainTime -= CGameProperty::GetElapsedTime();
            if (m_fMotionRemainTime <= 0.0f)
            {
                m_pMotion->Stop();
                m_state = STATE_OPENED;

                if (m_hAtari)
                {
                    CMapCollisionModel::RemoveCollisionModel(m_hAtari);
                    m_hAtari = 0;
                };
            };
        }
        break;

    case STATE_OPENED:
        {
            ;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CNormalDoorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_state == STATE_WAIT)
            m_bStart = true;
    };
};


const CNormalDoorGimmick::KINDINFO& CNormalDoorGimmick::kindinfo(void) const
{
    ASSERT(m_subid >= 0 && m_subid < COUNT_OF(m_aDoorKindInfoList));
    return m_aDoorKindInfoList[m_subid];
};