#include "MadCarGimmick.hpp"
#include "MadCarGimmickManager.hpp"

#include "Game/Component/Effect/Light.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/MadCarGimmickMove.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"


struct MADCAR_SUBINFO
{
    const char* m_pszModelName;
    RwV3d m_vAttackPoint[2];
    float m_fAttackRadius;
    int32 m_nAttackPower;
    RwV3d m_vLightPos;
    float m_fLightRadius;
};


static const MADCAR_SUBINFO s_aMadCarSubinfo[] =
{
    { "d21",    { { 0.0f, 0.15f, 2.0f }, { 0.0f, 0.15f, 0.5f } },   1.3f, 25, { 0.9f,           0.91f,          3.1575201f  },  0.8f },
    { "devil",  { { 0.0f, -0.1f, 3.0f }, { 0.0f, -0.1f, 0.3f } },   1.5f, 25, { 0.91597998f,    0.73869997f,    3.99037f    },  0.8f },
    { "imp59",  { { 0.0f, -0.1f, 2.9f }, { 0.0f, -0.1f, 0.7f } },   1.5f, 25, { 1.10531f,       0.77525002f,    4.0164399f  },  0.8f },
    { "mstu",   { { 0.0f, -0.7f, 1.9f }, { 0.0f, -0.7f, -0.2f } },  1.6f, 25, { 0.85887998f,    0.67212999f,    2.8431201f  },  0.8f },
};


static_assert(COUNT_OF(s_aMadCarSubinfo) == (CMadCarGimmick::SUBID_NUM - CMadCarGimmick::SUBID_START_LIGHTOFF), "update me");


CMadCarGimmick::CCarHeadLight::CCarHeadLight(void)
: m_hLight(0)
{
    ;
};


CMadCarGimmick::CCarHeadLight::~CCarHeadLight(void)
{
    ASSERT(!m_hLight);
};


void CMadCarGimmick::CCarHeadLight::Init(const RwV3d* pvPosition, float fRadius)
{
    ASSERT(pvPosition);
    ASSERT(fRadius > 0.0f);
    ASSERT(!m_hLight);

    m_hLight = CEffectLightManager::Regist(pvPosition, fRadius, { 0xFF, 0xFF, 0xFF, 0xFF });
    ASSERT(m_hLight);
};


void CMadCarGimmick::CCarHeadLight::Term(void)
{
    ASSERT(m_hLight);
    
    CEffectLightManager::Remove(m_hLight);
    m_hLight = 0;
};


void CMadCarGimmick::CCarHeadLight::Update(const RwV3d* pvPosition)
{
    ASSERT(m_hLight);
    
    CEffectLightManager::SetPosition(m_hLight, pvPosition);
};


/*static*/ int32 CMadCarGimmick::GetCarType(SUBID subid)
{
    ASSERT((subid >= SUBID_START) && (subid < SUBID_NUM));
    if (subid >= SUBID_START_LIGHTOFF)
        return (subid - SUBID_START_LIGHTOFF);
    else
        return subid;
};


CMadCarGimmick::CMadCarGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_pMove(nullptr)
, m_subid(0)
, m_szPathName()
, m_aCarLight()
, m_bCarLight(false)
{
    GIMMICKPARAM::GIMMICK_TERMS_TIME* pInitParam = (GIMMICKPARAM::GIMMICK_TERMS_TIME*)pParam;
    ASSERT(pInitParam);

    ASSERT(std::strlen(pInitParam->m_szTargetGimmick) < COUNT_OF(m_szPathName));
    std::strcpy(m_szPathName, pInitParam->m_szTargetGimmick);
    
    m_subid = GetCarType(SUBID(pInitParam->m_subid));
    m_bCarLight = (pInitParam->m_subid < SUBID_START_LIGHTOFF);

    m_pMove = new CMadCarGimmickMove;
    ASSERT(m_pMove);
    m_pMove->SetPosition(&Math::VECTOR3_ZERO);
    m_pMove->SetRotation(&Math::VECTOR3_ZERO);
    m_pMove->InitPath(m_szPathName, 0.0f);
    m_pMove->SetPathLength(CMadCarGimmickManager::GetPathLength(m_szPathName));
    m_pMove->SetPathTimeRate(pInitParam->m_fTime);
    m_pMove->SetCarDirection(CMadCarGimmickManager::IsPathDirection(m_szPathName));
    m_pMove->Start();
    SetMoveStrategy(m_pMove);
    
    RwV3d vPos = Math::VECTOR3_ZERO;
    m_pMove->GetPosition(&vPos);
    
    CModel* pModel = CModelManager::CreateModel(s_aMadCarSubinfo[m_subid].m_pszModelName);
    ASSERT(pModel);
    pModel->SetLightingEnable(false);    
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetPosition(&vPos);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    if (m_bCarLight)
    {
        RwV3d vLightPos = Math::VECTOR3_ZERO;
        
        for (int32 i = 0; i < COUNT_OF(m_aCarLight); ++i)
        {
            vLightPos = s_aMadCarSubinfo[m_subid].m_vLightPos;
            if (i == 1)
                vLightPos.x = -vLightPos.x;

            Math::Vec3_Add(&vLightPos, &vLightPos, &pInitParam->m_vPosition);

            m_aCarLight[i].Init(&vLightPos, s_aMadCarSubinfo[m_subid].m_fLightRadius);
        };
    };
};


CMadCarGimmick::~CMadCarGimmick(void)
{
    if (m_bCarLight)
    {
        for (int32 i = 0; i < COUNT_OF(m_aCarLight); ++i)
            m_aCarLight[i].Term();
    };
    
    if (m_pMove)
    {
        delete m_pMove;
        m_pMove = nullptr;
    };
};


void CMadCarGimmick::PostMove(void)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pMove->GetRotation(&vRotation);
    m_model.SetRotation(&vRotation);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pMove->GetPosition(&vPosition);

    if (m_bCarLight)
    {
        for (int32 i = 0; i < COUNT_OF(m_aCarLight); ++i)
        {
            RwMatrix matRot;
            RwMatrixSetIdentityMacro(&matRot);
            Math::Matrix_Rotate(&matRot, &vRotation);

            RwV3d vLightPos = s_aMadCarSubinfo[m_subid].m_vLightPos;
            if (i == 1)
                vLightPos.x = -vLightPos.x;

            RwV3dTransformPoint(&vLightPos, &vLightPos, &matRot);
            Math::Vec3_Add(&vLightPos, &vLightPos, &vPosition);

            m_aCarLight[i].Update(&vLightPos);
        };
    };

    if (m_bCarLight)
    {
        CHitAttackData Attack;
        Attack.SetObject(GetHandle());
        Attack.SetPower(s_aMadCarSubinfo[m_subid].m_nAttackPower);
        Attack.SetTarget(CHitAttackData::TARGET(CHitAttackData::TARGET_ENEMY | CHitAttackData::TARGET_PLAYER));
        Attack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        Attack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        Attack.SetFlyawayParameter(0.0f, 12.0f);
        Attack.SetMotion("MadCarMotion");

        for (int32 i = 0; i < COUNT_OF(m_aCarLight); ++i)
        {
            RwV3d vAttackHitCenter = Math::VECTOR3_ZERO;
            
            RwMatrix matRotY;
            RwMatrixSetIdentityMacro(&matRotY);
            Math::Matrix_RotateY(&matRotY, vRotation.y);
            
            RwV3dTransformPoint(
                &vAttackHitCenter,
                &s_aMadCarSubinfo[m_subid].m_vAttackPoint[i],
                &matRotY
            );

            Math::Vec3_Add(&vAttackHitCenter, &vAttackHitCenter, &vPosition);

            RwSphere AttackSphere = { vAttackHitCenter, s_aMadCarSubinfo[m_subid].m_fAttackRadius };
            Attack.SetShape(CHitAttackData::SHAPE_SPHERE);
            Attack.SetSphere(&AttackSphere);

            CHitAttackManager::RegistAttack(&Attack);            
        };
    };

    if (m_pMove->IsPathEnd())
        Release();
};