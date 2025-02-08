#include "ApproachingTrailerGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Light.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/ApproachingTrailerGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/Component/Gimmick/GimmickMotion.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Motion/MotionManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Misc/DebugShape.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


class CAppTrailerAttackBase
{
public:
    CAppTrailerAttackBase(CApproachingTrailerGimmick* pTrailer);
    virtual ~CAppTrailerAttackBase(void) {};
    virtual bool Attach(void) = 0;
    virtual bool Detach(void) = 0;
    virtual bool Move(void) = 0;
    bool Run(void);
    bool Charge(void);
    void ShotEnergyBall(const RwV3d* pvPos, const RwV3d* pvDir, float fScale);

    inline const RwV3d* GetShotPos(void) const { return &m_vShotPos; };
    inline void SetShotPos(const RwV3d* pvPos) { m_vShotPos = *pvPos; };

    inline CApproachingTrailerGimmick& Trailer(void) const { return *m_pTrailer; };
    inline float GetSpeed(void) const { return m_fSpeed; };
    
protected:
    CApproachingTrailerGimmick* m_pTrailer;
    RwV3d m_vShotPos;
    int32 m_iPlayerNum;
    float m_fSpeed;
    int32 m_eState;
};


class CAppTrailerAttackAimingShot final : public CAppTrailerAttackBase
{
public:
    CAppTrailerAttackAimingShot(CApproachingTrailerGimmick* pTrailer, int32 iShotCnt);
    virtual bool Attach(void) override;
    virtual bool Detach(void) override { return true; };
    virtual bool Move(void) override;

private:
    int32 m_iTargetPlayerNo;
    int32 m_iShotCnt;
    int32 m_iShotNum;
    float m_fTimerCounter;
};


class CAppTrailerAttackAreaShot final : public CAppTrailerAttackBase
{
public:
    CAppTrailerAttackAreaShot(CApproachingTrailerGimmick* pTrailer, float fOfsX);
    virtual bool Attach(void) override;
    virtual bool Detach(void) override { return true; };
    virtual bool Move(void) override;
    
private:
    float m_fOfsX;
};


class CAppTrailerAttackCenterLinearShot final : public CAppTrailerAttackBase
{
public:
    CAppTrailerAttackCenterLinearShot(CApproachingTrailerGimmick* pTrailer);
    virtual bool Attach(void) override;
    virtual bool Detach(void) override { return true; };
    virtual bool Move(void) override;

private:
    int32 m_iTargetPlayerNo;
    int32 m_iShotCnt;
    int32 m_iShotNum;
    float m_fTimerCounter;
    RwV3d* m_pvPos;
    RwV3d m_avShotPoint[3];
};


class CAppTrailerAttackSurroundShot final : public CAppTrailerAttackBase
{
public:
    CAppTrailerAttackSurroundShot(CApproachingTrailerGimmick* pTrailer);
    virtual bool Attach(void) override;
    virtual bool Detach(void) override { return true; };
    virtual bool Move(void) override;
    
private:
    int32 m_iTargetPlayerNo;
};


class CAppTrailerAttackThreePointShot final : public CAppTrailerAttackBase
{
public:
    CAppTrailerAttackThreePointShot(CApproachingTrailerGimmick* pTrailer, float fOfsX);
    virtual bool Attach(void) override;
    virtual bool Detach(void) override { return true; };
    virtual bool Move(void) override;
    
private:
    int32 m_iTargetPlayerNo;
    int32 m_iShotCnt;
    int32 m_iShotNum;
    float m_fTimerCounter;
    RwV3d* m_pvPos;
    RwV3d m_avShotPoint[3];
    float m_fOfsX;
};


CAppTrailerAttackBase::CAppTrailerAttackBase(CApproachingTrailerGimmick* pTrailer)
: m_pTrailer(pTrailer)
, m_vShotPos(Math::VECTOR3_ZERO)
, m_iPlayerNum(0)
, m_fSpeed(0.0f)
, m_eState(0)
{
    ASSERT(m_pTrailer);
};


bool CAppTrailerAttackBase::Run(void)
{
    bool bResult = false;
    
    switch (m_eState)
    {
    case 0:
        {
            if (Charge())
                m_eState = 1;
        }
        break;

    case 1:
        {
            bResult = Move();
        }
        break;
    };
    
    return bResult;
};


bool CAppTrailerAttackBase::Charge(void)
{
    float fPathT = m_pTrailer->GetMovement()->GetPathT();
    float fAimAttackPathT = m_pTrailer->GetAimAttackPathTime();
    
    if (fAimAttackPathT < fPathT)
    {
        m_fSpeed = 3.5f;
        return true;
    };
    
    return false;
};


void CAppTrailerAttackBase::ShotEnergyBall(const RwV3d* pvPos, const RwV3d* pvDir, float fSpeed)
{
    CMagicManager::CParameter MagicParam;
    MagicParam.SetPositon(pvPos);
    MagicParam.SetDirection(pvDir);
    MagicParam.SetObject(m_pTrailer);
    MagicParam.SetSoundPlay(false);

    uint32 hMagic = CMagicManager::Play(MAGICID::ID_GUNBALL, &MagicParam);
    ASSERT(hMagic);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Normalize(&vVelocity, pvDir);
    Math::Vec3_Scale(&vVelocity, &vVelocity, Clamp(fSpeed, 15.0f, 22.0f));

    CMagicManager::SetSpeed(hMagic, &vVelocity);
};


//
// *********************************************************************************
//


CAppTrailerAttackAimingShot::CAppTrailerAttackAimingShot(CApproachingTrailerGimmick* pTrailer, int32 iShotCnt)
: CAppTrailerAttackBase(pTrailer)
, m_iTargetPlayerNo(0)
, m_iShotCnt(iShotCnt)
, m_iShotNum(0)
, m_fTimerCounter(0.0f)
{
    ;
};


bool CAppTrailerAttackAimingShot::Attach(void)
{
    m_iTargetPlayerNo = 0;
    m_iShotNum = m_iShotCnt;
    m_fTimerCounter = 0.0f;
    m_fSpeed = 10.0f;
    m_eState = 0;

    return true;
};


bool CAppTrailerAttackAimingShot::Move(void)
{
    if (m_fTimerCounter >= 0.0f)
    {
        m_fTimerCounter -= CGameProperty::GetElapsedTime();
        return false;
    };

    if (m_iShotNum > 0)
    {
        m_iPlayerNum += Math::RandRange(0, (GAMETYPES::PLAYERS_MAX * 2));
        m_iPlayerNum %= CGameProperty::GetPlayerNum();

        if (!CGameProperty::Player(m_iTargetPlayerNo)->IsAlive())
            m_iTargetPlayerNo = GAMETYPES::PLAYERS_MAX;

        RwV3d* pvPos = m_pTrailer->GetPlayerPos(m_iTargetPlayerNo);
        RwMatrix* pModeling = m_pTrailer->GetNormalModel()->GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
        
        RwMatrix ltm;
        RwMatrixSetIdentityMacro(&ltm);
        Math::Matrix_Invert(&ltm, pModeling);

        RwV3d vLocal = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vLocal, pvPos, &ltm);
        vLocal.z += 12.0f;

        RwV3d vWorld = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vWorld, &vLocal, pModeling);

        RwV3d vShotDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vShotDir, &vWorld, &m_vShotPos);

        ShotEnergyBall(&m_vShotPos, &vShotDir, 20.0f);

        --m_iShotNum;
        m_fTimerCounter = 0.5f;
        return false;
    };

    return true;
};


//
// *********************************************************************************
//


CAppTrailerAttackAreaShot::CAppTrailerAttackAreaShot(CApproachingTrailerGimmick* pTrailer, float fOfsX)
: CAppTrailerAttackBase(pTrailer)
, m_fOfsX(fOfsX)
{
    ;
};


bool CAppTrailerAttackAreaShot::Attach(void)
{
    m_fSpeed = 10.0f;
    m_eState = 0;

    return true;
};


bool CAppTrailerAttackAreaShot::Move(void)
{
    RwV3d avLocal[] =
    {
        { (m_fOfsX * 0.5f), 0.0f, 25.0f },
        { (m_fOfsX * 1.5f), 0.0f, 20.0f },
        { (m_fOfsX * 3.5f), 0.0f, 24.0f },
    };

    RwMatrix* pModeling = m_pTrailer->GetNormalModel()->GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    
    for (int32 i = 0; i < COUNT_OF(avLocal); ++i)
    {        
        RwV3d vWorld = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vWorld, &avLocal[i], pModeling);

        RwV3d vShotDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vShotDir, &vWorld, &m_vShotPos);

        ShotEnergyBall(&m_vShotPos, &vShotDir, 20.0f);
    };
    
    return true;
};


//
// *********************************************************************************
//


CAppTrailerAttackCenterLinearShot::CAppTrailerAttackCenterLinearShot(CApproachingTrailerGimmick* pTrailer)
: CAppTrailerAttackBase(pTrailer)
, m_iTargetPlayerNo(0)
, m_iShotCnt(0)
, m_iShotNum(0)
, m_fTimerCounter(0.0f)
, m_pvPos(nullptr)
, m_avShotPoint()
{
    ;
};


bool CAppTrailerAttackCenterLinearShot::Attach(void)
{
    m_iTargetPlayerNo += Math::RandRange(0, (GAMETYPES::PLAYERS_MAX * 2));
    m_iTargetPlayerNo %= CGameProperty::GetPlayerNum();

    if (!CGameProperty::Player(m_iTargetPlayerNo)->IsAlive())
        m_iTargetPlayerNo = GAMETYPES::PLAYERS_MAX;

    m_iShotCnt = 0;
    m_iShotNum = 3;    
    m_pvPos = nullptr;    
    m_fTimerCounter = 0.5f;
    
    m_fSpeed = 10.0f;
    m_eState = 0;

    return true;
};


bool CAppTrailerAttackCenterLinearShot::Move(void)
{
    if (!m_pvPos)
    {
        m_pvPos = m_pTrailer->GetPlayerPos(m_iTargetPlayerNo);

        RwMatrix* pModeling = m_pTrailer->GetNormalModel()->GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

        RwMatrix ltm;
        RwMatrixSetIdentityMacro(&ltm);
        Math::Matrix_Invert(&ltm, pModeling);

        RwV3d vLocal = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vLocal, m_pvPos, &ltm);

        for (int32 i = 0; i < COUNT_OF(m_avShotPoint); ++i)
        {
            static float s_afOfsZ[] =
            {
                10.0f,
                15.0f,
                20.0f,
            };

            static_assert(COUNT_OF(s_afOfsZ) == COUNT_OF(m_avShotPoint), "should equal");

            RwV3d vLocalPoint = { 0.0f, vLocal.y, (vLocal.z + s_afOfsZ[i]) };
            RwV3dTransformPoint(&m_avShotPoint[i], &vLocalPoint, pModeling);
        };
    };

    if (m_fTimerCounter >= 0.0f)
    {
        m_fTimerCounter -= CGameProperty::GetElapsedTime();
        return false;
    };

    if (m_iShotNum > 0)
    {
        RwV3d vShotDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vShotDir, &m_avShotPoint[m_iShotCnt++], &m_vShotPos);

        ShotEnergyBall(&m_vShotPos, &vShotDir, 20.0f);

        --m_iShotNum;
        m_fTimerCounter = 0.1f;
        return false;
    };

    return true;
};


//
// *********************************************************************************
//


CAppTrailerAttackSurroundShot::CAppTrailerAttackSurroundShot(CApproachingTrailerGimmick* pTrailer)
: CAppTrailerAttackBase(pTrailer)
, m_iTargetPlayerNo(0)
{
    ;
};


bool CAppTrailerAttackSurroundShot::Attach(void)
{
    m_iTargetPlayerNo += Math::RandRange(0, (GAMETYPES::PLAYERS_MAX * 2));
    m_iTargetPlayerNo %= CGameProperty::GetPlayerNum();

    if (!CGameProperty::Player(m_iTargetPlayerNo)->IsAlive())
        m_iTargetPlayerNo = GAMETYPES::PLAYERS_MAX;

    m_fSpeed = 10.0f;
    m_eState = 0;

    return true;
};


bool CAppTrailerAttackSurroundShot::Move(void)
{
    RwV3d* pvPos = m_pTrailer->GetPlayerPos(m_iTargetPlayerNo);

    RwMatrix* pModeling = m_pTrailer->GetNormalModel()->GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

    RwMatrix ltm;
    RwMatrixSetIdentityMacro(&ltm);
    Math::Matrix_Invert(&ltm, pModeling);

    RwV3d vLocal = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vLocal, pvPos, &ltm);
    vLocal.x = 0.0f;

    RwV3d vWorld = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vWorld, &vLocal, pModeling);

    struct
    {
        float fTheta;
        float fOfsX;
    } static const s_aShotInfo[] =
    {
        { -(MATH_PI / 8.0f),     0.0f },
        { -(MATH_PI / 16.0f),   -1.0f },
        {  (MATH_PI / 16.0f),    1.0f },
        {  (MATH_PI / 8.0f),     0.0f },
    };

    for (int32 i = 0; i < COUNT_OF(s_aShotInfo); ++i)
    {        
        RwV3d vShotPoint = Math::VECTOR3_ZERO;
        vShotPoint.x = vWorld.x + Math::Sin(s_aShotInfo[i].fTheta) * 12.0f + s_aShotInfo[i].fOfsX;
        vShotPoint.y = vWorld.y;
        vShotPoint.z = vWorld.z + Math::Cos(s_aShotInfo[i].fTheta) * 12.0f;

        RwV3d vShotDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vShotDir, &vShotPoint, &m_vShotPos);

        ShotEnergyBall(&m_vShotPos, &vShotDir, 20.0f);
    };

    return true;
};


//
// *********************************************************************************
//


CAppTrailerAttackThreePointShot::CAppTrailerAttackThreePointShot(CApproachingTrailerGimmick* pTrailer, float fOfsX)
: CAppTrailerAttackBase(pTrailer)
, m_iTargetPlayerNo(0)
, m_iShotCnt(0)
, m_iShotNum(0)
, m_fTimerCounter(0.0f)
, m_pvPos(nullptr)
, m_avShotPoint()
, m_fOfsX(fOfsX)
{
    ;
};


bool CAppTrailerAttackThreePointShot::Attach(void)
{
    m_iTargetPlayerNo += Math::RandRange(0, (GAMETYPES::PLAYERS_MAX * 2));
    m_iTargetPlayerNo %= CGameProperty::GetPlayerNum();

    if (!CGameProperty::Player(m_iTargetPlayerNo)->IsAlive())
        m_iTargetPlayerNo = GAMETYPES::PLAYERS_MAX;

    m_iShotCnt = 0;
    m_iShotNum = 3;
    m_pvPos = nullptr;
    m_fTimerCounter = -1.0f;

    m_fSpeed = 10.0f;
    m_eState = 0;

    return true;
};


bool CAppTrailerAttackThreePointShot::Move(void)
{
    if (!m_pvPos)
    {
        m_pvPos = m_pTrailer->GetPlayerPos(m_iTargetPlayerNo);

        RwMatrix* pModeling = m_pTrailer->GetNormalModel()->GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

        RwMatrix ltm;
        RwMatrixSetIdentityMacro(&ltm);
        Math::Matrix_Invert(&ltm, pModeling);

        RwV3d vLocal = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vLocal, m_pvPos, &ltm);

        for (int32 i = 0; i < COUNT_OF(m_avShotPoint); ++i)
        {
            RwV3d vLocalPoint = Math::VECTOR3_ZERO;
            vLocalPoint.x = (m_fOfsX + m_fOfsX) * float(i);
            vLocalPoint.y = vLocal.y;
            vLocalPoint.z = vLocal.z + 12.0f;
            RwV3dTransformPoint(&m_avShotPoint[i], &vLocalPoint, pModeling);
        };
    };

    if (m_fTimerCounter >= 0.0f)
    {
        m_fTimerCounter -= CGameProperty::GetElapsedTime();
        return false;
    };

    if (m_iShotNum > 0)
    {
        RwV3d vShotDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vShotDir, &m_avShotPoint[m_iShotCnt++], &m_vShotPos);

        ShotEnergyBall(&m_vShotPos, &vShotDir, 20.0f);

        --m_iShotNum;
        m_fTimerCounter = 0.2f;
        return false;
    };

    return true;
};


//
// *********************************************************************************
//

static const RwV3d s_avATEffectOffset[] =
{
    {  3.87f,   12.43f, -10.88f },
    { -3.87f,   12.43f, -10.88f },
    {  4.527f,  0.0f,   -3.96f  },
    { -4.527f,  0.0f,   -3.96f  },
};


static const RwV3d s_avATLightOffset[] =
{
    {  3.03f, 3.13f, 0.08f },
    { -3.03f, 3.13f, 0.08f },
};


static const char* s_apszUVAnimModel[] =
{
    "m43n_smoke",
    "m43n_smoke2",
    "m43n_fire1",
    "m43n_fire2",
    "m43n_flash",
};


static float s_afPathLength[] = { 0.0f, 0.0f };
static const float s_fHitRadius = 151.0f;
static const RwV3d s_vOffset = { 0.0f, 0.0f, -150.0f };


CApproachingTrailerGimmick::CApproachingTrailerGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_fFlyawayNearestPathT(0.0f)
, m_bFlyawayRq(false)
, m_pApproachingTrailerGimmickMove(nullptr)
, m_pApproachingTrailerHeadGimmickMove(nullptr)
, m_pBodyHitData(nullptr)
, m_model()
, m_szPathName()
, m_nPower(0)
, m_fFlyawayVelXZ(0.0f)
, m_fFlyawayVelY(0.0f)
, m_fAimAttackPathT(0.0f)
, m_fNearestPathT(0.0f)
, m_fAimPathTimerate(0.0f)
, m_fPathTimerate(0.0f)
, m_pPos(nullptr)
, m_pRot(nullptr)
, m_eState(STATE_WAIT)
, m_fStepTimer(0.0f)
, m_pAttack(nullptr)
, m_apAttackVariation()
, m_nNumAttackVariation(0)
, m_subid(0)
, m_ahEffect()
, m_fEffectTimer(0.0f)
, m_aLight()
, m_aPlayerInfo()
, m_playerInfoCenter()
, m_iNearestPlayerNo(-1)
, m_iPlayerNum(-1)
, m_pMotion(nullptr)
, m_vHitOffset(Math::VECTOR3_ZERO)
, m_bSEPlay(false)
, m_bSEBeep(false)
, m_bSEStop(false)
{
    static_assert(COUNT_OF(m_ahEffect)  == COUNT_OF(s_avATEffectOffset), "should equal");
    static_assert(COUNT_OF(m_aLight)    == COUNT_OF(s_avATLightOffset), "should equal");
    static_assert(COUNT_OF(s_afPathLength) == SUBIDNUM, "should equal");

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    ASSERT(pInitParam->m_subid >= 0);
    ASSERT(pInitParam->m_subid < SUBIDNUM);

    m_subid         = pInitParam->m_subid;
    m_nPower        = (m_subid == SUBID_DESTROYED ? 10 : 40);
    m_fFlyawayVelXZ = 25.0f;
    m_fFlyawayVelY  = 5.0f;
    
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);

    //
    //  init display model
    //
    CModel* pDispModel = CModelManager::CreateModel("m43n_truck");
    ASSERT(pDispModel);

    pDispModel->SetClippingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pDispModel);

    //
    //  init yarare model
    //
    CModel* pYarareModel = CModelManager::CreateModel("m43n_truckhead");
    ASSERT(pYarareModel);

    pYarareModel->SetClippingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pYarareModel);

    //
    //  init trailer movement
    //
	m_pApproachingTrailerGimmickMove = new CApproachingTrailerGimmickMove;

    static const char* s_apszPathName[] =
    {
        "truckpath1",
        "truckpath2",
    };

    static_assert(COUNT_OF(s_apszPathName) == SUBIDNUM, "should equal");

    switch (m_subid)
    {
    case SUBID_NORMAL:
        s_afPathLength[SUBID_NORMAL]    = m_pApproachingTrailerGimmickMove->GetPathLength(s_apszPathName[SUBID_NORMAL]);
        s_afPathLength[SUBID_DESTROYED] = m_pApproachingTrailerGimmickMove->GetPathLength(s_apszPathName[SUBID_DESTROYED]);
        break;
        
    case SUBID_DESTROYED:
        m_fPathTimerate = static_cast<PARAM*>(pInitParam)->fPathTimerate;
        break;

    default:
        ASSERT(false);
        break;
    };

    m_pApproachingTrailerGimmickMove->InitPath(s_apszPathName[m_subid], 0.0f);
    m_pApproachingTrailerGimmickMove->SetPathLength(s_afPathLength[m_subid]);
    m_pApproachingTrailerGimmickMove->SetRotation(&vRotation);    

    //
    //  init head movement
    //
    m_pApproachingTrailerHeadGimmickMove = new CApproachingTrailerHeadGimmickMove;    

    //
    //  init model position & rotation (required do it after init movement - see InitPath() for details)
    //
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetPosition(&vPosition);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&vPosition);
    m_model.UpdateFrame();

    //
    //  init body hit
    //
    RwMatrix* pModeling = m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

    RwV3d vBodyPos = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vBodyPos, &s_vOffset, pModeling);

    m_pBodyHitData = CBodyHitManager::AllocData();
    ASSERT(m_pBodyHitData);
    m_pBodyHitData->SetState(CBodyHitData::STATE_SELF_TO_OTHER, true);
    m_pBodyHitData->InitData(&vBodyPos, s_fHitRadius);
    
    //
    //  init map model
    //
    for (int32 i = 0; i < COUNT_OF(s_apszUVAnimModel); ++i)
        CMapClumpModelManager::SetDrawEnable(s_apszUVAnimModel[i], false);
    
    //
    //  init attack
    //
    m_apAttackVariation[ATTACKTYPE_THREE_L]  = new CAppTrailerAttackThreePointShot(this, -1.0f);
    m_apAttackVariation[ATTACKTYPE_THREE_R]  = new CAppTrailerAttackThreePointShot(this, 1.0f);
    m_apAttackVariation[ATTACKTYPE_SURROUND] = new CAppTrailerAttackSurroundShot(this);
    m_apAttackVariation[ATTACKTYPE_CENTER]   = new CAppTrailerAttackCenterLinearShot(this);
    m_apAttackVariation[ATTACKTYPE_AREA_L]   = new CAppTrailerAttackAreaShot(this, -1.0f);
    m_apAttackVariation[ATTACKTYPE_AREA_R]   = new CAppTrailerAttackAreaShot(this, 1.0f);
    m_apAttackVariation[ATTACKTYPE_AIMING_1] = new CAppTrailerAttackAimingShot(this, 1);
    m_apAttackVariation[ATTACKTYPE_AIMING_3] = new CAppTrailerAttackAimingShot(this, 3);
    m_apAttackVariation[ATTACKTYPE_AIMING_5] = new CAppTrailerAttackAimingShot(this, 5);

    m_pAttack = m_apAttackVariation[ATTACKTYPE_THREE_L];

    m_nNumAttackVariation = ATTACKTYPENUM + 1;

    //
    //  init effect
    //
    EffectCreate();
    
    //
    //  init light
    //
    LightCreate();
    
    //
    //  init motion
    //
    CMotionManager::SetCurrentMotionSet("clash");
    
    m_pMotion = new CGimmickMotion(pYarareModel);
    m_pMotion->AddMotion("clash");
    m_pMotion->SetMotion("clash", 0.0f, 0.0f, 0.0f, false);

    //
    //  setup model & movement strat
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pApproachingTrailerGimmickMove);
};


CApproachingTrailerGimmick::~CApproachingTrailerGimmick(void)
{
    if(m_pMotion)
    {
        delete m_pMotion;
        m_pMotion = nullptr;
    };

    LightDestroy();
    EffectDestroy();

    for (int32 i = 0; i < COUNT_OF(m_apAttackVariation); ++i)
    {
        if (m_apAttackVariation[i])
        {
            delete m_apAttackVariation[i];
            m_apAttackVariation[i] = nullptr;
        };
    };

    if (m_pBodyHitData)
    {
        CBodyHitManager::FreeData(m_pBodyHitData);
        m_pBodyHitData = nullptr;
    };

    if (m_pApproachingTrailerHeadGimmickMove)
    {
        delete m_pApproachingTrailerHeadGimmickMove;
        m_pApproachingTrailerHeadGimmickMove = nullptr;
    };

    if (m_pApproachingTrailerGimmickMove)
    {
        delete m_pApproachingTrailerGimmickMove;
        m_pApproachingTrailerGimmickMove = nullptr;
    };
};


void CApproachingTrailerGimmick::PostMove(void)
{
    SetPlayerInfo();

    m_fStepTimer += CGameProperty::GetElapsedTime();
    
    switch (m_eState)
    {
    case STATE_WAIT:
        Wait();
        break;

    case STATE_CRUISE:
        Cruise();
        break;

    case STATE_PRE_CHARGE:
        PreCharge();
        break;

    case STATE_CHARGE:
        Charge();
        break;

    case STATE_POST_CHARGE:
        PostCharge();
        break;

    case STATE_SLEEP:
        Sleep();
        break;

    case STATE_PRE_ATTACK:
        PreAttack();
        break;

    case STATE_ATTACK:
        Attack();
        break;

    case STATE_POST_ATTACK:
        PostAttack();
        break;

    case STATE_EXPLOSE:
        Explose();
        break;

    case STATE_CHANGE:
        Change();
        break;

    case STATE_HEAD_PRE_CHARGE:
        HeadPreCharge();
        break;

    case STATE_HEAD_CHARGE:
        HeadCharge();
        break;

    case STATE_HEAD_POST_CHARGE:
        HeadPostCharge();
        break;

    case STATE_STOP:
        Stop();
        break;
        
    default:
        ASSERT(false);
        break;
    };

    m_fPathTimerate = RateControl(m_fPathTimerate, m_fAimPathTimerate, CGameProperty::GetElapsedTime() * 0.02f);
    m_pApproachingTrailerGimmickMove->SetPathTimeRate(m_fPathTimerate);

    CommonUpdate();

    if (m_subid == SUBID_NORMAL)
    {
        PLAYERTYPES::STATUS eNearestStatus = m_aPlayerInfo[m_iNearestPlayerNo].m_eStatus;
        if ((eNearestStatus == PLAYERTYPES::STATUS_DOWN_FRONT) ||
            (eNearestStatus == PLAYERTYPES::STATUS_DOWN_BACK))
        {
            LightSet(0.0f, true);

            if (m_aPlayerInfo[m_iNearestPlayerNo].m_eDistance == DISTANCE_SHORT)
            {
                m_fPathTimerate = 0.00001f;
                m_fAimPathTimerate = 0.0f;
            };
        };
    };

    if (m_pApproachingTrailerGimmickMove->IsPathEnd() && (m_eState < STATE_EXPLOSE))
    {
        switch (m_subid)
        {
        case SUBID_NORMAL:
            {
                PARAM param;
                std::memset(&param, 0, sizeof(param));

                param.m_subid = SUBID_DESTROYED;
                std::strcpy(param.m_szName, "AppTrailer2");
                CGimmickUtils::QuaternionFromAxisAngle(&param.m_quat, &Math::VECTOR3_AXIS_Y, 0.0f);
                param.fPathTimerate = (m_fPathTimerate / s_afPathLength[SUBID_DESTROYED]) * s_afPathLength[SUBID_NORMAL];

                CGimmick* pGimmick = CGimmickManager::Create(GIMMICKID::ID_K_M43N, 0, &param);
                ASSERT(pGimmick);

                Release();
            }
            break;

        case SUBID_DESTROYED:
            {
                SetState(STATE_EXPLOSE);                
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CApproachingTrailerGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;
    
    GetMoveStrategy()->Start();    
    SetState(STATE_PRE_CHARGE);
    SetState(STATE_CHARGE);
    LightSet(0.5f, false);
    m_bSEPlay = true;
};


void CApproachingTrailerGimmick::OnAttackResult(CHitCatchData* pCatch)
{
    CGameObject* pGameObj = CGameObjectManager::GetObject(pCatch->GetObjectHandle());
    ASSERT(pGameObj);

    if (pGameObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
        return;

    if (m_subid != SUBID_NORMAL)
        return;

    if (static_cast<CCharacter*>(pGameObj)->GetCharacterType() != CCharacter::TYPE_PLAYER)
        return;

    if (m_model.GetDrawType() != CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)
        return;

    if (m_eState != STATE_SLEEP)
    {
        RwV3d vPos = Math::VECTOR3_ZERO;
        static_cast<CCharacter*>(pGameObj)->GetPosition(&vPos);

        RwV3d vDir = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDir, &vPos, pCatch->GetAttack()->GetObjectPos());
		Math::Vec3_Normalize(&vDir, &vDir);

        RwV3d vFlyawayPos = Math::VECTOR3_ZERO;
        GetFlyawayPosition(&vFlyawayPos, &vPos, &vDir);

#ifdef _DEBUG
        if (CGimmickDebug::SHOW_AREA)
        {
            RwSphere HitSphere = { vFlyawayPos, 3.0f };

            CDebugShape::m_fDuration = 5.0f;
            CDebugShape::ShowSphere(&HitSphere, CGimmickDebug::COLOR_AREA_SPHERE);
            CDebugShape::ShowLabel(&vFlyawayPos, "FLYAWAY POS");
            CDebugShape::m_fDuration = 0.0f;
        };
#endif        
		vFlyawayPos.z -= 8.0f;
        m_fFlyawayNearestPathT = m_pApproachingTrailerGimmickMove->GetNearestPathT(&vFlyawayPos);
        m_bFlyawayRq = true;
    };

    SetState(STATE_SLEEP);
};


void CApproachingTrailerGimmick::Wait(void)
{
    m_fAimPathTimerate = 0.0f;

    if (m_subid == SUBID_DESTROYED)
    {
        GetMoveStrategy()->Start();
        m_bSEPlay = true;

        if ((m_playerInfoCenter.m_fDistance > 60.0f) || (m_fStepTimer > 10.0f) || (m_pApproachingTrailerGimmickMove->GetPathT() > 0.2f))
        {
            SetState(STATE_CHARGE);
            m_fNearestPathT = 1.5f;
            m_fAimPathTimerate = GetPathTimerateByMPS(20.0f);
        }
        else
        {
            m_fAimPathTimerate = GetPathTimerateByMPS(10.0f);
        };
    };
};


void CApproachingTrailerGimmick::Cruise(void)
{
    if (m_fStepTimer < 0.3f)
        return;

    float fMPS = 3.5f;

    switch (m_playerInfoCenter.m_eDistance)
    {
    case DISTANCE_SHORT:
        {
            if (m_nNumAttackVariation == ATTACKTYPENUM)
            {
                fMPS = 1.75f;
                if (m_fStepTimer > 2.0f)
                    m_nNumAttackVariation = ATTACKTYPENUM + 1;
                m_fAimPathTimerate = GetPathTimerateByMPS(fMPS);
            };

            if (Math::RandRange(0, 100) >= 25)
            {
                fMPS = 3.5f;
                SetState(STATE_CRUISE);
                m_fAimPathTimerate = GetPathTimerateByMPS(fMPS);
            }
            else
            {
                SetState(STATE_PRE_CHARGE);
                m_nNumAttackVariation = ATTACKTYPENUM;
                m_fAimPathTimerate = GetPathTimerateByMPS(fMPS);
            };
        }
        break;

    case DISTANCE_MIDDLE:
        {
            uint32 rnd = Math::RandRange(0, m_nNumAttackVariation);
            if (rnd != ATTACKTYPENUM)
            {
                m_pAttack = m_apAttackVariation[rnd];
                SetState(STATE_PRE_ATTACK);
                m_nNumAttackVariation = ATTACKTYPENUM + 1;
                m_fAimPathTimerate = GetPathTimerateByMPS(fMPS);
            }
            else
            {
                SetState(STATE_PRE_CHARGE);
                m_nNumAttackVariation = ATTACKTYPENUM;
                m_fAimPathTimerate = GetPathTimerateByMPS(fMPS);
            };
        }
        break;

    case DISTANCE_LONG:
        {
            fMPS = 40.0f;
            SetState(STATE_CRUISE);
            m_fAimPathTimerate = GetPathTimerateByMPS(fMPS);
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CApproachingTrailerGimmick::PreCharge(void)
{
    if (m_fStepTimer <= 3.0f)
    {
        //
        //  TODO lights blinking
        //
    }
    else
    {
        SetState(STATE_CHARGE);
    };
};


void CApproachingTrailerGimmick::Charge(void)
{
    if (m_subid == SUBID_DESTROYED)
        return;

    float interval = CGameProperty::GetElapsedTime();
    float interval_step = interval * 0.02f;
    float aimpath = m_fNearestPathT;
    float aimtimerate = GetPathTimerateByMPS(5.0f);

    if (m_pApproachingTrailerGimmickMove->IsAccelerationToAimPathT(aimpath, aimtimerate, interval_step, interval))
        SetState(STATE_POST_CHARGE);
};


void CApproachingTrailerGimmick::PostCharge(void)
{
    m_fAimPathTimerate = GetPathTimerateByMPS(5.0f);
    
    if (m_fStepTimer > 1.0f)
        SetState(STATE_CRUISE);
};


void CApproachingTrailerGimmick::Sleep(void)
{
    static float s_afAwakeTime[] =
    {
        3.0f,
        2.0f,
        1.0f,
    };

    static_assert(COUNT_OF(s_afAwakeTime) == DISTANCENUM, "should equal");

    if (m_fStepTimer > s_afAwakeTime[m_aPlayerInfo[m_iNearestPlayerNo].m_eDistance])
    {
        SetState(STATE_CRUISE);
    };

    if (m_bFlyawayRq)
    {
        float interval = CGameProperty::GetElapsedTime();
        float interval_step = interval * 0.02f;
        float aimpath = m_fFlyawayNearestPathT;
        float aimtimerate = 0.0f;
        
        if (m_pApproachingTrailerGimmickMove->IsAccelerationToAimPathT(aimpath, aimtimerate, interval_step, interval))
        {
            m_bFlyawayRq = false;
            m_fAimPathTimerate = GetPathTimerateByMPS(0.0f);
        };
    }
    else
    {
        m_fAimPathTimerate = GetPathTimerateByMPS(0.0f);
    };
};


void CApproachingTrailerGimmick::PreAttack(void)
{
    if (m_pAttack->Attach())
        SetState(STATE_ATTACK);
};


void CApproachingTrailerGimmick::Attack(void)
{
    RwV3d vShotPos = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetPosition(&vShotPos);
    vShotPos.y += 7.6f;

    m_pAttack->SetShotPos(&vShotPos);
    m_fAimPathTimerate = GetPathTimerateByMPS(m_pAttack->GetSpeed());

    if (m_pAttack->Run())
        SetState(STATE_POST_ATTACK);
};


void CApproachingTrailerGimmick::PostAttack(void)
{
    if (m_pAttack->Detach())
        SetState(STATE_CRUISE);
};


void CApproachingTrailerGimmick::Explose(void)
{
    float fScale = 1.0f;
    
    RwV3d vEffectPos = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetPosition(&vEffectPos);
    
    if (m_fStepTimer <= 0.71428573f)
    {
        fScale = 1.4f;
        
        vEffectPos.x += Math::RandFloatRange(-3.5f, 3.5f);
        vEffectPos.y += Math::RandFloatRange(0.0f, 4.0f) + 1.0f;
        vEffectPos.z += Math::RandFloatRange(-1.0f, 1.0f) + 8.0f;
    }
    else
    {
        SetState(STATE_CHANGE);
        
        fScale = 2.8f;
        vEffectPos.y += 8.0f;
    };

    uint32 hEffect = CEffectManager::Play("expl_b1", &vEffectPos);
    ASSERT(hEffect);

    if (hEffect)
        CEffectManager::SetScale(hEffect, fScale);
};


void CApproachingTrailerGimmick::Change(void)
{
    for (int32 i = 0; i < COUNT_OF(s_apszUVAnimModel); ++i)
        CMapClumpModelManager::SetDrawEnable(s_apszUVAnimModel[i], true);

    m_model.SetDrawType(CNormalGimmickModel::MODELTYPE_DRAW_BREAK);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetPosition(&vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetRotation(&vRotation);

    m_pApproachingTrailerHeadGimmickMove->SetRotation(&vRotation);
    m_pApproachingTrailerHeadGimmickMove->SetPosition(&vPosition);

    SetState(STATE_HEAD_PRE_CHARGE);
};


void CApproachingTrailerGimmick::HeadPreCharge(void)
{
    if (m_fStepTimer > 1.0f)
    {
        SetState(STATE_HEAD_CHARGE);
        m_pMotion->StartOne(1.0f);
        m_pMotion->SetPlayrate(0.5f);
    };
};


void CApproachingTrailerGimmick::HeadCharge(void)
{
    float fEndT = m_pMotion->GetEndTime();

    if ((fEndT + 2.0f) < m_fStepTimer)
    {
        SetState(STATE_HEAD_POST_CHARGE);
        CGimmickManager::PostEvent("GMSGROUP_A000", GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
    };

    m_pMotion->Update();

    if (m_fStepTimer >= fEndT)
    {
        m_fPathTimerate = 0.0f;
        m_fAimPathTimerate = 0.0f;
    }
    else
    {
        m_vHitOffset.z += (CGameProperty::GetElapsedTime() / fEndT * 33.5f);
    };
};


void CApproachingTrailerGimmick::HeadPostCharge(void)
{
    CGimmickManager::PostEvent("ATEnd", GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
    m_pMotion->Update();
    SetState(STATE_STOP);
};


void CApproachingTrailerGimmick::Stop(void)
{
    m_pMotion->Update();
    m_fAimPathTimerate = 0.0f;
    GetMoveStrategy()->Stop();
};


void CApproachingTrailerGimmick::CommonUpdate(void)
{
    //
    //  transform hit pos from obj space to world space
    //
    RwMatrix* pModeling = m_model.GetMatrix(m_model.GetDrawType());

    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vPosition, &s_vOffset, pModeling);

    Math::Vec3_Add(&vPosition, &vPosition, &m_vHitOffset);

    //
    //  regist catch
    //
    {
        RwSphere HitSphere = { vPosition, s_fHitRadius };

        CHitCatchData CatchData;
        CatchData.Cleanup();
        CatchData.SetObject(GetHandle());
        CatchData.SetObjectType(GetType());
        CatchData.SetShape(CHitCatchData::SHAPE_SPHERE);
        CatchData.SetSphere(&HitSphere);

        CHitAttackManager::RegistCatch(&CatchData);
    }
    
    //
    //  regist attack
    //
    {
        RwSphere HitSphere = { vPosition, (s_fHitRadius + 0.25f) };

        CHitAttackData AttackData;
        AttackData.Cleanup();
        AttackData.SetObject(GetHandle());
        AttackData.SetShape(CHitAttackData::SHAPE_SPHERE);
        AttackData.SetSphere(&HitSphere);
        AttackData.SetObjectPos(&vPosition);
        AttackData.SetPower(m_nPower);
        AttackData.SetTarget(CHitAttackData::TARGET_ALL_EXCEPT_SELF);
        AttackData.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        AttackData.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        AttackData.SetStatusParameter(m_fFlyawayVelXZ, m_fFlyawayVelY);

        if (GetPathTimerateByMPS(3.325f) < m_fPathTimerate)
            CHitAttackManager::RegistAttack(&AttackData);
    }

    //
    //  update model
    //
    RwV3d vMovPos = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetPosition(&vMovPos);
    
    RwV3d vMovRot = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetRotation(&vMovRot);

    m_model.SetRotation(&vMovRot);
    m_model.SetPosition(&vMovPos);
    m_model.UpdateFrame();

    //
    //  update body pos
    //
    m_pBodyHitData->SetCurrentPos(&vPosition);
    
    //
    //  update effect
    //
    EffectUpdate();

    //
    //  update light
    //
    LightUpdate();
    
    //
    //  update SE
    //
    SEControl();
};


void CApproachingTrailerGimmick::SetPlayerInfo(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    float fNearestDist = TYPEDEF::FLOAT_MAX;
    int32 iAlivePlayerNum = 0;

    RwV3d vTrailerPos = Math::VECTOR3_ZERO;
    GetMoveStrategy()->GetPosition(&vTrailerPos);

    m_iPlayerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < m_iPlayerNum; ++i)
    {
        PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[i];
        
        if (!CGameProperty::Player(i)->IsAlive())
        {
            std::memset(pPlayerInfo, 0, sizeof(*pPlayerInfo));
            continue;
        };

        ++iAlivePlayerNum;

        RwV3d vPlayerPos = Math::VECTOR3_ZERO;
        CGameProperty::Player(i)->GetPosition(&vPlayerPos);

        Math::Vec3_Add(&vPosition, &vPosition, &vPlayerPos);

        RwV3d vDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDist, &vPlayerPos, &vTrailerPos);

        pPlayerInfo->m_vPosition = vPlayerPos;
        pPlayerInfo->m_eStatus = CGameProperty::Player(i)->GetStatus();
        pPlayerInfo->m_fDistance = Math::Vec3_Length(&vDist);

        if (pPlayerInfo->m_fDistance >= 25.0f)
            pPlayerInfo->m_eDistance = DISTANCE_LONG;
        else if (pPlayerInfo->m_fDistance >= 10.0f)
            pPlayerInfo->m_eDistance = DISTANCE_MIDDLE;
        else
            pPlayerInfo->m_eDistance = DISTANCE_SHORT;

        if (pPlayerInfo->m_fDistance < fNearestDist)
        {
            fNearestDist = pPlayerInfo->m_fDistance;
            m_iNearestPlayerNo = i;
        };
    };

    if (m_iPlayerNum > 0)
    {
        std::memset(&m_playerInfoCenter, 0, sizeof(m_playerInfoCenter));

        Math::Vec3_Scale(&m_playerInfoCenter.m_vPosition, &vPosition, (1.0f / float(iAlivePlayerNum)));

        RwV3d vDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDist, &m_playerInfoCenter.m_vPosition, &vTrailerPos);

        m_playerInfoCenter.m_eStatus = PLAYERTYPES::STATUS_IDLE;
        m_playerInfoCenter.m_fDistance = Math::Vec3_Length(&vDist);

        if (m_playerInfoCenter.m_fDistance >= 25.0f)
            m_playerInfoCenter.m_eDistance = DISTANCE_LONG;
        else if (m_playerInfoCenter.m_fDistance >= 10.0f)
            m_playerInfoCenter.m_eDistance = DISTANCE_MIDDLE;
        else
            m_playerInfoCenter.m_eDistance = DISTANCE_SHORT;
    };
};


void CApproachingTrailerGimmick::SEControl(void)
{
    //
    //  TODO app trailer SE ctrl
    //
};


void CApproachingTrailerGimmick::SetState(STATE eState)
{
    switch (eState)
    {
    case STATE_WAIT:
        break;

    case STATE_CRUISE:
        break;

    case STATE_PRE_CHARGE:
        {
            m_fAimPathTimerate = 0.0f;
            CGameSound::PlayObjectSE(this, SDCODE_SE(4223));
        }
        break;

    case STATE_CHARGE:
        {
            static float s_afDistOfsZ[] =
            {
                13.0f,
                22.0f,
                29.0f,
            };

            static_assert(COUNT_OF(s_afDistOfsZ) == DISTANCENUM, "should equal");

            RwV3d vPosition = m_playerInfoCenter.m_vPosition;
            vPosition.z += s_afDistOfsZ[m_playerInfoCenter.m_eDistance];

            m_fNearestPathT = m_pApproachingTrailerGimmickMove->GetNearestPathT(&vPosition);
            m_fAimPathTimerate = GetPathTimerateByMPS(20.0f);
        }
        break;

    case STATE_POST_CHARGE:
        {
            ASSERT(m_eState == STATE_CHARGE);
        }
        break;

    case STATE_SLEEP:
        LightSet(0.0f, true);
        break;

    case STATE_PRE_ATTACK:
        m_fAimAttackPathT = m_pApproachingTrailerGimmickMove->GetNearestPathT(&m_playerInfoCenter.m_vPosition);
        break;

    case STATE_ATTACK:
        ASSERT(m_eState == STATE_PRE_ATTACK);
        break;

    case STATE_POST_ATTACK:
        ASSERT(m_eState == STATE_ATTACK);
        break;

    case STATE_EXPLOSE:
        {
            m_bSEPlay = false;
            CGameSound::PlayObjectSE(this, SDCODE_SE(4249));
            CMessageManager::Request(SEGROUPID::VALUE(109));
        }
        break;

    case STATE_CHANGE:
        ASSERT(m_eState == STATE_EXPLOSE);
        break;

    case STATE_HEAD_PRE_CHARGE:
        {
            ASSERT(m_eState == STATE_CHANGE);
            CGameSound::PlayObjectSE(this, SDCODE_SE(4259));
        }
        break;

    case STATE_HEAD_CHARGE:
        ASSERT(m_eState == STATE_HEAD_PRE_CHARGE);
        break;

    case STATE_HEAD_POST_CHARGE:
        ASSERT(m_eState == STATE_HEAD_CHARGE);
        break;

    case STATE_STOP:
        ASSERT(m_eState == STATE_HEAD_POST_CHARGE);
        break;

    default:
        break;
    };

    m_eState = eState;
    m_fStepTimer = 0.0f;
};


void CApproachingTrailerGimmick::EffectCreate(void)
{
    RwMatrix* pModeling = m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

    //
    //  pipe smoke effect
    //
    for (int32 i = 0; i < COUNT_OF(m_ahEffect) - 2; ++i)
    {
        RwV3d vEffectPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vEffectPos, &s_avATEffectOffset[i], pModeling);

        m_ahEffect[i] = CEffectManager::Play(EFFECTID::ID_BLACKSMOKE, &vEffectPos);
        ASSERT(m_ahEffect[i]);
    };
    
    //
    //  wheel smoke effect
    //
    for (int32 i = 2; i < COUNT_OF(m_ahEffect); ++i)
        m_ahEffect[i] = 0;
};


void CApproachingTrailerGimmick::EffectDestroy(void)
{
    for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
    {
        if (m_ahEffect[i])
        {
            CEffectManager::Finish(m_ahEffect[i]);
            m_ahEffect[i] = 0;
        };
    };
};


void CApproachingTrailerGimmick::EffectUpdate(void)
{
    RwMatrix* pModeling = m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

    //
    //  pipe smoke effect
    //
    for (int32 i = 0; i < COUNT_OF(m_ahEffect) - 2; ++i)
    {
        if (!m_ahEffect[i])
            continue;
        
        RwV3d vEffectPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vEffectPos, &s_avATEffectOffset[i], pModeling);

        CEffectManager::SetPosition(m_ahEffect[i], &vEffectPos);
    };

    //
    //  wheel smoke effect
    //
    if ((m_subid == SUBID_DESTROYED) || (m_eState > STATE_WAIT))
    {
        m_fEffectTimer += CGameProperty::GetElapsedTime();
        if (m_fEffectTimer > 0.1f)
        {
            m_fEffectTimer = 0.0f;
            
            for (int32 i = 2; i < COUNT_OF(m_ahEffect); ++i)
            {
                RwV3d vEffectPos = Math::VECTOR3_ZERO;
                RwV3dTransformPoint(&vEffectPos, &s_avATEffectOffset[i], pModeling);

                uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DASH2, &vEffectPos);
                ASSERT(hEffect);
            };
        };
    };
};


void CApproachingTrailerGimmick::LightCreate(void)
{
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pApproachingTrailerGimmickMove->GetPosition(&vPosition);

    for (int32 i = 0; i < COUNT_OF(m_aLight); ++i)
    {
        m_aLight[i].m_hLight = CEffectLightManager::Regist(&vPosition, 2.0f, { 0xFF, 0xFF, 0xFF, 0xFF });
        ASSERT(m_aLight[i].m_hLight);
    };

	if (m_subid == SUBID_NORMAL)
		LightSet(0.5f, true);
};


void CApproachingTrailerGimmick::LightDestroy(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLight); ++i)
    {
        ASSERT(m_aLight[i].m_hLight);
        CEffectLightManager::Remove(m_aLight[i].m_hLight);
        m_aLight[i].m_hLight = 0;
    };
};


void CApproachingTrailerGimmick::LightUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLight); ++i)
    {
        LIGHT* pLight = &m_aLight[i];

        //
        //  update light pos
        //
        RwV3d vLightPos = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vLightPos, &s_avATLightOffset[i], m_model.GetMatrix(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL));

        CEffectLightManager::SetPosition(pLight->m_hLight, &vLightPos);

		if (!pLight->m_bRqUpd)
			continue;

        //
        //  update light color
        //
        if (pLight->m_fTimerCounter >= 0.0f)
        {
            pLight->m_fTimerCounter -= CGameProperty::GetElapsedTime();
            float progress = Clamp(pLight->m_fTimerCounter / pLight->m_fTime, 0.0f, 1.0f);
            CEffectLightManager::SetColor(pLight->m_hLight, { 0xFF, 0xFF, 0xFF, RwUInt8(progress * 255.0f) });
        }
        else
        {
            CEffectLightManager::SetColor(pLight->m_hLight, { 0xFF, 0xFF, 0xFF, RwUInt8(pLight->m_bRqOff ? 0x00 : 0xFF) });
            pLight->m_bRqUpd = false;
            pLight->m_fTime = 0.0f;
            pLight->m_fTimerCounter = 0.0f;
        };
    };
};


void CApproachingTrailerGimmick::LightSet(float fTime, bool bOff)
{
    for (int32 i = 0; i < COUNT_OF(m_aLight); ++i)
    {
        m_aLight[i].m_fTime = fTime;
        m_aLight[i].m_fTimerCounter = fTime;
        m_aLight[i].m_bRqUpd = true;
        m_aLight[i].m_bRqOff = bOff;
    };
};


RwV3d* CApproachingTrailerGimmick::GetPlayerPos(int32 iPlayerNo)
{
    return (iPlayerNo >= COUNT_OF(m_aPlayerInfo) ? &m_playerInfoCenter.m_vPosition : &m_aPlayerInfo[iPlayerNo].m_vPosition);
};


float CApproachingTrailerGimmick::RateControl(float fCur, float fAim, float dt) const
{
    float fDlt = std::fabs(fCur - fAim);
    
    if (fDlt >= dt)
        return (fCur >= fAim ? (fCur - dt) : (fCur + dt));
    else
        return fAim;
};


float CApproachingTrailerGimmick::GetPathTimerateByMPS(float fMPS) const
{
    float fDlt = m_pApproachingTrailerGimmickMove->GetPathTByMeterPerSecond(fMPS) - m_pApproachingTrailerGimmickMove->GetPathT();
    return (fDlt / CGameProperty::GetElapsedTime());
};


void CApproachingTrailerGimmick::GetFlyawayPosition(RwV3d* pvResult, const RwV3d* pvPlayerPos, const RwV3d* pvDir) const
{
    float dt = CGameProperty::GetElapsedTime();
    float g = CGameProperty::GetGravity();
    float t = 0.0f;
    RwV3d vPos = *pvPlayerPos;
    float fVelY = m_fFlyawayVelY;
    
    while (vPos.y > -16.0f)
    {
        vPos.y += (fVelY * dt);
        fVelY += (g * dt);
        t += dt;

        if (t > 10.0f)
            ASSERT(false);
    };

    RwV3d vDir = *pvDir;
    vDir.y = 0.0f;

    pvResult->x = (vPos.x + (vDir.x * (t * m_fFlyawayVelXZ)));
    pvResult->y = (vDir.y - 16.0f);
    pvResult->z = (vPos.z + (vDir.z * (t * m_fFlyawayVelXZ)));
};