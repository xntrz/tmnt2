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
, m_model()
, m_hAtari(0)
, m_bFlashFlag(false)
, m_state(STATE_WAIT)
, m_fTimer(0.0f)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_szTargetGimmickName()
, m_nHitNum(0)
, m_fInvinT(0.0f)
{
    m_szTargetGimmickName[0] = '\0';

    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_TERMS*>(pParam);
    ASSERT(pInitParam);

    //
    //  Init model
    //
    CModel* pModelOn = CModelManager::CreateModel("console_on");
    ASSERT(pModelOn);

    CModel* pModelOff = CModelManager::CreateModel("console_off");
    ASSERT(pModelOff);

    CModel* pModelAtari = CModelManager::CreateModel("console_a");
    ASSERT(pModelAtari);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelOff);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pModelOn);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);
    
    m_vPosition = pInitParam->m_vPosition;
    m_model.SetPosition(&m_vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    m_model.SetRotation(&vRotation);
    
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    //
    //  Init dir
    //
    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    Math::Matrix_RotateY(&matrix, vRotation.y);

    RwV3d vDirection = { 0.0f, 0.0f, -1.0f };
    RwV3dTransformPoint(&vDirection, &vDirection, &matrix);
    m_vDirection = vDirection;    

    //
    //  Init other
    //
    collisionCreate(COLLTYPE_ON);
    m_bFlashFlag = false;
    
    ASSERT(std::strlen(pInitParam->m_szTargetGimmick) < COUNT_OF(m_szTargetGimmickName));
    std::strcpy(m_szTargetGimmickName, pInitParam->m_szTargetGimmick);
};


CConsoleGimmick::~CConsoleGimmick(void)
{
    collisionDestroy();
};


void CConsoleGimmick::GetPosition(RwV3d* pvPosition) const
{
    *pvPosition = m_vPosition;
};


void CConsoleGimmick::PostMove(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();

    switch (m_state)
    {
    case STATE_WAIT:
        {
            if (m_fInvinT <= 0.0f)
            {
                RwSphere hitSphere = {};
                hitSphere.center = m_vPosition;
                hitSphere.center.y += 0.75f;
                hitSphere.radius = 0.0005f;

                CHitCatchData hitCatch;
                hitCatch.SetObject(GetHandle());
                hitCatch.SetObjectType(GetType());
                hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
                hitCatch.SetSphere(&hitSphere);

                CHitAttackManager::RegistCatch(&hitCatch);
            }
            else
            {
                m_fInvinT -= CGameProperty::GetElapsedTime();
            };

            if (m_model.GetDrawType() || (m_fTimer <= 0.2f))
            {
                if ((m_model.GetDrawType() == CNormalGimmickModel::MODELTYPE_DRAW_BREAK) &&
                    (m_fTimer > 0.6f))
                {
                    m_model.SetVisualNormal();
                    m_fTimer = 0.0f;
                };
            }
            else
            {
                m_model.SetVisualBreak();
                m_fTimer = 0.0f;                
            };
        }
        break;

    case STATE_START_INPUT:
        {
            if (m_fTimer > 0.15f)
            {
                CGameSound::PlayObjectSE(this, SDCODE_SE(4221));
                m_state = STATE_INPUT;
            };
        }
        break;

    case STATE_INPUT:
        {
            if (m_fTimer > 1.0f)
            {
                m_model.SetVisualNormal();

                if (std::strcmp(m_szTargetGimmickName, ""))
                    CGimmickManager::PostEvent(m_szTargetGimmickName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);

                CGameSound::PlayObjectSE(this, SDCODE_SE(4148));
                m_state = STATE_UNLOCK;
            };
        }
        break;

    case STATE_UNLOCK:
        break;

    default:
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
    if (m_state != STATE_WAIT)
        return;
    
    CGameObject* pAttacker = pAttack->GetObject();
    if (!pAttack)
        return;

    if (pAttacker->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    CCharacter* pCharacter = static_cast<CCharacter*>(pAttacker);

    switch (pAttack->GetShape())
    {
    case CHitAttackData::SHAPE_SPHERE:
        {
            static const int32 HIT_NUM_FOR_ON = 10;

            RwSphere hitSphere = *pAttack->GetSphere();
            EFFECT_GENERIC::CallHitEffect(pAttack, &hitSphere.center);

            CGameSound::PlayObjectSE(this, SDCODE_SE(4197));

            if (pCharacter->GetCharacterType() == CCharacter::TYPE_PLAYER)
            {
                CPlayerCharacter* pPlayerCharacter = static_cast<CPlayerCharacter*>(pCharacter);
                if (pPlayerCharacter->GetID() == PLAYERID::ID_SPL)
                    m_nHitNum = (HIT_NUM_FOR_ON - 1);
            };
            
            if (++m_nHitNum >= HIT_NUM_FOR_ON)
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
    *pvDirection = m_vDirection;
};


void CConsoleGimmick::consoleOn(void)
{
    m_model.SetVisualBreak();

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

    MAPTYPES::GIMMICKTYPE gimmickType = (colltype == COLLTYPE_ON) ? MAPTYPES::GIMMICKTYPE_CONSOLE :
                                                                    MAPTYPES::GIMMICKTYPE_NORMAL;

    m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), gimmickType);    
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

