#include "HitAttackData.hpp"
#include "HitCatchData.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"


CHitAttackData::CHitAttackData(void)
: m_bNewAllocated(true)
{
    Cleanup();
};


CHitAttackData::~CHitAttackData(void)
{
    ;
};


void CHitAttackData::Cleanup(void)
{
    m_uObjectHandle       = 0;
    m_posObj              = Math::VECTOR3_ZERO;
    m_target              = TARGET_NONE;
    m_nAttackNo           = 0;
    m_nPower              = 0;
    m_szMotion[0]         = '\0';
    m_shapeType           = SHAPE_SPHERE;
    std::memset(&m_shape, 0x00, sizeof(m_shape));    
    m_antiguard           = ANTIGUARD_ENABLE;
    m_status              = STATUS_KNOCK;
    m_effect              = EFFECT_NONE;
    m_afStatusParametr[0] = 0.0f;
    m_afStatusParametr[1] = 0.0f;
    m_fHitDist            = 0.0f;
    m_pHitCatchData       = nullptr;
    m_bConfusion          = false;
    m_bSlice              = false;
    m_bGuardImpact        = false;
    m_bReflectShot        = false;
    m_bAlive              = true;
};


void CHitAttackData::CopyData(CHitAttackData* pHitAttackData) const
{
    pHitAttackData->m_uObjectHandle       = m_uObjectHandle;
    pHitAttackData->m_posObj              = m_posObj;
    pHitAttackData->m_target              = m_target;
    pHitAttackData->m_nAttackNo           = m_nAttackNo;
    pHitAttackData->m_nPower              = m_nPower;
    std::strcpy(pHitAttackData->m_szMotion, m_szMotion);
    pHitAttackData->m_shapeType           = m_shapeType;
    std::memcpy(&pHitAttackData->m_shape, &m_shape, sizeof(m_shape));
    pHitAttackData->m_antiguard           = m_antiguard;
    pHitAttackData->m_status              = m_status;
    pHitAttackData->m_effect              = m_effect;
    pHitAttackData->m_afStatusParametr[0] = m_afStatusParametr[0];
    pHitAttackData->m_afStatusParametr[1] = m_afStatusParametr[1];
    pHitAttackData->m_fHitDist            = m_fHitDist;
    pHitAttackData->m_pHitCatchData       = m_pHitCatchData;
    pHitAttackData->m_bConfusion          = m_bConfusion;
    pHitAttackData->m_bSlice              = m_bSlice;
    pHitAttackData->m_bGuardImpact        = m_bGuardImpact;
    pHitAttackData->m_bReflectShot        = m_bReflectShot;
};


void CHitAttackData::SetShape(SHAPE shape)
{
    m_shapeType = shape;
};


void CHitAttackData::SetSphere(const RwSphere* pSphere)
{
    ASSERT(m_shapeType == SHAPE_SPHERE);
    ASSERT(pSphere);
    
    m_shape.m_sphere = *pSphere;
};


void CHitAttackData::SetLine(const RwLine* pLine)
{
    ASSERT(m_shapeType == SHAPE_LINE);
    ASSERT(pLine);

    m_shape.m_line = *pLine;
};


void CHitAttackData::SetObject(uint32 hObj)
{
    ASSERT(hObj != 0);
    
    m_uObjectHandle = hObj;
};


void CHitAttackData::SetObjectPos(const RwV3d* pPos)
{
    ASSERT(pPos);
    
    m_posObj = *pPos;
};


void CHitAttackData::SetTarget(TARGET target)
{
    m_target = target;
    if (!m_target)
        m_target = TARGET_CHARACTER_GIMMICK;
};


void CHitAttackData::SetAttackNo(int32 no)
{
    m_nAttackNo = no;
};


void CHitAttackData::SetMotion(const char* pszMotion)
{
    ASSERT(pszMotion);
    ASSERT(std::strlen(pszMotion) < sizeof(m_szMotion));
    
    std::strcpy(m_szMotion, pszMotion);
};


void CHitAttackData::SetPower(int32 amount)
{
    m_nPower = amount;
};


void CHitAttackData::SetAntiguard(ANTIGUARD antiguard)
{
    ASSERT((antiguard == ANTIGUARD_ENABLE) ||
           (antiguard == ANTIGUARD_BREAK)  ||
           (antiguard == ANTIGUARD_INVALID));

    m_antiguard = antiguard;
};


void CHitAttackData::SetStatus(STATUS status)
{
    m_status = status;
};


void CHitAttackData::SetStatusParameter(float param1, float param2)
{
    switch (m_status)
    {
    case STATUS_FLYAWAY:
        SetFlyawayParameter(param1, param2);
        break;

    case STATUS_STUN:
    case STATUS_DINDLE:
    case STATUS_SLEEP:
    case STATUS_FREEZE:
    case STATUS_BIND:
        SetTroubleParameter(param1);
        if (param1 == 0.0f)
            SetTroubleParameter(2.0f);        
        break;

    default:
        break;
    };
};


void CHitAttackData::SetFlyawayParameter(float velXZ, float velY)
{
    ASSERT(m_status == STATUS_FLYAWAY);

    m_afStatusParametr[0] = -velXZ;
    m_afStatusParametr[1] = velY;
};


void CHitAttackData::SetTroubleParameter(float duration)
{
    ASSERT(IsTroubleAttack() == true);

    m_afStatusParametr[0] = duration;
};


void CHitAttackData::SetFlagConfusion(bool bSet)
{
    m_bConfusion = bSet;
};


void CHitAttackData::SetFlagSlice(bool bSet)
{
    m_bSlice = bSet;
};


void CHitAttackData::SetFlagGuardImpact(bool bSet)
{
    m_bGuardImpact = bSet;
};


void CHitAttackData::SetFlagReflectShot(bool bSet)
{
    m_bReflectShot = bSet;
};


void CHitAttackData::SetFlagAllocated(bool bSet)
{
    m_bNewAllocated = bSet;
};


void CHitAttackData::SetFlagAlive(bool bSet)
{
    m_bAlive = bSet;
};


void CHitAttackData::SetEffect(EFFECT effect)
{
    m_effect = effect;
};


void CHitAttackData::SetHitDistance(float dist)
{
    m_fHitDist = dist;
};


void CHitAttackData::SetCatch(CHitCatchData* pCatch)
{
    m_pHitCatchData = pCatch;
};


CHitAttackData::SHAPE CHitAttackData::GetShape(void) const
{
    return m_shapeType;
};


const RwSphere* CHitAttackData::GetSphere(void) const
{
    ASSERT(m_shapeType == SHAPE_SPHERE);
    
    return &m_shape.m_sphere;
};


const RwLine* CHitAttackData::GetLine(void) const
{
    ASSERT(m_shapeType == SHAPE_LINE);

    return &m_shape.m_line;
};


CGameObject* CHitAttackData::GetObject(void) const
{
    return CGameObjectManager::GetObject(m_uObjectHandle);
};


CGameObject* CHitAttackData::GetCatchObject(void) const
{
    return m_pHitCatchData->GetObject();
};


uint32 CHitAttackData::GetObjectHandle(void) const
{
    return m_uObjectHandle;
};


const RwV3d* CHitAttackData::GetObjectPos(void) const
{
    return &m_posObj;
};


CHitAttackData::TARGET CHitAttackData::GetTarget(void) const
{
    return m_target;
};


int32 CHitAttackData::GetAttackNo(void) const
{
    return m_nAttackNo;
};


const char* CHitAttackData::GetMotion(void) const
{
    return m_szMotion;
};


int32 CHitAttackData::GetPower(void) const
{
    return m_nPower;
};


CHitAttackData::ANTIGUARD CHitAttackData::GetAntiguard(void) const
{
    return m_antiguard;
};


CHitAttackData::STATUS CHitAttackData::GetStatus(void) const
{
    return m_status;
};


float CHitAttackData::GetStatusDuration(void) const
{
    ASSERT(IsTroubleAttack() == true);

    return m_afStatusParametr[0];
};


float CHitAttackData::GetFlyawayVelXZ(void) const
{
    return m_afStatusParametr[0];
};


float CHitAttackData::GetFlyawayVelY(void) const
{
    return m_afStatusParametr[1];
};


bool CHitAttackData::IsTroubleAttack(void) const
{
    return ((m_status == STATUS_STUN)   ||
            (m_status == STATUS_DINDLE) ||
            (m_status == STATUS_SLEEP)  ||
            (m_status == STATUS_FREEZE) ||
            (m_status == STATUS_BIND));
};


bool CHitAttackData::IsFlagConfusion(void) const
{
    return m_bConfusion;
};


bool CHitAttackData::IsFlagSlice(void) const
{
    return m_bSlice;
};


bool CHitAttackData::IsFlagGuardImpact(void) const
{
    return m_bGuardImpact;
};


bool CHitAttackData::IsFlagReflectShot(void) const
{
    return m_bReflectShot;
};


bool CHitAttackData::IsFlagAllocated(void) const
{
    return m_bNewAllocated;
};


bool CHitAttackData::IsFlagAlive(void) const
{
    return m_bAlive;
};


CHitAttackData::EFFECT CHitAttackData::GetEffect(void) const
{
    return m_effect;
};


float CHitAttackData::GetHitDistance(void) const
{
    return m_fHitDist;
};


CHitCatchData* CHitAttackData::GetCatch(void) const
{
    return m_pHitCatchData;
};


const RwV3d* CHitAttackData::GetHitPos(void) const
{
    return m_pHitCatchData->GetHitPos();
};

