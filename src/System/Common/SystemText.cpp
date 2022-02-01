#include "SystemText.hpp"
#include "TextData.hpp"


static CTextData s_SystemText;


/*static*/ void CSystemText::Initialize(void)
{
    ;
};


/*static*/ void CSystemText::Terminate(void)
{
    s_SystemText.Clear();
};


/*static*/ void CSystemText::Read(const void* pBuffer, uint32 uBufferSize)
{
    s_SystemText.Read(pBuffer, uBufferSize);
};


/*static*/ const wchar* CSystemText::GetText(SYSTEXT::VALUE id)
{
    return s_SystemText.GetText(id);
};


/*static*/ int32 CSystemText::GetTextNum(void)
{
    return s_SystemText.GetTextNum();
};


/*static*/ const wchar* CSystemText::EmptyText(void)
{
    return &CTextData::EMPTYTEXT;
};