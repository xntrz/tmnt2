#include "BreakableCarGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/MessageManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


static const RwV3d s_vCarSize = { 1.5f, 1.0f, 4.0f };


CBreakableCarGimmick::CBreakableCarGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_nLife(50)
, m_fTimer(0.0f)
, m_fTimerExplosionInterval(0.0f)
, m_eState(STATE_IDLE)
, m_eSmokeState(SMOKESTATE_IDLE)
, m_model()
, m_ahEffect()
, m_hAtari(0)
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    ASSERT(pInitParam != nullptr);

    static const char* s_apszModelName[] =
    {
        "car1",
        "car2",
        "car3",
        "car4",
    };

    int32 subid = pInitParam->m_subid;

    ASSERT(subid >= 0);
    ASSERT(subid < COUNT_OF(s_apszModelName));

    //
    //  init normal draw model
    //
    CModel* pTempModel = CModelManager::CreateModel(s_apszModelName[subid]);
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pTempModel);

    //
    //  init normal atari model
    //
    char szTempModelA[256];
    szTempModelA[0] = '\0';
    
    std::sprintf(szTempModelA, "%s_a", s_apszModelName[subid]);

    pTempModel = CModelManager::CreateModel(szTempModelA);
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pTempModel);

    //
    //  init broken draw model
    //
    pTempModel = CModelManager::CreateModel("brokencar");
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pTempModel);

    //
    //  init broken atari model
    //
    pTempModel = CModelManager::CreateModel("brokencar_a");
    ASSERT(pTempModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_BREAK, pTempModel);

    m_model.SetDrawType(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    m_model.SetAtariType(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL);

    //
    //  init model pos & rot & type
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    
    m_model.SetDrawType(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    m_model.SetAtariType(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL);
	
	SetModelStrategy(&m_model);
    
    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 6.0f);
    };
};


CBreakableCarGimmick::~CBreakableCarGimmick(void)
{
    for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
    {
        if (m_ahEffect[i])
        {
            CEffectManager::Finish(m_ahEffect[i]);
            m_ahEffect[i] = 0;
        };
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CBreakableCarGimmick::Run(void)
{
    carControl();
};


void CBreakableCarGimmick::GetPosition(RwV3d* pvPosition) const
{
    if (m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL))
        m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)->GetPosition(pvPosition);
    else
        *pvPosition = Math::VECTOR3_ZERO;
};


void CBreakableCarGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_eState == STATE_IDLE)
    {
        if ((pAttack->GetStatus() == CHitAttackData::STATUS_THROW) ||
            (pAttack->GetStatus() == CHitAttackData::STATUS_BIND))
            return;

        m_nLife -= pAttack->GetPower();

        CGimmickUtils::PlayHitEffect(pAttack);
    };
};


void CBreakableCarGimmick::carControl(void)
{
    switch (m_eState)
    {
    case STATE_IDLE:
        {
            if (m_nLife >= 0)
            {
                setHitAttack();
            }
            else
            {
                m_eState = STATE_EXPLOSION_SIGN;
                m_fTimer = 0.0f;
            };
        }
        break;

    case STATE_EXPLOSION_SIGN:
        {
            if (smokeControl(false))
            {
                m_eState = STATE_EXPLOSION;
                m_fTimer = 0.0f;
                m_fTimerExplosionInterval = 0.0f;
            };
        }
        break;

    case STATE_EXPLOSION:
        {
            if (explosionControl())
            {
                m_eState = STATE_SMOLDER;
                m_fTimer = 0.0f;

                if (!(Math::Rand() % 3))
                    CMessageManager::Request(SEGROUPID::VALUE(25));
            };
        }
        break;

    case STATE_SMOLDER:
        {
            if (smokeControl(true))
            {
                m_eState = STATE_END;
                m_fTimer = 0.0f;
            };
        }
        break;

    case STATE_END:
        break;

    default:
        break;
    };
};


bool CBreakableCarGimmick::smokeControl(bool bWaitErase)
{
    bool bResult = false;

    m_fTimer += CGameProperty::GetElapsedTime();

    switch (m_eSmokeState)
    {
    case SMOKESTATE_IDLE:
        {
            for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                vPosition.x = Math::RandFloatRange(-1.0f, 1.0f) * 1.5f;
                vPosition.y = 0.5f;
                vPosition.z = Math::RandFloatRange(-1.0f, 1.0f) * 3.0f;
                
                RpClump* pClump = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)->GetClump();
                RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
                RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);
                
                RwV3dTransformPoint(&vPosition, &vPosition, pMatrix);

                m_ahEffect[i] = CEffectManager::Play(EFFECTID::ID_BLACKSMOKE, &vPosition);
            };

            m_eSmokeState   = SMOKESTATE_GENERATE;
            m_fTimer        = 0.0f;
        }
        break;

    case SMOKESTATE_GENERATE:
        {
            if (m_fTimer >= 2.5f)
            {
                for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
                {
                    if (m_ahEffect[i])
                        CEffectManager::SetActive(m_ahEffect[i], false);
                };

                m_fTimer        = 0.0f;
                m_eSmokeState   = (bWaitErase ? SMOKESTATE_WAIT : SMOKESTATE_IDLE);
                bResult         = (bWaitErase ? false : true);                
            };
        }
        break;

    case SMOKESTATE_WAIT:
        {
            if (m_fTimer >= 3.0f)
            {
                for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
                {
                    if (m_ahEffect[i])
                    {
                        CEffectManager::Finish(m_ahEffect[i]);
                        m_ahEffect[i] = 0;
                    };
                };

                m_fTimer        = 0.0f;
                m_eSmokeState   = SMOKESTATE_IDLE;
                bResult         = true;
            };
        }
        break;

    default:
        break;
    };

    return bResult;
};


bool CBreakableCarGimmick::explosionControl(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();
    m_fTimerExplosionInterval += CGameProperty::GetElapsedTime();

    //
    //  check explosion model
    //
    if ((m_model.GetDrawType() == CNormalGimmickModel::MODELTYPE_DRAW_NORMAL) && (m_fTimer >= 0.75f))
    {
        m_model.SetVisualBreak();
        m_model.SetCollisionBreak();

        if (m_model.GetCollisionModelClump())
        {
            ASSERT(m_hAtari);

            CMapCollisionModel::RemoveCollisionModel(m_hAtari);
            m_hAtari = 0;

            RpClump* pClump = m_model.GetCollisionModelClump();
            
            m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
            ASSERT(m_hAtari);

            if (m_hAtari)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 6.0f);
        };
    };

    //
    //  play explosion
    //
    if (m_fTimerExplosionInterval >= 0.2f)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        vPosition.x = Math::RandFloatRange(-1.0f, 1.0f) * s_vCarSize.x;
        vPosition.y = Math::RandFloatRange(-1.0f, 1.0f) * s_vCarSize.y;
        vPosition.z = Math::RandFloatRange(-1.0f, 1.0f) * s_vCarSize.z;

        CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
        RpClump* pClump = pModel->GetClump();
        RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

        RwV3dTransformPoint(&vPosition, &vPosition, pMatrix);

        uint32 hMagic = CMagicManager::Play(MAGICID::GetNameFromID(MAGICID::ID_EXPL_B1_CHARA), &vPosition);
        if (hMagic)
            CMagicManager::SetScale(hMagic, Math::RandFloatRange(1.0f, 3.0f));

        CGameSound::PlayObjectSE(this, SDCODE_SE(4201));

        m_fTimerExplosionInterval = 0.0f;
    };

    //
    //  wait for explosions
    //
    if (m_fTimer < 1.5f)
        return false;

    //
    //  shutdown smoke
    //
    for (int32 i = 0; i < COUNT_OF(m_ahEffect); ++i)
    {
        if (m_ahEffect[i])
        {
            CEffectManager::Finish(m_ahEffect[i]);
            m_ahEffect[i] = 0;
        };
    };

    return true;
};


void CBreakableCarGimmick::setHitAttack(void)
{
    const int32 catchNum = 3;
    const RwV3d vBasePos = { 0.0f, 0.5f, -1.5f };

    for (int32 i = 0; i < catchNum; ++i)
    {
        RwV3d vPosition = vBasePos;
        vPosition.z += (static_cast<float>(i) * 3.0f);

        CModel* pModel = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
        RpClump* pClump = pModel->GetClump();
        RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

        RwV3dTransformPoint(&vPosition, &vPosition, pMatrix);

        RwSphere hitSphere = { vPosition, 1.5f };

        CHitCatchData hitCatch;
        hitCatch.Cleanup();
        hitCatch.SetObject(GetHandle());
        hitCatch.SetObjectType(GetType());
        hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
        hitCatch.SetSphere(&hitSphere);

        CHitAttackManager::RegistCatch(&hitCatch);
    };
};