#include "ProcessList.hpp"

#include "Game/Sequence/MainSequence.hpp"
#include "Game/Sequence/GameMainSequence.hpp"
#include "Game/Sequence/MovieSequence.hpp"
#include "Game/Sequence/Menu/LogoSequence.hpp"
#include "Game/Sequence/Menu/TitleSequence.hpp"
#include "Game/Sequence/Menu/CharacterSelectSequence.hpp"
#include "Game/Sequence/Menu/UnlockSequence.hpp"
#include "Game/Sequence/SaveLoad/SaveLoadAutoSequence.hpp"
#include "Game/Sequence/SaveLoad/SaveLoadCheckSequence.hpp"
#include "Game/Sequence/SaveLoad/SaveLoadMenuLoadSequence.hpp"
#include "Game/Sequence/SaveLoad/SaveLoadMenuSaveSequence.hpp"
#include "Game/Sequence/Home/HomeStageSequence.hpp"
#include "Game/Sequence/Home/HomeSequence.hpp"
#include "Game/Sequence/Area/AreaSequence.hpp"
#include "Game/Sequence/Area/AreaPlaySequence.hpp"
#include "Game/Sequence/Stage/NormalStageSequence.hpp"
#include "Game/Sequence/Stage/PlayDemoSequence.hpp"
#include "Game/Sequence/Ride/RideStageSequence.hpp"
#include "Game/Sequence/Nexus/NexusMenuSequence.hpp"
#include "Game/Sequence/Nexus/NexusStageSequence.hpp"
#include "Game/Sequence/Nexus/NexusRetrySequence.hpp"
#include "Game/Sequence/Result/ResultSequence.hpp"
#include "Game/Sequence/Result/EnbuSequence.hpp"
#include "Game/Sequence/Result/EndingSequence.hpp"
#include "Game/Sequence/Result/StaffRollSequence.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"


const PROCESSTYPES::PROCESS g_aProcessList[] =
{
    //
    //  Sequence
    //
    { "MainSeq",                PROCESSTYPES::LABEL_SEQ_MAIN,               PROCESSTYPES::PRIORITY_MEDIUM, CMainSequence::Instance,             },
    { "GameMainSeq",            PROCESSTYPES::LABEL_SEQ_GAMEMAIN,           PROCESSTYPES::PRIORITY_MEDIUM, CGameMainSequence::Instance,         },
    { "MovieSeq",               PROCESSTYPES::LABEL_SEQ_MOVIE,              PROCESSTYPES::PRIORITY_MEDIUM, CMovieSequence::Instance,            },
    { "LogoSeq",                PROCESSTYPES::LABEL_SEQ_LOGO,               PROCESSTYPES::PRIORITY_MEDIUM, CLogoSequence::Instance,             },
    { "TitleSeq",               PROCESSTYPES::LABEL_SEQ_TITLE,              PROCESSTYPES::PRIORITY_MEDIUM, CTitleSequence::Instance,            },
    { "SaveLoadAutoSeq",        PROCESSTYPES::LABEL_SEQ_SAVELOADAUTO,       PROCESSTYPES::PRIORITY_MEDIUM, CSaveLoadAutoSequence::Instance      },
    { "SaveLoadCheckSeq",       PROCESSTYPES::LABEL_SEQ_SAVELOADCHECK,      PROCESSTYPES::PRIORITY_MEDIUM, CSaveLoadCheckSequence::Instance,    },
    { "SaveLoadMenuSaveSeq",    PROCESSTYPES::LABEL_SEQ_SAVELOADMENUSAVE,   PROCESSTYPES::PRIORITY_MEDIUM, CSaveLoadMenuSaveSequence::Instance  },
    { "SaveLoadMenuLoadSeq",    PROCESSTYPES::LABEL_SEQ_SAVELOADMENULOAD,   PROCESSTYPES::PRIORITY_MEDIUM, CSaveLoadMenuLoadSequence::Instance  },
    { "CharacterSelectSeq",		PROCESSTYPES::LABEL_SEQ_CHARSELECT,			PROCESSTYPES::PRIORITY_MEDIUM, CCharacterSelectSequence::Instance   },
    { "UnlockSeq",		        PROCESSTYPES::LABEL_SEQ_UNLOCK,			    PROCESSTYPES::PRIORITY_MEDIUM, CUnlockSequence::Instance            },
    { "AreaSeq",                PROCESSTYPES::LABEL_SEQ_AREA,               PROCESSTYPES::PRIORITY_MEDIUM, CAreaSequence::Instance              },
    { "AreaPlaySeq",            PROCESSTYPES::LABEL_SEQ_AREAPLAY,           PROCESSTYPES::PRIORITY_MEDIUM, CAreaPlaySequence::Instance          },
    { "StageNormalSeq",         PROCESSTYPES::LABEL_SEQ_STAGENORMAL,        PROCESSTYPES::PRIORITY_MEDIUM, CNormalStageSequence::Instance       },
    { "StageNexusSeq",          PROCESSTYPES::LABEL_SEQ_STAGENEXUS,         PROCESSTYPES::PRIORITY_MEDIUM, CNexusStageSequence::Instance        },
    { "StageRideSeq",           PROCESSTYPES::LABEL_SEQ_STAGERIDE,          PROCESSTYPES::PRIORITY_MEDIUM, CRideStageSequence::Instance         },
    { "StageDemoSeq",           PROCESSTYPES::LABEL_SEQ_STAGEDEMO,          PROCESSTYPES::PRIORITY_MEDIUM, CPlayDemoSequence::Instance          },
    { "StageVSSeq",             PROCESSTYPES::LABEL_SEQ_STAGEVS,            PROCESSTYPES::PRIORITY_MEDIUM, nullptr                              },
    { "ResultSeq",              PROCESSTYPES::LABEL_SEQ_RESULT,             PROCESSTYPES::PRIORITY_MEDIUM, CResultSequence::Instance            },
    { "EnbuSeq",                PROCESSTYPES::LABEL_SEQ_ENBU,               PROCESSTYPES::PRIORITY_MEDIUM, CEnbuSequence::Instance              },
    { "HomeSeq",                PROCESSTYPES::LABEL_SEQ_HOME,               PROCESSTYPES::PRIORITY_MEDIUM, CHomeSequence::Instance              },
    { "HomeStageSeq",           PROCESSTYPES::LABEL_SEQ_HOMESTAGE,          PROCESSTYPES::PRIORITY_MEDIUM, CHomeStageSequence::Instance         },
    { "AprilSeq",               PROCESSTYPES::LABEL_SEQ_ANTIQUESHOP,        PROCESSTYPES::PRIORITY_MEDIUM, nullptr                              },
    { "DatabaseSeq",            PROCESSTYPES::LABEL_SEQ_DATABASE,           PROCESSTYPES::PRIORITY_MEDIUM, nullptr                              },
    { "NexusMenuSeq",           PROCESSTYPES::LABEL_SEQ_NEXUSMENU,          PROCESSTYPES::PRIORITY_MEDIUM, CNexusMenuSequence::Instance         },
    { "NexusRetrySeq",          PROCESSTYPES::LABEL_SEQ_NEXUSRETRY,         PROCESSTYPES::PRIORITY_MEDIUM, CNexusRetrySequence::Instance        },
    { "EndingSeq",              PROCESSTYPES::LABEL_SEQ_ENDING,             PROCESSTYPES::PRIORITY_MEDIUM, CEndingSequence::Instance            },
    { "StaffRollSeq",           PROCESSTYPES::LABEL_SEQ_STAFFROLL,          PROCESSTYPES::PRIORITY_MEDIUM, CStaffRollSequence::Instance         },

    //
    //  Process
    //
    { "ScreenFade",     PROCESSTYPES::LABEL_SCREENFADE,         PROCESSTYPES::PRIORITY_LOW,     CScreenFade::Instance,          },
    { "LoadingDisp",    PROCESSTYPES::LABEL_LOADINGDISPLAY,     PROCESSTYPES::PRIORITY_LOW,     CLoadingDisplay::Instance,      },
    { "Timeout",        PROCESSTYPES::LABEL_TIMEOUT,            PROCESSTYPES::PRIORITY_MEDIUM,  nullptr                         },
    { "PadConnCheck",   PROCESSTYPES::LABEL_PADCONNECTCHECK,    PROCESSTYPES::PRIORITY_MEDIUM,  nullptr                         },
    { "SoftwareReset",  PROCESSTYPES::LABEL_SOFTWARERESET,      PROCESSTYPES::PRIORITY_LOW,     CSoftwareReset::Instance        },

    //
    //  List end
    //
    { "EOL", PROCESSTYPES::LABEL_EOL, PROCESSTYPES::PRIORITY_MEDIUM, nullptr, },
};


static_assert(COUNT_OF(g_aProcessList) - 1 == PROCESSTYPES::LABEL_MAX, "update me");
