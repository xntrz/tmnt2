#include "EnemyTestMakeFileSequence.hpp"

#ifdef _DEBUG

#include "Game/ProcessList.hpp"
#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Component/Effect/EffectID.hpp"
#include "Game/Sequence/Test/MakeFileSequence.hpp"
#include "Game/Sequence/Test/DebugUtils.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/File/FileID.hpp"

#ifdef TARGET_PC
#include "System/PC/PCDebug.hpp"
#endif /* TARGET_PC */

#include <queue>
#include <map>


class CEnemyTestMakeFileSequence final : public CSequence
{
public:
    union MAKEFILE
    {
        enum TYPE
        {
            TYPE_ENEMY = 0,
            TYPE_ENEMY_ALL,
            TYPE_EFFECT_ALL,
            TYPE_ITEM,
            TYPE_CUSTOM_MAKE_ENEMY_TABLE,
            TYPE_CUSTOM_MAKE_EFFECT_TABLE,
        };

        struct
        {
            uint32 enemyId : 16;
            uint32 type    : 16;
        };
        uint32 value;
    };

    static_assert(sizeof(MAKEFILE) == sizeof(uint32), "size should equal");

private:    
    struct REQUEST
    {
        enum TYPE
        {
            TYPE_PRINT = 0,
            TYPE_EXTRACT,
            TYPE_ENUM,
        };

        TYPE        type;
        int32       fileId;
        const char* filename;
    };

public:
    static void Call(CSequence* pCaller, MAKEFILE makefile);
    static void LpacEnumCallback(int32 fileId, uint32 fileSize, const char* pszFilename, void* param);

    CEnemyTestMakeFileSequence(void);
    virtual ~CEnemyTestMakeFileSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void OnEnumerate(int32 fileId, const char* pszFilename);
    bool UpdateQueue(void);
    void Extract(int32 fileId, const char* filename);
    void Print(int32 fileId);
    void Enum(int32 fileId);
    void ExtractEnemy(int32 fileIdStart, int32 fileIdEnd);

private:
    static bool                  m_bBusy;
    MAKEFILE                     m_makefile;
    std::queue<REQUEST>          m_queueMakeFile;
    CDebugMenuCtrl               m_menu;
    int32                        m_progressNow;
    int32                        m_progressMax;
    char                         m_szProgressText[128];
    std::map<std::string, int32> m_mapNameToFile;
};


/*static*/ bool CEnemyTestMakeFileSequence::m_bBusy = false;



/*static*/ void CEnemyTestMakeFileSequence::Call(CSequence* pCaller, MAKEFILE makefile)
{
    ASSERT(m_bBusy == false);
    m_bBusy = true;

    pCaller->Call(PROCLABEL_SEQ_ENEMYMAKEFILE, reinterpret_cast<const void*>(makefile.value));
};


/*static*/ void CEnemyTestMakeFileSequence::LpacEnumCallback(int32 fileId, uint32 fileSize, const char* pszFilename, void* param)
{
    CEnemyTestMakeFileSequence* pSequence = static_cast<CEnemyTestMakeFileSequence*>(param);
    ASSERT(pSequence);

    pSequence->OnEnumerate(fileId, pszFilename);
};


CEnemyTestMakeFileSequence::CEnemyTestMakeFileSequence(void)
: m_queueMakeFile()
, m_menu()
, m_progressNow(0)
, m_progressMax(0)
, m_szProgressText()
{
    m_szProgressText[0] = '\0';
};


CEnemyTestMakeFileSequence::~CEnemyTestMakeFileSequence(void)
{
    ;
};


bool CEnemyTestMakeFileSequence::OnAttach(const void* pParam)
{
    uint32 param = reinterpret_cast<uint32>(pParam);

    MAKEFILE makefile;
    makefile.value = param;

    m_makefile = makefile;

    switch (makefile.type)
    {
    case MAKEFILE::TYPE_ENEMY:
        {
            ENEMYID::VALUE enemyId = static_cast<ENEMYID::VALUE>(makefile.enemyId);

            int32 fileid = EnemyTestGetFileId(enemyId);
            if (fileid == -1)
                break;

            const char* filename = EnemyTestGetFilename(enemyId);
            if (!filename)
                break;

            Extract(fileid, filename);
        }
        break;

    case MAKEFILE::TYPE_ENEMY_ALL:
        {
            for (int32 i = ENEMYID::ID_START; i < ENEMYID::ID_MAX; ++i)
            {
                ENEMYID::VALUE enemyId = static_cast<ENEMYID::VALUE>(i);

                int32 fileid = EnemyTestGetFileId(enemyId);
                if (fileid == -1)
                    continue;
                
                const char* filename = EnemyTestGetFilename(enemyId);
                if (!filename)
                    continue;

                Extract(fileid, filename);
            };
        }
        break;

    case MAKEFILE::TYPE_EFFECT_ALL:
        {
            for (int32 i = 0; i < EFFECTID::ID_MAX; ++i)
            {
                EFFECTID::VALUE effectId = static_cast<EFFECTID::VALUE>(i);

                int32 fileid = EnemyTestGetEffectFileId(effectId);
                if (fileid == -1)
                    continue;

                const char* filename = EnemyTestGetEffectFilename(effectId);
                if (!filename)
                    continue;

                Extract(fileid, filename);
            };
        }
        break;

    case MAKEFILE::TYPE_ITEM:
        {
            int32 fileid = FILEID::ID_ST01T;

            int32 i = 0;

            const char* pszFilename = EnemyTestGetItemNeededFile(i);
            while (pszFilename)
            {
                Extract(fileid, pszFilename);
                pszFilename = EnemyTestGetItemNeededFile(++i);
            };
        }
        break;

    case MAKEFILE::TYPE_CUSTOM_MAKE_ENEMY_TABLE:
        {
            for (int32 i = FILEID::ID_ST60X_D10; i >= FILEID::ID_ST01T; --i)
                Enum(i);
        }
        break;

    case MAKEFILE::TYPE_CUSTOM_MAKE_EFFECT_TABLE:
        {
            for (int32 i = FILEID::ID_STCOMMON; i <= FILEID::ID_SPLINTER_SHIP; ++i)
                Enum(i);
        }
        break;

    default:
        ASSERT(false, "unknown makefile type %" PRIu32, makefile.type);
        break;
    };

    m_menu.AddTextDisp(m_szProgressText);
    m_menu.AddTrigger("EXIT", nullptr);

    return true;
};


void CEnemyTestMakeFileSequence::OnDetach(void)
{
    m_bBusy = false;

#ifdef TARGET_PC
    CPCDebug::SetDispTimeFileLine(false);
#endif /* TARGET_PC */

    OUTPUT("\n\n");

    switch (m_makefile.type)
    {
    case MAKEFILE::TYPE_CUSTOM_MAKE_ENEMY_TABLE:
        {
            for (auto& it : m_mapNameToFile)
            {
                int32 enemyId = -1;
                std::sscanf(it.first.c_str(), "enemy%03" SCNd32, &enemyId);

                const char* pszName = ENEMYID::GetName(static_cast<ENEMYID::VALUE>(enemyId));
                ASSERT(pszName);

                OUTPUT("{ %s, \"%s\", %" PRIi32 " },\n", pszName, it.first.c_str(), it.second);
            };
        }
        break;

    case MAKEFILE::TYPE_CUSTOM_MAKE_EFFECT_TABLE:
        {
            for (auto& it : m_mapNameToFile)
                OUTPUT("{ \"%s\", %" PRIi32 " },\n", it.first.c_str(), it.second);            
        }
        break;

    default:
        break;
    };

    OUTPUT("\n\n");

#ifdef TARGET_PC
    CPCDebug::SetDispTimeFileLine(true);
#endif /* TARGET_PC */
};


void CEnemyTestMakeFileSequence::OnMove(bool bRet, const void* pReturnValue)
{
    std::sprintf(m_szProgressText,
                 "Progress: %" PRIi32 " / %" PRIi32,
                 m_progressNow, m_progressMax);

    m_menu.Period();

    if (!UpdateQueue())
        Ret();
};


void CEnemyTestMakeFileSequence::OnDraw(void) const
{
    m_menu.Draw();
};


void CEnemyTestMakeFileSequence::OnEnumerate(int32 fileId, const char* pszFilename)
{
    bool bAddToTable = false;

    switch (m_makefile.type)
    {
    case MAKEFILE::TYPE_CUSTOM_MAKE_ENEMY_TABLE:
        {
            if (std::strstr(pszFilename, "enemy"))
                bAddToTable = true;
        }
        break;

    case MAKEFILE::TYPE_CUSTOM_MAKE_EFFECT_TABLE:
        {
            EFFECTID::VALUE effectId = EFFECTID::GetIDFromName(pszFilename);
            if (effectId != EFFECTID::ID_UNKNOWN)
                bAddToTable = true;
        }
        break;

    default:
        break;
    };

    if (bAddToTable)
    {
        auto it = m_mapNameToFile.find(pszFilename);
        auto itEnd = m_mapNameToFile.end();
        if (it == itEnd)
            m_mapNameToFile.insert({ pszFilename, fileId });
    };
};


bool CEnemyTestMakeFileSequence::UpdateQueue(void)
{
    if (m_queueMakeFile.empty())
        return false;
    
    const REQUEST& request = m_queueMakeFile.front();

    switch (request.type)
    {
    case REQUEST::TYPE_PRINT:
        PrintLPACFile(this, request.fileId);
        break;

    case REQUEST::TYPE_EXTRACT:
        ExtractLPACFile(this, request.fileId, request.filename);
        break;

    case REQUEST::TYPE_ENUM:
        EnumLPACFile(this, request.fileId, &CEnemyTestMakeFileSequence::LpacEnumCallback, this);
        break;

    default:
        ASSERT(false);
        break;
    };

    m_queueMakeFile.pop();

    ++m_progressNow;

    return true;
};


void CEnemyTestMakeFileSequence::Extract(int32 fileId, const char* filename)
{
    m_queueMakeFile.push({ REQUEST::TYPE_EXTRACT, fileId, filename });
    ++m_progressMax;
};


void CEnemyTestMakeFileSequence::Print(int32 fileId)
{
    m_queueMakeFile.push({ REQUEST::TYPE_PRINT, fileId });
    ++m_progressMax;
};


void CEnemyTestMakeFileSequence::Enum(int32 fileId)
{
    m_queueMakeFile.push({ REQUEST::TYPE_ENUM, fileId });
    ++m_progressMax;
};


void CEnemyTestMakeFileSequence::ExtractEnemy(int32 fileIdStart, int32 fileIdEnd)
{
    for (int32 i = fileIdStart; i <= fileIdEnd; ++i)
    {
        for (int32 j = 0; j < ENEMYID::ID_MAX; ++j)
        {
            const char* pszExtName = ENEMYID::GetExtName(static_cast<ENEMYID::VALUE>(j));
            if (!pszExtName)
                continue;

            Extract(i, pszExtName);
        };
    };
};


struct ENEMYTEST_LOAD_INFO
{
    ENEMYID::VALUE  id;
    const char*     filename;
    int32           fileid;
};


struct ENEMYTEST_EFFECT_INFO
{
    const char*     filename;
    int32           fileid;
};


static const ENEMYTEST_LOAD_INFO s_aEnemyTestLoadInfo[] =
{
    //-----------------------------------------------------------------------
    //| enemyId                                     fileName        fileId  |
    //-----------------------------------------------------------------------
    {   ENEMYID::ID_PURPLE_DRAGON_GANG,             "enemy001",     98      },
    {   ENEMYID::ID_FOOT_NINJA_SWORD,               "enemy002",     90      },
    {   ENEMYID::ID_FOOT_NINJA_STAFF,               "enemy003",     90      },
    {   ENEMYID::ID_FEUDAL_FOOT_NINJA_SWORD,        "enemy004",     62      },
    {   ENEMYID::ID_FEUDAL_FOOT_NINJA_STAFF,        "enemy005",     61      },
    {   ENEMYID::ID_UTROMS_SECURITY_PATROL,         "enemy006",     94      },
    {   ENEMYID::ID_FEDERATION_SOLDIER,             "enemy007",     96      },
    {   ENEMYID::ID_MOBSTER,                        "enemy010",     98      },
    {   ENEMYID::ID_FOOT_NINJA_ARCHER,              "enemy011",     115     },
    {   ENEMYID::ID_FEUDAL_FOOT_NINJA_ARCHER,       "enemy012",     62      },
    {   ENEMYID::ID_FEDERATION_SHOOTER,             "enemy013",     96      },
    {   ENEMYID::ID_EXO_SUITED_UTROMS,              "enemy014",     94      },
    {   ENEMYID::ID_CAPTAIN_OF_FEDERATION,          "enemy015",     99      },
    {   ENEMYID::ID_LARGE_MOBSTER,                  "enemy020",     98      },
    {   ENEMYID::ID_LARGE_FOOT_NINJA,               "enemy021",     76      },
    {   ENEMYID::ID_FEUDAL_LARGE_FOOT_NINJA,        "enemy022",     62      },
    {   ENEMYID::ID_LARGE_FEDERATION_SOLDIET,       "enemy023",     97      },
    {   ENEMYID::ID_LARGE_EXO_SUITED_UTROMS,        "enemy024",     97      },
    {   ENEMYID::ID_FOOT_TECH_NINJA,                "enemy030",     118     },
    {   ENEMYID::ID_FOOT_SUMO_NINJA,                "enemy031",     117     },
    {   ENEMYID::ID_FOOT_GUNNER_FOOT_MECH,          "enemy032",     121     },
    {   ENEMYID::ID_CAPTAIN_OF_FEUDAL_FOOT_NINJA,   "enemy033",     123     },
    {   ENEMYID::ID_TRICERATION_SOLDIER,            "enemy035",     53      },
    {   ENEMYID::ID_TRICERATION_SHOOTER,            "enemy036",     122     },
    {   ENEMYID::ID_TRICERATION_FLYING_HARNESS,     "enemy037",     51      },
    {   ENEMYID::ID_STONE_BITER,                    "enemy040",     120     },
    {   ENEMYID::ID_BERGOX,                         "enemy041",     105     },
    {   ENEMYID::ID_SPIDER_MONSTER,                 "enemy042",     104     },
    {   ENEMYID::ID_REDURION,                       "enemy043",     108     },
    {   ENEMYID::ID_KING_NAIL,                      "enemy044",     120     },
    {   ENEMYID::ID_GREENPUS,                       "enemy045",     108     },
    {   ENEMYID::ID_RAZORFIST,                      "enemy046",     120     },
    {   ENEMYID::ID_MAD_MUNCHER,                    "enemy047",     108     },
    {   ENEMYID::ID_RYNOKK,                         "enemy048",     108     },
    {   ENEMYID::ID_BLOOD_SUCKER,                   "enemy050",     92      },
    {   ENEMYID::ID_POISON_BAT,                     "enemy051",     124     },
    {   ENEMYID::ID_HYPNOSIS_BAT,                   "enemy052",     124     },
    {   ENEMYID::ID_BOMB_BAT_MECH,                  "enemy053",     124     },
    {   ENEMYID::ID_KURAGE,                         "enemy055",     82      },
    {   ENEMYID::ID_KABUTO,                         "enemy056",     84      },
    {   ENEMYID::ID_UTROM_SAUCER,                   "enemy057",     33      },
    {   ENEMYID::ID_MOUSER_ROBOT,                   "enemy060",     91      },
    {   ENEMYID::ID_MOUSER_ROBOT_B,                 "enemy061",     92      },
    {   ENEMYID::ID_KROKODIL_MOUSER,                "enemy062",     125     },
    {   ENEMYID::ID_RAPTOR,                         "enemy063",     102     },
    {   ENEMYID::ID_CARNIVOROUS_PLANT,              "enemy065",     106     },
    {   ENEMYID::ID_MURAL_CARNIVOROUS_PLANT,        "enemy066",     47      },
    {   ENEMYID::ID_NINJA_RATS_A,                   "enemy070",     123     },
    {   ENEMYID::ID_NINJA_RATS_B,                   "enemy071",     123     },
    {   ENEMYID::ID_NINJA_RATS_C,                   "enemy072",     123     },
    {   ENEMYID::ID_NINJA_RATS_D,                   "enemy073",     123     },
    {   ENEMYID::ID_TRAXIMUS,                       "enemy074",     122     },
    {   ENEMYID::ID_LEATHER_HEAD,                   "enemy075",     109     },
    {   ENEMYID::ID_ELITE_FOOT_A,                   "enemy076",     116     },
    {   ENEMYID::ID_ELITE_FOOT_B,                   "enemy077",     116     },
    {   ENEMYID::ID_ELITE_FOOT_C,                   "enemy078",     113     },
    {   ENEMYID::ID_ELITE_FOOT_D,                   "enemy079",     111     },
    {   ENEMYID::ID_ULTIMATE_NINJA,                 "enemy080",     124     },
    {   ENEMYID::ID_SPASMOSAUR,                     "enemy081",     56      },
    {   ENEMYID::ID_HUN_A,                          "enemy082",     25      },
    {   ENEMYID::ID_HUN_B,                          "enemy083",     115     },
    {   ENEMYID::ID_KARAI,                          "enemy084",     118     },
    {   ENEMYID::ID_MIYAMOTO_USAGI,                 "enemy085",     128     },
    {   ENEMYID::ID_FOOT_MECH_SPLINTER,             "enemy086",     117     },
    {   ENEMYID::ID_SLASSHUR,                       "enemy087",     127     },
    {   ENEMYID::ID_SHREDDER,                       "enemy088",     65      },
    {   ENEMYID::ID_FEUDAL_JAPAN_SHREDDER,          "enemy089",     63      },
    {   ENEMYID::ID_ULTIMATE_SHREDDER,              "enemy090",     119     },
    {   ENEMYID::ID_DORAKO,                         "enemy091",     129     },
    {   ENEMYID::ID_ELITE_FOOT_DUMMY_A,             "enemy092",     116     },
    {   ENEMYID::ID_ELITE_FOOT_DUMMY_B,             "enemy093",     116     },
    {   ENEMYID::ID_ELITE_FOOT_DUMMY_C,             "enemy094",     113     },
    {   ENEMYID::ID_ELITE_FOOT_DUMMY_D,             "enemy095",     111     },
    {   ENEMYID::ID_SPLINTER,                       "enemy096",     126     },
    {   ENEMYID::ID_FUGITOID,                       "enemy098",     37      },
};


static const ENEMYTEST_EFFECT_INFO s_aEnemyTestEffectInfo[] =
{
    //-----------------------------------
    //| fileName                fileId  |
    //-----------------------------------
    {   "all_breath",           19      },
    {   "all_count",            19      },
    {   "all_dash1",            19      },
    {   "all_dash2",            19      },
    {   "all_dash_snow",        19      },
    {   "all_downsmoke",        19      },
    {   "all_freezend",         19      },
    {   "all_guard1",           19      },
    {   "all_guard2",           19      },
    {   "all_guardbreak",       19      },
    {   "all_hit_b",            19      },
    {   "all_hit_s",            19      },
    {   "all_jump",             19      },
    {   "all_jumpfail",         19      },
    {   "all_nencut",           19      },
    {   "all_nenshi1",          19      },
    {   "all_pitan",            19      },
    {   "all_piyohit",          19      },
    {   "all_piyori",           19      },
    {   "all_sleep",            19      },
    {   "all_tag",              19      },
    {   "all_tame1",            19      },
    {   "all_tame2",            19      },
    {   "all_tame3",            19      },
    {   "all_tame_flash",       19      },
    {   "all_w_dobon",          19      },
    {   "all_w_hamon",          19      },
    {   "all_w_splash",         19      },
    {   "all_wake",             19      },
    {   "barrier_start",        19      },
    {   "biribiri",             19      },
    {   "blacksmoke",           19      },
    {   "blade_flash",          128     },
    {   "breakice",             27      },
    {   "breaksign",            25      },
    {   "bubble",               41      },
    {   "cas_sp_ball",          19      },
    {   "cas_sp_end",           19      },
    {   "ceramics",             23      },
    {   "don_hit_b",            19      },
    {   "don_hit_s",            19      },
    {   "don_laser_beam",       19      },
    {   "don_laser_fire",       19      },
    {   "don_laser_hit",        19      },
    {   "dragonsmoke",          68      },
    {   "drk_pillar",           129     },
    {   "drkball",              129     },
    {   "drkball_end",          129     },
    {   "drkball_start",        129     },
    {   "dustbox",              66      },
    {   "enginespark",          38      },
    {   "expl_b1",              19      },
    {   "fire_arrow",           23      },
    {   "fire_arrowshot",       23      },
    {   "fire_torch",           23      },
    {   "flame_map",            23      },
    {   "freezegas",            19      },
    {   "grass",                24      },
    {   "gunball",              68      },
    {   "gunball_fire",         68      },
    {   "hpsteal",              43      },
    {   "hpsteal_light",        43      },
    {   "i_cryshine_g",         19      },
    {   "i_cryshine_o",         19      },
    {   "i_cryshine_r",         19      },
    {   "i_cryshine_w",         19      },
    {   "i_cyget_g",            19      },
    {   "i_cyget_o",            19      },
    {   "i_cyget_r",            19      },
    {   "i_cyget_w",            19      },
    {   "i_get",                19      },
    {   "i_getantique",         19      },
    {   "i_recover_b",          19      },
    {   "i_recover_s",          19      },
    {   "kar_sp",               19      },
    {   "ko_expl",              19      },
    {   "ko_flash",             19      },
    {   "ko_smoke",             19      },
    {   "laser_beam_all",       41      },
    {   "laser_beam_fed",       31      },
    {   "laser_beam_tri",       45      },
    {   "laser_fire_all",       41      },
    {   "laser_fire_fed",       31      },
    {   "laser_fire_tri",       45      },
    {   "laser_hit_all",        21      },
    {   "laser_hit_fed",        31      },
    {   "laser_hit_tri",        45      },
    {   "leo_hit_b",            19      },
    {   "leo_hit_s",            19      },
    {   "leo_sp",               19      },
    {   "m12_breakrock",        34      },
    {   "metal",                25      },
    {   "meteorite",            57      },
    {   "mic_hit_b",            19      },
    {   "mic_hit_s",            19      },
    {   "mic_reflect",          19      },
    {   "mic_sp_ball",          19      },
    {   "mic_sp_end",           19      },
    {   "pipe_freezegas",       23      },
    {   "pipe_steam",           23      },
    //{   "rain",                 77      },
    {   "rap_hit_b",            19      },
    {   "rap_hit_s",            19      },
    {   "rap_sp",               19      },
    {   "ride_engine",          21      },
    {   "ride_particle",        21      },
    {   "ride_wind",            21      },
    {   "rock",                 27      },
    {   "rod_beam",             129     },
    {   "rod_beam_hit",         129     },
    {   "rod_flash",            129     },
    {   "shrd_elball",          63      },
    {   "shrd_elec",            23      },
    {   "shrd_elend",           63      },
    {   "sickle_warp",          43      },
    {   "sla_barrier",          43      },
    {   "sla_shadow",           43      },
    {   "sla_sp",               19      },
    {   "slaball",              43      },
    {   "slaball_end",          43      },
    {   "sleep_arrow",          23      },
    {   "sleep_arrowshot",      23      },
    {   "sleepgas",             19      },
    {   "spark_slow",           37      },
    {   "sparks",               27      },
    {   "spidernet",            28      },
    {   "spidersmoke",          28      },
    {   "spraywater",           37      },
    {   "spsm_kabecrash",       47      },
    {   "spsm_runsmoke",        56      },
    {   "spsm_tbshot",          27      },
    {   "spsm_yodar1",          56      },
    {   "spsm_yodar2",          56      },
    {   "stup_attack",          19      },
    {   "stup_charge",          19      },
    {   "stup_deffens",         19      },
    {   "stup_muteki",          19      },
    {   "stup_shrken",          19      },
    {   "sweep_splash",         38      },
    {   "tbshot",               28      },
    {   "tbshot_mark",          28      },
    {   "tengsword",            63      },
    {   "thunder",              88      },
    {   "ulnin_enmaku",         68      },
    {   "warp_out",             43      },
    {   "warp_start",           25      },
    {   "wfall_smoke",          24      },
    {   "wfall_sprash",         24      },
    {   "whitesmoke",           19      },
    {   "wind_press",           25      },
    {   "windwarp_out",         64      },
    {   "windwarp_start",       64      },
    {   "wood",                 23      },
};


static inline const ENEMYTEST_LOAD_INFO* EnemyTestGetLoadInfo(ENEMYID::VALUE enemyId)
{
    for (int32 i = 0; i < COUNT_OF(s_aEnemyTestLoadInfo); ++i)
    {
        if (s_aEnemyTestLoadInfo[i].id == enemyId)
            return &s_aEnemyTestLoadInfo[i];
    };

    return nullptr;
};


static inline const ENEMYTEST_EFFECT_INFO* EnemyTestGetEffectInfo(EFFECTID::VALUE effectId)
{
    for (int32 i = 0; i < COUNT_OF(s_aEnemyTestEffectInfo); ++i)
    {
        if (!std::strcmp(s_aEnemyTestEffectInfo[i].filename, EFFECTID::GetNameFromID(effectId)))
            return &s_aEnemyTestEffectInfo[i];
    };

    return nullptr;
};


void EnemyTestMakeEnemyTable(class CSequence* pCaller)
{
    CEnemyTestMakeFileSequence::MAKEFILE makefile;
    makefile.type = CEnemyTestMakeFileSequence::MAKEFILE::TYPE_CUSTOM_MAKE_ENEMY_TABLE;

    CEnemyTestMakeFileSequence::Call(pCaller, makefile);
};


void EnemyTestMakeEffectTable(class CSequence* pCaller)
{
    CEnemyTestMakeFileSequence::MAKEFILE makefile;
    makefile.type = CEnemyTestMakeFileSequence::MAKEFILE::TYPE_CUSTOM_MAKE_EFFECT_TABLE;

    CEnemyTestMakeFileSequence::Call(pCaller, makefile);
};


void EnemyTestMakeFileEnemy(class CSequence* pCaller, ENEMYID::VALUE enemyId)
{
    CEnemyTestMakeFileSequence::MAKEFILE makefile;
    makefile.type    = CEnemyTestMakeFileSequence::MAKEFILE::TYPE_ENEMY;
    makefile.enemyId = static_cast<uint32>(enemyId);

    CEnemyTestMakeFileSequence::Call(pCaller, makefile);
};


void EnemyTestMakeFileEnemyAll(class CSequence* pCaller)
{
    CEnemyTestMakeFileSequence::MAKEFILE makefile;
    makefile.type = CEnemyTestMakeFileSequence::MAKEFILE::TYPE_ENEMY_ALL;

    CEnemyTestMakeFileSequence::Call(pCaller, makefile);
};


void EnemyTestMakeFileEffectAll(class CSequence* pCaller)
{
    CEnemyTestMakeFileSequence::MAKEFILE makefile;
    makefile.type = CEnemyTestMakeFileSequence::MAKEFILE::TYPE_EFFECT_ALL;

    CEnemyTestMakeFileSequence::Call(pCaller, makefile);
};


void EnemyTestMakeFileItems(class CSequence* pCaller)
{    
    CEnemyTestMakeFileSequence::MAKEFILE makefile;
    makefile.type = CEnemyTestMakeFileSequence::MAKEFILE::TYPE_ITEM;

    CEnemyTestMakeFileSequence::Call(pCaller, makefile);
};


const char* EnemyTestGetItemNeededFile(int32 no /*= -1*/)
{
    static const char* s_apszItemNeededFilename[] =
    {
        "box",
        "ceramics",
        "wood",
    };

    if (no < COUNT_OF(s_apszItemNeededFilename))
        return s_apszItemNeededFilename[no];

    return nullptr;
};


const char* EnemyTestGetFilename(ENEMYID::VALUE enemyId)
{
    const ENEMYTEST_LOAD_INFO* pLoadInfo = EnemyTestGetLoadInfo(enemyId);
    if (pLoadInfo)
        return pLoadInfo->filename;

    return nullptr;
};


int32 EnemyTestGetFileId(ENEMYID::VALUE enemyId)
{
    const ENEMYTEST_LOAD_INFO* pLoadInfo = EnemyTestGetLoadInfo(enemyId);
    if (pLoadInfo)
        return pLoadInfo->fileid;

    return -1;
};


const char* EnemyTestGetEffectFilename(EFFECTID::VALUE effectId)
{
    const ENEMYTEST_EFFECT_INFO* pEffectInfo = EnemyTestGetEffectInfo(effectId);
    if (pEffectInfo)
        return pEffectInfo->filename;

    return nullptr;
};


int32 EnemyTestGetEffectFileId(EFFECTID::VALUE effectId)
{
    const ENEMYTEST_EFFECT_INFO* pEffectInfo = EnemyTestGetEffectInfo(effectId);
    if (pEffectInfo)
        return pEffectInfo->fileid;

    return -1;
};


/*class*/ CProcess* EnemyTestMakeFileSequenceInstance(void)
{
    return new CEnemyTestMakeFileSequence;
};

#endif /* _DEBUG */