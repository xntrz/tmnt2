#include "SeesawGimmick.hpp"

#include "Game/Component/Effect/MagicParameter.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/SeesawGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Sound/MessageManager.hpp"


CSeesawGimmick::CSeesawGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pSeesawMove(nullptr)
, m_iAxisNum(0)
, m_model()
, m_hAtari(0)
, m_bIsSoundCall(false)
, m_fSoundCallTimer(0.0f)
{
    struct INITPARAM
    {
        const char* pszModelName;
        const char* pszAtariName;
    };

    static const INITPARAM s_aInitParam[] =
    {
        { "seesaw_bar", "seesaw_bar_a"  },
        { "seesaw",     "seesaw_a"      },
    };

    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    ASSERT(pBasicParam->m_subid >= 0);
    ASSERT(pBasicParam->m_subid < COUNT_OF(s_aInitParam));

    const INITPARAM* pInitParam = &s_aInitParam[pBasicParam->m_subid];
    
    m_iAxisNum = 1;

    //
    //  init disp model
    //
    CModel* pModel = CModelManager::CreateModel(pInitParam->pszModelName);
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init atari model
    //
    CModel* pAtariModel = CModelManager::CreateModel(pInitParam->pszAtariName);
    ASSERT(pAtariModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);
    
    //
    //  init movement
    //
	m_pSeesawMove = new CSeesawGimmickMove;
    m_pSeesawMove->SetPosition(&pBasicParam->m_vPosition);
    m_pSeesawMove->SetRotateY(CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat));
    m_pSeesawMove->Start();

    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pSeesawMove->GetTheta(&vRotation);

    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_SEESAW);
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 6.0f);
    };
    
    //
    //  setup model & movement strat
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pSeesawMove);
};


CSeesawGimmick::~CSeesawGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };

    if (m_pSeesawMove)
    {
        delete m_pSeesawMove;
        m_pSeesawMove = nullptr;
    };
};


void CSeesawGimmick::PreMove(void)
{
    CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    if (pModel)
        m_pSeesawMove->SetMatrixFromClump(pModel->GetClump());
};


void CSeesawGimmick::PostMove(void)
{
    if (!m_hAtari)
        return;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pSeesawMove->GetPosition(&vPosition);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pSeesawMove->GetTheta(&vRotation);

    m_model.SetPosition(&vPosition);
    m_model.SetRotation(&vRotation);

    RwV3d vRotVel = Math::VECTOR3_ZERO;
    m_pSeesawMove->GetOmega(&vRotVel);

    float fRotSpeed = Math::Vec3_Length(&vRotVel);
    if (fRotSpeed <= 0.001f)
    {
        m_bIsSoundCall = false;
    }
    else if (!m_bIsSoundCall)
    {
        m_bIsSoundCall = true;
        CGameSound::PlayObjectSE(this, SDCODE_SE(4180), 0);
    };

    CMapCollisionModel::UpdateCollisionModel(m_hAtari, 0, &vRotVel);

    m_fSoundCallTimer -= CGameProperty::GetElapsedTime();
};


void CSeesawGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    m_pSeesawMove->SetCharacterPositionFromName(pszSender);

    if (!CGameSound::IsVoicePlaying() && (m_fSoundCallTimer < 0.0f))
    {
        CGameObject* pGameObj = CGameObjectManager::GetObject(pszSender);
        ASSERT(pGameObj);

        if (pGameObj->GetType() != GAMEOBJECTTYPE::CHARACTER)
            return;

        if (static_cast<CCharacter*>(pGameObj)->GetCharacterType() != CCharacter::TYPE_PLAYER)
            return;

        int32 nVOXCode = 0;
        
        if (static_cast<CPlayerCharacter*>(pGameObj)->GetID() == PLAYERID::ID_MIC)
            nVOXCode = SDCODE_VOICE(20762);
        else if (static_cast<CPlayerCharacter*>(pGameObj)->GetID() == PLAYERID::ID_LEO)
            nVOXCode = SDCODE_VOICE(20763);

        if (nVOXCode)
        {
            CMessageManager::DirectCall(nVOXCode);
            m_fSoundCallTimer = 3.0f;
        };
    };
};