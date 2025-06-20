#include "EggGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


static float s_aShakeRange[] =
{
    15.0f,  // IDLE
    12.0f,  // LONG
    9.0f,   // MIDDLE
    0.0f    // SHORT
};


static float s_aShakeInterval[] =
{
    0.0f,   // IDLE
    3.0f,   // LONG
    2.0f,   // MIDDLE
    1.0f    // SHORT
};


static int32 s_aShakeNum[] =
{
    0,      // IDLE
    5,      // LONG
    6,      // MIDDLE
    7       // SHORT
};


static float s_aShakeMin[] =
{
    0.0f,   // IDLE
    5.0f,   // LONG
    5.0f,   // MIDDLE
    10.0f   // SHORT
};


static float s_aShakeMax[] =
{
    0.0f,   // IDLE 
    10.0f,  // LONG
    15.0f,  // MIDDLE
    20.0f   // SHORT
};


static_assert(COUNT_OF(s_aShakeRange)   == CEggGimmick::SHAKESTATENUM, "checkout");
static_assert(COUNT_OF(s_aShakeInterval)== CEggGimmick::SHAKESTATENUM, "checkout");
static_assert(COUNT_OF(s_aShakeNum)     == CEggGimmick::SHAKESTATENUM, "checkout");
static_assert(COUNT_OF(s_aShakeMin)     == CEggGimmick::SHAKESTATENUM, "checkout");
static_assert(COUNT_OF(s_aShakeMax)     == CEggGimmick::SHAKESTATENUM, "checkout");


CEggGimmick::CEggGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_generator()
, m_model()
, m_move()
, m_pModuleMan(nullptr)
, m_fNearestPlayerDist(-1.0f)
, m_fTimer(0.0f)
, m_state(STATE_SHAKE)
, m_shakeState(SHAKESTATE_IDLE)
, m_nShake(0)
{
    GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_SINGLE* pAppearParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_SINGLE*>(pParam);

    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel("egg");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pAppearParam->m_quat);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pAppearParam->m_vPosition);

    //
    //  init movement
    //
    m_move.SetPosition(&pAppearParam->m_vPosition);
    
    //
    //  init generator
    //
    m_generator.Init(pAppearParam, true);
    
    //
    //  init module mananger
    //
    const float fShadowWH = 2.0f;
    
    m_pModuleMan = new CModuleManager;
    m_pModuleMan->Include(CCircleShadowModule::New(this, fShadowWH, fShadowWH, true));
    
    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(&m_move);
};


/*virtual*/ CEggGimmick::~CEggGimmick(void)
{
    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };
};


/*virtual*/ void CEggGimmick::Run(void) /*override*/
{
    eggCtrl();

    if (m_pModuleMan)
        m_pModuleMan->Run();
};


/*virtual*/ void CEggGimmick::Draw(void) const /*override*/
{
    CGimmick::Draw();
    
    if (m_pModuleMan)
        m_pModuleMan->Draw();
};


/*virtual*/ void CEggGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) /*override*/
{
    m_generator.HandleEvent(pszSender, eventtype);

    if (eventtype == GIMMICKTYPES::EVENTTYPE_ENEMYKILL)
        m_state = STATE_END;
};


/*virtual*/ void CEggGimmick::OnCatchAttack(CHitAttackData* pAttack) /*override*/
{
    if (m_state != STATE_SHAKE)
        return;

    CGameObject* pGameObj = CGameObjectManager::GetObject(pAttack->GetObjectHandle());
    switch (pGameObj->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        {
            CCharacter* pChara = static_cast<CCharacter*>(pGameObj);

            if (pChara->GetCharacterType() != CCharacter::TYPE_PLAYER)
                return;

            if ((pAttack->GetStatus() != CHitAttackData::STATUS_THROW) &&
                (pAttack->GetStatus() != CHitAttackData::STATUS_BIND))
            {
                birthForce();
            };
        }
        break;

    case GAMEOBJECTTYPE::EFFECT:
        {
            CMagic* pMagic = static_cast<CMagic*>(pGameObj);

            if ((pMagic->GetID() == MAGICID::ID_DON_LASER_BEAM)
                || (pMagic->GetID() == MAGICID::ID_EXPL_B1)
                || (pMagic->GetID() == MAGICID::ID_EXPL_B1_PLAYER)
                || (pMagic->GetID() == MAGICID::ID_EXPL_B1_CHARA))
            {
                if (CMagicManager::IsChargeAttack(pMagic))
                    birthForce();
            };
        }
        break;

    case GAMEOBJECTTYPE::SHOT:
        {
            if (pAttack->GetTarget() == CHitAttackData::TARGET_ENEMY_GIMMICK)
                birthForce();
        }
        break;

    default:
        break;
    };
};


void CEggGimmick::eggCtrl(void)
{
    switch (m_state)
    {
    case STATE_SHAKE:
        {
            shakeMoveGrandCtrl();
            setHitAttack();

            if (birth())
            {
                m_fTimer = 0.0f;
                m_state = STATE_GENERATE_ENEMY;
            };
        }
        break;

    case STATE_GENERATE_ENEMY:
        {
            if (enemyGenerate())
            {
                m_fTimer = 0.0f;
                m_state = STATE_ERASE_EGG;
            };
        }
        break;

    case STATE_ERASE_EGG:
        {
            if (eraseEgg())
            {
                m_fTimer = 0.0f;
                m_state = STATE_END;
            };
        }
        break;

    case STATE_END:
        {
            Release();
        }        
        break;

    default:
        ASSERT(false);
        return;
    };
};


void CEggGimmick::shakeMoveGrandCtrl(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_model.GetPosition(&vPosition);

    m_fNearestPlayerDist = CGimmickUtils::CalcNearestPlayerDistance(&vPosition);

    switch (m_shakeState)
    {
    case SHAKESTATE_IDLE:
        {
            if (m_fNearestPlayerDist < s_aShakeRange[SHAKESTATE_IDLE])
                changeShakeState(SHAKESTATE_LONG_RANGE);
        }
        break;
        
    case SHAKESTATE_LONG_RANGE:
        {
            if (m_fNearestPlayerDist > s_aShakeRange[SHAKESTATE_IDLE])
            {
                changeShakeState(SHAKESTATE_IDLE);
                break;
            };
            
            if (m_fNearestPlayerDist < s_aShakeRange[SHAKESTATE_LONG_RANGE])
            {
                changeShakeState(SHAKESTATE_MIDDLE_RANGE);
                break;
            };

            shakeMoveCtrl();
        }
        break;
        
    case SHAKESTATE_MIDDLE_RANGE:
        {
            if (m_fNearestPlayerDist > s_aShakeRange[SHAKESTATE_LONG_RANGE])
            {
                changeShakeState(SHAKESTATE_LONG_RANGE);
                break;
            };

            if (m_fNearestPlayerDist < s_aShakeRange[SHAKESTATE_MIDDLE_RANGE])
            {
                changeShakeState(SHAKESTATE_SHORT_RANGE);
                break;
            };

            shakeMoveCtrl();
        }
        break;
        
    case SHAKESTATE_SHORT_RANGE:
        {
            if (m_fNearestPlayerDist > s_aShakeRange[SHAKESTATE_MIDDLE_RANGE])
            {
                changeShakeState(SHAKESTATE_MIDDLE_RANGE);
                break;
            };

            shakeMoveCtrl();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CEggGimmick::shakeMoveCtrl(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();

    if (m_fTimer >= s_aShakeInterval[m_shakeState])
    {
        CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

        RwV3d vRotation = Math::VECTOR3_ZERO;
        pModel->GetRotation(&vRotation);

        float fRotX = vRotation.x;

        if (m_nShake == (s_aShakeNum[m_shakeState] - 1))
        {
            fRotX = -vRotation.x;
        }
        else
        {
            float fAngle = Math::RandFloatRange(s_aShakeMin[m_shakeState], s_aShakeMax[m_shakeState]);

            fRotX = MATH_DEG2RAD(fAngle);
            if (vRotation.x > 0.0f)
                fRotX *= -1.0f;
        };

        vRotation.x = Math::RadianCorrection(vRotation.x + fRotX);
        
        m_model.SetRotation(&vRotation);
        m_model.UpdateFrame();

        if (++m_nShake >= s_aShakeNum[m_shakeState])
        {
            m_fTimer = 0.0f;
            m_nShake = 0;
        };
    };
};


void CEggGimmick::changeShakeState(SHAKESTATE shakeState)
{
    m_shakeState    = shakeState;
    m_nShake        = 0;
    m_fTimer        = s_aShakeInterval[m_shakeState];
};


void CEggGimmick::setHitAttack(void)
{
    RwSphere hitSphere = { { 0.0f, 0.5f, 0.0f }, 0.7f };
    RwV3dTransformPoint(&hitSphere.center, &hitSphere.center, m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL));

    CHitCatchData hitCatch;
    hitCatch.Cleanup();
    hitCatch.SetObject(GetHandle());
    hitCatch.SetObjectType(GetType());
    hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
    hitCatch.SetSphere(&hitSphere);

    CHitAttackManager::RegistCatch(&hitCatch);
};


bool CEggGimmick::birth(void)
{
    if (m_fNearestPlayerDist <= 4.0f)
    {
        m_generator.Start();
        return true;
    };

    return false;
};


void CEggGimmick::birthForce(void)
{
    if (m_state == STATE_SHAKE)
    {
        m_generator.Start();        
        m_fTimer = 0.0f;
        m_state = STATE_GENERATE_ENEMY;
    };
};


bool CEggGimmick::enemyGenerate(void)
{
    m_generator.Run(CGameProperty::GetElapsedTime());

    if ((m_generator.CountAliveEnemy() <= 0) && (m_generator.CountDeadEnemy() <= 0))
        return false;
    
    m_generator.Finish();    
    return true;
};


bool CEggGimmick::eraseEgg(void)
{    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_model.GetPosition(&vPosition);

    CEffectManager::Play(EFFECTID::ID_CERAMICS, &vPosition);

    CGameSound::PlayObjectSE(this, SDCODE_SE(4341));

    return true;
};