#include "AccumulateModule.hpp"

#include "Game/Component/Effect/Accumulate.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Enemy/CharacterCompositor.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Player/PlayerCharacter.hpp"
#include "Game/System/Character/Character.hpp"


CAccumulateUnit::CAccumulateUnit(void)
: m_pAccumulate(nullptr)
, m_fRadius(0.0f)
, m_fTime(0.0f)
, m_bInitialized(false)
{
    ;
};


CAccumulateUnit::~CAccumulateUnit(void)
{
    if (m_pAccumulate)
    {
        delete m_pAccumulate;
        m_pAccumulate = nullptr;
    };
};


void CAccumulateUnit::Run(void)
{
    if (!m_bInitialized)
        return;

    ASSERT(m_pAccumulate);
    
    m_fTime += CGameProperty::GetElapsedTime();

    float fRadius = (Math::Sin(m_fTime * (MATH_PI2 * 10.0f)) * 0.3f) * m_fRadius + m_fRadius;
	
    m_pAccumulate->SetTextureSize(fRadius, fRadius);
};


void CAccumulateUnit::Draw(void)
{
    if (m_bInitialized)
    {
        ASSERT(m_pAccumulate);
        m_pAccumulate->Draw();
    };
};


void CAccumulateUnit::Initialize(CModel* pModel, int32 nBoneID, const RwV3d* pvTop, const RwV3d* pvBottom)
{ 
    ASSERT(!m_pAccumulate);
    ASSERT(!m_bInitialized);
    
    if (m_bInitialized)
        return;

    m_pAccumulate = new CAccumulate(pModel, nBoneID, pvTop, pvBottom);
    m_bInitialized = true;
};


void CAccumulateUnit::SetRadius(float fRadius)
{
    if (m_bInitialized)
    {
        ASSERT(m_pAccumulate);
        m_pAccumulate->SetTextureSize(fRadius, fRadius);

        m_fRadius = fRadius;
    };
};


void CAccumulateUnit::SetColor(const RwRGBA& color)
{
    if (m_bInitialized)
    {
        ASSERT(m_pAccumulate);
        m_pAccumulate->SetColor(color);
    };
};


//
// *********************************************************************************
//


static const RwRGBA STEP_ZERO_COLOR  = { 0x00, 0x00, 0x00 ,0x00 };
static const RwRGBA STEP_ONE_COLOR   = { 0xFF, 0x33, 0x33 ,0xFF };
static const RwRGBA STEP_TWO_COLOR   = { 0xFF, 0x64, 0x64 ,0xFF };
static const RwRGBA STEP_THREE_COLOR = { 0xFF, 0xc8, 0xC8 ,0xFF };


/*static*/ CList<CAccumulateModule> CAccumulateModule::m_listDraw;


/*static*/  CAccumulateModule* CAccumulateModule::New(CCharacter* pCharacter)
{
    ASSERT(pCharacter);

    switch (pCharacter->GetCharacterType())
    {
    case CCharacter::TYPE_PLAYER:
        {
            CPlayerCharacter* pPlayerChr = static_cast<CPlayerCharacter*>(pCharacter);

            switch (pPlayerChr->GetID())
            {
            case PLAYERID::ID_LEO: return new CLeonardoAccumulateModule(pCharacter);
            case PLAYERID::ID_RAP: return new CRaphaelAccumulateModule(pCharacter);
            case PLAYERID::ID_MIC: return new CMichelangeroAccumulateModule(pCharacter);
            case PLAYERID::ID_DON: return new CDonatelloAccumulateModule(pCharacter);
            case PLAYERID::ID_SLA: return new CSlashuurAccumulateModule(pCharacter);
            case PLAYERID::ID_CAS: return new CCaseyAccumulateModule(pCharacter);
            case PLAYERID::ID_KAR: return new CKaraiAccumulateModule(pCharacter);
            case PLAYERID::ID_SPL: return new CSplinterAccumulateModule(pCharacter);
            default: ASSERT(false); break;
            };
        }
        break;

    case CCharacter::TYPE_ENEMY:
        {
            CCharacterCompositor* pChrCompositor = static_cast<CCharacterCompositor*>(pCharacter);

            switch (pChrCompositor->GetID())
            {
            case ENEMYID::ID_SLASSHUR:           return new CEnemySlashuurAccumulateModule(pCharacter);
            case ENEMYID::ID_FOOT_MECH_SPLINTER: return new CSplinterAccumulateModule(pCharacter);
            case ENEMYID::ID_SPLINTER:           return new CSplinterAccumulateModule(pCharacter);
            default: ASSERT(false); break;
            };
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return nullptr;
};


/*static*/ void CAccumulateModule::DrawAllAccumulate(void)
{
    for (CAccumulateModule& it : m_listDraw)
        it.DrawAccumulate();    
};


CAccumulateModule::CAccumulateModule(CCharacter* pCharacter)
: IModule(MODULETYPE::ACCUMULATE)
, m_pCharacter(pCharacter)
, m_hEffectSmall(0)
, m_hEffectMedium(0)
, m_hEffectLarge(0)
, m_bDraw(false)
{
    ASSERT(m_pCharacter);
    
    m_nodeDraw.data = this;
    m_listDraw.push_back(&m_nodeDraw);    
};


/*virtual*/ CAccumulateModule::~CAccumulateModule(void)
{
    ASSERT(!m_listDraw.empty());
    m_listDraw.erase(&m_nodeDraw);
};


/*virtual*/ void CAccumulateModule::Run(void) /*override*/
{
    for (CAccumulateUnit& it : m_listAccumulate)
        it.Run();

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetBodyPosition(&vPosition);

    EffectSetPosition(m_hEffectSmall, &vPosition);
    EffectSetPosition(m_hEffectMedium, &vPosition);
    EffectSetPosition(m_hEffectLarge, &vPosition);
};


/*virtual*/ void CAccumulateModule::Draw(void) /*override*/
{
    ;
};


void CAccumulateModule::DrawAccumulate(void)
{
    if (!m_bDraw)
        return;
    
    for (CAccumulateUnit& it : m_listAccumulate)
        it.Draw();
};


void CAccumulateModule::SetDrawOn(void)
{
    m_bDraw = true;
};


void CAccumulateModule::SetDrawOff(void)
{
    m_bDraw = false;
};


void CAccumulateModule::SetEffectOff(void)
{
    EffectOff(m_hEffectSmall);
    EffectOff(m_hEffectMedium);
    EffectOff(m_hEffectLarge);
};


void CAccumulateModule::SetStepZero(void)
{
    for (CAccumulateUnit& it : m_listAccumulate)
        it.SetColor(STEP_ZERO_COLOR);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetBodyPosition(&vPosition);

    m_hEffectSmall = CEffectManager::Play(EFFECTID::ID_ALL_TAME1, &vPosition);
    
    EffectOff(m_hEffectMedium);
    EffectOff(m_hEffectLarge);
};


void CAccumulateModule::SetStepOne(void)
{
    for (CAccumulateUnit& it : m_listAccumulate)
        it.SetColor(STEP_ONE_COLOR);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetBodyPosition(&vPosition);

    m_hEffectMedium = CEffectManager::Play(EFFECTID::ID_ALL_TAME2, &vPosition);

    EffectOff(m_hEffectSmall);
    EffectOff(m_hEffectLarge);

    vPosition.y += 0.025f;
    CEffectManager::Play(EFFECTID::ID_ALL_TAME_FLASH, &vPosition);
};


void CAccumulateModule::SetStepTwo(void)
{
    for (CAccumulateUnit& it : m_listAccumulate)
        it.SetColor(STEP_TWO_COLOR);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetBodyPosition(&vPosition);

    m_hEffectLarge = CEffectManager::Play(EFFECTID::ID_ALL_TAME3, &vPosition);

    EffectOff(m_hEffectSmall);
    EffectOff(m_hEffectMedium);

    vPosition.y += 0.025f;
    CEffectManager::Play(EFFECTID::ID_ALL_TAME_FLASH, &vPosition);
};


void CAccumulateModule::SetStepThree(void)
{
    for (CAccumulateUnit& it : m_listAccumulate)
        it.SetColor(STEP_THREE_COLOR);

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_pCharacter->GetBodyPosition(&vPosition);
    vPosition.y += 0.025f;
    
    CEffectManager::Play(EFFECTID::ID_ALL_TAME_FLASH, &vPosition);
};


void CAccumulateModule::RegistUnit(CAccumulateUnit* pAccumulateUnit)
{
    ASSERT(pAccumulateUnit);
    m_listAccumulate.push_front(pAccumulateUnit);
};


void CAccumulateModule::EffectOff(uint32& hEffect)
{
    if (hEffect)
    {
        CEffectManager::Finish(hEffect);
        hEffect = 0;
    };
};


void CAccumulateModule::EffectSetPosition(uint32 hEffect, const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    if (hEffect)
        CEffectManager::SetPosition(hEffect, pvPosition);
};


//
// *********************************************************************************
//


CLeonardoAccumulateModule::CLeonardoAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d LEONARDO_TOP_RIGHT   = {  0.02f,  1.0f,   0.02f  };
    static const RwV3d LEONARDO_BOTTOM_RIGHT= { -0.01f, -0.3f,  -0.01f  };
    static const RwV3d LEONARDO_TOP_LEFT    = {  0.0f,   1.0f,  -0.01f  };
    static const RwV3d LEONARDO_BOTTOM_LEFT = {  0.01f, -0.3f,  -0.01f  };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &LEONARDO_TOP_RIGHT, &LEONARDO_BOTTOM_RIGHT);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    m_aAccumulateUnit[1].Initialize(pModel, 8, &LEONARDO_TOP_LEFT, &LEONARDO_BOTTOM_LEFT);
    m_aAccumulateUnit[1].SetRadius(0.15f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
};


//
// *********************************************************************************
//


CRaphaelAccumulateModule::CRaphaelAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d RAPHAEL_TOP_RIGHT    = { 0.0f,   0.6f,  0.0f   };
    static const RwV3d RAPHAEL_BOTTOM_RIGHT = { 0.0f,  -0.1f,  0.0f   };
    static const RwV3d RAPHAEL_TOP_LEFT     = { 0.0f,   0.6f,  0.0f   };
    static const RwV3d RAPHAEL_BOTTOM_LEFT  = { 0.0f,  -0.1f,  0.0f   };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &RAPHAEL_TOP_RIGHT, &RAPHAEL_BOTTOM_RIGHT);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    m_aAccumulateUnit[1].Initialize(pModel, 8, &RAPHAEL_TOP_LEFT, &RAPHAEL_BOTTOM_LEFT);
    m_aAccumulateUnit[1].SetRadius(0.15f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
};


//
// *********************************************************************************
//


CMichelangeroAccumulateModule::CMichelangeroAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d MIC_TOP_RIGHT        = { 0.0f,    0.21f,     0.0f };
    static const RwV3d MIC_BOTTOM_RIGHT     = { 0.0f,    -0.1f,     0.0f };
    static const RwV3d MIC_TOP_LEFT         = { 0.0f,    0.31f,     0.0f };
    static const RwV3d MIC_BOTTOM_LEFT      = { 0.0f,   -0.05f,     0.0f };
    static const RwV3d MIC_TOP_RIGHT_S      = { 0.0f,     0.4f,     0.0f };
    static const RwV3d MIC_BOTTOM_RIGHT_S   = { 0.0f,    -0.1f,     0.0f };
    static const RwV3d MIC_TOP_LEFT_S       = { 0.0f,     0.5f,     0.0f };
    static const RwV3d MIC_BOTTOM_LEFT_S    = { 0.0f,     0.0f,     0.0f };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &MIC_TOP_RIGHT, &MIC_BOTTOM_RIGHT);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    m_aAccumulateUnit[1].Initialize(pModel, 12, &MIC_TOP_RIGHT_S, &MIC_BOTTOM_RIGHT_S);
    m_aAccumulateUnit[1].SetRadius(0.15f);

    m_aAccumulateUnit[2].Initialize(pModel, 8, &MIC_TOP_LEFT, &MIC_BOTTOM_LEFT);
    m_aAccumulateUnit[2].SetRadius(0.15f);

    m_aAccumulateUnit[3].Initialize(pModel, 13, &MIC_TOP_LEFT_S, &MIC_BOTTOM_LEFT_S);
    m_aAccumulateUnit[3].SetRadius(0.15f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
    RegistUnit(&m_aAccumulateUnit[2]);
    RegistUnit(&m_aAccumulateUnit[3]);
};


//
// *********************************************************************************
//


CDonatelloAccumulateModule::CDonatelloAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d DON_TOP_RIGHT    = { 0.0f,   1.05f,   -0.02f };
    static const RwV3d DON_BOTTOM_RIGHT = { 0.0f,   -1.1f,    0.0f  };    

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &DON_TOP_RIGHT, &DON_BOTTOM_RIGHT);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    RegistUnit(&m_aAccumulateUnit[0]);
};


//
// *********************************************************************************
//


CSlashuurAccumulateModule::CSlashuurAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d SLASHUUR_STICK_TOP           = { -0.7f,   0.0f,  0.0f };
    static const RwV3d SLASHUUR_STICK_BOTTOM        = {  1.3f,   0.0f,  0.0f };
    static const RwV3d SLASHUUR_NECK_TOP            = {  1.23f,  0.0f,  0.0f };
    static const RwV3d SLASHUUR_NECK_BOTTOM         = {  1.4f,  -0.25f, 0.0f };
    static const RwV3d SLASHUUR_OCCIPITAL_TOP       = {  1.28f, -0.45f, 0.0f };
    static const RwV3d SLASHUUR_OCCIPITAL_BOTTOM    = {  1.45f, -0.09f, 0.0f };
    static const RwV3d SLASHUUR_BEAK_TOP            = {  1.45f, -0.09f, 0.0f };
    static const RwV3d SLASHUUR_BEAK_BOTTOM         = {  1.38f,  0.47f, 0.0f };
    static const RwV3d SLASHUUR_BEAK2_TOP           = {  1.38f,  0.47f, 0.0f };
    static const RwV3d SLASHUUR_BEAK2_BOTTOM        = {  1.2f,   0.95f, 0.0f };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &SLASHUUR_STICK_TOP, &SLASHUUR_STICK_BOTTOM);
    m_aAccumulateUnit[0].SetRadius(0.2f);

    m_aAccumulateUnit[1].Initialize(pModel, 7, &SLASHUUR_NECK_TOP, &SLASHUUR_NECK_BOTTOM);
    m_aAccumulateUnit[1].SetRadius(0.31f);

    m_aAccumulateUnit[2].Initialize(pModel, 7, &SLASHUUR_OCCIPITAL_TOP, &SLASHUUR_OCCIPITAL_BOTTOM);
    m_aAccumulateUnit[2].SetRadius(0.31f);

    m_aAccumulateUnit[3].Initialize(pModel, 7, &SLASHUUR_BEAK_TOP, &SLASHUUR_BEAK_BOTTOM);
    m_aAccumulateUnit[3].SetRadius(0.31f);

    m_aAccumulateUnit[4].Initialize(pModel, 7, &SLASHUUR_BEAK2_TOP, &SLASHUUR_BEAK2_BOTTOM);
    m_aAccumulateUnit[4].SetRadius(0.31f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
    RegistUnit(&m_aAccumulateUnit[2]);
    RegistUnit(&m_aAccumulateUnit[3]);
    RegistUnit(&m_aAccumulateUnit[4]);
};


//
// *********************************************************************************
//


CCaseyAccumulateModule::CCaseyAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d CASEY_SHAFT_TOP      = {  0.0f,  0.9f, 0.0f };
    static const RwV3d CASEY_SHAFT_BOTTOM   = {  0.0f, -0.2f, 0.0f };
    static const RwV3d CASEY_TIP_TOP        = {  0.0f,  0.9f, 0.0f };
    static const RwV3d CASEY_TIP_BOTTOM     = { -0.35f, 0.9f, 0.0f };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &CASEY_SHAFT_TOP, &CASEY_SHAFT_BOTTOM);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    m_aAccumulateUnit[1].Initialize(pModel, 7, &CASEY_TIP_TOP, &CASEY_TIP_BOTTOM);
    m_aAccumulateUnit[1].SetRadius(0.3f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
};


//
// *********************************************************************************
//


CKaraiAccumulateModule::CKaraiAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d KARAI_RIGHT_TOP      = {  0.0f,  0.8f, 0.0f };
    static const RwV3d KARAI_RIGHT_BOTTOM   = {  0.0f, -0.2f, 0.0f };
    static const RwV3d KARAI_LEFT_TOP       = {  0.0f,  0.8f, 0.0f };
    static const RwV3d KARAI_LEFT_BOTTOM    = {  0.0f, -0.2f, 0.0f };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &KARAI_RIGHT_TOP, &KARAI_RIGHT_BOTTOM);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    m_aAccumulateUnit[1].Initialize(pModel, 8, &KARAI_LEFT_TOP, &KARAI_LEFT_BOTTOM);
    m_aAccumulateUnit[1].SetRadius(0.3f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
};


//
// *********************************************************************************
//


CSplinterAccumulateModule::CSplinterAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
{
    static const RwV3d SPLINTER_RIGHT_TOP       = { -0.09f, -0.03f, -0.5f  };
    static const RwV3d SPLINTER_RIGHT_BOTTOM    = {  0.05f,  0.01f,  0.35f };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &SPLINTER_RIGHT_TOP, &SPLINTER_RIGHT_BOTTOM);
    m_aAccumulateUnit[0].SetRadius(0.15f);

    RegistUnit(&m_aAccumulateUnit[0]);
};


//
// *********************************************************************************
//


CEnemySlashuurAccumulateModule::CEnemySlashuurAccumulateModule(CCharacter* pCharacter)
: CAccumulateModule(pCharacter)
, m_color({ 0x64, 0x32, 0xFF, 0xFF })
, m_fFadeTiming(0.0f)
, m_fNowTime(0.0f)
, m_step(STEP_IDLE)
{
    static const RwV3d SLASHUUR_STICK_TOP           = { -0.7f,   0.0f,  0.0f };
    static const RwV3d SLASHUUR_STICK_BOTTOM        = {  1.3f,   0.0f,  0.0f };
    static const RwV3d SLASHUUR_NECK_TOP            = {  1.23f,  0.0f,  0.0f };
    static const RwV3d SLASHUUR_NECK_BOTTOM         = {  1.4f,  -0.25f, 0.0f };
    static const RwV3d SLASHUUR_OCCIPITAL_TOP       = {  1.28f, -0.45f, 0.0f };
    static const RwV3d SLASHUUR_OCCIPITAL_BOTTOM    = {  1.45f, -0.09f, 0.0f };
    static const RwV3d SLASHUUR_BEAK_TOP            = {  1.45f, -0.09f, 0.0f };
    static const RwV3d SLASHUUR_BEAK_BOTTOM         = {  1.38f,  0.47f, 0.0f };
    static const RwV3d SLASHUUR_BEAK2_TOP           = {  1.38f,  0.47f, 0.0f };
    static const RwV3d SLASHUUR_BEAK2_BOTTOM        = {  1.2f,   0.95f, 0.0f };

    CModel* pModel = m_pCharacter->GetModel();
    ASSERT(pModel);

    m_aAccumulateUnit[0].Initialize(pModel, 7, &SLASHUUR_STICK_TOP, &SLASHUUR_STICK_BOTTOM);
    m_aAccumulateUnit[0].SetRadius(0.2f);

    m_aAccumulateUnit[1].Initialize(pModel, 7, &SLASHUUR_NECK_TOP, &SLASHUUR_NECK_BOTTOM);
    m_aAccumulateUnit[1].SetRadius(0.31f);

    m_aAccumulateUnit[2].Initialize(pModel, 7, &SLASHUUR_OCCIPITAL_TOP, &SLASHUUR_OCCIPITAL_BOTTOM);
    m_aAccumulateUnit[2].SetRadius(0.31f);

    m_aAccumulateUnit[3].Initialize(pModel, 7, &SLASHUUR_BEAK_TOP, &SLASHUUR_BEAK_BOTTOM);
    m_aAccumulateUnit[3].SetRadius(0.31f);

    m_aAccumulateUnit[4].Initialize(pModel, 7, &SLASHUUR_BEAK2_TOP, &SLASHUUR_BEAK2_BOTTOM);
    m_aAccumulateUnit[4].SetRadius(0.31f);

    RegistUnit(&m_aAccumulateUnit[0]);
    RegistUnit(&m_aAccumulateUnit[1]);
    RegistUnit(&m_aAccumulateUnit[2]);
    RegistUnit(&m_aAccumulateUnit[3]);
    RegistUnit(&m_aAccumulateUnit[4]);
};


/*virtual*/ void CEnemySlashuurAccumulateModule::Run(void) /*override*/
{
    if (m_step == STEP_IDLE)
        return;
    
    for (CAccumulateUnit& it : m_listAccumulate)
    {
        it.Run();

        if (m_step)
            it.SetColor(m_color);
    };

    switch (m_step)
    {
    case STEP_FADE_IN:
        {
            if (m_fNowTime <= m_fFadeTiming)
            {
                m_color.alpha += static_cast<RwUInt8>((255.0f / m_fFadeTiming) * CGameProperty::GetElapsedTime());
            }            
            else
            {
                m_step = STEP_FADE_OUT;
                m_color.alpha = 0xFF;
            };
        }
        break;

    case STEP_FADE_OUT:
        {
            if (m_fNowTime <= (m_fFadeTiming * 2.0f))
            {
                m_color.alpha -= static_cast<RwUInt8>((255.0f / m_fFadeTiming) * CGameProperty::GetElapsedTime());
            }
            else
            {
                m_step = STEP_IDLE;
                m_color.alpha = 0x00;
				SetDrawOff();
            };
        }
        break;

    default:
        break;
    };

    m_fNowTime += CGameProperty::GetElapsedTime();
};


/*virtual*/ void CEnemySlashuurAccumulateModule::Draw(void) /*override*/
{
    ;
};


void CEnemySlashuurAccumulateModule::Fade(float fFadeTiming)
{
    m_fFadeTiming = fFadeTiming;
    Fade();
};


void CEnemySlashuurAccumulateModule::Fade(void)
{
    m_step = STEP_FADE_IN;
    m_fNowTime = 0.0f;
    m_color.alpha = 0;
	SetDrawOn();
};


void CEnemySlashuurAccumulateModule::SetColor(const RwRGBA& color)
{
    m_color = color;
};


void CEnemySlashuurAccumulateModule::SetColor(const RwRGBA* color)
{
    m_color = *color;
};