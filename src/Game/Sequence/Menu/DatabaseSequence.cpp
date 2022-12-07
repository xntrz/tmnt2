#include "DatabaseSequence.hpp"

#include "System/Common/Process/ProcessList.hpp"


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


bool CDatabaseSequence::OnAttach(const void* param)
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


void CDatabaseSequence::OnMove(bool bRet, const void* param)
{
    if (bRet)
    {
        if (param)
            m_iLabelCurrent = int32(param);
        else
            m_iLabelCurrent = DatabaseBranch(m_iLabelCurrent);
    }
    else
    {
        m_iLabelCurrent = PROCESSTYPES::LABEL_SEQ_DATABASE;
    };

    bool bResult = false;
    if (m_iLabelCurrent == PROCESSTYPES::LABEL_SEQ_HOME)
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
    case PROCESSTYPES::LABEL_SEQ_DATABASE:
        return PROCESSTYPES::LABEL_SEQ_UNLOCK;

    case PROCESSTYPES::LABEL_SEQ_DATABASEVIEWER:
        return PROCESSTYPES::LABEL_SEQ_DATABASE;

    case PROCESSTYPES::LABEL_SEQ_UNLOCK:
        return PROCESSTYPES::LABEL_SEQ_HOME;

    default:
        return PROCESSTYPES::LABEL_EOL;
    };
};