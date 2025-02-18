#include "Filename.hpp"
#include "FileID.hpp"
#include "FileTypes.hpp"

#include "System/Common/Configure.hpp"


static const char* s_pszCommonDataPath = "common/";


static const char* s_apszLanguageDataPath[] =
{
    "language/english/",
#ifdef TMNT2_BUILD_EU
    "language/german/",
    "language/french/",
    "language/spanish/",
    "language/italian/",
#endif /* TMNT2_BUILD_EU */
};


/* checkouts with idb */
#ifdef TMNT2_BUILD_EU
static_assert(COUNT_OF(s_apszLanguageDataPath) == TYPEDEF::CONFIG_LANG_NUM, "lang data path table incorrect");
#else /* TMNT2_BUILD_EU */
static_assert(COUNT_OF(s_apszLanguageDataPath) == 1, "lang data path table incorrect");
#endif /* TMNT2_BUILD_EU */


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
    // "stage/ste3demo1.lpac",
    // "stage/ste3demo2.lpac",
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

#ifdef TMNT2_BUILD_EU
    "movietext/m01t01.lpac",
    "movietext/m02n01.lpac",
    "movietext/m02n02.lpac",
    "movietext/m04n01.lpac",
    "movietext/m05n01.lpac",
    "movietext/m06r01.lpac",
    "movietext/m09n01.lpac",
    "movietext/m10n01.lpac",
    "movietext/m11j01.lpac",
    "movietext/m12n01.lpac",
    "movietext/m14n01.lpac",
    "movietext/m15n01.lpac",
    "movietext/m17n01.lpac",
    "movietext/m18fb01.lpac",
    "movietext/m18fb02.lpac",
    "movietext/m20fb01.lpac",
    "movietext/m20fb02.lpac",
    "movietext/m21n01.lpac",
    "movietext/m21n02.lpac",
    "movietext/m24n01.lpac",
    "movietext/m24n02.lpac",
    "movietext/m25j01.lpac",
    "movietext/m27n01.lpac",
    "movietext/m29n01.lpac",
    "movietext/m29n02.lpac",
    "movietext/m30nb01.lpac",
    "movietext/m31nb01.lpac",
    "movietext/m32r01.lpac",
    "movietext/m33j01.lpac",
    "movietext/m38nb01.lpac",
    "movietext/m38nb02.lpac",
    "movietext/m39n01.lpac",
    "movietext/m40ob01.lpac",
    "movietext/m40ob02.lpac",
    "movietext/m41nb01.lpac",
    "movietext/m42nb01.lpac",
    "movietext/m43n01.lpac",
    "movietext/m44nb01.lpac",
    "movietext/m45n01.lpac",
    "movietext/m45n02.lpac",
    "movietext/m46r01.lpac",
    "movietext/m47ob01.lpac",
    "movietext/m47ob02.lpac",
    "movietext/m48n01.lpac",
    "movietext/m50nb01.lpac",
    "movietext/m50nb02.lpac",
    "movietext/m56nb01.lpac",
    "movietext/m57nb01.lpac",
    "movietext/m57nb02.lpac",
    "movietext/m58ob01.lpac",
    "movietext/m58ob02.lpac",
    "movietext/m59s01.lpac",
    "movietext/m59s02.lpac",
    "movietext/m59s03.lpac",
    "movietext/m60x03.lpac",
    "movietext/m62x01.lpac",
#endif /* TMNT2_BUILD_EU */
    
    "text/text.lpac",

#ifdef TMNT2_BUILD_EU
    "charselect/charselect.lpac",
    "nexusmenu/nexusmenu.lpac",
    "res/res.lpac",
    "enbu_rank/enbu_rank.lpac",
    "areaselect/areaselect.lpac",
    "gauge_eu/gauge_eu.lpac",
    "home_eu/home_eu.lpac",
    "ride_eu/ride_eu.lpac",
#endif /* TMNT2_BUILD_EU */
};


/* checkouts with idb */
#ifdef TMNT2_BUILD_EU
static_assert(COUNT_OF(s_apszFilename) == 302, "multilang eu build has 302 max files");
#else /* TMNT2_BUILD_EU */
static_assert(COUNT_OF(s_apszFilename) == 238, "en build has 238 max files");
#endif


/* common checkout */
static_assert(COUNT_OF(s_apszFilename) == FILEID::ID_MAX, "update me");


/*static*/ int32 CFilename::ID(const char* pszFilename)
{
    ASSERT(std::strlen(pszFilename) < static_cast<size_t>(FILETYPES::FILE_NAME_MAX));

    char szFilename[FILETYPES::FILE_NAME_MAX];
    szFilename[0] = '\0';
    
    /* converting path to the table format */
    std::strcpy(szFilename, pszFilename);
    ConvPathTable(szFilename);

    /* search in common container first */
    size_t len = std::strlen(s_pszCommonDataPath);
    if (!std::strncmp(szFilename, s_pszCommonDataPath, len))
    {
        for (int32 i = FILEID::COMMONBEGIN; i < FILEID::COMMONEND; ++i)
        {
            if (!std::strcmp(&szFilename[len], s_apszFilename[i]))
                return i;
        };
    };

    /* now search in lang container */
    len = std::strlen(s_apszLanguageDataPath[0]);
    if (!std::strncmp(szFilename, s_apszLanguageDataPath[0], len))
    {
        for (int32 i = FILEID::LANGBEGIN; i < FILEID::LANGEND; ++i)
        {
            if (!strcmp(&szFilename[len], s_apszFilename[i]))
                return i;
        };
    };

    /* file not found */
    return FILEID::ID_INVALID;
};


/*static*/ void CFilename::ConvPathTable(char* pszPath)
{
    while (*pszPath)
    {
        if (*pszPath == '\\')
        {
            *pszPath = '/';
        }
        else if ((*pszPath >= 'A') &&
                 (*pszPath <= 'Z'))
        {
            *pszPath = (*pszPath + 0x20); // to lower case
        };

        ++pszPath;
    };
};


/*static*/ void CFilename::ConvPathPlatform(char* pszPath)
{
    while (*pszPath)
    {
#ifdef TARGET_PC
        if (*pszPath == '/')
            *pszPath = '\\';
#else /* TARGET_PC */
        if (*Path == '\\')
            *Path = '/';
#endif /* TARGET_PC */
        ++pszPath;
    };
};


/*static*/ void CFilename::Filename(char* Buff, int32 FileId)
{
    ASSERT(FileId >= 0);
    ASSERT(FileId < FILEID::ID_MAX);

    if (FileId < FILEID::COMMONEND)
    {
        std::strcpy(Buff, s_pszCommonDataPath);
    }
    else if (FileId < FILEID::LANGEND)
    {
        TYPEDEF::CONFIG_LANG lang = CConfigure::GetLanguage();
        std::strcpy(Buff, s_apszLanguageDataPath[lang]);
    };

    std::strcat(Buff, s_apszFilename[FileId]);
};