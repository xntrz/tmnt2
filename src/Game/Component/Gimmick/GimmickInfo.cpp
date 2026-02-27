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
static GIMMICKSUBINFO s_aSubInfo_GMSANDSWI[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSGROUP[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSGCLEAR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSGOVER[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSMOVIE[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSCAMERA[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSEVCAM[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSSOUND[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSWALL[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSFALLPT[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSRESET[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSHELP[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSTUTO[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMSEKILL[]     = { {} };


//
//  EVENT
//
static GIMMICKSUBINFO s_aSubInfo_GMEPDEAD []    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMETIME[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEEBRK[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEEBRKAL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEPWALL[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEPSPHER[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEPVOLUM[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEGBRK[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEEWALL[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEUTRVOL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEDONLSR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMERRSPHR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMEFTWALL[]    = { {} };


//
//  GENERATOR
//
static GIMMICKSUBINFO s_aSubInfo_GMGSET[]       = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGGROUND[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGAIR[]       = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGEGG[]       = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGCAVE[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGDOOR[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGCAR[]       = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMGSSHIPS[]    = { {} };


//
//  NORMAL
//
static GIMMICKSUBINFO s_aSubInfo_GMNTHROW[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNITEMBX[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNBRKCAR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNBRKOBJ[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNROLOBJ[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMVFLLI[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMVFLPA[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMVFLRO[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNCONSOL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSWITCH[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSWIPIL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNBRKPIL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSHKPIL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNNORPIL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNRAPROC[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNLEOS[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNLEOM[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNLEOL[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNGRASS[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNCRYSTL[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNGUILLO[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSEESAW[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNFALOBJ[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMETEO[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNDOOR[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNUTROM[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNLASER[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMINE[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNBELCON[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSTOPER[]    = { {} };


//
//  RIDE
//
static GIMMICKSUBINFO s_aSubInfo_GMRFLAG[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMRTHNDER[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMRASTRID[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMRSHUTTR[]    = { {} };


//
//  OTHER
//
static GIMMICKSUBINFO s_aSubInfo_GMNITEM[]      = { {} };


//
//  MAP
//
static GIMMICKSUBINFO s_aSubInfo_GMNLIGHT[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNWEATHER[]   = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSMOKE[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNFLAME[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSTEAM[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSPARK[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMOTH[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNFISH[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNBUBBLE[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNMIST[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNSPLASH[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMNEXP[]       = { {} };


//
//  KOYUU
//
static GIMMICKSUBINFO s_aSubInfo_GMKM12N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM02N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM05N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM38B[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM38MAN[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM17N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM43N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM28NGR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM28NPT[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM28NPR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM58OB[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM58OBR[]    = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM58OBRM[]   = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM44N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM44NM[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM50NB[]     = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM23N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM04N[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM20F[]      = { {} };
static GIMMICKSUBINFO s_aSubInfo_GMKM53F[]      = { {} };


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
    { "", CGimmickInfo::PARAMTYPE_NONE, CGimmickInfo::DRAWPRI_PRECHARA, 0, nullptr },

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
