#include "StopperGimmick.hpp"
#include "PushingGimmick.hpp"

#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CStopperGimmick::CStopperGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_hAtari(0)
, m_bIsPushingBig(false)
{
    std::memset(m_apBodyHitData, 0x00, sizeof(m_apBodyHitData));

    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;

    if (pInitParam->m_subid == 0)
    {
        CModel* pModelDisp = CModelManager::CreateModel("stopper");
        CModel* pModelAtari = CModelManager::CreateModel("stopper_a");
        
        ASSERT(pModelDisp);
        ASSERT(pModelAtari);

        pModelDisp->SetLightingEnable(false);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelDisp);
        m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);

        RwV3d vRotation = Math::VECTOR3_ZERO;        
        RwMatrix matrix;
        RwMatrixSetIdentityMacro(&matrix);
        
        CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
        CGimmickUtils::MatrixToRotation(&matrix, &vRotation);
        m_model.SetPosition(&pInitParam->m_vPosition);
        m_model.SetRotation(&vRotation);

        SetModelStrategy(&m_model);

        if (m_model.GetCollisionModelClump())
        {
            RpClump* pClump = m_model.GetCollisionModelClump();
            
            m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
            ASSERT(m_hAtari);
            
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 2.0f);
        };
    };

    RwV3d vBodyPosition = pInitParam->m_vPosition;
    float fRotY = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    
    vBodyPosition.x += Math::Sin(fRotY) * 1.3f;
    vBodyPosition.x += 0.5f;
    vBodyPosition.z += Math::Cos(fRotY) * 1.3f;

    for (int32 i = 0; i < COUNT_OF(m_apBodyHitData); ++i)
    {
        CBodyHitData* pBodyHitData = CBodyHitManager::AllocData();
        ASSERT(pBodyHitData);

        pBodyHitData->InitData(&vBodyPosition, 0.5f);
        pBodyHitData->SetState(CBodyHitData::STATE_SELF_TO_OTHER, true);
        m_apBodyHitData[i] = pBodyHitData;

        vBodyPosition.y += 1.0f;        
    };

    m_vPosition = pInitParam->m_vPosition;
};


CStopperGimmick::~CStopperGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    for (int32 i = 0; i < COUNT_OF(m_apBodyHitData); ++i)
    {
        if (m_apBodyHitData[i])
        {
            CBodyHitManager::FreeData(m_apBodyHitData[i]);
            m_apBodyHitData[i] = nullptr;
        };
    };
};


void CStopperGimmick::PostMove(void)
{
    float fNearestPlayerDist = CGimmickUtils::CalcNearestPlayerDistanceXZ(&m_vPosition);
    
    if (fNearestPlayerDist < 4.0f && isNearRaphRockGimmick())
        setBodyHitDataState(true);
    else
        setBodyHitDataState(false);
};


void CStopperGimmick::setBodyHitDataState(bool bState)
{
    for (int32 i = 0; i < COUNT_OF(m_apBodyHitData); ++i)
    {
        if (i == 2)
        {
            if (m_bIsPushingBig)
                m_apBodyHitData[i]->SetState(CBodyHitData::STATE_ENABLE, bState);
            else
                m_apBodyHitData[i]->SetState(CBodyHitData::STATE_ENABLE, false);
        }
        else
        {
            m_apBodyHitData[i]->SetState(CBodyHitData::STATE_ENABLE, bState);
        };
    };
};


bool CStopperGimmick::isNearRaphRockGimmick(void)
{
    bool bResult = false;
    
    CGameObject* pGameObject = CGameObjectManager::GetNext();
    while (pGameObject)
    {
        if (pGameObject->GetType() == GAMEOBJECTTYPE::GIMMICK)
        {
            CGimmick* pGimmick = (CGimmick*)pGameObject;
            if (pGimmick->GetID() == GIMMICKID::ID_N_RAPROC)
            {
                CPushingGimmick* pPushingGimmick = (CPushingGimmick*)pGimmick;
                
                m_bIsPushingBig = pPushingGimmick->IsBig();
                
                RwV3d vPushingPosition = Math::VECTOR3_ZERO;
                pPushingGimmick->GetPosition(&vPushingPosition);

                RwV3d vDist = Math::VECTOR3_ZERO;
                Math::Vec3_Sub(&vDist, &vPushingPosition, &m_vPosition);
                
                float fDist = Math::Vec3_LengthXZ(&vDist);
                if (m_bIsPushingBig)
                {
                    if (fDist < 3.75f)
                    {
                        bResult = true;
                        break;
                    };
                }
                else
                {
                    if (fDist < 3.0f)
                    {
                        bResult = true;
                        break;
                    };
                };
            };
        };

        pGameObject = CGameObjectManager::GetNext(pGameObject);
    };

    return bResult;
};