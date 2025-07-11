#include "FenceGimmick.hpp"
#include "FenceGimmickManager.hpp"

#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/Shot/Shot.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "System/Common/Screen.hpp"


static const int32 FENCE_LIFE = 120;


CFenceGimmick::CFenceGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_move()
, m_model()
, m_nLife(FENCE_LIFE)
, m_eState(STATE_IDLE)
, m_hAtari(0)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    //
    //  init normal display model
    //
    CModel* pTempModel = CModelManager::CreateModel("fence");
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);

    //
    //  init normal atari model
    //
    pTempModel = CModelManager::CreateModel("fence_a");
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pTempModel);

    //
    //  init map collision model
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);
    };

    //
    //  init pos & rot
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);

    m_move.SetPosition(&pBasicParam->m_vPosition);
    m_move.SetVelocity(&Math::VECTOR3_ZERO);
    m_move.SetRotation(&vRotation);
    m_move.SetRotVelocity(&Math::VECTOR3_ZERO);

    m_model.SetPosition(&pBasicParam->m_vPosition);
    m_model.SetRotation(&vRotation);
    m_model.UpdateFrame();

    //
    //  setup model & movement strat
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(&m_move);
};


CFenceGimmick::~CFenceGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CFenceGimmick::PostMove(void)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_move.GetRotation(&vRotation);
    
    m_model.SetRotation(&vRotation);

    switch (m_eState)
    {
    case STATE_IDLE:
        {
            CModel* pMdl = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

            RwSphere hitSphere;
            pMdl->GetPosition(&hitSphere.center);
            hitSphere.radius = 2.0f;

            CHitCatchData hitCatch;
            hitCatch.Cleanup();
            hitCatch.SetObject(GetHandle());
            hitCatch.SetObjectType(GetType());
            hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
            hitCatch.SetSphere(&hitSphere);

            CHitAttackManager::RegistCatch(&hitCatch);
        }
        break;

    case STATE_BREAK:
        {
            if (DecreaseMatAlpha())
                Release();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CFenceGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_eState != STATE_IDLE)
        return;

    CGameObject* pAttacker = CGameObjectManager::GetObject(pAttack->GetObjectHandle());
    if (pAttacker->GetType() == GAMEOBJECTTYPE::CHARACTER)
    {
        CCharacter* pAttackerChr = static_cast<CCharacter*>(pAttacker);
        if (pAttackerChr->GetCharacterType() != CCharacter::TYPE_PLAYER)
            return;
    };

    CGimmickUtils::PlayHitEffect(pAttack);
    CFenceGimmickManager::DispatchInformation(pAttack);

    m_nLife -= pAttack->GetPower();
    if (m_nLife < 0)
    {
        EffectCallAtBreak();
        SetVelocityAtBreak(pAttacker);

        m_eState = STATE_BREAK;
        m_move.Start();

        if (m_hAtari)
        {
            CMapCollisionModel::RemoveCollisionModel(m_hAtari);
            m_hAtari = 0;
        };
    };
};


void CFenceGimmick::EffectCallAtBreak(void)
{
    RwV3d avBreakPos[] =
    {
        { -0.8f, 2.3f, 0.0f  },
        {  0.8f, 2.3f, 0.0f  },
        { -1.0f, 1.5f, 0.0f  },
        {  1.0f, 1.5f, 0.0f  },
        { -1.0f, 0.6f, 0.0f  },
        {  1.0f, 0.6f, 0.0f  },
    };

    CModel* pMdl = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RpClump* pClump = pMdl->GetClump();
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    RwV3dTransformPoints(avBreakPos, avBreakPos, COUNT_OF(avBreakPos), pMatrix);

    for (int32 i = 0; i < COUNT_OF(avBreakPos); ++i)
    {
        CEffectManager::Play(EFFECTID::ID_ALL_DOWNSMOKE, &avBreakPos[i]);
        CEffectManager::Play(EFFECTID::ID_WOOD, &avBreakPos[i]);
    };
};


void CFenceGimmick::SetVelocityAtBreak(const CGameObject* pObj)
{
    RwMatrix matRotY;
    RwMatrixSetIdentityMacro(&matRotY);
    
    switch (pObj->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        {
            const CCharacter* pChr = static_cast<const CCharacter*>(pObj);
            ASSERT(pChr->GetCharacterType() == CCharacter::TYPE_PLAYER, "only player chr can attack fence");

            Math::Matrix_RotateY(&matRotY, pChr->GetDirection());
        }
        break;

    case GAMEOBJECTTYPE::SHOT:
        {
            const CShot* pShot = static_cast<const CShot*>(pObj);

            Math::Matrix_RotateY(&matRotY, pShot->GetDirection());
        }
        break;

    case GAMEOBJECTTYPE::EFFECT:
        {
            const CEffect* pEffect = static_cast<const CEffect*>(pObj);
            ASSERT(pEffect->GetEffectType() == CEffect::TYPE_WITHHIT, "only magic effect can attack fence");

            RwV3d vRot = Math::VECTOR3_ZERO;
            m_move.GetRotation(&vRot);
            
            Math::Matrix_RotateY(&matRotY, vRot.y);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    RwV3d vecRotation = { 0.0f, 1.0f, 1.0f };
    RwV3dTransformVector(&vecRotation, &vecRotation, &matRotY);
    Math::Vec3_Normalize(&vecRotation, &vecRotation);

    RwV3d vecVelocity = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vecVelocity, &vecRotation, 8.0f);

    float fRandRotRatioY = static_cast<float>((Math::Rand() % TYPEDEF::UINT32_MAX) / TYPEDEF::UINT32_MAX);
    
    RwV3d vecVelocityRotation = Math::VECTOR3_ZERO;
    vecVelocityRotation.x = MATH_DEG2RAD(vecVelocity.z * 20.0f);
    vecVelocityRotation.y = MATH_DEG2RAD((fRandRotRatioY * 200.0f) - 100.0f);
    vecVelocityRotation.z = MATH_DEG2RAD(vecVelocity.x);

    m_move.SetRotVelocity(&vecVelocityRotation);
    m_move.SetVelocity(&vecVelocity);
};


bool CFenceGimmick::DecreaseMatAlpha(void)
{
    RwUInt8 decValue = static_cast<RwUInt8>(255.0f / CScreen::Framerate());

    CModel* pMdl = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    ASSERT(pMdl);

    RwRGBA materialColor = pMdl->GetColor();
    if (materialColor.alpha >= decValue)
        materialColor.alpha -= decValue;
    pMdl->SetColor(materialColor);

    return (materialColor.alpha < decValue);
};