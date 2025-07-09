#include "MapEffectGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/Effect/Light.hpp"


CMapEffectLightGimmick::CMapEffectLightGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_Color({ 0xFF, 0xFF, 0xFF, 0xFF })
, m_hLight(0)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_BASIC* pBasic = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    m_vPosition = pBasic->m_vPosition;

    float afScale[3] = { 1.0f, 1.5f, 2.0f };
    ASSERT(pBasic->m_subid >= 0);
    ASSERT(pBasic->m_subid < COUNT_OF(afScale));

    m_hLight = CEffectLightManager::Regist(&m_vPosition, afScale[pBasic->m_subid], m_Color);
    ASSERT(m_hLight);
};


CMapEffectLightGimmick::~CMapEffectLightGimmick(void)
{
    if (m_hLight)
    {
        CEffectLightManager::Remove(m_hLight);
        m_hLight = 0;
    };
};


void CMapEffectLightGimmick::Draw(void) const
{
    ;
};


void CMapEffectLightGimmick::PostMove(void)
{
    ;
};


void CMapEffectLightGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    switch (eventtype)
    {
    case GIMMICKTYPES::EVENTTYPE_ACTIVATE:
        setLightEnable(true);
        break;

    case GIMMICKTYPES::EVENTTYPE_INACTIVATE:
        setLightEnable(false);
        break;

    default:
        break;
    };
};


void CMapEffectLightGimmick::setLightEnable(bool bEnable)
{
    m_Color.alpha = 0xFF;
    
    if (!bEnable)
        m_Color.alpha = 0x0;    

    if (m_hLight)
        CEffectLightManager::SetColor(m_hLight, m_Color);
};


//
// *********************************************************************************
//


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aSmokeEffectKindInfo[] =
{
    { "whitesmoke", false, true, 1.0f, 1.0f, 0.0f },
    { "whitesmoke", false, true, 1.0f, 2.0f, 0.0f },
    { "whitesmoke", false, true, 1.0f, 3.0f, 0.0f },
    { "blacksmoke", false, true, 1.0f, 1.0f, 0.0f },
    { "blacksmoke", false, true, 1.0f, 2.0f, 0.0f },
    { "blacksmoke", false, true, 1.0f, 3.0f, 0.0f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aFlameEffectKindInfo[] =
{
    { "fire_torch", false, true, 1.0f, 2.0f, 0.0f },
    { "fire_torch", false, true, 1.0f, 3.0f, 0.0f },
    { "fire_torch", false, true, 1.0f, 5.0f, 0.0f },
    { "fire_torch", true,  true, 1.0f, 2.0f, 0.0f },
    { "fire_torch", true,  true, 1.0f, 3.0f, 0.0f },
    { "fire_torch", true,  true, 1.0f, 5.0f, 0.0f },
    { "flame_map",  false, true, 1.0f, 1.0f, 0.0f },
    { "flame_map",  false, true, 1.0f, 2.0f, 0.0f },
    { "flame_map",  false, true, 1.0f, 3.0f, 0.0f },
    { "flame_map",  false, true, 1.0f, 4.0f, 0.0f },
    { "flame_map",  true,  true, 1.0f, 1.0f, 0.0f },
    { "flame_map",  true,  true, 1.0f, 2.0f, 0.0f },
    { "flame_map",  true,  true, 1.0f, 3.0f, 0.0f },
    { "flame_map",  true,  true, 1.0f, 4.0f, 0.0f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aSteamEffectKindInfo[] =
{
    { "pipe_steam", false, false, 6.0f, 1.0f, 0.0f },
    { "pipe_steam", true,  false, 6.0f, 1.0f, 0.0f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aSparkEffectKindInfo[] =
{
    { "sparks",      false, false, 1.0f, 1.0f, 0.0f },
    { "spark_slow",  false, true,  1.0f, 1.0f, 0.0f },
    { "enginespark", false, true,  1.0f, 1.0f, 0.0f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aExploEffectKindInfo[] =
{
    { "ko_expl", false, false, 0.6f, 0.8f, 1.5f },
    { "ko_expl", false, false, 1.2f, 1.6f, 2.5f },
    { "ko_expl", false, false, 1.8f, 2.4f, 3.5f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aBubbleEffectKindInfo[] =
{
    { "bubble", false, false, 1.5f, 1.0f, 2.0f },
    { "bubble", false, false, 1.0f, 1.0f, 2.0f },
    { "bubble", false, false, 0.5f, 1.0f, 2.0f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aWatterFalleMistEffectKindInfo[] =
{
    { "wfall_smoke", false, true, 0.6f, 0.5f, 2.0f },
    { "wfall_smoke", false, true, 0.6f, 1.0f, 2.0f },
    { "wfall_smoke", false, true, 0.6f, 1.5f, 2.0f },
};


/*static*/ const CMapEffectGimmick::EFFECTINFO CMapEffectGimmick::m_aWatterFalleSprashEffectKindInfo[] =
{
    { "wfall_sprash", false, true, 0.6f, 0.25f, 2.0f },
    { "wfall_sprash", false, true, 0.6f, 0.5f, 2.0f },
    { "wfall_sprash", false, true, 0.6f, 1.0f, 2.0f },
};


CMapEffectGimmick::CMapEffectGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_fTime(0.0f)
, m_hMagic(0)
, m_hEffect(0)
, m_pEffectInfo(nullptr)
, m_bEnable(true)
, m_bLoopActive(false)
{
    ASSERT(pParam);

    GIMMICKPARAM::GIMMICK_BASIC* pBasic = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);
    m_vPosition = pBasic->m_vPosition;
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pBasic->m_quat);
    RwV3dTransformPoint(&m_vDirection, &Math::VECTOR3_AXIS_Z, &matrix);
    
    setEffectInfo(pBasic->m_id, pBasic->m_subid);
    
    if (m_pEffectInfo && !m_pEffectInfo->m_bLoop)
        m_fTime = Math::RandFloatRange(0.0f, m_pEffectInfo->m_fIntervalTime);
};


CMapEffectGimmick::~CMapEffectGimmick(void)
{
    removeEffect();
};


void CMapEffectGimmick::PostMove(void)
{
    if (m_pEffectInfo && m_bEnable)
    {
        if (m_pEffectInfo->m_bLoop)
        {
            if (!CGimmickUtils::IsPositionVisible(&m_vPosition, 2.0f, true))
            {
                if (m_bLoopActive)
                {
                    removeEffect();
                    m_bLoopActive = false;
                };
            }
            else if(!m_bLoopActive)
            {
                m_bLoopActive = playEffect();
            };
        }
        else
        {
            if (m_pEffectInfo->m_fIntervalTime < m_fTime)
            {
                if (CGimmickUtils::IsPositionVisible(&m_vPosition, 2.0f, true))
                    playEffect();
                
                m_fTime -= m_pEffectInfo->m_fIntervalTime;
            };

            m_fTime += CGameProperty::GetElapsedTime();
        };
    };
};


void CMapEffectGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    switch (eventtype)
    {
    case GIMMICKTYPES::EVENTTYPE_ACTIVATE:
        {
            m_bEnable = true;
        }
        break;

    case GIMMICKTYPES::EVENTTYPE_INACTIVATE:
        {
            m_bEnable = false;
            
            if (m_pEffectInfo->m_bLoop)
            {
                if (m_bLoopActive)
                {
                    removeEffect();
                    m_bLoopActive = false;
                };
            };
        }
        break;

    default:
        break;
    };
};


void CMapEffectGimmick::setEffectInfo(int32 id, int32 subid)
{
    const EFFECTINFO* pEffectInfo = nullptr;

    switch (id)
    {
    case GIMMICKID::ID_N_SMOKE:
        ASSERT(subid < COUNT_OF(m_aSmokeEffectKindInfo));
        pEffectInfo = &m_aSmokeEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_FLAME:
        ASSERT(subid < COUNT_OF(m_aFlameEffectKindInfo));
        pEffectInfo = &m_aFlameEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_STEAM:
        ASSERT(subid < COUNT_OF(m_aSteamEffectKindInfo));
        pEffectInfo = &m_aSteamEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_SPARK:
        ASSERT(subid < COUNT_OF(m_aSparkEffectKindInfo));
        pEffectInfo = &m_aSparkEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_BUBBLE:
        ASSERT(subid < COUNT_OF(m_aBubbleEffectKindInfo));
        pEffectInfo = &m_aBubbleEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_MIST:
        ASSERT(subid < COUNT_OF(m_aWatterFalleMistEffectKindInfo));
        pEffectInfo = &m_aWatterFalleMistEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_SPLASH:
        ASSERT(subid < COUNT_OF(m_aWatterFalleSprashEffectKindInfo));
        pEffectInfo = &m_aWatterFalleSprashEffectKindInfo[subid];
        break;

    case GIMMICKID::ID_N_EXP:
        ASSERT(subid < COUNT_OF(m_aExploEffectKindInfo));
        pEffectInfo = &m_aExploEffectKindInfo[subid];
        break;

    default:
        break;
    };

    m_pEffectInfo = pEffectInfo;
};


void CMapEffectGimmick::removeEffect(void)
{
    if (m_hMagic)
    {
        CMagicManager::Finish(m_hMagic);
        m_hMagic = 0;
    };

    if (m_hEffect)
    {
        CEffectManager::Finish(m_hEffect);
        m_hEffect = 0;
    };
};


bool CMapEffectGimmick::playEffect(void)
{
    ASSERT(!m_hMagic);
    ASSERT(!m_hEffect);

    bool bResult = false;
    RwV3d vPosition = m_vPosition;

    if (!m_pEffectInfo->m_bLoop && m_pEffectInfo->m_fRandAreaRadius > 0.0f)
    {
        vPosition.x += Math::RandFloatRange(0.0f, m_pEffectInfo->m_fRandAreaRadius);
        vPosition.z += Math::RandFloatRange(0.0f, m_pEffectInfo->m_fRandAreaRadius);

        if (GetID() != GIMMICKID::ID_N_BUBBLE)
            vPosition.y += Math::RandFloatRange(0.0f, m_pEffectInfo->m_fRandAreaRadius);
    };

    float fScale = m_pEffectInfo->m_fScale;

    if (!m_pEffectInfo->m_bLoop)
        fScale *= Math::RandFloatRange(1.0f, 1.5f);

    if (m_pEffectInfo->m_bMagic)
    {
        CMagicManager::CParameter param;
        param.SetPositon(&vPosition);
        param.SetDirection(&m_vDirection);

        uint32 hMagic = CMagicManager::Play(m_pEffectInfo->m_pszName, &param);
        if (hMagic)
        {
            CMagicManager::SetScale(hMagic, fScale);
            if (m_pEffectInfo->m_bLoop)
                m_hMagic = hMagic;

            bResult = true;
        };
    }
    else
    {
        uint32 hEffect = CEffectManager::Play(m_pEffectInfo->m_pszName, &m_vPosition);
        if (hEffect)
        {
            CEffectManager::SetDirection(hEffect, &m_vDirection);
            CEffectManager::SetScale(hEffect, fScale);

            if (m_pEffectInfo->m_bLoop)
                m_hEffect = hEffect;

            bResult = true;
        };
    };

    return bResult;
};