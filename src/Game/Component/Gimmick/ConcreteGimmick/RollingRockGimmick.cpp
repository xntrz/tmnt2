#include "RollingRockGimmick.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Gimmick/MoveStrategy/RollingRockGimmickMove.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CRollingRockGimmick::CRollingRockGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_eState(STATE_NONE)
, m_pRollingMove(nullptr)
, m_model()
, m_pModuleMan(nullptr)
, m_nBoundSE(0)
{
    struct INITPARAM
    {
        const char* m_pszModelName;
        float m_fHitRadius;
        int32 m_nPower;        
    };

    static const INITPARAM s_aInitParam[] =
    {
        { "rolobj",  2.5f, 30 },
        { "rolobj2", 1.5f, 10 },
    };

    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    ASSERT(pBasicParam);
    ASSERT(pBasicParam->m_subid >= 0);
    ASSERT(pBasicParam->m_subid < COUNT_OF(s_aInitParam));

    const INITPARAM* pInitParam = &s_aInitParam[pBasicParam->m_subid];

    m_nPower = pInitParam->m_nPower;

    //
    //  init model
    //
    CModel* pTempModel = CModelManager::CreateModel(pInitParam->m_pszModelName);
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);

    //
    //  init movement
    //
    m_pRollingMove = new CRollingRockGimmickMove;
    m_pRollingMove->SetPosition(&pBasicParam->m_vPosition);
    m_pRollingMove->SetInitPosition(&pBasicParam->m_vPosition);
    m_pRollingMove->SetVelocity(&Math::VECTOR3_ZERO);
    m_pRollingMove->SetHitRadius(pInitParam->m_fHitRadius);

    //
    //  init pos & rot
    //
    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.SetRotation(&Math::VECTOR3_ZERO);
    m_model.UpdateFrame();

    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pRollingMove);

    //
    //  init module man
    //
    float fShadowWH = (pInitParam->m_fHitRadius * 2.0f);
    
    m_pModuleMan = new CModuleManager;
    m_pModuleMan->Include(CCircleShadowModule::New(this, fShadowWH, fShadowWH, true));
    
    //
    //  init sd code
    //
    STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();
    
    if ((idStage == STAGEID::ID_ST12N) ||
        (idStage == STAGEID::ID_ST13R) ||
        (idStage == STAGEID::ID_ST14N))
    {
        m_nBoundSE = SDCODE_SE(4350);
    }
	else
	{
		m_nBoundSE = (pBasicParam->m_subid != 0 ? SDCODE_SE(4183) : SDCODE_SE(4184));
	};
};


CRollingRockGimmick::~CRollingRockGimmick(void)
{
    if (m_pRollingMove)
    {
        delete m_pRollingMove;
        m_pRollingMove = nullptr;
    };

    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };
};


void CRollingRockGimmick::Draw(void) const
{
    CGimmick::Draw();

    if (m_pModuleMan)
        m_pModuleMan->Draw();
};


void CRollingRockGimmick::PostMove(void)
{
    if (m_model.GetCollisionModelClump())
    {
        float fRadius = m_pRollingMove->GetHitRadius();
        
        //
        //  update model pos & rot
        //
        RwV3d vPosition = Math::VECTOR3_ZERO;
        m_pRollingMove->GetPosition(&vPosition);

        RwV3d vRotation = Math::VECTOR3_ZERO;
        m_pRollingMove->GetRotation(&vRotation);

        m_model.SetPosition(&vPosition);
        m_model.SetRotation(&vRotation);

        //
        //  regist hit catch
        //
        {
            RwSphere hitSphere = { vPosition, fRadius };
            
            CHitCatchData hitCatch;
            hitCatch.Cleanup();
            hitCatch.SetObject(GetHandle());
            hitCatch.SetObjectType(GetType());
            hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
            hitCatch.SetSphere(&hitSphere);

            CHitAttackManager::RegistCatch(&hitCatch);
        }

        //
        //  regist hit attack
        //
        {
            RwSphere hitSphere = { vPosition, fRadius };

            CHitAttackData hitAttack;
            hitAttack.Cleanup();
#ifdef _DEBUG
            hitAttack.SetMotion("RollingRock");
#endif            
            hitAttack.SetObject(GetHandle());
            hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);
            hitAttack.SetSphere(&hitSphere);
            hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER);
            hitAttack.SetPower(m_nPower);
            hitAttack.SetObjectPos(&vPosition);
            hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
            hitAttack.SetStatus(CHitAttackData::STATUS_FLYAWAY);

            CHitAttackManager::RegistAttack(&hitAttack);
        }
        
        //
        //  check for reset
        //
        if (m_pRollingMove->IsReset())
        {
            switch (m_eState)
            {
            case STATE_MOVING:
                m_pRollingMove->Reset();
                break;

            case STATE_CAPTURED:
                Release();
                break;

            default:
                break;
            };
        };
    };

    if (m_pModuleMan)
        m_pModuleMan->Run();
};


void CRollingRockGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    switch (m_eState)
    {
    case STATE_NONE:
        {
            m_pRollingMove->Start();
            m_eState = STATE_MOVING;

            CGameSound::PlayObjectSE(this, SDCODE_SE(4352));
        }
        break;

    case STATE_MOVING:
        {
            m_eState = STATE_CAPTURED;
            CGameSound::FadeOutSE(SDCODE_SE(4352), CGameSound::FADESPEED_FAST);
        }
        break;

    case STATE_CAPTURED:
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CRollingRockGimmick::OnTouchedDown(void)
{
    CGameSound::PlayObjectSE(this, SDCODE_SE(m_nBoundSE));
};