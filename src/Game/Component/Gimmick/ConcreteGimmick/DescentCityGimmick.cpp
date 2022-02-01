#include "DescentCityGimmick.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"
#include "Game/System/Misc/UVAnim.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


static const float s_fTotalRotation = 43.9823f;


CDescentCityGimmick::CDescentCityGimmick(const char* pszName, void* pParam)
: CGimmick(pszName, pParam)
, m_state(STATE_IDLE)
, m_fElapsedTime(0.0f)
, m_fAddHeightAtFrame(0.0f)
, m_fAddRotateAtFrame(0.0f)
, m_vPosition(Math::VECTOR3_ZERO)
, m_vRotation(Math::VECTOR3_ZERO)
, m_fAccel(0.0f)
, m_fFirstSpeed(0.0f)
, m_fLastSpeed(0.0f)
, m_fSlowDownPoint(0.0f)
{
    for (int32 i = 0; i < 4; ++i)
    {
        ASSERT(i < COUNT_OF(m_apModel));
        ASSERT(i < COUNT_OF(m_ahUVAnim));
        
        m_apModel[i] = nullptr;
        m_ahUVAnim[i] = nullptr;
    };

    allClear();
    initParam();
    initModel();
};


CDescentCityGimmick::~CDescentCityGimmick(void)
{
    for (int32 i = 0; i < 4; ++i)
    {
        ASSERT(i < COUNT_OF(m_apModel));
        ASSERT(i < COUNT_OF(m_ahUVAnim));

        if (m_apModel[i])
        {
            delete m_apModel[i];
            m_apModel[i] = nullptr;
        };

        if (m_ahUVAnim[i])
        {
            CUVAnim::DestroyAnimatedMaterialsList(m_ahUVAnim[i]);
            m_ahUVAnim[i] = nullptr;
        };
    };
};


void CDescentCityGimmick::GetCenterPosition(RwV3d* pvPosition) const
{
#ifdef _DEBUG
    *pvPosition = m_vPosition;
#endif    
};


void CDescentCityGimmick::Draw(void) const
{
    CRenderStateManager::SetForDrawBeginning();

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSU, rwTEXTUREADDRESSCLAMP);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTUREADDRESSV, rwTEXTUREADDRESSCLAMP);

    for (int32 i = 0; i < COUNT_OF(m_apModel); ++i)
    {
        ASSERT(m_apModel[i]);
        
        m_apModel[i]->Draw();
    };

    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSV);
    RENDERSTATE_POP(rwRENDERSTATETEXTUREADDRESSU);
    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);

	CGimmick::Draw();
};


void CDescentCityGimmick::PostMove(void)
{
    uvAnimUpdate();
    cityMoveControl();
    timerCount();
};


void CDescentCityGimmick::allClear(void)
{
    ;
};


void CDescentCityGimmick::initParam(void)
{
    m_fAddRotateAtFrame = (s_fTotalRotation * 0.8f) / (CScreen::Framerate() * 96.0f);
    m_fAddHeightAtFrame = -60.0f / (CScreen::Framerate() * 120.0f);
    m_fFirstSpeed = m_fAddRotateAtFrame;
    m_fAccel = (-m_fFirstSpeed / 24.0f);
    m_state = STATE_RUN;
    m_vPosition = Math::VECTOR3_ZERO;
    m_vRotation = Math::VECTOR3_ZERO;
};


void CDescentCityGimmick::initModel(void)
{
    const char* apszModel[] =
    {
        "world",
        "cloud",
        "lightning",
        "rain",
    };

    static_assert(COUNT_OF(m_apModel) == COUNT_OF(apszModel), "update me");

    for (int32 i = 0; i < COUNT_OF(apszModel); ++i)
    {
        CNormalGimmickModel* pGimmickModel = new CNormalGimmickModel;
        ASSERT(pGimmickModel);

        CModel* pModel = CModelManager::CreateModel(apszModel[i]);
        ASSERT(pModel);

        pGimmickModel->SetModel(CNormalGimmickModel::MODELKIND_VISUAL_NORMAL, pModel);
        pGimmickModel->SetVisualNormal();
        pGimmickModel->SetPosition(&m_vPosition);
        pGimmickModel->SetRotation(&m_vRotation);

        if (CUVAnim::IsExistsUVAnimDict(apszModel[i]))
        {
            m_ahUVAnim[i] = CUVAnim::CreateAnimatedMaterialsList(pModel->GetClump());
            ASSERT(m_ahUVAnim[i]);
        };

        m_apModel[i] = pGimmickModel;
    };
};


void CDescentCityGimmick::uvAnimUpdate(void)
{
    for (int32 i = 0; i < COUNT_OF(m_ahUVAnim); ++i)
    {
        if (m_ahUVAnim[i])
            CUVAnim::UpdateUVAnim(m_ahUVAnim[i], CGameProperty::GetElapsedTime());
    };
};


void CDescentCityGimmick::cityMoveControl(void)
{
    cityRise();
    cityRotation();
    
    for (int32 i = 0; i < COUNT_OF(m_apModel); ++i)
    {
        CNormalGimmickModel* pGimmickModel = m_apModel[i];
        ASSERT(pGimmickModel);

        pGimmickModel->SetRotation(&m_vRotation);
        pGimmickModel->SetPosition(&m_vPosition);
    };
};


void CDescentCityGimmick::cityRise(void)
{
    if (m_state == STATE_RUN)
        m_vPosition.y += m_fAddHeightAtFrame;
};


void CDescentCityGimmick::cityRotation(void)
{
    m_vRotation.y += m_fAddRotateAtFrame;
    m_vRotation.y = Math::RadianCorrect(m_vRotation.y);
};


void CDescentCityGimmick::timerCount(void)
{
    if (m_state == STATE_RUN)
    {
        m_fElapsedTime += CGameProperty::GetElapsedTime();
        if (m_fElapsedTime >= 120.0f)
            m_state = STATE_END;
    };
};