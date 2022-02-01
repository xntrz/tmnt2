#include "AfsFilename.hpp"
#include "AfsFileID.hpp"
#include "FileTypes.hpp"


static const char* s_pszCommonDataPath = "common/";


static const char* s_apszLanguageDataPath[] =
{
    "language/english/",
};


static const char* s_apszFilename[] =
{
    "fonts/mainfont.met",
    "fonts/fonts.lpac",
    "menu/logo/logo.lpac",
    "menu/title/title.lpac",
    "menu/title/title2.lpac",
    "menu/characterselect/charselect.lpac",
    "menu/comingsoon/comingsoon.lpac",
    "menu/option/options_ps2.lpac",
    "menu/antiqueshop/antique_ps2.lpac",
    "demo/pc/demo0020.dat",
    "demo/pc/demo0021.dat",
    "demo/pc/demo0022.dat",
    "demo/pc/demo0023.dat",
    "demo/pc/demo0220.dat",
    "demo/pc/demo0221.dat",
    "demo/pc/demo0222.dat",
    "demo/pc/demo0223.dat",
    "menu/window/unlock/unlock.lpac",
    "nexus/nexusmenu.lpac",
    "stage/stcommon.lpac",
    "stage/stcommon_h.lpac",
    "stage/stcommon_r.lpac",
    "stage/stcommon_n.lpac",
    "stage/st01t.lpac",
    "stage/st02n.lpac",
    "stage/st02nb.lpac",
    "stage/st03s.lpac",
    "stage/st04n.lpac",
    "stage/st04nb.lpac",
    "stage/st05n.lpac",
    "stage/st06r.lpac",
    "stage/st09n.lpac",
    "stage/st10n.lpac",
    "stage/st11j.lpac",
    "stage/st12n.lpac",
    "stage/st13r.lpac",
    "stage/st14n.lpac",
    "stage/st15n.lpac",
    "stage/st17n.lpac",
    "stage/st18f.lpac",
    "stage/st18fb.lpac",
    "stage/st19n.lpac",
    "stage/st20f.lpac",
    "stage/st20fb.lpac",
    "stage/st21n.lpac",
    "stage/st21nb.lpac",
    "stage/st22r.lpac",
    "stage/st23n.lpac",
    "stage/st24n.lpac",
    "stage/st25j.lpac",
    "stage/st26n.lpac",
    "stage/st27n.lpac",
    "stage/st28n.lpac",
    "stage/st29n.lpac",
    "stage/st30n.lpac",
    "stage/st30nb.lpac",
    "stage/st31nb.lpac",
    "stage/st32r.lpac",
    "stage/st33j.lpac",
    "stage/st34n.lpac",
    "stage/st35n.lpac",
    "stage/st36n.lpac",
    "stage/st37n.lpac",
    "stage/st38nb.lpac",
    "stage/st39n.lpac",
    "stage/st40ob.lpac",
    "stage/st41n.lpac",
    "stage/st42nb.lpac",
    "stage/st43n.lpac",
    "stage/st44n.lpac",
    "stage/st44nb.lpac",
    "stage/st45n.lpac",
    "stage/st46r.lpac",
    "stage/st47o.lpac",
    "stage/st47ob.lpac",
    "stage/st48n.lpac",
    "stage/st49n.lpac",
    "stage/st50nb.lpac",
    "stage/st52f.lpac",
    "stage/st52fb.lpac",
    "stage/st53f.lpac",
    "stage/st53fb.lpac",
    "stage/st54f.lpac",
    "stage/st54fb.lpac",
    "stage/st55f.lpac",
    "stage/st55fb.lpac",
    "stage/st56nb.lpac",
    "stage/st57nb.lpac",
    "stage/st58ob1.lpac",
    "stage/st58ob2.lpac",
    "stage/st60x_a01.lpac",
    "stage/st60x_a02.lpac",
    "stage/st60x_a03.lpac",
    "stage/st60x_a04.lpac",
    "stage/st60x_a05.lpac",
    "stage/st60x_a06.lpac",
    "stage/st60x_a07.lpac",
    "stage/st60x_a08.lpac",
    "stage/st60x_a09.lpac",
    "stage/st60x_a10.lpac",
    "stage/st60x_b01.lpac",
    "stage/st60x_b02.lpac",
    "stage/st60x_b03.lpac",
    "stage/st60x_b04.lpac",
    "stage/st60x_b05.lpac",
    "stage/st60x_b06.lpac",
    "stage/st60x_b07.lpac",
    "stage/st60x_b08.lpac",
    "stage/st60x_b09.lpac",
    "stage/st60x_b10.lpac",
    "stage/st60x_c01.lpac",
    "stage/st60x_c02.lpac",
    "stage/st60x_c03.lpac",
    "stage/st60x_c04.lpac",
    "stage/st60x_c05.lpac",
    "stage/st60x_c06.lpac",
    "stage/st60x_c07.lpac",
    "stage/st60x_c08.lpac",
    "stage/st60x_c09.lpac",
    "stage/st60x_c10.lpac",
    "stage/st60x_d01.lpac",
    "stage/st60x_d02.lpac",
    "stage/st60x_d03.lpac",
    "stage/st60x_d04.lpac",
    "stage/st60x_d05.lpac",
    "stage/st60x_d06.lpac",
    "stage/st60x_d07.lpac",
    "stage/st60x_d08.lpac",
    "stage/st60x_d09.lpac",
    "stage/st60x_d10.lpac",
    "player/donatello/donatello.lpac",
    "player/donatello/donatello_exa.lpac",
    "player/donatello/donatello_exb.lpac",
    "player/leonardo/leonardo.lpac",
    "player/leonardo/leonardo_exa.lpac",
    "player/leonardo/leonardo_exb.lpac",
    "player/michelangero/michelangero.lpac",
    "player/michelangero/michelangero_exa.lpac",
    "player/michelangero/michelangero_exb.lpac",
    "player/raphael/raphael.lpac",
    "player/raphael/raphael_exa.lpac",
    "player/raphael/raphael_exb.lpac",
    "player/casey/casey.lpac",
    "player/slashuur/slashuur.lpac",
    "player/splinter/splinter.lpac",
    "player/karai/karai.lpac",
    "ride/casey/casey.lpac",
    "ride/casey/casey_ship.lpac",
    "ride/donatello/donatello.lpac",
    "ride/donatello/donatello_exa.lpac",
    "ride/donatello/donatello_exb.lpac",
    "ride/donatello/donatello_ship.lpac",
    "ride/karai/karai.lpac",
    "ride/karai/karai_ship.lpac",
    "ride/leonardo/leonardo.lpac",
    "ride/leonardo/leonardo_exa.lpac",
    "ride/leonardo/leonardo_exb.lpac",
    "ride/leonardo/leonardo_ship.lpac",
    "ride/michelangero/michelangero.lpac",
    "ride/michelangero/michelangero_exa.lpac",
    "ride/michelangero/michelangero_exb.lpac",
    "ride/michelangero/michelangero_ship.lpac",
    "ride/raphael/raphael.lpac",
    "ride/raphael/raphael_exa.lpac",
    "ride/raphael/raphael_exb.lpac",
    "ride/raphael/raphael_ship.lpac",
    "ride/slashuur/slashuur.lpac",
    "ride/slashuur/slashuur_ship.lpac",
    "ride/splinter/splinter.lpac",
    "ride/splinter/splinter_ship.lpac",
    "result/result.lpac",
    "result/result_r.lpac",
    "result/result_n.lpac",
    "result/result_n_lose.lpac",
    "result/result_e.lpac",
    "area/area_ny.lpac",
    "area/area_dho.lpac",
    "area/area_tri.lpac",
    "area/area_jpn.lpac",
    "area/area_fny.lpac",
    "area/area_kur.lpac",
    "menu/database/database.lpac",
    "menu/database/character/db_leo_a.lpac",
    "menu/database/character/db_leo_b.lpac",
    "menu/database/character/db_sla.lpac",
    "menu/database/character/db_rap_a.lpac",
    "menu/database/character/db_rap_b.lpac",
    "menu/database/character/db_cas.lpac",
    "menu/database/character/db_mic_a.lpac",
    "menu/database/character/db_mic_b.lpac",
    "menu/database/character/db_kar.lpac",
    "menu/database/character/db_don_a.lpac",
    "menu/database/character/db_don_b.lpac",
    "menu/database/character/db_spl.lpac",
    "menu/database/character/db_apl.lpac",
    "menu/database/enemy/db_shredder.lpac",
    "menu/database/enemy/db_dorako.lpac",
    "menu/database/enemy/db_rats.lpac",
    "menu/database/enemy/db_traximus.lpac",
    "menu/database/enemy/db_leather.lpac",
    "menu/database/enemy/db_foot.lpac",
    "menu/database/enemy/db_spasmo.lpac",
    "menu/database/enemy/db_hun.lpac",
    "menu/database/enemy/db_ultimate.lpac",
    "menu/database/enemy/db_miyamoto.lpac",
    "menu/database/enemy/db_zako_a.lpac",
    "menu/database/enemy/db_zako_b.lpac",
    "menu/database/enemy/db_others.lpac",
    "menu/database/bg/db_ny.lpac",
    "menu/database/bg/db_dhoonib.lpac",
    "menu/database/bg/db_japan.lpac",
    "menu/database/bg/db_kuraiyama.lpac",
    "menu/database/bg/db_dimension.lpac",
    "menu/database/bg/db_vehicle.lpac",
    "menu/database/bg/db_anime.lpac",
    "menu/database/bg/db_cg.lpac",
    "menu/database/etc/db_play_a.lpac",
    "menu/database/etc/db_play_b.lpac",
    "menu/database/etc/db_play_c.lpac",
    "menu/database/etc/db_play_d.lpac",
    "menu/database/etc/db_gallery.lpac",
    "enbu/leo/enbu_leo.lpac",
    "enbu/rap/enbu_rap.lpac",
    "enbu/mic/enbu_mic.lpac",
    "enbu/don/enbu_don.lpac",
    "enbu/sls/enbu_sls.lpac",
    "enbu/cas/enbu_cas.lpac",
    "enbu/kri/enbu_kri.lpac",
    "enbu/spl/enbu_spl.lpac",
    "enbu/leo/enbu_leoexa.lpac",
    "enbu/rap/enbu_rapexa.lpac",
    "enbu/mic/enbu_micexa.lpac",
    "enbu/don/enbu_donexa.lpac",
    "enbu/leo/enbu_leoexb.lpac",
    "enbu/rap/enbu_rapexb.lpac",
    "enbu/mic/enbu_micexb.lpac",
    "enbu/don/enbu_donexb.lpac",
    
    "text/text.lpac",
};


static_assert(COUNT_OF(s_apszFilename) == AFSFILEID::ID_MAX, "update me");


/*static*/ void CAfsFilename::MapReadInfo(const char* pszFilename, int32* piFileID, int32* piPartitionID)
{
    ASSERT(pszFilename);
    ASSERT(piFileID);
    ASSERT(piPartitionID);

    MapReadInfo(FileID(pszFilename), piFileID, piPartitionID);
};


/*static*/ void CAfsFilename::MapReadInfo(int32 iFileID, int32* piFileID, int32* piPartitionID)
{
    ASSERT(piFileID);
    ASSERT(piPartitionID);
    
    if (iFileID < 237)
    {
        *piPartitionID = 0;
        *piFileID = iFileID;
    }
    else if (iFileID < 238)
    {
        *piPartitionID = 1;
        *piFileID = iFileID - 237;
    }
    else
    {
        ASSERT(false);
    };
};


/*static*/ void CAfsFilename::Filename(char* pszBuffer, int32 iFileID)
{
    ASSERT(iFileID >= 0 && iFileID < AFSFILEID::ID_MAX);

    if (iFileID < 237)
        strcpy(pszBuffer, s_pszCommonDataPath);
    else if (iFileID < 238)
        strcpy(pszBuffer, s_apszLanguageDataPath[0]);

    strcat(pszBuffer, s_apszFilename[iFileID]);
};


/*static*/ int32 CAfsFilename::FileID(const char* pszFilename)
{
    ASSERT(strlen(pszFilename) < FILETYPES::FILE_NAME_MAX);

    char szFilename[FILETYPES::FILE_NAME_MAX];
    int32 PathLen = 0;
    
    strcpy(szFilename, pszFilename);
    ConvPathTable(szFilename);
    
    PathLen = strlen(s_pszCommonDataPath);
    if (!strncmp(szFilename, s_pszCommonDataPath, PathLen))
    {
        for (int32 i = 0; i < 237; i++)
        {
            if (!strcmp(&szFilename[PathLen], s_apszFilename[i]))
                return i;
        };
    };

    PathLen = strlen(s_apszLanguageDataPath[0]);
    if (!strncmp(szFilename, s_apszLanguageDataPath[0], PathLen))
    {
        for (int32 i = 237; i < 238; i++)
        {
            if (!strcmp(&szFilename[PathLen], s_apszFilename[i]))
                return i;
        };
    };

    return AFSFILEID::ID_INVALID;
};


/*static*/ void CAfsFilename::ConvPathTable(char* pszPath)
{
    while (*pszPath)
    {
        if (*pszPath == '\\')
            *pszPath = '/';
        else if (*pszPath >= 'A' && *pszPath <= 'Z')
            *pszPath = *pszPath + 0x20;

        ++pszPath;
    };
};


/*static*/ void CAfsFilename::ConvPathPlatform(char* pszPath)
{
    while (*pszPath)
    {
#ifdef _TARGET_PC
        if (*pszPath == '/')
            *pszPath = '\\';
#else
        if (*pszPath == '\\')
            *pszPath = '/';
#endif        
        ++pszPath;
    };
};