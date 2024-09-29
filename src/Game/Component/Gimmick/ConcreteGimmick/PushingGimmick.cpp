#include "PushingGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickQuery.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/MoveStrategy/PushingGimmickMove.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ const CPushingGimmick::KINDINFO CPushingGimmick::m_aPushingGimmickKindInfoList[]
{
    { "pushrock2x2", "pushrock2x2_a", 0.92f, 4.0f, 1.5f, SDCODE_SE(0x1038) },
    { "pushrock2x2", "pushrock2x2_a", 0.92f, 4.0f, 2.0f, SDCODE_SE(0x1038) },
    { "pushrock3x3", "pushrock3x3_a", 1.45f, 6.0f, 1.0f, SDCODE_SE(0x1037) },
    { "pushrock3x3", "pushrock3x3_a", 1.45f, 6.0f, 1.0f, SDCODE_SE(0x1037) },
};


CPushingGimmick::CPushingGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pPushingMove(nullptr)
, m_vPrePos(Math::VECTOR3_ZERO)
, m_fEffectTimer(0.0f)
, m_bSE(false)
, m_subid(0)
, m_hAtari(0)
, m_pBodyHitData(nullptr)
{
    init(pParam);
};


CPushingGimmick::~CPushingGimmick(void)
{
    if (m_pPushingMove)
    {
        delete m_pPushingMove;
        m_pPushingMove = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    if (m_pBodyHitData)
    {
        CBodyHitManager::FreeData(m_pBodyHitData);
        m_pBodyHitData = nullptr;
    };
};


void CPushingGimmick::GetPosition(RwV3d* pvPosition) const
{
    if (m_pPushingMove)
        m_pPushingMove->GetPosition(pvPosition);
};


void CPushingGimmick::PreMove(void)
{
    m_pPushingMove->GetPosition(&m_vPrePos);
};


void CPushingGimmick::PostMove(void)
{
    RwV3d vPostPos = Math::VECTOR3_ZERO;
    m_pPushingMove->GetPosition(&vPostPos);

    RwV3d vVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vVelocity, &vPostPos, &m_vPrePos);

    if (m_hAtari)
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, &vVelocity);

    if (m_pBodyHitData)
        m_pBodyHitData->SetCurrentPos(&vPostPos);

	float fSpeed = Math::Vec3_Length(&vVelocity);

	switch (m_pPushingMove->GetPushState())
    {
    case CPushingGimmickMove::PUSHSTATE_PUSH:
        {
            if (fSpeed > 0.0f)
            {
				startSE();

                if (m_fEffectTimer >= 0.7f)
                {
                    m_fEffectTimer -= 0.7f;

                    RwV3d vDirection = { vVelocity.z, 0.0f, -vVelocity.x, };
                    RwV3d vPosition = Math::VECTOR3_ZERO;

                    float fDirection = Math::ATan2(-vVelocity.x, -vVelocity.z);
                    Math::Vec3_Normalize(&vDirection, &vDirection);
                    Math::Vec3_Add(&vPosition, &m_vPrePos, &vDirection);

                    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DASH2, &vPosition, fDirection);
                    if (hEffect)
                        CEffectManager::SetScale(hEffect, 1.5f);


                    Math::Vec3_Scale(&vDirection, &vDirection, -1.0f);
                    Math::Vec3_Add(&vPosition, &m_vPrePos, &vDirection);

                    hEffect = CEffectManager::Play(EFFECTID::ID_ALL_DASH2, &vPosition, fDirection);
                    if (hEffect)
                        CEffectManager::SetScale(hEffect, 1.5f);
                };

                m_fEffectTimer += CGameProperty::GetElapsedTime();
            }
            else
			{
				stopSE();
				m_fEffectTimer = 0.0f;
            };
        }
        break;

    case CPushingGimmickMove::PUSHSTATE_ROTATION:
    case CPushingGimmickMove::PUSHSTATE_FALL:
    case CPushingGimmickMove::PUSHSTATE_END:
		{
			if (fSpeed <= 0.0f)
				stopSE();
		}
		break;	

    default:
        ASSERT(false);
        break;
	};

	if (m_pPushingMove->GetPushState() == CPushingGimmickMove::PUSHSTATE_END)
		Release();
};


void CPushingGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        CGameObject* pSender = CGameObjectManager::GetObject(pszSender);
        if (pSender && pSender->GetType() == GAMEOBJECTTYPE::CHARACTER)
        {
            CPlayerCharacter* pPlayerCharacter = (CPlayerCharacter*)pSender;
            m_pPushingMove->RequestPlayerPushGimmick(pPlayerCharacter);
        };
    };
};


void CPushingGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    ;
};


bool CPushingGimmick::IsBig(void) const
{
    return (m_subid == 2 || m_subid == 3);
};


void CPushingGimmick::init(void* pParam)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = (GIMMICKPARAM::GIMMICK_BASIC*)pParam;
    m_subid = pInitParam->m_subid;

    CModel* pModelDisp = CModelManager::CreateModel(kindinfo().m_pszModelName);
    CModel* pModelAtari = CModelManager::CreateModel(kindinfo().m_pszModelNameAtari);

    ASSERT(pModelDisp);
    ASSERT(pModelAtari);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelDisp);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);
    
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();
    SetModelStrategy(&m_model);

    m_pPushingMove = new CPushingGimmickMove;
    ASSERT(m_pPushingMove);
	m_pPushingMove->SetName(GetName());
    m_pPushingMove->SetPosition(&pInitParam->m_vPosition);
    m_pPushingMove->SetRotation(&vRotation);
    m_pPushingMove->SetSpeed(kindinfo().m_fSpeed);
    m_pPushingMove->SetRadius(kindinfo().m_fRadius);
    m_pPushingMove->Start();
    SetMoveStrategy(m_pPushingMove);

    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();
        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_PUSHOBJ);
        ASSERT(m_hAtari);
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, kindinfo().m_fCollisionRadius);
    };

    m_pBodyHitData = CBodyHitManager::AllocData();
    ASSERT(m_pBodyHitData);

    RwV3d vPosition = pInitParam->m_vPosition;
    vPosition.y += kindinfo().m_fRadius;

    m_pBodyHitData->InitData(&vPosition, kindinfo().m_fRadius);
    m_pBodyHitData->SetState(CBodyHitData::STATE_SELF_TO_OTHER, true);    
};


void CPushingGimmick::startSE(void)
{
	if (!m_bSE)
	{
		CGameSound::PlaySE(kindinfo().m_nSE);
		m_bSE = true;
	};
};


void CPushingGimmick::stopSE(void)
{
    if (m_bSE)
    {
        CGameSound::StopSE(kindinfo().m_nSE);
        m_bSE = false;
    };
};


const CPushingGimmick::KINDINFO& CPushingGimmick::kindinfo(void) const
{
    ASSERT(m_subid >= 0 && m_subid < COUNT_OF(m_aPushingGimmickKindInfoList));
    return m_aPushingGimmickKindInfoList[m_subid];
};