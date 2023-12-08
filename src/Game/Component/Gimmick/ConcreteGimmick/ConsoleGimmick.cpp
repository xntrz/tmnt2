#include "ConsoleGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CConsoleGimmick::CConsoleGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_hAtari(0)
, m_bFlashFlag(false)
, m_state(STATE_WAIT)
, m_fTimer(0.0f)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_nHitNum(0)
, m_fInvinT(0.0f)
{
    m_szTargetGimmickName[0] = '\0';

    ASSERT(pParam);
    
    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS*)pParam;

    CModel* pModelOn = CModelManager::CreateModel("console_on");
    CModel* pModelOff = CModelManager::CreateModel("console_off");
    CModel* pModelAtari = CModelManager::CreateModel("console_a");

    ASSERT(pModelOn);
    ASSERT(pModelOff);
    ASSERT(pModelAtari);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelOn);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pModelOff);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);
    
    m_vPosition = pInitParam->m_vPosition;
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    
    m_model.SetPosition(&m_vPosition);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    RwV3d vDirection = { 0.0f, 0.0f, -1.0f };

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    Math::Matrix_RotateY(&matrix, vRotation.y);
    RwV3dTransformPoint(&vDirection, &vDirection, &matrix);    
    m_vDirection = vDirection;

    collisionCreate(COLLTYPE_ON);
    m_bFlashFlag = false;
    
    ASSERT(std::strlen(pInitParam->m_szTargetGimmick) < COUNT_OF(m_szTargetGimmickName));
    std::strcpy(m_szTargetGimmickName, pInitParam->m_szTargetGimmick);
};


CConsoleGimmick::~CConsoleGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CConsoleGimmick::GetPosition(RwV3d* pvPosition) const
{
    *pvPosition = m_vPosition;
};


void CConsoleGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_WAIT:
        {
            if (m_fInvinT <= 0.0f)
            {
                RwSphere sphere = { 0 };
                sphere.center = m_vPosition;
                sphere.center.y += 0.75f;
                sphere.radius = 0.25f;

                CHitCatchData Catch;
                Catch.SetObject(GetHandle());
                Catch.SetObjectType(GetType());
                Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
                Catch.SetSphere(&sphere);

                CHitAttackManager::RegistCatch(&Catch);
            }
            else
            {
                m_fInvinT -= CGameProperty::GetElapsedTime();
            };

            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer >= 0.2f)
            {
                if (m_bFlashFlag)
                {
                    m_fTimer = 0.0f;
                    m_bFlashFlag = false;
                    m_model.SetVisualNormal();
                }
                else
                {
                    m_fTimer = 0.0f;
                    m_bFlashFlag = true;
                    m_model.SetVisualBreak();
                };
            };
        }
        break;

    case STATE_START_INPUT:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer >= 0.15f)
            {
                CGameSound::PlayObjectSE(this, SDCODE_SE(4221));
                m_state = STATE_INPUT;
                m_fTimer = 0.0f;
            };
        }
        break;

    case STATE_INPUT:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > 1.0f)
            {
                if (std::strcmp(m_szTargetGimmickName, ""))
                    CGimmickManager::PostEvent(m_szTargetGimmickName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);

                CGameSound::PlayObjectSE(this, SDCODE_SE(4148));
                m_state = STATE_UNLOCK;
                m_fTimer = 0.0f;
            };
        }
        break;

    case STATE_UNLOCK:
        {
            ;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CConsoleGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_state == STATE_WAIT)
            consoleOn();
    };
};


void CConsoleGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);

    if (m_state != STATE_WAIT)
        return;
    
    CGameObject* pAttacker = pAttack->GetObject();
    ASSERT(pAttacker);

    if (pAttacker->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    CCharacter* pCharacter = (CCharacter*)pAttacker;

    switch (pAttack->GetShape())
    {
    case CHitAttackData::SHAPE_SPHERE:
        {
            RwSphere sphere = *pAttack->GetSphere();
            EFFECT_GENERIC::CallHitEffect(pAttack, &sphere.center);
            CGameSound::PlayObjectSE(this, SDCODE_SE(4197));

            if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
            {
                CPlayerCharacter* pPlayerCharacter = (CPlayerCharacter*)pCharacter;
                if (pPlayerCharacter->GetID() == PLAYERID::ID_SPL)
                    m_nHitNum = 10;
            };
            
            if (++m_nHitNum >= 10)
                consoleOn();
            else
                m_fInvinT = 0.1f;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CConsoleGimmick::GetDirection(RwV3d* pvDirection)
{
    ASSERT(pvDirection);
    *pvDirection = m_vDirection;
};


void CConsoleGimmick::consoleOn(void)
{
    collisionDestroy();
    collisionCreate(COLLTYPE_OFF);
    m_fTimer = 0.0f;
    m_state = STATE_START_INPUT;
};


void CConsoleGimmick::collisionCreate(COLLTYPE colltype)
{
    ASSERT(!m_hAtari);

    RpClump* pClump = m_model.GetCollisionModelClump();
    ASSERT(pClump);

    m_hAtari = CMapCollisionModel::RegistCollisionModel(
        pClump,
        GetName(),
        colltype == COLLTYPE_ON ? MAPTYPES::GIMMICKTYPE_CONSOLE : MAPTYPES::GIMMICKTYPE_NORMAL
    );
    
    ASSERT(m_hAtari);
    if (m_hAtari)
        CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 2.0f);
};


void CConsoleGimmick::collisionDestroy(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};

