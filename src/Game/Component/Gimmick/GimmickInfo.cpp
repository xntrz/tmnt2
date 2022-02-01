#include "GimmickInfo.hpp"


struct GIMMICKSUBINFO
{
    uint32 dummy;
};


struct GIMMICKINFO
{
    const char* m_pszBaseName;
    CGimmickInfo::PARAMTYPE m_paramtype;
    CGimmickInfo::DRAWPRI m_drawpri;
    int32 m_nNumSubID;
    const GIMMICKSUBINFO* m_pSubInfoTop;
};


struct GIMMICKINFOTABLE
{
    GIMMICKID::VALUE m_idStart;
    int32 m_nNumInfo;
    const GIMMICKINFO* m_pInfoArray;
};


//
//  SYSTEM
//
static GIMMICKSUBINFO s_aSubInfo_GMSANDSWI[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSGROUP[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSGCLEAR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSGOVER[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSMOVIE[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSCAMERA[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSEVCAM[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSSOUND[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSWALL[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSFALLPT[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSRESET[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSHELP[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSTUTO[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMSEKILL[]     = { { 0 } };


//
//  EVENT
//
static GIMMICKSUBINFO s_aSubInfo_GMEPDEAD []    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMETIME[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEEBRK[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEEBRKAL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEPWALL[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEPSPHER[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEPVOLUM[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEGBRK[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEEWALL[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEUTRVOL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEDONLSR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMERRSPHR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMEFTWALL[]    = { { 0 } };


//
//  GENERATOR
//
static GIMMICKSUBINFO s_aSubInfo_GMGSET[]       = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGGROUND[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGAIR[]       = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGEGG[]       = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGCAVE[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGDOOR[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGCAR[]       = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMGSSHIPS[]    = { { 0 } };


//
//  NORMAL
//
static GIMMICKSUBINFO s_aSubInfo_GMNTHROW[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNITEMBX[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNBRKCAR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNBRKOBJ[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNROLOBJ[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMVFLLI[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMVFLPA[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMVFLRO[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNCONSOL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSWITCH[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSWIPIL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNBRKPIL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSHKPIL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNNORPIL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNRAPROC[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNLEOS[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNLEOM[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNLEOL[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNGRASS[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNCRYSTL[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNGUILLO[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSEESAW[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNFALOBJ[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMETEO[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNDOOR[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNUTROM[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNLASER[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMINE[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNBELCON[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSTOPER[]    = { { 0 } };


//
//  RIDE
//
static GIMMICKSUBINFO s_aSubInfo_GMRFLAG[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMRTHNDER[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMRASTRID[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMRSHUTTR[]    = { { 0 } };


//
//  OTHER
//
static GIMMICKSUBINFO s_aSubInfo_GMNITEM[]      = { { 0 } };


//
//  MAP
//
static GIMMICKSUBINFO s_aSubInfo_GMNLIGHT[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNWEATHER[]   = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSMOKE[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNFLAME[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSTEAM[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSPARK[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMOTH[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNFISH[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNBUBBLE[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNMIST[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNSPLASH[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMNEXP[]       = { { 0 } };


//
//  KOYUU
//
static GIMMICKSUBINFO s_aSubInfo_GMKM12N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM02N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM05N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM38B[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM38MAN[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM17N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM43N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM28NGR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM28NPT[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM28NPR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM58OB[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM58OBR[]    = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM58OBRM[]   = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM44N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM44NM[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM50NB[]     = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM23N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM04N[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM20F[]      = { { 0 } };
static GIMMICKSUBINFO s_aSubInfo_GMKM53F[]      = { { 0 } };


static const GIMMICKINFO s_infoSystemGimmick[] =
{
    { "GMSANDSWI",  CGimmickInfo::PARAMTYPE_SWITCH_AND, CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSANDSWI),    s_aSubInfo_GMSANDSWI    },
    { "GMSGROUP",   CGimmickInfo::PARAMTYPE_GROUP,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSGROUP),     s_aSubInfo_GMSGROUP     },
    { "GMSGCLEAR",  CGimmickInfo::PARAMTYPE_BASIC,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSGCLEAR),    s_aSubInfo_GMSGCLEAR    },
    { "GMSGOVER",   CGimmickInfo::PARAMTYPE_BASIC,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSGOVER),     s_aSubInfo_GMSGOVER     },
    { "GMSMOVIE",   CGimmickInfo::PARAMTYPE_MOVIE,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSMOVIE),     s_aSubInfo_GMSMOVIE     },
    { "GMSCAMERA",  CGimmickInfo::PARAMTYPE_BASIC,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSCAMERA),    s_aSubInfo_GMSCAMERA    },
    { "GMSEVCAM",   CGimmickInfo::PARAMTYPE_CAMERA,     CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSEVCAM),     s_aSubInfo_GMSEVCAM     },
    { "GMSSOUND",   CGimmickInfo::PARAMTYPE_SOUND,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSSOUND),     s_aSubInfo_GMSSOUND     },
    { "GMSWALL",    CGimmickInfo::PARAMTYPE_BASIC,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSWALL),      s_aSubInfo_GMSWALL      },
    { "GMSFALLPT",  CGimmickInfo::PARAMTYPE_BASIC,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSFALLPT),    s_aSubInfo_GMSFALLPT    },
    { "GMSRESET",   CGimmickInfo::PARAMTYPE_BASIC,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSRESET),     s_aSubInfo_GMSRESET     },
    { "GMSHELP",    CGimmickInfo::PARAMTYPE_HELP,       CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSHELP),      s_aSubInfo_GMSHELP      },
    { "GMSTUTO",    CGimmickInfo::PARAMTYPE_TUTO,       CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSTUTO),      s_aSubInfo_GMSTUTO      },
    { "GMSEKILL",   CGimmickInfo::PARAMTYPE_SWITCH,     CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMSEKILL),     s_aSubInfo_GMSEKILL     },
};


static const GIMMICKINFO s_infoEventGimmick[] =
{
    { "GMEPDEAD",   CGimmickInfo::PARAMTYPE_BASIC,              CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEPDEAD),     s_aSubInfo_GMEPDEAD     },
    { "GMETIME",    CGimmickInfo::PARAMTYPE_SWITCH_TIME,        CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMETIME),      s_aSubInfo_GMETIME      },
    { "GMEEBRK",    CGimmickInfo::PARAMTYPE_SWITCH_GENERATOR,   CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEEBRK),      s_aSubInfo_GMEEBRK      },
    { "GMEEBRKAL",  CGimmickInfo::PARAMTYPE_SWITCH_GENERATOR,   CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEEBRKAL),    s_aSubInfo_GMEEBRKAL    },
    { "GMEPWALL",   CGimmickInfo::PARAMTYPE_SWITCH_BOX,         CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEPWALL),     s_aSubInfo_GMEPWALL     },
    { "GMEPSPHER",  CGimmickInfo::PARAMTYPE_SWITCH_SPHERE,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEPSPHER),    s_aSubInfo_GMEPSPHER    },
    { "GMEPVOLUM",  CGimmickInfo::PARAMTYPE_SWITCH_BOX,         CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEPVOLUM),    s_aSubInfo_GMEPVOLUM    },
    { "GMEGBRK",    CGimmickInfo::PARAMTYPE_SWITCH_TARGET,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEGBRK),      s_aSubInfo_GMEGBRK      },
    { "GMEEWALL",   CGimmickInfo::PARAMTYPE_SWITCH_BOX,         CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEEWALL),     s_aSubInfo_GMEEWALL     },
    { "GMEUTRVOL",  CGimmickInfo::PARAMTYPE_SWITCH_BOX,         CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEUTRVOL),    s_aSubInfo_GMEUTRVOL    },
    { "GMEDONLSR",  CGimmickInfo::PARAMTYPE_SWITCH,             CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEDONLSR),    s_aSubInfo_GMEDONLSR    },
    { "GMERRSPHR",  CGimmickInfo::PARAMTYPE_SWITCH_SPHERE,      CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMERRSPHR),    s_aSubInfo_GMERRSPHR    },
    { "GMEFTWALL",  CGimmickInfo::PARAMTYPE_SWITCH_BOX,         CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMEFTWALL),    s_aSubInfo_GMEFTWALL    },
};


static const GIMMICKINFO s_infoGeneratorGimmick[] =
{
    { "GMGSET",     CGimmickInfo::PARAMTYPE_ENEMY_PLACE,    CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGSET),    s_aSubInfo_GMGSET       },
    { "GMGGROUND",  CGimmickInfo::PARAMTYPE_ENEMY_SINGLE,   CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGGROUND), s_aSubInfo_GMGGROUND    },
    { "GMGAIR",     CGimmickInfo::PARAMTYPE_ENEMY_SINGLE,   CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGAIR),    s_aSubInfo_GMGAIR       },
    { "GMGEGG",     CGimmickInfo::PARAMTYPE_ENEMY_SINGLE,   CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGEGG),    s_aSubInfo_GMGEGG       },
    { "GMGCAVE",    CGimmickInfo::PARAMTYPE_ENEMY_MULTIPLE, CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGCAVE),   s_aSubInfo_GMGCAVE      },
    { "GMGDOOR",    CGimmickInfo::PARAMTYPE_ENEMY_MULTIPLE, CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGDOOR),   s_aSubInfo_GMGDOOR      },
    { "GMGCAR",     CGimmickInfo::PARAMTYPE_ENEMY_MULTIPLE, CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGCAR),    s_aSubInfo_GMGCAR       },
    { "GMGSSHIPS",  CGimmickInfo::PARAMTYPE_ENEMY_MULTIPLE, CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMGSSHIPS), s_aSubInfo_GMGSSHIPS    },
};


static const GIMMICKINFO s_infoNormalGimmick[] =
{
    { "GMNTHROW",   CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNTHROW),  s_aSubInfo_GMNTHROW     },
    { "GMNITEMBX",  CGimmickInfo::PARAMTYPE_ITEMBOX,        CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNITEMBX), s_aSubInfo_GMNITEMBX    },
    { "GMNBRKCAR",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNBRKCAR), s_aSubInfo_GMNBRKCAR    },
    { "GMNBRKOBJ",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNBRKOBJ), s_aSubInfo_GMNBRKOBJ    },
    { "GMNROLOBJ",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNROLOBJ), s_aSubInfo_GMNROLOBJ    },
    { "GMNMVFLLI",  CGimmickInfo::PARAMTYPE_FLOOR_LINER,    CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNMVFLLI), s_aSubInfo_GMNMVFLLI    },
    { "GMNMVFLPA",  CGimmickInfo::PARAMTYPE_FLOOR_PATH,     CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNMVFLPA), s_aSubInfo_GMNMVFLPA    },
    { "GMNMVFLRO",  CGimmickInfo::PARAMTYPE_FLOOR_ROUND,    CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNMVFLRO), s_aSubInfo_GMNMVFLRO    },
    { "GMNCONSOL",  CGimmickInfo::PARAMTYPE_SWITCH,         CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNCONSOL), s_aSubInfo_GMNCONSOL    },
    { "GMNSWITCH",  CGimmickInfo::PARAMTYPE_SWITCH,         CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNSWITCH), s_aSubInfo_GMNSWITCH    },
    { "GMNSWIPIL",  CGimmickInfo::PARAMTYPE_SWITCH,         CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNSWIPIL), s_aSubInfo_GMNSWIPIL    },
    { "GMNBRKPIL",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNBRKPIL), s_aSubInfo_GMNBRKPIL    },
    { "GMNSHKPIL",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNSHKPIL), s_aSubInfo_GMNSHKPIL    },
    { "GMNNORPIL",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNNORPIL), s_aSubInfo_GMNNORPIL    },
    { "GMNRAPROC",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNRAPROC), s_aSubInfo_GMNRAPROC    },
    { "GMNLEOS",    CGimmickInfo::PARAMTYPE_AREA_BOX,       CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNLEOS),   s_aSubInfo_GMNLEOS      },
    { "GMNLEOM",    CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTALPHAMAP, COUNT_OF(s_aSubInfo_GMNLEOM),   s_aSubInfo_GMNLEOM      },
    { "GMNLEOL",    CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNLEOL),   s_aSubInfo_GMNLEOL      },
    { "GMNGRASS",   CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNGRASS),  s_aSubInfo_GMNGRASS     },
    { "GMNCRYSTL",  CGimmickInfo::PARAMTYPE_SWITCH,         CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNCRYSTL), s_aSubInfo_GMNCRYSTL    },
    { "GMNGUILLO",  CGimmickInfo::PARAMTYPE_GUILLOTINE,     CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNGUILLO), s_aSubInfo_GMNGUILLO    },
    { "GMNSEESAW",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNSEESAW), s_aSubInfo_GMNSEESAW    },
    { "GMNFALOBJ",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTALPHAMAP, COUNT_OF(s_aSubInfo_GMNFALOBJ), s_aSubInfo_GMNFALOBJ    },
    { "GMNMETEO",   CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNMETEO),  s_aSubInfo_GMNMETEO     },
    { "GMNDOOR",    CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNDOOR),   s_aSubInfo_GMNDOOR      },
    { "GMNUTROM",   CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNUTROM),  s_aSubInfo_GMNUTROM     },
    { "GMNLASER",   CGimmickInfo::PARAMTYPE_AREA_BOX,       CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNLASER),  s_aSubInfo_GMNLASER     },
    { "GMNMINE",    CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMNMINE),   s_aSubInfo_GMNMINE      },
    { "GMNBELCON",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNBELCON), s_aSubInfo_GMNBELCON    },
    { "GMNSTOPER",  CGimmickInfo::PARAMTYPE_BASIC,          CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMNSTOPER), s_aSubInfo_GMNSTOPER    },
};


static const GIMMICKINFO s_infoRideGimmick[] =
{
    { "GMRFLAG",    CGimmickInfo::PARAMTYPE_BASIC, CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMRFLAG),   s_aSubInfo_GMRFLAG      },
    { "GMRTHNDER",  CGimmickInfo::PARAMTYPE_BASIC, CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMRTHNDER), s_aSubInfo_GMRTHNDER    },
    { "GMRASTRID",  CGimmickInfo::PARAMTYPE_BASIC, CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMRASTRID), s_aSubInfo_GMRASTRID    },
    { "GMRSHUTTR",  CGimmickInfo::PARAMTYPE_BASIC, CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMRSHUTTR), s_aSubInfo_GMRSHUTTR    },
};


static const GIMMICKINFO s_infoOtherGimmick[] =
{
    { "GMNITEM",  CGimmickInfo::PARAMTYPE_BASIC, CGimmickInfo::DRAWPRI_POSTCHARA, COUNT_OF(s_aSubInfo_GMNITEM), s_aSubInfo_GMNITEM    },
};


static const GIMMICKINFO s_infoMapGimmick[] =
{
    { "GMNLIGHT",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNLIGHT),      s_aSubInfo_GMNLIGHT      },
    { "GMNWEATHER",  CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNWEATHER),    s_aSubInfo_GMNWEATHER    },
    { "GMNSMOKE",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNSMOKE),      s_aSubInfo_GMNSMOKE      },
    { "GMNFLAME",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNFLAME),      s_aSubInfo_GMNFLAME      },
    { "GMNSTEAM",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNSTEAM),      s_aSubInfo_GMNSTEAM      },
    { "GMNSPARK",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNSPARK),      s_aSubInfo_GMNSPARK      },
    { "GMNMOTH",     CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNMOTH),       s_aSubInfo_GMNMOTH       },
    { "GMNFISH",     CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNFISH),       s_aSubInfo_GMNFISH       },
    { "GMNBUBBLE",   CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNBUBBLE),     s_aSubInfo_GMNBUBBLE     },
    { "GMNMIST",     CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNMIST),       s_aSubInfo_GMNMIST       },
    { "GMNSPLASH",   CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNSPLASH),     s_aSubInfo_GMNSPLASH     },
    { "GMNEXP",      CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,     COUNT_OF(s_aSubInfo_GMNEXP),        s_aSubInfo_GMNEXP        },
};


static const GIMMICKINFO s_infoKoyuuGimmick[] =
{
    { "GMKM12N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMKM12N),   s_aSubInfo_GMKM12N      },
    { "GMKM02N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM02N),   s_aSubInfo_GMKM02N      },
    { "GMKM05N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM05N),   s_aSubInfo_GMKM05N      },
    { "GMKM38B",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM38B),   s_aSubInfo_GMKM38B      },
    { "GMKM38MAN",  CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM38MAN), s_aSubInfo_GMKM38MAN    },
    { "GMKM17N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM17N),   s_aSubInfo_GMKM17N      },
    { "GMKM43N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM43N),   s_aSubInfo_GMKM43N      },
    { "GMKM28NGR",  CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM28NGR), s_aSubInfo_GMKM28NGR    },
    { "GMKM28NPT",  CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_PRECHARA,     COUNT_OF(s_aSubInfo_GMKM28NPT), s_aSubInfo_GMKM28NPT    },
    { "GMKM28NPR",  CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM28NPR), s_aSubInfo_GMKM28NPR    },
    { "GMKM58OB",   CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM58OB),  s_aSubInfo_GMKM58OB     },
    { "GMKM58OBR",  CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM58OBR), s_aSubInfo_GMKM58OBR    },
    { "GMKM58OBRM", CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM58OBRM),s_aSubInfo_GMKM58OBRM   },
    { "GMKM44N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM44N),   s_aSubInfo_GMKM44N      },
    { "GMKM44NM",   CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM44NM),  s_aSubInfo_GMKM44NM     },
    { "GMKM50NB",   CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM50NB),  s_aSubInfo_GMKM50NB     },
    { "GMKM23N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM23N),   s_aSubInfo_GMKM23N      },
    { "GMKM04N",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM04N),   s_aSubInfo_GMKM04N      },
    { "GMKM20F",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM20F),   s_aSubInfo_GMKM20F      },
    { "GMKM53F",    CGimmickInfo::PARAMTYPE_BASIC,  CGimmickInfo::DRAWPRI_POSTCHARA,    COUNT_OF(s_aSubInfo_GMKM53F),   s_aSubInfo_GMKM53F      },
};


static const GIMMICKINFO s_infoDummyGimmick[] =
{
    { "", }
};


static const GIMMICKINFOTABLE s_infoTableSystem =
{
    GIMMICKID::SYSTEMSTART,    
    GIMMICKID::SYSTEMEND - GIMMICKID::SYSTEMSTART,
    s_infoSystemGimmick
};


static const GIMMICKINFOTABLE s_infoTableEvent =
{
    GIMMICKID::EVENTSTART,
    GIMMICKID::EVENTEND - GIMMICKID::EVENTSTART,
    s_infoEventGimmick
};


static const GIMMICKINFOTABLE s_infoTableGenerator =
{
    GIMMICKID::GENERATORSTART,
    GIMMICKID::GENERATOREND - GIMMICKID::GENERATORSTART,
    s_infoGeneratorGimmick
};


static const GIMMICKINFOTABLE s_infoTableNormal =
{
    GIMMICKID::NORMALSTART,
    GIMMICKID::NORMALEND - GIMMICKID::NORMALSTART,
    s_infoNormalGimmick
};


static const GIMMICKINFOTABLE s_infoTableRide =
{
    GIMMICKID::RIDESTART,
    GIMMICKID::RIDEEND - GIMMICKID::RIDESTART,
    s_infoRideGimmick
};


static const GIMMICKINFOTABLE s_infoTableOther =
{
    GIMMICKID::OTHERSTART,
    GIMMICKID::OTHEREND - GIMMICKID::OTHERSTART,
    s_infoOtherGimmick
};


static const GIMMICKINFOTABLE s_infoTableMap =
{
    GIMMICKID::MAPSTART,
    GIMMICKID::MAPEND - GIMMICKID::MAPSTART,
    s_infoMapGimmick
};


static const GIMMICKINFOTABLE s_infoTableKoyuu =
{
    GIMMICKID::KOYUUSTART,
    GIMMICKID::KOYUUEND - GIMMICKID::KOYUUSTART,
    s_infoKoyuuGimmick
};


static const GIMMICKINFOTABLE s_infoTableDummy =
{
    GIMMICKID::ID_INVALID,
    0,
    s_infoDummyGimmick
};


static inline const GIMMICKINFOTABLE& GimmickInfoTable(GIMMICKID::VALUE idGimmick)
{
    if (idGimmick >= GIMMICKID::SYSTEMSTART && idGimmick <= GIMMICKID::SYSTEMEND)
    {
        return s_infoTableSystem;
    }
    else if (idGimmick >= GIMMICKID::EVENTSTART && idGimmick <= GIMMICKID::EVENTEND)
    {
        return s_infoTableEvent;
    }
    else if (idGimmick >= GIMMICKID::GENERATORSTART && idGimmick <= GIMMICKID::GENERATOREND)
    {
        return s_infoTableGenerator;
    }
    else if (idGimmick >= GIMMICKID::NORMALSTART && idGimmick <= GIMMICKID::NORMALEND)
    {
        return s_infoTableNormal;
    }
    else if (idGimmick >= GIMMICKID::RIDESTART && idGimmick <= GIMMICKID::RIDEEND)
    {
        return s_infoTableRide;
    }
    else if (idGimmick >= GIMMICKID::OTHERSTART && idGimmick <= GIMMICKID::OTHEREND)
    {
        return s_infoTableOther;
    }
    else if (idGimmick >= GIMMICKID::MAPSTART && idGimmick <= GIMMICKID::MAPEND)
    {
        return s_infoTableMap;
    }
    else if (idGimmick >= GIMMICKID::KOYUUSTART && idGimmick <= GIMMICKID::KOYUUEND)
    {
        return s_infoTableKoyuu;
    }
    else
    {
        ASSERT(false);
        return s_infoTableDummy;
    };
};


static inline const GIMMICKINFO& GimmickInfo(GIMMICKID::VALUE idGimmick)
{
	const GIMMICKINFO* pInfoArray = GimmickInfoTable(idGimmick).m_pInfoArray;
	GIMMICKID::VALUE idStart = GimmickInfoTable(idGimmick).m_idStart;	
	ASSERT(idStart <= idGimmick);
	return pInfoArray[idGimmick - idStart];
};


/*static*/ void CGimmickInfo::MakeName(char* pszGimmickName, GIMMICKID::VALUE idGimmick, int32 subid, int32 no)
{
    ASSERT(pszGimmickName);
    ASSERT(subid >= 0 && subid < 26);
    ASSERT(no >= 0 && no < 1000);

    const char* pszBaseName = GetBaseName(idGimmick);
    ASSERT(pszBaseName);

    std::sprintf(
        pszGimmickName,
        "%s_%c%03d",
        pszBaseName,
        subid + 85,
        no
    );
};


/*static*/ const char* CGimmickInfo::GetBaseName(GIMMICKID::VALUE idGimmick)
{
    return GimmickInfo(idGimmick).m_pszBaseName;
};


/*static*/ CGimmickInfo::DRAWPRI CGimmickInfo::GetDrawPriority(GIMMICKID::VALUE idGimmick)
{
    return GimmickInfo(idGimmick).m_drawpri;
};
