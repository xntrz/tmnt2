#include "InkThicknessCtrlModule.hpp"

#include "Game/System/Character/Character.hpp"
#include "Game/System/Model/ModelToon.hpp"
#include "Game/Component/GameMain/GameProperty.hpp"

#include "System/Common/Screen.hpp"


CInkThicknessCtrlModule::CInkThicknessCtrlModule(CCharacter* pCharacter)
: IModule(MODULETYPE::INK_THICKNESS_CONTROL)
, m_pCharacter(pCharacter)
, m_fScaling(0.0f)
{
	ASSERT(m_pCharacter);
	ASSERT(m_pCharacter->GetModel()->GetType() == CModelManager::MODELTYPE_TOON);

	ASSERT(CScreen::Width() > 0);
	m_fScaling = (CScreen::Width() / TYPEDEF::DEFAULT_SCREEN_WIDTH);
};


void CInkThicknessCtrlModule::Run(void)
{
	RwMatrix matView;
	CGameProperty::GetCameraViewMatrix(&matView);

	RwV3d vPosition = Math::VECTOR3_ZERO;
	m_pCharacter->GetBodyPosition(&vPosition);

	RwV3dTransformPoint(&vPosition, &vPosition, &matView);	

	float fThickness = 0.0f;

	if (vPosition.z > 2.0f)
	{
		if (vPosition.z < 20.0f)
			fThickness = (1.5f - (vPosition.z - 2.0f) / 18.0f * 1.5f) * m_fScaling;
		else
			fThickness = 0.0f;
	}
	else
	{
		fThickness = 1.5f * m_fScaling;
	};

	CModelToon* pModel = static_cast<CModelToon*>(m_pCharacter->GetModel());
	pModel->SetThickness(fThickness);
};