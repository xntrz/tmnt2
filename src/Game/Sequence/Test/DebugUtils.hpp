#pragma once

#ifdef _DEBUG

#include "rtcharse.h"


class CDebugFontCtrl
{
public:
    CDebugFontCtrl(void);
    ~CDebugFontCtrl(void);
    void Background(const RwRGBA& color);
    void Color(const RwRGBA& color);
    void Position(int32 x, int32 y);
    void PositionAdd(int32 x, int32 y);
    void Print(const char* format, ...);
    void SetAutoStep(int32 x = 0, int32 y = 0);
    void Reset(void);
    
private:
    RtCharset* m_pCharset;
    RwRGBA m_ColorBg;
    RwRGBA m_ColorText;
    int32 m_stepX;
    int32 m_stepY;
    int32 m_x;
    int32 m_y;
    int32 m_orgX;
    int32 m_orgY;  
};


class CDebugMenuCtrl
{
public:
    enum RESULT
    {
        RESULT_NONE = 0,
        RESULT_MOVE,
        RESULT_OK,
        RESULT_CANCEL,
    };

private:
    struct ITEM : public CListNode<ITEM>
    {
		struct INT
		{
			int32 Value;
			int32 ValueStep;
			int32 ValueMax;
			int32 ValueMin;
			void(*Callback)(int32, bool);
			const char** ValueStrList;
		};

		struct FLOAT
		{
			float Value;
			float ValueStep;
			float ValueMax;
			float ValueMin;
			void(*Callback)(float, bool);
		};

		struct BOOLEAN
		{
			bool Value;
			void(*Callback)(bool, bool);
		};

		struct TRIGGER
		{
			void* Param;
			void(*Callback)(void*);
        };

        struct SEPARATOR
        {
            bool Empty;
        };

        enum TYPE
        {
            TYPE_NONE = 0,
            TYPE_INT,
            TYPE_FLOAT,
            TYPE_BOOLEAN,
            TYPE_TRIGGER,
            TYPE_SEPARATOR,
        };

        TYPE Type;
        const char* Name;
        bool State;
        bool Hex;
        union
        {
			INT Int;
			FLOAT Float;
			BOOLEAN Boolean;
            TRIGGER Trigger;
            SEPARATOR Separator;
        };
    };
    
    enum MOVEDIR
    {
        MOVEDIR_UP = 0,
        MOVEDIR_DOWN,
        MOVEDIR_LEFT,
        MOVEDIR_RIGHT,
    };

    enum OPTFLAG : uint32
    {
        OPTFLAG_DISP_HEX = (1 << 0),
        OPTFLAG_REPE_MOV = (1 << 1),
    };

public:
    CDebugMenuCtrl(void);
    ~CDebugMenuCtrl(void);
    void Reset(void);
    void Period(void);
    void Draw(void) const;
    void AddInt(const char* pszName, int32 nValueMin, int32 nValueMax, int32 nValueStep, int32 nValueInit, void(*pfnCallback)(int32, bool));
    void AddInt(const char* pszName, int32 nValueMin, int32 nValueMax, int32 nValueStep, int32 nValueInit, const char** apszValueList, void(*pfnCallback)(int32, bool));
    void AddInt(const char* pszName, int32 nValueMin, int32 nValueMax, int32 nValueStep, int32 nValueInit, std::initializer_list<const char*> InitList, void(*pfnCallback)(int32, bool));
    void AddBool(const char* pszName, void(*pfnCallback)(bool, bool));
    void AddFloat(const char* pszName, float fValueMin, float fValueMax, float fValueStep, float fValueInit, void(*pfnCallback)(float, bool));
    void AddTrigger(const char* pszName, void(*pfnCallback)(void*));
    void AddTrigger(const char* pszName, void(*pfnCallback)(void*), void* param);
    void AddTrigger(const char* pszName, void(*pfnCallback)(void*), int32 param);
    void AddSeparator(bool bEmpty = false);
    void AddItem(const char* pszText, void* param = 0);
    void* GetItemParam(void) const;

    inline void SetDispMax(int32 iMax)                  { m_iDispMax = iMax; };
    inline void SetPos(int32 x, int32 y)                { m_iPosX = x; m_iPosY = y; };
    inline void SetHeight(int32 h)                      { m_iHeight = h; };
    inline void SetColorSelect(const RwRGBA& color)     { m_ColorSelect = color; };
    inline void SetColorUnselect(const RwRGBA& color)   { m_ColorUnselect = color; };
    inline void SetColorBackground(const RwRGBA& color) { m_font.Background(color); };
    inline int32 GetSelect(void) const                  { return m_iSelect; };
    inline RESULT GetResult(void) const                 { return m_Result; };
    inline void HexDispBegin(void)                      { m_uOptFlag |= (OPTFLAG_DISP_HEX); };
    inline void HexDispEnd(void)                        { m_uOptFlag &= (~OPTFLAG_DISP_HEX); };
    inline void DigitalOK(uint32 DigitalMask)           { m_uDigitalOK = DigitalMask; };
    inline void DigitalCANCEL(uint32 DigitalMask)       { m_uDigitalCANCEL = DigitalMask; };

protected:
    void checkInput(void);
    void handleMove(MOVEDIR MoveDir);
    void handleOk(void);
    void handleCancel(void);
    ITEM* itemAlloc(void);
    void itemFree(ITEM* pItem);
    const char* itemCopyString(const char* str) const;
    const char** itemCopyStringArray(const char** str, int32 cnt) const;

protected:
    ITEM m_aItem[256];
    CList<ITEM> m_listItemAlloc;
    CList<ITEM> m_listItemFree;
    mutable CDebugFontCtrl m_font;
    RwRGBA m_ColorUnselect;
    RwRGBA m_ColorSelect;
    int32 m_iSelect;
    int32 m_iSelectMax;
    int32 m_iDispMin;
    int32 m_iDispMax;
    int32 m_iDispMaxOrg;
    int32 m_iPosX;
    int32 m_iPosY;
    int32 m_iHeight;
    RESULT m_Result;
    uint32 m_uOptFlag;
    uint32 m_uDigitalOK;
    uint32 m_uDigitalCANCEL;
};


class CDebugSequenceCheckObj
{
public:
    CDebugSequenceCheckObj(void);
    CDebugSequenceCheckObj& Check(int32 iTargetSeqLbl);

    inline bool IsStarted(void) const { return (m_iPrevSeqLbl != m_iTargSeqLbl) && (m_iCurrSeqLbl == m_iTargSeqLbl); };
    inline bool IsRunning(void) const { return (m_iPrevSeqLbl == m_iTargSeqLbl) && (m_iCurrSeqLbl == m_iTargSeqLbl); };
    inline bool IsEnded(void) const   { return (m_iPrevSeqLbl == m_iTargSeqLbl) && (m_iCurrSeqLbl != m_iTargSeqLbl); };

private:
    int32 m_iTargSeqLbl;
    int32 m_iPrevSeqLbl;
    int32 m_iCurrSeqLbl;
};


#endif /* _DEBUG */