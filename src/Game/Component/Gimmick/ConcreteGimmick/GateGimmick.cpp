#include "GateGimmick.hpp"

#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/Screen.hpp"


CGateGimmick::CGateGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_hAtari(0)
, m_fTimer(0.0f)
, m_bStart(false)
, m_bMoveEnd(false)
, m_state(STATE_WAIT)
, m_aEffect()
{
    GIMMICKPARAM::GIMMICK_BASIC* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_BASIC*>(pParam);

    //
    //  init disp model
    //
    CModel* pModelDispl = CModelManager::CreateModel("m23n_gate");
    ASSERT(pModelDispl);

    pModelDispl->SetLightingEnable(false);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModelDispl);

    //
    //  init atari model
    //
    CModel* pModelAtari = CModelManager::CreateModel("gate_atari");
    ASSERT(pModelAtari);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModelAtari);

    //
    //  init model pos & rot
    //
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
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 8.0f);
    };
    
    //
    //  setup model strategy
    //
    SetModelStrategy(&m_model);
    
    //
    //  init effect
    //
    struct INITEFFECT
    {
        EFFECTID::VALUE id;
        RwV3d           pos;
        bool            belongFlag;
        float           interval;
        float           scale;
    };

    static const INITEFFECT s_aInitEffect[] =
    {
        {   EFFECTID::ID_SPSM_KABECRASH,  { -7.3f,  2.8f, 4.5f }, true,   1.0f,   1.0f    },
        {   EFFECTID::ID_SPSM_KABECRASH,  {  2.3f,  2.8f, 4.5f }, true,   1.1f,   1.0f    },
        {   EFFECTID::ID_SPSM_KABECRASH,  { -2.0f,  2.8f, 4.5f }, true,   1.2f,   1.0f    },
        {   EFFECTID::ID_ALL_DOWNSMOKE,   { -7.3f,  0.0f, 4.5f }, true,   0.7f,   4.0f    },
        {   EFFECTID::ID_ALL_DOWNSMOKE,   {  2.3f,  0.0f, 4.5f }, true,   0.8f,   1.0f    },
        {   EFFECTID::ID_ALL_DOWNSMOKE,   { -7.0f, -2.4f, 4.5f }, false,  0.7f,   4.0f    },
        {   EFFECTID::ID_ALL_DOWNSMOKE,   {  2.0f, -2.4f, 4.5f }, false,  0.5f,   4.0f    },
    };

    static_assert(COUNT_OF(s_aInitEffect) == COUNT_OF(m_aEffect), "should equal");

    for (int32 i = 0; i < COUNT_OF(s_aInitEffect); ++i)
    {
        const INITEFFECT* pInitEffect = &s_aInitEffect[i];
        EFFECT* pEffect = &m_aEffect[i];

        pEffect->m_vEffectPos       = pInitEffect->pos;
        pEffect->m_fTimer           = Math::RandFloatRange(0.0, pInitEffect->interval);
        pEffect->m_fInterval        = pInitEffect->interval;
        pEffect->m_bBelongToGate    = pInitEffect->belongFlag;
        pEffect->m_pModel           = m_model.GetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL);
        pEffect->m_fScale           = pInitEffect->scale;
        pEffect->m_effectId         = pInitEffect->id;
    };
};


CGateGimmick::~CGateGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CGateGimmick::Run(void)
{
    cameraVibCtrl();
    gateMoveCtrl();
    gateEffectCtrl();
};


void CGateGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype != GIMMICKTYPES::EVENTTYPE_ACTIVATE)
        return;

    if (m_state == STATE_WAIT)
    {
        ASSERT(m_bStart != true);
        m_bStart = true;
    };
};


void CGateGimmick::cameraVibCtrl(void)
{
    switch (m_state)
    {
    case STATE_WAIT:
        {
            RwV3d vPosition = Math::VECTOR3_ZERO;
            m_model.GetPosition(&vPosition);

            float fNearestPlayerDist = CGimmickUtils::CalcNearestPlayerDistanceXZ(&vPosition);
            if (m_bStart && (fNearestPlayerDist < 15.0f))
            {
                cameraVibRq(0.1f, 1.5f, 10);
                m_fTimer = 0.0f;
                m_state = STATE_BEFORE_CAMERA_VIB;
                CGameSound::PlayObjectSE(this, SDCODE_SE(4260));
            };
        }
        break;

    case STATE_BEFORE_CAMERA_VIB:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > 1.5f)
            {
                cameraVibRq(0.3f, 5.5f, 10);
                m_fTimer = 0.0f;
                m_state = STATE_GO_DOWN;
            };
        }
        break;

    case STATE_GO_DOWN:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > 5.5f)
            {
                cameraVibRq(0.1f, 1.5f, 10);
                m_fTimer = 0.0f;
                m_state = STATE_AFTER_CAMERA_VIB;
            };
        }
        break;

    case STATE_AFTER_CAMERA_VIB:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer > 3.0f)
            {
                m_fTimer = 0.0f;
                m_state = STATE_END;
            };
        }
        break;

    case STATE_END:
        {
            if (m_bMoveEnd)
            {
                CGameSound::StopSE(SDCODE_SE(4260));
                Release();
            };
        }        
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CGateGimmick::gateMoveCtrl(void)
{
    if ((m_state == STATE_WAIT) || (m_state == STATE_BEFORE_CAMERA_VIB))
        return;

    if (m_bMoveEnd)
        return;

    RwV3d vPosition = Math::VECTOR3_ZERO;
    m_model.GetPosition(&vPosition);

    vPosition.y -= (7.21f / (CScreen::Framerate() * 7.0f));

    m_model.SetPosition(&vPosition);

    if (vPosition.y < -2.8f)
    {
        if (m_hAtari)
        {
            CMapCollisionModel::RemoveCollisionModel(m_hAtari);
            m_hAtari = 0;
        };

        m_bMoveEnd = true;
    };
};


void CGateGimmick::gateEffectCtrl(void)
{
    if (m_state == STATE_WAIT)
        return;

    if (m_bMoveEnd)
        return;
    
    for (int32 i = 0; i < COUNT_OF(m_aEffect); ++i)
    {
        EFFECT* pEffect = &m_aEffect[i];

        pEffect->m_fTimer += CGameProperty::GetElapsedTime();
        if (pEffect->m_fTimer >= pEffect->m_fInterval)
        {
			pEffect->m_fTimer = 0.0f;

            RwV3d vEffectPos = pEffect->m_vEffectPos;

            if (pEffect->m_bBelongToGate)
            {
                CModel* pModel = m_model.GetVisualModel();
                RpClump* pClump = pModel->GetClump();
                RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
                RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

                RwV3dTransformPoint(&vEffectPos, &pEffect->m_vEffectPos, pMatrix);
            };

            uint32 hEffect = CEffectManager::Play(pEffect->m_effectId, &vEffectPos);
            if (hEffect)
                CEffectManager::SetScale(hEffect, pEffect->m_fScale);
        };
    };
};


void CGateGimmick::cameraVibRq(float fPower, float fTime, int32 nFreq)
{
    CGameProperty::StopCameraVibration();
    CGameProperty::SetCameraVibration(fPower, fTime, nFreq);
};