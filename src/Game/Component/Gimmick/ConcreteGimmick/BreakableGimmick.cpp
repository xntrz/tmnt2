#include "BreakableGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/MapInfo.hpp"
#include "Game/Component/Gimmick/ConcreteGimmick/FallGimmick.hpp"
#include "Game/Component/Gimmick/MoveStrategy/FallGimmickMove.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Shot/Shot.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/GameObject/GameObjectManager.hpp"
#include "Game/System/Hit/BodyHitData.hpp"
#include "Game/System/Hit/BodyHitManager.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/Screen.hpp"


static const CBreakableGimmick::INITPARAM s_aInitParam[] =
{
    { "fireplug",   false,  10, 1, 1.0f },
    { "fireplug",   false,  10, 1, 1.0f },
    { "pc",         true,   10, 1, 1.0f },
    { "pc",         true,   10, 1, 1.0f },
    { "signal",     true,   10, 2, 1.0f },
    { "streetlamp", true,   10, 3, 1.0f },
    { "billboad",	false,  10, 1, 1.0f },	// NOTE: not a mistake in name
    { "neon",       false,  10, 1, 1.0f },
};


static_assert(COUNT_OF(s_aInitParam) == CBreakableGimmick::SUBIDNUM, "checkout");


static const CBreakableGimmick::DETAILPARAM s_aDetailParam[] =
{
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f, { 0.1f,       0.4f,   0.0f    }},
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f, { 0.2f,       1.0f,   0.3f    }},
    { EFFECTID::ID_EXPL_B1,         0.0f,   0.7f, { 0.0f,       1.0f,   0.3f    }},
    { EFFECTID::ID_EXPL_B1,         0.0f,   0.7f, { 0.0f,       1.0f,   0.3f    }},
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f, { -0.3f,      2.3f,   0.4f    }},
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f, { 0.1f,       4.2f,   1.4f    }},
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f, { 0.0f,       3.5f,   -0.2f   }},
    { EFFECTID::ID_ALL_DOWNSMOKE,   0.0f,   1.0f, { 0.0f,       1.0f,   0.0f    }},
    { EFFECTID::ID_ALL_DOWNSMOKE,   0.0f,   1.0f, { 0.0f,       1.0f,   0.0f    }},
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f,               {}           },
    { EFFECTID::ID_UNKNOWN,         1.0f,   1.0f,               {}           },

    { EFFECTID::ID_SPRAYWTER,       0.05f,  1.0f, { 0.1f,       1.0f,   0.0f    }},
    { EFFECTID::ID_SPRAYWTER,       0.05f,  1.0f, { 0.2f,       1.0f,   0.3f    }},
    { EFFECTID::ID_SHRD_ELEC,       0.0f,   1.0f, { 0.0f,       1.0f,   0.3f    }},
    { EFFECTID::ID_SHRD_ELEC,       0.0f,   1.0f, { 0.0f,       1.0f,   0.3f    }},
    { EFFECTID::ID_SHRD_ELEC,       0.0f,   0.8f, { 0.0f,       4.0f,   2.8f    }},
    { EFFECTID::ID_SPARK_SLOW,      0.0f,   0.6f, { 0.1f,       5.8f,   2.0f    }},
    { EFFECTID::ID_SPARK_SLOW,      0.0f,   0.6f, { 0.0f,       3.5f,   -0.2f   }},
    { EFFECTID::ID_UNKNOWN,         0.0f,   1.0f, { 0.0f,       1.0f,   0.0f    }},
    { EFFECTID::ID_UNKNOWN,         0.0f,   1.0f, { 0.0f,       1.0f,   0.0f    }},
    { EFFECTID::ID_SHRD_ELEC,       0.0f,   0.8f,               {}           },
    { EFFECTID::ID_SHRD_ELEC,       0.0f,   0.8f,               {}           },
};


static_assert(COUNT_OF(s_aDetailParam) == (CBreakableGimmick::DETAILKINDNUM * CBreakableGimmick::EFFECTTIMINGNUM), "checkout");


/*static*/ bool CBreakableGimmick::m_bUvAnimOwnerExist = false;


CBreakableGimmick::CBreakableGimmickEffect::CBreakableGimmickEffect(void)
: m_fTimer(0.0f)
, m_fInterval(0.0f)
, m_fIntervalTimer(0.0f)
, m_fScale(1.0f)
, m_effectId(EFFECTID::ID_UNKNOWN)
, m_bStart(false)
, m_detailKind(DETAILKIND_UNKNOWN)
, m_effectTiming(EFFECTTIMING_ATBREAK)
, m_effectState(EFFECTSTATE_WAIT)
, m_pParentModel(nullptr)
, m_vEffectDir(Math::VECTOR3_ZERO)
, m_vEffectPos(Math::VECTOR3_ZERO)
, m_hEffect(0)
{
    ;
};


void CBreakableGimmick::CBreakableGimmickEffect::Init(CModel* pModel, SUBID subid, EFFECTTIMING timing)
{
    m_detailKind = getDetailKind(subid);

    const int32 paramNo = m_detailKind + (DETAILKINDNUM * timing);
    const DETAILPARAM* pDetailParam = &s_aDetailParam[paramNo];
    
    m_effectId      = pDetailParam->m_effectId;
    m_fInterval     = pDetailParam->m_fInterval;
    m_vEffectPos    = pDetailParam->m_vEffectPosition;
    m_fScale        = pDetailParam->m_fScale;
    m_effectTiming  = timing;
    m_pParentModel  = pModel;
    m_effectState   = EFFECTSTATE_WAIT;
    pModel->GetRotation(&m_vEffectDir);
    m_fTimer        = 0.0f;
    m_fIntervalTimer= 0.0f;
    m_bStart        = false;
};


void CBreakableGimmick::CBreakableGimmickEffect::Start(void)
{
    m_bStart = true;
};


void CBreakableGimmick::CBreakableGimmickEffect::Run(void)
{
    if (!m_bStart)
        return;

    switch (m_effectState)
    {
    case EFFECTSTATE_WAIT:
        {
            m_effectState = EFFECTSTATE_RUN;
            
            if (m_effectTiming == EFFECTTIMING_AFTERBREAK)
            {
                if ((m_detailKind == DETAILKIND_FIREPLUG_NY) ||
                    (m_detailKind == DETAILKIND_FIREPLUG_SP))
                {
                    RwV3d vPosition = Math::VECTOR3_ZERO;                    
                    m_pParentModel->GetPosition(&vPosition);
                    
                    CGameSound::PlayPositionSE(&vPosition, SDCODE_SE(4261));
                };
            };
        }
        break;

    case EFFECTSTATE_RUN:
        {
            play();

            m_fTimer += CGameProperty::GetElapsedTime();

            if ((m_fTimer >= 10.0f) || (m_effectTiming == EFFECTTIMING_ATBREAK))
            {
                if (m_hEffect)
                {
                    CEffectManager::Finish(m_hEffect);
                    m_hEffect = 0;
                };

                if (m_effectTiming == EFFECTTIMING_AFTERBREAK)
                {
                    if ((m_detailKind == DETAILKIND_FIREPLUG_NY) ||
                        (m_detailKind == DETAILKIND_FIREPLUG_SP))
                        CGameSound::StopSE(SDCODE_SE(4261));
                };

                m_effectState = EFFECTSTATE_END;
            };
        }
        break;

    case EFFECTSTATE_END:
        break;

    default:
        ASSERT(false);
        break;
    };
};


CBreakableGimmick::DETAILKIND CBreakableGimmick::CBreakableGimmickEffect::getDetailKind(SUBID subid)
{
    DETAILKIND retKind = DETAILKIND_UNKNOWN;
    
    STAGEID::VALUE      idStage = CGameData::PlayParam().GetStage();
    MAPID::VALUE        idMap   = CStageInfo::GetMapID(idStage);
    CMapInfo::CATEGORY  catMap  = CMapInfo::GetCategory(idMap);

    bool bSpace = (catMap == CMapInfo::CATEGORY_SPACE) ||
                  (catMap == CMapInfo::CATEGORY_ZERO);

    switch (subid)
    {
    case SUBID_FIREPLUG:
        retKind = (bSpace ? DETAILKIND_FIREPLUG_SP : DETAILKIND_FIREPLUG_NY);
        break;

    case SUBID_PC:
        retKind = (bSpace ? DETAILKIND_PC_SP : DETAILKIND_PC_NY);
        break;

    case SUBID_SIGNAL:
        retKind = DETAILKIND_SIGNAL_NY;
        break;

    case SUBID_STREETLAMP:
        retKind = (bSpace ? DETAILKIND_STREETLAMP_SP : DETAILKIND_STREETLAMP_NY);
        break;

    case SUBID_BILLBOARD:
        retKind = (bSpace ? DETAILKIND_BILLBOARD_SP : DETAILKIND_BILLBOARD_NY);
        break;

    case SUBID_NEON:
        retKind = (bSpace ? DETAILKIND_NEON_SP : DETAILKIND_NEON_NY);
        break;

    default:
        ASSERT(false);
        break;
    };
    
    return retKind;
};


void CBreakableGimmick::CBreakableGimmickEffect::play(void)
{
    if (m_effectId == EFFECTID::ID_UNKNOWN)
        return;

    switch (m_effectTiming)
    {
    case EFFECTTIMING_ATBREAK:
        atBreakEffectPlay();
        break;

    case EFFECTTIMING_AFTERBREAK:
        afterBreakEffectPlay();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CBreakableGimmick::CBreakableGimmickEffect::atBreakEffectPlay(void)
{
    switch (m_detailKind)
    {
    case DETAILKIND_FIREPLUG_NY:
    case DETAILKIND_FIREPLUG_SP:
        break;
        
    case DETAILKIND_PC_NY:
    case DETAILKIND_PC_SP:
    case DETAILKIND_SIGNAL_NY:
    case DETAILKIND_STREETLAMP_NY:
    case DETAILKIND_STREETLAMP_SP:
    case DETAILKIND_BILLBOARD_NY:
    case DETAILKIND_BILLBOARD_SP:
    case DETAILKIND_NEON_NY:
    case DETAILKIND_NEON_SP:
        defaultOneShotEffectRun();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CBreakableGimmick::CBreakableGimmickEffect::afterBreakEffectPlay(void)
{
    switch (m_detailKind)
    {
    case DETAILKIND_FIREPLUG_NY:
    case DETAILKIND_FIREPLUG_SP:
        sprayWaterEffectRun();
        break;
        
    case DETAILKIND_PC_NY:
    case DETAILKIND_PC_SP:
    case DETAILKIND_SIGNAL_NY:
    case DETAILKIND_STREETLAMP_NY:
    case DETAILKIND_STREETLAMP_SP:
    case DETAILKIND_NEON_NY:
    case DETAILKIND_NEON_SP:
        defaultLoopEffectRun();
        break;
        
    case DETAILKIND_BILLBOARD_NY:
    case DETAILKIND_BILLBOARD_SP:
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CBreakableGimmick::CBreakableGimmickEffect::defaultOneShotEffectRun(void)
{
    m_fIntervalTimer += CGameProperty::GetElapsedTime();
    
    if (m_fIntervalTimer >= m_fInterval)
    {
        RpClump* pClump = m_pParentModel->GetClump();
        RwFrame* pFrame = RpClumpGetFrame(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vPosition, &m_vEffectPos, pMatrix);
        
        if (m_effectId == EFFECTID::ID_SPRAYWTER)
        {
            uint32 hMagic = CMagicManager::Play("spraywater", &vPosition, &m_vEffectDir);
            if (hMagic)
                CMagicManager::SetFlyawaySpeed(hMagic, 10.0f, 5.0f);
        }
        else
        {
            uint32 hEffect = CEffectManager::Play(m_effectId, &vPosition);
            if (hEffect)
            {
                if (m_fScale != 1.0f)
                    CEffectManager::SetScale(hEffect, m_fScale);
            };
        };
    };
};


void CBreakableGimmick::CBreakableGimmickEffect::defaultLoopEffectRun(void)
{
    if (m_hEffect)
        return;
    
    RpClump* pClump = m_pParentModel->GetClump();
    RwFrame* pFrame = RpClumpGetFrame(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vPosition, &m_vEffectPos, pMatrix);

    RwV3d vRotation = Math::VECTOR3_ZERO;
    m_pParentModel->GetRotation(&vRotation);

    m_hEffect = CEffectManager::Play(m_effectId, &vPosition, vRotation.y, false);
    if (m_hEffect)
    {
        if (m_fScale != 1.0f)
            CEffectManager::SetScale(m_hEffect, m_fScale);
    };    
};


void CBreakableGimmick::CBreakableGimmickEffect::sprayWaterEffectRun(void)
{
    if (m_fIntervalTimer <= m_fInterval)
    {
        RpClump* pClump = m_pParentModel->GetClump();
        RwFrame* pFrame = RpClumpGetFrame(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

        RwV3d vPosition = Math::VECTOR3_ZERO;
        RwV3dTransformPoint(&vPosition, &m_vEffectPos, pMatrix);

        m_vEffectDir = *RwMatrixGetAt(pMatrix);

        uint32 hMagic = CMagicManager::Play("spraywater", &vPosition, &m_vEffectDir);
        if (hMagic)
            CMagicManager::SetFlyawaySpeed(hMagic, 10.0f, 5.0f);

        m_fIntervalTimer = 0.0f;
    }
    else
    {
        m_fIntervalTimer += CGameProperty::GetElapsedTime();
    };
};


//
// *********************************************************************************
//


CBreakableGimmick::CBreakableGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_pModel(nullptr)
, m_pMove(nullptr)
, m_aEffect()
, m_state(STATE_NORMAL)
, m_subid(SUBIDNUM)
, m_nLife(0)
, m_nHitSphere(0)
, m_fHitRadius(0.0f)
, m_pBodyHit(nullptr)
, m_hAtari(0)
, m_pAnimatedMaterials(nullptr)
, m_bUvAnimOwner(false)
{
    allClear();
    
    initSubid(pParam);
    initModel(pParam);
    initUVAnim();
    initEffect();
};


CBreakableGimmick::~CBreakableGimmick(void)
{
    if (m_pModel)
    {
        delete m_pModel;
        m_pModel = nullptr;
    };

    if (m_pMove)
    {
        delete m_pMove;
        m_pMove = nullptr;
    };

    if (m_pBodyHit)
    {
        CBodyHitManager::FreeData(m_pBodyHit);
        m_pBodyHit = nullptr;
    };

    if (m_bUvAnimOwner)
    {
        if (m_pAnimatedMaterials)
        {
            CUVAnim::DestroyAnimatedMaterialsList(m_pAnimatedMaterials);
            m_pAnimatedMaterials = nullptr;
        };

        m_bUvAnimOwnerExist = false;
        m_bUvAnimOwner = false;
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CBreakableGimmick::Run(void)
{
    CGimmick::Run();

    //
    //  update uv
    //
    if (m_bUvAnimOwner)
    {
        if (m_pAnimatedMaterials)
            CUVAnim::UpdateUVAnim(m_pAnimatedMaterials, CGameProperty::GetElapsedTime());
    };

    //
    //  bilboard break fade effect
    //
    if (billBoardEraseSlowly())
        Release();    

    //
    //  effect update
    //
    for (int32 i = COUNT_OF(m_aEffect) - 1; i > 0; --i)
        m_aEffect[i].Run();
};


void CBreakableGimmick::GetPosition(RwV3d* pvPosition) const
{
    if (m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL))
        m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)->GetPosition(pvPosition);
    else
        *pvPosition = Math::VECTOR3_ZERO;
};


void CBreakableGimmick::PostMove(void)
{
    registHitAtack();
};


void CBreakableGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_state != STATE_NORMAL)
        return;

    if (pAttack->GetStatus() == CHitAttackData::STATUS_THROW ||
        pAttack->GetStatus() == CHitAttackData::STATUS_BIND)
        return;
    
    m_nLife -= pAttack->GetPower();

    CGimmickUtils::PlayHitEffect(pAttack);

    if (m_nLife < 0)
    {
        m_state = STATE_BREAK;
        
        //
        //  update break disp model
        //
        m_pModel->SetVisualBreak();

        //
        //  update break atari model
        //
        if (m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_ATARI_BREAK))
        {
            m_pModel->SetCollisionBreak();

            if (m_hAtari)
            {
                CMapCollisionModel::RemoveCollisionModel(m_hAtari);
                m_hAtari = 0;
            };
            
            RpClump* pClump = m_pModel->GetCollisionModelClump();
            ASSERT(pClump);
            
            m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
            ASSERT(m_hAtari);

            if (m_hAtari)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 3.0f);
        };

        //
        //  handle kind specific case
        //
        CGameObject* pAttacker = CGameObjectManager::GetObject(pAttack->GetObjectHandle());

        firePlugRotateAtBreak(pAttacker);
        billBoardSetFlyAwayParam(pAttacker);

        //
        //  play effect & sound
        //
        m_aEffect[0].Start();
        m_aEffect[1].Start();
        
        breakSdCall();
    };
};


void CBreakableGimmick::allClear(void)
{
    m_pModel = nullptr;
    m_pMove = nullptr;    
    m_state = STATE_NORMAL;
    m_subid = SUBID_FIREPLUG;
    m_nLife = 0;
    m_nHitSphere = 0;
    m_fHitRadius = 0.0f;
    m_pBodyHit = nullptr;
    m_hAtari = 0;
    m_pAnimatedMaterials = nullptr;
    m_bUvAnimOwner = false;
};


void CBreakableGimmick::initSubid(void* pParam)
{
    m_subid = convertSubid(static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam)->m_subid);

    ASSERT(m_subid >= 0);
    ASSERT(m_subid < COUNT_OF(s_aInitParam));
    static_assert(COUNT_OF(s_aInitParam) == SUBIDNUM, "should equal");
    
    const INITPARAM* pInitParam = &s_aInitParam[m_subid];

    m_state         = STATE_NORMAL;
    m_nLife         = pInitParam->m_nLife;
    m_nHitSphere    = pInitParam->m_nHitSphere;
    m_fHitRadius    = pInitParam->m_fHitRadius;
};


void CBreakableGimmick::initModel(void* pParam)
{
    GIMMICKPARAM::GIMMICK_BASIC* pBasicParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    const INITPARAM* pInitParam = &s_aInitParam[m_subid];

    m_pModel = new CNormalGimmickModel;

    char szTempName[32];
    szTempName[0] = '\0';

    //
    //  init normal display model
    //
    std::strcpy(szTempName, pInitParam->m_pszBaseLabel);

    CModel* pTempModel = CModelManager::CreateModel(szTempName);
    ASSERT(pTempModel);

    m_pModel->SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);

    //
    //  init break display model
    //
    std::sprintf(szTempName, "%s_b", pInitParam->m_pszBaseLabel);

    pTempModel = CModelManager::CreateModel(szTempName);
    ASSERT(pTempModel);

    m_pModel->SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pTempModel);

    //
    //  set normal display model
    //
    m_pModel->SetVisualNormal();

    //
    //  init collision hit model if exist otherwise body hit model
    //
    if (pInitParam->m_bExistHitModel)
    {
        //
        //  init normal ATARI model
        //
        std::sprintf(szTempName, "%s_a", pInitParam->m_pszBaseLabel);

		pTempModel = CModelManager::CreateModel(szTempName);
        ASSERT(pTempModel);

        m_pModel->SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pTempModel);

        //
        //  init break ATARI model
        //
        std::sprintf(szTempName, (m_subid == SUBID_PC ? "%s_a" : "%s_b_a"), pInitParam->m_pszBaseLabel);

		pTempModel = CModelManager::CreateModel(szTempName);
        ASSERT(pTempModel);

        m_pModel->SetModel(CNormalGimmickModel::MODELTYPE_ATARI_BREAK, pTempModel);

        //
        //  set normal atari model
        //
        m_pModel->SetCollisionNormal();

        //
        //  init map collision model
        //
        RpClump* pClump = m_pModel->GetCollisionModelClump();
        ASSERT(pClump);

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 3.0f);

        m_pBodyHit = nullptr;
    }
    else
    {
        m_pBodyHit = CBodyHitManager::AllocData();
        ASSERT(m_pBodyHit);

        RwV3d vBodyPos = pBasicParam->m_vPosition;
        vBodyPos.y += 0.5f;

        m_pBodyHit->InitData(&vBodyPos, 0.5f);
        m_pBodyHit->SetState(CBodyHitData::STATE_SELF_TO_OTHER, true);
    };

    //
    //  init position & rotation
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pBasicParam->m_quat);

    m_pModel->SetPosition(&pBasicParam->m_vPosition);
    m_pModel->SetRotation(&vRotation);
    m_pModel->UpdateFrame();

    //
    //  init billboard specific case - fall movement
    //
    if (m_subid == SUBID_BILLBOARD)
    {
        CModel* pMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

        RwV3d vMdlPos = Math::VECTOR3_ZERO;
        pMdl->GetPosition(&vMdlPos);

        RwV3d vMdlRot = Math::VECTOR3_ZERO;
        pMdl->GetRotation(&vMdlRot);

        const float fReflection = 0.7f;
        const float fCollisionRadius = 0.1f;
        
        m_pMove = new CFallGimmickMove(CFallGimmick::MOVETYPE_BOUND, fReflection, fCollisionRadius);
        m_pMove->SetCollisionRadiusAutoChangeEnable(true);
        m_pMove->SetPosition(&vMdlPos);
        m_pMove->SetRotation(&vMdlRot);
    };

    //
    //  setup model & move strategy
    //
    SetModelStrategy(m_pModel);
    SetMoveStrategy(m_pMove);
};


void CBreakableGimmick::initUVAnim(void)
{
    if (m_bUvAnimOwnerExist)
        return;

    CModel* pMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);

    if (CUVAnim::IsExistsUVAnimDict(pMdl->GetName()))
    {
        m_pAnimatedMaterials = CUVAnim::CreateAnimatedMaterialsList(pMdl->GetClump());
        ASSERT(m_pAnimatedMaterials);

        m_bUvAnimOwnerExist = true;
        m_bUvAnimOwner = true;
    };
};


void CBreakableGimmick::initEffect(void)
{
    static_assert(COUNT_OF(m_aEffect) == EFFECTTIMINGNUM, "should equal");

    for (int32 i = 0; i < COUNT_OF(m_aEffect); ++i)
        m_aEffect[i].Init(m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL), m_subid, EFFECTTIMING(i));
};


void CBreakableGimmick::registHitAtack(void)
{
    if (m_state != STATE_NORMAL)
        return;

    for (int32 i = 0; i < m_nHitSphere; ++i)
    {
        float fRadius = m_fHitRadius;

        RwV3d vPosition = Math::VECTOR3_ZERO;
        vPosition.y = ((static_cast<float>(i) * m_fHitRadius) *
                       (static_cast<float>(i) * m_fHitRadius));

        RpClump* pClump = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)->GetClump();
        RwFrame* pFrame = RpClumpGetFrame(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

        RwV3dTransformPoint(&vPosition, &vPosition, pMatrix);

        RwSphere hitSphere = { vPosition, fRadius };

        CHitCatchData hitCatch;
        hitCatch.Cleanup();
        hitCatch.SetObject(GetHandle());
        hitCatch.SetObjectType(GetType());
        hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
        hitCatch.SetSphere(&hitSphere);

        CHitAttackManager::RegistCatch(&hitCatch);
    };
};


void CBreakableGimmick::breakSdCall(void)
{
    switch (m_subid)
    {
    case SUBID_PC:
        CGameSound::PlayObjectSE(this, SDCODE_SE(4195));
        break;
        
    case SUBID_BILLBOARD:
        CGameSound::PlayObjectSE(this, SDCODE_SE(4189));
        break;
        
    case SUBID_NEON:
        CGameSound::PlayObjectSE(this, SDCODE_SE(4187));
        break;

    default:
        break;
    };
};


void CBreakableGimmick::setAttackerDirection(const CGameObject* pObj)
{
    CModel* pNormMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    
    RwV3d vRotation = Math::VECTOR3_ZERO;
    pNormMdl->GetRotation(&vRotation);

    switch (pObj->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        vRotation.y = static_cast<const CCharacter*>(pObj)->GetDirection();
        break;

    case GAMEOBJECTTYPE::SHOT:
        vRotation.y = static_cast<const CShot*>(pObj)->GetDirection();
        break;

    default:
        break;
    };

    m_pModel->SetRotation(&vRotation);
    m_pModel->UpdateFrame();
};


void CBreakableGimmick::setFlyAwayParam(const CGameObject* pObj)
{
    if (!m_pMove)
        return;

    if (m_pBodyHit)
    {
        CBodyHitManager::FreeData(m_pBodyHit);
        m_pBodyHit = nullptr;
    };

    RwV3d vVelocity = Math::VECTOR3_ZERO;

    switch (pObj->GetType())
    {
    case GAMEOBJECTTYPE::CHARACTER:
        {
            CModel* pChrMdl = static_cast<const CCharacter*>(pObj)->GetModel();
            RpClump* pClump = pChrMdl->GetClump();
            RwFrame* pFrame = RpClumpGetFrame(pClump);
            RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

            vVelocity = pMatrix->at;
            
            vVelocity.x *= 5.0f;
            vVelocity.y = 6.0f;
            vVelocity.z *= 5.0f;
        }
        break;

    case GAMEOBJECTTYPE::SHOT:
        {
            float fShotDir = static_cast<const CShot*>(pObj)->GetDirection();

            CModel* pGmkMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
            RpClump* pClump = pGmkMdl->GetClump();
            RwFrame* pFrame = RpClumpGetFrame(pClump);
            RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

            RwMatrix matTmp;
            RwMatrixSetIdentity(&matTmp);
            
            std::memcpy(&matTmp, pMatrix, sizeof(matTmp));
            
            Math::Matrix_RotateY(&matTmp, fShotDir);

            vVelocity = matTmp.at;

            vVelocity.x *= 3.0f;
            vVelocity.y = 6.0f;
            vVelocity.z *= 3.0f;
        }
        break;

    default:
        {
            CModel* pGmkMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
            RpClump* pClump = pGmkMdl->GetClump();
            RwFrame* pFrame = RpClumpGetFrame(pClump);
            RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

            vVelocity = pMatrix->at;

            vVelocity.x *= 3.0f;
            vVelocity.y = 6.0f;
            vVelocity.z *= 3.0f;
        }
        break;
    };

    m_pMove->SetVelocity(&vVelocity);
    m_pMove->Start();
};


void CBreakableGimmick::firePlugRotateAtBreak(const CGameObject* pObj)
{
    if ((m_subid == SUBID_FIREPLUG) && (m_state == STATE_BREAK))
        setAttackerDirection(pObj);
};


void CBreakableGimmick::billBoardSetFlyAwayParam(const CGameObject* pObj)
{
    if ((m_subid == SUBID_BILLBOARD) && (m_state == STATE_BREAK))
        setFlyAwayParam(pObj);
};


bool CBreakableGimmick::billBoardEraseSlowly(void)
{
    if ((m_subid == SUBID_BILLBOARD) && (m_state == STATE_BREAK))
        return eraseSlowly();
    
    return false;
};


bool CBreakableGimmick::eraseSlowly(void)
{
    RwUInt8 decValue = static_cast<RwUInt8>(255.0f / CScreen::Framerate());

    CModel* pMdl = m_pModel->GetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK);
    ASSERT(pMdl);

    RwRGBA materialColor = pMdl->GetColor();
    materialColor.alpha -= decValue;
    pMdl->SetColor(materialColor);

    return (materialColor.alpha < decValue);
};


CBreakableGimmick::SUBID CBreakableGimmick::convertSubid(int32 subid) const
{
    SUBID retSubid = SUBID_FIREPLUG;

    switch (subid)
    {
    case 0:
    case 1:
        retSubid = SUBID_FIREPLUG;
        break;

    case 2:
    case 3:
        retSubid = SUBID_PC;
        break;

    case 4:
        retSubid = SUBID_SIGNAL;
        break;

    case 5:
        retSubid = SUBID_STREETLAMP;
        break;

    case 6:
        retSubid = SUBID_BILLBOARD;
        break;

    case 7:
        retSubid = SUBID_NEON;
        break;

    default:
        ASSERT(false);
        break;
    };
    
    return retSubid;
};