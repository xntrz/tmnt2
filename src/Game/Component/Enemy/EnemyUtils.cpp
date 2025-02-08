#include "EnemyUtils.hpp"
#include "CharacterCompositor.hpp"
#include "EnemyCharacter.hpp"
#include "AIUtils.hpp"

#include "Game/Component/Shot/Shot.hpp"
#include "Game/Component/Effect/Effect.hpp"
#include "Game/Component/Effect/EffectGeneric.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/Magic.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/GameMain/GamePlayer.hpp"
#include "Game/Component/GameMain/StageInfo.hpp"
#include "Game/Component/GameMain/MapInfo.hpp"
#include "Game/Component/Module/AmbientLightModule.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Map/WorldMap.hpp"
#include "Game/System/Sound/GameSound.hpp"

#include <cctype> // tolower


/*static*/ const float CEnemyUtils::INVALID_WORLD_MAP_HEIGHT = -100.0f;


/*static*/ float CEnemyUtils::CModelColorControl::CalcColorChange(float src, float dst, float change)
{
    if (Math::FEqual(src, dst))
        return dst;

    float f = std::fabs(dst - src);

    if (f < change)
        change = f;

    return (src + change);
};


CEnemyUtils::CModelColorControl::CModelColorControl(void)
: m_bEnable(false)
, m_colorBase({ 0, 0, 0, 0 })
, m_colorSrc({ 0.0f, 0.0f, 0.0f })
, m_colorDst({ 0.0f, 0.0f, 0.0f })
, m_colorChange({ 0.0f, 0.0f, 0.0f })
, m_bLight(false)
, m_pChrCompositor(nullptr)
{
    ;
};


CEnemyUtils::CModelColorControl::~CModelColorControl(void)
{
    ;
};


void CEnemyUtils::CModelColorControl::Initialize(CCharacterCompositor* pChrCompositor)
{
    m_pChrCompositor = pChrCompositor;
    m_bEnable = false;
    m_bLight = true;
};


void CEnemyUtils::CModelColorControl::Period(void)
{
    if (!m_bEnable)
        return;

    m_colorSrc.r = CalcColorChange(m_colorSrc.r, m_colorDst.r, m_colorChange.r);
    m_colorSrc.g = CalcColorChange(m_colorSrc.g, m_colorDst.g, m_colorChange.g);
    m_colorSrc.b = CalcColorChange(m_colorSrc.b, m_colorDst.b, m_colorChange.b);

    CModel* pModel = m_pChrCompositor->GetModel();
    if (pModel)
    {
        RwRGBA color = m_colorBase;
        
        color.red   = static_cast<RwUInt8>(m_colorSrc.r);
        color.green = static_cast<RwUInt8>(m_colorSrc.g);
        color.blue  = static_cast<RwUInt8>(m_colorSrc.b);

        pModel->SetColor(color);
    };
};


void CEnemyUtils::CModelColorControl::SetModelColorChange(float r, float g, float b, float fTime)
{
    ASSERT(!Math::FEqual(fTime, 0.0f));

    m_colorDst = { r, g, b };

    m_colorChange = {
        std::fabs((m_colorDst.r - m_colorSrc.r) * (CGameProperty::GetElapsedTime() / fTime)),
        std::fabs((m_colorDst.g - m_colorSrc.g) * (CGameProperty::GetElapsedTime() / fTime)),
        std::fabs((m_colorDst.b - m_colorSrc.b) * (CGameProperty::GetElapsedTime() / fTime)),
    };
};


void CEnemyUtils::CModelColorControl::GetBaseModelColor(float* r, float* g, float* b)
{
    ASSERT(r && g && b);

    *r = static_cast<float>(m_colorBase.red);
    *g = static_cast<float>(m_colorBase.green);
    *b = static_cast<float>(m_colorBase.blue);
};


void CEnemyUtils::CModelColorControl::SetModelColorControlEnable(bool bEnable)
{
    if (!bEnable)
        return;

    if (m_bEnable)
        return;

    CModel* pModel = m_pChrCompositor->GetModel();
    if (pModel)
        m_colorBase = pModel->GetColor();

    m_colorSrc    = { 0.0f, 0.0f, 0.0f };
    m_colorDst    = { 0.0f, 0.0f, 0.0f };
    m_colorChange = { 0.0f, 0.0f, 0.0f };

};


void CEnemyUtils::CModelColorControl::SetAmbientLightEnable(bool bEnable)
{
    if (m_bLight != bEnable)
    {
        m_bLight = bEnable;
        CEnemyUtils::SetAmbientLightEnable(m_pChrCompositor, bEnable);
    };
};


//
// *********************************************************************************
//


CEnemyUtils::CKnockBackControl::CKnockBackControl(void)
: m_bControlEnabled(false)
, m_bNowControl(false)
, m_iDamageHeap(0)
, m_iDamageKnockBackControl(0)
, m_fTimer(0.0f)
, m_fTimerKnockBackControl(0.0f)
, m_pEnemyChr(nullptr)
{
    ;
};


CEnemyUtils::CKnockBackControl::~CKnockBackControl(void)
{
    ;
};


void CEnemyUtils::CKnockBackControl::Initialize(CEnemyCharacter* pEnemyChr,
                                                int32 iDamageKnockBackControl,
                                                float fTimerKnockBackControl)
{
    m_bNowControl     = false;
    m_iDamageHeap     = 0;
    m_fTimer          = 0.0f;
    m_bControlEnabled = true;
    m_pEnemyChr       = pEnemyChr;

    SetDamageKnockBackControl(iDamageKnockBackControl);
    SetTimeKnockBackControl(fTimerKnockBackControl);
};


void CEnemyUtils::CKnockBackControl::Period(void)
{
    if (!m_bControlEnabled)
        return;
    
    if (m_bNowControl)
    {
        ENEMYTYPES::STATUS status = m_pEnemyChr->GetStatus();

        if (!CEnemyUtils::IsStunState(status))
            m_pEnemyChr->SetFlag(ENEMYTYPES::FLAG_NOREACTION);
        else
            m_pEnemyChr->ClearFlag(ENEMYTYPES::FLAG_NOREACTION);

        if (m_fTimer < m_fTimerKnockBackControl)
        {
            if (!CEnemyUtils::IsKnockState(status)   &&
                !CEnemyUtils::IsFlyawayState(status) &&
                !CEnemyUtils::IsDownState(status)    &&
                !CEnemyUtils::IsStunState(status)    &&
                !CEnemyUtils::IsLiftState(status))
            {
                m_fTimer += CGameProperty::GetElapsedTime();
            };
        }
        else
        {
            m_bNowControl   = false;
            m_iDamageHeap   = 0;
            m_fTimer        = 0.0f;

            m_pEnemyChr->ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
        };
    }
    else if (m_iDamageHeap >= m_iDamageKnockBackControl)
    {
        m_bNowControl   = true;
        m_iDamageHeap   = 0;
        m_fTimer        = 0.0f;

        m_pEnemyChr->SetFlag(ENEMYTYPES::FLAG_NOREACTION);
    };
};


void CEnemyUtils::CKnockBackControl::ReceivedDamage(int32 iDamage)
{
    if (m_bControlEnabled && !m_bNowControl)
        m_iDamageHeap += iDamage;
};


void CEnemyUtils::CKnockBackControl::ClearKnockBackControl(void)
{    
    m_bNowControl = false;
    m_iDamageHeap = 0;
    m_fTimer      = 0.0f;

    m_pEnemyChr->ClearFlag(ENEMYTYPES::FLAG_NOREACTION);
};


void CEnemyUtils::CKnockBackControl::SetKnockBackControlEnable(bool bEnable)
{
    m_bControlEnabled = bEnable;

    ClearKnockBackControl();
};


void CEnemyUtils::CKnockBackControl::SetDamageKnockBackControl(int32 iDamageKnockBackControl)
{
    if (iDamageKnockBackControl >= 0)
    {
        m_iDamageKnockBackControl = iDamageKnockBackControl;
    }
    else
    {
        float fDamage = ((GetPlayerNumRate() * -40.0f) + 80.0f);

        GAMETYPES::DIFFICULTY difficulty = CGameProperty::GetDifficulty();
        switch (difficulty)
        {
        case GAMETYPES::DIFFICULTY_EASY: fDamage *= 1.25f; break;
        case GAMETYPES::DIFFICULTY_HARD: fDamage *= 0.75f; break;
        default: break;
        };

        m_iDamageKnockBackControl = static_cast<int32>(fDamage);
    };
};


void CEnemyUtils::CKnockBackControl::SetTimeKnockBackControl(float fTimerKnockBackControl)
{
    if (fTimerKnockBackControl >= 0.0f)
        m_fTimerKnockBackControl = fTimerKnockBackControl;
    else
        m_fTimerKnockBackControl = (GetPlayerNumRate() * 0.0f) + 0.5f;
};


float CEnemyUtils::CKnockBackControl::GetPlayerNumRate(void) const
{
    int32 playerNum = CAIUtils::GetPlayerNum();
    float fRate = static_cast<float>(playerNum - 1) / 3.0f;
    return Clamp(fRate, 0.0f, 1.0f);
};


//
// *********************************************************************************
//


/*static*/ MAPID::VALUE CEnemyUtils::GetNowMapID(void)
{
    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
    return CStageInfo::GetMapID(stageId);
};


/*static*/ bool CEnemyUtils::CheckNowMapForMapID(MAPID::VALUE idMap)
{
    /* compare current stage map */
    STAGEID::VALUE stageId = CGameData::PlayParam().GetStage();
    MAPID::VALUE stageMapId = CStageInfo::GetMapID(stageId);

    if (stageMapId)
        return (stageMapId == idMap);

    /**
     * compare map names if stage map invalid
     */

    /* extract world map name */
    const CWorldMap::MAPINFO* pMapInfo = CWorldMap::GetMapInfo();
    if (!pMapInfo)
        return false;

    char szWorldMapName[32];
    szWorldMapName[0] = '\0';

    ASSERT(std::strlen(pMapInfo->m_szName) < sizeof(szWorldMapName));
    std::strcpy(szWorldMapName, pMapInfo->m_szName);

    /* extract check map name */
    const char* pszMapName = CMapInfo::GetName(idMap);

    char szMapName[32];
    szMapName[0] = '\0';

    ASSERT(std::strlen(pszMapName) < sizeof(szMapName));
    std::strcpy(szMapName, pszMapName);

    /* compare their len */
    size_t lenWorldMapName = std::strlen(szWorldMapName);
    size_t lenMapName = std::strlen(szMapName);

    if (lenWorldMapName != lenMapName)
        return false;

    /* now convert to lower and compare names */
    for (size_t i = 0; i < lenWorldMapName; ++i)
        szWorldMapName[i] = static_cast<char>(std::tolower(static_cast<int>(szWorldMapName[i])));

    for (size_t i = 0; i < lenMapName; ++i)
        szMapName[i] = static_cast<char>(std::tolower(static_cast<int>(szMapName[i])));

    return (std::strcmp(szWorldMapName, szMapName) == 0);
};


/*static*/ float CEnemyUtils::GetEnemyHPRate(const CEnemyCharacter* pEnemyChr)
{
    const CEnemyCharacter::PARAMETER& chrParameter = pEnemyChr->CharacterParameter();
    const ENEMYTYPES::FEATURE& chrFeature = chrParameter.m_feature;

    float fHP = static_cast<float>(chrFeature.m_iHP);
    float fHPMax = static_cast<float>(chrFeature.m_iHPMax);

    ASSERT(!Math::FEqual(fHPMax, 0.0f));

    return (fHP / fHPMax);
};


/*static*/ float CEnemyUtils::RadianCorrect(float rad)
{
    return Math::RadianCorrect(rad);
};


/*static*/ uint32 CEnemyUtils::GetHandleFromHitObj(const CGameObject* pObj)
{
    GAMEOBJECTTYPE::VALUE objType = pObj->GetType();

    switch (objType)
    {
    case GAMEOBJECTTYPE::GIMMICK:
        break;

    case GAMEOBJECTTYPE::CHARACTER:
        if (static_cast<const CCharacter*>(pObj)->GetCharacterType() == CCharacter::TYPE_PLAYER)
            return static_cast<const CCharacter*>(pObj)->GetHandle();
        break;

    case GAMEOBJECTTYPE::EFFECT:
        if (static_cast<const CEffect*>(pObj)->GetType() == CEffect::TYPE_WITHHIT)
            return static_cast<const CMagic*>(pObj)->GetParent();
        break;

    case GAMEOBJECTTYPE::SHOT:
        return static_cast<const CShot*>(pObj)->GetParentHandle();

    default:
        ASSERT(false);
        break;
    };

    return 0;
};


/*static*/ void CEnemyUtils::BezierCalc(RwV3d*       result,
                                        const RwV3d* p0,
                                        const RwV3d* p1,
                                        const RwV3d* p2,
                                        float        t)
{
    ASSERT(result);
    ASSERT(p0);
    ASSERT(p1);
    ASSERT(p2);
    ASSERT(t >= 0.0f);
    ASSERT(t <= 1.0f);

    float t0 = (1.0f - t) * (1.0f - t);
    result->x = (p0->x * t0);
    result->y = (p0->y * t0);
    result->z = (p0->z * t0);

    float t1 = (2.0f * t * (1.0f - t));
    result->x += (p1->x * t1);
    result->y += (p1->y * t1);
    result->z += (p1->z * t1);

    float t2 = (t * t);
    result->x += (p2->x * t2);
    result->y += (p2->y * t2);
    result->z += (p2->z * t2);
};


/*static*/ bool CEnemyUtils::SetAmbientLightEnable(CCharacterCompositor* pChrCompositor, bool bEnable)
{
    CAmbientLightModule* pAmbientLightModule = static_cast<CAmbientLightModule*>(pChrCompositor->GetModule(MODULETYPE::AMBIENT_LIGHT));
    if (pAmbientLightModule)
        pAmbientLightModule->SetEnable(bEnable);
    
    return (pAmbientLightModule != nullptr);
};


/*static*/ bool CEnemyUtils::CheckInfoWater(const CCharacterCompositor* pChrCompositor)
{
    const CCharacter::COLLISIONGROUNDINFO* pGroundCollision = pChrCompositor->GetCollisionGround();

    return (pGroundCollision->m_bHit &&
            EFFECT_GENERIC::IsNeedWaterEffect(pGroundCollision->m_attribute));
};


/*static*/ int32 CEnemyUtils::FindPlayerNo(uint32 hObject)
{
    int32 playerNum = CGameProperty::GetPlayerNum();
    for (int32 i = 0; i < playerNum; ++i)
    {
        IGamePlayer* pGameplayer = CGameProperty::Player(i);
        if (pGameplayer->GetCurrentCharacter()->GetHandle() == hObject)
            return i;
    };

    return -1;
};


/*static*/ void CEnemyUtils::ProcAerialMotion(CCharacterCompositor* pChrCompositor, float fJumpInitSpeed)
{
    float fTime = 0.0f;

    if (fJumpInitSpeed != 0.0f)
    {
        RwV3d vVelocity = Math::VECTOR3_ZERO;
        pChrCompositor->GetVelocity(&vVelocity);

        fTime = (vVelocity.y / fJumpInitSpeed);
    };

    fTime = Clamp(fTime, -1.0f, 1.0f);

    fTime = pChrCompositor->GetMotionEndTime() * ((fTime * -0.5f) + 0.5f);
    pChrCompositor->SetMotionTime(fTime);
};


/*static*/ void CEnemyUtils::GetJumpSpeedPosToPos(RwV3d*        initSpeed,
                                                  float         fJumpInitSpeed,
                                                  const RwV3d*  jumpPos,
                                                  const RwV3d*  markPos,
                                                  float         fGravity)
{
    float fTime = CalcJumpTouchdownTime(fJumpInitSpeed, jumpPos->y, markPos->y, fGravity);
    if (!Math::FEqual(fTime, 0.0f))
    {
        initSpeed->x = (markPos->x - jumpPos->x) * (1.0f / fTime);
        initSpeed->y = fJumpInitSpeed;
        initSpeed->z = (markPos->z - jumpPos->z) * (1.0f / fTime);
    }
    else
    {
        initSpeed->x = 0.0f;
        initSpeed->y = fJumpInitSpeed;
        initSpeed->z = 0.0f;
    };
};


/*static*/ float CEnemyUtils::CalcJumpTouchdownTime(float fJumpInitSpeed, float fStartheight, float fEndHeight, float fGravity)
{
    ASSERT(!Math::FEqual(fGravity, 0.0f));

    fGravity = std::fabs(fGravity);

    float fHeight = GetJumpableHeight(fJumpInitSpeed, fGravity) + fStartheight - fEndHeight;

    float g = 1.0f / fGravity;
    if (fHeight <= 0.0f)
        return (g * fJumpInitSpeed) + (g * fJumpInitSpeed);
    
    return (g * fJumpInitSpeed) + std::sqrt((fHeight + fHeight) * g);
};


/*static*/ float CEnemyUtils::GetJumpableHeight(float fJumpInitSpeed, float fGravity)
{
    ASSERT(!Math::FEqual(fGravity, 0.0f));

    fGravity = std::fabs(fGravity);
    return (fJumpInitSpeed * fJumpInitSpeed) / (fGravity + fGravity);
};


/*static*/ float CEnemyUtils::GetDistance(const RwV3d* pos, const RwV3d* at)
{
    ASSERT(pos);
    ASSERT(at);

    RwV3d vec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vec, pos, at);

    return std::fabs(Math::Vec3_Length(&vec));
};


/*static*/ float CEnemyUtils::GetDistance(const CCharacterCompositor* pChrCompositor, const RwV3d* at)
{
    RwV3d pos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&pos);

    return GetDistance(&pos, at);
};


/*static*/ float CEnemyUtils::GetDistanceFromPlayer(int32 playerNo, const RwV3d* pos)
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(playerNo);
    if (!pPlayerChr)
        return static_cast<float>(TYPEDEF::UINT16_MAX);

    RwV3d vPlayerPos = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&vPlayerPos);

    RwV3d vec = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vec, &vPlayerPos, pos);
    vec.y = 0.0f;

    return std::fabs(Math::Vec3_Length(&vec));
};


/*static*/ float CEnemyUtils::GetDirection(const RwV3d* pos, const RwV3d* at)
{
    ASSERT(pos);
    ASSERT(at);
	ASSERT(!(Math::FEqual(at->x, pos->x) && Math::FEqual(at->z, pos->z)));

    return Math::ATan2(at->x - pos->x,
                       at->z - pos->z);
};


/*static*/ float CEnemyUtils::GetDirection(const CCharacterCompositor* pChrCompositor, const RwV3d* at)
{
    RwV3d pos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&pos);

    return GetDirection(&pos, at);
};


/*static*/ float CEnemyUtils::GetDirection(const RwV3d* at)
{
    return GetDirection(&Math::VECTOR3_ZERO, at);
};


/*static*/ int32 CEnemyUtils::RotateToPlayer(CCharacterCompositor* pChrCompositor, int32 playerNo, float fRotSpeed, float fRad)
{
    CPlayerCharacter* pPlayerChr = CAIUtils::GetActivePlayer(playerNo);
    if (!pPlayerChr)
        return -1;

    RwV3d pos = Math::VECTOR3_ZERO;
    pPlayerChr->GetFootPosition(&pos);

    return RotateToPosition(pChrCompositor, &pos, fRotSpeed, fRad);
};


/*static*/ int32 CEnemyUtils::RotateToPosition(CCharacterCompositor* pChrCompositor, const RwV3d* pos, float fRotSpeed, float fRad)
{
    RwV3d vecFootPos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&vecFootPos);

    RwV3d vecPos = *pos;
    vecPos.y = vecFootPos.y;

    if (Math::Vec3_IsEqual(&vecFootPos, &vecPos))
        return 0;        

    float fDir = GetDirection(&vecFootPos, &vecPos);
    pChrCompositor->RotateDirection(fDir, fRotSpeed);

    float fDiff = fDir - pChrCompositor->GetDirection();
    fDiff = RadianCorrect(fDiff);
    fDiff = std::fabs(fDiff);

    if (fDiff >= fRad)
        return 0;

    return 1;
};


/*static*/ bool CEnemyUtils::IsAttackState(ENEMYTYPES::STATUS status)
{
    return ((status >= ENEMYTYPES::STATUS_ATTACK_A) &&
            (status <= ENEMYTYPES::STATUS_FIRE_CC));
};


/*static*/ bool CEnemyUtils::IsStunState(ENEMYTYPES::STATUS status)
{
    return ((status >= ENEMYTYPES::STATUS_DINDLE) &&
            (status <= ENEMYTYPES::STATUS_BIND));
};


/*static*/ bool CEnemyUtils::IsKnockState(ENEMYTYPES::STATUS status)
{
    return ((status >= ENEMYTYPES::STATUS_KNOCK) &&
            (status <= ENEMYTYPES::STATUS_KNOCK_BACK));
};


/*static*/ bool CEnemyUtils::IsFlyawayState(ENEMYTYPES::STATUS status)
{
    switch (status)
    {
    case ENEMYTYPES::STATUS_FLYAWAY_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BACK:
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_FRONT:
    case ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK:
    case ENEMYTYPES::STATUS_THROWN_FRONT:
    case ENEMYTYPES::STATUS_THROWN_BACK:
        return true;

    default:
        return false;
    };
};


/*static*/ bool CEnemyUtils::IsDownState(ENEMYTYPES::STATUS status)
{
    return (((status >  ENEMYTYPES::STATUS_FLYAWAY_BOUND_BACK) &&
             (status <= ENEMYTYPES::STATUS_CRASHWALL_TOUCHDOWN_BACK)) ||
             (status == ENEMYTYPES::STATUS_GETUP));
};


/*static*/ bool CEnemyUtils::IsLiftState(ENEMYTYPES::STATUS status)
{
    return ((status >= ENEMYTYPES::STATUS_CAUGHT) &&
            (status <= ENEMYTYPES::STATUS_LIFTED_WALK));
};


/*static*/ bool CEnemyUtils::IsMoveState(ENEMYTYPES::STATUS status)
{
    return ((status >= ENEMYTYPES::STATUS_WALK) &&
            (status <= ENEMYTYPES::STATUS_DASH));
};


/*static*/ void CEnemyUtils::EntryTouchDownEffect(const CCharacterCompositor* pChrCompositor, float fScale, bool bPlaySound /*= true*/)
{
    RwV3d pos = Math::VECTOR3_ZERO;
    pChrCompositor->GetFootPosition(&pos);

    EFFECT_GENERIC::CallTouchDownEffect(pChrCompositor->GetCollisionGround()->m_attribute, &pos, fScale, bPlaySound);
};


/*static*/ void CEnemyUtils::EntryTouchDownEffect(const CEnemyCharacter* pEnemyChr, bool bPlaySound /*= true*/)
{
    float fScale = (pEnemyChr->CharacterParameter().m_fShadowRadius / 1.5f);

    EntryTouchDownEffect(&pEnemyChr->Compositor(), fScale, bPlaySound);
};


/*static*/ void CEnemyUtils::EntryNexusDeathEffect(const CEnemyCharacter* pEnemyChr, float fScale)
{
    RwV3d pos = Math::VECTOR3_ZERO;
    pEnemyChr->Compositor().GetFootPosition(&pos);

    uint32 hEffect = CEffectManager::Play(EFFECTID::ID_WARP_START, &pos);
    if (hEffect)
    {
        if (fScale < 0.0f)
            fScale = pEnemyChr->CharacterParameter().m_fShadowRadius * 0.5f;

        CEffectManager::SetScale(hEffect, fScale);
    };

    CGameSound::PlayObjectSE(&pEnemyChr->Compositor(), SDCODE_SE(4205));
};


/*static*/ void CEnemyUtils::PlayTouchdownSE(const CCharacterCompositor* pChrCompositor)
{
    bool bGround = false;

    bool bWater = CheckInfoWater(pChrCompositor);
    if (!bWater && (pChrCompositor->GetCollisionGround()->m_attribute != MAPTYPES::ATTRIBUTE_DEATH))
        bGround = true;

    int32 seCode = -1;

    ENEMYID::VALUE eID = pChrCompositor->GetID();
    switch (eID)
    {
    case ENEMYID::ID_ELITE_FOOT_DUMMY_D:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_C:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_B:
    case ENEMYID::ID_ELITE_FOOT_DUMMY_A:
    case ENEMYID::ID_ELITE_FOOT_D:
    case ENEMYID::ID_ELITE_FOOT_C:
    case ENEMYID::ID_ELITE_FOOT_B:
    case ENEMYID::ID_ELITE_FOOT_A:
        {
            if (bWater)
                seCode = SDCODE_SE(4209);
            else if (bGround)
                seCode = SDCODE_SE(4372);
        }
        break;

    case ENEMYID::ID_MIYAMOTO_USAGI:
        {
            if (bWater)
                seCode = SDCODE_SE(4117);
            else if (bGround)
                seCode = SDCODE_SE(4116);
        }
        break;

    case ENEMYID::ID_DORAKO:
    case ENEMYID::ID_TRAXIMUS:
    case ENEMYID::ID_SPIDER_MONSTER:
    case ENEMYID::ID_HUN_B:
    case ENEMYID::ID_HUN_A:
    case ENEMYID::ID_LEATHER_HEAD:
        {
            if (bWater)
                seCode = SDCODE_SE(4210);
            else if (bGround)
                seCode = SDCODE_SE(4207);
        }
        break;

    case ENEMYID::ID_ULTIMATE_SHREDDER:
    case ENEMYID::ID_FEUDAL_JAPAN_SHREDDER:
    case ENEMYID::ID_SHREDDER:
    case ENEMYID::ID_ULTIMATE_NINJA:
    case ENEMYID::ID_SPLINTER:
    case ENEMYID::ID_FOOT_MECH_SPLINTER:
    case ENEMYID::ID_SLASSHUR:
    case ENEMYID::ID_KARAI:
    case ENEMYID::ID_NINJA_RATS_D:
    case ENEMYID::ID_NINJA_RATS_C:
    case ENEMYID::ID_NINJA_RATS_B:
    case ENEMYID::ID_NINJA_RATS_A:
        {
            if (bWater)
                seCode = SDCODE_SE(4209);
            else if (bGround)
                seCode = SDCODE_SE(4208);
        }
        break;

    default:
        break;
    };

    if (seCode >= 0)
        CGameSound::PlayObjectSE(pChrCompositor, seCode);
};


/*static*/ void CEnemyUtils::EntryTouchDownEffectAndSE(const CEnemyCharacter* pEnemyChr)
{
    EntryTouchDownEffect(pEnemyChr);
    PlayTouchdownSE(&pEnemyChr->Compositor());
};


/*static*/ void CEnemyUtils::EntryFlyawayBoundEffectAndSE(const CEnemyCharacter* pEnemyChr)
{
    EntryTouchDownEffect(pEnemyChr, true);

    if (pEnemyChr->Compositor().GetCollisionGround()->m_attribute != MAPTYPES::ATTRIBUTE_DEATH)
        CGameSound::PlayObjectSE(&pEnemyChr->Compositor(), SDCODE_SE(4121));
};