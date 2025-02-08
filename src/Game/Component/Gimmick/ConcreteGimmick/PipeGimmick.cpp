#include "PipeGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/Component/Gimmick/Utils/GimmickUtils.hpp"
#include "Game/Component/Gimmick/GimmickParam.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/System/Hit/HitAttackData.hpp"
#include "Game/System/Hit/HitCatchData.hpp"
#include "Game/System/Hit/HitAttackManager.hpp"
#include "Game/Component/Effect/EffectManager.hpp"
#include "Game/Component/Effect/MagicManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/DebugShape.hpp"


CPipeGimmick::CPipeGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vDirection(Math::VECTOR3_ZERO)
, m_fTimer(0.0f)
, m_subid(0)
, m_state(STATE_NONE)
, m_line({ 0 })
, m_hMagic(0)
{
    init(pParam);
};


CPipeGimmick::~CPipeGimmick(void)
{
    if (m_hMagic)
    {
        CMagicManager::Finish(m_hMagic);
        m_hMagic = 0;
    };
};


void CPipeGimmick::GetPosition(RwV3d* pvPosition) const
{
    *pvPosition = m_vPosition;
};


GIMMICKTYPES::FEATURE CPipeGimmick::GetFeatures(void) const
{
    return GIMMICKTYPES::FEATURE_ABLE_SLEEP;
};


void CPipeGimmick::PostMove(void)
{
    switch (m_state)
    {
    case STATE_NONE:
        m_state = STATE_WAIT;
        break;
        
    case STATE_WAIT:
        waiting();
        break;
        
    case STATE_CUT:
        cutting();
        break;
        
    case STATE_SPRAY:
        spray();
        break;
        
    case STATE_END:
        m_state = STATE_NONE;
        break;
        
    default:
        ASSERT(false);
        break;
    };
};


void CPipeGimmick::OnCatchAttack(CHitAttackData* pAttack)
{
    ASSERT(pAttack);
    
    if (m_state == STATE_WAIT)
    {
        if (pAttack->IsFlagSlice())
        {
            m_state = STATE_CUT;
            CGimmickUtils::PlayHitEffect(pAttack);

            RwV3d vPosition = *pAttack->GetHitPos();
            setSprayEffect(&vPosition);
        };
    };
};


void CPipeGimmick::init(void* pParam)
{
    ASSERT(pParam);
    
    GIMMICKPARAM::GIMMICK_AREA_BOX* pAreaBoxParam = (GIMMICKPARAM::GIMMICK_AREA_BOX*)pParam;
    m_subid = pAreaBoxParam->m_subid;
    
    RwV3d box = pAreaBoxParam->m_box;

    struct SIDE
    {
        int32 no;
        float pt;
    } aSide[3] =
    {
        { 0, box.x },
        { 1, box.y },
        { 2, box.z },
    };

    float pt = 0.0f;
    int32 no = 0;
    for (int32 i = 0; i < COUNT_OF(aSide); i++)
    {
        if (pt < aSide[i].pt)
        {
            pt = aSide[i].pt;
            no = aSide[i].no;
        };
    };

    pt *= 0.5f;
    RwV3d start, end;
    start = end = Math::VECTOR3_ZERO;

    switch (no)
    {
    case 0:
        start = { 0.0f, 0.0f, -pt };
        end = { 0.0f, 0.0f, pt };
        break;

    case 1:
        start = { 0.0f, pt * 2.0f, 0.0f };
        end = { 0.0f, 0.0f,	0.0f };
        break;

    case 2:
        start = { -pt, 0.0f, 0.0f };
        end = { pt, 0.0f, 0.0f };
        break;
    };

    RwMatrix matrixRotation;
    RwMatrixSetIdentityMacro(&matrixRotation);
    CGimmickUtils::QuaternionToRotationMatrix(&matrixRotation, &pAreaBoxParam->m_quat);
    
    RwMatrix matrixTranslation;
    RwMatrixSetIdentityMacro(&matrixTranslation);
    RwMatrixTranslate(&matrixTranslation, &pAreaBoxParam->m_vPosition, rwCOMBINEREPLACE);
    
    RwMatrix matrix;
    RwMatrixSetIdentityMacro(&matrix);
    Math::Matrix_Multiply(&matrix, &matrixRotation, &matrixTranslation);
    
    RwV3dTransformPoint(&m_line.start, &start, &matrix);
    RwV3dTransformPoint(&m_line.end, &end, &matrix);
    Math::Vec3_Normalize(&m_vDirection, &matrixRotation.at);
    
    m_vPosition = pAreaBoxParam->m_vPosition;
};


void CPipeGimmick::waiting(void)
{
    RwV3d vTmp = Math::VECTOR3_ZERO;
    Math::Vec3_Sub(&vTmp, &m_line.end, &m_line.start);
    float fLineLength = Math::Vec3_Length(&vTmp);
    Math::Vec3_Normalize(&vTmp, &vTmp);

    int32 nCheckNum = int32(fLineLength) + 1;
    RwV3d vPosition = m_line.start;
    for (int32 i = 0; i < nCheckNum; ++i)
    {
        RwSphere sphere = { 0 };
        sphere.center = vPosition;
        sphere.radius = 0.5f;

        Math::Vec3_Add(&vPosition, &vPosition, &vTmp);

        CHitCatchData Catch;
        Catch.SetObject(GetHandle());
        Catch.SetObjectType(GetType());
        Catch.SetSphere(&sphere);
        Catch.SetCatchNo(i);

        CHitAttackManager::RegistCatch(&Catch);
    };
};


void CPipeGimmick::cutting(void)
{
    CGameSound::PlayObjectSE(this, SDCODE_SE(4176));
    m_state = STATE_SPRAY;
};


void CPipeGimmick::spray(void)
{
    m_fTimer += CGameProperty::GetElapsedTime();
    if (m_fTimer > 6.0f)
    {
        if (m_hMagic)
        {
            CMagicManager::Finish(m_hMagic);
            m_hMagic = 0;
        };

        m_fTimer = 0.0f;
        m_state = STATE_END;
    };
};


void CPipeGimmick::setSprayEffect(const RwV3d* pvPosition)
{
    ASSERT(pvPosition);
    
    MAGICID::VALUE idMagic = MAGICID::ID_UNKNOWN;
    
    switch (m_subid)
    {
    case SUBID_STEAM:
        idMagic = MAGICID::ID_PIPE_STEAM;
        break;

    case SUBID_FREEZEGAS:
        idMagic = MAGICID::ID_PIPE_FREEZEGAS;
        break;

    default:
        ASSERT(false);
        break;
    };

    CMagicManager::CParameter param;
    param.SetPositon(pvPosition);
    param.SetDirection(&m_vDirection);
    param.SetObject(this);
    
    m_hMagic = CMagicManager::Play(idMagic, &param);
    ASSERT(m_hMagic);
    if (m_hMagic)
        CMagicManager::SetScale(m_hMagic, 1.5f);
};