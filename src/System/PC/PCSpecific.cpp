#include "PCSpecific.hpp"
#include "PCFramework.hpp"
#include "PCGraphicsDevice.hpp"
#include "PCPhysicalController.hpp"

#include <cctype> // std::toupper
#include <algorithm> // std::replace


struct KEYINFO
{
    bool        bEnable;
    const char* pszKeyName;
#ifdef TMNT2_BUILD_EU    
    char        szKeyNameUC[64];
    wchar       wszKeyNameUC[64];
#endif /* TMNT2_BUILD_EU */
};


static KEYINFO s_aKeyInfoTbl[] =
{
    { false,	"NULL" 			}, //   0
    { false, 	"NULL" 			}, //   1
    { true, 	"1" 			}, //   2
    { true, 	"2" 			}, //   3
    { true, 	"3" 			}, //   4
    { true, 	"4" 			}, //   5
    { true, 	"5" 			}, //   6
    { true, 	"6" 			}, //   7
    { true, 	"7" 			}, //   8
    { true, 	"8" 			}, //   9
    { true, 	"9" 			}, //   10
    { true, 	"0" 			}, //   11
    { true, 	"dash" 			}, //   12
    { false, 	"NULL" 			}, //   13
    { true, 	"back" 			}, //   14
    { true, 	"tab" 			}, //   15
    { true, 	"q" 			}, //   16
    { true, 	"w" 			}, //   17
    { true, 	"e" 			}, //   18
    { true, 	"r" 			}, //   19
    { true, 	"t" 			}, //   20
    { true, 	"y" 			}, //   21
    { true, 	"u" 			}, //   22
    { true, 	"i" 			}, //   23
    { true, 	"o" 			}, //   24
    { true, 	"p" 			}, //   25
    { true, 	"lbrace"		}, //   26
    { true, 	"rbrace"		}, //   27
    { false, 	"NULL" 			}, //   28
    { true, 	"lctrl" 		}, //   29
    { true, 	"a" 			}, //   30
    { true, 	"s" 			}, //   31
    { true, 	"d" 			}, //   32
    { true, 	"f" 			}, //   33
    { true, 	"g" 			}, //   34
    { true, 	"h" 			}, //   35
    { true, 	"j" 			}, //   36
    { true, 	"k" 			}, //   37
    { true, 	"l" 			}, //   38
    { true, 	"colon" 		}, //   39
    { false, 	"NULL" 			}, //   40
    { false, 	"NULL" 			}, //   41
    { true, 	"lshift"		}, //   42
    { true, 	"bkslash" 		}, //   43
    { true, 	"z" 			}, //   44
    { true, 	"x" 			}, //   45
    { true, 	"c" 			}, //   46
    { true, 	"v" 			}, //   47
    { true, 	"b" 			}, //   48
    { true, 	"n" 			}, //   49
    { true, 	"m" 			}, //   50
    { true, 	"comma" 		}, //   51
    { true, 	"period" 		}, //   52
    { true, 	"slash" 		}, //   53
    { true, 	"rshift" 		}, //   54
    { true, 	"numpad star" 	}, //   55
    { true, 	"lalt" 			}, //   56
    { true, 	"space" 		}, //   57
    { false, 	"capslock" 		}, //   58
    { true, 	"f1" 			}, //   59
    { true, 	"f2" 			}, //   60
    { true, 	"f3" 			}, //   61
    { true, 	"f4" 			}, //   62
    { true, 	"f5" 			}, //   63
    { true, 	"f6" 			}, //   64
    { true, 	"f7" 			}, //   65
    { true, 	"f8" 			}, //   66
    { true, 	"f9" 			}, //   67
    { true, 	"f10" 			}, //   68
    { true, 	"numlock" 		}, //   69
    { true, 	"scroll" 		}, //   70
    { true, 	"numpad 7" 		}, //   71
    { true, 	"numpad 8" 		}, //   72
    { true, 	"numpad 9" 		}, //   73
    { true, 	"numpad minus" 	}, //   74
    { true, 	"numpad 4" 		}, //   75
    { true, 	"numpad 5" 		}, //   76
    { true, 	"numpad 6" 		}, //   77
    { true, 	"numpad plus" 	}, //   78
    { true, 	"numpad 1" 		}, //   79
    { true, 	"numpad 2" 		}, //   80
    { true, 	"numpad 3" 		}, //   81
    { true, 	"numpad 0" 		}, //   82
    { true, 	"numpad del" 	}, //   83
    { false, 	"NULL" 			}, //   84
    { false, 	"NULL" 			}, //   85
    { false, 	"NULL" 			}, //   86
    { true, 	"f11" 			}, //   87
    { true, 	"f12" 			}, //   88
    { false, 	"NULL" 			}, //   89
    { false, 	"NULL" 			}, //   90
    { false, 	"NULL" 			}, //   91
    { false, 	"NULL" 			}, //   92
    { false, 	"NULL" 			}, //   93
    { false, 	"NULL" 			}, //   94
    { false, 	"NULL" 			}, //   95
    { false, 	"NULL" 			}, //   96
    { false, 	"NULL" 			}, //   97
    { false, 	"NULL" 			}, //   98
    { false, 	"NULL" 			}, //   99
    { false, 	"NULL" 			}, //   100
    { false, 	"NULL" 			}, //   101
    { false, 	"NULL" 			}, //   102
    { false, 	"NULL" 			}, //   103
    { false, 	"NULL" 			}, //   104
    { false, 	"NULL" 			}, //   105
    { false, 	"NULL" 			}, //   106
    { false, 	"NULL" 			}, //   107
    { false, 	"NULL" 			}, //   108
    { false, 	"NULL" 			}, //   109
    { false, 	"NULL" 			}, //   110
    { false, 	"NULL" 			}, //   111
    { false, 	"NULL" 			}, //   112
    { false, 	"NULL" 			}, //   113
    { false, 	"NULL" 			}, //   114
    { false, 	"NULL" 			}, //   115
    { false, 	"NULL" 			}, //   116
    { false, 	"NULL" 			}, //   117
    { false, 	"NULL" 			}, //   118
    { false, 	"NULL" 			}, //   119
    { false, 	"NULL" 			}, //   120
    { false, 	"NULL" 			}, //   121
    { false, 	"NULL" 			}, //   122
    { false, 	"NULL" 			}, //   123
    { false, 	"NULL" 			}, //   124
    { false, 	"NULL" 			}, //   125
    { false, 	"NULL" 			}, //   126
    { false, 	"NULL" 			}, //   127
    { false, 	"NULL" 			}, //   128
    { false, 	"NULL" 			}, //   129
    { false, 	"NULL" 			}, //   130
    { false, 	"NULL" 			}, //   131
    { false, 	"NULL" 			}, //   132
    { false, 	"NULL" 			}, //   133
    { false, 	"NULL" 			}, //   134
    { false, 	"NULL" 			}, //   135
    { false, 	"NULL" 			}, //   136
    { false, 	"NULL" 			}, //   137
    { false, 	"NULL" 			}, //   138
    { false, 	"NULL" 			}, //   139
    { false, 	"NULL" 			}, //   140
    { false, 	"NULL" 			}, //   141
    { false, 	"NULL" 			}, //   142
    { false, 	"NULL" 			}, //   143
    { false, 	"NULL" 			}, //   144
    { false, 	"NULL" 			}, //   145
    { false, 	"NULL" 			}, //   146
    { false, 	"NULL" 			}, //   147
    { false, 	"NULL" 			}, //   148
    { false, 	"NULL" 			}, //   149
    { false, 	"NULL" 			}, //   150
    { false, 	"NULL" 			}, //   151
    { false, 	"NULL" 			}, //   152
    { false, 	"NULL" 			}, //   153
    { false, 	"NULL" 			}, //   154
    { false, 	"NULL" 			}, //   155
    { false, 	"NULL" 			}, //   156
    { true, 	"rctrl" 		}, //   157
    { false, 	"NULL" 			}, //   158
    { false, 	"NULL" 			}, //   159
    { false, 	"NULL" 			}, //   160
    { false, 	"NULL" 			}, //   161
    { false, 	"NULL" 			}, //   162
    { false, 	"NULL" 			}, //   163
    { false, 	"NULL" 			}, //   164
    { false, 	"NULL" 			}, //   165
    { false, 	"NULL" 			}, //   166
    { false, 	"NULL" 			}, //   167
    { false, 	"NULL" 			}, //   168
    { false, 	"NULL" 			}, //   169
    { false, 	"NULL" 			}, //   170
    { false, 	"NULL" 			}, //   171
    { false, 	"NULL" 			}, //   172
    { false, 	"NULL" 			}, //   173
    { false, 	"NULL" 			}, //   174
    { false, 	"NULL" 			}, //   175
    { false, 	"NULL" 			}, //   176
    { false, 	"NULL" 			}, //   177
    { false, 	"NULL" 			}, //   178
    { false, 	"NULL" 			}, //   179
    { false, 	"NULL" 			}, //   180
    { true, 	"numpad slash" 	}, //   181
    { false, 	"NULL" 			}, //   182
    { false, 	"NULL" 			}, //   183
    { true, 	"ralt" 			}, //   184
    { false, 	"NULL" 			}, //   185
    { false, 	"NULL" 			}, //   186
    { false, 	"NULL" 			}, //   187
    { false, 	"NULL" 			}, //   188
    { false, 	"NULL" 			}, //   189
    { false, 	"NULL" 			}, //   190
    { false, 	"NULL" 			}, //   191
    { false, 	"NULL" 			}, //   192
    { false, 	"NULL" 			}, //   193
    { false, 	"NULL" 			}, //   194
    { false, 	"NULL" 			}, //   195
    { false, 	"NULL" 			}, //   196
    { false, 	"NULL" 			}, //   197
    { false, 	"NULL" 			}, //   198
    { true, 	"home" 			}, //   199
    { true, 	"up" 			}, //   200
    { true, 	"pgup" 			}, //   201
    { false, 	"NULL" 			}, //   202
    { true, 	"left" 			}, //   203
    { false, 	"NULL" 			}, //   204
    { true, 	"right" 		}, //   205
    { false, 	"NULL" 			}, //   206
    { true, 	"end" 			}, //   207
    { true, 	"down" 			}, //   208
    { true, 	"pgdown" 		}, //   209
    { true, 	"ins" 			}, //   210
    { true, 	"del" 			}, //   211
    { false, 	"NULL" 			}, //   212
    { false, 	"NULL" 			}, //   213
    { false, 	"NULL" 			}, //   214
    { false, 	"NULL" 			}, //   215
    { false, 	"NULL" 			}, //   216
    { false, 	"NULL" 			}, //   217
    { false, 	"NULL" 			}, //   218
    { false, 	"lwin" 			}, //   219
    { false, 	"rwin" 			}, //   220
    { false, 	"NULL" 			}, //   221
    { false, 	"NULL" 			}, //   222
    { false, 	"NULL" 			}, //   223
    { false, 	"NULL" 			}, //   224
    { false, 	"NULL" 			}, //   225
    { false, 	"NULL" 			}, //   226
    { false, 	"NULL" 			}, //   227
    { false, 	"NULL" 			}, //   228
    { false, 	"NULL" 			}, //   229
    { false, 	"NULL" 			}, //   230
    { false, 	"NULL" 			}, //   231
    { false, 	"NULL" 			}, //   232
    { false, 	"NULL" 			}, //   233
    { false, 	"NULL" 			}, //   234
    { false, 	"NULL" 			}, //   235
    { false, 	"NULL" 			}, //   236
    { false, 	"NULL" 			}, //   237
    { false, 	"NULL" 			}, //   238
    { false, 	"NULL" 			}, //   239
    { false, 	"NULL" 			}, //   240
    { false, 	"NULL" 			}, //   241
    { false, 	"NULL" 			}, //   242
    { false, 	"NULL" 			}, //   243
    { false, 	"NULL" 			}, //   244
    { false, 	"NULL" 			}, //   245
    { false, 	"NULL" 			}, //   246
    { false, 	"NULL" 			}, //   247
    { false, 	"NULL" 			}, //   248
    { false, 	"NULL" 			}, //   249
    { false, 	"NULL" 			}, //   250
    { false, 	"NULL" 			}, //   251
    { false, 	"NULL" 			}, //   252
    { false, 	"NULL" 			}, //   253
    { false, 	"NULL" 			}, //   254
    { false, 	"NULL" 			}, //   255
};


static_assert(COUNT_OF(s_aKeyInfoTbl) == 256, "check out");


#ifdef TMNT2_BUILD_EU    
static bool KeyInfoInitializer(void)
{
    for (int32 i = 0; i < COUNT_OF(s_aKeyInfoTbl); ++i)
    {
        KEYINFO* pKeyInfo = &s_aKeyInfoTbl[i];

        const char* pszPtr = pKeyInfo->pszKeyName;
        size_t index = 0;
        while (*pszPtr)
        {
            pKeyInfo->szKeyNameUC[index++] = static_cast<char>(std::toupper(static_cast<int>(*pszPtr)));
            ++pszPtr;
        };

        std::mbstowcs(pKeyInfo->wszKeyNameUC, pKeyInfo->szKeyNameUC, COUNT_OF(pKeyInfo->wszKeyNameUC));
    };
    
    return true;
};
#endif /* TMNT2_BUILD_EU */


static const KEYINFO* PCKeyInfo(int32 iDIKey)
{
#ifdef TMNT2_BUILD_EU
    static bool s_bKeyInfoInitialize = KeyInfoInitializer();
#endif /* TMNT2_BUILD_EU */
    
    if ((iDIKey >= 0) && (iDIKey < COUNT_OF(s_aKeyInfoTbl)))
        return &s_aKeyInfoTbl[iDIKey];

    return nullptr;
};


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

    const KEYINFO* pKeyInfo = PCKeyInfo(iDIKey);
    if (pKeyInfo)
        return pKeyInfo->bEnable;

    return false;
};


/*static*/ const char* CPCSpecific::GetKeyName(int32 iDIKey)
{
    const KEYINFO* pKeyInfo = PCKeyInfo(iDIKey);
    if (pKeyInfo)
    {
        const char* pszKeyName = nullptr;
#ifdef TMNT2_BUILD_EU
        pszKeyName = pKeyInfo->szKeyNameUC;
#else /* TMNT2_BUILD_EU */
        pszKeyName = pKeyInfo->pszKeyName;
#endif /* TMNT2_BUILD_EU */
        return pszKeyName;
    };
    
    return nullptr;
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