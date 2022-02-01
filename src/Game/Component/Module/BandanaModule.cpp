#include "BandanaModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Particle/Cloth.hpp"
#include "System/Common/Camera.hpp"
#include "System/Common/Screen.hpp"


static RwRGBA s_aBandanaColor[] =
{
    { 0x75, 0x09, 0x01, 0xFF },
    { 0xCF, 0x4B, 0x0E, 0xFF },
    { 0x33, 0x1F, 0x44, 0xFF },
    { 0x06, 0x56, 0x86, 0xFF },
    { 0x00, 0x00, 0x00, 0xFF },
    { 0x33, 0x33, 0x33, 0xFF },
    { 0x0D, 0x20, 0x3D, 0xFF },
    { 0x3A, 0x38, 0x3E, 0xFF },
    { 0x47, 0x47, 0x47, 0xFF },
    { 0x59, 0x13, 0x18, 0xFF },
};


CBandanaModule::CBandanaModule(CCharacter* pCharacter, CModel* pModel, int32 nBoneID, const RwV3d* pvOffset, BANDANACOLOR color)
: IModule(MODULETYPE::BANDANA)
, m_pCharacter(pCharacter)
, m_pModel(pModel)
, m_pCloth(nullptr)
, m_nBoneID(nBoneID)
, m_vOffset(*pvOffset)
, m_vJointPosition(Math::VECTOR3_ZERO)
, m_bWindChrSync(true)
{
    ASSERT(m_pCharacter);
    ASSERT(m_pModel);
    
    m_pCloth = new CCloth;
    ASSERT(m_pCloth);

    m_pCloth->SetBaseParameter(2, 8, 0.06f, 0.7f);
    m_pCloth->SetColor(s_aBandanaColor[color]);
    m_pCloth->SetGravity(CGameProperty::GetGravity() * (1.0f / CScreen::Framerate()));
    m_pCloth->SetWeight(35.0f);
    m_pCloth->SetTension(226.7f);
    m_pCloth->SetShear(276.7f);
    m_pCloth->SetLength(100.0f);
    m_pCloth->SetDrag(0.01f);
    m_pCloth->Create();
};


CBandanaModule::~CBandanaModule(void)
{
    ASSERT(m_pCloth);

    if (m_pCloth)
    {
        delete m_pCloth;
        m_pCloth = nullptr;
    };
};


void CBandanaModule::Run(void)
{
    RwMatrix matrix;
    Math::Matrix_RotateY(&matrix, m_pCharacter->GetDirection());

    RwV3d vJointPos = m_vOffset;
    RwV3dTransformPoint(&vJointPos, &vJointPos, &matrix);

    RwV3d vBonePos = *m_pModel->GetBonePositionFromID(m_nBoneID);
    Math::Vec3_Add(&vJointPos, &vJointPos, &vBonePos);

    m_vJointPosition = vJointPos;
    m_pCloth->SetJointPosition(&m_vJointPosition);

	if (m_bWindChrSync)
	{
		float fDir = m_pCharacter->GetDirection();
		RwV3d vWind = Math::VECTOR3_ZERO;

		vWind.x = Math::Sin(fDir) * -1.0f * 3.0f;
		vWind.y = 0.0f;
		vWind.z = Math::Cos(fDir) * -1.0f * 3.0f;

		m_pCloth->SetWind(&vWind);
	};

    float dt = CGameProperty::GetElapsedTime();
    m_pCloth->Update(dt);
};


void CBandanaModule::Draw(void)
{
    if (!m_pModel->IsDrawEnable())
        return;

    RwCamera* pCameraCurrent = CCamera::CameraCurrent();
    ASSERT(pCameraCurrent);

    RwMatrix* pViewMat = RwCameraGetViewMatrixMacro(pCameraCurrent);
    ASSERT(pViewMat);
    
    RwV3d vPosition = Math::VECTOR3_ZERO;
    RwV3dTransformPoint(&vPosition, &m_vJointPosition, pViewMat);

    if (vPosition.z > 0.0f)
    {
        float fNear = RwCameraGetNearClipPlaneMacro(pCameraCurrent);
        float fFar = RwCameraGetFarClipPlaneMacro(pCameraCurrent);
        
        vPosition.x *= (1.0f / vPosition.z);
        vPosition.y *= (1.0f / vPosition.y);

        if (vPosition.z >= fNear    &&
            vPosition.z <= fFar     &&
            vPosition.x >= -0.2f    &&
            vPosition.x <= 1.2f     &&
            vPosition.y >= -0.2f    &&
            vPosition.y <= 1.2f)
        {
            m_pCloth->Draw();
        };
    };
};


void CBandanaModule::Reset(const RwV3d* pvPosition)
{
    ASSERT(m_pCloth);
    ASSERT(pvPosition);
    
    m_pCloth->ResetCloth(pvPosition);
};


void CBandanaModule::SetWind(const RwV3d* pvWind)
{
    ASSERT(m_pCloth);
    ASSERT(pvWind);
    
    m_bWindChrSync = false;
    m_pCloth->SetWind(pvWind);
};