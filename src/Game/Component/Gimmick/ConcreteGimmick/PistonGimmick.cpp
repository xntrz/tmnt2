#include "PistonGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"


/*static*/ CPistonGimmick::STATE CPistonGimmick::m_eState;

/*static*/ const CPistonGimmick::SUBINFO CPistonGimmick::m_aSubInfo[] =
{
    { "piston0", "piston_a", 0.0f,                  MATH_DEG2RAD(61.2f) },
    { "piston1", "piston_a", MATH_DEG2RAD(36.0f),   MATH_DEG2RAD(61.2f) },
    { "piston2", "piston_a", 0.0f,                  MATH_DEG2RAD(61.2f) },
    { "piston3", "piston_a", MATH_DEG2RAD(108.0f),  MATH_DEG2RAD(61.2f) },
    { "piston4", "piston_a", 0.0f,                  MATH_DEG2RAD(61.2f) },
    { "piston5", "piston_a", MATH_DEG2RAD(36.0f),   MATH_DEG2RAD(61.2f) },
};


CPistonGimmick::CPistonGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_hAtari(0)
, m_subid(SUBID_NUM)
, m_fRotation(0.0f)
, m_fRotVelocity(0.0f)
, m_vInitPos(Math::VECTOR3_ZERO)
, m_vPos(Math::VECTOR3_ZERO)
, m_vPrePos(Math::VECTOR3_ZERO)
{
    static_assert(COUNT_OF(m_aSubInfo) == SUBID_NUM, "should equal");

    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    ASSERT(pInitParam->m_subid >= SUBID_START);
    ASSERT(pInitParam->m_subid < SUBID_NUM);

    m_subid         = static_cast<SUBID>(pInitParam->m_subid);
    m_vPos          = pInitParam->m_vPosition;
    m_vInitPos      = pInitParam->m_vPosition;
    m_fRotation     = m_aSubInfo[m_subid].fInitRot;
    m_fRotVelocity  = m_aSubInfo[m_subid].fRotVelocity;
    m_eState        = STATE_NONE;

    //
    //  init disp model
    //
    CModel* pModelTemp = CModelManager::CreateModel(m_aSubInfo[m_subid].pszModelName);
    ASSERT(pModelTemp);

    pModelTemp->SetLightingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelTemp);

    //
    //  init atari model
    //
    pModelTemp = CModelManager::CreateModel(m_aSubInfo[m_subid].pszAtariName);
    ASSERT(pModelTemp);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelTemp);

    //
    //  init model pos & rot
    //
    m_model.SetPosition(&pInitParam->m_vPosition);
    
    //
    //  setup model strat
    //
    SetModelStrategy(&m_model);
};


CPistonGimmick::~CPistonGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CPistonGimmick::PreMove(void)
{
    m_vPrePos = m_vPos;
};


void CPistonGimmick::PostMove(void)
{
    float fPrevRotation = m_fRotation;

    m_fRotation += (CGameProperty::GetElapsedTime() * m_fRotVelocity);
    m_fRotation = Math::RadianCorrection(m_fRotation);

    m_vPos.y = (m_vInitPos.y + (std::sin(m_fRotation) * 2.5f));
    m_model.SetPosition(&m_vPos);

    switch (m_eState)
    {
    case STATE_RUN:
        {
            const float fAngle = MATH_DEG2RAD(86.4f);

            if (((fPrevRotation <  fAngle) && (m_fRotation >=  fAngle)) ||
                ((fPrevRotation < -fAngle) && (m_fRotation >= -fAngle)))
                CGameSound::PlayObjectSE(this, SDCODE_SE(4219));

            if (!m_hAtari && m_model.GetCollisionModelClump())
            {
                RpClump* pClump = m_model.GetCollisionModelClump();

                m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
                if (m_hAtari)
                    CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 20.0f);
            };
        }
        break;

    case STATE_END:
        {
            if (m_hAtari)
            {
                CMapCollisionModel::RemoveCollisionModel(m_hAtari);
                m_hAtari = 0;
            };
        }
        break;

    default:
        break;
    };

    if (m_hAtari)
    {
        RwV3d vVelocity = { 0.0f,  (m_vPos.y - m_vPrePos.y), 0.0f };
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, &vVelocity);
    };
};


void CPistonGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    switch (m_eState)
    {
    case STATE_NONE:
        m_eState = STATE_RUN;    
        break;

    case STATE_RUN:
        m_eState = STATE_END;
        break;

    default:
        break;
    };
};