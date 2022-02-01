#include "MainSequence.hpp"

#include "Game/System/Misc/SoftwareReset.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/ProcessList.hpp"


/*static*/ CProcess* CMainSequence::Instance(void)
{
    return new CMainSequence;
};


CMainSequence::CMainSequence(void)
{
    ;
};


CMainSequence::~CMainSequence(void)
{
    ;
};


bool CMainSequence::OnAttach(const void* param)
{
#ifdef _DEBUG
    CSoftwareReset::Initialize();
#endif        
    
    return true;
};


void CMainSequence::OnDetach(void)
{
#ifdef _DEBUG
    CSoftwareReset::Terminate();
#endif
};


void CMainSequence::OnMove(bool bResume, const void* param)
{
    if (!bResume)
    {
        Call(PROCESSTYPES::LABEL_SEQ_GAMEMAIN);
    }
    else
    {
#ifdef _DEBUG        
        if (CSoftwareReset::IsRequested())
        {
            CSoftwareReset::Complete();
            CController::UnlockAllControllers();
            Call(PROCESSTYPES::LABEL_SEQ_GAMEMAIN);
        }
        else
        {
            Ret();
        };
#else
        Ret();
#endif        
    };
};


void CMainSequence::OnDraw(void) const
{
    ;
};