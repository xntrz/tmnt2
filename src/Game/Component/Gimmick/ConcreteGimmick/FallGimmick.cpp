#include "FallGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
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
, m_pMove(nullptr)
, m_pModuleManager(nullptr)
, m_target(CHitAttackData::TARGET_NONE)
, m_eSubid(SUBID_NUM)
, m_eState(STATE_NUM)
, m_fTimer(0.0f)
, m_fInterval(0.0f)
, m_vPreMovePosition(Math::VECTOR3_ZERO)
, m_nPower(0)
, m_fFriction(0.0f)
, m_bStart(false)
, m_bAttack(false)
, m_bTouched(false)
{
    static_assert(COUNT_OF(m_aSubInfo) == SUBID_NUM, "update me");

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    ASSERT(pInitParam);
    
    m_eSubid = SUBID(pInitParam->m_subid);
    
    m_model.SetModel(
        CNormalGimmickModel::MODELKIND_VISUAL_NORMAL,
        CModelManager::CreateModel(m_aSubInfo[m_eSubid].m_pszModelName)
    );
    RwV3d vRotation = { 0.0f, CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat), 0.0f };
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.SetRotation(&vRotation);
    SetModelStrategy(&m_model);
    
    m_pMove = new CFallGimmickMove(
        m_aSubInfo[m_eSubid].m_eMovetype,
        m_aSubInfo[m_eSubid].m_fReflection,
        m_aSubInfo[m_eSubid].m_fCollisionRadius
    );
    ASSERT(m_pMove);
    m_pMove->SetPosition(&pInitParam->m_vPosition);
    m_pMove->SetRotation(&vRotation);
    m_pMove->SetCollisionRadiusAutoChangeEnable(true);
    SetMoveStrategy(m_pMove);

    if (m_eSubid && (m_eSubid != SUBID_STALACTITE))
    {
        RwV3d vRotationVelocity =
        {
            Math::RandFloatRange(-1.0f, 1.0f) * Math::PI,
            Math::RandFloatRange(-1.0f, 1.0f) * Math::PI,
            Math::RandFloatRange(-1.0f, 1.0f) * Math::PI,
        };

        m_pMove->SetRotationVelocity(&vRotationVelocity);
    };

    m_pModuleManager = new CModuleManager;
    ASSERT(m_pModuleManager);
    m_pModuleManager->Include(CCircleShadowModule::New(this, 1.5f, 1.5f, true));

    m_eState = STATE_WAIT;
    m_fTimer = 0.0f;
    m_fInterval = 0.6f;
    m_vPreMovePosition = Math::VECTOR3_ZERO;
    m_nPower = m_aSubInfo[m_eSubid].m_nPower;
    m_fFriction = m_aSubInfo[m_eSubid].m_fFriction;
    m_bStart = false;
    m_bAttack = false;
    m_bTouched = false;
    m_target = (CHitAttackData::TARGET_ENEMY | CHitAttackData::TARGET_PLAYER);
};


CFallGimmick::~CFallGimmick(void)
{
    if (m_pModuleManager)
    {
        delete m_pModuleManager;
        m_pModuleManager = nullptr;
    };

    if (m_pMove)
    {
        delete m_pMove;
        m_pMove = nullptr;
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
    if (m_eState && (m_eState != STATE_END))
    {
        CGimmick::Draw();
        m_pModuleManager->Draw();
    };
};


void CFallGimmick::PreMove(void)
{
    m_pMove->GetPosition(&m_vPreMovePosition);
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
        m_pMove->GetPosition(&vPosition);
        m_pMove->SetRotationVelocity(&Math::VECTOR3_ZERO);

        CEffectManager::Play(m_aSubInfo[m_eSubid].m_eFragmentId, &vPosition, false);
    };
};


void CFallGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    if (!m_bTouched)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pMove->GetPosition(&vPosition);
        
        CEffectManager::Play(m_aSubInfo[m_eSubid].m_eFragmentId, &vPosition, true);
    };
};


void CFallGimmick::SetBodyHitRadiusAutoChangeEnable(bool bEnable)
{
    ASSERT(m_pMove);
    m_pMove->SetCollisionRadiusAutoChangeEnable(bEnable);
};


void CFallGimmick::move(void)
{
    if (m_pMove)
    {
        CGimmickMove::MOVESTATE eResult = m_pMove->Move(CGameProperty::GetElapsedTime());        
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
        };

        RwV3d vRotation = Math::VECTOR3_ZERO;
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pMove->GetPosition(&vPosition);
        m_pMove->GetRotation(&vRotation);
        
        m_model.SetPosition(&vPosition);
        m_model.SetRotation(&vRotation);
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
                m_pMove->Start();
                m_bAttack = true;
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
    m_pMove->GetVelocity(&vVelocity);

    float fSpeed = Math::Vec3_Length(&vVelocity);
    if (fSpeed > 0.0f)
    {
        Math::Vec3_Scale(&vVelocity, &vVelocity, m_fFriction);
        m_pMove->SetVelocity(&vVelocity);

        fSpeed = Math::Vec3_Length(&vVelocity);
        if (fSpeed < Math::EPSILON)
            m_pMove->SetVelocity(&Math::VECTOR3_ZERO);
    };
};


void CFallGimmick::setHitAttack(void)
{
    if (!m_bAttack)
        return;
    
    RwSphere AttackSphere = { 0 };
    AttackSphere.radius = m_aSubInfo[m_eSubid].m_fAttackRadius;
    m_pMove->GetPosition(&AttackSphere.center);

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
    
    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL);
    RwRGBA MaterialColor = pModel->GetColor();
    
    int32 Alpha = int32(float(MaterialColor.alpha) - fStep);
    Alpha = Math::Clamp(Alpha, 0, 255);
    MaterialColor.alpha = uint8(Alpha);;
    
    pModel->SetColor(MaterialColor);
    
    return (MaterialColor.alpha == 0);
};


void CFallGimmick::breakSign(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pMove->GetPosition(&vPosition);

    vPosition.y = CWorldMap::GetMapHeight(&vPosition) + 10.0f;

    CEffectManager::Play(EFFECTID::ID_BREAKSIGN, &vPosition);

    CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(0x104C), 0);
};


bool CFallGimmick::heightCheck(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pMove->GetPosition(&vPosition);

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