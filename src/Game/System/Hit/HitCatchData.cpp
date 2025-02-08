#include "HitCatchData.hpp"
#include "HitAttackData.hpp"

#include "Game/System/GameObject/GameObjectManager.hpp"


CHitCatchData::CHitCatchData(void)
: m_bNewAllocated(true)
{
    Cleanup();
};


CHitCatchData::~CHitCatchData(void)
{
    ;
};


void CHitCatchData::Cleanup(void)
{
    m_uObjectHandle         = 0;
    m_ObjectType            = GAMEOBJECTTYPE::CHARACTER;
    m_nCatchNo              = 0;
    m_shapeType             = SHAPE_SPHERE;
    m_shape.m_sphere        = { { Math::VECTOR3_ZERO }, 0.0f };
    m_result                = RESULT_INVALID;
    m_bCounter              = false;
    m_bIsHitPosCalculated   = false;
    m_bAlive                = true;
    m_vHitPos               = Math::VECTOR3_ZERO;
    m_pHitAttackData        = nullptr;
};


void CHitCatchData::CopyData(CHitCatchData* pHitCatchData) const
{
    pHitCatchData->m_uObjectHandle  = m_uObjectHandle;
    pHitCatchData->m_ObjectType     = m_ObjectType;
    pHitCatchData->m_nCatchNo       = m_nCatchNo;
    pHitCatchData->m_shapeType      = m_shapeType;
    std::memcpy(&pHitCatchData->m_shape, &m_shape, sizeof(m_shape));    
    pHitCatchData->m_result         = m_result;
    pHitCatchData->m_bCounter       = m_bCounter;
    pHitCatchData->m_pHitAttackData = m_pHitAttackData;
};


void CHitCatchData::SetShape(SHAPE shape)
{
    m_shapeType = shape;
};


void CHitCatchData::SetSphere(const RwSphere* pSphere)
{
    ASSERT(m_shapeType == SHAPE_SPHERE);
    ASSERT(pSphere);

    m_shape.m_sphere = *pSphere;
};


void CHitCatchData::SetObject(uint32 hObj)
{
    ASSERT(hObj);
    
    m_uObjectHandle = hObj;
};


void CHitCatchData::SetObjectType(GAMEOBJECTTYPE::VALUE objtype)
{
    m_ObjectType = objtype;
};


void CHitCatchData::SetCatchNo(int32 no)
{
    m_nCatchNo = no;
};


void CHitCatchData::SetFlagCounter(bool bSet)
{
    m_bCounter = bSet;
};


void CHitCatchData::SetFlagAllocated(bool bSet)
{
    m_bNewAllocated = bSet;
};


void CHitCatchData::SetFlagAlive(bool bSet)
{
    m_bAlive = bSet;
};


void CHitCatchData::SetFlagHitPosCalculated(bool bSet)
{
    m_bIsHitPosCalculated = bSet;
};


void CHitCatchData::SetAttack(CHitAttackData* pAttack)
{
    m_pHitAttackData = pAttack;
};


void CHitCatchData::SetResult(RESULT result)
{
    m_result = result;
};


CHitCatchData::SHAPE CHitCatchData::GetShape(void) const
{
    return m_shapeType;
};


const RwSphere* CHitCatchData::GetSphere(void) const
{
    ASSERT(m_shapeType == SHAPE_SPHERE);

    return &m_shape.m_sphere;
};


CGameObject* CHitCatchData::GetObject(void) const
{
    return CGameObjectManager::GetObject(m_uObjectHandle);
};


uint32 CHitCatchData::GetObjectHandle(void) const
{
    return m_uObjectHandle;
};


GAMEOBJECTTYPE::VALUE CHitCatchData::GetObjectType(void) const
{
    return m_ObjectType;
};


int32 CHitCatchData::GetCatchNo(void) const
{
    return m_nCatchNo;
};


bool CHitCatchData::IsFlagCounter(void) const
{
    return m_bCounter;
};


bool CHitCatchData::IsFlagAllocated(void) const
{
    return m_bNewAllocated;
};


bool CHitCatchData::IsFlagAlive(void) const
{
    return m_bAlive;
};


CHitAttackData* CHitCatchData::GetAttack(void) const
{
    return m_pHitAttackData;
};


const RwV3d* CHitCatchData::GetHitPos(void)
{
    if (m_bIsHitPosCalculated)
        return &m_vHitPos;

    ASSERT(m_pHitAttackData);

    switch (m_pHitAttackData->GetShape())
    {
    case CHitAttackData::SHAPE_SPHERE:
        {
            const RwSphere* pSphere = m_pHitAttackData->GetSphere();

            Math::Vec3_Sub(&m_vHitPos, &pSphere->center, &m_shape.m_sphere.center);
            Math::Vec3_Multiply(
                &m_vHitPos,
                &m_vHitPos,
                m_shape.m_sphere.radius / (pSphere->radius + m_shape.m_sphere.radius)
            );
            Math::Vec3_Add(&m_vHitPos, &m_vHitPos, &m_shape.m_sphere.center);
        }
        break;

    case CHitAttackData::SHAPE_LINE:
        {
            const RwLine* pLine = m_pHitAttackData->GetLine();
            float fDist = m_pHitAttackData->GetHitDistance();
            fDist = Clamp(fDist, 0.0f, 1.0f);

            Math::Vec3_Sub(&m_vHitPos, &pLine->end, &pLine->start);
            Math::Vec3_Multiply(&m_vHitPos, &m_vHitPos, fDist);
            Math::Vec3_Add(&m_vHitPos, &m_vHitPos, &pLine->start);
        }
        break;

    default:        
		ASSERT(false);
        break;
    };

    m_bIsHitPosCalculated = true;
    
    return &m_vHitPos;
};


CHitCatchData::RESULT CHitCatchData::GetResult(void) const
{
    return m_result;
};