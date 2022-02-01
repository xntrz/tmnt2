#include "MagicID.hpp"


struct MAGICNAME
{
    MAGICID::VALUE m_idMagic;
    const char* m_pszMagicName;
};


static const MAGICNAME s_aMagicNameList[] =
{
    { MAGICID::ID_UNKNOWN,         "unknown"           },
    { MAGICID::ID_LEO_SP,          "leo_sp"            },
    { MAGICID::ID_RAP_SP,          "rap_sp"            },
    { MAGICID::ID_MIC_SP,          "mic_sp_ball"       },
    { MAGICID::ID_SLA_SP,          "sla_sp"            },
    { MAGICID::ID_KAR_SP,          "kar_sp"            },
    { MAGICID::ID_CAS_SP,          "cas_sp_ball"       },
    { MAGICID::ID_DON_LASER_BEAM,  "don_laser_beam"    },
    { MAGICID::ID_EXPL_B1,         "expl_b1"           },
    { MAGICID::ID_EXPL_B1_PLAYER,  "expl_b1_player"    },
    { MAGICID::ID_EXPL_B1_CHARA,   "expl_b1_chara"     },
    { MAGICID::ID_PIPE_STEAM,      "pipe_steam"        },
    { MAGICID::ID_PIPE_FREEZEGAS,  "pipe_freezegas"    },
    { MAGICID::ID_SLEEPGAS,        "sleepgas"          },
    { MAGICID::ID_FREEZEGAS,       "freezegas"         },
    { MAGICID::ID_FIRE_TORCH,      "fire_torch"        },
    { MAGICID::ID_FLAME_MAP,       "flame_map"         },
    { MAGICID::ID_SPRAY_WATER,     "spraywater"        },
    { MAGICID::ID_SPSM_TBSHOT,     "spsm_tbshot"       },
    { MAGICID::ID_SLEEPGAS_1,      "sleepgas"          },
    { MAGICID::ID_TENGSWORD,       "tengsword"         },
    { MAGICID::ID_SHRD_ELBALL,     "shrd_elball"       },
    { MAGICID::ID_THUNDER,         "thunder"           },
    { MAGICID::ID_DRKBALL,         "drkball"           },
    { MAGICID::ID_DRK_PILLAR,      "drk_pillar"        },
    { MAGICID::ID_ROD_BEAM,        "rod_beam"          },
    { MAGICID::ID_ULNIN_ENMAKU,    "ulnin_enmaku"      },
    { MAGICID::ID_SLABALL,         "slaball"           },
    { MAGICID::ID_HPSTEAL,         "hpsteal"           },
    { MAGICID::ID_WINDPRESS,       "wind_press"        },
    { MAGICID::ID_KO_EXPL,         "ko_expl"           },
    { MAGICID::ID_LASER_BEAM_FED,  "laser_beam_fed"    },
    { MAGICID::ID_LASER_BEAM_TRI,  "laser_beam_tri"    },
    { MAGICID::ID_LASER_BEAM_ALL,  "laser_beam_all"    },
    { MAGICID::ID_SPIDERNET,       "spidernet"         },
    { MAGICID::ID_TBSHOT,          "tbshot"            },
    { MAGICID::ID_TBSHOT_MARK,     "tbshot_mark"       },
    { MAGICID::ID_SLEEPGAS_2,      "sleepgas"          },
    { MAGICID::ID_GUNBALL,         "gunball"           },
};


static_assert(COUNT_OF(s_aMagicNameList) == MAGICID::ID_MAX, "update me");


namespace MAGICID
{
    VALUE GetIDFromName(const char* pszName)
    {
        for (int32 i = 0; i < COUNT_OF(s_aMagicNameList); ++i)
        {
            if (!std::strcmp(s_aMagicNameList[i].m_pszMagicName, pszName))
                return s_aMagicNameList[i].m_idMagic;
        };

        ASSERT(false);
        return ID_UNKNOWN;
    };


    const char* GetNameFromID(VALUE idMagic)
    {
        ASSERT(idMagic >= 0 && idMagic < ID_MAX);

        return s_aMagicNameList[idMagic].m_pszMagicName;
    };
};