#include "SpaceShipGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


CSpaceShipGimmick::CSpaceShipGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_generator()
, m_model()
, m_nLife(0)
, m_status(STATUS_IDLE)
, m_fTimer(0.0f)
, m_fInterval(0.0f)
, m_fDecreaseRate(0.0f)
, m_fSeTimer(0.0f)
, m_shipType(SHIPTYPE_UNKNOWN)
, m_aSmokeData()
, m_hAtari(0)
{
    GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE*>(pParam);

    ASSERT(pInitParam);
    ASSERT(pInitParam->m_subid >= 0);
    ASSERT(pInitParam->m_subid < SHIPTYPENUM);

    m_shipType = SHIPTYPE(pInitParam->m_subid);
    m_nLife = 100;
    m_fInterval = 0.08f;
    m_fDecreaseRate = 0.95f;
    
    std::memset(m_aSmokeData, 0, sizeof(m_aSmokeData));

    //
    //  init NORMAL DISP & ATARI model name
    //
    char szDisplModel[32];
    szDisplModel[0] = '\0';
    
    char szAtariModel[32];
    szAtariModel[0] = '\0';

    switch (m_shipType)
    {
    case SHIPTYPE_FED:
        std::strcpy(szDisplModel, "spaceship_f");
        std::strcpy(szAtariModel, "spaceship_f_a");
        break;

    case SHIPTYPE_TRI:
        std::strcpy(szDisplModel, "spaceship_t");
        std::strcpy(szAtariModel, "spaceship_t_a");
        break;

    default:
        ASSERT(false);
        break;
    };
    
    //
    //  init NORMAL DISP model
    //
    CModel* pModelTemp = CModelManager::CreateModel(szDisplModel);
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelTemp);

    //
    //  init NORMAL ATARI model
    //
    pModelTemp = CModelManager::CreateModel(szAtariModel);
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelTemp);
    
    //
    //  init BREAK DISP model
    //
    pModelTemp = CModelManager::CreateModel("break");
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_BREAK, pModelTemp);

    //
    //  INIT BREAK ATARI model
    //
    pModelTemp = CModelManager::CreateModel("break_a");
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_BREAK, pModelTemp);
    
    //
    //  init model position & rotation
    //
    RwV3d vRotation = Math::VECTOR3_ZERO;
    vRotation.y = CGimmickUtils::QuaternionToRotationY(&pInitParam->m_quat);

    m_model.SetRotation(&vRotation);
    m_model.SetPosition(&pInitParam->m_vPosition);
    m_model.UpdateFrame();
    
    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 16.0f);
    };

    //
    //  init generator
    //
    static const RwV3d s_aGeneratePointOffset[] =
    {
        {  0.0f, 1.2f, 1.0f },
        { -0.8f, 0.3f, 0.5f },
    };

    static_assert(COUNT_OF(s_aGeneratePointOffset) == SHIPTYPENUM, "should equal");

    m_generator.Init(pInitParam, true);
    m_generator.SetGeneratePoint(&s_aGeneratePointOffset[m_shipType], 0.0f);

    //
    //  setup model & movement strategy
    //
    SetModelStrategy(&m_model);
};


CSpaceShipGimmick::~CSpaceShipGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CSpaceShipGimmick::GetPosition(RwV3d* pvPosition) const
{
    if (m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL))
        m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)->GetPosition(pvPosition);
    else
        *pvPosition = Math::VECTOR3_ZERO;
};


bool CSpaceShipGimmick::Query(CGimmickQuery* pQuery) const
{
    if (m_generator.HandleQuery(pQuery))
        return true;

    if (pQuery->GetType() == GIMMICKTYPES::QUERY_EVE_BROKEN)
    {
        ++static_cast<CStateGimmickQuery*>(pQuery)->m_nTarget;

        if (m_status >= STATUS_EXPLOSION)
        {
            ++static_cast<CStateGimmickQuery*>(pQuery)->m_nState;
            return true;
        };
    };

    return CGimmick::Query(pQuery);
};


void CSpaceShipGimmick::PreMove(void)
{
    RegistHitAttack();

    switch (m_status)
    {
    case STATUS_IDLE:
    case STATUS_END:
        break;

    case STATUS_RUN:
        {
            m_generator.Run(CGameProperty::GetElapsedTime());
        }
        break;

    case STATUS_EXPLOSION:
        {
            if (ExplosionAndModelChange())
            {
                m_generator.Stop();
                m_status = STATUS_SMOKE_GENERATION;
            };
        }
        break;

    case STATUS_SMOKE_GENERATION:
        {
            if (SmokeOperation())
                m_status = STATUS_END;
        }
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CSpaceShipGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (m_status != STATUS_IDLE)
    {
        m_generator.HandleEvent(pszSender, eventtype);
        return;
    };

    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        m_status = STATUS_RUN;
        m_generator.Start();
    };
};


void CSpaceShipGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    if (m_status != STATUS_RUN)
        return;

    if ((pAttack->GetStatus() == CHitAttackData::STATUS_THROW) ||
        (pAttack->GetStatus() == CHitAttackData::STATUS_BIND))
        return;

    CGimmickUtils::PlayHitEffect(pAttack);

    m_nLife -= pAttack->GetPower();
    if (m_nLife < 0)
    {
        m_fTimer = 0.0f;
        m_status = STATUS_EXPLOSION;        
        CGameSound::PlayObjectSE(this, SDCODE_SE(4136));
    };
};


void CSpaceShipGimmick::RegistHitAttack(void)
{
    static const RwSphere s_aHitSphere[2][3] =
    {
        //
        //  FED
        //
        {
            { { -3.2f, 0.0f,  0.0f }, 1.6f },
            { {  0.0f, 0.0f,  0.6f }, 4.0f },
            { {  3.1f, 0.0f, -0.9f }, 2.2f },
        },

        //
        //  TRI
        //
        {
            { { -5.6f, 1.7f, 0.4f }, 3.8f },
            { { -0.9f, 1.8f, 1.9f }, 5.0f },
            { {  3.4f, 0.7f, 0.9f }, 4.6f },
        },
    };

    static_assert(COUNT_OF(s_aHitSphere) == SHIPTYPENUM, "should equal");

    CModel* pMdl = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
    RpClump* pClump = pMdl->GetClump();
    RwFrame* pFrame = RpClumpGetFrame(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

    for (int32 i = 0; i < COUNT_OF(s_aHitSphere[0]); ++i)
    {
        RwSphere hitSphere = s_aHitSphere[m_shipType][i];
        RwV3dTransformPoint(&hitSphere.center, &hitSphere.center, pMatrix);

        CHitCatchData hitCatch;
        hitCatch.Cleanup();
        hitCatch.SetObject(GetHandle());
        hitCatch.SetObjectType(GetType());
        hitCatch.SetShape(CHitCatchData::SHAPE_SPHERE);
        hitCatch.SetSphere(&hitSphere);

        CHitAttackManager::RegistCatch(&hitCatch);
    };
};


bool CSpaceShipGimmick::ExplosionAndModelChange(void)
{
    Explosion();

    if (m_model.GetDrawType())
        return (m_fInterval > 0.5f);

    if(m_fInterval < 0.05f)
    {
        m_fDecreaseRate = 2.0f;

        m_model.SetVisualBreak();
        m_model.SetCollisionBreak();

        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;

        if (m_model.GetCollisionModelClump())
        {
            RpClump* pClump = m_model.GetCollisionModelClump();

            m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump);
            ASSERT(m_hAtari);

            if (m_hAtari)
                CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 16.0f);
        };
        
        for (int32 i = 0; i < COUNT_OF(m_aSmokeData); ++i)
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_model.GetVisualModel()->GetPosition(&vPosition);

            float fBuffY = vPosition.y;

            GetRandomPosInsideBody(&vPosition);
            vPosition.y = fBuffY;

            uint32 hEffect = CEffectManager::Play("blacksmoke", &vPosition);
            ASSERT(hEffect);

            m_aSmokeData[i].m_hEffect = hEffect;
            m_aSmokeData[i].m_fSmokeTimer = 0.0f;
            m_aSmokeData[i].m_fSmokeGenerateTime = (10.0f + Math::RandFloatRange(0.0f, 3.0f));
            m_aSmokeData[i].m_status = SMOKEDATA::STATUS_GENERATION;
        };
    };

    return false;
};


void CSpaceShipGimmick::Explosion(void)
{
    static float s_aScale[] =
    {
        2.0f,
        3.5f,
    };

    static_assert(COUNT_OF(s_aScale) == SHIPTYPENUM, "should equal");

    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer >= m_fInterval)
    {
        RwV3d vPosition = Math::VECTOR3_ZERO;
        GetRandomPosInsideBody(&vPosition);

        uint32 hEffect = CEffectManager::Play("expl_b1", &vPosition);
        ASSERT(hEffect);

        if (hEffect)
            CEffectManager::SetScale(hEffect, s_aScale[m_shipType]);

        SeControl();

        m_fTimer = 0.0f;
        m_fInterval *= m_fDecreaseRate;
    };
};


void CSpaceShipGimmick::SeControl(void)
{
    m_fSeTimer += CGameProperty::GetElapsedTime();
    if (m_fSeTimer > 0.5f)
    {
        CGameSound::PlayObjectSE(this, SDCODE_SE(4200));
        m_fSeTimer = 0.0f;
    };
};


void CSpaceShipGimmick::GetRandomPosInsideBody(RwV3d* pvPos) const
{
    static const RwV3d s_aShipSize[] =
    {
        { 11.2f, 3.6f, 5.0f },
        { 15.0f, 5.4f, 8.0f },
        {  8.0f, 1.6f, 7.2f },
    };

    RwV3d vShipSize = Math::VECTOR3_ZERO;

    if (m_model.GetDrawType() != CNormalGimmickModel::MODELTYPE_DRAW_NORMAL)
        vShipSize = s_aShipSize[2];
    else
        vShipSize = (m_shipType == SHIPTYPE_TRI ? s_aShipSize[0] : s_aShipSize[1]);

    pvPos->x = float(Math::Rand() % uint32(vShipSize.x * 10.0f)) * 0.1f - vShipSize.x * 0.5f;
    pvPos->y = float(Math::Rand() % uint32(vShipSize.y * 10.0f)) * 0.1f;
    pvPos->z = float(Math::Rand() % uint32(vShipSize.z * 10.0f)) * 0.1f - vShipSize.z * 0.5f;

    CModel* pMdl = m_model.GetVisualModel();
    RpClump* pClump = pMdl->GetClump();
    RwFrame* pFrame = RpClumpGetFrame(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrix(pFrame);

    RwV3dTransformPoint(pvPos, pvPos, pMatrix);
};


bool CSpaceShipGimmick::SmokeOperation(void)
{
    //
    //  wait until all smoke goes to the end status
    //

    bool abSmokeStatus[COUNT_OF(m_aSmokeData)] = { false };

    for (int32 i = 0; i < COUNT_OF(m_aSmokeData); ++i)
        abSmokeStatus[i] = SmokeGeneration(&m_aSmokeData[i]);

    int32 nEndCnt = 0;
    for (int32 i = 0; i < COUNT_OF(abSmokeStatus); ++i)
        nEndCnt += (abSmokeStatus[i] ? 1 : 0);

    return (nEndCnt == COUNT_OF(m_aSmokeData));
};


bool CSpaceShipGimmick::SmokeGeneration(SMOKEDATA* pSmokeData)
{
    switch (pSmokeData->m_status)
    {
    case SMOKEDATA::STATUS_GENERATION:
        {
            pSmokeData->m_fSmokeTimer += CGameProperty::GetElapsedTime();
            if (pSmokeData->m_fSmokeTimer > pSmokeData->m_fSmokeGenerateTime)
            {
                CEffectManager::SetActive(pSmokeData->m_hEffect, false);
                pSmokeData->m_fSmokeTimer = 0.0f;
                pSmokeData->m_status = SMOKEDATA::STATUS_WAIT_ERASE;
            };
        }
        break;

    case SMOKEDATA::STATUS_WAIT_ERASE:
        {
            pSmokeData->m_fSmokeTimer += CGameProperty::GetElapsedTime();
            if (pSmokeData->m_fSmokeTimer > 2.0f)
            {
                CEffectManager::Finish(pSmokeData->m_hEffect);
                pSmokeData->m_hEffect = 0;                
                pSmokeData->m_fSmokeTimer = 0.0f;
                pSmokeData->m_status = SMOKEDATA::STATUS_END;
            };
        }
        break;

    default:
        return true;
    };

    return (pSmokeData->m_status >= SMOKEDATA::STATUS_END);
};