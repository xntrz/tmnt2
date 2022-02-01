#include "MessageWindow.hpp"


/*static*/ int32 CMessageWindow::m_iWindowNum = 0;
/*static*/ CMessageWindow::WINDOWINFO CMessageWindow::m_aWindowTable[CMessageWindow::MESSAGEWINDOW_MAX];
/*static*/ RwRGBA CMessageWindow::m_aColorTable[]
{
    { 0x7E, 0xFF, 0x78, 0xFF },
    { 0x79, 0xA2, 0xFF, 0xFF },
    { 0xFF, 0x79, 0xB1, 0xFF },
    { 0x7E, 0xFF, 0x78, 0x80 },
};


/*staic*/ void CMessageWindow::Initialize(void)
{
    ASSERT(m_iWindowNum == 0);

    for (int32 i = 0; i < COUNT_OF(m_aWindowTable); ++i)
    {
        m_aWindowTable[i].m_pWindow = nullptr;
        m_aWindowTable[i].m_priority = PRIORITY_NORMAL;
    };
};


/*staic*/ void CMessageWindow::Terminate(void)
{
    ;
};


/*staic*/ void CMessageWindow::DrawNormal(void)
{
    for (int32 i = 0; i < m_iWindowNum; ++i)
    {
        if (m_aWindowTable[i].m_pWindow &&
            m_aWindowTable[i].m_priority == CMessageWindow::PRIORITY_NORMAL)
        {
            m_aWindowTable[i].m_pWindow->Draw();
        };
    };
};


/*staic*/ void CMessageWindow::DrawFront(void)
{
    for (int32 i = 0; i < m_iWindowNum; ++i)
    {
        if (m_aWindowTable[i].m_pWindow &&
            m_aWindowTable[i].m_priority == CMessageWindow::PRIORITY_TOP)
        {
            m_aWindowTable[i].m_pWindow->Draw();
        };
    };
};


/*staic*/ void CMessageWindow::Move(void)
{
    for (int32 i = 0; i < m_iWindowNum; ++i)
    {
        if (m_aWindowTable[i].m_pWindow)
            m_aWindowTable[i].m_pWindow->Period();        
    };
};


CMessageWindow::CMessageWindow(COLOR color)
: m_iWindowIndex(-1)
{
    ASSERT(m_iWindowNum < MESSAGEWINDOW_MAX);

    for (int32 i = 0; i < COUNT_OF(m_aWindowTable); ++i)
    {
        if (!m_aWindowTable[ i ].m_pWindow)
        {
            m_iWindowIndex = i;
            break;
        };
    };

    ASSERT(m_iWindowIndex != -1);

    m_aWindowTable[m_iWindowIndex].m_pWindow = this;
    m_aWindowTable[m_iWindowIndex].m_priority = PRIORITY_NORMAL;

	++m_iWindowNum;

    SetColor(color);
};


CMessageWindow::~CMessageWindow(void)
{
    ASSERT(m_iWindowNum > 0);
    ASSERT(m_iWindowIndex >= 0 && m_iWindowIndex < MESSAGEWINDOW_MAX);
    ASSERT(m_aWindowTable[m_iWindowIndex].m_pWindow);
    ASSERT(m_aWindowTable[m_iWindowIndex].m_pWindow == this);

    m_aWindowTable[m_iWindowIndex].m_pWindow = nullptr;
    --m_iWindowNum;
};


void CMessageWindow::SetColor(COLOR color)
{
    ASSERT(color >= 0 && color < COUNT_OF(m_aColorTable));
    
    m_aSprite[0].SetRGBA(m_aColorTable[color]);
    m_aSprite[1].SetRGBA(m_aColorTable[color]);
    m_aSprite[2].SetRGBA(m_aColorTable[color]);
};


void CMessageWindow::SetPriority(PRIORITY priority)
{
    ASSERT(m_iWindowIndex >= 0 && m_iWindowIndex < MESSAGEWINDOW_MAX);
    ASSERT(m_aWindowTable[m_iWindowIndex].m_pWindow);
    ASSERT(m_aWindowTable[m_iWindowIndex].m_pWindow == this);

    m_aWindowTable[m_iWindowIndex].m_priority = priority;
};

