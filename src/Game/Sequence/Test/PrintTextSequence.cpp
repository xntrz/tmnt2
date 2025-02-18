#include "PrintTextSequence.hpp"

#ifdef _DEBUG

#include "Game/System/Text/GameText.hpp"
#include "System/Common/SystemText.hpp"


bool CPrintTextSequence::OnAttach(const void* pParam)
{
    m_type = reinterpret_cast<int32>(pParam);
    
    return true;
};


void CPrintTextSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_type)
    {
    case 0:
        {
            int32 SysTextNum = CSystemText::GetTextNum();
            for (int32 i = 0; i < SysTextNum; ++i)
                std::wprintf(L"%d (0x%X): %s\n", i, i, CSystemText::GetText(SYSTEXT(i)));
        }
        break;

    default:        
    case 1:
        {

            int32 GameTextNum = CGameText::GetTextNum();
            for (int32 i = 0; i < GameTextNum; ++i)
                std::wprintf(L"%d (0x%X): %s\n", i, i, CGameText::GetText(GAMETEXT(i)));
        }
        break;
    };

    Ret();
};


#endif /* _DEBUG */