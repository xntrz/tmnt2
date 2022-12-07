#include "SdTestSeq.hpp"


/*static*/ CProcess* CSdTestSeq::Instance(void)
{
    return new CSdTestSeq;
};


CSdTestSeq::CSdTestSeq(void)
: m_pImpl(nullptr)
{
    ;
};


CSdTestSeq::~CSdTestSeq(void)
{
    ;
};


bool CSdTestSeq::OnAttach(const void* param)
{
    return true;
};


void CSdTestSeq::OnDetach(void)
{
    ;
};


void CSdTestSeq::OnMove(bool bRet, const void* param)
{
    Ret();
};


void CSdTestSeq::OnDraw(void) const
{
    ;
};