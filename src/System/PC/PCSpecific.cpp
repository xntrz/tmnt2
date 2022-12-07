#include "PCSpecific.hpp"
#include "PCFramework.hpp"
#include "PCGraphicsDevice.hpp"


static bool RemoveEvaluationLogo(void)
{
	enum
	{
		STEP_WND = BIT(0),
		STEP_INIT = BIT(1),
		STEP_OPEN = BIT(2),
		STEP_START = BIT(3),
	};

	uint32 uStepFlag = 0;
	HWND hWnd = NULL;
	RwCamera* pCamera = nullptr;
	bool bResult = false;

	try
	{
		hWnd = CreateWindowA("STATIC", NULL, NULL, 0, 0, 64, 64, NULL, NULL, NULL, NULL);
		if (hWnd == NULL)
			throw std::exception("creating dummy window");

		FLAG_SET(uStepFlag, STEP_WND);

		if (!RwEngineInit(nullptr, 0, 0))
			throw std::exception("rw engine init");

		FLAG_SET(uStepFlag, STEP_INIT);

		RwEngineOpenParams OpenParams;
		OpenParams.displayID = hWnd;
		if (!RwEngineOpen(&OpenParams))
			throw std::exception("rw engine open");

		FLAG_SET(uStepFlag, STEP_OPEN);

		if (!RwEngineStart())
			throw std::exception("rw engine start");

		FLAG_SET(uStepFlag, STEP_START);

		pCamera = RwCameraCreate();
		if (pCamera)
		{
			RwCameraEndUpdateFunc pfnCameraEnd = pCamera->endUpdate;
			SYSTEM_INFO SystemInfo;
			DWORD dwPrevProtect = 0;

			GetSystemInfo(&SystemInfo);

			if (VirtualProtect(pfnCameraEnd, SystemInfo.dwPageSize, PAGE_EXECUTE_READWRITE, &dwPrevProtect))
			{
				const uint8 NOP = 0x90;

#ifdef _DEBUG
				*((uint8*)pfnCameraEnd + 0x74) = NOP;
				*((uint8*)pfnCameraEnd + 0x75) = NOP;
				*((uint8*)pfnCameraEnd + 0x76) = NOP;
				*((uint8*)pfnCameraEnd + 0x77) = NOP;
				*((uint8*)pfnCameraEnd + 0x78) = NOP;
#else /*NDEBUG*/
				*((uint8*)pfnCameraEnd + 0x6) = NOP;
				*((uint8*)pfnCameraEnd + 0x7) = NOP;
				*((uint8*)pfnCameraEnd + 0x8) = NOP;
				*((uint8*)pfnCameraEnd + 0x9) = NOP;
				*((uint8*)pfnCameraEnd + 0xA) = NOP;
#endif                

				VirtualProtect(pfnCameraEnd, SystemInfo.dwPageSize, dwPrevProtect, &dwPrevProtect);
			}
			else
			{
				throw std::exception("change mem page protection");
			};
			
			RwCameraDestroy(pCamera);
			pCamera = nullptr;
			
			bResult = true;
		}
		else
		{
			throw std::exception("create camera");
		};

		OUTPUT(" Remove Evaluation Logo success!\n");
	}
	catch (std::exception& e)
	{
		REF(e);
		OUTPUT(" Remove Evaluation Logo failed: %s\n", e.what());
	};

	if (pCamera)
		RwCameraDestroy(pCamera);

	if (FLAG_TEST(uStepFlag, STEP_START))
		RwEngineStop();

	if (FLAG_TEST(uStepFlag, STEP_OPEN))
		RwEngineClose();

	if (FLAG_TEST(uStepFlag, STEP_INIT))
		RwEngineTerm();

	if (FLAG_TEST(uStepFlag, STEP_WND))
		DestroyWindow(hWnd);

	return bResult;
};


/*static*/ HINSTANCE CPCSpecific::m_hInstance = NULL;
/*static*/ HWND CPCSpecific::m_hWnd = NULL;


/*static*/ void CPCSpecific::Initialize(void)
{
	if (!RemoveEvaluationLogo())
		OUTPUT("removing eval logo failed");
};


/*static*/ void CPCSpecific::Terminate(void)
{
	;
};


/*static*/ void CPCSpecific::SetFlipEnable(bool bEnable)
{
	CPCFramework::Instance().GraphicsDevice().SetFlipEnable(bEnable);
};


/*static*/ bool CPCSpecific::SetVideomode(int32 iWidth, int32 iHeight, int32 iDepth)
{
	return CPCFramework::Instance().GraphicsDevice().SetVideomode(iWidth, iHeight, iDepth);
};


/*static*/ bool CPCSpecific::SetVideomode(int32 iVideomodeNo)
{
	return CPCFramework::Instance().GraphicsDevice().SetVideomode(iVideomodeNo);
};


/*static*/ int32 CPCSpecific::GetVideomodeNum(void)
{
	return CPCFramework::Instance().GraphicsDevice().GetVideomodeNum();
};


/*static*/ int32 CPCSpecific::GetVideomodeCur(void)
{
	return CPCFramework::Instance().GraphicsDevice().GetVideomodeCur();
};


/*static*/ void CPCSpecific::GetVideomode(int32 iVideomodeNo, int32& w, int32& h, int32& d)
{
	CPCFramework::Instance().GraphicsDevice().GetVideomode(iVideomodeNo, w, h, d);
};


/*static*/ bool CPCSpecific::IsVideomodeWindow(int32 iVideomodeNo)
{
	return CPCFramework::Instance().GraphicsDevice().IsVideomodeWindow(iVideomodeNo);
};


/*static*/ void CPCSpecific::GetModulePath(char* pszModulePathBuff, int32 buffsize)
{
	GetModuleFileNameA(GetModuleHandle(NULL), pszModulePathBuff, buffsize);
};


/*static*/ void CPCSpecific::DisplayCursor(bool bDispFlag)
{
	if (bDispFlag)
	{
		while (ShowCursor(TRUE) < 0)
			;
	}
	else
	{
		while (ShowCursor(FALSE) >= 0)
			;
	};
};