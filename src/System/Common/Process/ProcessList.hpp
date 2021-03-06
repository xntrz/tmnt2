#pragma once

#include "ProcessTypes.hpp"


namespace PROCESSTYPES
{
    enum
    {
        //
        //  Sequences
        //
        LABEL_SEQ_MAIN = LABEL_TOP,
        LABEL_SEQ_GAMEMAIN,
        LABEL_SEQ_MOVIE,
        LABEL_SEQ_LOGO,
        LABEL_SEQ_TITLE,
        LABEL_SEQ_SAVELOADAUTO,
        LABEL_SEQ_SAVELOADCHECK,
        LABEL_SEQ_SAVELOADMENUSAVE,
        LABEL_SEQ_SAVELOADMENULOAD,
        LABEL_SEQ_CHARSELECT,
        LABEL_SEQ_UNLOCK,
        LABEL_SEQ_AREA,
        LABEL_SEQ_AREAPLAY,
        LABEL_SEQ_STAGENORMAL,
        LABEL_SEQ_STAGENEXUS,
        LABEL_SEQ_STAGERIDE,
        LABEL_SEQ_STAGEDEMO,
        LABEL_SEQ_STAGEVS,
        LABEL_SEQ_RESULT,
        LABEL_SEQ_ENBU,
        LABEL_SEQ_HOME,
        LABEL_SEQ_HOMESTAGE,
        LABEL_SEQ_ANTIQUESHOP,
        LABEL_SEQ_DATABASE,
        LABEL_SEQ_NEXUSMENU,
        LABEL_SEQ_NEXUSRETRY,
        LABEL_SEQ_ENDING,
        LABEL_SEQ_STAFFROLL,

        //
        //  Processes
        //
        LABEL_SCREENFADE,
        LABEL_LOADINGDISPLAY,
        LABEL_TIMEOUT,
        LABEL_PADCONNECTCHECK,
        LABEL_SOFTWARERESET,

        LABEL_MAX,
    };
};


extern const PROCESSTYPES::PROCESS g_aProcessList[];