#include "GimmickMotion.hpp"

#include "Game/System/Motion/MotionController.hpp"
#include "Game/System/Model/Model.hpp"


CGimmickMotion::CGimmickMotion(CModel* pModel)
: m_pMotionController(nullptr)
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


void CGimmickMotion::Update(float dt)
{
    m_pMotionController->Update(dt);
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

