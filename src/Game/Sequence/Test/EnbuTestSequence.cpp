#include "EnbuTestSequence.hpp"

#ifdef _DEBUG

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Sequence/Result/EnbuProc.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/ProcessList.hpp"


#define ParamAsInt(x) \
    static_cast<int32>(reinterpret_cast<uintptr_t>(x))


#define IntAsParam(x) \
    reinterpret_cast<void*>(static_cast<uintptr_t>(x))


/*static*/ CProcess* CEnbuTestSequence::Instance(void)
{
    return new CEnbuTestSequence;
};


CEnbuTestSequence::CEnbuTestSequence(void)
: m_iActiveMenu(-1)
, m_aMenu()
{
    ;
};


CEnbuTestSequence::~CEnbuTestSequence(void)
{
    ;
};


bool CEnbuTestSequence::OnAttach(const void* pParam)
{
    InitMenuChr(m_aMenu[MENU_CHR]);
    InitMenuRank(m_aMenu[MENU_RANK]);

    m_iActiveMenu = MENU_CHR;

    return true;
};


void CEnbuTestSequence::OnDetach(void)
{
    ;
};


void CEnbuTestSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
    {
        CScreenFade::BlackIn(0.0f);
        CGameData::Attribute().SetInteractive(true);
    };

    CDebugMenuCtrl& menu = m_aMenu[m_iActiveMenu];

    menu.Period();
    
    switch (menu.GetResult())
    {
    case CDebugMenuCtrl::RESULT_CANCEL:
        {
            if (m_iActiveMenu > 0)
            {
                --m_iActiveMenu;
            }
            else
            {
                CGameData::Attribute().SetInteractive(false);
                Ret();
            };
        }
        break;

    case CDebugMenuCtrl::RESULT_OK:
        {
            static PLAYERID::VALUE s_idPlayer = PLAYERID::ID_INVALID;

            switch (m_iActiveMenu)
            {
            case MENU_CHR:
                {
                    PLAYERID::VALUE idPlayer =
                        static_cast<PLAYERID::VALUE>(ParamAsInt(menu.GetItemParam()));

                    CGameData::PlayParam().Clear();
                    CGameData::PlayParam().AddPlayerCharacter(0, idPlayer);

                    s_idPlayer = idPlayer;
                    m_iActiveMenu = MENU_RANK;
                }
                break;

            case MENU_RANK:
                {
                    GAMETYPES::CLEARRANK rank =
                        static_cast<GAMETYPES::CLEARRANK>(ParamAsInt(menu.GetItemParam()));

                    CGameData::OnBeginArea();
                    CGameData::OnBeginStage(STAGEID::ID_ST01T);
                    CGameData::OnEndStage(STAGEID::ID_ST01T);
                    CGameData::PlayResult().Evaluate();

                    CEnbuProc::SetTestRankAndMvp(s_idPlayer, rank);

                    CGameData::Attribute().SetInteractive(false);
                    Call(PROCLABEL_SEQ_ENBU);
                }
                break;

            default:
                break;
            };
        }
        break;

    default:
        break;
    };
};


void CEnbuTestSequence::OnDraw(void) const
{
    m_aMenu[m_iActiveMenu].Draw();
};


void CEnbuTestSequence::InitMenuChr(CDebugMenuCtrl& menu)
{
    InitMenuBase(menu);

    for (int32 i = 0; i < PLAYERID::ID_MAX; ++i)
        menu.AddTrigger(PLAYERID::GetName(PLAYERID::VALUE(i)), nullptr, IntAsParam(i));
};


void CEnbuTestSequence::InitMenuRank(CDebugMenuCtrl& menu)
{
    static const char* s_apszRankName[] =
    {
        "NONE", "E", "D", "C", "B", "A", "S", "SS",
    };

    InitMenuBase(menu);

    for (int32 i = (GAMETYPES::CLEARRANK_NONE + 1); i < GAMETYPES::CLEARRANK_NUM; ++i)
        menu.AddTrigger(s_apszRankName[i], nullptr, IntAsParam(i));
};


void CEnbuTestSequence::InitMenuBase(CDebugMenuCtrl& menu)
{
    menu.SetDispMax(20);
    menu.SetPos(100, 100);
    menu.DigitalOK(IGamepad::DIGITAL_OK);
    menu.DigitalCANCEL(IGamepad::DIGITAL_CANCEL);
};

#endif /* _DEBUG */