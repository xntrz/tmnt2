#include "DatabaseSequence.hpp"

#include "Game/ProcessList.hpp"


static int32 s_iDBViewerNo = 0;


/*static*/ void CDatabaseSequence::SetDBViewerNo(int32 iDBViewerNo)
{
    s_iDBViewerNo = iDBViewerNo;
};


/*static*/ int32 CDatabaseSequence::GetDBViewerNo(void)
{
    return s_iDBViewerNo;
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
    return true;
};


void CDatabaseSequence::OnDetach(void)
{
    ;
};


void CDatabaseSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
        if (pReturnValue)
            m_iLabelCurrent = reinterpret_cast<int32>(pReturnValue);
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