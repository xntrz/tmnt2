#include "MvTestSeq.hpp"

#include "Game/Component/GameMain/MovieID.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Movie/MovieDataManager.hpp"
#include "System/Common/Process/ProcessList.hpp"
#include "System/Common/Controller.hpp"

#include "rtcharse.h"


class CMvTest
{
public:
    static const int32 VIEW_ITEM_MAX = 30;
    static const int32 POS_X = 100;
    static const int32 POS_Y = 200;
    
    enum RESULT
    {
        RESULT_NONE = 0,
        RESULT_EXIT,
        RESULT_CALLTEST,
    };
    
public:
    CMvTest(void);
    ~CMvTest(void);
    void Move(void);
    void Draw(void);
    void Input(void);
    void OnReturn(void);
    RESULT GetResult(void);
    int32 GetMovieID(void);

private:
    RtCharset* m_pCharset;
    RESULT m_Result;
    int32 m_iCursor;
    int32 m_iItemDispMin;
    int32 m_iItemDispMax;
    int32 m_iCursorMin;
    int32 m_iCursorMax;
};


CMvTest::CMvTest(void)
: m_pCharset(nullptr)
, m_Result(RESULT_NONE)
, m_iCursor(0)
, m_iCursorMin(0)
, m_iCursorMax(MOVIEID::ID_MAX)
, m_iItemDispMin(0)
, m_iItemDispMax(VIEW_ITEM_MAX)
{
    RwRGBA ColorForeground = { 0xFF, 0xFF, 0xFF, 0xFF };
    RwRGBA ColorBackground = { 0x00, 0x00, 0x00, 0x00 };    
    m_pCharset = RtCharsetCreate(&ColorForeground, &ColorBackground);
    ASSERT(m_pCharset);
};


CMvTest::~CMvTest(void)
{
    if (m_pCharset)
    {
        RtCharsetDestroy(m_pCharset);
        m_pCharset = nullptr;
    };
};


void CMvTest::Move(void)
{
    Input();
};


void CMvTest::Draw(void)
{
    RwRGBA ColorBackground = { 0x00, 0x00, 0x00, 0x00 };
    RwRGBA ColorForegroundUnselect = { 0xFF, 0xFF, 0xFF, 0xFF };
    RwRGBA ColorForegroundSelect = { 0xD0, 0x00, 0x00, 0xFF };

	RtCharsetSetColors(m_pCharset, &ColorForegroundUnselect, &ColorBackground);
	RtCharsetPrint(m_pCharset, "<START>  - Play movie", POS_X, POS_Y - 100);
	RtCharsetPrint(m_pCharset, "<SELECT> - Exit", POS_X, POS_Y - 80);

	for (int32 i = m_iItemDispMin; i < m_iItemDispMax; ++i)
	{
		char Buffer[256];
		Buffer[0] = '\0';

		std::sprintf(Buffer, "%02d) %s", i + 1, CMovieDataManager::GetLabel(i));

		if (i == m_iCursor)
			RtCharsetSetColors(m_pCharset, &ColorForegroundSelect, &ColorBackground);
		else
			RtCharsetSetColors(m_pCharset, &ColorForegroundUnselect, &ColorBackground);

        RtCharsetPrint(
            m_pCharset,
            Buffer,
            POS_X,
            POS_Y + ((i - m_iItemDispMin) * 20)
        );
    };
};


void CMvTest::Input(void)
{
    if (CController::GetDigitalRepeat(0, CController::DIGITAL_UP))
    {
        m_iCursor = Math::Clamp(m_iCursor - 1, m_iCursorMin, m_iCursorMax - 1);
        if (m_iCursor < m_iItemDispMin)
        {
            --m_iItemDispMin;
            --m_iItemDispMax;
        };
    }
    else if (CController::GetDigitalRepeat(0, CController::DIGITAL_DOWN))
    {
        m_iCursor = Math::Clamp(m_iCursor + 1, m_iCursorMin, m_iCursorMax - 1);
        if (m_iCursor >= m_iItemDispMax)
        {
            ++m_iItemDispMin;
            ++m_iItemDispMax;
        };
    }
    else if (CController::GetDigitalTrigger(0, CController::DIGITAL_OK))
    {
        m_Result = RESULT_CALLTEST;
    }
    else if (CController::GetDigitalTrigger(0, CController::DIGITAL_CANCEL))
    {
        m_Result = RESULT_EXIT;
    };
};


void CMvTest::OnReturn(void)
{
    m_Result = RESULT_NONE;
};


CMvTest::RESULT CMvTest::GetResult(void)
{
    return m_Result;
};


int32 CMvTest::GetMovieID(void)
{
    return m_iCursor;
};


/*static*/ CProcess* CMvTestSeq::Instance(void)
{
    return new CMvTestSeq;
};


CMvTestSeq::CMvTestSeq(void)
: m_pImpl(nullptr)
{
    ;
};


CMvTestSeq::~CMvTestSeq(void)
{
    ;
};


bool CMvTestSeq::OnAttach(const void* param)
{
    CScreenFade::StartOut(0.0f);
    
    m_pImpl = new CMvTest;
    return true;
};


void CMvTestSeq::OnDetach(void)
{    
    if (m_pImpl)
    {
        delete m_pImpl;
        m_pImpl = nullptr;
    };
    
    CScreenFade::StartIn(0.0f);
};


void CMvTestSeq::OnMove(bool bRet, const void* param)
{
    if (bRet)
    {
        m_pImpl->OnReturn();
    }
    else
    {
        m_pImpl->Move();
        
        CMvTest::RESULT Result = m_pImpl->GetResult();
        switch (Result)
        {
        case CMvTest::RESULT_EXIT:            
            Ret();
            break;

        case CMvTest::RESULT_CALLTEST:
            Call(PROCESSTYPES::LABEL_SEQ_MOVIE, false, (void*)m_pImpl->GetMovieID());
            break;
        };
    };    
};


void CMvTestSeq::OnDraw(void) const
{
    m_pImpl->Draw();
};