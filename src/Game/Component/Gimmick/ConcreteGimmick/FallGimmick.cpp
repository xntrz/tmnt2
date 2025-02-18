#include "FallGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/MoveStrategy/FallGimmickMove.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "System/Common/Screen.hpp"


/*static*/ const CFallGimmick::SUBINFO CFallGimmick::m_aSubInfo[] =
{
    { "turara",     0.5f,   0.0f,   0.5f,   0.01f,  10, MOVETYPE_STOP,  EFFECTID::ID_BREAKICE   },
    { "turara",     0.5f,   0.0f,   0.5f,   0.01f,  10, MOVETYPE_STOP,  EFFECTID::ID_BREAKICE   },
    { "rakuban",    0.35f,  0.15f,  1.1f,   0.2f,   20, MOVETYPE_BOUND, EFFECTID::ID_ROCK       },
    { "scrap",      0.5f,   0.15f,  1.2f,   0.2f,   30, MOVETYPE_BOUND, EFFECTID::ID_METAL      },
};


CFallGimmick::CFallGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_pFallMove(nullptr)
, m_pModuleManager(nullptr)
, m_target(CHitAttackData::TARGET_NONE)
, m_eSubid(SUBID_NUM)
, m_eState(STATE_NUM)
, m_fTimer(0.0f)
, m_fInterval(0.6f)
, m_vPreMovePosition(Math::VECTOR3_ZERO)
, m_nPower(0)
, m_fFriction(0.0f)
, m_bStart(false)
, m_bAttack(false)
, m_bTouched(false)
{
    static_assert(COUNT_OF(m_aSubInfo) == SUBID_NUM, "update me");

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    m_eSubid = SUBID(pInitParam->m_subid);
    
    ASSERT(m_eSubid >= 0);
    ASSERT(m_eSubid < COUNT_OF(m_aSubInfo));

    const SUBINFO* pSubInfo = &m_aSubInfo[m_eSubid];

    m_eState    = STATE_WAIT;
    m_nPower    = pSubInfo->m_nPower;
    m_fFriction = pSubInfo->m_fFriction;
    m_target    = (CHitAttackData::TARGET_ENEMY | CHitAttackData::TARGET_PLAYER);

    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel(pSubInfo->m_pszModelName);
    ASSERT(pModel);
    
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    
    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);

    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();
    
    //
    //  init movement
    //    
    m_pFallMove = new CFallGimmickMove(pSubInfo->m_eMovetype, pSubInfo->m_fReflection, pSubInfo->m_fCollisionRadius);
    m_pFallMove->SetPosition(&pInitParam->m_vPosition);
    m_pFallMove->SetRotation(&vRotation);
    m_pFallMove->SetCollisionRadiusAutoChangeEnable(true);

    if (m_eSubid && (m_eSubid != SUBID_STALACTITE))
    {
        RwV3d vRotationVelocity =
        {
            Math::RandFloatRange(-1.0f, 1.0f) * Math::PI,
            Math::RandFloatRange(-1.0f, 1.0f) * Math::PI,
            Math::RandFloatRange(-1.0f, 1.0f) * Math::PI,
        };

        m_pFallMove->SetRotationVelocity(&vRotationVelocity);
    };

    //
    //  init module man
    //
    m_pModuleManager = new CModuleManager;
    m_pModuleManager->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, false));
    
    //
    //  setup model & movement strat
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pFallMove);
};


CFallGimmick::~CFallGimmick(void)
{
    if (m_pModuleManager)
    {
        delete m_pModuleManager;
        m_pModuleManager = nullptr;
    };

    if (m_pFallMove)
    {
        delete m_pFallMove;
        m_pFallMove = nullptr;
    };
};


void CFallGimmick::Run(void)
{
    PreMove();
    
    fallGimmickCtrl();
    move();
    
    PostMove();
};


void CFallGimmick::Draw(void) const
{
    if ((m_eState > STATE_WAIT) && (m_eState != STATE_END))
        CGimmick::Draw();
};


void CFallGimmick::PreMove(void)
{
    m_pFallMove->GetPosition(&m_vPreMovePosition);
};


void CFallGimmick::PostMove(void)
{
    m_pModuleManager->Run();
};


void CFallGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (m_eState == STATE_WAIT)
    {
        if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
            m_bStart = true;
    };
};


void CFallGimmick::OnTouchedDown(void)
{
    if (m_bAttack)
        m_bAttack = false;

    if (!m_bTouched)
    {
        m_bTouched = true;
        
        seCtrl();

        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pFallMove->GetPosition(&vPosition);
        m_pFallMove->SetRotationVelocity(&Math::VECTOR3_ZERO);

        CEffectManager::Play(m_aSubInfo[m_eSubid].m_eFragmentId, &vPosition, false);
    };
};


void CFallGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    if (!m_bTouched)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pFallMove->GetPosition(&vPosition);
        
        CEffectManager::Play(m_aSubInfo[m_eSubid].m_eFragmentId, &vPosition, true);
    };
};


void CFallGimmick::SetBodyHitRadiusAutoChangeEnable(bool bEnable)
{
    ASSERT(m_pFallMove);
    m_pFallMove->SetCollisionRadiusAutoChangeEnable(bEnable);
};


void CFallGimmick::move(void)
{
    if (m_pFallMove)
    {
        CGimmickMove::MOVESTATE eResult = m_pFallMove->Move(CGameProperty::GetElapsedTime());        
        switch (eResult)
        {
        case CGimmickMove::MOVESTATE_ON_GROUND:
            {
                if (!m_bTouched)
                    OnTouchedDown();

                onGround();
            }
            break;

        case CGimmickMove::MOVESTATE_TOUCHDOWN:
            {
                if (m_aSubInfo[m_eSubid].m_eMovetype == MOVETYPE_BOUND)
                {
                    OnTouchedDown();
                }
                else
                {
                    OnTouchedDown();
                    onGround();
                };
            }
            break;

        default:
            break;
        };

        RwV3d vRotation = Math::VECTOR3_ZERO;
        m_pFallMove->GetRotation(&vRotation);
        m_model.SetRotation(&vRotation);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pFallMove->GetPosition(&vPosition);
        m_model.SetPosition(&vPosition);
    };
};


void CFallGimmick::fallGimmickCtrl(void)
{
    switch (m_eState)
    {
    case STATE_WAIT:
        {
            if (m_bStart)
                changeStatus(STATE_SIGN);
        }
        break;

    case STATE_SIGN:
        {
			breakSign();
            changeStatus(STATE_WAIT_TIMER);
        }
        break;

    case STATE_WAIT_TIMER:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > m_fInterval)
            {
                changeStatus(STATE_FALL);
                m_pFallMove->Start();
                m_bAttack = true;
                
                CCircleShadowModule* pModule = static_cast<CCircleShadowModule*>(m_pModuleManager->GetModule(MODULETYPE::CIRCLE_SHADOW));
                ASSERT(pModule);
                
                pModule->SetEnable(true);
            };
        }
        break;

    case STATE_FALL:
        {
            if (m_bTouched || heightCheck())
                changeStatus(STATE_FALL_END);
            else
                setHitAttack();
        }
        break;

    case STATE_FALL_END:
        {
            if (changeMaterialAlpha())
                changeStatus(STATE_END);
        }
        break;

    case STATE_END:
        {
            Release();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CFallGimmick::changeStatus(STATE eState)
{
    m_eState = eState;
    m_fTimer = 0.0f;
};


void CFallGimmick::onGround(void)
{
    RwV3d vVelocity = Math::VECTOR3_ZERO;
    m_pFallMove->GetVelocity(&vVelocity);

    float fSpeed = Math::Vec3_Length(&vVelocity);
    if (fSpeed > 0.0f)
    {
        Math::Vec3_Scale(&vVelocity, &vVelocity, m_fFriction);
        m_pFallMove->SetVelocity(&vVelocity);

        fSpeed = Math::Vec3_Length(&vVelocity);
        if (fSpeed < Math::EPSILON)
            m_pFallMove->SetVelocity(&Math::VECTOR3_ZERO);
    };
};


void CFallGimmick::setHitAttack(void)
{
    if (!m_bAttack)
        return;
    
    RwSphere AttackSphere = { 0 };
    AttackSphere.radius = m_aSubInfo[m_eSubid].m_fAttackRadius;
    m_pFallMove->GetPosition(&AttackSphere.center);

    CHitAttackData Attack;
    Attack.SetObject(GetHandle());
    Attack.SetObjectPos(&m_vPreMovePosition);
    Attack.SetShape(CHitAttackData::SHAPE_SPHERE);
    Attack.SetSphere(&AttackSphere);
    Attack.SetPower(m_nPower);
    Attack.SetTarget(CHitAttackData::TARGET(m_target));
    Attack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
    Attack.SetStatus(CHitAttackData::STATUS_KNOCK);
#ifdef _DEBUG    
    Attack.SetMotion("FallGimmick");
#endif    

    CHitAttackManager::RegistAttack(&Attack);    
};


bool CFallGimmick::changeMaterialAlpha(void)
{
    float fStep = (255.0f / CScreen::Framerate());
    
    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RwRGBA MaterialColor = pModel->GetColor();
    
    int32 Alpha = int32(float(MaterialColor.alpha) - fStep);
    Alpha = Clamp(Alpha, 0, 255);
    MaterialColor.alpha = uint8(Alpha);;
    
    pModel->SetColor(MaterialColor);
    
    return (MaterialColor.alpha == 0);
};


void CFallGimmick::breakSign(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pFallMove->GetPosition(&vPosition);

    vPosition.y = CWorldMap::GetMapHeight(&vPosition) + 7.0f;

    CEffectManager::Play(EFFECTID::ID_BREAKSIGN, &vPosition);

    CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(0x104C));
};


bool CFallGimmick::heightCheck(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pFallMove->GetPosition(&vPosition);

    return (vPosition.y < -200.0f);
};


void CFallGimmick::seCtrl(void)
{
    switch (m_eSubid)
    {
    case SUBID_TSURARA:
    case SUBID_STALACTITE:
        CGameSound::PlayObjectSE(this, SDCODE_SE(0x10FC));
        break;

    case SUBID_CAVEIN:
        CGameSound::PlayObjectSE(this, SDCODE_SE(0x104B));
        break;

    case SUBID_SCRAP:
        CGameSound::PlayObjectSE(this, SDCODE_SE(0x104A));
        break;

    default:
        ASSERT(false);
        break;
    };
};