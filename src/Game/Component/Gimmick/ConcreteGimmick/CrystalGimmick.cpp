#include "CrystalGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/MoveStrategy/CrystalGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/Component/Module/ModuleManager.hpp"
#include "Game/Component/Module/CircleShadowModule.hpp"
#include "Game/Component/Module/BarrierModule.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CCrystalGimmick::CCrystalEffect::CCrystalEffect(void)
: m_hEffect(0)
, m_fTimer(0.0f)
, m_fTargetTime(0.0f)
{
    ;
};


CCrystalGimmick::CCrystalEffect::~CCrystalEffect(void)
{
    ;
};


void CCrystalGimmick::CCrystalEffect::Start(EFFECTID::VALUE id, const RwV3d* pvPos)
{
    m_fTimer = 0.0f;
    m_fTargetTime = 1.5f;

    End();
    
    m_hEffect = CEffectManager::Play(id, pvPos);
    ASSERT(m_hEffect);

    if (m_hEffect)
        CEffectManager::SetScale(m_hEffect, 1.6f);
};


void CCrystalGimmick::CCrystalEffect::End(void)
{
    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };
};


void CCrystalGimmick::CCrystalEffect::Run(void)
{
    if (!m_hEffect)
        return;

    if (CEffectManager::GetEffectID(m_hEffect) != EFFECTID::ID_SHRD_ELEC)
        return;

    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer >= m_fTargetTime)
    {
        End();
        m_fTimer = 0.0f;
        m_fTargetTime = 0.0f;
    };
};


CCrystalGimmick::CCrystalGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_pCryMove(nullptr)
, m_pModuleMan(nullptr)
, m_szTargetName()
, m_state(STATE_NORMAL)
, m_nLife(100)
, m_fTimer(0.0f)
, m_nLaserPower(34)
, m_effectObj()
{
    GIMMICKPARAM::GIMMICK_TERMS* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_TERMS*>(pParam);

    ASSERT(std::strlen(pInitParam->m_szTargetGimmick) < GAMEOBJECTTYPES::NAME_MAX);
    std::strcpy(m_szTargetName, pInitParam->m_szTargetGimmick);

    RwMatrix rotMat;
    RwMatrixSetIdentityMacro(&rotMat);
    CGimmickUtils::QuaternionToRotationMatrix(&rotMat, &pInitParam->m_quat);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    CGimmickUtils::MatrixToRotation(&rotMat, &vRotation);

    //
    //  init model
    //
    CModel* pModel = CModelManager::CreateModel("crystal");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);
    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    
    //
    //  init movement
    //
    RwV3d vRotatePerSec = { 0.0f, Math::PI, 0.0 };

    if ((Math::Rand() % 10) <= 4)
        Math::Vec3_Negate(&vRotatePerSec, &vRotatePerSec);

    m_pCryMove = new CCrystalGimmickMove(&pInitParam->m_vPosition);
    m_pCryMove->SetRotation(&vRotation);
    m_pCryMove->SetRotationVelocity(&vRotatePerSec);
    m_pCryMove->SetPosition(&pInitParam->m_vPosition);
    m_pCryMove->Start();
    
    //
    //  init module man
    //
    const float fShadowWH = 1.0f;
    
    m_pModuleMan = new CModuleManager;
    m_pModuleMan->Include(CCircleShadowModule::New(this, fShadowWH, fShadowWH, true));
    
    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
    SetMoveStrategy(m_pCryMove);
};


CCrystalGimmick::~CCrystalGimmick(void)
{
    if (m_pCryMove)
    {
        delete m_pCryMove;
        m_pCryMove = nullptr;
    };

    if (m_pModuleMan)
    {
        delete m_pModuleMan;
        m_pModuleMan = nullptr;
    };
};


void CCrystalGimmick::Draw(void) const
{
    CGimmick::Draw();
    
    if (m_pModuleMan)
        m_pModuleMan->Draw();
};


void CCrystalGimmick::PreMove(void)
{
    crystalCtrl();
    m_effectObj.Run();
};


void CCrystalGimmick::PostMove(void)
{
    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pCryMove->GetRotation(&vRotation);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCryMove->GetPosition(&vPosition);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&vPosition);

    if (m_pModuleMan)
        m_pModuleMan->Run();
};


void CCrystalGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    CGimmickUtils::PlayHitEffect(pAttack);

    if (isValidAttack(pAttack))
    {
        m_nLife -= (11 * 3);
        CGameSound::PlayObjectSE(this, SDCODE_SE(4251));
    }
    else
    {
        m_nLife -= 11;
        CGameSound::PlayObjectSE(this, SDCODE_SE(4252));
    };

    RwV3d vEffectPos = Math::VECTOR3_ZERO;
    m_pCryMove->GetPosition(&vEffectPos);

    m_effectObj.Start(EFFECTID::ID_SHRD_ELEC, &vEffectPos);
};


void CCrystalGimmick::crystalCtrl(void)
{
    switch (m_state)
    {
    case STATE_NORMAL:
        {
            if (m_nLife > 0)
            {
                setHitAttack();
            }
            else
            {
                m_pCryMove->SetMoveType(CCrystalGimmickMove::MOVETYPE_SHAKE);

                RwV3d vEffectPos = Math::VECTOR3_ZERO;
                m_pCryMove->GetPosition(&vEffectPos);

                m_effectObj.Start(EFFECTID::ID_SHRD_ELEC, &vEffectPos);
                m_state = STATE_DESTROYED;
            };
        }
        break;
        
    case STATE_DESTROYED:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > 1.5f)
            {
                m_effectObj.End();

                RwV3d vEffectPos = Math::VECTOR3_ZERO;
                m_pCryMove->GetPosition(&vEffectPos);

                m_effectObj.Start(EFFECTID::ID_EXPL_B1, &vEffectPos);
                m_state = STATE_END;
            };
        }
        break;
        
    case STATE_END:
        {
            CGimmickManager::PostEvent(m_szTargetName, GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);
            CGameSound::PlayObjectSE(this, SDCODE_SE(4250));
            Release();
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CCrystalGimmick::setHitAttack(void)
{
    CModel* pMdl = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RpClump* pClump = pMdl->GetClump();
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

    RwSphere HitSphere = { { 0.0f, 0.0f, 0.0f }, 1.0f };
    RwV3dTransformPoint(&HitSphere.center, &HitSphere.center, pMatrix);

    CHitCatchData CatchData;
    CatchData.Cleanup();
    CatchData.SetObject(GetHandle());
    CatchData.SetObjectType(GetType());
    CatchData.SetShape(CHitCatchData::SHAPE_SPHERE);
    CatchData.SetSphere(&HitSphere);

    CHitAttackManager::RegistCatch(&CatchData);
};


bool CCrystalGimmick::isValidAttack(CHitAttackData* pAttack)
{
    CGameObject* pGameObj = CGameObjectManager::GetObject(pAttack->GetObjectHandle());
    ASSERT(pGameObj);

    switch (pGameObj->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        {
            CCharacter* pChara = static_cast<CCharacter*>(pGameObj);

            bool bIsPlayer  = (pChara->GetCharacterType() == CCharacter::TYPE_PLAYER);
            bool bIsBarrier = (std::strcmp(pAttack->GetMotion(), CBarrierModule::BARRIER_MOTIONNAME) == 0);
            bool bIsDon     = (std::strcmp(pGameObj->GetName(), "donatello") == 0);
            bool bIsSpl     = (std::strcmp(pGameObj->GetName(), "splinter") == 0);

            if (bIsPlayer && bIsBarrier && (bIsDon || bIsSpl))
                return true;
        }
        break;

    case GAMEOBJECTTYPE::EFFECT:
        {
            CMagic* pMagic = static_cast<CMagic*>(pGameObj);

            if (pMagic->GetID() == MAGICID::ID_DON_LASER_BEAM)
                return true;
        }
        break;
    };

    return false;
};