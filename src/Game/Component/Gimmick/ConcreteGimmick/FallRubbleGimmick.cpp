#include "FallRubbleGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/FallRubbleGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/Screen.hpp"


/*static*/ const CFallRubbleGimmick::INITPARAM CFallRubbleGimmick::m_aInitParam[]
{
    { "scrap", 0.15f },
    { "scrap", 0.15f },
    { "scrap", 0.15f },
};


CFallRubbleGimmick::CFallRubbleGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pFallRubbleMove(nullptr)
, m_pModuleMan(nullptr)
, m_nDamage(0)
, m_vPreMovePosition(Math::VECTOR3_ZERO)
, m_eState(STATE_FALL)
, m_eKind(KIND_UNKNOWN)
, m_fAliveTime(0.0f)
, m_bErase(false)
{
    static_assert(COUNT_OF(m_aInitParam) == KINDNUM, "should equal");
    
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    ASSERT(pBasicParam);
    ASSERT(pBasicParam->m_subid >= 0);
    ASSERT(pBasicParam->m_subid < KINDNUM);
    
    m_eKind         = KIND(pBasicParam->m_subid);
    m_nDamage       = 5;
    m_eState        = STATE_FALL;
    m_bErase        = false;
    m_fAliveTime    = 3.0f;

    //
    //  init model
    //    
    CModel* pTempModel = CModelManager::CreateModel(m_aInitParam[m_eKind].m_pszName);
    ASSERT(pTempModel);

    m_pModel = new CNormalGimmickModel;
    m_pModel->SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);

    //
    //  init move
    //
    const float fRefCoe = m_aInitParam[m_eKind].m_fRefrectionCoefficient;
    const float fHitRadius = 1.0f;
        
    m_pFallRubbleMove = new CFallRubbleGimmickMove(fRefCoe, fHitRadius);
    m_pFallRubbleMove->Start();

    //
    //  init module man
    //
    m_pModuleMan = new CModuleManager;
    m_pModuleMan->Include(CCircleShadowModule::New(this, 1.0f, 1.0f, true));

    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    
    RwMatrix matRot;
    RwMatrixSetIdentityMacro(&matRot);
    
    CGimmickUtils::QuaternionToRotationMatrix(&matRot, &pBasicParam->m_quat);
    CGimmickUtils::MatrixToRotation(&matRot, &vRotation);

    m_pFallRubbleMove->SetPosition(&pBasicParam->m_vPosition);
    m_pFallRubbleMove->SetRotation(&vRotation);
    
    m_pModel->SetPosition(&pBasicParam->m_vPosition);
    m_pModel->SetRotation(&vRotation);

    //
    //  init fall velocity
    //
    m_pFallRubbleMove->SetVelocity(&Math::VECTOR3_ZERO);
    
    RwV3d vVelocity = Math::VECTOR3_ZERO;
    vVelocity.x = Math::RandFloatRange(-1.0f, 1.0f) * (Math::PI / 3.0f);
    vVelocity.y = Math::RandFloatRange(-1.0f, 1.0f) * (Math::PI / 3.0f);
    vVelocity.z = Math::RandFloatRange(-1.0f, 1.0f) * (Math::PI / 3.0f);
    
    m_pFallRubbleMove->SetVelocity(&vVelocity);
    
    //
    //  setup model & move strategy
    //
    SetModelStrategy(m_pModel);
    SetMoveStrategy(m_pFallRubbleMove);
};


CFallRubbleGimmick::~CFallRubbleGimmick(void)
{
    if (m_pModel)
    {
        delete m_pModel;
        m_pModel = nullptr;
    };

    if (m_pFallRubbleMove)
    {
        delete m_pFallRubbleMove;
        m_pFallRubbleMove = nullptr;
    };

    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };
};


void CFallRubbleGimmick::Run(void)
{
    PreMove();
    
    if (m_pMoveStrategy)
    {
        CGimmickMove::RESULT Result = m_pMoveStrategy->OnMove(CGameProperty::GetElapsedTime());
        if ((Result == CGimmickMove::MOVESTATE_TOUCHDOWN) ||
            (Result == CGimmickMove::MOVESTATE_ON_GROUND))
        {
            OnTouchedDown();
            
            if (Result == CGimmickMove::MOVESTATE_ON_GROUND)
                onOnGround();
        };

        if (m_pModelStrategy)
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_pMoveStrategy->GetPosition(&vPosition);

            m_pModelStrategy->SetPosition(&vPosition);
        };
    };

    PostMove();
};


void CFallRubbleGimmick::Draw(void) const
{
    CGimmick::Draw();

    if (m_pModuleMan)
        m_pModuleMan->Draw();
};


void CFallRubbleGimmick::PreMove(void)
{
    m_pFallRubbleMove->GetPosition(&m_vPreMovePosition);

    if (m_pModuleMan)
        m_pModuleMan->Run();
};


void CFallRubbleGimmick::PostMove(void)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pFallRubbleMove->GetRotation(&vRotation);
    
    m_pModel->SetRotation(&vRotation);

    switch (m_eState)
    {
    case STATE_FALL:
        {
            m_fAliveTime -= CGameProperty::GetElapsedTime();

            if (m_bErase || (m_fAliveTime < 0.0f))
                m_eState = STATE_ERASE;
            else
                setHitAttack();
        }
        break;
        
    case STATE_ERASE:
        {
            if (decreaseMaterialAlpha())
                Release();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CFallRubbleGimmick::OnTouchedDown(void)
{
    if (m_bErase)
        return;
    
    const EFFECTID::VALUE aIdEffect[] =
    {
        EFFECTID::ID_METAL,
        EFFECTID::ID_METAL,
        EFFECTID::ID_ROCK
    };

    static_assert(COUNT_OF(aIdEffect) == KINDNUM, "should equal");

    bool bSoundCall = true;
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pFallRubbleMove->GetPosition(&vPosition);

    uint32 hEffect = CEffectManager::Play(aIdEffect[m_eKind], &vPosition, bSoundCall);
    ASSERT(hEffect);

    m_pFallRubbleMove->SetRotVelocity(&Math::VECTOR3_ZERO);
};


bool CFallRubbleGimmick::decreaseMaterialAlpha(void)
{
    uint8 decValue = uint8(255.0f / CScreen::Framerate());

    CModel* pMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK);
    ASSERT(pMdl);

    RwRGBA MaterialColor = pMdl->GetColor();
    MaterialColor.alpha -= decValue;
    pMdl->SetColor(MaterialColor);

    return (MaterialColor.alpha < decValue);
};


void CFallRubbleGimmick::setHitAttack(void)
{
    RwSphere HitSphere = { Math::VECTOR3_ZERO, 1.5f };
    m_pFallRubbleMove->GetPosition(&HitSphere.center);

    CHitAttackData AttackData;
    AttackData.Cleanup();
#ifdef _DEBUG
    AttackData.SetMotion("FallRubble");
#endif
    AttackData.SetObject(GetHandle());
    AttackData.SetObjectPos(&m_vPreMovePosition);
    AttackData.SetPower(m_nDamage);
    AttackData.SetTarget(CHitAttackData::TARGET_PLAYER);
    AttackData.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
    AttackData.SetStatus(CHitAttackData::STATUS_KNOCK);
    AttackData.SetShape(CHitAttackData::SHAPE_SPHERE);
    AttackData.SetSphere(&HitSphere);

    CHitAttackManager::RegistAttack(&AttackData);
};


void CFallRubbleGimmick::onOnGround(void)
{
    RwV3d vVelocity = Math::VECTOR3_ZERO;
    m_pFallRubbleMove->GetVelocity(&vVelocity);
    
    float fSpeed = Math::Vec3_Length(&vVelocity);

    if (!Math::FEqual(fSpeed, 0.0f))
    {
        Math::Vec3_Scale(&vVelocity, &vVelocity, 0.2f);
        m_pFallRubbleMove->SetVelocity(&vVelocity);

        fSpeed = Math::Vec3_Length(&vVelocity);
        if (fSpeed < 0.01f)
            m_pFallRubbleMove->SetVelocity(&Math::VECTOR3_ZERO);
    };
};