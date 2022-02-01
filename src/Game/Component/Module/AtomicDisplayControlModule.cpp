#include "AtomicDisplayControlModule.hpp"

#include "Game/Component/GameMain/GameProperty.hpp"
#include "Game/System/MotionParameter/MotionParameterController.hpp"
#include "Game/System/Character/Character.hpp"
#include "Game/System/Model/Model.hpp"


CAtomicDisplayControlModule::CAtomicDisplayControlModule(CCharacter* pCharacter)
: IModule(MODULETYPE::ATOMIC_DISPLAY_CONTROL)
, m_pModel(pCharacter->GetModel())
, m_pCharacter(pCharacter)
{
    ASSERT(m_pCharacter);
    ASSERT(m_pModel);
};


CAtomicDisplayControlModule::~CAtomicDisplayControlModule(void)
{
    ;
};


void CAtomicDisplayControlModule::Run(void)
{
    float fNowTime = m_pCharacter->GetMotionTime();
    float fPrevTime = fNowTime - CGameProperty::GetElapsedTime();

    if (fPrevTime < 0.0f)
        fPrevTime = 0.0f;

    CMotionParameterController* pMPController = m_pCharacter->GetMotionParameterController();
    ASSERT(pMPController);

    int32 nNumDrawAtomic = pMPController->GetNumDrawAtomicData(fPrevTime, fNowTime);
    for (int32 i = 0; i < nNumDrawAtomic; ++i)
    {
        CMotionParameterController::DRAW_ATOMIC DrawAtomic = { 0 };
        pMPController->GetDrawAtomicData(i, &DrawAtomic);
        
        m_pModel->SetPartsDrawEnable(
            DrawAtomic.m_nAtomic,
            DrawAtomic.m_bDraw
        );
    };
};


void CAtomicDisplayControlModule::Draw(void)
{
    ;
};


void CAtomicDisplayControlModule::Reset(void)
{
    CMotionParameterController* pMPController = m_pCharacter->GetMotionParameterController();
    ASSERT(pMPController);

    int32 nNumParts = m_pModel->GetPartsNum();
    for (int32 i = 0; i < nNumParts; ++i)
    {        
        bool bDrawAtomic = pMPController->IsDrawAtomic(i);
        m_pModel->SetPartsDrawEnable(i, bDrawAtomic);
    };
};