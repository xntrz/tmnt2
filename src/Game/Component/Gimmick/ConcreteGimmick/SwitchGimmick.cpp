#include "SwitchGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CSwitchGimmick::CSwitchGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_switchtype(SWITCHTYPE_SLOTMACHINE_LEVER)
, m_switchstate(SWITCHSTATE_OFF)
, m_fTimer(0.0f)
, m_bEnable(true)
{
    m_szTargetGimmickName[0] = '\0';

    ASSERT(pParam);
    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS*)pParam;

    CModel* pModelOn = CModelManager::CreateModel("switch_on");
    ASSERT(pModelOn);

    CModel* pModelOff = CModelManager::CreateModel("switch_off");
    ASSERT(pModelOff);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelOn);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pModelOff);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    
    SetModelStrategy(&m_model);

    m_switchtype = SWITCHTYPE(pInitParam->m_subid);
    ASSERT(m_switchtype >= 0 && m_switchtype < SWITCHTYPENUM);

    ASSERT(std::strlen(pInitParam->m_szTargetGimmick) < COUNT_OF(m_szTargetGimmickName));
    std::strcpy(m_szTargetGimmickName, pInitParam->m_szTargetGimmick);
};


CSwitchGimmick::~CSwitchGimmick(void)
{
    ;
};


GIMMICKTYPES::FEATURE CSwitchGimmick::GetFeatures(void) const
{
    return GIMMICKTYPES::FEATURE_NONE;
};


bool CSwitchGimmick::Query(CGimmickQuery* pQuery) const
{
    ASSERT(pQuery);

    if (pQuery->GetType() == GIMMICKTYPES::QUERY_SYS_SWITCH)
    {
        CStateGimmickQuery* pStateQuery = (CStateGimmickQuery*)pQuery;
        ++pStateQuery->m_nTarget;
        
        if (m_switchstate == SWITCHSTATE_ON)
        {
            ++pStateQuery->m_nState;
            return true;
        };
    };

    return CGimmick::Query(pQuery);
};


void CSwitchGimmick::PostMove(void)
{
    if (m_bEnable)
    {
        CModel* pModel = m_model.GetVisualModel();
        ASSERT(pModel);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        pModel->GetPosition(&vPosition);
        
        RwSphere sphere = { 0 };
        sphere.center = vPosition;
        sphere.radius = 0.75f;
        
        CHitCatchData Catch;
        Catch.SetObject(GetHandle());
        Catch.SetObjectType(GetType());
        Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
        Catch.SetSphere(&sphere);

        CHitAttackManager::RegistCatch(&Catch);
    }
    else if (m_fTimer > 0.0f)
    {
        m_fTimer -= CGameProperty::GetElapsedTime();
        if (m_fTimer <= 0.0f)
        {
            m_bEnable = true;
            m_fTimer = 0.0f;
            if (m_switchtype != SWITCHTYPE_ONCE)
            {
                m_switchstate = SWITCHSTATE_OFF;
                if (m_switchtype == SWITCHTYPE_TOGGLE)
                {
                    CGimmickManager::PostEvent(m_szTargetGimmickName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
                    CGameSound::PlayObjectSE(this, SDCODE_SE(4178));
                };
            };
        };
    };


    switch (m_switchstate)
    {
    case SWITCHSTATE_OFF:
        m_model.SetVisualBreak();
        break;

    case SWITCHSTATE_ON:
        m_model.SetVisualNormal();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CSwitchGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        CGimmickManager::PostEvent(m_szTargetGimmickName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
};


void CSwitchGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);
    
    if (m_bEnable)
    {
        CGameObject* pAttacker = pAttack->GetObject();
        if (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CCharacter* pCharacter = (CCharacter*)pAttacker;
            if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
                onSwitchChange();
        };
    };
};


void CSwitchGimmick::onSwitchChange(void)
{
    switch (m_switchtype)
    {
    case 0:
        m_fTimer = 0.5f;
        m_switchstate = SWITCHSTATE_ON;
        m_bEnable = false;
        break;

    case 1:
        m_fTimer = 5.0f;
        m_switchstate = SWITCHSTATE_ON;
        m_bEnable = false;
        break;

    case 2:
        m_fTimer = 0.5f;
        m_switchstate = (m_switchstate == SWITCHSTATE_ON ? SWITCHSTATE_OFF : SWITCHSTATE_ON);
        m_bEnable = false;
        break;
        
    case 3:
        m_fTimer = 0.0f;
        m_switchstate = SWITCHSTATE_ON;
        m_bEnable = false;
        break;

    default:
        ASSERT(false);
        break;
    };


    switch (m_switchstate)
    {
    case SWITCHSTATE_OFF:
        CGimmickManager::PostEvent(m_szTargetGimmickName, GetName(), GIMMICKTYPES::EVENTTYPE_INACTIVATE);
        break;
        
    case SWITCHSTATE_ON:
        CGimmickManager::PostEvent(m_szTargetGimmickName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
        break;

    default:
        ASSERT(false);
        break;
    };

    CGameSound::PlayObjectSE(this, SDCODE_SE(4179));
};

