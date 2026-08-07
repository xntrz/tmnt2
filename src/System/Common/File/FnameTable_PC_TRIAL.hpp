
#if !defined(FILETABLE_IMPL)

namespace FILEID
{
    enum
    {
        COMMON_BEGIN = 0,
        COMMON_END   = 23,
        COMMON_MAX   = (COMMON_END - COMMON_BEGIN),

        LANGUAGE_BEGIN = 23,
        LANGUAGE_END   = 24,
        LANGUAGE_MAX   = (LANGUAGE_END - LANGUAGE_BEGIN),

        FILEID_MAX = (COMMON_MAX + LANGUAGE_MAX),
    };
};

#else /* !defined(FILETABLE_IMPL) */

/*static*/ const char* CFilename::m_apszFilename[] =
{
    "fonts/mainfont.met",
    "fonts/fonts.lpac",
    "menu/logo/logo.lpac",
    "menu/title/title.lpac",
    "menu/title/title2.lpac",
    "menu/characterselect/charselect.lpac",
    "menu/option/options_ps2.lpac",
    "stage/stcommon.lpac",
    "stage/stcommon_r.lpac",
    "stage/st12n.lpac",
    "stage/st13r.lpac",
    "stage/st14n.lpac",
    "stage/st19n.lpac",
    "stage/st20f.lpac",
    "stage/st20fb.lpac",
    "player/donatello/donatello.lpac",
    "player/leonardo/leonardo.lpac",
    "player/michelangero/michelangero.lpac",
    "player/raphael/raphael.lpac",
    "ride/donatello/donatello.lpac",
    "ride/leonardo/leonardo.lpac",
    "ride/michelangero/michelangero.lpac",
    "ride/raphael/raphael.lpac",
    "text/text.lpac",
};

#endif /* !defined(FILETABLE_IMPL) */
