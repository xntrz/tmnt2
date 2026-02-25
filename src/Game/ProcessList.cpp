#include "Game/ProcessList.hpp"

#include "Game/Sequence/MovieSequence.hpp"
#include "Game/Sequence/GameMainSequence.hpp"
#include "Game/Sequence/Menu/LogoSequence.hpp"
#include "Game/Sequence/Menu/TitleSequence.hpp"
#include "Game/Sequence/Menu/PreArcade.hpp"
#include "Game/Sequence/Menu/ComingSoon.hpp"
#include "Game/Sequence/Menu/CharacterSelectSequence.hpp"
#include "Game/Sequence/Menu/UnlockSequence.hpp"
#include "Game/Sequence/Menu/AntiqueShopSequence.hpp"
#include "Game/Sequence/Menu/DatabaseSequence.hpp"
#include "Game/Sequence/Menu/Database.hpp"
#include "Game/Sequence/Menu/DatabaseViewer.hpp"
#include "Game/Sequence/Menu/OptionsSequence.hpp"
#include "Game/Sequence/Home/HomeStageSequence.hpp"
#include "Game/Sequence/Home/HomeSequence.hpp"
#include "Game/Sequence/Area/AreaSequence.hpp"
#include "Game/Sequence/Area/AreaPlaySequence.hpp"
#include "Game/Sequence/Stage/NormalStageSequence.hpp"
#include "Game/Sequence/Stage/PlayDemoSequence.hpp"
#include "Game/Sequence/SaveLoad/SaveLoadSequence.hpp"
#include "Game/Sequence/Ride/RideStageSequence.hpp"
#include "Game/Sequence/Nexus/NexusMenuSequence.hpp"
#include "Game/Sequence/Nexus/NexusStageSequence.hpp"
#include "Game/Sequence/Nexus/NexusRetrySequence.hpp"
#include "Game/Sequence/Result/ResultSequence.hpp"
#include "Game/Sequence/Result/EnbuSequence.hpp"
#include "Game/Sequence/Result/EndingSequence.hpp"
#include "Game/Sequence/Result/StaffRollSequence.hpp"

#include "Game/System/Misc/PadConnectCheck.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/Misc/SoftwareReset.hpp"

#ifdef _DEBUG
#include "Game/Sequence/Test/DebugMainSequence.hpp"
#include "Game/Sequence/Test/TestMovieSequence.hpp"
#include "Game/Sequence/Test/TestSoundSequence.hpp"
#include "Game/Sequence/Test/TestPadSequence.hpp"
#include "Game/Sequence/Test/EnemySelTestSequence.hpp"
#include "Game/Sequence/Test/EnemyTestSequence.hpp"
#include "Game/Sequence/Test/DebugMenuProcess.hpp"
#include "Game/Sequence/Test/DbgUnlockProcess.hpp"
#endif


extern const PROCESSTYPES::PROCESS g_aProcessList[] =
{
    { PROCLABEL_SEQ_GAMEMAIN,           PROCESSTYPES::PRIORITY_NORMAL,      CGameMainSequence::Instance         },
#ifdef _DEBUG
    { PROCLABEL_SEQ_DBGMAIN,            PROCESSTYPES::PRIORITY_NORMAL,      CDebugMainSequence::Instance        },
    { PROCLABEL_SEQ_TESTMV,             PROCESSTYPES::PRIORITY_NORMAL,      CTestMovieSequence::Instance        },
    { PROCLABEL_SEQ_TESTSD,             PROCESSTYPES::PRIORITY_NORMAL,      TestSoundSequenceInstance        	},
    { PROCLABEL_SEQ_TESTPAD,            PROCESSTYPES::PRIORITY_NORMAL,      CTestPadSequence::Instance          },
    { PROCLABEL_SEQ_TESTENEMYSEL,       PROCESSTYPES::PRIORITY_NORMAL,      CEnemySelTestSequence::Instance     },
    { PROCLABEL_SEQ_TESTENEMY,          PROCESSTYPES::PRIORITY_NORMAL,      CEnemyTestSequence::Instance        },
    { PROCLABEL_DBGMENU,                PROCESSTYPES::PRIORITY_NORMAL + 1,  CDebugMenuProcess::Instance         },
    { PROCLABEL_DBGUNLOCK,              PROCESSTYPES::PRIORITY_HIGH,        CDbgUnlockProcess::Instance         },
#endif /* _DEBUG */
    { PROCLABEL_SEQ_MOVIE,              PROCESSTYPES::PRIORITY_NORMAL,      CMovieSequence::Instance            },
    { PROCLABEL_SEQ_LOGODISP,           PROCESSTYPES::PRIORITY_NORMAL,      CLogoDisplaySequence::Instance		},
    { PROCLABEL_SEQ_TITLE,              PROCESSTYPES::PRIORITY_NORMAL,      CTitleSequence::Instance            },
#ifdef TMNT2_BUILD_EU
    { PROCLABEL_SEQ_PRE_ARCADE,         PROCESSTYPES::PRIORITY_NORMAL,      CPreArcadeSequence::Instance        },
#endif /* TMNT2_BUILD_EU */
    { PROCLABEL_SEQ_AREAPLAY,           PROCESSTYPES::PRIORITY_NORMAL,      CAreaPlaySequence::Instance         },
    { PROCLABEL_SEQ_ENDING,             PROCESSTYPES::PRIORITY_NORMAL,      CEndingSequence::Instance           },
    { PROCLABEL_SEQ_PLAYDEMO,           PROCESSTYPES::PRIORITY_NORMAL,      CPlayDemoSequence::Instance         },
    { PROCLABEL_SEQ_NORMALSTAGE,        PROCESSTYPES::PRIORITY_NORMAL,      CNormalStageSequence::Instance      },
    { PROCLABEL_SEQ_RIDESTAGE,          PROCESSTYPES::PRIORITY_NORMAL,      CRideStageSequence::Instance        },
    { PROCLABEL_SEQ_CHARASELECT,        PROCESSTYPES::PRIORITY_NORMAL,      CCharacterSelectSequence::Instance  },
    { PROCLABEL_SEQ_DATABASESEQ,        PROCESSTYPES::PRIORITY_NORMAL,      CDatabaseSequence::Instance         },
    { PROCLABEL_SEQ_DATABASE,           PROCESSTYPES::PRIORITY_NORMAL,      CDatabase::Instance                 },
    { PROCLABEL_SEQ_DATABASEVIEWER,     PROCESSTYPES::PRIORITY_NORMAL,      CDatabaseViewer::Instance           },
    { PROCLABEL_SEQ_ANTIQUESHOP,        PROCESSTYPES::PRIORITY_NORMAL,      CAntiqueShopSequence::Instance      },
    { PROCLABEL_SEQ_HOME,               PROCESSTYPES::PRIORITY_NORMAL,      CHomeSequence::Instance             },
    { PROCLABEL_SEQ_HOMESTAGE,          PROCESSTYPES::PRIORITY_NORMAL,      CHomeStageSequence::Instance        },
    { PROCLABEL_SEQ_OPTIONS,            PROCESSTYPES::PRIORITY_NORMAL,      COptionsSequence::Instance          },
    { PROCLABEL_SEQ_RESULT,             PROCESSTYPES::PRIORITY_NORMAL,      CResultSequence::Instance           },
    { PROCLABEL_SEQ_AREA,               PROCESSTYPES::PRIORITY_NORMAL,      CAreaSequence::Instance             },
    { PROCLABEL_SEQ_ENBU,               PROCESSTYPES::PRIORITY_NORMAL,      CEnbuSequence::Instance             },
    { PROCLABEL_SEQ_NEXUSMENU,          PROCESSTYPES::PRIORITY_NORMAL,      CNexusMenuSequence::Instance        },
    { PROCLABEL_SEQ_NEXUSSTAGE,         PROCESSTYPES::PRIORITY_NORMAL,      CNexusStageSequence::Instance       },
    { PROCLABEL_SEQ_NEXUSRETRY,         PROCESSTYPES::PRIORITY_NORMAL,      CNexusRetrySequence::Instance       },
    { PROCLABEL_SEQ_STAFFROLL,          PROCESSTYPES::PRIORITY_NORMAL,      CStaffRollSequence::Instance        },
    { PROCLABEL_SEQ_UNLOCK,             PROCESSTYPES::PRIORITY_NORMAL,      CUnlockSequence::Instance           },
    { PROCLABEL_SEQ_SAVELOADCHECK,      PROCESSTYPES::PRIORITY_NORMAL,      CSaveLoadCheckSequence::Instance    },
    { PROCLABEL_SEQ_SAVELOADMENUSAVE,   PROCESSTYPES::PRIORITY_NORMAL,      CSaveLoadMenuSaveSequence::Instance },
    { PROCLABEL_SEQ_SAVELOADMENULOAD,   PROCESSTYPES::PRIORITY_NORMAL,      CSaveLoadMenuLoadSequence::Instance },
    { PROCLABEL_SEQ_SAVELOADAUTO,       PROCESSTYPES::PRIORITY_NORMAL,      CSaveLoadAutoSequence::Instance     },
    { PROCLABEL_SEQ_COMINGSOON,         PROCESSTYPES::PRIORITY_NORMAL,      CComingSoonSequence::Instance       },
    { PROCLABEL_SCREENFADE,             PROCESSTYPES::PRIORITY_NORMAL + 2,  CScreenFadeProcess::Instance		},
    { PROCLABEL_LOADDISP,               PROCESSTYPES::PRIORITY_NORMAL + 2,  CLoadingDisplay::Instance           },
    { PROCLABEL_SOFTWARERESET,          PROCESSTYPES::PRIORITY_NORMAL - 2,  CSoftwareResetProcess::Instance		},
    { PROCLABEL_TIMEOUT,                PROCESSTYPES::PRIORITY_NORMAL - 2,  CTimeoutProcess::Instance           },
    { PROCLABEL_PADCONNECTCHECK,        PROCESSTYPES::PRIORITY_NORMAL + 1,  CPadConnectCheckProcess::Instance   },

    { PROCESSTYPES::LABEL_EOL }
};