#include "rweval.hpp"
#include "PCTypedefs.hpp"


static void rwevalMemIo(void* addr, void* bytes, uint32 size, bool write)
{
    DWORD dwPrevProtect = 0;

    if (VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &dwPrevProtect))
    {
        std::memcpy(
            (write ? addr : bytes),
            (write ? bytes : addr),
            size
        );
        
        VirtualProtect(addr, size, dwPrevProtect, &dwPrevProtect);
    };
};


bool rwevalInitialize(void)
{
	uint32 ver = RwEngineGetVersion();
	if (ver != 0x37002)
		return false;

    enum
    {
        STEP_WND = BIT(0),
        STEP_INIT = BIT(1),
        STEP_OPEN = BIT(2),
        STEP_START = BIT(3),
    };

    bool bResult = false;
    uint32 StepFlag = 0;
	RwCamera* pCamera = nullptr;
    
    HWND hWnd = CreateWindowA("STATIC", NULL, NULL, 0, 0, 64, 64, NULL, NULL, NULL, NULL);
    if (hWnd == NULL)
        goto label_cleanup;

    FLAG_SET(StepFlag, STEP_WND);

    if (!RwEngineInit(nullptr, 0, 0))
        goto label_cleanup;

    FLAG_SET(StepFlag, STEP_INIT);

    RwEngineOpenParams OpenParams;
    OpenParams.displayID = hWnd;
    if (!RwEngineOpen(&OpenParams))
        goto label_cleanup;

    FLAG_SET(StepFlag, STEP_OPEN);

    if (!RwEngineStart())
        goto label_cleanup;

    FLAG_SET(StepFlag, STEP_START);

    pCamera = RwCameraCreate();
    if (pCamera)
    {
        RwCameraEndUpdateFunc pfnCameraEnd = pCamera->endUpdate;

        uint32 Offset = 0;
#ifdef _DEBUG
        Offset = 0x74;
#else
        Offset = 0x6;
#endif

        uint8 Bytes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 }; // NOP x5
        rwevalMemIo((uint8*)pfnCameraEnd + Offset, Bytes, COUNT_OF(Bytes), true);

        bResult = true;
    };

label_cleanup:
    if (pCamera)
        RwCameraDestroy(pCamera);

    if (FLAG_TEST(StepFlag, STEP_START))
        RwEngineStop();

    if (FLAG_TEST(StepFlag, STEP_OPEN))
        RwEngineClose();

    if (FLAG_TEST(StepFlag, STEP_INIT))
        RwEngineTerm();

    if (FLAG_TEST(StepFlag, STEP_WND))
        DestroyWindow(hWnd);

    return bResult;
};


void rwevalTerminate(void)
{
    ;
};