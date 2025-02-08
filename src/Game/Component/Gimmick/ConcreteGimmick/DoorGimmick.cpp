#include "DoorGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/RenderState.hpp"


CDoorGimmick::CDoorGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_generator()
, m_status(STATUS_IDLE)
, m_fMoveVolume(0.0f)
, m_apModel()
, m_fTimer(0.0f)
, m_bEnd(false)
{
    GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE* pGeneratorParam = static_cast<GIMMICKPARAM::GIMMICK_ENEMY_APPEARANCE_MULTIPLE*>(pParam);    

    //
    //  init model
    //
    const char* apszModelName[] =
    {
        "door_l",
        "door_r",
    };

    static_assert(COUNT_OF(apszModelName) == COUNT_OF(m_apModel), "update me");

    int32 nDoorCnt = (pGeneratorParam->m_subid ? 1 : 2);
    for (int32 i = 0; i < nDoorCnt; ++i)
    {    
        CModel* pModel = CModelManager::CreateModel(apszModelName[i]);
        ASSERT(pModel);

        RwV3d vRotation = Math::VECTOR3_ZERO;
        vRotation.y = CGimmickUtils::QuaternionToRotationY(&pGeneratorParam->m_quat);

        pModel->SetPosition(&pGeneratorParam->m_vPosition);
        pModel->UpdateFrame();
        pModel->SetRotation(&vRotation);
        pModel->UpdateFrame();
        pModel->SetBoundingSphereRadius(4.0f);
        pModel->UpdateFrame();

        m_apModel[i] = pModel;
    };

    //
    //  init generator
    //
    float fGenRotY = 0.0f;
    RwV3d vGenPoint = Math::VECTOR3_ZERO;
    if (pGeneratorParam->m_subid)
        vGenPoint = { -1.0f, 0.0f, -1.0f };
    else
        vGenPoint = { 0.0f, 0.0f, -1.0f };
    
    m_generator.Init(pGeneratorParam, true);
    m_generator.SetGeneratePoint(&vGenPoint, fGenRotY);
};


CDoorGimmick::~CDoorGimmick(void)
{
    for (int32 i = 0; i < COUNT_OF(m_apModel); ++i)
    {
        if (m_apModel[i])
        {
            CModelManager::DestroyModel(m_apModel[i]);
            m_apModel[i] = nullptr;
        };
    };
};


void CDoorGimmick::Run(void)
{
    doorMoveOperate();
};


void CDoorGimmick::Draw(void) const
{
    CRenderStateManager::SetForDrawBeginning();

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSU, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSV, rwTEXTUREADDRESSCLAMP);

	for (int32 i = 0; i < COUNT_OF(m_apModel); ++i)
	{
		if (m_apModel[i])
			m_apModel[i]->Draw();
	};

    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSV);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSU);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
};


bool CDoorGimmick::Query(CGimmickQuery* pQuery) const
{
    if (m_generator.HandleQuery(pQuery))
        return true;

    return CGimmick::Query(pQuery);
};


void CDoorGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        if (m_status == STATUS_IDLE)
        {
            m_generator.Start();
            m_status = STATUS_CLOSED;
        };
    }
    else
    {
        m_generator.HandleEvent(pszSender, eventtype);
        m_bEnd = true;
    };
};


void CDoorGimmick::forceGenerateEnd(void)
{
    m_generator.Finish();
};


void CDoorGimmick::doorMoveOperate(void)
{
    float fMoveVolume = 0.0f;
    float fMovePerFrame = 2.0f * (CGameProperty::GetElapsedTime() / 0.5f);
    
    switch (m_status)
    {
    case STATUS_IDLE:
        {
            ;
        }
        break;

    case STATUS_OPENING:
        {
            fMoveVolume = -fMovePerFrame;            
            m_fMoveVolume -= fMovePerFrame;
            if (m_fMoveVolume <= -2.0f)
            {
                fMoveVolume = (-2.0f - m_fMoveVolume);
                m_fMoveVolume = -2.0f;
                m_status = STATUS_OPENED;
            };
        }
        break;

    case STATUS_OPENED:
        {
            m_fTimer += CGameProperty::GetElapsedTime();
            if (m_fTimer >= 0.5f)
            {
                m_fTimer = 0.0f;
                m_status = STATUS_CLOSING;
                CGameSound::PlayObjectSE(this, SDCODE_SE(4231));
            };
        }
        break;

    case STATUS_CLOSING:
        {
            fMoveVolume = fMovePerFrame;
            m_fMoveVolume += fMovePerFrame;
            if (m_fMoveVolume >= 0.0f)
            {
                fMoveVolume = -m_fMoveVolume;
                m_fMoveVolume = 0.0f;
                m_status = (m_bEnd ? STATUS_END : STATUS_CLOSED);
            };
        }
        break;

    case STATUS_CLOSED:
        {
            if (m_bEnd)
            {
                m_status = STATUS_END;
                m_generator.Finish();
            }
            else
            {
                switch (m_generator.Run(CGameProperty::GetElapsedTime()))
                {
                case CDoorEnemyGenerator::STATE_GENERATE_ENEMY:
                    {
                        m_status = STATUS_OPENING;
                        m_generator.ShowEnemy();
                        CGameSound::PlayObjectSE(this, SDCODE_SE(4231));
                    }
                    break;

                case CDoorEnemyGenerator::STATE_END:
                    {
                        m_bEnd = true;
                        m_status = STATUS_END;
                        m_generator.Finish();
                    }
                    break;
                };
            };
        }
        break;

    default:
        break;
    };

    if (Math::FEqual(fMoveVolume, 0.0f))
        return;

    for (int32 i = 0; i < COUNT_OF(m_apModel); ++i)
    {
		if (!m_apModel[i])
			continue;

        CModel* pMdl = m_apModel[i];
        RpClump* pClump = pMdl->GetClump();
        RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
        RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);

        if (i == 1)
            fMoveVolume *= -1.0f;

        RwV3d vPos = Math::VECTOR3_ZERO;
        vPos.x = fMoveVolume;        

        RwV3dTransformPoint(&vPos, &vPos, pMatrix);

        pMdl->SetPosition(&vPos);
        pMdl->UpdateFrame();
    };
};