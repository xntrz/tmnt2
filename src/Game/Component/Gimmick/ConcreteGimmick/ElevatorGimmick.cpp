#include "ElevatorGimmick.hpp"

#include "Game/Component/Effect/MagicParameter.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickManager.hpp"
#include "Game/System/Map/MapCollisionModel.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/RenderState.hpp"


static const int32 ELEVATOR_SE = SDCODE_SE(0x1074);


CElevatorGimmick::CElevatorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_model()
, m_hAtari(0)
, m_apParts()
, m_id(ID_M02N_ELEVATOR)
, m_state(STATE_NONE)
, m_fTimer(0.0f)
, m_generator()
{
    GIMMICKPARAM::GIMMICK_ENEMY_PLACE* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_PLACE*>(pParam);

    if (pInitParam->m_id == GIMMICKID::ID_K_M02N)
    {
        m_id = ID_M02N_ELEVATOR;

        //
        //  init floor
        //
        RwV3d vPosition     = { 49.0f, -2.6f, 0.0f };
        RwV3d vGoalPosition = { 49.0f,  5.0f, 0.5f };
        RwV3d vVelocity     = { 0.0f,   1.6f, 0.0f };

        m_apParts[0] = new CElevatorPartsFloorGimmick;
        m_apParts[0]->CreateModel("elevator", true);
        m_apParts[0]->SetPosition(&vPosition);
        m_apParts[0]->SetGoalPosition(&vGoalPosition);
        m_apParts[0]->SetVelocity(&vVelocity);

        //
        //  init hatch1
        //
        vPosition       = { 49.0f, 4.8f, 0.0f  };
        vGoalPosition   = { 49.0f, 4.8f, 4.18f };
        vVelocity       = { 0.0f,  0.0f, 1.0f  };

        m_apParts[1] = new CElevatorPartsHatchGimmick;
        m_apParts[1]->CreateModel("hatch1", true);
        m_apParts[1]->SetPosition(&vPosition);
        m_apParts[1]->SetGoalPosition(&vGoalPosition);
        m_apParts[1]->SetVelocity(&vVelocity);

        //
        //  init hatch2
        //
        vPosition       = { 49.0f, 4.8f,  0.0f  };
        vGoalPosition   = { 49.0f, 4.8f, -4.18f };
        vVelocity       = { 0.0f,  0.0f, -1.0f  };
        
        m_apParts[2] = new CElevatorPartsHatchGimmick;
        m_apParts[2]->CreateModel("hatch2", true);
        m_apParts[2]->SetPosition(&vPosition);
        m_apParts[2]->SetGoalPosition(&vGoalPosition);
        m_apParts[2]->SetVelocity(&vVelocity);

        //
        //  init generator
        //
        vPosition = { 49.0f, -2.6f, 0.0f };
        
        m_generator.Init(vPosition, 0.0f, 5, 15.0f, false);

        struct
        {
            RwV3d vPos;
            ENEMYID::VALUE id;
        } static const s_aEnemyInfo[5] =
        {
            { {  1.5f, 0.0f, -1.5f }, ENEMYID::ID_FOOT_NINJA_STAFF },
            { {  1.5f, 0.0f,  1.5f }, ENEMYID::ID_FOOT_NINJA_STAFF },
            { {  0.0f, 0.0f,  0.0f }, ENEMYID::ID_FOOT_NINJA_SWORD },
            { { -1.5f, 0.0f, -1.5f }, ENEMYID::ID_FOOT_NINJA_STAFF },
            { { -1.5f, 0.0f,  1.5f }, ENEMYID::ID_FOOT_NINJA_STAFF },
        };

        for (int32 i = 0; i < COUNT_OF(s_aEnemyInfo); ++i)
            m_generator.SetGenerateInfo(i, s_aEnemyInfo[i].vPos, -90.0f, s_aEnemyInfo[i].id, 1);
    }
    else
    {
        m_id = ID_M05N_ELEVATOR;

        //
        //  init pillar
        //
        RwV3d vPosition     = { -11.871f, -7.421f, -6.8649998f };
        RwV3d vGoalPosition = { vPosition.x,  vPosition.y + 8.0f, vPosition.z };
        RwV3d vVelocity     = { 0.0f, 1.6f, 0.0f };

        m_apParts[0] = new CElevatorPartsFloorGimmick;
        m_apParts[0]->CreateModel("elevator", false);
        m_apParts[0]->SetPosition(&vPosition);
        m_apParts[0]->SetGoalPosition(&vGoalPosition);
        m_apParts[0]->SetVelocity(&vVelocity);

        //
        //  init pillar cage model
        //
        CModel* pModel = CModelManager::CreateModel("elevator_a");
        ASSERT(pModel);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModel);
        m_model.SetPosition(&vPosition);
        m_model.UpdateFrame();
    };
};


CElevatorGimmick::~CElevatorGimmick(void)
{
	CGameSound::FadeOutSE(ELEVATOR_SE, CGameSound::FADESPEED_NORMAL);

    for (int32 i = 0; i < COUNT_OF(m_apParts); ++i)
    {
        if (m_apParts[i])
        {
            delete m_apParts[i];
            m_apParts[i] = nullptr;
        };
    };

    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CElevatorGimmick::Draw(void) const
{
    CRenderStateManager::SetForDrawBeginning();

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSU, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSV, rwTEXTUREADDRESSCLAMP);

    for (int32 i = 0; i < COUNT_OF(m_apParts); ++i)
    {
        if (m_apParts[i])
            m_apParts[i]->Draw();
    };

    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSV);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSU);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
};


bool CElevatorGimmick::Query(CGimmickQuery* pQuery) const
{
    if (m_generator.HandleQuery(pQuery))
        return true;

    return CGimmick::Query(pQuery);
};


void CElevatorGimmick::PreMove(void)
{
    float dt = CGameProperty::GetElapsedTime();
    
    m_fTimer += dt;
    if (m_id == ID_M02N_ELEVATOR)
        m_generator.Run(dt);
};


void CElevatorGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_MOVE_START:
        {
            for (int32 i = 0; i < COUNT_OF(m_apParts); ++i)
            {
                if (m_apParts[i])
                    m_apParts[i]->MoveStart();
            };

            if (m_id == ID_M02N_ELEVATOR)
                m_generator.Start();

            m_fTimer = 0.0f;
            m_state = STATE_MOVE;
        }
        break;

    case STATE_MOVE:
        {
            if ((m_id == ID_M05N_ELEVATOR) && (m_fTimer > 2.0f))
                CGimmickManager::PostEvent("GMSCAMERA_A000", GetName(), GIMMICKTYPES::EVENTTYPE_ACTIVATE);

            for (int32 i = 0; i < COUNT_OF(m_apParts); ++i)
            {
                if (!m_apParts[i])
                    continue;

                if (!m_apParts[i]->IsMoveEnd())
                    break;

                if (i == 2)
                    m_state = STATE_MOVE_END;
            };
        }
        break;

    case STATE_MOVE_END:
        {
            if (m_id == ID_M02N_ELEVATOR)
            {
				CGameSound::FadeOutSE(ELEVATOR_SE, CGameSound::FADESPEED_NORMAL);

                for (int32 i = 1; i < COUNT_OF(m_apParts); ++i) // skip first part - floor
                {
                    if (m_apParts[i])
                    {
                        delete m_apParts[i];
                        m_apParts[i] = nullptr;
                    };
                };

                m_fTimer = 0.0f;
                m_state = STATE_ENEMY_START;
            }
            else
			{
				ASSERT(m_id == ID_M05N_ELEVATOR);
				m_state = STATE_END;
            };
        }
        break;

    case STATE_ENEMY_START:
        {
            ASSERT(m_id == ID_M02N_ELEVATOR);

			if (m_fTimer > 0.3)
			{
				m_generator.ActivateEnemy();
				m_state = STATE_END;
			};
        }
        break;

    default:
        break;
    };

    for (int32 i = 0; i < COUNT_OF(m_apParts); ++i)
    {
        if (m_apParts[i])
            m_apParts[i]->Run(CGameProperty::GetElapsedTime());
    };
};


void CElevatorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    switch (eventtype)
    {
    case GIMMICKTYPES::EVENTTYPE_ACTIVATE:
        {
            if (m_state != STATE_NONE)
                return;

            m_state = STATE_MOVE_START;
			CGameSound::PlayObjectSE(this, ELEVATOR_SE);

            if (m_id == ID_M05N_ELEVATOR)
            {
                if (m_model.GetCollisionModelClump())
                {
                    RpClump* pClump = m_model.GetCollisionModelClump();

                    m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, GetName(), MAPTYPES::GIMMICKTYPE_NORMAL);
                    ASSERT(m_hAtari);

                    if (m_hAtari)
                        CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 10.0f);
                };
            };
        }
        break;

    case GIMMICKTYPES::EVENTTYPE_INACTIVATE:
        {
            if (m_id == ID_M05N_ELEVATOR)
            {
                RwV3d vPosition = Math::VECTOR3_ZERO;
                m_apParts[0]->GetGoalPosition(&vPosition);
                m_apParts[0]->SetPosition(&vPosition);

                if (m_hAtari)
                {
                    CMapCollisionModel::RemoveCollisionModel(m_hAtari);
                    m_hAtari = 0;
                };

                CGameSound::FadeOutSE(ELEVATOR_SE, CGameSound::FADESPEED_NORMAL);
                m_state = STATE_END;
            };
        }
        break;

    default:
        m_generator.HandleEvent(pszSender, eventtype);
        break;
    };
};


//
// *********************************************************************************
//


CElevatorPartsGimmick::CElevatorPartsGimmick(void)
: m_model()
, m_hAtari(0)
, m_state(STATE_NONE)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vGoalPosition(Math::VECTOR3_ZERO)
, m_vVelocity(Math::VECTOR3_ZERO)
{
    ;
};


CElevatorPartsGimmick::~CElevatorPartsGimmick(void)
{
    if (m_hAtari)
    {
        CMapCollisionModel::RemoveCollisionModel(m_hAtari);
        m_hAtari = 0;
    };
};


void CElevatorPartsGimmick::Draw(void) const
{
    m_model.Draw();
};


void CElevatorPartsGimmick::RegistCollisionModel(RwV3d* pvDltPos)
{
    if (m_hAtari)
        CMapCollisionModel::UpdateCollisionModel(m_hAtari, pvDltPos);
};


void CElevatorPartsGimmick::CreateModel(const char* pszModelName, bool bAtari)
{
    //
    //  init disp model
    //
    CModel* pModel = CModelManager::CreateModel(pszModelName);
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELTYPE_DRAW_NORMAL, pModel);

    //
    //  init atari model if required
    //
    if (bAtari)
    {
        char szAtariName[32];
        szAtariName[0] = '\0';

        std::strcpy(szAtariName, pszModelName);
        std::strcat(szAtariName, "_a");

        pModel = CModelManager::CreateModel(szAtariName);
        ASSERT(pModel);

        m_model.SetModel(CNormalGimmickModel::MODELTYPE_ATARI_NORMAL, pModel);
    };

    //
    //  init map collision
    //
    if (m_model.GetCollisionModelClump())
    {
        RpClump* pClump = m_model.GetCollisionModelClump();

        m_hAtari = CMapCollisionModel::RegistCollisionModel(pClump, "ELEVFLOOR", MAPTYPES::GIMMICKTYPE_NORMAL);
        ASSERT(m_hAtari);

        if (m_hAtari)
            CMapCollisionModel::SetBoundingSphereRadius(m_hAtari, 5.0f);
    };
};


//
// *********************************************************************************
//


void CElevatorPartsFloorGimmick::Run(float dt)
{
    RwV3d vDltPos = Math::VECTOR3_ZERO;

    switch (m_state)
    {
    case STATE_MOVE_START:
        m_state = STATE_MOVE;
        break;

    case STATE_MOVE:
        Move(&vDltPos, dt);
        break;

    default:
        break;
    };

    RegistCollisionModel(&vDltPos);
};


void CElevatorPartsFloorGimmick::Move(RwV3d* pvDltPos, float dt)
{
    ASSERT(m_vVelocity.y != 0.0f);

    RwV3d vDltPos = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltPos, &m_vVelocity, dt);

    RwV3d vNewPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewPos, &m_vPosition, &vDltPos);

    if (((m_vVelocity.y < 0.0f) && (vNewPos.y <= m_vGoalPosition.y)) ||
        ((m_vVelocity.y > 0.0f) && (vNewPos.y >= m_vGoalPosition.y)))
    {
        vNewPos.y = m_vGoalPosition.y;
        m_state = STATE_MOVE_END;
    };

    Math::Vec3_Sub(pvDltPos, &vNewPos, &m_vPosition);

    m_vPosition = vNewPos;
    m_model.SetPosition(&m_vPosition);
};


//
// *********************************************************************************
//


void CElevatorPartsHatchGimmick::Run(float dt)
{
    RwV3d vDltPos = Math::VECTOR3_ZERO;

    switch (m_state)
    {
    case STATE_MOVE_START:
        m_state = STATE_MOVE;
        break;

    case STATE_MOVE:
        Move(&vDltPos, dt);
        break;

    default:
        break;
    };

    RegistCollisionModel(&vDltPos);
};


void CElevatorPartsHatchGimmick::Move(RwV3d* pvDltPos, float dt)
{
    ASSERT(m_vVelocity.z != 0.0f);

    RwV3d vDltPos = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDltPos, &m_vVelocity, dt);

    RwV3d vNewPos = Math::VECTOR3_ZERO;
    Math::Vec3_Add(&vNewPos, &m_vPosition, &vDltPos);

    if (((m_vVelocity.z < 0.0f) && (vNewPos.z <= m_vGoalPosition.z)) ||
        ((m_vVelocity.z > 0.0f) && (vNewPos.z >= m_vGoalPosition.z)))
    {
        if (m_hAtari)
        {
            CMapCollisionModel::RemoveCollisionModel(m_hAtari);
            m_hAtari = 0;
        };

        vNewPos.z = m_vGoalPosition.z;
        m_state = STATE_MOVE_END;
    };

    Math::Vec3_Sub(pvDltPos, &vNewPos, &m_vPosition);
    
    m_vPosition = vNewPos;
    m_model.SetPosition(&m_vPosition);
};