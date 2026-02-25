#include "TestMovieSequence.hpp"

#ifdef _DEBUG

#include "DebugUtils.hpp"

#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Movie/MovieDataManager.hpp"
#include "Game/ProcessList.hpp"


/*static*/ CProcess* CTestMovieSequence::Instance(void)
{
    return new CTestMovieSequence;
};


CTestMovieSequence::CTestMovieSequence(void)
{
    ;
};


CTestMovieSequence::~CTestMovieSequence(void)
{
    ;
};


bool CTestMovieSequence::OnAttach(const void* pParam)
{
    CScreenFade::BlackIn(0.0f);

    m_menu.SetDispMax(20);
    m_menu.SetPos(100, 100);
    m_menu.DigitalOK(IPad::DIGITAL_START);
    m_menu.DigitalCANCEL(IPad::DIGITAL_SELECT);

    int32 mvDataNum = CMovieDataManager::GetMovieDataNum();
    for (int32 i = 0; i < mvDataNum; ++i)
    {
        m_menu.AddTrigger(CMovieDataManager::GetMovieData(i)->pszFilename,
                          nullptr,
            const_cast<char*>(CMovieDataManager::GetMovieData(i)->pszFilename));
    };

    return true;
};


void CTestMovieSequence::OnDetach(void)
{
    CScreenFade::BlackOut(0.0f);
};


void CTestMovieSequence::OnMove(bool bRet, const void* pReturnValue)
{
    m_menu.Period();

    CDebugMenuCtrl::RESULT Result = m_menu.GetResult();
    switch (Result)
    {
    case CDebugMenuCtrl::RESULT_OK:        
        Call(PROCLABEL_SEQ_MOVIE, m_menu.GetItemParam());
        break;

    case CDebugMenuCtrl::RESULT_CANCEL:
        Ret();
        break;

    default:
        break;
    };
};


void CTestMovieSequence::OnDraw(void) const
{
    m_font.Color({ 0xFF, 0xFF, 0xFF, 0xFF });
    m_font.Background({ 0x00, 0x00, 0x00, 0xFF });
    m_font.Position(80, 40);
    m_font.Print("<START>   - Play");
    m_font.Position(80, 60);
    m_font.Print("<SELECT>  - Exit");

    m_menu.Draw();
};

#endif /* _DEBUG */