#include "EffectID.hpp"


struct EFFECTNAME
{
    EFFECTID::VALUE m_idEffect;
    const char* m_pszEffectName;
};


static const EFFECTNAME s_aEffectNameList[] =
{
    { EFFECTID::ID_UNKNOWN,         "unknown"           },

    //
    //  Common
    //
    { EFFECTID::ID_LEO_HIT_S,       "leo_hit_s"         },
    { EFFECTID::ID_LEO_HIT_B,       "leo_hit_b"         },
    { EFFECTID::ID_LEO_SP,          "leo_sp"            },
    { EFFECTID::ID_RAP_HIT_S,       "rap_hit_s"         },
    { EFFECTID::ID_RAP_HIT_B,       "rap_hit_b"         },
    { EFFECTID::ID_RAP_SP,          "rap_sp"            },
    { EFFECTID::ID_DON_HIT_S,       "don_hit_s"         },
    { EFFECTID::ID_DON_HIT_B,       "don_hit_b"         },
    { EFFECTID::ID_DON_LASER_BEAM,  "don_laser_beam"    },
    { EFFECTID::ID_DON_LASER_FIRE,  "don_laser_fire"    },
    { EFFECTID::ID_DON_LASER_HIT,   "don_laser_hit"     },
    { EFFECTID::ID_MIC_HIT_S,       "mic_hit_s"         },
    { EFFECTID::ID_MIC_HIT_B,       "mic_hit_b"         },
    { EFFECTID::ID_MIC_SP_BALL,     "mic_sp_ball"       },
    { EFFECTID::ID_MIC_SP_END,      "mic_sp_end"        },
    { EFFECTID::ID_SLA_SP,          "sla_sp"            },
    { EFFECTID::ID_KAR_SP,          "kar_sp"            },
    { EFFECTID::ID_CAS_SP_BALL,     "cas_sp_ball"       },
    { EFFECTID::ID_CAS_SP_END,      "cas_sp_end"        },
    { EFFECTID::ID_ALL_HIT_S,       "all_hit_s"         },
    { EFFECTID::ID_ALL_HIT_B,       "all_hit_b"         },
    { EFFECTID::ID_ALL_PIYOHIT,     "all_piyohit"       },
    { EFFECTID::ID_ALL_GUARD1,      "all_guard1"        },
    { EFFECTID::ID_ALL_GUARD2,      "all_guard2"        },
    { EFFECTID::ID_ALL_GUARDBREAK,  "all_guardbreak"    },
    { EFFECTID::ID_ALL_COUNT,       "all_count"         },
    { EFFECTID::ID_MIC_REFLECT,     "mic_reflect"       },
    { EFFECTID::ID_ALL_PIYORI,      "all_piyori"        },
    { EFFECTID::ID_ALL_SLEEP,       "all_sleep"         },
    { EFFECTID::ID_ALL_WAKE,        "all_wake"          },
    { EFFECTID::ID_ALL_NENSHI1,     "all_nenshi1"       },
    { EFFECTID::ID_ALL_NENCUT,      "all_nencut"        },
    { EFFECTID::ID_ALL_FREEZEND,    "all_freezend"      },
    { EFFECTID::ID_ALL_DASH1,       "all_dash1"         },
    { EFFECTID::ID_ALL_DASH2,       "all_dash2"         },
    { EFFECTID::ID_BARRIER_START,   "barrier_start"     },
    { EFFECTID::ID_BIRIBIRI,        "biribiri"          },
    { EFFECTID::ID_ALL_TAME3,       "all_tame3"         },
    { EFFECTID::ID_ALL_TAME2,       "all_tame2"         },
    { EFFECTID::ID_ALL_TAME1,       "all_tame1"         },
    { EFFECTID::ID_ALL_TAME_FLASH,  "all_tame_flash"    },
    { EFFECTID::ID_ALL_TAG,         "all_tag"           },
    { EFFECTID::ID_I_RECOVER_B,     "i_recover_b"       },
    { EFFECTID::ID_I_RECOVER_S,     "i_recover_s"       },
    { EFFECTID::ID_SETUP_METUKEI,   "stup_muteki"       },
    { EFFECTID::ID_SETUP_ATTACK,    "stup_attack"       },
    { EFFECTID::ID_SETUP_CHARGE,    "stup_charge"       },
    { EFFECTID::ID_SETUP_DEFENCE,   "stup_deffens"      },
    { EFFECTID::ID_SETUP_KNIFE,     "stup_shrken"       },
    { EFFECTID::ID_I_CRYSHINE_R,    "i_cryshine_r"      },
    { EFFECTID::ID_I_CRYSHINE_G,    "i_cryshine_g"      },
    { EFFECTID::ID_I_CRYSHINE_O,    "i_cryshine_o"      },
    { EFFECTID::ID_I_CRYSHINE_W,    "i_cryshine_w"      },
    { EFFECTID::ID_I_CRYGET_R,      "i_cyget_r"         },
    { EFFECTID::ID_I_CRYGET_G,      "i_cyget_g"         },
    { EFFECTID::ID_I_CRYGET_O,      "i_cyget_o"         },
    { EFFECTID::ID_I_CRYGET_W,      "i_cyget_w"         },
    { EFFECTID::ID_I_GETANTIQUE,    "i_getantique"      },
    { EFFECTID::ID_I_GET,           "i_get"             },
    { EFFECTID::ID_ALL_JUMP,        "all_jump"          },
    { EFFECTID::ID_ALL_PITAN,       "all_pitan"         },
    { EFFECTID::ID_ALL_DOWNSMOKE,   "all_downsmoke"     },
    { EFFECTID::ID_ALL_JUMPFAIL,    "all_jumpfail"      },
    { EFFECTID::ID_KO_FLASH,        "ko_flash"          },
    { EFFECTID::ID_KO_EXPL,         "ko_expl"           },
    { EFFECTID::ID_KO_SMOKE,        "ko_smoke"          },
    { EFFECTID::ID_EXPL_B1,         "expl_b1"           },
    { EFFECTID::ID_EXPL_B1_PLAYER,  "expl_b1_player"    },
    { EFFECTID::ID_EXPL_B1_CHARA,   "expl_b1_chara"     },
    { EFFECTID::ID_SLEEPGAS,        "sleepgas"          },
    { EFFECTID::ID_FREEZEGAS,       "freezegas"         },
    { EFFECTID::ID_ALL_W_HAMON,     "all_w_hamon"       },
    { EFFECTID::ID_ALL_W_DOBON,     "all_w_dobon"       },
    { EFFECTID::ID_ALL_W_SPLASH,    "all_w_splash"      },
    { EFFECTID::ID_ALL_DASH_SNOW,   "all_dash_snow"     },
    { EFFECTID::ID_ALL_BREATH,      "all_breath"        }, // NOTE: unused in game
    { EFFECTID::ID_WHITESMOKE,      "whitesmoke"        },
    { EFFECTID::ID_BLACKSMOKE,      "blacksmoke"        },

    //
    //  Individual
    //
    { EFFECTID::ID_FIRE_TORCH,      "fire_torch"        },
    { EFFECTID::ID_ROCK,            "rock"              },
    { EFFECTID::ID_WOOD,            "wood"              },
    { EFFECTID::ID_METAL,           "metal"             },
    { EFFECTID::ID_GRASS,           "grass"             },
    { EFFECTID::ID_DUSTBOX,         "dustbox"           },
    { EFFECTID::ID_CERAMICS,        "ceramics"          },
    { EFFECTID::ID_METEORITE,       "meteorite"         },
    { EFFECTID::ID_SPARKS,          "sparks"            },
    { EFFECTID::ID_BREAKSIGN,       "breaksign"         },
    { EFFECTID::ID_PIPE_STEAM,      "pipe_steam"        },
    { EFFECTID::ID_PIPE_FREEZEGAS,  "pipe_freezegas"    },
    { EFFECTID::ID_SPRAYWTER,       "spraywater"        },
    { EFFECTID::ID_SWEEP_SPLASH,    "sweep_splash"      },
    { EFFECTID::ID_FIRE_ARROWSHOT,  "fire_arrowshot"    },
    { EFFECTID::ID_SLEEP_ARROWSHOT, "sleep_arrowshot"   },
    { EFFECTID::ID_FIRE_ARROW,      "fire_arrow"        },
    { EFFECTID::ID_SLEEP_ARROW,     "sleep_arrow"       },
    { EFFECTID::ID_SPSM_YODAR2,     "spsm_yodar2"       },
    { EFFECTID::ID_SPSM_YODAR1,     "spsm_yodar1"       },
    { EFFECTID::ID_SPSM_KABECRASH,  "spsm_kabecrash"    },
    { EFFECTID::ID_SPSM_RUNSMOKE,   "spsm_runsmoke"     },
    { EFFECTID::ID_SPSM_TBSHOT,     "spsm_tbshot"       },
    { EFFECTID::ID_TENGSWORD,       "tengsword"         },
    { EFFECTID::ID_SHRD_ELEC,       "shrd_elec"         },
    { EFFECTID::ID_THUNDER,         "thunder"           },
    { EFFECTID::ID_SHRD_ELBALL,     "shrd_elball"       },
    { EFFECTID::ID_SHRD_ELEND,      "shrd_elend"        },
    { EFFECTID::ID_ROD_FLASH,       "rod_flash"         },
    { EFFECTID::ID_DRKBALL_START,   "drkball_start"     },
    { EFFECTID::ID_DRKBALL,         "drkball"           },
    { EFFECTID::ID_DRKBALL_END,     "drkball_end"       },
    { EFFECTID::ID_DRK_PILLAR,      "drk_pillar"        },
    { EFFECTID::ID_ROD_BEAM,        "rod_beam"          },
    { EFFECTID::ID_ROD_BEAM_HIT,    "rod_beam_hit"      },
    { EFFECTID::ID_DRAGONSMOKE,     "dragonsmoke"       },
    { EFFECTID::ID_WINDWARP_START,  "windwarp_start"    },
    { EFFECTID::ID_WINDWARP_OUT,    "windwarp_out"      },
    { EFFECTID::ID_ULNIN_ENMAKU,    "ulnin_enmaku"      },
    { EFFECTID::ID_WARP_START,      "warp_start"        },
    { EFFECTID::ID_WARP_OUT,        "warp_out"          },
    { EFFECTID::ID_HPSTEAL_LIGHT,   "hpsteal_light"     },
    { EFFECTID::ID_SICKLE_WARP,     "sickle_warp"       },
    { EFFECTID::ID_SLA_BARRIER,     "sla_barrier"       },
    { EFFECTID::ID_SLA_SHADOW,      "sla_shadow"        },
    { EFFECTID::ID_HPSTEAL,         "hpsteal"           },
    { EFFECTID::ID_SLABALL,         "slaball"           },
    { EFFECTID::ID_SLABALL_END,     "slaball_end"       },
    { EFFECTID::ID_BLADE_FLASh,     "blade_flash"       },
    { EFFECTID::ID_WIND_PRESS,      "wind_press"        },
    { EFFECTID::ID_LASER_BEAM_FED,  "laser_beam_fed"    },
    { EFFECTID::ID_LASER_FIRE_FED,  "laser_fire_fed"    },
    { EFFECTID::ID_LASER_HIT_FED,   "laser_hit_fed"     },
    { EFFECTID::ID_LASER_BEAM_TRI,  "laser_beam_tri"    },
    { EFFECTID::ID_LASER_FIRE_TRI,  "laser_fire_tri"    },
    { EFFECTID::ID_LASER_HIT_TRI,   "laser_hit_tri"     },
    { EFFECTID::ID_GUNBALL,         "gunball"           },
    { EFFECTID::ID_GUNBALL_FIRE,    "gunball_fire"      },
    { EFFECTID::ID_SPIDERNET,       "spidernet"         },
    { EFFECTID::ID_SPIDERSMOKE,     "spidersmoke"       },
    { EFFECTID::ID_TBSHOT,          "tbshot"            },
    { EFFECTID::ID_TBSHOT_MARK,     "tbshot_mark"       },
    { EFFECTID::ID_LASER_BEAM_ALL,  "laser_beam_all"    },
    { EFFECTID::ID_LASER_FIRE_ALL,  "laser_fire_all"    },
    { EFFECTID::ID_LASER_HIT_ALL,   "laser_hit_all"     },
    { EFFECTID::ID_FLAME_MAP,       "flame_map"         },
    { EFFECTID::ID_M12_BREAKROCK,   "m12_breakrock"     },
    { EFFECTID::ID_BREAKICE,        "breakice"          },
    { EFFECTID::ID_ENGINESPARK,     "enginespark"       },
    { EFFECTID::ID_SPARK_SLOW,      "spark_slow"        },
    { EFFECTID::ID_BUBBLE,          "bubble"            },
    { EFFECTID::ID_WFALL_SMOKE,     "wfall_smoke"       },
    { EFFECTID::ID_WFALL_SPRASH,    "wfall_sprash"      },
    { EFFECTID::ID_RIDE_ENGINE,     "ride_engine"       },
    { EFFECTID::ID_RIDE_PARTICLE,   "ride_particle"     },
    { EFFECTID::ID_RIDE_WIND,       "ride_wind"         },
    { EFFECTID::ID_RAIN,            "rain"              },
    { EFFECTID::ID_SNOW,            "snow"              },
};


static_assert(COUNT_OF(s_aEffectNameList) == EFFECTID::ID_MAX, "update me");


namespace EFFECTID
{
    VALUE GetIDFromName(const char* pszName)
    {
        for (int32 i = 0; i < COUNT_OF(s_aEffectNameList); ++i)
        {
            if (!std::strcmp(s_aEffectNameList[i].m_pszEffectName, pszName))
                return s_aEffectNameList[i].m_idEffect;
        };

        return ID_UNKNOWN;
    };


    const char* GetNameFromID(VALUE idEffect)
    {
        ASSERT(idEffect >= 0 && idEffect < ID_MAX);
        
        return s_aEffectNameList[idEffect].m_pszEffectName;
    };

    
    bool IsGeneric(VALUE idEffect)
    {
        return (idEffect >= EFFECTID::ID_GENERIC_START) &&
               (idEffect <= EFFECTID::ID_GENERIC_END);
    };
};