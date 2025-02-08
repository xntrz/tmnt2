#include "PCSpecific.hpp"
#include "PCFramework.hpp"
#include "PCGraphicsDevice.hpp"
#include "PCPhysicalController.hpp"


struct KEYINFO
{
	bool Enable;
	const char* Name;
};


static const KEYINFO s_aKeyInfoTbl[] =
{
	{ false,	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"1" 			},
	{ true, 	"2" 			},
	{ true, 	"3" 			},
	{ true, 	"4" 			},
	{ true, 	"5" 			},
	{ true, 	"6" 			},
	{ true, 	"7" 			},
	{ true, 	"8" 			},
	{ true, 	"9" 			},
	{ true, 	"0" 			},
	{ true, 	"dash" 			},
	{ false, 	"NULL" 			},
	{ true, 	"back" 			},
	{ true, 	"tab" 			},
	{ true, 	"q" 			},
	{ true, 	"w" 			},
	{ true, 	"e" 			},
	{ true, 	"r" 			},
	{ true, 	"t" 			},
	{ true, 	"y" 			},
	{ true, 	"u" 			},
	{ true, 	"i" 			},
	{ true, 	"o" 			},
	{ true, 	"p" 			},
	{ true, 	"lbrace"		},
	{ true, 	"rbrace"		},
	{ false, 	"NULL" 			},
	{ true, 	"lctrl" 		},
	{ true, 	"a" 			},
	{ true, 	"s" 			},
	{ true, 	"d" 			},
	{ true, 	"f" 			},
	{ true, 	"g" 			},
	{ true, 	"h" 			},
	{ true, 	"j" 			},
	{ true, 	"k" 			},
	{ true, 	"l" 			},
	{ true, 	"colon" 		},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"lshift"		},
	{ true, 	"bkslash" 		},
	{ true, 	"z" 			},
	{ true, 	"x" 			},
	{ true, 	"c" 			},
	{ true, 	"v" 			},
	{ true, 	"b" 			},
	{ true, 	"n" 			},
	{ true, 	"m" 			},
	{ true, 	"comma" 		},
	{ true, 	"period" 		},
	{ true, 	"slash" 		},
	{ true, 	"rshift" 		},
	{ true, 	"numpad star" 	},
	{ true, 	"lalt" 			},
	{ true, 	"space" 		},
	{ false, 	"capslock" 		},
	{ true, 	"f1" 			},
	{ true, 	"f2" 			},
	{ true, 	"f3" 			},
	{ true, 	"f4" 			},
	{ true, 	"f5" 			},
	{ true, 	"f6" 			},
	{ true, 	"f7" 			},
	{ true, 	"f8" 			},
	{ true, 	"f9" 			},
	{ true, 	"f10" 			},
	{ true, 	"numlock" 		},
	{ true, 	"scroll" 		},
	{ true, 	"numpad 7" 		},
	{ true, 	"numpad 8" 		},
	{ true, 	"numpad 9" 		},
	{ true, 	"numpad minus" 	},
	{ true, 	"numpad 4" 		},
	{ true, 	"numpad 5" 		},
	{ true, 	"numpad 6" 		},
	{ true, 	"numpad plus" 	},
	{ true, 	"numpad 1" 		},
	{ true, 	"numpad 2" 		},
	{ true, 	"numpad 3" 		},
	{ true, 	"numpad 0" 		},
	{ true, 	"numpad del" 	},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"f11" 			},
	{ true, 	"f12" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"rctrl" 		},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"numpad slash" 	},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"ralt" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ true, 	"home" 			},
	{ true, 	"up" 			},
	{ true, 	"pgup" 			},
	{ false, 	"NULL" 			},
	{ true, 	"left" 			},
	{ false, 	"NULL" 			},
	{ true, 	"right" 		},
	{ false, 	"NULL" 			},
	{ true, 	"end" 			},
	{ true, 	"down" 			},
	{ true, 	"pgdown" 		},
	{ true, 	"ins" 			},
	{ true, 	"del" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"lwin" 			},
	{ false, 	"rwin" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
	{ false, 	"NULL" 			},
};


static_assert(COUNT_OF(s_aKeyInfoTbl) == 256, "check out");


/*static*/ HINSTANCE CPCSpecific::m_hInstance = NULL;
/*static*/ HWND CPCSpecific::m_hWnd = NULL;


/*static*/ int32 CPCSpecific::GetKeyboradPort(void)
{
	return CPCPhysicalController::GetPort();
};


/*static*/ void CPCSpecific::MapDigital(uint32 btn, int32 iDIKey)
{
	CPCPhysicalController::MapDigital(btn, iDIKey);
};


/*static*/ void CPCSpecific::MapDigitalFixed(uint32 btn, int32 iDIKey)
{
	CPCPhysicalController::MapDigitalFixed(btn, iDIKey);
};


/*static*/ void CPCSpecific::MapAnalog(CController::ANALOG analog, int32 iDIKeyX, int32 iDIKeyY)
{
	CPCPhysicalController::MapAnalog(analog, iDIKeyX, iDIKeyY);
};


/*static*/ int32 CPCSpecific::GetDownKey(void)
{
	int32 iDIKey = CPCPhysicalController::GetDownKey();

	return (IsKeyValid(iDIKey) ? iDIKey : -1);
};


/*static*/ bool CPCSpecific::IsKeyDown(int32 iDIKey)
{
	return CPCPhysicalController::IsKeyDown(iDIKey);
};


/*static*/ bool CPCSpecific::IsKeyTrigger(int32 iDIKey)
{
    return CPCPhysicalController::IsKeyTrigger(iDIKey);
};


/*static*/ bool CPCSpecific::IsKeyValid(int32 iDIKey)
{
	if (!CPCPhysicalController::IsKeyNotFixed(iDIKey))
		return false;

	if ((iDIKey < 0) || (iDIKey >= COUNT_OF(s_aKeyInfoTbl)))
		return false;

	return s_aKeyInfoTbl[iDIKey].Enable;
};


/*static*/ const char* CPCSpecific::GetKeyName(int32 iDIKey)
{
	return ((iDIKey >= 0) && (iDIKey < COUNT_OF(s_aKeyInfoTbl)) ? s_aKeyInfoTbl[iDIKey].Name : nullptr);
};


/*static*/ bool CPCSpecific::SetVideomode(int32 No)
{
	return CPCFramework::Instance().SetVideomode(No);
};


/*static*/ bool CPCSpecific::GetVideomode(int32 No, PC::VIDEOMODE& vm)
{
	return CPCFramework::Instance().GetVideomode(No, vm);
};


/*static*/ int32 CPCSpecific::GetVideomodeNum(void)
{
	return CPCFramework::Instance().GetVideomodeNum();
};


/*static*/ int32 CPCSpecific::GetVideomodeCur(void)
{
	return CPCFramework::Instance().GetVideomodeCur();
};


/*static*/ void CPCSpecific::FrameSkipEnable(bool bEnable)
{
	CPCFramework::Instance().SetSkipEnable(bEnable);
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


/*static*/ void CPCSpecific::PathCorrection(std::string& str)
{
	//
	//	Replace spaces and " in path
	// 	Converts / to \\ in path
	//

	for (auto it : " \"")
		str.erase(std::remove(str.begin(), str.end(), it), str.end());

	std::replace(str.begin(), str.end(), '/', '\\');
};


/*static*/ bool CPCSpecific::MakeWindowScreenshotToClipboard(void)
{
    bool bResult = false;

    RECT rcWnd;
    GetClientRect(m_hWnd, &rcWnd);

    HDC hDCScr = GetDC(NULL);
    if (hDCScr != NULL)
    {
        HDC hDC = CreateCompatibleDC(hDCScr);
        if (hDC != NULL)
        {
            HBITMAP hBitmap = CreateCompatibleBitmap(hDCScr,
                                                     rcWnd.right - rcWnd.left,
                                                     rcWnd.bottom - rcWnd.top);
            if (hBitmap != NULL)
            {
                SelectObject(hDC, hBitmap);
                PrintWindow(m_hWnd, hDC, PW_CLIENTONLY);

                OpenClipboard(NULL);
                EmptyClipboard();
                SetClipboardData(CF_BITMAP, hBitmap);
                CloseClipboard();

                bResult = true;

                DeleteObject(hBitmap);
            };

            DeleteDC(hDC);
        };

        ReleaseDC(NULL, hDCScr);
    };

    return bResult;
};