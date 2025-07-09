#include "MineGimmick.hpp"

#include "Game/Component/Effect/MagicParameter.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


//
//  beta code only
//
//#define MINE_GIMMICK_ATTACK_EXPLODE


/*static*/ bool CMineGimmick::m_bAnimOwnerExist = false;


CMineGimmick::CMineGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_eState(STATE_IDLE)
, m_fRadius(0.0f)
, m_fCounter(0.0f)
, m_fHeight(0.0f)
, m_hAtari(0)
, m_bAnimOwner(false)
, m_pAnimatedMaterials(nullptr)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    m_fHeight = pBasicParam->m_vPosition.y;
    m_fRadius = 0.5f;

    //
    //  init disp model
    //
    CModel* pModel = CModelManager::CreateModel("jirai");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init break model
    //
    CModel* pYarareModel = CModelManager::CreateModel("jirai_2");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pYarareModel);
    
    //
    //  init pos & rot
    //
    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.UpdateFrame();
    
    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_SEESAW);
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 1.0f);
    };
    
    //
    //  init UV anim
    //
    m_pAnimatedMaterials = CUVAnim::CreateAnimatedMaterialsList(pYarareModel->GetClump());
    ASSERT(m_pAnimatedMaterials);

    if (m_bAnimOwnerExist)
    {
        m_bAnimOwner = false;        
    }
    else
    {
        m_bAnimOwner = true;
        m_bAnimOwnerExist = true;
    };

    //
    //  setup model & movement strat
    //
    SetModelStrategy(&m_model);
};


CMineGimmick::~CMineGimmick(void)
{
    if (m_bAnimOwner)
    {
        m_bAnimOwnerExist = false;
        m_bAnimOwner = false;
    };

    if (m_pAnimatedMaterials)
    {
        CUVAnim::DestroyAnimatedMaterialsList(m_pAnimatedMaterials);
        m_pAnimatedMaterials = nullptr;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CMineGimmick::PostMove(void)
{
    float dt = CGameProperty::GetElapsedTime();

    switch (m_eState)
    {
    case STATE_IDLE:
        {
            if (m_hAtari)
                CMapCollisionModel::UpdateCollisionModel(m_hAtari, nullptr, nullptr);
            
#ifdef MINE_GIMMICK_ATTACK_EXPLODE
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_model.GetPosition(&vPosition);

            RwSphere hitSphere = { vPosition, m_fRadius };

            CHitCatchData hitCatch;
            hitCatch.Cleanup();
            hitCatch.SetObject(GetHandle());
            hitCatch.SetObjectType(GetType());
            hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
            hitCatch.SetSphere(&hitSphere);

            CHitAttackManager::RegistCatch(&hitCatch);
#endif
        }
        break;

    case STATE_ON:
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_model.GetPosition(&vPosition);

            vPosition.y += dt;
            if (vPosition.y > (m_fHeight + 0.3f))
                vPosition.y = (m_fHeight + 0.3f);

            m_model.SetPosition(&vPosition);

            m_fCounter += dt;
            if ((m_fCounter > 2.0f) && IsEnableStateChange(STATE_EXPLODE))
                m_eState = STATE_EXPLODE;            
        }
        break;

    case STATE_EXPLODE:
        {
            m_eState = STATE_DELETE;

            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_model.GetPosition(&vPosition);

            CMagicManager::CParameter param;
            param.SetPositon(&vPosition);
            param.SetObject(this);
            param.SetSoundPlay(false);
            
            CMagicManager::Play(MAGICID::ID_EXPL_B1, &param);

            CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(4139));
        }
        break;

    case STATE_DELETE:
        {
            Release();
        }
        break;

    default:
        ASSERT(false);
        break;
    };

	if (!m_bAnimOwnerExist)
	{
		m_bAnimOwner = true;
		m_bAnimOwnerExist = true;
	};

    if (m_bAnimOwner)
        CUVAnim::UpdateUVAnim(m_pAnimatedMaterials, dt);
};


void CMineGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    if (m_eState != STATE_IDLE)
        return;
    
    m_eState = STATE_ON;
    m_model.SetDrawType(CNormalGimmickModel::MODELTYPE_DRAW_BREAK);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_model.GetPosition(&vPosition);

    CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(4167));
    CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(4166));
    CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(4165));
};


void CMineGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
#ifdef MINE_GIMMICK_ATTACK_EXPLODE
    if (IsEnableStateChange(STATE_EXPLODE))
        m_eState = STATE_EXPLODE;
#endif
};


bool CMineGimmick::IsEnableStateChange(STATE eState) const
{
    if (eState == STATE_ON)
    {
        if (m_eState == STATE_IDLE)
            return true;
    }
    else if (eState == STATE_EXPLODE)
    {
        if ((m_eState == STATE_IDLE) || (m_eState == STATE_ON))
            return true;
    };

    return false;
};