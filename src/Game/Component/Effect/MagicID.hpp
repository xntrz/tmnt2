#pragma once


namespace MAGICID
{
    enum VALUE
    {
        ID_UNKNOWN = 0,
        
        ID_LEO_SP,
        ID_RAP_SP,
        ID_MIC_SP,
        ID_SLA_SP,
        ID_KAR_SP,
        ID_CAS_SP,
        ID_DON_LASER_BEAM,
        ID_EXPL_B1,
        ID_EXPL_B1_PLAYER,
        ID_EXPL_B1_CHARA,
        ID_PIPE_STEAM,
        ID_PIPE_FREEZEGAS,
        ID_SLEEPGAS,
        ID_FREEZEGAS,
        ID_FIRE_TORCH,
        ID_FLAME_MAP,
        ID_SPRAY_WATER,
        ID_SPSM_TBSHOT,
        ID_SLEEPGAS_1,
        ID_TENGSWORD,
        ID_SHRD_ELBALL,
        ID_THUNDER,
        ID_DRKBALL,
        ID_DRK_PILLAR,
        ID_ROD_BEAM,
        ID_ULNIN_ENMAKU,
        ID_SLABALL,
        ID_HPSTEAL,
        ID_WINDPRESS,
        ID_KO_EXPL,
        ID_LASER_BEAM_FED,
        ID_LASER_BEAM_TRI,
        ID_LASER_BEAM_ALL,
        ID_SPIDERNET,
        ID_TBSHOT,
        ID_TBSHOT_MARK,
        ID_SLEEPGAS_2,
        ID_GUNBALL,

        ID_MAX,
    };

    VALUE GetIDFromName(const char* pszName);
    const char* GetNameFromID(VALUE idEffect);    
};