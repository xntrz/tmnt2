#include "main.hpp"
#include "Framework.hpp"
#include "Configure.hpp"


int32 CommonMain(int32 argc, char** argv)
{
    int32 iResult = 0;	

    CConfigure::m_launchmode = CConfigure::LAUNCHMODE_NORMAL;

    CFramework* pFramework = CFramework::GetConcreteInstance();
    if (pFramework)
    {
        pFramework->Startup();
        if (pFramework->Initialize())
        {
            pFramework->Run();
            pFramework->Terminate();
        }
        else
        {
            pFramework->Terminate();
            iResult = -1;
        };
        pFramework->Cleanup();

        delete pFramework;
        pFramework = nullptr;
    };

    return iResult;
};