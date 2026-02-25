#include "LaserGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
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
, m_type(TYPE_RED)
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
            if (i >= TYPE_TRANSPARENT)
                pModel->UpdateFrame();
            else
                pModel->Draw();
        };
    };

    if (m_type == TYPE_TRANSPARENT)
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
        { { -0.5,    0.5,   -0.5,   },  0.0, 0.0, },
        { { -0.5,   -0.5,   -0.5,   },  0.0, 1.0, },
        { { -0.5,    0.5,    0.5,   },  1.0, 0.0, },
        { { -0.5,   -0.5,    0.5,   },  1.0, 1.0, },
        { {  0.5,    0.5,    0.5,   },  0.0, 0.0, },
        { {  0.5,   -0.5,    0.5,   },  0.0, 1.0, },
        { {  0.5,    0.5,   -0.5,   },  1.0, 0.0, },
        { {  0.5,   -0.5,   -0.5,   },  1.0, 1.0, },
        { { -0.5,    0.5,   -0.5,   },  0.0, 0.0, },
        { { -0.5,   -0.5,   -0.5,   },  0.0, 1.0, },
    };

    RwIm3DVertex aVertex[10] = { 0 };

    RwV3d vScale = { 0.05f, m_fNorm, 0.05f };
    RwV3d vRotation = Math::VECTOR3_ZERO;
    RwV3d vDirXZ = { m_vDirection.x, 0.0f, m_vDirection.z };

    if (Math::Vec3_Length(&vDirXZ) >= 0.001f)
    {
        Math::Vec3_Normalize(&vDirXZ, &vDirXZ);
        
        if (vDirXZ.x <= 0.0f)
            vRotation.y = -Math::ACos(vDirXZ.z);
        else
            vRotation.y = Math::ACos(vDirXZ.z);

        float fD = Math::Vec3_Dot(&vDirXZ, &m_vDirection);        
        if (fD <= 0.0f)
            vRotation.x = -(MATH_PI05 - Math::ACos(fD));
        else
            vRotation.x = (MATH_PI05 - Math::ACos(fD));
    }
    else
    {
        vRotation.x = 0.0f;
        vRotation.y = 0.0f;
    };

    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    Math::Matrix_AffineTransformation(&matrix, &vScale, &vRotation, &m_vPosition);

    for (int32 i = 0; i < COUNT_OF(aVertex); ++i)
    {
        RwIm3DVertex* pVertex = &aVertex[i];

        RwIm3DVertexSetPos(pVertex, aPosList[i].pos.x, aPosList[i].pos.y, aPosList[i].pos.z);
        RwIm3DVertexSetNormal(pVertex, 0.0f, 0.0f, 0.0f);
        RwIm3DVertexSetU(pVertex, aPosList[i].u);
        RwIm3DVertexSetV(pVertex, aPosList[i].v);

        static RwRGBA s_aTypeColorTable[] =
        {
            /* TYPE_RED         */  { 0xFF, 0x14, 0x00, 0xFF },
            /* TYPE_PINK        */  { 0xFF, 0x00, 0xDE, 0xFF },
            /* TYPE_TRANSPARENT */  { 0xFF, 0xFF, 0xFF, 0x00 },
        };

        static_assert(COUNT_OF(s_aTypeColorTable) == CLaserGimmickModel::TYPENUM, "update table");

        RwRGBA color = s_aTypeColorTable[m_type];
        RwIm3DVertexSetRGBA(pVertex, color.red, color.green, color.blue, color.alpha);
    };

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, NULL);

    const uint32 flags = rwIM3D_VERTEXRGBA
                       | rwIM3D_VERTEXXYZ;

    if (RwIm3DTransform(aVertex, COUNT_OF(aVertex), &matrix, flags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        RwIm3DEnd();
    };

    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
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


void CLaserGimmickModel::SetType(TYPE type)
{
    m_type = type;
};


//
// *********************************************************************************
//


CLaserGimmick::CLaserGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_fNorm(0.0f)
, m_bSwitch(false)
, m_line({ 0 })
, m_type(0)
{
    GIMMICKPARAM::GIMMICK_AREA_BOX* pInitParam = static_cast<GIMMICKPARAM::GIMMICK_AREA_BOX*>(pParam);
    ASSERT(pInitParam);

    RwMatrix matrix;
    RwMatrixSetIdentity(&matrix);
    CGimmickUtils::QuaternionToRotationMatrix(&matrix, &pInitParam->m_quat);

    RwV3d vDirection = Math::VECTOR3_ZERO;
    Math::Vec3_Scale(&vDirection, &matrix.up, pInitParam->m_box.y);

    m_vPosition = pInitParam->m_vPosition;
    m_vPosition.x += (vDirection.x * 0.5f);
    m_vPosition.y += (vDirection.y * 0.5f);
    m_vPosition.z += (vDirection.z * 0.5f);

    m_fNorm = Math::Vec3_Length(&vDirection);
    Math::Vec3_Normalize(&m_vDirection, &vDirection);
    SetLasertForm(&pInitParam->m_vPosition, &vDirection);
        
    m_bSwitch = true;
    m_type = pInitParam->m_subid;
    
    m_model.SetDraw(true);
    m_model.SetType(static_cast<CLaserGimmickModel::TYPE>(m_type));
    SetModelStrategy(&m_model);
};


CLaserGimmick::~CLaserGimmick(void)
{
    ;
};


void CLaserGimmick::GetPosition(RwV3d* pvPosition) const
{
    *pvPosition = m_vPosition;
};


void CLaserGimmick::PostMove(void)
{
    if (m_bSwitch)
    {
        CHitAttackData hitAttack;
        hitAttack.SetObject(GetHandle());
        hitAttack.SetShape(CHitAttackData::SHAPE_LINE);
        hitAttack.SetLine(&m_line);
        hitAttack.SetObjectPos(&m_vPosition);
        hitAttack.SetPower(20);
        hitAttack.SetTarget(CHitAttackData::TARGET_PLAYER | CHitAttackData::TARGET_ENEMY);
        hitAttack.SetAntiguard(CHitAttackData::ANTIGUARD_INVALID);
        hitAttack.SetStatus(CHitAttackData::STATUS_FLYAWAY);
        hitAttack.SetFlyawayParameter(5.0f, 2.5f);

        CHitAttackManager::RegistAttack(&hitAttack);
    };
};


void CLaserGimmick::OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype)
{
    if (eventtype == GIMMICKTYPES::EVENTTYPE_ACTIVATE)
    {
        m_bSwitch = false;
        m_model.SetDraw(false);
        
        if (m_type != CLaserGimmickModel::TYPE_TRANSPARENT)
            CGameSound::PlayPositionSE(&m_vPosition, SDCODE_SE(4168));
    };
};


void CLaserGimmick::SetLasertForm(const RwV3d* pvPos, const RwV3d* pvDir)
{
    m_line.start = *pvPos;
    Math::Vec3_Add(&m_line.end, pvPos, pvDir);
    
    m_model.SetLaserForm(&m_vPosition, &m_vDirection, m_fNorm);
};