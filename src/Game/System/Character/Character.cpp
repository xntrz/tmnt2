#include "Character.hpp"
#include "CharacterManager.hpp"
#include "CharacterAttackCalculator.hpp"

#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/AtomicDisplayControlModule.hpp"
#include "Game/Component/Module/AmbientLightModule.hpp"
#include "Game/Component/Module/LocusModule.hpp"
#include "Game/Component/Module/InkThicknessCtrlModule.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Motion/MotionController.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/MotionParameter/MotionParameterController.hpp"
#include "Game/System/MotionParameter/MotionParameterManager.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/RenderStateStack.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "System/Common/RenderState.hpp"


CCharacter::CCharacter(const char* pszName, TYPE type)
: CGameObject(pszName, GAMEOBJECTTYPE::CHARACTER)
, m_pModuleMan(nullptr)
, m_collision({ 0.0f, 0.0f })
, m_collisionWallInfo({ 0 })
, m_collisionGroundInfo({ 0 })
, m_vPosition({ 0.0f, 0.0f, 0.0f })
, m_vVelocity(Math::VECTOR3_ZERO)
, m_vAcceleration(Math::VECTOR3_ZERO)
, m_fDirection(0.0f)
, m_fMaxVelocity(0.0f)
, m_fMotionSpeed(0.0f)
, m_chrtype(type)
, m_cflag(CHARACTERTYPES::FLAG_NONE)
, m_pModel(nullptr)
, m_pMotionController(nullptr)
, m_apBodyHitData(nullptr)
, m_nNumBodyHitData(0)
, m_pMotionParameterController(nullptr)
, m_liftinfo({ 0 })
, m_iDamageRequest(0)
{
    CCharacterManager::Regist(this);
};


CCharacter::~CCharacter(void)
{
    CCharacterManager::Remove(this);

    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };

    if (m_pMotionParameterController)
    {
        delete m_pMotionParameterController;
        m_pMotionParameterController = nullptr;        
    };

    if (m_pMotionController)
    {
        delete m_pMotionController;
        m_pMotionController = nullptr;
    };

    if (m_pModel)
    {
        CModelManager::DestroyModel(m_pModel);
        m_pModel = nullptr;
    };

    if (m_apBodyHitData)
    {
        ASSERT(m_nNumBodyHitData > 0);
        for (int32 i = 0; i < m_nNumBodyHitData; ++i)
        {
            CBodyHitData* pBodyHitData = m_apBodyHitData[i];
            m_apBodyHitData[i] = nullptr;

            ASSERT(pBodyHitData);
            CBodyHitManager::FreeData(pBodyHitData);
        };

        delete[] m_apBodyHitData;
        m_apBodyHitData = nullptr;
    };
};


void CCharacter::Run(void)
{
    UpdateParameter();

    RwV3d vFrameVelocity = Math::VECTOR3_ZERO;
    CalcVelocityPerFrame(vFrameVelocity);

    bool bPeriod = true;
    CheckCollisionForBody(vFrameVelocity, bPeriod);
    CheckCollisionForWall(vFrameVelocity, bPeriod);
    CheckCollisionForGround();

    UpdateModel();

    UpdateCollisionParameterForBody();
    UpdateCollisionParameterForCatch();
    UpdateCollisionParameterForAttack();

    CheckTimingData();
    
    m_pModuleMan->Run();
};


void CCharacter::MessageProc(int32 nMessageID, void* pParam)
{
    switch (nMessageID)
    {
    case GAMEOBJECTTYPES::MESSAGEID_SLEEP:
        SetEnableBodyHit(false);
        SetEnableCatchHit(false);
        CGameObject::MessageProc(nMessageID, pParam);
        break;

    case GAMEOBJECTTYPES::MESSAGEID_AWAKE:
        SetEnableBodyHit(true);
        SetEnableCatchHit(true);
        CGameObject::MessageProc(nMessageID, pParam);
        break;

    case GAMEOBJECTTYPES::MESSAGEID_CATCHATTACK:
        OnMessageCatchAttack(static_cast<CHitAttackData*>(pParam));
        break;

    case GAMEOBJECTTYPES::MESSAGEID_ATTACKRESULT:
        OnMessageAttackResult(static_cast<CHitCatchData*>(pParam));
        break;

    case CHARACTERTYPES::MESSAGEID_AERIAL:
        OnMessageAerial(*static_cast<float*>(pParam));
        break;
        
    case CHARACTERTYPES::MESSAGEID_TOUCHDOWN:
        OnMessageTouchdown(*static_cast<float*>(pParam));
        break;
        
    case CHARACTERTYPES::MESSAGEID_RECVDMG:
        OnMessageReceivedDamage(reinterpret_cast<int32>(pParam));
        break;
        
    case CHARACTERTYPES::MESSAGEID_DEATHFLOOR:
        OnSteppedDeathFloor();
        break;
        
    case CHARACTERTYPES::MESSAGEID_CATCH:
        OnMessageCatch(pParam);
        break;
        
    case CHARACTERTYPES::MESSAGEID_LIFT:
        OnMessageLift(pParam);
        break;
        
    case CHARACTERTYPES::MESSAGEID_THROW:
        OnMessageThrow(pParam);
        break;
        
    case CHARACTERTYPES::MESSAGEID_MISSTHROW:
        OnMessageMissThrow(pParam);
        break;
        
    case CHARACTERTYPES::MESSAGEID_COMBINATION:
        OnMessageCombination(pParam);
        break;

    default:
        CGameObject::MessageProc(nMessageID, pParam);
        break;
    };
};


CCharacter::TYPE CCharacter::GetAttackCharacterType(void) const
{
    return m_chrtype;
};


void CCharacter::Draw(void) const
{
    if (TestObjectFlag(GAMEOBJECTTYPES::FLAG_SLEEP))
        return;

    {
        CRenderStateStack RsStack;
        RsStack.Set(rwRENDERSTATEZTESTENABLE, true);
        RsStack.Set(rwRENDERSTATEZWRITEENABLE, true);
        RsStack.Set(rwRENDERSTATEFOGENABLE, false);
        RsStack.Set(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

        ASSERT(m_pModel);
        m_pModel->Draw();
    };

    ASSERT(m_pModuleMan);
    m_pModuleMan->Draw();
};


void CCharacter::IncludeBasicModule(void)
{
    m_pModuleMan->Include(new CAmbientLightModule(this));
    m_pModuleMan->Include(new CAtomicDisplayControlModule(this));
	
	if (m_pModel->GetType() == CModelManager::MODELTYPE_TOON)
		m_pModuleMan->Include(new CInkThicknessCtrlModule(this));

	CLocusModuleForCharacter* pLocusModule = CLocusModuleForCharacter::New(this);
	if (pLocusModule)
		m_pModuleMan->Include(pLocusModule);
};


void CCharacter::UpdateParameter(void)
{
    float dt = CGameProperty::GetElapsedTime();

    m_vVelocity.x += m_vAcceleration.x * dt;
    m_vVelocity.y += m_vAcceleration.y * dt;
    m_vVelocity.z += m_vAcceleration.z * dt;

    if (!TestCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_GRAVITY))
    {
        float fGravity = CGameProperty::GetGravity();
        m_vVelocity.y += (dt * fGravity);
    };

    if (TestCharacterFlag(CHARACTERTYPES::FLAG_CLAMP_VELOCITY_XZ))
    {
        float fBuffY = m_vVelocity.y;

        m_vVelocity.y = 0.0f;

        float fVelocity = Math::Vec3_Length(&m_vVelocity);
        if (fVelocity > m_fMaxVelocity)
        {
            m_vVelocity.x *= (m_fMaxVelocity / fVelocity);
            m_vVelocity.z *= (m_fMaxVelocity / fVelocity);
        };

        m_vVelocity.y = fBuffY;
    };
};


void CCharacter::CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame)
{
    float dt = CGameProperty::GetElapsedTime();

    rvVelocityPerFrame.x = dt * m_vVelocity.x;
    rvVelocityPerFrame.y = dt * m_vVelocity.y;
    rvVelocityPerFrame.z = dt * m_vVelocity.z;    
};


void CCharacter::CheckCollisionForBody(RwV3d& rvVelocityPerFrame, bool bPeriod)
{
    if (CheckCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_BODY))
        return;

    for (int32 i = 0; i < m_nNumBodyHitData; ++i)
    {
        int32 nBoneID = m_pMotionParameterController->GetHitBodyBoneID(i);
        RwSphere sphere = *m_pMotionParameterController->GetHitBodySphere(i);
        
        GetBonePosition(&sphere.center, nBoneID, &sphere.center);

        Math::Vec3_Add(&sphere.center, &sphere.center, &m_vPosition);

        ASSERT(m_apBodyHitData[i]);
        m_apBodyHitData[i]->SetCurrentPos(&sphere.center);

        RwV3d vecHitResult = Math::VECTOR3_ZERO;        
        if (CBodyHitManager::CheckHit(m_apBodyHitData[i], &rvVelocityPerFrame, &vecHitResult))
        {
            float buffY = rvVelocityPerFrame.y;
            rvVelocityPerFrame = vecHitResult;
            rvVelocityPerFrame.y = buffY;
        };
    };
};


void CCharacter::CheckCollisionForWall(RwV3d& rvVelocityPerFrame, bool bPeriod)
{    
    RwV3d vPosition = Math::VECTOR3_ZERO;
	GetPosition(&vPosition);

    RwV3d vBonePosition = Math::VECTOR3_ZERO;
    GetBonePositionOfGroundCheck(&vBonePosition);

    RwV3d vBodyCheckPosition = Math::VECTOR3_ZERO;    
    Math::Vec3_Add(&vBodyCheckPosition, &m_vPosition, &vBonePosition);

    CWorldMap::MOVE_HIT_FLAG hitFlag = CWorldMap::MOVE_HIT_FLAG_NONE;
    RwV3d vReplacePosition = Math::VECTOR3_ZERO;
    bool bCollision = CWorldMap::CheckCollisionCharacterMove(&vBodyCheckPosition,
                                                             &vReplacePosition,
                                                             &rvVelocityPerFrame,
                                                             &hitFlag,
                                                             m_collision.m_fRadius,
                                                             m_collision.m_fHeight);
    
    if ((!bCollision) || TestCharacterFlag(CHARACTERTYPES::FLAG_CANCEL_COLLISION_WALL))
    {
        Math::Vec3_Add(&vPosition, &m_vPosition, &rvVelocityPerFrame);
        SetPosition(&vPosition);

        m_collisionWallInfo.m_bHit = false;
        m_collisionWallInfo.m_bJumpWallHit = false;
    }
    else
    {
        Math::Vec3_Sub(&vPosition, &vReplacePosition, &vBonePosition);
        SetPosition(&vPosition);

        if (hitFlag == CWorldMap::MOVE_HIT_FLAG_XZ)
        {
            const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
            ASSERT(pCollisionResult);
            
            m_collisionWallInfo.m_vPosition         = pCollisionResult->m_vClosestPt;
            m_collisionWallInfo.m_vNormal           = pCollisionResult->m_vNormal;
            m_collisionWallInfo.m_vJumpWallPosition = pCollisionResult->m_mapwall.m_vClosestPt;
            m_collisionWallInfo.m_vJumpWallNormal   = pCollisionResult->m_mapwall.m_vNormal;
            m_collisionWallInfo.m_hittype           = pCollisionResult->m_type;
            m_collisionWallInfo.m_attribute         = pCollisionResult->m_attribute;
            m_collisionWallInfo.m_bHit              = true;
            m_collisionWallInfo.m_bJumpWallHit      = pCollisionResult->m_mapwall.m_bHit;

            SetGimmickInfo(&m_collisionWallInfo.m_gimmickinfo, pCollisionResult->m_type);
        }
        else
        {
            m_collisionWallInfo.m_bHit  = false;
            m_collisionWallInfo.m_bJumpWallHit = false;
        };
    };
};


void CCharacter::CheckCollisionForGround(void)
{
    float fFieldHeight = 0.0f;
    
    RwV3d vBonePosition = Math::VECTOR3_ZERO;
    GetBonePositionOfGroundCheck(&vBonePosition);

    RwV3d vGroundCheckPosition = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vGroundCheckPosition, &m_vPosition, &vBonePosition);
    
    float fDirection = m_fDirection;
    RwV3d vClosestMapPosition = Math::VECTOR3_ZERO;
    bool bCollision = CWorldMap::CheckCollisionCharacterHeight(&vGroundCheckPosition,
                                                               &vClosestMapPosition,
                                                               &fDirection,
                                                               m_collision.m_fRadius,
                                                               m_collision.m_fHeight);
    if (bCollision)
    {
        m_collisionGroundInfo.m_attribute = CWorldMap::GetCollisionResultAttribute();
        CGameObjectManager::SendMessage(this, CHARACTERTYPES::MESSAGEID_TOUCHDOWN, &fFieldHeight);

        if (fDirection != m_fDirection)
            SetDirection(fDirection);

        m_vVelocity.y               = 0.0f;
        m_vAcceleration.y           = 0.0f;
        m_collisionGroundInfo.m_bHit= true;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vPosition, &vClosestMapPosition, &vBonePosition);
        
        SetPosition(&vPosition);
    }
    else
    {
        fFieldHeight = CWorldMap::GetMapHeight(&vGroundCheckPosition, m_collision.m_fRadius);

        CGameObjectManager::SendMessage(this, CHARACTERTYPES::MESSAGEID_AERIAL, &fFieldHeight);

        m_collisionGroundInfo.m_attribute = CWorldMap::GetCollisionResultAttribute();
        m_collisionGroundInfo.m_bHit      = false;
    };

    const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult);

    m_collisionGroundInfo.m_Color   = pCollisionResult->m_Color;
    m_collisionGroundInfo.m_hittype = pCollisionResult->m_type;
    
    SetGimmickInfo(&m_collisionGroundInfo.m_gimmickinfo, pCollisionResult->m_type);

    if (m_collisionGroundInfo.m_bHit)
    {
        if ((m_collisionGroundInfo.m_attribute == MAPTYPES::ATTRIBUTE_DEATH) ||
            (m_collisionGroundInfo.m_attribute == MAPTYPES::ATTRIBUTE_DOBON))
        {
            OnSteppedDeathFloor();
        };
    };
};


void CCharacter::UpdateModel(void)
{
    ASSERT(m_pMotionController);
    ASSERT(m_pModel);

    UpdateTransformForModel();

    float dt = CGameProperty::GetElapsedTime();
    if (TestCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL))
        dt *= m_fMotionSpeed;

    m_pMotionController->Update(dt);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetBonePositionOfPositionCheck(&vPosition);

    m_pModel->SetBoundingSphereOffset(&vPosition);
};


void CCharacter::UpdateCollisionParameterForBody(void)
{
    for (int32 i = 0; i < m_nNumBodyHitData; ++i)
    {
        int32 nBoneID = m_pMotionParameterController->GetHitBodyBoneID(i);
        RwSphere sphere = *m_pMotionParameterController->GetHitBodySphere(i);

        GetBonePosition(&sphere.center, nBoneID, &sphere.center);

        Math::Vec3_Add(&sphere.center, &sphere.center, &m_vPosition);

        ASSERT(m_apBodyHitData[i]);
        m_apBodyHitData[i]->SetCurrentPos(&sphere.center);
    };
};


void CCharacter::UpdateCollisionParameterForCatch(void)
{
    if (TestCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_CATCH))
        return;

    int32 nNumCatch = m_pMotionParameterController->GetNumHitCatchData();
    if (!nNumCatch)
        return;
    
    CHitCatchData Catch;
    Catch.SetObject(GetHandle());
    Catch.SetObjectType(GetType());
    Catch.SetShape(CHitCatchData::SHAPE_SPHERE);
    
    for (int32 i = 0; i < nNumCatch; ++i)
    {
        m_pMotionParameterController->GetHitCatchData(i, &Catch);

        int32 nBoneID = m_pMotionParameterController->GetHitCatchBoneID(i);
        RwSphere sphere = *m_pMotionParameterController->GetHitCatchSphere(i);

        GetBonePosition(&sphere.center, nBoneID, &sphere.center);

        Math::Vec3_Add(&sphere.center, &sphere.center, &m_vPosition);

        Catch.SetSphere(&sphere);
        
        CHitAttackManager::RegistCatch(&Catch);
    };
};


void CCharacter::UpdateCollisionParameterForAttack(void)
{
    RwV3d vBodyPos = Math::VECTOR3_ZERO;
    GetBodyPosition(&vBodyPos);
    
    CHitAttackData Attack;
    Attack.SetObject(GetHandle());
    Attack.SetObjectPos(&vBodyPos);
    Attack.SetShape(CHitAttackData::SHAPE_SPHERE);

    int32 nNumAttack = m_pMotionParameterController->GetNumHitAttackData();

    for (int32 i = 0; i < nNumAttack; ++i)
    {
        if (!m_pMotionParameterController->IsHitAttackEnableTime(i, m_pMotionController->GetTime()))
            continue;

        m_pMotionParameterController->GetHitAttackData(i, &Attack);

        int32 nBoneID = m_pMotionParameterController->GetHitAttackBoneID(i);
        RwSphere sphere = *m_pMotionParameterController->GetHitAttackSphere(i);

        GetBonePosition(&sphere.center, nBoneID, &sphere.center);

        Math::Vec3_Add(&sphere.center, &sphere.center, &m_vPosition);
        
        Attack.SetSphere(&sphere);
        CalcAttackPower(Attack);
        
        CHitAttackManager::RegistAttack(&Attack);
    };
};


void CCharacter::CheckTimingData(void)
{
    ASSERT(m_pMotionParameterController);

    float fNowTime = m_pMotionController->GetTime();
    float fPrevTime = fNowTime - CGameProperty::GetElapsedTime();

    if (fPrevTime < 0.0f)
        fPrevTime = 0.0f;

    if (m_pMotionParameterController->IsAttackTiming(fPrevTime, fNowTime))
        SetCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING);
    else
        ClearCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_TIMING);

    if (m_pMotionParameterController->IsInvincibleTime(fNowTime))
        SetCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING);
    else
        ClearCharacterFlag(CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING);
};


void CCharacter::OnMessageAerial(float fFieldHeight)
{
    if (!OnMessageCanChangingAerial(fFieldHeight))
        return;

    RwV3d vGroundBonePos = Math::VECTOR3_ZERO;
    GetBonePositionOfGroundCheck(&vGroundBonePos);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    GetPosition(&vPosition);
    
    RwV3d vCheckPosition = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vCheckPosition, &vPosition, &vGroundBonePos);

	float fHeight = std::fabs(vCheckPosition.y - fFieldHeight);
    if (fHeight <= 0.2f)
    {
        vPosition.y += (vGroundBonePos.y + (fFieldHeight - vCheckPosition.y));
        SetPosition(&vPosition);
    }
    else
    {
        OnMessageDoChangingAerial();
    };
};


bool CCharacter::OnMessageCanChangingAerial(float fFieldHeight)
{
    return false;
};


void CCharacter::OnMessageDoChangingAerial(void)
{
    ;
};


void CCharacter::OnMessageTouchdown(float fFieldHeight)
{
    ;
};


void CCharacter::OnMessageCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);

    if (TestCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_CATCH))
        return;

    CCharacterAttackCalculator calculator(*this, *pAttack);
    
    CHARACTERTYPES::ATTACKRESULTTYPE attackresult = OnDamage(calculator);
    
    int32 iDamage = static_cast<int32>(calculator.CalcDamage(attackresult));
    if (iDamage > 0)
        CGameObjectManager::SendMessage(this, CHARACTERTYPES::MESSAGEID_RECVDMG, reinterpret_cast<void*>(iDamage));
    
    calculator.PlayEffect(attackresult);
    calculator.PlaySE(attackresult);
};


void CCharacter::OnMessageAttackResult(CHitCatchData* pCatch)
{
    ASSERT(pCatch);
};


void CCharacter::OnMessageReceivedDamage(int32 nDamage)
{
    ;
};


void CCharacter::CalcAttackPower(CHitAttackData& rAttack)
{
    ;
};


CHARACTERTYPES::ATTACKRESULTTYPE CCharacter::OnDamage(CCharacterAttackCalculator& calculator)
{
    return CHARACTERTYPES::ATTACKRESULTTYPE_INEFFECTIVE;
};


void CCharacter::OnMessageCatch(void* pParam)
{
    ;
};


void CCharacter::OnMessageLift(void* pParam)
{
    ;
};


void CCharacter::OnMessageThrow(void* pParam)
{
    ;
};


void CCharacter::OnMessageMissThrow(void* pParam)
{
    ;
};


void CCharacter::OnMessageCombination(void* pParam)
{
    ;
};


void CCharacter::OnChangeMotion(void)
{
    CLocusModule* pLocusModule = static_cast<CLocusModule*>(m_pModuleMan->GetModule(MODULETYPE::LOCUS));
	if (pLocusModule)
		pLocusModule->Reset();

    CAtomicDisplayControlModule* pAtomicDispCtrlModule = static_cast<CAtomicDisplayControlModule*>(m_pModuleMan->GetModule(MODULETYPE::ATOMIC_DISPLAY_CONTROL));
	if (pAtomicDispCtrlModule)
        pAtomicDispCtrlModule->Reset();

    CHARACTERTYPES::FLAG cflag  = CHARACTERTYPES::FLAG_OCCURED_TIMING
                                | CHARACTERTYPES::FLAG_OCCURED_INVINCIBILITY_TIMING;

    ClearCharacterFlag(cflag);
};


void CCharacter::OnSteppedDeathFloor(void)
{
    ;
};


void CCharacter::RequestDamage(int32 iDmgReq)
{
    m_iDamageRequest = iDmgReq;
};


int32 CCharacter::GetRequestedDamage(void) const
{
    return m_iDamageRequest;
};


void CCharacter::GetBonePosition(RwV3d* pvPosition, int32 nBoneID, const RwV3d* pvOffset) const
{
    ASSERT(m_pModel);
    ASSERT(pvPosition);
    
    if (pvOffset)
    {
        RwMatrix* pBoneMatrix = m_pModel->GetBoneMatrixFromID(nBoneID);
        ASSERT(pBoneMatrix);
        
        RwV3dTransformPoint(pvPosition, pvOffset, pBoneMatrix);
    }
    else
    {
        *pvPosition = *m_pModel->GetBonePositionFromID(nBoneID);
    };

    RwV3d vModelPos = Math::VECTOR3_ZERO;
    m_pModel->GetPosition(&vModelPos);

    Math::Vec3_Sub(pvPosition, pvPosition, &vModelPos);
};


void CCharacter::GetBonePositionOfPositionCheck(RwV3d* pvPosition) const
{
    ASSERT(m_pModel);
    ASSERT(pvPosition);

    RwV3d vPosModel = Math::VECTOR3_ZERO;
    m_pModel->GetPosition(&vPosModel);

    RwV3d vPosBone = *m_pModel->GetBonePositionFromID(CHARACTERTYPES::BONEID_POSITION);
    Math::Vec3_Sub(pvPosition, &vPosBone, &vPosModel);
};


void CCharacter::GetOffsetPosition(RwV3d* pvPosition, int32 no) const
{
    int32 nBoneID = 0;
    RwV3d vOffset = Math::VECTOR3_ZERO;
    
    m_pMotionParameterController->GetOffsetPosition(no, nBoneID, vOffset);

    GetBonePosition(pvPosition, nBoneID, &vOffset);
};


void CCharacter::GetBonePositionOfGroundCheck(RwV3d* pvPosition) const
{
    RwV3d vPosModel = Math::VECTOR3_ZERO;
    m_pModel->GetPosition(&vPosModel);

    RwV3d vPosBone = *m_pModel->GetBonePositionFromID(CHARACTERTYPES::BONEID_GROUND);
    Math::Vec3_Sub(pvPosition, &vPosBone, &vPosModel);
};


void CCharacter::Initialize(PARAMETER* pParameter)
{
    ASSERT(pParameter);

    if (pParameter->m_bToon)
        m_pModel = CModelManager::CreateModelToon(pParameter->m_pszModelName);
    else
        m_pModel = CModelManager::CreateModel(pParameter->m_pszModelName);

    ASSERT(m_pModel);
    
    m_pMotionController = new CMotionController(m_pModel);

    m_pMotionParameterController = new CMotionParameterController(pParameter->m_pszModelName, m_pModel->GetPartsNum());

    m_collision = { m_pMotionParameterController->GetCollisionParameter()->m_fRadius,
                    m_pMotionParameterController->GetCollisionParameter()->m_fHeight };

    CMotionManager::SetCurrentMotionSet(pParameter->m_pszMotionSetName);
    
    int32 nMotionNum = m_pMotionParameterController->GetMotionNum();
    for (int32 i = 0; i < nMotionNum; ++i)
        m_pMotionController->AddMotion(m_pMotionParameterController->GetMotionName(i));

    m_pModuleMan = new CModuleManager;

	m_cflag = CHARACTERTYPES::FLAG_NONE;
    m_collisionWallInfo.m_bHit = false;
    m_collisionGroundInfo.m_bHit = false;

    int32 nBodyHitNum = m_pMotionParameterController->GetNumHitBodyData();
    if (nBodyHitNum)
    {
        m_apBodyHitData = new CBodyHitData*[nBodyHitNum];
		m_nNumBodyHitData = nBodyHitNum;
        
        for (int32 i = 0; i < m_nNumBodyHitData; ++i)
        {
            int32 nBoneID = m_pMotionParameterController->GetHitBodyBoneID(i);
            RwSphere sphere = *m_pMotionParameterController->GetHitBodySphere(i);

            GetBonePosition(&sphere.center, nBoneID, &sphere.center);
            sphere.center = m_vPosition;

            CBodyHitData* pBodyHitData = CBodyHitManager::AllocData();
            ASSERT(pBodyHitData);

            pBodyHitData->InitData(&sphere.center, sphere.radius);
            pBodyHitData->SetHitID(GetHandle());

            m_apBodyHitData[i] = pBodyHitData;
        };
    };

    IncludeBasicModule();
};


void CCharacter::SetPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    m_vPosition = *pvPosition;
};


void CCharacter::SetVelocity(const RwV3d* pvVelocity)
{
    ASSERT(pvVelocity);

    m_vVelocity = *pvVelocity;
};


void CCharacter::SetAcceleration(const RwV3d* pvAcceleration)
{
    ASSERT(pvAcceleration);

    m_vAcceleration = *pvAcceleration;
};


void CCharacter::ResetVelocity(void)
{
    m_vVelocity = Math::VECTOR3_ZERO;
};


void CCharacter::ResetAcceleration(void)
{
    m_vAcceleration = Math::VECTOR3_ZERO;
};


void CCharacter::SetMaxVelocity(float fMaxVel)
{
    m_fMaxVelocity = fMaxVel;
};


float CCharacter::GetMaxVelocity(void) const
{
    return m_fMaxVelocity;
};


void CCharacter::GetBodyPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);

    GetBonePositionOfPositionCheck(pvPosition);
    Math::Vec3_Add(pvPosition, pvPosition, &m_vPosition);
};


void CCharacter::GetFootPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);

    GetBonePositionOfGroundCheck(pvPosition);
    Math::Vec3_Add(pvPosition, pvPosition, &m_vPosition);
};


void CCharacter::SetDirection(float fDir)
{
    if (TestCharacterFlag(CHARACTERTYPES::FLAG_FIXED_DIRECTION))
        return;

    fDir = Math::RadianNormalize(fDir);

    RwV3d vBodyPosition = Math::VECTOR3_ZERO;
    GetBonePositionOfPositionCheck(&vBodyPosition);

    if (!m_pMotionController->GetCurrentMotion() || (vBodyPosition.x == 0.0f) && (vBodyPosition.z == 0.0f))
    {
        m_fDirection = fDir;
    }
    else
    {
        m_fDirection = fDir;

        UpdateTransformForModel();
        UpdateMatrices();

        RwV3d vAfterPosition = Math::VECTOR3_ZERO;
        GetBonePositionOfPositionCheck(&vAfterPosition);

        RwV3d vNewPosition = Math::VECTOR3_ZERO;
        vNewPosition.x = vBodyPosition.x - vAfterPosition.x;
        vNewPosition.y = 0.0f;
        vNewPosition.z = vBodyPosition.z - vAfterPosition.z;
		
        vNewPosition.x += m_vPosition.x;
        vNewPosition.y += m_vPosition.y;
        vNewPosition.z += m_vPosition.z;

        SetPosition(&vNewPosition);
    };
};


void CCharacter::SetDirectionFromVector(const RwV3d* pvTargetDirection)
{
    ASSERT(pvTargetDirection);

    RwV3d vTargetDirection = *pvTargetDirection;
    vTargetDirection.y = 0.0f;
    
    Math::Vec3_Normalize(&vTargetDirection, &vTargetDirection);

    if (Math::Vec3_Length(&vTargetDirection) > 0.0f)
    {
        RwMatrix matrix;        
        RwMatrixSetIdentityMacro(&matrix);
        Math::Matrix_RotateY(&matrix, m_fDirection);
        
        RwV3d vDirection = Math::VECTOR3_ZERO;
        Math::Vec3_Normalize(&vDirection, &matrix.at);
        
        RwV3d vTemp = Math::VECTOR3_ZERO;
        Math::Matrix_RotateY(&matrix, -m_fDirection);
        RwV3dTransformVector(&vTemp, &vTargetDirection, &matrix);

        float fRotY = Math::ACos(Math::Vec3_Dot(&vTargetDirection, &vDirection));
        
        if (vTemp.x < 0.0f)
            fRotY = -fRotY;

        SetDirection(m_fDirection + fRotY);        
    };
};


void CCharacter::SetDirectionFromModelRotate(void)
{
    RwV3d vModelRot = Math::VECTOR3_ZERO;
    m_pModel->GetRotation(&vModelRot);

    m_fDirection = vModelRot.y;
};


void CCharacter::RotateVectorByDirection(RwV3d* pvOut, const RwV3d* pvIn) const
{
    RwMatrix matrixRotY;    
    RwMatrixSetIdentityMacro(&matrixRotY);
    
    Math::Matrix_RotateY(&matrixRotY, m_fDirection);
    RwV3dTransformVector(pvOut, pvIn, &matrixRotY);
};


bool CCharacter::IsWallCrashPossible(void) const
{
    if (!m_collisionWallInfo.m_bHit)
        return false;

    if ((m_collisionWallInfo.m_attribute != MAPTYPES::ATTRIBUTE_JUMP) &&
        (m_collisionWallInfo.m_attribute != MAPTYPES::ATTRIBUTE_MISSJUMP))
        return false;

    RwV3d vecVelocity = m_vVelocity;
    vecVelocity.y = 0.0f;
    
    if (Math::Vec3_Length(&vecVelocity) < 10.0f)
        return false;

    RwV3d vCharacter = Math::VECTOR3_ZERO;
    RotateVectorByDirection(&vCharacter, &Math::VECTOR3_AXIS_Z);

    RwV3d vWall = Math::VECTOR3_ZERO;    
    Math::Vec3_Cross(&vWall, &m_collisionWallInfo.m_vNormal, &Math::VECTOR3_AXIS_Y);    
    vWall.y = 0.0f;

    Math::Vec3_Normalize(&vWall, &vWall);

    if (std::fabs(Math::Vec3_Dot(&vWall, &vCharacter)) <= 0.35f)
        return true;

    return false;
};


void CCharacter::ChangeMotion(const char* pszMotionName, bool bForce)
{
    ASSERT(pszMotionName);
    ASSERT(m_pMotionController);
    ASSERT(m_pMotionParameterController);

    RwV3d vLocalBonePosPrev = Math::VECTOR3_ZERO;
    GetBonePositionOfPositionCheck(&vLocalBonePosPrev);

    float fBlendTime = 0.0f;

    if (m_pMotionParameterController->GetCurrentMotionParameter() &&
        m_pMotionParameterController->GetPlaymode())
    {
        m_pMotionParameterController->SetCurrentMotion(pszMotionName);
        fBlendTime = m_pMotionParameterController->GetBlendTime();
    }
    else
    {
        m_pMotionParameterController->SetCurrentMotion(pszMotionName);
    };

    m_pMotionController->SetCurrentMotion(pszMotionName, 0.0f, 0.0f, fBlendTime, bForce);
    
    switch (m_pMotionParameterController->GetPlaymode())
    {
    case CMotionParameterController::PLAYMODE_ONESHOT:
        m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_ONESHOT);
        m_pMotionController->Play();
        break;

    case CMotionParameterController::PLAYMODE_REPEAT:
        m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_REPEAT);
        m_pMotionController->Play();
        break;

    case CMotionParameterController::PLAYMODE_STOP:
        m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_ONESHOT);
        m_pMotionController->Pause();
        break;

    default:
        ASSERT(false);
        break;
    };

    RwV3d vLocalBonePosNew = Math::VECTOR3_ZERO;
    GetBonePositionOfPositionCheck(&vLocalBonePosNew);
    
    RwV3d vPos = m_vPosition;
    vPos.x += (vLocalBonePosPrev.x - vLocalBonePosNew.x);
    vPos.y += (vLocalBonePosPrev.y - vLocalBonePosNew.y);
    vPos.z += (vLocalBonePosPrev.z - vLocalBonePosNew.z);
    m_vPosition = vPos;

    RwV3d vFrameVelocity = Math::VECTOR3_ZERO;
    bool bPeriod = false;
    CheckCollisionForBody(vFrameVelocity, bPeriod);
    CheckCollisionForWall(vFrameVelocity, bPeriod);

    UpdateTransformForModel();
    UpdateMatrices();
    
    OnChangeMotion();
};


void CCharacter::PlayMotion(void)
{
    ASSERT(m_pMotionController);
    
    m_pMotionController->Play();
};


void CCharacter::StopMotion(void)
{
    ASSERT(m_pMotionController);

    m_pMotionController->Pause();
};


bool CCharacter::IsMotionEnd(void) const
{
    ASSERT(m_pMotionController);
    ASSERT(m_pMotionController->GetPlaymode() != CMotionController::PLAYMODE_REPEAT);

    return m_pMotionController->IsPaused();
};


void CCharacter::SetMotionTime(float fTime)
{
    ASSERT(m_pMotionController);

    m_pMotionController->SetTime(fTime);
};


void CCharacter::SetMotionCtrlTime(float fTime)
{
	SetMotionSpeed(fTime);
    SetCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
};


void CCharacter::ClearMotionCtrlTime(void)
{
    ClearCharacterFlag(CHARACTERTYPES::FLAG_MOTION_SPEED_CTRL);
	SetMotionSpeed(1.0f);
};


float CCharacter::GetMotionTime(void) const
{
    ASSERT(m_pMotionController);

    return m_pMotionController->GetTime();
};


float CCharacter::GetMotionEndTime(void) const
{
    ASSERT(m_pMotionController);

	return m_pMotionController->GetCurrentMotionEndTime();
};


const char* CCharacter::GetMotionName(void) const
{
    ASSERT(m_pMotionController);
    
    return m_pMotionController->GetMotionName();
};


float CCharacter::GetNextChainMotionConnectTime(void) const
{
    ASSERT(m_pMotionParameterController);

    float fResult = m_pMotionParameterController->GetNextChainMotionConnectTime(nullptr);
    if (fResult == 0.0f)
        fResult = m_pMotionController->GetCurrentMotionEndTime();

    return fResult;
};


void CCharacter::SetEnableBodyHit(bool bEnable)
{
	for (int32 i = 0; i < m_nNumBodyHitData; ++i)
        m_apBodyHitData[i]->SetState(CBodyHitData::STATE_ENABLE, bEnable);
    
    if (bEnable)
        ClearCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_BODY);
    else
        SetCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_BODY);
};


void CCharacter::SetEnableCatchHit(bool bEnable)
{
    if (bEnable)
        ClearCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_CATCH);
    else
        SetCharacterFlag(CHARACTERTYPES::FLAG_INVALID_HIT_CATCH);
};


void CCharacter::SetEnableBodyHitSelfToOther(bool bEnable)
{
    for (int32 i = 0; i < m_nNumBodyHitData; ++i)
    {
        ASSERT(m_apBodyHitData[i]);
        m_apBodyHitData[i]->SetState(CBodyHitData::STATE_SELF_TO_OTHER, bEnable);
    };
};


bool CCharacter::IsEnableBodyHitSelfToOther(void) const
{
    ASSERT(m_nNumBodyHitData > 0);
    ASSERT(m_apBodyHitData[0]);

    return m_apBodyHitData[0]->IsEnableState(CBodyHitData::STATE_SELF_TO_OTHER);
};


void CCharacter::UpdateTransformForModel(void)
{
    ASSERT(m_pModel);

    m_pModel->SetPosition(&m_vPosition);
    m_pModel->UpdateFrame();
    
    if (!TestCharacterFlag(CHARACTERTYPES::FLAG_FIXED_MODEL_ROTATION))
    {
        RwV3d vRotation = { 0.0f, m_fDirection, 0.0f };
        m_pModel->SetRotation(&vRotation);
        m_pModel->UpdateFrame();
    };
};


void CCharacter::UpdateMatrices(void)
{
    ASSERT(m_pModel);
    ASSERT(m_pMotionController);

    m_pModel->UpdateFrame();
    m_pMotionController->UpdateMatrices();
};


void CCharacter::SetMotionSpeed(float fMotionSpeed)
{
    ASSERT(fMotionSpeed > 0.0f);
    
    m_fMotionSpeed = fMotionSpeed;
};


void CCharacter::SetGimmickInfo(COLLISION_GIMMICK_INFO* pCollGimmickInfo, MAPTYPES::HITTYPE hittype)
{    
    ASSERT(pCollGimmickInfo);

    if (hittype != MAPTYPES::HITTYPE_GIMMICK)
        return;

    const CWorldMap::COLLISIONRESULT* pCollisionResult = CWorldMap::GetCollisionResult();
    ASSERT(pCollisionResult);
    
    std::strcpy(pCollGimmickInfo->m_szGimmickObjName, pCollisionResult->m_mapobj.m_szGimmickObjName);
    pCollGimmickInfo->m_type = pCollisionResult->m_mapobj.m_type;
};


void CCharacter::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);

    *pvPosition = m_vPosition;
};


void CCharacter::GetVelocity(RwV3d* pvVelocity) const
{
    ASSERT(pvVelocity);

    *pvVelocity = m_vVelocity;
};


const RwV3d& CCharacter::GetVelocity(void) const
{
    return m_vVelocity;
};


void CCharacter::GetAcceleration(RwV3d* pvAcceleration) const
{
    ASSERT(pvAcceleration);

    *pvAcceleration = m_vAcceleration;
};


float CCharacter::GetDirection(void) const
{
    return m_fDirection;
};


CModel* CCharacter::GetModel(void) const
{
    return m_pModel;
};


const CCharacter::COLLISIONWALLINFO* CCharacter::GetCollisionWall(void) const
{
    return &m_collisionWallInfo;
};


const CCharacter::COLLISIONGROUNDINFO* CCharacter::GetCollisionGround(void) const
{
    return &m_collisionGroundInfo;
};


CMotionParameterController* CCharacter::GetMotionParameterController(void) const
{
    return m_pMotionParameterController;
};


CCharacter::TYPE CCharacter::GetCharacterType(void) const
{
    return m_chrtype;
};


const CCharacter::COLLISIONPARAMETER& CCharacter::GetCollisionParameter(void) const
{
    return m_collision;
};


IModule* CCharacter::GetModule(MODULETYPE::VALUE type) const
{
    ASSERT(m_pModuleMan);
    return m_pModuleMan->GetModule(type);
};


bool CCharacter::IsModuleIncluded(MODULETYPE::VALUE type) const
{
    ASSERT(m_pModuleMan);
    return m_pModuleMan->IsIncluded(type);
};


CCharacter::LIFT_INFO& CCharacter::LiftInfo(void)
{
    return m_liftinfo;
};


void CCharacter::SetCharacterFlag(CHARACTERTYPES::FLAG flag)
{
    m_cflag |= flag;
};


void CCharacter::ClearCharacterFlag(CHARACTERTYPES::FLAG flag)
{
    m_cflag &= (~flag);
};


bool CCharacter::TestCharacterFlag(CHARACTERTYPES::FLAG flag) const
{
    return ((m_cflag & flag) == flag);
};


bool CCharacter::CheckCharacterFlag(CHARACTERTYPES::FLAG flag) const
{
    return ((m_cflag & flag) != 0);
};