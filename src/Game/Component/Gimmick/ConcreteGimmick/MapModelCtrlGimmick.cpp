#include "MapModelCtrlGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Map/MapClumpModelMan.hpp"
#include "Game/System/Map/MapCamera.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "System/Common/RenderState.hpp"


CMapModelCtrlGimmick::CMapModelCtrlGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
{
    CModel* pModel = CModelManager::CreateModel("m53f_pillar");
    ASSERT(pModel);

    m_model.SetModel(CNormalGimmickModel::MODELKIND_ATARI_NORMAL, pModel);

    RwV3d vRot = Math::VECTOR3_ZERO;
    RwV3d vPos = Math::VECTOR3_ZERO;
    m_model.SetPosition(&vPos);
    m_model.SetRotation(&vRot);

    SetModelStrategy(&m_model);
};


CMapModelCtrlGimmick::~CMapModelCtrlGimmick(void)
{
    ;
};


void CMapModelCtrlGimmick::Draw(void) const
{
    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    if (pMapCamera->GetCameraMode() != CMapCamera::MODE_INTRODUCTION)
    {
        CRenderStateManager::SetForDrawBeginning();

        RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLFRONT);
        m_model.Draw();
        RENDERSTATE_POP(rwRENDERSTATECULLMODE);

        RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLBACK);
        m_model.Draw();
        RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    };
};


void CMapModelCtrlGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    CMapCamera* pMapCamera = CGameProperty::GetMapCamera();
    if (pMapCamera->GetCameraMode() == CMapCamera::MODE_INTRODUCTION)
        CMapClumpModelManager::SetDrawEnable("hasira01", true);
    else
        CMapClumpModelManager::SetDrawEnable("hasira01", false);
};