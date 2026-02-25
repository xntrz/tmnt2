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
, m_model()
, m_hAtari(0)
, m_apBodyHitData()
, m_bIsPushingBig(false)
{
    std::memset(m_apBodyHitData, 0x00, sizeof(m_apBodyHitData));

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam);

    if (pInitParam->m_subid == 0)
    {
        /* init disp model */
        CModel* pModelDisp = CModelManager::CreateModel("stopper");
        ASSERT(pModelDisp);

        pModelDisp->SetLightingEnable(false);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelDisp);

        /* init atari model */
        CModel* pModelAtari = CModelManager::CreateModel("stopper_a");
        ASSERT(pModelAtari);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);

        /* init pos & rot */
        RwMatrix matrix;
        RwMatrixSetIdentity(&matrix);        
        CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);
        
        RwV3d vRotation = Math::VECTOR3_ZERO;
        CGimmickUtils::MatrixToRotation(&matrix, &vRotation);

        m_model.SetRotation(&vRotation);
        m_model.SetPosition(&pInitParam->m_vPosition);

        /* init start */
        SetModelStrategy(&m_model);

        /* init atari */
        if (m_model.GetCollisionModelClump())
        {
            RpClump* pClump = m_model.GetCollisionModelClump();
            
            m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
            if (m_hAtari)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 2.0f);
        };
    };

    float fRotY = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    
    RwV3d vBodyPosition = pInitParam->m_vPosition;
    vBodyPosition.x += Math::Sin(fRotY) * 1.3f;
    vBodyPosition.y += 0.5f;
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


/*virtual*/ CStopperGimmick::~CStopperGimmick(void)
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


/*virtual*/ void CStopperGimmick::PostMove(void) /*override*/
{
    float fNearestPlayerDist = CGimmickUtils::CalcNearestPlayerDistanceXZ(&m_vPosition);
    
    if ((fNearestPlayerDist < 4.0f) && isNearRaphRockGimmick())
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
    CGameObject* pGameObject = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK);
    while (pGameObject)
    {
        ASSERT(pGameObject->GetType() == GAMEOBJECTTYPE::GIMMICK);
        CGimmick* pGimmick = static_cast<CGimmick*>(pGameObject);

        if (pGimmick->GetID() == GIMMICKID::ID_N_RAPROC)
        {
            CPushingGimmick* pPushingGimmick = static_cast<CPushingGimmick*>(pGimmick);

            m_bIsPushingBig = pPushingGimmick->IsBig();

            RwV3d vPushingPosition = Math::VECTOR3_ZERO;
            pPushingGimmick->GetPosition(&vPushingPosition);

            RwV3d vDist = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vDist, &vPushingPosition, &m_vPosition);
            vDist.y = 0.0f;

            float fDist = Math::Vec3_Length(&vDist);
            if (m_bIsPushingBig)
            {
                if (fDist < 3.75f)
                    return true;
            }
            else
            {
                if (fDist < 3.0f)
                    return true;
            };
        };

        pGameObject = CGameObjectManager::GetNext(GAMEOBJECTTYPE::GIMMICK, pGameObject);
    };

    return false;
};