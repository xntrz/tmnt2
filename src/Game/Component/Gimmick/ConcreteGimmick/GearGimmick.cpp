#include "GearGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/RenderState.hpp"


/*static*/ CGearGimmick::STATE CGearGimmick::m_eState;


/*static*/ const CGearGimmick::SUBINFO CGearGimmick::m_aSubInfo[] =
{
    { "engine",         "",              0.0f,  false,   MATH_DEG2RAD(7.2f),     MATH_DEG2RAD(7.2f)             },
    { "core",           "",              0.0f,  false,  -MATH_DEG2RAD(18.0f),   -MATH_DEG2RAD(18.0f)            },
    { "gear_01",        "",              0.0f,  false,   MATH_DEG2RAD(72.0f),   (MATH_DEG2RAD(72.0f) * 0.5f)    },
    { "gear_02",        "",              0.0f,  false,   MATH_DEG2RAD(54.0f),   (MATH_DEG2RAD(54.0f) * 0.5f)    },
    { "gear_03",        "gear_03a",     20.0f,  false,   MATH_DEG2RAD(63.0f),    MATH_DEG2RAD(32.4f)            },
    { "gear_04",        "gear_04a",     10.0f,  false,   MATH_DEG2RAD(68.4f),    MATH_DEG2RAD(36.0f)            },
    { "gear_06",        "gear_06a",     10.0f,  false,   MATH_DEG2RAD(75.6f),    MATH_DEG2RAD(45.0f)            },
    { "gear_07",        "gear_07a",     16.0f,  false,  -MATH_DEG2RAD(81.0f),   -MATH_DEG2RAD(54.0f)            },
    { "gear_08",        "gear_08a",     10.0f,  false,  -MATH_DEG2RAD(81.0f),   -MATH_DEG2RAD(54.0f)            },
    { "gear_d01",       "gear_d01a",    20.0f,  false,  -MATH_DEG2RAD(270.0f),  -MATH_DEG2RAD(90.0f)            },
    { "gear_d02",       "gear_d02a",    20.0f,  false,  -MATH_DEG2RAD(81.0f),   -MATH_DEG2RAD(54.0f)            },
    { "energy",         "",              0.0f,  false,  -MATH_DEG2RAD(7.2f),    -MATH_DEG2RAD(7.2f)             },
    { "energy_light",   "",              0.0f,  true,    MATH_DEG2RAD(7.2f),     MATH_DEG2RAD(7.2f)             },
    { "energy_gard",    "",              0.0f,  false,   MATH_DEG2RAD(7.2f),     MATH_DEG2RAD(7.2f)             },
    { "core_light_02",  "",              0.0f,  false,  -MATH_DEG2RAD(18.0f),   -MATH_DEG2RAD(18.0f)            },
    { "core_light_03",  "",              0.0f,  false,   MATH_DEG2RAD(18.0f),    MATH_DEG2RAD(18.0f)            },
    { "core_light_01",  "",              0.0f,  true,   -MATH_DEG2RAD(18.0f),   -MATH_DEG2RAD(18.0f)            },
};


CGearGimmick::CGearGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_hAtari(0)
, m_subid(SUBID_NUM)
, m_fRotation(0.0f)
, m_fRotVelocity(0.0f)
{
    static_assert(COUNT_OF(m_aSubInfo) == SUBID_NUM, "should equal");
    
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    ASSERT(pInitParam->m_subid >= SUBID_START);
    ASSERT(pInitParam->m_subid <  SUBID_NUM);

    m_subid = SUBID(pInitParam->m_subid);
    m_fRotVelocity = m_aSubInfo[m_subid].fRotVelocity[0];
    m_eState = STATE_NONE;

    //
    //  init display model
    //
    CModel* pModelTemp = CModelManager::CreateModel(m_aSubInfo[m_subid].pszModelName);
    ASSERT(pModelTemp);
    
    pModelTemp->SetLightingEnable(false);
    
    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelTemp);

    //
    //  init atari model if exist
    //
    if (std::strcmp(m_aSubInfo[m_subid].pszAtariName, "") != 0)
    {
        pModelTemp = CModelManager::CreateModel(m_aSubInfo[m_subid].pszAtariName);
        ASSERT(pModelTemp);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelTemp);
    };

    //
    //  init model pos & rot
    //
    m_model.SetPosition(&pInitParam->m_vPosition);

    //
    //  setup model strategy
    //
    SetModelStrategy(&m_model);
};


CGearGimmick::~CGearGimmick(void)
{
    CGameSound::FadeOutSE(SDCODE_SE(4244), CGameSound::FADESPEED_NORMAL);
    CGameSound::FadeOutSE(SDCODE_SE(4222), CGameSound::FADESPEED_NORMAL);

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CGearGimmick::Draw(void) const
{
    if (!m_aSubInfo[m_subid].bOwnerDraw)
    {
        CGimmick::Draw();
        return;
    };

    CRenderStateManager::SetForDrawBeginning();

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, nullptr);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSU, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSV, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

    m_model.Draw();

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSV);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSU);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
};


void CGearGimmick::PreMove(void)
{
    float fRotVel = m_aSubInfo[m_subid].fRotVelocity[m_eState == STATE_SLOW ? 1 : 0];

    if (m_fRotVelocity > fRotVel)
    {
        m_fRotVelocity -= (CGameProperty::GetElapsedTime() * 3.0f);
        if (m_fRotVelocity < fRotVel)
            m_fRotVelocity = fRotVel;
    }
    else
    {
        m_fRotVelocity += (CGameProperty::GetElapsedTime() * 3.0f);
        if (m_fRotVelocity > fRotVel)
            m_fRotVelocity = fRotVel;
    };
};


void CGearGimmick::PostMove(void)
{
    float fDltRot = (CGameProperty::GetElapsedTime() * m_fRotVelocity);
    
    m_fRotation += fDltRot;
    m_fRotation = Math::RadianInvClamp(m_fRotation);

    RwV3d vRotation = { m_fRotation, 0.0f, 0.0f };
    m_model.SetRotation(&vRotation);

    if (m_eState >= STATE_FAST)
    {
		if (!m_hAtari &&  m_model.GetCollisionModelClump())
		{
			RpClump* pClump = m_model.GetCollisionModelClump();

			m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
			ASSERT(m_hAtari);

			if (m_hAtari)
				CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, m_aSubInfo[m_subid].fBSphereRadius);
		};
    };

    if (m_hAtari)
    {
        RwV3d vRotVelocity = { fDltRot, 0.0f, 0.0f };
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, nullptr, &vRotVelocity);
    };
};


void CGearGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    switch (m_eState)
    {
    case STATE_NONE:
        {
            m_eState = STATE_NORMAL;
            CGameSound::PlayObjectSE(this, SDCODE_SE(4244));
        }
        break;

    case STATE_NORMAL:
        {
            m_eState = STATE_FAST;
        }
        break;
        
    case STATE_FAST:
        {
            m_eState = STATE_SLOW;
            CGameSound::StopSE(SDCODE_SE(4244));
            CGameSound::PlayObjectSE(this, SDCODE_SE(4222));
        }
        break;

    default:
        break;
    };
};