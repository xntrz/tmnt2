#include "Gimmick.hpp"
#include "GimmickMove.hpp"
#include "GimmickModel.hpp"
#include "GimmickQuery.hpp"
#include "GimmickManager.hpp"
#include "GimmickParam.hpp"
#include "GimmickDebug.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "System/Common/RenderState.hpp"


CGimmick::CGimmick(const char* pszName, void* pParam)
: CGameObject(pszName, GAMEOBJECTTYPE::GIMMICK)
, m_id(GIMMICKID::ID_INVALID)
, m_pModelStrategy(nullptr)
, m_pMoveStrategy(nullptr)
{
    ASSERT(pParam);
    
#ifdef _DEBUG
    GIMMICKPARAM::GIMMICK_BASIC* pBasic = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    m_vDebugInitialPos = pBasic->m_vPosition;
#endif    
};


CGimmick::~CGimmick(void)
{
    ;
};


void CGimmick::Run(void)
{
    PreMove();

    if (m_pMoveStrategy)
    {
        if (m_pMoveStrategy->Move(CGameProperty::GetElapsedTime()) == CGimmickMove::MOVESTATE_TOUCHDOWN)
        {
            OnTouchedDown();
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


void CGimmick::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT:
        OnAttackResult((CHitCatchData*)pParam);
        break;

    case GAMEOBJECTTYPES::MESSAGEID_CATCHATTACK:
        OnCatchAttack((CHitAttackData*)pParam);
        break;

    case GIMMICKTYPES::MESSAGEID_RECVEVENT:
        {
            GIMMICKTYPES::EVENT_ARGS* pEventArgs = (GIMMICKTYPES::EVENT_ARGS*)pParam;
            OnReceiveEvent(pEventArgs->m_szSender, pEventArgs->m_type);
        }
        break;

    default:
        CGameObject::MessageProc(nMessageID, pParam);
        break;
    };
};


void CGimmick::Draw(void) const
{
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, nullptr);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSU, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSV, rwTEXTUREADDRESSCLAMP);

    if (m_pModelStrategy)
    {
        m_pModelStrategy->Draw();
    };

    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSV);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSU);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);

#ifdef _DEBUG
    if (CGimmickDebug::SHOW_ME)
        CDebugShape::ShowLabel(&m_vDebugInitialPos, GetName());
#endif    
};


void CGimmick::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);
    
    if (m_pMoveStrategy)
    {
        m_pMoveStrategy->GetPosition(pvPosition);
    }
    else
    {
        *pvPosition = Math::VECTOR3_ZERO;
    };
};


void CGimmick::GetCenterPosition(RwV3d* pvPosition) const
{
    GetPosition(pvPosition);
};


GIMMICKTYPES::FEATURE CGimmick::GetFeatures(void) const
{
    return GIMMICKTYPES::FEATURE_NONE;
};


bool CGimmick::Query(CGimmickQuery* pQuery) const
{
    if (pQuery)
    {
        pQuery->Default(this);        
    };

    return false;
};


void CGimmick::PreMove(void)
{
    ;
};


void CGimmick::PostMove(void)
{
    ;
};


void CGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    ;
};


void CGimmick::OnTouchedDown(void)
{
    ;
};


void CGimmick::OnDamaged(void)
{
    ;
};


void CGimmick::OnDie(void)
{
    ;
};


void CGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);
};


void CGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    ASSERT(pCatch);
};


bool CGimmick::IsSleep(void) const
{
    return (FLAG_TEST(GetObjectFlag(), GAMEOBJECTTYPES::FLAG_SLEEP));
};


void CGimmick::Init(GIMMICKID::VALUE idGimmick)
{
    m_id = idGimmick;
};


GIMMICKID::VALUE CGimmick::GetID(void) const
{
    return m_id;
};


void CGimmick::Release(void)
{
    CGimmickManager::Release(this);
};


void CGimmick::SetModelStrategy(CGimmickModel* pModel)
{
    m_pModelStrategy = pModel;
};


void CGimmick::SetMoveStrategy(CGimmickMove* pMove)
{
    m_pMoveStrategy = pMove;
};