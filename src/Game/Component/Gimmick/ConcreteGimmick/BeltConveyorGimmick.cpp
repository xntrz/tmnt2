#include "BeltConveyorGimmick.hpp"

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


/*static*/ bool CBeltConveyorGimmick::m_bAnimOwnerExist = false;


CBeltConveyorGimmick::CBeltConveyorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_hAtari(0)
, m_subid(SUBID_MODEL_DISP)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_fVelocity(0.0f)
, m_bAnimOwner(false)
, m_pAnimatedMaterials(nullptr)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pBasicParam);

    m_subid = static_cast<SUBID>(pBasicParam->m_subid);
    ASSERT(m_subid >= SUBID_START);
    ASSERT(m_subid < SUBID_NUM);

    static float s_afVelocity[] =
    {
        3.8f,
        2.6f
    };

    ASSERT(m_subid >= 0);
    ASSERT(m_subid < COUNT_OF(s_afVelocity));

    m_fVelocity = s_afVelocity[m_subid];

    //
    //  Init MODEL
    //
    CModel* pModel = CModelManager::CreateModel("belt");
    ASSERT(pModel);

    CModel* pAtariModel = CModelManager::CreateModel("belt_a");
    ASSERT(pAtariModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pAtariModel);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);

    SetPosition(&pBasicParam->m_vPosition);
    SetRotation(&vRotation);

    m_model.UpdateFrame();

    SetModelStrategy(&m_model);

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_BELTCONVEYOR);
        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 3.0f);
    };

    //
    //  init UV anim
    //
    if (!m_bAnimOwnerExist && (m_subid == SUBID_MODEL_DISP))
    {
        RpClump* pClump = pModel->GetClump();
        ASSERT(pClump != nullptr);

        m_pAnimatedMaterials = CUVAnim::CreateAnimatedMaterialsList(pClump);
        ASSERT(m_pAnimatedMaterials);

        m_bAnimOwner = true;
        m_bAnimOwnerExist = true;
    };
};


 CBeltConveyorGimmick::~CBeltConveyorGimmick(void)
{
    if (m_bAnimOwner)
    {
        if (m_pAnimatedMaterials)
        {
            CUVAnim::DestroyAnimatedMaterialsList(m_pAnimatedMaterials);
            m_pAnimatedMaterials = nullptr;
        };

        m_bAnimOwnerExist = false;
        m_bAnimOwner = false;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


 void CBeltConveyorGimmick::Draw(void) const
{
    if (m_subid == SUBID_MODEL_DISP)
        CGimmick::Draw();
};


 void CBeltConveyorGimmick::PostMove(void)
{
    float dt = CGameProperty::GetElapsedTime();

    if (m_bAnimOwner)
        CUVAnim::UpdateUVAnim(m_pAnimatedMaterials, dt * 3.0f);

    if (m_hAtari)
    {
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        Math::Vec3_Scale(&vVelocity, &m_vDirection, (m_fVelocity * dt));

        CMapCollisionModel::UpdateCollisionModel(m_hAtari, &vVelocity);
    };
};


void CBeltConveyorGimmick::SetPosition(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    m_model.SetPosition(pvPosition);
    m_vPosition = *pvPosition;
};


void CBeltConveyorGimmick::SetRotation(const RwV3d* pvRotation)
{
    ASSERT(pvRotation);
    
    m_model.SetRotation(pvRotation);

    RwMatrix matRot;
    RwMatrixSetIdentity(&matRot);
    Math::Matrix_Rotate(&matRot, pvRotation);

    RwV3d vDirection = Math::VECTOR3_AXIS_Z;
    RwV3dTransformVector(&m_vDirection, &vDirection, &matRot);
    Math::Vec3_Normalize(&m_vDirection, &m_vDirection);
};