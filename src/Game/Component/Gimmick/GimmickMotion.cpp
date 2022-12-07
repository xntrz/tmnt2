#include "GimmickMotion.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Motion/MotionController.hpp"
#include "Game/System/Model/Model.hpp"


CGimmickMotion::CGimmickMotion(CModel* pModel)
: m_pMotionController(nullptr)
, m_fPlayRate(1.0f)
{
    ASSERT(pModel);
    
    m_pMotionController = new CMotionController(pModel);
    ASSERT(m_pMotionController);
};


CGimmickMotion::~CGimmickMotion(void)
{
    if (m_pMotionController)
    {
        delete m_pMotionController;
        m_pMotionController = nullptr;
    };
};


void CGimmickMotion::Update(void)
{
    float fUpdateTime = CGameProperty::GetElapsedTime() * m_fPlayRate;
    while (fUpdateTime < 0.0f)
        fUpdateTime += GetEndTime();
    ASSERT(fUpdateTime >= 0.0f);

    m_pMotionController->Update(fUpdateTime);
};


void CGimmickMotion::Start(float fEndTime)
{
    m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_ONESHOT);
    m_pMotionController->Play();
};


void CGimmickMotion::Stop(void)
{
    m_pMotionController->Pause();
};


void CGimmickMotion::AddMotion(const char* pszName)
{
    m_pMotionController->AddMotion(pszName);
};


void CGimmickMotion::SetMotion(const char* pszName, float fStartTime, float fEndTime, float fBlendTime, bool bForce)
{
    m_pMotionController->SetCurrentMotion(pszName, fStartTime, fEndTime, fBlendTime, bForce);
};


float CGimmickMotion::GetEndTime(void) const
{
    return m_pMotionController->GetCurrentMotionEndTime();
};

