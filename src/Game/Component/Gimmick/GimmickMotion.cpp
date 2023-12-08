#include "GimmickMotion.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/Motion/MotionController.hpp"
#include "Game/System/Model/Model.hpp"


CGimmickMotion::CGimmickMotion(CModel* pModel)
: m_pMotionController(new CMotionController(pModel))
, m_fPlayRate(1.0f)
{
    ;
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


void CGimmickMotion::StartRepeat(float fPlayRate)
{
    m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_REPEAT);
    m_pMotionController->Play();
    m_fPlayRate = fPlayRate;
};


void CGimmickMotion::StartOne(float fPlayRate)
{
    m_pMotionController->SetPlaymode(CMotionController::PLAYMODE_ONESHOT);
    m_pMotionController->Play();
    m_fPlayRate = fPlayRate;
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


void CGimmickMotion::SetTime(float t)
{
    m_pMotionController->SetTime(t);
};


float CGimmickMotion::GetEndTime(void) const
{
    return m_pMotionController->GetCurrentMotionEndTime();
};

