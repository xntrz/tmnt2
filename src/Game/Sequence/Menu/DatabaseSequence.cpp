#include "DatabaseSequence.hpp"

#include "Game/ProcessList.hpp"


class CDatabaseSequence_Container
{
public:
    CDatabaseSequence_Container(void);
    ~CDatabaseSequence_Container(void);
    void SetDBViewerNo(int32 iDbViewerNo);
    int32 GetDBViewerNo(void) const;

private:
    int32 m_iDbViewerNo;
};


CDatabaseSequence_Container::CDatabaseSequence_Container(void)
: m_iDbViewerNo(0)
{
    ;
};


CDatabaseSequence_Container::~CDatabaseSequence_Container(void)
{
    ;
};


void CDatabaseSequence_Container::SetDBViewerNo(int32 iDbViewerNo)
{
    m_iDbViewerNo = iDbViewerNo;
};


int32 CDatabaseSequence_Container::GetDBViewerNo(void) const
{
    return m_iDbViewerNo;
};


static CDatabaseSequence_Container* s_pDatabaseSequence_Container = nullptr;


/*static*/ int32 CDatabaseSequence::GetDBViewerNo(void)
{
    return s_pDatabaseSequence_Container->GetDBViewerNo();
};


/*static*/ void CDatabaseSequence::SetDBViewerNo(int32 iDbViewerNo)
{
    s_pDatabaseSequence_Container->SetDBViewerNo(iDbViewerNo);
};


/*static*/ CProcess* CDatabaseSequence::Instance(void)
{
    return new CDatabaseSequence;
};


CDatabaseSequence::CDatabaseSequence(void)
: m_iLabelCurrent(0)
{
    ;
};


CDatabaseSequence::~CDatabaseSequence(void)
{
    ;
};


bool CDatabaseSequence::OnAttach(const void* pParam)
{
    if (!s_pDatabaseSequence_Container)
        s_pDatabaseSequence_Container = new CDatabaseSequence_Container;

    return true;
};


void CDatabaseSequence::OnDetach(void)
{
    if (s_pDatabaseSequence_Container)
    {
        delete s_pDatabaseSequence_Container;
        s_pDatabaseSequence_Container = nullptr;
    };
};


void CDatabaseSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
        if (pReturnValue)
            m_iLabelCurrent = int32(pReturnValue);
        else
            m_iLabelCurrent = DatabaseBranch(m_iLabelCurrent);
    }
    else
    {
        m_iLabelCurrent = PROCLABEL_SEQ_DATABASE;
    };

    bool bResult = false;
    if (m_iLabelCurrent == PROCLABEL_SEQ_HOME)
        bResult = Ret();
    else
        bResult = Call(m_iLabelCurrent);
    ASSERT(bResult != false);
};


void CDatabaseSequence::OnDraw(void) const
{
    ;
};


int32 CDatabaseSequence::DatabaseBranch(int32 iLabel)
{
    switch (iLabel)
    {
    case PROCLABEL_SEQ_DATABASE:
        return PROCLABEL_SEQ_UNLOCK;

    case PROCLABEL_SEQ_DATABASEVIEWER:
        return PROCLABEL_SEQ_DATABASE;

    case PROCLABEL_SEQ_UNLOCK:
        return PROCLABEL_SEQ_HOME;

    default:
        return PROCESSTYPES::LABEL_EOL;
    };
};