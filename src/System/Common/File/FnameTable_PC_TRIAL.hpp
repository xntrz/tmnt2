
#if !defined(FILETABLE_IMPL)

namespace FILEID
{
    enum
    {
        COMMON_BEGIN = 0,
        COMMON_END   = 13,
        COMMON_MAX   = (COMMON_END - COMMON_BEGIN),

        LANGUAGE_BEGIN = 13,
        LANGUAGE_END   = 14,
        LANGUAGE_MAX   = (LANGUAGE_END - LANGUAGE_BEGIN),

        FILEID_MAX = 14,
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
    "stage/stcommon.lpac",
    "stage/st02n.lpac",
    "stage/st02nb.lpac",
    "player/donatello/donatello.lpac",
    "player/leonardo/leonardo.lpac",
    "player/michelangero/michelangero.lpac",
    "player/raphael/raphael.lpac",
    "text/text.lpac",
};

#endif /* !defined(FILETABLE_IMPL) */
