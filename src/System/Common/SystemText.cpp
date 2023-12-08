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


/*static*/ int32 CSystemText::GetTextNum(void)
{
    return s_SystemText.GetTextNum();
};


/*static*/ const wchar* CSystemText::GetText(SYSTEXT id)
{
    return s_SystemText.GetText(id);
};


/*static*/ const wchar* CSystemText::EmptyText(void)
{
    return CTextData::EMPTYTEXT;
};