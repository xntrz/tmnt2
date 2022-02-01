#include "LaserGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "System/Common/RenderState.hpp"


CLaserGimmickModel::CLaserGimmickModel(void)
: CGimmickModel(4)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_fNorm(0.0f)
, m_type(0)
, m_bDraw(false)
{
    ;
};


CLaserGimmickModel::~CLaserGimmickModel(void)
{
    ;
};


void CLaserGimmickModel::Draw(void) const
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if (pModel)
        {
            if (i >= 2)
                pModel->UpdateFrame();
            else
                pModel->Draw();
        };
    };

    if (m_type == 2)
        return;

    if (!m_bDraw)
        return;

    struct POS
    {
        RwV3d pos;
        float u;
        float v;
    };

    static const POS aPosList[10] =
    {
        { { -0.5,   0.5,    -0.5,   },  0.0, 0.0, },
        { { -0.5,   -0.5,   -0.5,   },  0.0, 1.0, },
        { { -0.5,   0.5,    0.5,    },  1.0, 0.0, },
        { { -0.5,   -0.5,   0.5,    },  1.0, 1.0, },
        { { 0.5,    0.5,    0.5,    },  0.0, 0.0, },
        { { 0.5,    -0.5,   0.5,    },  0.0, 1.0, },
        { { 0.5,    0.5,    -0.5,   },  1.0, 0.0, },
        { { 0.5,    -0.5,   -0.5,   },  1.0, 1.0, },
        { { -0.5,   0.5,    -0.5,   },  0.0, 0.0, },
        { { -0.5,   -0.5,   -0.5,   },  0.0, 1.0, },
    };

    RwIm3DVertex aVertex[10] = { 0 };

    RwV3d vScale = { 0.05f, m_fNorm, 0.05f };
    RwV3d vRotation = Math::VECTOR3_ZERO;
    RwV3d vDirXZ = { m_vDirection.x, 0.0f, m_vDirection.z };

    if (Math::Vec3_Length(&vDirXZ) >= 0.0f)
    {
        Math::Vec3_Normalize(&vDirXZ, &vDirXZ);
        
        if (vDirXZ.x <= 0.0f)
        {
            vRotation.y = -Math::ACos(vDirXZ.z);
        }
        else
        {
            vRotation.y = Math::ACos(vDirXZ.z);
        };

        float fD = Math::Vec3_Dot(&vDirXZ, &m_vDirection);
        
        if (fD <= 0.0f)
        {
            vRotation.x = -(Math::PI05 - Math::ACos(fD));
        }
        else
        {
            vRotation.x = (Math::PI05 - Math::ACos(fD));
        };
    }
    else
    {
        vRotation.x = 0.0f;
        vRotation.y = 0.0f;
    };

    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);

    Math::Matrix_AffineTransformation(&matrix, &vScale, &vRotation, &m_vPosition);

    for (int32 i = 0; i < COUNT_OF(aVertex); ++i)
    {
        RwIm3DVertex* pVertex = &aVertex[i];

        pVertex->objVertex = aPosList[ i ].pos;
        pVertex->u = aPosList[ i ].u;
        pVertex->v = aPosList[ i ].v;
        pVertex->objNormal = Math::VECTOR3_ZERO;

        switch (m_type)
        {
        case 0:            
            {
                RwRGBA color = { 0xFF, 0x14, 0x0, 0xFF };
                pVertex->color = COLOR_TO_INTEGER_RWRGBA(color);
            }
            break;
            
        case 1:
            {
                RwRGBA color = { 0xFF, 0x00, 0xDE, 0xFF };
                pVertex->color = COLOR_TO_INTEGER_RWRGBA(color);
            }
            break;

        case 2:
            {
                RwRGBA color = { 0xFF, 0xFF, 0xFF, 0x00 };
                pVertex->color = COLOR_TO_INTEGER_RWRGBA(color);
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    const uint32 uFlags = rwIM3D_VERTEXRGBA | rwIM3D_VERTEXXYZ;

    if (RwIm3DTransform(aVertex, COUNT_OF(aVertex), &matrix, uFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };
};


void CLaserGimmickModel::SetLaserForm(const RwV3d* pvPos, const RwV3d* pvDir, float fNorm)
{
    m_vPosition = *pvPos;
    m_vDirection = *pvDir;
    m_fNorm = fNorm;
};


void CLaserGimmickModel::SetDraw(bool bEnable)
{
    m_bDraw = bEnable;
};


void CLaserGimmickModel::SetType(int32 type)
{
    m_type = type;
};


CLaserGimmick::CLaserGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_fNorm(0.0f)
, m_bSwitch(false)
, m_line({ 0 })
, m_type(0)
{
    ASSERT(pParam);
    
    GIMMICKPARAM::GIMMICK_AREA_BOX* pInitParam = (GIMMICKPARAM::GIMMICK_AREA_BOX*)pParam;

    RwV3d vPosition = pInitParam->m_vPosition;
    RwV3d vDirection = Math::VECTOR3_ZERO;
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);

    Math::Vec3_Scale(&vDirection, &matrix.up, pInitParam->m_box.y);

    m_vPosition.x = vDirection.x * 0.5f + vPosition.x;
    m_vPosition.y = vDirection.y * 0.5f + vPosition.y;
    m_vPosition.z = vDirection.z * 0.5f + vPosition.z;
    m_fNorm = Math::Vec3_Length(&vDirection);
    Math::Vec3_Normalize(&m_vDirection, &vDirection);
    SetLasertForm(&vPosition, &vDirection);
        
    m_bSwitch = true;
    m_type = pInitParam->m_subid;
    
    m_model.SetDraw(true);
    m_model.SetType(m_type);
    SetModelStrategy(&m_model);
};


CLaserGimmick::~CLaserGimmick(void)
{
    ;
};


void CLaserGimmick::GetPosition(RwV3d* pvPosition) const
{
    ASSERT(pvPosition);
    *pvPosition = m_vPosition;
};


void CLaserGimmick::PostMove(void)
{
    if (m_bSwitch)
    {
        CHitAttackData Attack;
        Attack.SetObject(GetHandle());
        Attack.SetShape(CHitAttackData::SHAPE_LINE);
        Attack.SetLine(&m_line);
        Attack.SetObjectPos(&m_vPosition);
        Attack.SetPower(20);
        Attack.SetTarget(CHitAttackData::TARGET(CHitAttackData::TARGET_PLAYER | CHitAttackData::TARGET_ENEMY));
        Attack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        Attack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        Attack.SetFlyawayParameter(5.0f, 2.5f);

        CHitAttackManager::RegistAttack(&Attack);
    };
};


void CLaserGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        m_bSwitch = false;
        m_model.SetDraw(false);
        
        if (m_type != 2)
            CGameSound::PlayPositionSE(&m_vPosition, SDCODE_SE(4168), 0);
    };
};


void CLaserGimmick::SetLasertForm(const RwV3d* pvPos, const RwV3d* pvDir)
{
    m_line.start = *pvPos;
    Math::Vec3_Add(&m_line.end, pvPos, pvDir);
    
    m_model.SetLaserForm(&m_vPosition, &m_vDirection, m_fNorm);
};