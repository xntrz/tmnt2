#include "DebugMainSequence.hpp"

#ifdef _DEBUG

#include "DebugMenuProcess.hpp"
#include "DbgUnlockProcess.hpp"

#include "Enemy/EnemyTestPlayerAI.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/Gimmick/GimmickDebug.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Configure.hpp"


static CSequence* s_pDebugMainSeq = nullptr;
static bool s_bNormalGamePAI = false;


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
        CDbgUnlockProcess::Launch(s_pDebugMainSeq, CDbgUnlockProcess::UNLOCKFLAG_ALL);
    case 0:
        s_pDebugMainSeq->Jump(PROCLABEL_SEQ_SAVELOADCHECK);
        break;

    case -2:
        s_pDebugMainSeq->Kill(PROCESSTYPES::LABEL_EOL);
        break;

    default:
        s_pDebugMainSeq->Call(int32(param));
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
#define CALLAREA_FLAG_STAGE_SERIES                   (1 << 1)


//
//	Calls specified area play sequence
// 	Values for param:
// 		any NORMAL range of area id
//
static void CallArea(void* param)
{
    uint32 AreaParam = uint32(param);

    AREAID::VALUE idArea = AREAID::VALUE(CallAreaParamGetAreaId(AreaParam));
    //ASSERT(idArea >= AREAID::NORMALSTART);
    //ASSERT(idArea < AREAID::NORMALMAX);

    int32 StageNo = CallAreaParamGetStageNo(AreaParam);
    ASSERT(StageNo >= 0);
    ASSERT(StageNo < GAMETYPES::STAGE_MAX);
    ASSERT(StageNo < CAreaInfo::GetStageNum(idArea));

    CGameData::PlayParam().Clear();
    int32 SecretChar = 0;//(Math::Rand() % 100) >= 50 ? 4 : 0;
    for (int32 i = 0; i < GAMETYPES::PLAYERS_MAX; ++i)
    {
        CGameData::PlayParam().AddPlayerCharacter(0, PLAYERID::VALUE(SecretChar + i));
        CGameData::PlayParam().PlayerContext(0).m_iShurikenNum = 30;
    };
    CGameData::PlayParam().SetStartArea(idArea, StageNo);
    CGameData::Record().Item().ForceSetCrystalNum(GAMETYPES::CRYSTAL_MAX);
    CGameData::Record().Item().SetItemTaken(ITEMID::ID_DON_LASER);
    CGameData::Option().Display().SetEnableFontEffect(true);
    CGameData::Option().Play().SetDifficulty(GAMETYPES::DIFFICULTY_HARD);

    uint32 callAreaFlag = CallAreaParamGetFlag(AreaParam);

    if (callAreaFlag & CALLAREA_FLAG_STAGE_SERIES)
        CGameData::PlayParam().SetAreaPlaymode(CGamePlayParam::AREAPLAYMODE_SERIES);
    else
        CGameData::PlayParam().SetAreaPlaymode(CGamePlayParam::AREAPLAYMODE_ONE_STAGE);

    if (callAreaFlag & CALLAREA_FLAG_GMKGEN_DISABLE)
        CGimmickDebug::DISABLE_GENERERATOR = true;
    else
        CGimmickDebug::DISABLE_GENERERATOR = false;

    bool bSkipIntroOutroMovies = true;
    s_pDebugMainSeq->Call(PROCLABEL_SEQ_AREAPLAY, reinterpret_cast<const void*>(bSkipIntroOutroMovies));
};


//
//	Calls text print sequence
// 	Values for param:
// 		0 - system text
//      1 - game text
//
static void CallPrintText(void* param)
{
    s_pDebugMainSeq->Call(PROCLABEL_SEQ_MAKETEXT, param, true);
};


/*
 *  All tests (except enemy) runs in singleplayer mode so we need to have keyboard as main controller (first)
 */
static void LockControllersWithKeyboardPriority(void)
{
    UnlockAllControllers();

    int32 controllerNum = CController::Max();
    for (int32 i = 0; i < controllerNum; ++i)
    {
        if (ControllerIsKeyboard(i))
        {
            CController::Lock(i);
            break;
        };
    };
};


/*static*/ CProcess* CDebugMainSequence::Instance(void)
{
    return new CDebugMainSequence;
};


CDebugMainSequence::CDebugMainSequence(void)
: m_menu()
{
    ;
};


CDebugMainSequence::~CDebugMainSequence(void)
{
    ;
};


bool CDebugMainSequence::OnAttach(const void* pParam)
{
    CDebugMenuProcess::Initialize(this);
    
    LockControllersWithKeyboardPriority();
    CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_LOCKED_ON_VIRTUAL);

    CScreenFade::BlackIn(0.0f);
    
    s_pDebugMainSeq = this;

    m_menu.SetDispMax(20);
    m_menu.SetPos(100, 100);
    m_menu.AddTrigger("Test movie",             CallSeq,        PROCLABEL_SEQ_TESTMV);
    m_menu.AddTrigger("Test sound",             CallSeq,        PROCLABEL_SEQ_TESTSD);
    m_menu.AddTrigger("Test pad",               CallSeq,        PROCLABEL_SEQ_TESTPAD);
    m_menu.AddTrigger("Test enemy",             CallSeq,        PROCLABEL_SEQ_ENEMYTESTSEL);
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
            s_pDebugMainSeq->Call(PROCLABEL_SEQ_AREA, (const void*)idAreaOfWorld[nValue]);
    });
    m_menu.AddSeparator();
//    m_menu.AddBool("ENABLE player AI", s_bNormalGamePAI, [](bool value, bool trigger) {
//        if (value != s_bNormalGamePAI)
//        {
//            s_bNormalGamePAI = value;
//            if (s_bNormalGamePAI)
//                EnemyTestPlayerAILaunch(s_pDebugMainSeq);
//            else
//                EnemyTestPlayerAIKill(s_pDebugMainSeq);
//        };
//    });
    m_menu.AddTrigger("ultimate ninja nexus",     CallArea,       CallAreaParamMake(AREAID::ID_AREA60_D, 4, 0)); // tourney
    m_menu.AddTrigger("ultimate ninja bridge p",  CallArea,       CallAreaParamMake(AREAID::ID_AREA44, 1, 0)); // bridge pillar
    m_menu.AddTrigger("ultimate ninja bridge",    CallArea,       CallAreaParamMake(AREAID::ID_AREA44, 0, CALLAREA_FLAG_STAGE_SERIES)); // bridge
    m_menu.AddTrigger("ultimate ninja park",      CallArea,       CallAreaParamMake(AREAID::ID_AREA43, 0, 0)); // park
    m_menu.AddTrigger("shredder ultimate nexus",  CallArea,       CallAreaParamMake(AREAID::ID_AREA60_C, 9, 0)); // tourney
    m_menu.AddTrigger("shredder ultimate",        CallArea,       CallAreaParamMake(AREAID::ID_AREA58, 0, CALLAREA_FLAG_STAGE_SERIES)); // deck
    m_menu.AddTrigger("shredder japan",           CallArea,       CallAreaParamMake(AREAID::ID_AREA38, 0, 0)); // cliff
    m_menu.AddTrigger("shredder",                 CallArea,       CallAreaParamMake(AREAID::ID_AREA40, 0, 0)); // transmat
    m_menu.AddTrigger("traximus",                 CallArea,       CallAreaParamMake(AREAID::ID_AREA30, 1, 0)); // prison
    //m_menu.AddTrigger("usagi",                    CallArea,       CallAreaParamMake(AREAID::ID_AREA60_D, 8, 0)); // tourney
    //m_menu.AddTrigger("foot mech splinter 1",     CallArea,       CallAreaParamMake(AREAID::ID_AREA47, 1, 0)); // sewers
    //m_menu.AddTrigger("foot mech splinter 2",     CallArea,       CallAreaParamMake(AREAID::ID_AREA60_C, 7, 0)); // tourney
    //m_menu.AddTrigger("splinter",                 CallArea,       CallAreaParamMake(AREAID::ID_AREA60_D, 6, 0)); // tourney
    //m_menu.AddTrigger("elite guard A",            CallArea,       CallAreaParamMake(AREAID::ID_AREA52, 1, 0)); // docks
    //m_menu.AddTrigger("elite guard B",            CallArea,       CallAreaParamMake(AREAID::ID_AREA53, 1, 0)); // storage
    //m_menu.AddTrigger("elite guard C",            CallArea,       CallAreaParamMake(AREAID::ID_AREA54, 1, 0)); // lab
    //m_menu.AddTrigger("elite guard D",            CallArea,       CallAreaParamMake(AREAID::ID_AREA55, 1, 0)); // engine room
    //m_menu.AddTrigger("leatherhead test 1",     CallArea,       CallAreaParamMake(AREAID::ID_AREA42, 0, 0)); // turtles old lair
    //m_menu.AddTrigger("leatherhead test 2",     CallArea,       CallAreaParamMake(AREAID::ID_AREA60_B, 9, 0)); // tourney
    //m_menu.AddTrigger("hun 1 test",             CallArea,       CallAreaParamMake(AREAID::ID_AREA02, 1, 0)); // palace
    //m_menu.AddTrigger("hun 2 test",             CallArea,       CallAreaParamMake(AREAID::ID_AREA56, 0, 0)); // weapon depot
    //m_menu.AddTrigger("hun 3 test",             CallArea,       CallAreaParamMake(AREAID::ID_AREA60_C, 5, 0)); // tourney
    //m_menu.AddTrigger("rats 1 test",            CallArea,       CallAreaParamMake(AREAID::ID_AREA18, 1, 0)); // sewers
    //m_menu.AddTrigger("rats 2 test",            CallArea,       CallAreaParamMake(AREAID::ID_AREA60_D, 3, 0)); // tourney
    //m_menu.AddTrigger("spasmosaur test",        CallArea,       AREAID::ID_AREA31); // tri arena
    //m_menu.AddTrigger("drako test",             CallArea,       CallAreaParamMake(AREAID::ID_AREA60_D, 9, 0));
    //m_menu.AddTrigger("slashuur 1 test",        CallArea,       CallAreaParamMake(AREAID::ID_AREA20, 1, 0));    // factory
    //m_menu.AddTrigger("slashuur 2 test",        CallArea,       AREAID::ID_AREA50); // air
    //m_menu.AddTrigger("slashuur 3 test",        CallArea,       CallAreaParamMake(AREAID::ID_AREA60_D, 7, 0)); // tourney
    //m_menu.AddTrigger("karai 1 test",           CallArea,       AREAID::ID_AREA57); // ship
    //m_menu.AddTrigger("karai 2 test",           CallArea,       CallAreaParamMake(AREAID::ID_AREA60_C, 8, 0)); // tourney
    //m_menu.AddTrigger("SNOW ride test",         CallArea,       AREAID::ID_AREA13);
    //m_menu.AddTrigger("TRICERATOR ride test",   CallArea,       AREAID::ID_AREA32);
    //m_menu.AddTrigger("japan shred camera test",CallArea,       AREAID::ID_AREA38);
    //m_menu.AddTrigger("RAIN test",              CallArea,       AREAID::ID_AREA49);
    m_menu.AddTrigger("switch pillar test",     CallArea,       AREAID::ID_AREA04);
    m_menu.AddTrigger("gs machine test",        CallArea,       AREAID::ID_AREA17);
    m_menu.AddTrigger("fall rubble test",       CallArea,       CallAreaParamMake(AREAID::ID_AREA58, 1, CALLAREA_FLAG_GMKGEN_DISABLE));
    m_menu.AddTrigger("space ship SE test",     CallArea,       AREAID::ID_AREA28);
    //m_menu.AddTrigger("trailer chase test",     CallArea,       AREAID::ID_AREA43);
    //m_menu.AddTrigger("fugitoid demo",          CallArea,       AREAID::ID_AREA15);
    //m_menu.AddTrigger("fps drop test",          CallArea,       AREAID::ID_AREA54);
    //m_menu.AddTrigger("demo sync test",         CallSeq,        PROCLABEL_SEQ_PLAYDEMO);
    //m_menu.AddTrigger("roof",                   CallArea,       AREAID::ID_AREA02);
    m_menu.AddSeparator();
    m_menu.AddTrigger("Start NORMAL game!", CallSeq, 0);
    m_menu.AddTrigger("Start UNLOCK game!", CallSeq, -1);
    m_menu.AddTrigger("Exit",               CallSeq, -2);
 
    return true;
};


void CDebugMainSequence::OnDetach(void)
{
    s_pDebugMainSeq = nullptr;

    CScreenFade::BlackOut(0.0f);

    UnlockAllControllers();

    Math::SRand(123456);
};


void CDebugMainSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
        CScreenFade::BlackIn(0.0f);

    m_menu.Period();
};


void CDebugMainSequence::OnDraw(void) const
{
    m_menu.Draw();
};

#endif /* _DEBUG */