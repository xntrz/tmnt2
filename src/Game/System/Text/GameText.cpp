#include "GameText.hpp"

#include "System/Common/TextData.hpp"


static CTextData s_GameText;


/*static*/ void CGameText::Initialize(void)
{
    ;
};


/*static*/ void CGameText::Terminate(void)
{
    s_GameText.Clear();
};


/*static*/ void CGameText::Read(const void* pBuffer, uint32 uBufferSize)
{
    s_GameText.Read(pBuffer, uBufferSize);
};


/*static*/ int32 CGameText::GetTextNum(void)
{
    return s_GameText.GetTextNum();
};


/*static*/ const wchar* CGameText::GetText(GAMETEXT id)
{
    return s_GameText.GetText(id);
};


/*static*/ const wchar* CGameText::EmptyText(void)
{
    return CTextData::EMPTYTEXT;
};

