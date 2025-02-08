#include "RideCharacterEffectModule.hpp"

#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Map/WorldMap.hpp"


class CRideEffectUnit : public CListNode<CRideEffectUnit>
{
protected:
    enum STEP
    {
        STEP_APPEAR = 0,
        STEP_IDLE,
        STEP_DISAPPEAR,
        STEP_REVUP,
    };
    
public:
    CRideEffectUnit(void);
    virtual ~CRideEffectUnit(void);
    virtual void Run(void);
    void Initialize(CRideCharacterEffectModule::RIDERTYPE ridertype, CPlayerCharacter* pPlayerChr, int32 nBoneID, EFFECTID::VALUE idEffect, const RwV3d* pvOffset);
    void SetDrawEnable(bool bEnable);
    void SetScale(float fScale);
    void Appear(void);
    void Idle(void);
    void Disappear(void);
    void RevUp(void);

protected:
    CRideCharacterEffectModule::RIDERTYPE m_ridertype;
    uint32 m_hEffect;
    EFFECTID::VALUE m_idEffect;
    CPlayerCharacter* m_pPlayerChr;
    int32 m_nBoneID;
    RwV3d m_vOffset;
    STEP m_step;
    float m_fTime;
    float m_fPower;
    float m_fMark;
    float m_fChangeTime;
};


class CRideExhaustUnit : public CRideEffectUnit
{
public:
    CRideExhaustUnit(void);
    virtual ~CRideExhaustUnit(void);
    virtual void Run(void) override;
};


class CBoardEffectModule : public CRideCharacterEffectModule
{
public:
    CBoardEffectModule(CPlayerCharacter* pPlayerChr);
    virtual ~CBoardEffectModule(void);

private:
    CRideEffectUnit m_aRideEffectUnit[3];
    CRideExhaustUnit m_ExhaustUnit;
};


class CSpaceShipEffectModule : public CRideCharacterEffectModule
{
public:
    CSpaceShipEffectModule(CPlayerCharacter* pPlayerChr);
    virtual ~CSpaceShipEffectModule(void);

private:
    CRideEffectUnit m_aRideEffectUnit[2];
};


CRideEffectUnit::CRideEffectUnit(void)
: m_ridertype(CRideCharacterEffectModule::RIDERTYPE_BOARD)
, m_hEffect(0)
, m_idEffect(EFFECTID::ID_UNKNOWN)
, m_pPlayerChr(nullptr)
, m_nBoneID(-1)
, m_vOffset(Math::VECTOR3_ZERO)
, m_step(STEP_APPEAR)
, m_fTime(0.0f)
, m_fPower(1.0f)
, m_fMark(1.0f)
, m_fChangeTime(0.0f)
{
    ;
};


CRideEffectUnit::~CRideEffectUnit(void)
{
    ;
};


void CRideEffectUnit::Run(void)
{
    switch (m_step)
    {
    case STEP_APPEAR:
        Appear();
        break;

    case STEP_IDLE:
        Idle();
        break;

    case STEP_DISAPPEAR:
        Disappear();
        break;

    case STEP_REVUP:
        RevUp();
        break;

    default:
        ASSERT(false);
        break;
    };

    m_fTime += CGameProperty::GetElapsedTime();

    CModel* pModel = m_pPlayerChr->GetModel();
    ASSERT(pModel);

    RwMatrix* pMatrix = pModel->GetBoneMatrixFromID(m_nBoneID);
    ASSERT(pMatrix);

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    std::memcpy(&matrix, pMatrix, sizeof(matrix));

    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vPosition, &m_vOffset, &matrix);

    RwV3d vDirection = Math::VECTOR3_ZERO;

    switch (m_ridertype)
    {
    case CRideCharacterEffectModule::RIDERTYPE_BOARD:
        {
            vDirection = pMatrix->up;
            Math::Vec3_Normalize(&vDirection, &vDirection);
            Math::Vec3_Scale(&vDirection, &vDirection, -1.0f);
        }
        break;

    case CRideCharacterEffectModule::RIDERTYPE_SHIP:
        {
            vDirection = pMatrix->at;
            Math::Vec3_Normalize(&vDirection, &vDirection);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_hEffect)
    {
        CEffectManager::SetPosition(m_hEffect, &vPosition);
        CEffectManager::SetDirection(m_hEffect, &vDirection);
    };
};


void CRideEffectUnit::Initialize(CRideCharacterEffectModule::RIDERTYPE ridertype, CPlayerCharacter* pPlayerChr, int32 nBoneID, EFFECTID::VALUE idEffect, const RwV3d* pvOffset)
{
    m_ridertype = ridertype;
    m_nBoneID = nBoneID;
    m_idEffect = idEffect;
    m_pPlayerChr = pPlayerChr;
    m_vOffset = *pvOffset;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pPlayerChr->GetFootPosition(&vPosition);

    m_hEffect = CEffectManager::Play(m_idEffect, &vPosition, 0.0f);
    ASSERT(m_hEffect);
};


void CRideEffectUnit::SetDrawEnable(bool bEnable)
{
    if (m_hEffect)
        CEffectManager::SetActive(m_hEffect, bEnable);
};


void CRideEffectUnit::SetScale(float fScale)
{
    if (m_hEffect)
        CEffectManager::SetScale(m_hEffect, fScale);
};


void CRideEffectUnit::Appear(void)
{
    if (m_fTime <= m_fChangeTime)
    {
        if (m_fTime > 0.0f)
        {
            float fActual = m_fMark / m_fChangeTime * m_fTime;
            SetScale(fActual / m_fPower);
            m_fPower = fActual;
        };
    }
    else
    {
        m_step = STEP_IDLE;
    };
};


void CRideEffectUnit::Idle(void)
{
    ;
};


void CRideEffectUnit::Disappear(void)
{
    if (m_fTime <= m_fChangeTime)
    {
        if (m_fTime > 0.0f)
        {
            float fActual = (m_fMark / m_fChangeTime * (m_fChangeTime - m_fTime));
            SetScale(fActual / m_fPower);
            m_fPower = fActual;
        };
    }
    else
    {
        m_step = STEP_IDLE;
    };
};


void CRideEffectUnit::RevUp(void)
{
    if (m_fTime <= m_fChangeTime)
    {
        if (m_fTime > 0.0f)
        {
            float fActual = (m_fMark / m_fChangeTime * (m_fChangeTime - m_fTime)) + 1.0f;
            SetScale(fActual / m_fPower);
            m_fPower = fActual;
        };
    }
    else
    {
        m_step = STEP_IDLE;
    };
};


//
// *********************************************************************************
//


CRideExhaustUnit::CRideExhaustUnit(void)
{
    ;
};


CRideExhaustUnit::~CRideExhaustUnit(void)
{
    ;
};


void CRideExhaustUnit::Run(void)
{
    CModel* pModel = m_pPlayerChr->GetModel();
    ASSERT(pModel);
    
    RwV3d* pvPosition = pModel->GetBonePositionFromID(m_nBoneID);
    ASSERT(pvPosition);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vPosition, pvPosition, &m_vOffset);

    RwV3d vPtStart  = { vPosition.x,    vPosition.y,           vPosition.z };
    RwV3d vPtEnd    = { vPosition.x,    vPosition.y - 1.0f,    vPosition.z };

    if (CWorldMap::CheckCollisionLine(&vPtStart, &vPtEnd))
    {
        const CWorldMap::COLLISIONRESULT* pResult = CWorldMap::GetCollisionResult();
        ASSERT(pResult);

        SetDrawEnable(true);
        if (m_hEffect)
            CEffectManager::SetPosition(m_hEffect, &pResult->m_vClosestPt);        
    }
    else
    {
        SetDrawEnable(false);        
    };

    PLAYERTYPES::STATUS status = m_pPlayerChr->GetStatus();
    ASSERT(status >= PLAYERTYPES::RIDESTART);
    ASSERT(status < PLAYERTYPES::RIDEMAX);

    if (status == PLAYERTYPES::RIDESTATUS_CRASH_WALL)
        SetDrawEnable(false);    
};


//
// *********************************************************************************
//


CBoardEffectModule::CBoardEffectModule(CPlayerCharacter* pPlayerChr)
: CRideCharacterEffectModule(pPlayerChr, RIDERTYPE_BOARD)
{
    static const RwV3d RIDE_BOARD_LEFTWING  = { -0.47f,  0.44f,  -0.026f };
    static const RwV3d RIDE_BOARD_RIGHTWING = {  0.47f,  0.44f,  -0.026f };
    static const RwV3d RIDE_BOARD_CENTER    = {  0.0f,   1.65f,   0.2f    };
    static const RwV3d RIDE_EXHAUST         = {  0.0f,   0.0f,    1.0f    };

    m_aRideEffectUnit[0].Initialize(m_ridertype, m_pPlayerChr, CHARACTERTYPES::BONEID_RIDE_WING, EFFECTID::ID_RIDE_PARTICLE, &RIDE_BOARD_LEFTWING);
    m_aRideEffectUnit[1].Initialize(m_ridertype, m_pPlayerChr, CHARACTERTYPES::BONEID_RIDE_WING, EFFECTID::ID_RIDE_PARTICLE, &RIDE_BOARD_RIGHTWING);
    m_aRideEffectUnit[2].Initialize(m_ridertype, m_pPlayerChr, CHARACTERTYPES::BONEID_RIDE_WING, EFFECTID::ID_RIDE_ENGINE, &RIDE_BOARD_CENTER);
    m_ExhaustUnit.Initialize(m_ridertype, m_pPlayerChr, CHARACTERTYPES::BONEID_RIDE_WING, EFFECTID::ID_RIDE_WIND, &RIDE_EXHAUST);
    
    RegistUnit(&m_aRideEffectUnit[0]);
    RegistUnit(&m_aRideEffectUnit[1]);
    RegistUnit(&m_aRideEffectUnit[2]);
    RegistUnit(&m_ExhaustUnit);
};


CBoardEffectModule::~CBoardEffectModule(void)
{
    ;
};


//
// *********************************************************************************
//


CSpaceShipEffectModule::CSpaceShipEffectModule(CPlayerCharacter* pPlayerChr)
: CRideCharacterEffectModule(pPlayerChr, RIDERTYPE_SHIP)
{
    static const RwV3d RIDE_SPACESHIP_LEFTJET   = { -0.152f, 0.04f, -0.8f };
    static const RwV3d RIDE_SPACESHIP_RIGHTJET  = {  0.152f, 0.04f, -0.8f };

    m_aRideEffectUnit[0].Initialize(m_ridertype, m_pPlayerChr, CHARACTERTYPES::BONEID_BODY, EFFECTID::ID_RIDE_ENGINE, &RIDE_SPACESHIP_LEFTJET);
    m_aRideEffectUnit[1].Initialize(m_ridertype, m_pPlayerChr, CHARACTERTYPES::BONEID_BODY, EFFECTID::ID_RIDE_ENGINE, &RIDE_SPACESHIP_RIGHTJET);

    RegistUnit(&m_aRideEffectUnit[0]);
    RegistUnit(&m_aRideEffectUnit[1]);
};


CSpaceShipEffectModule::~CSpaceShipEffectModule(void)
{
    ;
};


/*static*/ CRideCharacterEffectModule* CRideCharacterEffectModule::New(CPlayerCharacter* pPlayerChr, RIDERTYPE ridertype)
{
    ASSERT(pPlayerChr);

    CRideCharacterEffectModule* pRet = nullptr;

    switch (ridertype)
    {
    case RIDERTYPE_BOARD:
        pRet = new CBoardEffectModule(pPlayerChr);
        break;

    case RIDERTYPE_SHIP:
        pRet = new CSpaceShipEffectModule(pPlayerChr);
        break;

    default:
        ASSERT(false);
        break;
    };

    ASSERT(pRet);
    return pRet;
};


//
// *********************************************************************************
//


CRideCharacterEffectModule::CRideCharacterEffectModule(CPlayerCharacter* pPlayerChr, RIDERTYPE ridertype)
: IModule(MODULETYPE::RIDE_EFFECT)
, m_pPlayerChr(pPlayerChr)
, m_ridertype(ridertype)
, m_bDrawFlag(false)
{
    ASSERT(m_pPlayerChr);
};


CRideCharacterEffectModule::~CRideCharacterEffectModule(void)
{
    ;
};


void CRideCharacterEffectModule::Run(void)
{
    for (CRideEffectUnit& it : m_listRideEffectUnit)
        it.Run();
};


void CRideCharacterEffectModule::Draw(void)
{
    ;
};


void CRideCharacterEffectModule::SetDrawEnable(bool bEnable)
{
    m_bDrawFlag = bEnable;

    for (CRideEffectUnit& it : m_listRideEffectUnit)
        it.SetDrawEnable(m_bDrawFlag);
};


void CRideCharacterEffectModule::RegistUnit(CRideEffectUnit* pUnit)
{
    ASSERT(pUnit);
    m_listRideEffectUnit.push_back(pUnit);
};