#include "DebugUtils.hpp"

#ifdef _DEBUG

#include "Game/System/Misc/Gamepad.hpp"
#include "System/Common/Process/Sequence.hpp"


CDebugFontCtrl::CDebugFontCtrl(void)
: m_pCharset(nullptr)
, m_ColorBg({0xFF, 0xFF,0xFF,0xFF})
, m_ColorText({ 0x00, 0x00, 0x00, 0x00 })
, m_stepX(0)
, m_stepY(0)
, m_x(0)
, m_y(0)
, m_orgX(0)
, m_orgY(0)
{
    m_pCharset = RtCharsetCreate(&m_ColorText, &m_ColorBg);
    ASSERT(m_pCharset);
};


CDebugFontCtrl::~CDebugFontCtrl(void)
{
    if (m_pCharset)
    {
        RtCharsetDestroy(m_pCharset);
        m_pCharset = nullptr;
    };
};


void CDebugFontCtrl::Background(const RwRGBA& color)
{
    m_ColorBg = color;
    RtCharsetSetColors(m_pCharset, &m_ColorText, &m_ColorBg);
};


void CDebugFontCtrl::Color(const RwRGBA& color)
{
    m_ColorText = color;
    RtCharsetSetColors(m_pCharset, &m_ColorText, &m_ColorBg);
};


void CDebugFontCtrl::Position(int32 x, int32 y)
{
    m_orgX = x;
    m_orgY = y;

    Reset();
};


void CDebugFontCtrl::PositionAdd(int32 x, int32 y)
{
    m_x += x;
    m_y += y;
};


void CDebugFontCtrl::Print(const char* format, ...)
{
    char Buffer[4096];
    Buffer[0] = '\0';
    
    va_list vl;
    va_start(vl, format);
    std::vsprintf(Buffer, format, vl);
    va_end(vl);

    if (RtCharsetPrint(m_pCharset, Buffer, m_x, m_y))
    {
        m_x += (m_stepX);
        m_y += (m_stepY);
    };
};


void CDebugFontCtrl::SetAutoStep(int32 x, int32 y)
{
    m_stepX = x;
    m_stepY = y;
};


void CDebugFontCtrl::Reset(void)
{
    m_x = m_orgX;
    m_y = m_orgY;
};


//
// *********************************************************************************
//


CDebugMenuCtrl::CDebugMenuCtrl(void)
: m_aItem()
, m_listItemAlloc()
, m_listItemFree()
, m_font()
, m_ColorUnselect()
, m_ColorSelect()
, m_iSelect(0)
, m_iSelectMax(0)
, m_iDispMin(0)
, m_iDispMax(0)
, m_iPosX(0)
, m_iPosY(0)
, m_iHeight(0)
, m_Result(RESULT_NONE)
, m_uOptFlag(0)
, m_uDigitalOK(CController::DIGITAL_START | CController::DIGITAL_RDOWN)         // START or A
, m_uDigitalCANCEL(CController::DIGITAL_SELECT | CController::DIGITAL_RRIGHT)   // SELECT or B
{
	for (int32 i = 0; i < COUNT_OF(m_aItem); ++i)
        m_listItemFree.push_back(&m_aItem[i]);

    SetHeight(15);
    SetDispMax(10);
    SetPos(100, 100);
    SetColorSelect({ 0xFF, 0x30, 0x00, 0xFF });
    SetColorUnselect({ 0xFF, 0xFF, 0xFF, 0xFF });
    SetColorBackground({ 0x00, 0x00, 0x00, 0xFF });
};


CDebugMenuCtrl::~CDebugMenuCtrl(void)
{
    while (!m_listItemAlloc.empty())
    {
        ITEM* pItem = m_listItemAlloc.front();
        itemFree(pItem);
    };
};


void CDebugMenuCtrl::Period(void)
{
    m_Result = RESULT_NONE;
    
    if (m_listItemAlloc.empty())
        return;

    if (m_iDispMax > m_iSelectMax)
        m_iDispMax = m_iSelectMax;

    checkInput();
};


void CDebugMenuCtrl::Draw(void) const
{
    if (m_listItemAlloc.empty())
        return;

    int32 x = m_iPosX + 10;
    int32 y = m_iPosY;
    int32 h = m_iHeight;
    
    const int32 pad = 24;
    const char cursor = '>';

    //
    //  draw cursor
    //
    m_font.Color(m_ColorSelect);
    m_font.Position(x - 10, y + ((m_iSelect - m_iDispMin) * h));
    m_font.Print("%c", cursor);

    //
    //  draw items
    //
    int32 i = 0;

    for (auto& it : m_listItemAlloc)
    {
        ITEM* pItem = &it;

        if ((i < m_iDispMin) || (i >= m_iDispMax))
        {
            ++i;
            continue;
        };

        m_font.Color(i == m_iSelect ? m_ColorSelect : m_ColorUnselect);
        m_font.Position(x, y + ((i - m_iDispMin) * h));

        switch (pItem->Type)
        {
        case ITEM::TYPE_INT:
            {
                if (pItem->Int.ValueStrList)
                {
                    m_font.Print("%-*s %s", pad, pItem->Name, pItem->Int.ValueStrList[pItem->Int.Value]);
                }
                else
                {
                    if(pItem->Hex)
                        m_font.Print("%-*s 0x%X", pad, pItem->Name, pItem->Int.Value);
                    else
                        m_font.Print("%-*s %d", pad, pItem->Name, pItem->Int.Value);
                };
            }
            break;

        case ITEM::TYPE_FLOAT:
            {
                m_font.Print("%-*s %.02f", pad, pItem->Name, pItem->Float.Value);
            }            
            break;

        case ITEM::TYPE_BOOLEAN:
            {
                m_font.Print("%-*s %s", pad, pItem->Name, pItem->Boolean.Value ? "true" : "false");
            }            
            break;

        case ITEM::TYPE_TRIGGER:
            {
                m_font.Print("%-*s", pad, pItem->Name);
            }            
            break;

        case ITEM::TYPE_SEPARATOR:
            {
                if (pItem->Separator.Empty)
                    break;

                char buff[pad + 8];
                int32 j = 0;

                for (j = 0; j < sizeof(buff) - 1; ++j)
                    buff[j] = '-';
                
                buff[j] = '\0';

                m_font.Print("%s", buff);
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        ++i;
    };
};


void CDebugMenuCtrl::AddInt(const char* pszName, int32 nValueMin, int32 nValueMax, int32 nValueStep, int32 nValueInit, void(*pfnCallback)(int32, bool))
{
    AddInt(pszName, nValueMin, nValueMax, nValueStep, nValueInit, nullptr, pfnCallback);
};


void CDebugMenuCtrl::AddInt(const char* pszName, int32 nValueMin, int32 nValueMax, int32 nValueStep, int32 nValueInit, const char** apszValueList, void(*pfnCallback)(int32, bool))
{
    ITEM* pItem = itemAlloc();
    ASSERT(pItem);

    pItem->Type = ITEM::TYPE_INT;
    pItem->Name = itemCopyString(pszName);

    ITEM::INT* pItemInt = &pItem->Int;
    pItemInt->Value = nValueInit;
    pItemInt->ValueMax = nValueMax;
    pItemInt->ValueMin = nValueMin;
    pItemInt->ValueStep = nValueStep;
    pItemInt->Callback = pfnCallback;
    pItemInt->ValueStrList = itemCopyStringArray(apszValueList, nValueMax);
};


void CDebugMenuCtrl::AddInt(const char* pszName, int32 nValueMin, int32 nValueMax, int32 nValueStep, int32 nValueInit, std::initializer_list<const char*> InitList, void(*pfnCallback)(int32, bool))
{
	ASSERT(nValueMax == int32(InitList.size()), "value max and enum list must match!!");

    const char** apszValueList = new const char*[InitList.size()];
    if (apszValueList)
    {
        for (int32 i = 0; i < int32(InitList.size()); ++i)
            apszValueList[i] = *(InitList.begin() + i);

        AddInt(pszName, nValueMin, nValueMax, nValueStep, nValueInit, apszValueList, pfnCallback);

        delete[] apszValueList;
        apszValueList = nullptr;
    };    
};


void CDebugMenuCtrl::AddBool(const char* pszName, void(*pfnCallback)(bool, bool))
{
    ITEM* pItem = itemAlloc();
    ASSERT(pItem);

    pItem->Type = ITEM::TYPE_BOOLEAN;
    pItem->Name = itemCopyString(pszName);

    ITEM::BOOLEAN* pItemBoolean = &pItem->Boolean;
    pItemBoolean->Value = false;
    pItemBoolean->Callback = pfnCallback;
};


void CDebugMenuCtrl::AddFloat(const char* pszName, float fValueMin, float fValueMax, float fValueStep, float fValueInit, void(*pfnCallback)(float, bool))
{
    ITEM* pItem = itemAlloc();
    ASSERT(pItem);

    pItem->Type = ITEM::TYPE_FLOAT;
    pItem->Name = itemCopyString(pszName);

    ITEM::FLOAT* pItemFloat = &pItem->Float;
    pItemFloat->Value = fValueInit;
    pItemFloat->ValueMax = fValueMax;
    pItemFloat->ValueMin = fValueMin;
    pItemFloat->ValueStep = fValueStep;
    pItemFloat->Callback = pfnCallback;
};


void CDebugMenuCtrl::AddTrigger(const char* pszName, void(*pfnCallback)(void*))
{
    AddTrigger(pszName, pfnCallback, nullptr);
};


void CDebugMenuCtrl::AddTrigger(const char* pszName, void(*pfnCallback)(void*), void* param)
{
    ITEM* pItem = itemAlloc();
    ASSERT(pItem);

    pItem->Type = ITEM::TYPE_TRIGGER;
    pItem->Name = itemCopyString(pszName);

    ITEM::TRIGGER* pItemTrigger = &pItem->Trigger;
    pItemTrigger->Param = param;
    pItemTrigger->Callback = pfnCallback;
};


void CDebugMenuCtrl::AddTrigger(const char* pszName, void(*pfnCallback)(void*), int32 param)
{
    AddTrigger(pszName, pfnCallback, reinterpret_cast<void*>(param));
};


void CDebugMenuCtrl::AddSeparator(bool bEmpty)
{
    ITEM* pItem = itemAlloc();
    ASSERT(pItem);

    pItem->Type = ITEM::TYPE_SEPARATOR;
    pItem->Name = nullptr;
    pItem->State = false;

    ITEM::SEPARATOR* pItemSeparator = &pItem->Separator;
    pItemSeparator->Empty = bEmpty;
};


void CDebugMenuCtrl::AddItem(const char* pszText, void* param)
{
    AddTrigger(pszText, nullptr, param);    
};


void* CDebugMenuCtrl::GetItemParam(void) const
{
    if (m_aItem[m_iSelect].Type != ITEM::TYPE_TRIGGER)
        return nullptr;

    return m_aItem[m_iSelect].Trigger.Param;
};


void CDebugMenuCtrl::checkInput(void)
{
    uint32 DigitalTrigger   = 0;
    uint32 DigitalRepeat    = 0;

    DigitalTrigger  |= CController::GetDigitalTrigger(CController::CONTROLLER_LOCKED_ON_VIRTUAL);
    DigitalTrigger  |= CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);

    DigitalRepeat   |= CController::GetDigitalRepeat(CController::CONTROLLER_LOCKED_ON_VIRTUAL);
    DigitalRepeat   |= CController::GetDigitalRepeat(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);

    if (DigitalRepeat & CController::DIGITAL_LUP)
    {
        m_Result = RESULT_MOVE;
        handleMove(MOVEDIR_UP);
    }
    else if (DigitalRepeat & CController::DIGITAL_LDOWN)
    {
        m_Result = RESULT_MOVE;
        handleMove(MOVEDIR_DOWN);
    }
    else if (DigitalRepeat & CController::DIGITAL_LLEFT)
    {
        m_Result = RESULT_MOVE;
        handleMove(MOVEDIR_LEFT);
    }
    else if (DigitalRepeat & CController::DIGITAL_LRIGHT)
    {
        m_Result = RESULT_MOVE;
        handleMove(MOVEDIR_RIGHT);
    }
    else if (DigitalTrigger & m_uDigitalOK)
    {
        m_Result = RESULT_OK;
        handleOk();
    }
    else if (DigitalTrigger & m_uDigitalCANCEL)
    {
        m_Result = RESULT_CANCEL;
        handleCancel();
    };
};


void CDebugMenuCtrl::handleMove(MOVEDIR MoveDir)
{
    if ((MoveDir == MOVEDIR_LEFT) ||
        (MoveDir == MOVEDIR_RIGHT))
    {
        ITEM* pCurrSelect = &m_aItem[m_iSelect];

        switch (pCurrSelect->Type)
        {
        case ITEM::TYPE_INT:
            {
                ITEM::INT* pItemInt = &pCurrSelect->Int;
                pItemInt->Value += (MoveDir == MOVEDIR_LEFT ? -pItemInt->ValueStep : pItemInt->ValueStep);
                pItemInt->Value = Clamp(pItemInt->Value, pItemInt->ValueMin, pItemInt->ValueMax - 1);
                
                if (pItemInt->Callback)
                    pItemInt->Callback(pItemInt->Value, false);
            }
            break;

        case ITEM::TYPE_FLOAT:
            {
                ITEM::FLOAT* pItemFloat = &pCurrSelect->Float;
                pItemFloat->Value += (MoveDir == MOVEDIR_LEFT ? -pItemFloat->ValueStep : pItemFloat->ValueStep);
                pItemFloat->Value = Clamp(pItemFloat->Value, pItemFloat->ValueMin, pItemFloat->ValueMax);
                
                if (pItemFloat->Callback)
                    pItemFloat->Callback(pItemFloat->Value, false);
            }
            break;

        case ITEM::TYPE_BOOLEAN:
            {
                ITEM::BOOLEAN* pItemBoolean = &pCurrSelect->Boolean;
                pItemBoolean->Value = !pItemBoolean->Value;

                if (pItemBoolean->Callback)
                    pItemBoolean->Callback(pItemBoolean->Value, false);
            }
            break;

        case ITEM::TYPE_TRIGGER:
            break;

        case ITEM::TYPE_SEPARATOR:
            ASSERT(false);
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    if (MoveDir == MOVEDIR_UP)
    {
		int32 count = 0;
		int32 prev = m_iSelect;

        do
        {
            m_iSelect = InvClamp(--m_iSelect, 0, m_iSelectMax - 1);
        } while (!m_aItem[m_iSelect].State);

		count = m_iSelect - prev;
        if (m_iSelect < m_iDispMin)
        {
			m_iDispMin += count;
			m_iDispMax += count;
        }
		else if (count > 0)
		{
            int32 dispCount = m_iDispMax - m_iDispMin;
            m_iDispMin = m_iSelectMax - dispCount;
            m_iDispMax = m_iDispMin + dispCount;
		};
    };

    if (MoveDir == MOVEDIR_DOWN)
    {
		int32 count = 0;
        int32 prev = m_iSelect;

        do
        {
            m_iSelect = InvClamp(++m_iSelect, 0, m_iSelectMax - 1);
        } while (!m_aItem[m_iSelect].State);

		count = m_iSelect - prev;
		if (m_iSelect >= m_iDispMax)
		{
			m_iDispMin += count;
			m_iDispMax += count;
		}
		else if (count < 0)
        {
            int32 dispCount = m_iDispMax - m_iDispMin;
            m_iDispMin = 0;
            m_iDispMax = dispCount;
		};
    };
};


void CDebugMenuCtrl::handleOk(void)
{
    ITEM* pCurrSelect = &m_aItem[m_iSelect];

    switch (pCurrSelect->Type)
    {
    case ITEM::TYPE_INT:
        {
            ITEM::INT* pItemInt = &pCurrSelect->Int;
            
            if (pItemInt->Callback)
                pItemInt->Callback(pItemInt->Value, true);
        }
        break;

    case ITEM::TYPE_FLOAT:
        {
            ITEM::FLOAT* pItemFloat = &pCurrSelect->Float;
            
            if (pItemFloat->Callback)
                pItemFloat->Callback(pItemFloat->Value, true);
        }
        break;

    case ITEM::TYPE_BOOLEAN:
        {
            ITEM::BOOLEAN* pItemBoolean = &pCurrSelect->Boolean;

            if (pItemBoolean->Callback)
                pItemBoolean->Callback(pItemBoolean->Value, true);
        }
        break;

    case ITEM::TYPE_TRIGGER:
        {
            ITEM::TRIGGER* pItemTrigger = &pCurrSelect->Trigger;

            if (pItemTrigger->Callback)
                pItemTrigger->Callback(pItemTrigger->Param);
        }
        break;
    };
};


void CDebugMenuCtrl::handleCancel(void)
{
    ;
};


CDebugMenuCtrl::ITEM* CDebugMenuCtrl::itemAlloc(void)
{
    if (m_listItemFree.empty())
        return nullptr;

    ITEM* pItem = m_listItemFree.front();
    m_listItemFree.erase(pItem);
    m_listItemAlloc.push_back(pItem);

    pItem->State = true;
    pItem->Hex = bool(FLAG_TEST(m_uOptFlag, OPTFLAG_DISP_HEX));

    ++m_iSelectMax;
    
    return pItem;
};


void CDebugMenuCtrl::itemFree(ITEM* pItem)
{
    ASSERT(!m_listItemAlloc.empty());

    ASSERT(m_iSelectMax > 0);
    --m_iSelectMax;

    if (pItem->Name)
    {
        delete[] pItem->Name;
        pItem->Name = nullptr;
    };

    if (pItem->Type == ITEM::TYPE_INT)
    {
        ITEM::INT* pItemInt = &pItem->Int;
        if (pItemInt->ValueStrList)
        {
            for (int32 i = 0; i < pItemInt->ValueMax; ++i)
            {
                delete[] pItemInt->ValueStrList[i];
                pItemInt->ValueStrList[i] = nullptr;
            };

            delete[] pItemInt->ValueStrList;
            pItemInt->ValueStrList = nullptr;
        };
    };

    m_listItemAlloc.erase(pItem);
    m_listItemFree.push_back(pItem);
};


const char* CDebugMenuCtrl::itemCopyString(const char* str) const
{
    char* result = new char[std::strlen(str) + 1];
    std::strcpy(result, str);
    return result;
};


const char** CDebugMenuCtrl::itemCopyStringArray(const char** str, int32 cnt) const
{
    if (!str)
        return nullptr;

    const char** result = new const char*[cnt];
    for (int32 i = 0; i < cnt; ++i)
        result[i] = itemCopyString(str[i]);

    return result;
};


//
// *********************************************************************************
//


CDebugSequenceCheckObj::CDebugSequenceCheckObj(void)
: m_iTargSeqLbl(PROCESSTYPES::LABEL_EOL)
, m_iPrevSeqLbl(PROCESSTYPES::LABEL_EOL)
, m_iCurrSeqLbl(PROCESSTYPES::LABEL_EOL)
{
    ;
};


CDebugSequenceCheckObj& CDebugSequenceCheckObj::Check(int32 iTargetSeqLbl)
{
    m_iTargSeqLbl = iTargetSeqLbl;
    m_iPrevSeqLbl = m_iCurrSeqLbl;
    m_iCurrSeqLbl = CSequence::GetCurrently();

    return *this;
};


#endif /* _DEBUG */