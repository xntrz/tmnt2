#include "DebugMenuSequence.hpp"

#ifdef _DEBUG

#include "DbgUnlockProcess.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Configure.hpp"


static CSequence* s_pDebugMenuSeq = nullptr;


//
//	Calls specified sequence
// 	Values for param:
// 		>0	- sequence label
//      =0  - start normal game sequence
//      -1  - start "all completed" game sequence
// 		-2 	- exit
//
static void CallSeq(void* param)
{
    switch (int32(param))
    {
    case -1:
        CDbgUnlockProcess::Launch(s_pDebugMenuSeq, CDbgUnlockProcess::UNLOCKFLAG_ALL);
    case 0:
        s_pDebugMenuSeq->Jump(PROCLABEL_SEQ_SAVELOADCHECK);
        break;

    case -2:
        s_pDebugMenuSeq->Kill(PROCESSTYPES::LABEL_EOL);
        break;

    default:
        s_pDebugMenuSeq->Call(int32(param));
        break;
    };
};


#define CallAreaParamMake(AreaId, StageNo, Flag)    ((AreaId)   & 0xFF)          | \
                                                    (((Flag)    & 0xFFFF) << 8)  | \
                                                    (((StageNo) & 0xFF)   << 24)

#define CallAreaParamMakeEx(AreaId, StageNo)        CallAreaParamMake(AreaId, StageNo)

#define CallAreaParamGetAreaId(Param)               ((Param)         & 0xFF)
#define CallAreaParamGetStageNo(Param)              (((Param) >> 24) & 0xFF)
#define CallAreaParamGetFlag(Param)                 (((Param) >> 8)  & 0xFFFF)

#define CALLAREA_FLAG_GMKGEN_DISABLE                 (1 << 0)

//
//	Calls specified area play sequence
// 	Values for param:
// 		any NORMAL range of area id
//
static void CallArea(void* param)
{
    uint32 AreaParam = uint32(param);

    AREAID::VALUE idArea = AREAID::VALUE(CallAreaParamGetAreaId(AreaParam));
    ASSERT(idArea >= AREAID::NORMALSTART);
    ASSERT(idArea < AREAID::NORMALMAX);

    int32 StageNo = CallAreaParamGetStageNo(AreaParam);
    ASSERT(StageNo >= 0);
    ASSERT(StageNo < GAMETYPES::STAGE_MAX);
    ASSERT(StageNo < CAreaInfo::GetStageNum(idArea));

    CGameData::PlayParam().Clear();
    int32 SecretChar = (Math::Rand() % 100) >= 50 ? 4 : 0;
    for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
        CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::VALUE(SecretChar + i), GAMETYPES::COSTUME(Math::Rand() % GAMETYPES::COSTUME_NUM));
    CGameData::PlayParam().SetStartArea(idArea, StageNo);
    
    for (int32 i = 0; i < GAMETYPES::CRY_MAX; ++i)
	{
		CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_RED, i, true);
		CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_GREEN, i, true);
		CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_WHITE, i, true);
		CGameData::Record().Item().DebugSetCryTaken(GAMETYPES::CRYSTALTYPE_ORANGE, i, true);
    };

    CGameData::Option().Display().SetEnableFontEffect(false);
    CGameData::Option().Play().SetClassicInput(true);
	CGameData::PlayParam().SetAreaPlaymode(CGamePlayParam::AREAPLAYMODE_ONE_STAGE);

    uint32 Flag = CallAreaParamGetFlag(AreaParam);
    CGimmickDebug::DISABLE_GENERERATOR = bool(Flag & CALLAREA_FLAG_GMKGEN_DISABLE);

    bool bSkipIntroOutroMovies = true;
    s_pDebugMenuSeq->Call(PROCLABEL_SEQ_AREAPLAY, reinterpret_cast<const void*>(bSkipIntroOutroMovies));
};


//
//	Calls text print sequence
// 	Values for param:
// 		0 - system text
//      1 - game text
//
static void CallPrintText(void* param)
{
    s_pDebugMenuSeq->Call(PROCLABEL_SEQ_MAKETEXT, param, true);
};


/*static*/ CProcess* CDebugMenuSequence::Instance(void)
{
    return new CDebugMenuSequence;
};


CDebugMenuSequence::CDebugMenuSequence(void)
: m_menu()
{
    ;
};


CDebugMenuSequence::~CDebugMenuSequence(void)
{
    ;
};


bool CDebugMenuSequence::OnAttach(const void* pParam)
{
    CScreenFade::BlackIn(0.0f);
    
    s_pDebugMenuSeq = this;

    m_menu.SetDispMax(40);
    m_menu.SetPos(100, 100);
    m_menu.AddTrigger("Test movie",             CallSeq,        PROCLABEL_SEQ_TESTMV);
    m_menu.AddTrigger("Test sound",             CallSeq,        PROCLABEL_SEQ_TESTSD);
    m_menu.AddTrigger("Test pad",               CallSeq,        PROCLABEL_SEQ_TESTPAD);
    m_menu.AddTrigger("Print system text",      CallPrintText,  0);
    m_menu.AddTrigger("Print game text",        CallPrintText,  1);
    m_menu.AddSeparator();
    m_menu.AddInt("Call area map view", 0, 7, 1, 0, { "NY", "ZERO", "DHO", "TRI", "JPN", "FNY", "KURA" }, [](int32 nValue, bool bTrigger) {    
        const AREAID::VALUE idAreaOfWorld[] =
        {
            AREAID::ID_AREA01,  // ny
            AREAID::ID_AREA12,  // zero
            AREAID::ID_AREA15,  // dho
            AREAID::ID_AREA25,  // tri
            AREAID::ID_AREA33,  // japan
            AREAID::ID_AREA48,  // fny
            AREAID::ID_AREA52,  // kura
        };
        
        if (bTrigger)
            s_pDebugMenuSeq->Call(PROCLABEL_SEQ_AREA, (const void*)idAreaOfWorld[nValue]);
    });
    m_menu.AddSeparator();
    m_menu.AddTrigger("SNOW test",              CallArea,       AREAID::ID_AREA14);
    m_menu.AddTrigger("SNOW ride test",         CallArea,       AREAID::ID_AREA13);
    m_menu.AddTrigger("RAIN test",              CallArea,       AREAID::ID_AREA49);
    m_menu.AddTrigger("test StaffRoll",         CallSeq,        PROCLABEL_SEQ_STAFFROLL);
    m_menu.AddTrigger("switch pillar test",     CallArea,       AREAID::ID_AREA04);
    m_menu.AddTrigger("Utrom warp test",        CallArea,       AREAID::ID_AREA40);
    m_menu.AddTrigger("crumble floor test",     CallArea,       AREAID::ID_AREA12);
    m_menu.AddTrigger("gs machine test",        CallArea,       AREAID::ID_AREA17);
    m_menu.AddTrigger("fall rubble test",       CallArea,       CallAreaParamMake(AREAID::ID_AREA58, 1, CALLAREA_FLAG_GMKGEN_DISABLE));
    m_menu.AddTrigger("space ship engine test", CallArea,       AREAID::ID_AREA28);
    m_menu.AddTrigger("trailer chase test",     CallArea,       AREAID::ID_AREA43);
    m_menu.AddSeparator();
    m_menu.AddTrigger("Start NORMAL game!", CallSeq, 0);
    m_menu.AddTrigger("Start UNLOCK game!", CallSeq, -1);
    m_menu.AddTrigger("Exit",               CallSeq, -2);

    return true;
};


void CDebugMenuSequence::OnDetach(void)
{
    s_pDebugMenuSeq = nullptr;

    CScreenFade::BlackOut(0.0f);
};


void CDebugMenuSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
        CScreenFade::BlackIn(0.0f);

    m_menu.Period();
};


void CDebugMenuSequence::OnDraw(void) const
{
    m_menu.Draw();
};

#endif /* _DEBUG */