#include "RideGimmick.hpp"

#include "Game/Component/Gimmick/MoveStrategy/RideGimmickMove.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Sequence/Ride/RideStage.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"


CRideGimmick::CRideGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pRideMove(nullptr)
, m_pModuleManager(nullptr)
, m_bCatchHit(false)
, m_iPower(0)
, m_fRadius(0.5f)
, m_fOffset(0.5f)
{
    ;
};


CRideGimmick::~CRideGimmick(void)
{
    if (m_pModuleManager)
    {
        delete m_pModuleManager;
        m_pModuleManager = nullptr;
    };

    if (m_pRideMove)
    {
        delete m_pRideMove;
        m_pRideMove = nullptr;
    };
};


void CRideGimmick::Draw(void) const
{
    if (IsProcess(PROCESSTYPE_DRAW_MODEL))
        CGimmick::Draw();

    if (m_pModuleManager)
    {
        if (IsProcess(PROCESSTYPE_DRAW_SHADOW))
            m_pModuleManager->Draw();
    };
};


void CRideGimmick::PostMove(void)
{
    ASSERT(m_pRideMove);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pRideMove->GetRotation(&vRotation);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pRideMove->GetPosition(&vPosition);
    
    m_model.SetPosition(&vPosition);
    m_model.SetRotation(&vRotation);

    if (IsProcess(PROCESSTYPE_ATTACK))
    {
        RwSphere hitSphere = { 0 };
        hitSphere.center = { vPosition.x, vPosition.y + m_fOffset, vPosition.z };
        hitSphere.radius = m_fRadius;

        CHitAttackData hitAttack;
        hitAttack.SetObject(GetHandle());
        hitAttack.SetObjectPos(&vPosition);
        hitAttack.SetShape(CHitAttackData::SHAPE_SPHERE);
        hitAttack.SetSphere(&hitSphere);
        hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER);
        hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        hitAttack.SetPower(m_iPower);

        CHitAttackManager::RegistAttack(&hitAttack);
    };

    if (m_bCatchHit && IsProcess(PROCESSTYPE_DAMAGE))
    {
        RwSphere hitSphere = { vPosition, m_fRadius };
        hitSphere.center = { vPosition.x, vPosition.y + m_fOffset, vPosition.z };
        hitSphere.radius = m_fRadius;

        CHitCatchData hitCatch;
        hitCatch.SetObject(GetHandle());
        hitCatch.SetObjectType(GetType());
        hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
        hitCatch.SetSphere(&hitSphere);

        CHitAttackManager::RegistCatch(&hitCatch);
    };

    if (m_pModuleManager && IsProcess(PROCESSTYPE_DRAW_SHADOW))
    {
        CCircleShadowModule* pCircleShadowMod = static_cast<CCircleShadowModule*>(m_pModuleManager->GetModule(MODULETYPE::CIRCLE_SHADOW));
        if (pCircleShadowMod)
            pCircleShadowMod->SetEnable(true);

        m_pModuleManager->Run();
    };
};


bool CRideGimmick::IsProcess(PROCESSTYPE processtype) const
{
    static const float s_afCheckDistance[PROCESSTYPENUM] =
    {
        50.0f,
        20.0f,
        200.0f,
        200.0f,
    };
 
    RwV3d vBasisPoint = Math::VECTOR3_ZERO;
    CRideStage::GetBasisPoint(&vBasisPoint);

    RwV3d vPosition = Math::VECTOR3_ZERO;
	if (m_pRideMove)
		m_pRideMove->GetPosition(&vPosition);
	else
		m_model.GetPosition(&vPosition);

    float fDist = vPosition.z - vBasisPoint.z;
    if ((fDist < s_afCheckDistance[processtype]) && (fDist > -15.0f))
        return true;
    
    return false;
};