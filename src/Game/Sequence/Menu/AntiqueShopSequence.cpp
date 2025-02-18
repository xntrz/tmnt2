#include "AntiqueShopSequence.hpp"

#include "Game/Component/Menu/Dialog.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AntiqueID.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/SystemTime.hpp"
#include "System/Common/TextData.hpp"


static float s_fAntiqueFontScale = 2.0f;


static int32 s_aAntiqueMap[7][10] =
{
    // FOOT
    { ANTIQUEID::ID_AT01, ANTIQUEID::ID_AT02, ANTIQUEID::ID_AT47, ANTIQUEID::ID_AT48, ANTIQUEID::ID_AT49, ANTIQUEID::ID_AT50, ANTIQUEID::ID_AT51, ANTIQUEID::ID_AT52, ANTIQUEID::ID_AT53, ANTIQUEID::ID_AT54 },

    // OTHER
    { ANTIQUEID::ID_AT03, ANTIQUEID::ID_AT04, ANTIQUEID::ID_AT05, ANTIQUEID::ID_AT09, ANTIQUEID::ID_AT10, ANTIQUEID::ID_AT11, ANTIQUEID::ID_AT28, ANTIQUEID::ID_AT55, -1, -1 },

    // UTROM
    { ANTIQUEID::ID_AT06, ANTIQUEID::ID_AT07, ANTIQUEID::ID_AT08, ANTIQUEID::ID_AT35, ANTIQUEID::ID_AT36, -1, -1, -1, -1, -1 },

    // D'HOONNIB
    { ANTIQUEID::ID_AT12, ANTIQUEID::ID_AT13, ANTIQUEID::ID_AT14, ANTIQUEID::ID_AT15, ANTIQUEID::ID_AT16, ANTIQUEID::ID_AT17, ANTIQUEID::ID_AT18, ANTIQUEID::ID_AT19, ANTIQUEID::ID_AT20, ANTIQUEID::ID_AT21 },

    // TRICERATOR
    { ANTIQUEID::ID_AT22, ANTIQUEID::ID_AT23, ANTIQUEID::ID_AT24, ANTIQUEID::ID_AT25, ANTIQUEID::ID_AT26, ANTIQUEID::ID_AT27, -1, -1, -1, -1 },

    // JAPAN
    { ANTIQUEID::ID_AT29, ANTIQUEID::ID_AT30, ANTIQUEID::ID_AT31, ANTIQUEID::ID_AT32, ANTIQUEID::ID_AT33, ANTIQUEID::ID_AT34, -1, -1, -1, -1 },

    // NY
    { ANTIQUEID::ID_AT37, ANTIQUEID::ID_AT38, ANTIQUEID::ID_AT39, ANTIQUEID::ID_AT40, ANTIQUEID::ID_AT41, ANTIQUEID::ID_AT42, ANTIQUEID::ID_AT43, ANTIQUEID::ID_AT44, ANTIQUEID::ID_AT45, ANTIQUEID::ID_AT46 },
};


static int32 s_aAntiquePrice[ANTIQUEID::ID_MAX] =
{
    0,  // ANTIQUEID::ID_NONE
    3,  // ANTIQUEID::ID_AT01
    3,  // ANTIQUEID::ID_AT02
    2,  // ANTIQUEID::ID_AT03
    3,  // ANTIQUEID::ID_AT04
    3,  // ANTIQUEID::ID_AT05
    2,  // ANTIQUEID::ID_AT06
    2,  // ANTIQUEID::ID_AT07
    2,  // ANTIQUEID::ID_AT08
    3,  // ANTIQUEID::ID_AT09
    3,  // ANTIQUEID::ID_AT10
    2,  // ANTIQUEID::ID_AT11
    2,  // ANTIQUEID::ID_AT12
    3,  // ANTIQUEID::ID_AT13
    2,  // ANTIQUEID::ID_AT14
    2,  // ANTIQUEID::ID_AT15
    3,  // ANTIQUEID::ID_AT16
    3,  // ANTIQUEID::ID_AT17
    3,  // ANTIQUEID::ID_AT18
    3,  // ANTIQUEID::ID_AT19
    2,  // ANTIQUEID::ID_AT20
    3,  // ANTIQUEID::ID_AT21
    2,  // ANTIQUEID::ID_AT22
    3,  // ANTIQUEID::ID_AT23
    1,  // ANTIQUEID::ID_AT24
    2,  // ANTIQUEID::ID_AT25
    1,  // ANTIQUEID::ID_AT26
    2,  // ANTIQUEID::ID_AT27
    3,  // ANTIQUEID::ID_AT28
    4,  // ANTIQUEID::ID_AT29
    4,  // ANTIQUEID::ID_AT30
    4,  // ANTIQUEID::ID_AT31
    1,  // ANTIQUEID::ID_AT32
    2,  // ANTIQUEID::ID_AT33
    4,  // ANTIQUEID::ID_AT34
    1,  // ANTIQUEID::ID_AT35
    3,  // ANTIQUEID::ID_AT36
    2,  // ANTIQUEID::ID_AT37
    2,  // ANTIQUEID::ID_AT38
    3,  // ANTIQUEID::ID_AT39
    2,  // ANTIQUEID::ID_AT40
    5,  // ANTIQUEID::ID_AT41
    5,  // ANTIQUEID::ID_AT42
    2,  // ANTIQUEID::ID_AT43
    3,  // ANTIQUEID::ID_AT44
    3,  // ANTIQUEID::ID_AT45
    3,  // ANTIQUEID::ID_AT46
    4,  // ANTIQUEID::ID_AT47
    1,  // ANTIQUEID::ID_AT48
    2,  // ANTIQUEID::ID_AT49
    3,  // ANTIQUEID::ID_AT50
    2,  // ANTIQUEID::ID_AT51
    1,  // ANTIQUEID::ID_AT52
    5,  // ANTIQUEID::ID_AT53
    4,  // ANTIQUEID::ID_AT54
    5,  // ANTIQUEID::ID_AT55
};


static_assert(COUNT_OF(s_aAntiquePrice) == ANTIQUEID::ID_MAX, "update table");


static const char* s_apszAprilFace[] =
{
    "shop_aprilCPS",
    "shop_aprilEPS",
    "shop_aprilDPS",
    "shop_aprilCPS",
    "shop_aprilBPS",
    "shop_aprilAPS",
};


static const char* s_apszAntiqueTexOrg[] =
{
#ifdef TMNT2_BUILD_EU
    "Antique_019",
    "Antique_021",
    "Antique_023",
    "Antique_025",
    "Antique_060",
    "Antique_062",
    "Antique_064",
    "Antique_066",
    "Antique_068",
    "Antique_070",
    "Antique_095",
    "Antique_097",
    "Antique_099",
    "Antique_101",
    "Antique_103",
    "Antique_105",
    "Antique_072",
    "Antique_074",
    "Antique_076",
    "Antique_078",
    "Antique_080",
    "Antique_082",
    "Antique_116",
    "Antique_118",
    "Antique_120",
    "Antique_122",
    "Antique_124",
    "Antique_126",
    "Antique_128",
    "Antique_130",
    "Antique_132",
    "Antique_134",
    "Antique_144",
    "Antique_146",
    "Antique_148",
    "Antique_150",
    "Antique_152",
    "Antique_154",
    "Antique_136",
    "Antique_138",
    "Antique_140",
    "Antique_142",
    "Antique_157",
    "Antique_159",
    "Antique_161",
    "Antique_163",
    "Antique_165",
    "Antique_167",
    "Antique_169",
    "Antique_171",
    "Antique_173",
    "Antique_175",
    "Antique_177",
    "Antique_179",
    "Antique_084",
    "Antique_086",
    "Antique_182",
    "Antique_184",
    "Antique_186",
    "Antique_188",
    "Antique_190",
    "Antique_192",
    "Antique_194",
    "Antique_196",
    "Antique_198",
    "Antique_200",
    "Antique_202",
    "Antique_204",
    "Antique_107",
    "Antique_109",
    "Antique_111",
    "Antique_113",
    "Antique_207",
    "Antique_209",
    "Antique_211",
    "Antique_213",
    "Antique_215",
    "Antique_217",
    "Antique_219",
    "Antique_221",
    "Antique_223",
    "Antique_225",
    "Antique_227",
    "Antique_229",
    "Antique_231",
    "Antique_233",
    "Antique_235",
    "Antique_237",
    "Antique_239",
    "Antique_241",
    "Antique_243",
    "Antique_245",
    "Antique_027",
    "Antique_029",
    "Antique_031",
    "Antique_033",
    "Antique_035",
    "Antique_037",
    "Antique_039",
    "Antique_041",
    "Antique_043",
    "Antique_045",
    "Antique_047",
    "Antique_049",
    "Antique_051",
    "Antique_053",
    "Antique_055",
    "Antique_057",
    "Antique_088",
    "Antique_090",
#else /* TMNT2_BUILD_EU */    
    "Antique_017",
    "Antique_019",
    "Antique_021",
    "Antique_023",
    "Antique_058",
    "Antique_060",
    "Antique_062",
    "Antique_064",
    "Antique_066",
    "Antique_068",
    "Antique_093",
    "Antique_095",
    "Antique_097",
    "Antique_099",
    "Antique_101",
    "Antique_103",
    "Antique_070",
    "Antique_072",
    "Antique_074",
    "Antique_076",
    "Antique_078",
    "Antique_080",
    "Antique_114",
    "Antique_116",
    "Antique_118",
    "Antique_120",
    "Antique_122",
    "Antique_124",
    "Antique_126",
    "Antique_128",
    "Antique_130",
    "Antique_132",
    "Antique_142",
    "Antique_144",
    "Antique_146",
    "Antique_148",
    "Antique_150",
    "Antique_152",
    "Antique_134",
    "Antique_136",
    "Antique_138",
    "Antique_140",
    "Antique_155",
    "Antique_157",
    "Antique_159",
    "Antique_161",
    "Antique_163",
    "Antique_165",
    "Antique_167",
    "Antique_169",
    "Antique_171",
    "Antique_173",
    "Antique_175",
    "Antique_177",
    "Antique_082",
    "Antique_084",
    "Antique_180",
    "Antique_182",
    "Antique_184",
    "Antique_186",
    "Antique_188",
    "Antique_190",
    "Antique_192",
    "Antique_194",
    "Antique_196",
    "Antique_198",
    "Antique_200",
    "Antique_202",
    "Antique_105",
    "Antique_107",
    "Antique_109",
    "Antique_111",
    "Antique_205",
    "Antique_207",
    "Antique_209",
    "Antique_211",
    "Antique_213",
    "Antique_215",
    "Antique_217",
    "Antique_219",
    "Antique_221",
    "Antique_223",
    "Antique_225",
    "Antique_227",
    "Antique_229",
    "Antique_231",
    "Antique_233",
    "Antique_235",
    "Antique_237",
    "Antique_239",
    "Antique_241",
    "Antique_243",
    "Antique_025",
    "Antique_027",
    "Antique_029",
    "Antique_031",
    "Antique_033",
    "Antique_035",
    "Antique_037",
    "Antique_039",
    "Antique_041",
    "Antique_043",
    "Antique_045",
    "Antique_047",
    "Antique_049",
    "Antique_051",
    "Antique_053",
    "Antique_055",
    "Antique_086",
    "Antique_088",
#endif /* TMNT2_BUILD_EU */
};


static const char* s_apszAntiqueTex[] =
{
    nullptr,
    "shop_A01BPS",
    "shop_A02BPS",
    "shop_A03BPS",
    "shop_A04BPS",
    "shop_A05BPS",
    "shop_A06BPS",
    "shop_A07BPS",
    "shop_A08BPS",
    "shop_A09BPS",
    "shop_A10BPS",
    "shop_A11BPS",
    "shop_A12BPS",
    "shop_A13BPS",
    "shop_A14BPS",
    "shop_A15BPS",
    "shop_A16BPS",
    "shop_A17BPS",
    "shop_A18BPS",
    "shop_A19BPS",
    "shop_A20BPS",
    "shop_A21BPS",
    "shop_A22BPS",
    "shop_A23BPS",
    "shop_A24BPS",
    "shop_A25BPS",
    "shop_A26BPS",
    "shop_A27BPS",
    "shop_A28BPS",
    "shop_A29BPS",
    "shop_A30BPS",
    "shop_A31BPS",
    "shop_A32BPS",
    "shop_A33BPS",
    "shop_A34BPS",
    "shop_A35BPS",
    "shop_A36BPS",
    "shop_A37BPS",
    "shop_A38BPS",
    "shop_A39BPS",
    "shop_A40BPS",
    "shop_A41BPS",
    "shop_A42BPS",
    "shop_A43BPS",
    "shop_A44BPS",
    "shop_A45BPS",
    "shop_A46BPS",
    "shop_A47BPS",
    "shop_A48BPS",
    "shop_A49BPS",
    "shop_A50BPS",
    "shop_A51BPS",
    "shop_A52BPS",
    "shop_A53BPS",
    "shop_A54BPS",
    "shop_A55BPS",
};


static_assert(COUNT_OF(s_apszAntiqueTex) == ANTIQUEID::ID_MAX, "update table");


static GAMETEXT s_aAntiqueText[] =
{
    /* ANTIQUEID::ID_NONE */    GAMETEXT_EMPTY,
    /* ANTIQUEID::ID_AT01 */    GAMETEXT_AT_01,
    /* ANTIQUEID::ID_AT02 */    GAMETEXT_AT_02,
    /* ANTIQUEID::ID_AT03 */    GAMETEXT_AT_03,
    /* ANTIQUEID::ID_AT04 */    GAMETEXT_AT_04,
    /* ANTIQUEID::ID_AT05 */    GAMETEXT_AT_05,
    /* ANTIQUEID::ID_AT06 */    GAMETEXT_AT_06,
    /* ANTIQUEID::ID_AT07 */    GAMETEXT_AT_07,
    /* ANTIQUEID::ID_AT08 */    GAMETEXT_AT_08,
    /* ANTIQUEID::ID_AT09 */    GAMETEXT_AT_09,
    /* ANTIQUEID::ID_AT10 */    GAMETEXT_AT_10,
    /* ANTIQUEID::ID_AT11 */    GAMETEXT_AT_11,
    /* ANTIQUEID::ID_AT12 */    GAMETEXT_AT_12,
    /* ANTIQUEID::ID_AT13 */    GAMETEXT_AT_13,
    /* ANTIQUEID::ID_AT14 */    GAMETEXT_AT_14,
    /* ANTIQUEID::ID_AT15 */    GAMETEXT_AT_15,
    /* ANTIQUEID::ID_AT16 */    GAMETEXT_AT_16,
    /* ANTIQUEID::ID_AT17 */    GAMETEXT_AT_17,
    /* ANTIQUEID::ID_AT18 */    GAMETEXT_AT_18,
    /* ANTIQUEID::ID_AT19 */    GAMETEXT_AT_19,
    /* ANTIQUEID::ID_AT20 */    GAMETEXT_AT_20,
    /* ANTIQUEID::ID_AT21 */    GAMETEXT_AT_21,
    /* ANTIQUEID::ID_AT22 */    GAMETEXT_AT_22,
    /* ANTIQUEID::ID_AT23 */    GAMETEXT_AT_23,
    /* ANTIQUEID::ID_AT24 */    GAMETEXT_AT_24,
    /* ANTIQUEID::ID_AT25 */    GAMETEXT_AT_25,
    /* ANTIQUEID::ID_AT26 */    GAMETEXT_AT_26,
    /* ANTIQUEID::ID_AT27 */    GAMETEXT_AT_27,
    /* ANTIQUEID::ID_AT28 */    GAMETEXT_AT_28,
    /* ANTIQUEID::ID_AT29 */    GAMETEXT_AT_29,
    /* ANTIQUEID::ID_AT30 */    GAMETEXT_AT_30,
    /* ANTIQUEID::ID_AT31 */    GAMETEXT_AT_31,
    /* ANTIQUEID::ID_AT32 */    GAMETEXT_AT_32,
    /* ANTIQUEID::ID_AT33 */    GAMETEXT_AT_33,
    /* ANTIQUEID::ID_AT34 */    GAMETEXT_AT_34,
    /* ANTIQUEID::ID_AT35 */    GAMETEXT_AT_35,
    /* ANTIQUEID::ID_AT36 */    GAMETEXT_AT_36,
    /* ANTIQUEID::ID_AT37 */    GAMETEXT_AT_37,
    /* ANTIQUEID::ID_AT38 */    GAMETEXT_AT_38,
    /* ANTIQUEID::ID_AT39 */    GAMETEXT_AT_39,
    /* ANTIQUEID::ID_AT40 */    GAMETEXT_AT_40,
    /* ANTIQUEID::ID_AT41 */    GAMETEXT_AT_41,
    /* ANTIQUEID::ID_AT42 */    GAMETEXT_AT_42,
    /* ANTIQUEID::ID_AT43 */    GAMETEXT_AT_43,
    /* ANTIQUEID::ID_AT44 */    GAMETEXT_AT_44,
    /* ANTIQUEID::ID_AT45 */    GAMETEXT_AT_45,
    /* ANTIQUEID::ID_AT46 */    GAMETEXT_AT_46,
    /* ANTIQUEID::ID_AT47 */    GAMETEXT_AT_47,
    /* ANTIQUEID::ID_AT48 */    GAMETEXT_AT_48,
    /* ANTIQUEID::ID_AT49 */    GAMETEXT_AT_49,
    /* ANTIQUEID::ID_AT50 */    GAMETEXT_AT_50,
    /* ANTIQUEID::ID_AT51 */    GAMETEXT_AT_51,
    /* ANTIQUEID::ID_AT52 */    GAMETEXT_AT_52,
    /* ANTIQUEID::ID_AT53 */    GAMETEXT_AT_53,
    /* ANTIQUEID::ID_AT54 */    GAMETEXT_AT_54,
    /* ANTIQUEID::ID_AT55 */    GAMETEXT_AT_55,
};


static_assert(COUNT_OF(s_aAntiqueText) == ANTIQUEID::ID_MAX, "update table");


static GAMETEXT s_aAntiqueComment[] =
{
    /* ANTIQUEID::ID_NONE */    GAMETEXT(0x0),
    /* ANTIQUEID::ID_AT01 */    GAMETEXT(0x427),
    /* ANTIQUEID::ID_AT02 */    GAMETEXT(0x428),
    /* ANTIQUEID::ID_AT03 */    GAMETEXT(0x429),
    /* ANTIQUEID::ID_AT04 */    GAMETEXT(0x42A),
    /* ANTIQUEID::ID_AT05 */    GAMETEXT(0x42B),
    /* ANTIQUEID::ID_AT06 */    GAMETEXT(0x42C),
    /* ANTIQUEID::ID_AT07 */    GAMETEXT(0x42D),
    /* ANTIQUEID::ID_AT08 */    GAMETEXT(0x42E),
    /* ANTIQUEID::ID_AT09 */    GAMETEXT(0x42F),
    /* ANTIQUEID::ID_AT10 */    GAMETEXT(0x430),
    /* ANTIQUEID::ID_AT11 */    GAMETEXT(0x431),
    /* ANTIQUEID::ID_AT12 */    GAMETEXT(0x432),
    /* ANTIQUEID::ID_AT13 */    GAMETEXT(0x433),
    /* ANTIQUEID::ID_AT14 */    GAMETEXT(0x434),
    /* ANTIQUEID::ID_AT15 */    GAMETEXT(0x435),
    /* ANTIQUEID::ID_AT16 */    GAMETEXT(0x436),
    /* ANTIQUEID::ID_AT17 */    GAMETEXT(0x437),
    /* ANTIQUEID::ID_AT18 */    GAMETEXT(0x438),
    /* ANTIQUEID::ID_AT19 */    GAMETEXT(0x439),
    /* ANTIQUEID::ID_AT20 */    GAMETEXT(0x43A),
    /* ANTIQUEID::ID_AT21 */    GAMETEXT(0x43B),
    /* ANTIQUEID::ID_AT22 */    GAMETEXT(0x43C),
    /* ANTIQUEID::ID_AT23 */    GAMETEXT(0x43D),
    /* ANTIQUEID::ID_AT24 */    GAMETEXT(0x43E),
    /* ANTIQUEID::ID_AT25 */    GAMETEXT(0x43F),
    /* ANTIQUEID::ID_AT26 */    GAMETEXT(0x440),
    /* ANTIQUEID::ID_AT27 */    GAMETEXT(0x441),
    /* ANTIQUEID::ID_AT28 */    GAMETEXT(0x442),
    /* ANTIQUEID::ID_AT29 */    GAMETEXT(0x443),
    /* ANTIQUEID::ID_AT30 */    GAMETEXT(0x444),
    /* ANTIQUEID::ID_AT31 */    GAMETEXT(0x445),
    /* ANTIQUEID::ID_AT32 */    GAMETEXT(0x446),
    /* ANTIQUEID::ID_AT33 */    GAMETEXT(0x447),
    /* ANTIQUEID::ID_AT34 */    GAMETEXT(0x448),
    /* ANTIQUEID::ID_AT35 */    GAMETEXT(0x449),
    /* ANTIQUEID::ID_AT36 */    GAMETEXT(0x44A),
    /* ANTIQUEID::ID_AT37 */    GAMETEXT(0x44B),
    /* ANTIQUEID::ID_AT38 */    GAMETEXT(0x44C),
    /* ANTIQUEID::ID_AT39 */    GAMETEXT(0x44D),
    /* ANTIQUEID::ID_AT40 */    GAMETEXT(0x44E),
    /* ANTIQUEID::ID_AT41 */    GAMETEXT(0x44F),
    /* ANTIQUEID::ID_AT42 */    GAMETEXT(0x450),
    /* ANTIQUEID::ID_AT43 */    GAMETEXT(0x451),
    /* ANTIQUEID::ID_AT44 */    GAMETEXT(0x452),
    /* ANTIQUEID::ID_AT45 */    GAMETEXT(0x453),
    /* ANTIQUEID::ID_AT46 */    GAMETEXT(0x454),
    /* ANTIQUEID::ID_AT47 */    GAMETEXT(0x455),
    /* ANTIQUEID::ID_AT48 */    GAMETEXT(0x456),
    /* ANTIQUEID::ID_AT49 */    GAMETEXT(0x457),
    /* ANTIQUEID::ID_AT50 */    GAMETEXT(0x458),
    /* ANTIQUEID::ID_AT51 */    GAMETEXT(0x459),
    /* ANTIQUEID::ID_AT52 */    GAMETEXT(0x45A),
    /* ANTIQUEID::ID_AT53 */    GAMETEXT(0x45B),
    /* ANTIQUEID::ID_AT54 */    GAMETEXT(0x45C),
    /* ANTIQUEID::ID_AT55 */    GAMETEXT(0x45D),
};


static_assert(COUNT_OF(s_aAntiqueComment) == ANTIQUEID::ID_MAX, "update table");


#ifdef TMNT2_BUILD_EU

#define ABOUT_WND_POS_X     (-30.0f)
#define ABOUT_WND_POS_Y     (-25.0f)

static RwV2d s_vMarker       = { 173.0f,  -4.0f };
static RwV2d s_vMarkerInc    = {  10.0f,  11.0f };
static RwV2d s_vMarkerSize   = {   8.0f,   8.0f };
static RwV2d s_vAboutPos     = { ABOUT_WND_POS_X, ABOUT_WND_POS_Y };
static RwV2d s_vAboutItemPos = {  70.0f, -65.0f };
static RwV2d s_vAboutStarPos = { -20.0f,   8.0f };
static RwV2d s_avAboutTextPos [] =
{
    { -235.0f, -150.0f },
    { -235.0f,  -65.0f },
    { -235.0f,    3.0f },
};
static Rt2dBBox s_aAboutBox[] =
{
    { -237.0f,   50.0f, 200.0f,  96.0f },
    { -226.0f, -137.0f, 380.0f, 100.0f },
    { -237.0f,   -5.0f, 200.0f,  64.0f },
};
static RwV2d s_vGetDatePos      = { -225.0f,  18.0f };
static RwV2d s_vAboutArrowUPos  = {  171.0f,  40.0f };
static RwV2d s_vAboutArrowDPos  = {  171.0f, 133.0f };

#else /* TMNT2_BUILD_EU */

#define ABOUT_WND_POS_X     (-38.0f)
#define ABOUT_WND_POS_Y     (0.0f)

static RwV2d s_vMarker       = { 173.0f,  -4.0f };
static RwV2d s_vMarkerInc    = {  10.0f,  11.0f };
static RwV2d s_vMarkerSize   = {   8.0f,   8.0f };
static RwV2d s_vAboutPos     = { ABOUT_WND_POS_X, ABOUT_WND_POS_Y };
static RwV2d s_vAboutItemPos = {  50.0f, -58.0f };
static RwV2d s_vAboutStarPos = { -45.0f,  18.0f };
static RwV2d s_avAboutTextPos[] =
{
    { -226.0f, -136.0f },
    { -226.0f,  -68.0f },
    { -226.0f,    0.0f },
};
static Rt2dBBox s_aAboutBox[] =
{
    { -226.0f,   70.0f, 158.0f,  64.0f },
    { -226.0f, -137.0f, 380.0f, 100.0f },
    { -226.0f,    2.0f, 158.0f,  64.0f },
};
static RwV2d s_vGetDatePos     = { -226.0f,  16.0f };
static RwV2d s_vAboutArrowUPos = {  170.0f,  48.0f };
static RwV2d s_vAboutArrowDPos = {  170.0f, 140.0f };

#endif /* TMNT2_BUILD_EU */


#define ABOUT_WND_MOV_TIME  (15) // frames
#define ABOUT_WND_MOV_DIST  (640.0f)
#define ABOUT_WND_MOV_STEP  ((ABOUT_WND_MOV_DIST + ABOUT_WND_POS_X) / static_cast<float>(ABOUT_WND_MOV_TIME))


class CAntiqueShop
{
private:
    class CAntiqueSprite final : public CSprite
    {
    public:
        inline CAntiqueSprite(void) : m_bDispState(false) {};
        inline virtual ~CAntiqueSprite(void) {};
        inline void SetDisplay(bool bState) { m_bDispState = bState; };
        inline bool IsDisplaying(void) const { return m_bDispState; };

    private:
        bool m_bDispState;
    };

    struct CTRLINFO
    {
        GAMETEXT TextId;
        RwV2d    Position;
    };

public:
    CAntiqueShop(void);
    ~CAntiqueShop(void);
    void Attach(void);
    void Detach(void);
    bool Move(void);
    void Draw(void);
    void DlgCreate(void);
    void DlgDestroy(void);
    void AntiqueInit(void);
    void AntiqueCheck(void);
    void AntiqueDispChange(CAnimation2D* pAnimation2D);
    void AntiqueDispCtrl(void);
    void AntiqueOpen(void);
    void AntiqueClose(void);
    void AntiqueItemSet(void);
    void AntiqueIdentify(int32 idAntique);
    void AprilFaceChange(bool bIdentify);
    
private:
    CAnimation2D*           m_pAnim2D;
    CAntiqueRecord::STATE   m_aAntiqueState[ANTIQUEID::ID_MAX];
    int32                   m_iCursorV;
    int32                   m_iCursorH;
    int32                   m_idAntiqueSel;
    CAntiqueSprite          m_aAntiqueSprite[ANTIQUEID::ID_MAX];
    CAntiqueSprite          m_aAboutSprite[9];
    bool                    m_bAntiqueOpenFlag;
    bool                    m_bAnimFlag;
    bool                    m_bDispAboutAntiqText;
    bool                    m_bDispAboutCtrlText;
    uint32                  m_uAnimCnt;
    float                   m_fWndOfsX;
    char                    m_szTextAntiqueTakenDate[256];
    RwV2d                   m_vTextAntiqueTakenDatePos;
    GAMETEXT                m_idTextAntiqueName;
    Rt2dBBox                m_bboxTextAntiqueName;
    AREAID::VALUE           m_idAntiqueTakenArea;
    Rt2dBBox                m_bboxTextAntiqueArea;
    GAMETEXT                m_idTextAntiqueComment;
    Rt2dBBox                m_bboxTextAntiqueComment;
    int32                   m_iTextAntiqueCommentLineNum;
    int32                   m_iTextAntiqueCommentLineCur;
    CTimeObj                m_takendate;
    const char*             m_pszAprilFace;
    CDialog*                m_pDlg;
    CTRLINFO                m_aCtrlInfo[3];
};


//
// *********************************************************************************
//


CAntiqueShop::CAntiqueShop(void)
: m_pAnim2D(nullptr)
, m_aAntiqueState()
, m_iCursorV(0)
, m_iCursorH(0)
, m_idAntiqueSel(ANTIQUEID::ID_NONE)
, m_aAntiqueSprite()
, m_aAboutSprite()
, m_bAntiqueOpenFlag(false)
, m_bAnimFlag(false)
, m_bDispAboutAntiqText(false)
, m_bDispAboutCtrlText(false)
, m_uAnimCnt(0)
, m_fWndOfsX(0.0f)
, m_szTextAntiqueTakenDate()
, m_vTextAntiqueTakenDatePos(Math::VECTOR2_ZERO)
, m_idTextAntiqueName(GAMETEXT_EMPTY)
, m_bboxTextAntiqueName()
, m_idAntiqueTakenArea(AREAID::ID_NONE)
, m_bboxTextAntiqueArea()
, m_idTextAntiqueComment(GAMETEXT_EMPTY)
, m_bboxTextAntiqueComment()
, m_iTextAntiqueCommentLineNum(0)
, m_iTextAntiqueCommentLineCur(0)
, m_takendate()
, m_pszAprilFace(nullptr)
, m_pDlg(nullptr)
, m_aCtrlInfo()
{
    ;
};


CAntiqueShop::~CAntiqueShop(void)
{
    DlgDestroy();
};


void CAntiqueShop::Attach(void)
{
    DlgCreate();
    AntiqueCheck();
    AntiqueInit();    
};


void CAntiqueShop::Detach(void)
{
    DlgDestroy();
};


bool CAntiqueShop::Move(void)
{
    bool bResult = false;

    if (m_pAnim2D->CheckMessageGetURL("Home"))
        bResult = true;

    if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_CANCEL))
    {
        if (!m_bAntiqueOpenFlag)
        {
            CGameSound::PlaySE(SDCODE_SE(0x1001));
            if (!m_pDlg->IsOpen())
            {
                m_pDlg->Open();
                m_pDlg->SetStatus(CDialog::STATUS_NO);
                
                CMenuController::KeyLock(CController::DIGITAL_LUP);
                CMenuController::KeyLock(CController::DIGITAL_LDOWN);
                CMenuController::KeyLock(CController::DIGITAL_LLEFT);
                CMenuController::KeyLock(CController::DIGITAL_LRIGHT);
                CMenuController::KeyLock(CController::DIGITAL_OK);
            };
        };
    };

    if (!m_pDlg->IsOpen())
    {
        CMenuController::KeyUnlock(CController::DIGITAL_LUP);
        CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
        CMenuController::KeyUnlock(CController::DIGITAL_LLEFT);
        CMenuController::KeyUnlock(CController::DIGITAL_LRIGHT);
        CMenuController::KeyUnlock(CController::DIGITAL_OK);

        if (m_pDlg->GetStatus() == CDialog::STATUS_YES)
        {
            bResult = true;
            AntiqueDispCtrl();
        }
        else
        {
            AntiqueDispCtrl();
        };
    };

    return bResult;
};


void CAntiqueShop::Draw(void)
{
    CSystem2D::PushRenderState();
    
    for (int32 idAntiq = ANTIQUEID::ID_FIRST; idAntiq < ANTIQUEID::ID_MAX; ++idAntiq)
    {
        if (m_aAntiqueSprite[idAntiq].IsDisplaying() && (m_idAntiqueSel != idAntiq))
            m_aAntiqueSprite[idAntiq].Draw();
    };

    for (int32 i = 0; i < COUNT_OF(m_aAboutSprite); ++i)
    {
        if (m_aAboutSprite[i].IsDisplaying())
            m_aAboutSprite[i].Draw();
    };
    
    CSystem2D::PopRenderState();

#if !defined(TMNT2_BUILD_EU)
    CGameFont::SetHeight(15.0f);
#endif /* !defined(TMNT2_BUILD_EU) */
    
    if (m_bDispAboutAntiqText)
    {
        CGameFont::SetRGBA(255, 255, 255, 255);

#ifdef TMNT2_BUILD_EU
        CGameFont::SetHeight(12.0f);
        CGameFont::Flow(CGameText::GetText(m_idTextAntiqueName),
                        &m_bboxTextAntiqueName);
#else /* TMNT2_BUILD_EU */
        CGameFont::Flow(CGameText::GetText(m_idTextAntiqueName),
                        &m_bboxTextAntiqueName,
                        rt2dJUSTIFYCENTER);
#endif /* TMNT2_BUILD_EU */

#ifdef TMNT2_BUILD_EU
        if ((CConfigure::GetLanguage() == TYPEDEF::CONFIG_LANG_GERMAN) && (m_idAntiqueTakenArea == AREAID::ID_AREA18))
            CGameFont::SetHeight(11.0f);
#endif /* TMNT2_BUILD_EU */
        CGameFont::Flow(CAreaInfo::GetDispName(m_idAntiqueTakenArea),
                        &m_bboxTextAntiqueArea);        

#ifdef TMNT2_BUILD_EU
        CGameFont::SetHeight(12.0f);
#endif /* TMNT2_BUILD_EU */    
        CGameFont::Show(m_szTextAntiqueTakenDate,
                        m_vTextAntiqueTakenDatePos.x,
                        m_vTextAntiqueTakenDatePos.y);

#ifdef TMNT2_BUILD_EU
        CGameFont::SetHeight(15.0f);
#endif /* TMNT2_BUILD_EU */    
        CGameFont::FlowEx(CGameText::GetText(m_idTextAntiqueComment),
                          m_iTextAntiqueCommentLineCur,
                          &m_bboxTextAntiqueComment);
        
        CGameFont::SetRGBA(255, 180, 0, 255);

        RwV2d vecNoTextPos = Math::VECTOR2_ZERO;
#ifdef TMNT2_BUILD_EU
        vecNoTextPos.x =  -30.0f;
        vecNoTextPos.y = -145.0f;
#else /* TMNT2_BUILD_EU */
        vecNoTextPos.x =  -55.0f;
        vecNoTextPos.y = -140.0f;
#endif /* TMNT2_BUILD_EU */

        wchar wszBuff[256];
        wszBuff[0] = UTEXT('\0');

        CTextData::Sprintf(wszBuff, CGameText::GetText(GAMETEXT_AT_NO), m_idAntiqueSel);

        CGameFont::Show(wszBuff,
                        (vecNoTextPos.x + m_fWndOfsX),
                        vecNoTextPos.y);
    };

    if (m_bDispAboutCtrlText)
    {
        CGameFont::SetRGBA(255, 180, 0, 255);
#ifdef TMNT2_BUILD_EU
        CGameFont::SetHeight(15.0f);
#endif /* TMNT2_BUILD_EU */
        for (int32 i = 0; i < COUNT_OF(m_aCtrlInfo); ++i)
        {
            CGameFont::Show(CGameText::GetText(m_aCtrlInfo[i].TextId),
                            m_aCtrlInfo[i].Position.x,
                            m_aCtrlInfo[i].Position.y);
        };
    };

#ifdef TMNT2_BUILD_EU
    CSystem2D::PushRenderState();

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, 255);
    const wchar* pwszText = CGameText::GetText(GAMETEXT_EU_HELP_11);

    if (CGameFont::GetStringWidth(pwszText) >= 500.0f)
    {
        Rt2dBBox bbox;
        bbox.x = -272.0f;
        bbox.y = -230.0f;
        bbox.w =  500.0f;
        bbox.h =   60.0f;

        CGameFont::Flow(pwszText, &bbox);
    }
    else
    {
        CGameFont::Show(pwszText, -272.0f, 190.0f);
    };

    CSystem2D::PopRenderState();
#endif /* TMNT2_BUILD_EU */
};


void CAntiqueShop::DlgCreate(void)
{
    m_pDlg = new CDialog();
    m_pDlg->Set(0.0f, 33.0f, 640.0f, 192.0f);
    m_pDlg->SetStatus(CDialog::STATUS_NO);
    m_pDlg->SetOpenAction(true);
    m_pDlg->SetController(CGameData::Attribute().GetVirtualPad());
    m_pDlg->SetTitle(CGameText::GetText(GAMETEXT_AT_RET),
                     CGameFont::GetHeightScaled() * s_fAntiqueFontScale,
                     { 0xFF, 0xFF, 0xFF, 0xFF });
};


void CAntiqueShop::DlgDestroy(void)
{
    if (m_pDlg)
    {
        delete m_pDlg;
        m_pDlg = nullptr;
    };
};


void CAntiqueShop::AntiqueInit(void)
{
    m_iCursorV = 0;
    m_iCursorH = 0;
    m_idAntiqueSel = ANTIQUEID::ID_NONE;
    m_bAntiqueOpenFlag = false;
    m_bAnimFlag = false;
    m_uAnimCnt = 0;

    for (int32 i = 0; i < COUNT_OF(m_aAntiqueSprite); ++i)
        m_aAntiqueSprite[i].SetDisplay(false);

    for (int32 i = 0; i < COUNT_OF(m_aAboutSprite); ++i)
        m_aAboutSprite[i].SetDisplay(false);

    m_bDispAboutAntiqText = false;
    m_bDispAboutCtrlText = false;

#ifdef TMNT2_BUILD_EU
    m_pszAprilFace = "Antique_258";
#else /* TMNT2_BUILD_EU */
    m_pszAprilFace = "Antique_259";
#endif /* TMNT2_BUILD_EU */
};


void CAntiqueShop::AntiqueCheck(void)
{
    for (int32 idAntique = ANTIQUEID::ID_FIRST; idAntique < ANTIQUEID::ID_MAX; ++idAntique)
    {
        if (CGameData::Record().Antique().IsAntiqueIdentified(ANTIQUEID::VALUE(idAntique)))
        {
            m_aAntiqueState[idAntique] = CAntiqueRecord::STATE_IDENTIFIED;
        }
        else if (CGameData::Record().Antique().IsAntiqueTaken(ANTIQUEID::VALUE(idAntique)))
        {
            m_aAntiqueState[idAntique] = CAntiqueRecord::STATE_UNIDENTIFIED;
        }
        else
        {
            m_aAntiqueState[idAntique] = CAntiqueRecord::STATE_NONE;
        };
    };
};


void CAntiqueShop::AntiqueDispChange(CAnimation2D* pAnimation2D)
{
    m_pAnim2D = pAnimation2D;
    ASSERT(m_pAnim2D != nullptr);

    const char** pszAntiqTexOrg = s_apszAntiqueTexOrg;
    const char** pszAntiqTex = &s_apszAntiqueTex[1];
    
    for (int32 i = 1; i < COUNT_OF(m_aAntiqueState); ++i)
    {
        switch (m_aAntiqueState[i])
        {
        case CAntiqueRecord::STATE_NONE:
            {
                m_pAnim2D->SetTexture(pszAntiqTexOrg[0], "shop_marubaceAPS");
                m_pAnim2D->SetTexture(pszAntiqTexOrg[1], "shop_questAPS");
            }
            break;

        case CAntiqueRecord::STATE_UNIDENTIFIED:
            {
                m_pAnim2D->SetTexture(pszAntiqTexOrg[1], *pszAntiqTex);
            }
            break;

        case CAntiqueRecord::STATE_IDENTIFIED:
            {
                m_pAnim2D->SetTexture(pszAntiqTexOrg[0], "shop_marubaceAPS");
                m_pAnim2D->SetTexture(pszAntiqTexOrg[1], *pszAntiqTex);
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        pszAntiqTexOrg += 2;
        pszAntiqTex += 1;
    };

    for (int32 i = 0; i < COUNT_OF(s_aAntiqueMap); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(s_aAntiqueMap[0]); ++j)
        {
            int32 idAntiq = s_aAntiqueMap[i][j];
            if (idAntiq <= 0)
                break;

            ASSERT(idAntiq >= 0);
            ASSERT(idAntiq < COUNT_OF(m_aAntiqueState));

            ASSERT(idAntiq >= 0);
            ASSERT(idAntiq < COUNT_OF(m_aAntiqueSprite));

            CAntiqueSprite& sprite = m_aAntiqueSprite[idAntiq];
            switch (m_aAntiqueState[idAntiq])
            {
            case CAntiqueRecord::STATE_NONE:
                {
                    sprite.SetDisplay(false);                    
                }
                break;

            case CAntiqueRecord::STATE_UNIDENTIFIED:
                {
                    sprite.SetDisplay(true);
                    sprite.SetTexture(CTextureManager::GetRwTexture("shop_MiconAPS"));
                    sprite.SetRGBA(255, 255, 255, 255);
                    sprite.Resize(s_vMarkerSize.x, s_vMarkerSize.y);
                    sprite.Move(s_vMarker.x + (s_vMarkerInc.x * float(j)),
                                s_vMarker.y - (s_vMarkerInc.y * float(i)));
                }
                break;

            case CAntiqueRecord::STATE_IDENTIFIED:
                {
                    sprite.SetDisplay(true);
                    sprite.SetTexture(CTextureManager::GetRwTexture("shop_MiconBPS"));
                    sprite.SetRGBA(255, 255, 255, 255);
                    sprite.Resize(s_vMarkerSize.x, s_vMarkerSize.y);
                    sprite.Move(s_vMarker.x + (s_vMarkerInc.x * float(j)),
                                s_vMarker.y - (s_vMarkerInc.y * float(i)));
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CAntiqueShop::AntiqueDispCtrl(void)
{
    if (m_pAnim2D->CheckMessageGetURL("PressUp"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorV =  InvClamp(m_iCursorV + 1, 0, 6);
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    }
    else if (m_pAnim2D->CheckMessageGetURL("PressDown"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorV = InvClamp(m_iCursorV - 1, 0, 6);
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    }
    else if (m_pAnim2D->CheckMessageGetURL("PressLeft"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorH = InvClamp(m_iCursorH - 1, 0, 10 - 1);
        m_iTextAntiqueCommentLineCur = 0;
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    }
    else if (m_pAnim2D->CheckMessageGetURL("PressRight"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorH = InvClamp(m_iCursorH + 1, 0, 10 - 1);
        m_iTextAntiqueCommentLineCur = 0;
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    };

    m_idAntiqueSel = s_aAntiqueMap[m_iCursorV][m_iCursorH];
    if ((m_idAntiqueSel > 0) && m_aAntiqueState[m_idAntiqueSel])
        AprilFaceChange(true);
    else
        AprilFaceChange(false);
    
    if (!m_bAntiqueOpenFlag)
    {
        if (m_pAnim2D->CheckMessageGetURL("OpenWindow"))
        {
            if (m_idAntiqueSel <= ANTIQUEID::ID_NONE)
            {
                CGameSound::PlaySE(SDCODE_SE(0x1000));
            }
            else
            {
                CGameSound::PlaySE(SDCODE_SE(0x1002));
                m_bAntiqueOpenFlag = true;
                m_bAnimFlag = true;
            };
        };
    }
    else
    {
        if (m_pAnim2D->CheckMessageGetURL("Exit"))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1001));
            m_bAntiqueOpenFlag = false;
            m_bAnimFlag = true;
        }
        else if(m_idAntiqueSel <= ANTIQUEID::ID_NONE)
        {
            m_bAntiqueOpenFlag = false;
            m_bAnimFlag = true;
        };
    };

    if (m_bAntiqueOpenFlag)
        AntiqueOpen();
    else
        AntiqueClose();
};


void CAntiqueShop::AntiqueOpen(void)
{
    CMenuController::KeyLock(CController::DIGITAL_LUP);
    CMenuController::KeyLock(CController::DIGITAL_LDOWN);

    if (m_bAnimFlag)
    {
        if (++m_uAnimCnt >= ABOUT_WND_MOV_TIME)
            m_bAnimFlag = false;

        m_fWndOfsX = float(m_uAnimCnt) * ABOUT_WND_MOV_STEP - ABOUT_WND_MOV_DIST;
    };

    AntiqueItemSet();
};


void CAntiqueShop::AntiqueClose(void)
{
    if (m_bAnimFlag)
    {
        if (--m_uAnimCnt == 0)
        {
            m_bAnimFlag = false;
            m_iTextAntiqueCommentLineCur = 0;
            CMenuController::KeyUnlock(CController::DIGITAL_LUP);
            CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
        };

        m_fWndOfsX = float(m_uAnimCnt) * ABOUT_WND_MOV_STEP - ABOUT_WND_MOV_DIST;

        AntiqueItemSet();
    }
    else
    {
        for (int32 i = 0; i < COUNT_OF(m_aAboutSprite); ++i)
            m_aAboutSprite[i].SetDisplay(false);

        m_bDispAboutAntiqText = false;
        m_bDispAboutCtrlText = false;
    };
};


void CAntiqueShop::AntiqueItemSet(void)
{
    m_aAboutSprite[0].SetDisplay(true);
    m_aAboutSprite[0].SetTexture(CTextureManager::GetRwTexture("shop_PageBgPS"));
    m_aAboutSprite[0].Move(m_fWndOfsX + s_vAboutPos.x, s_vAboutPos.y);
    m_aAboutSprite[0].Resize(512.0f, 512.0f);

    m_bDispAboutCtrlText = true;

    m_aCtrlInfo[0] = { GAMETEXT_AT_NAME, { s_avAboutTextPos[0].x + m_fWndOfsX, s_avAboutTextPos[0].y } };
    m_aCtrlInfo[1] = { GAMETEXT_AT_AREA, { s_avAboutTextPos[1].x + m_fWndOfsX, s_avAboutTextPos[1].y } };
    m_aCtrlInfo[2] = { GAMETEXT_AT_DATE, { s_avAboutTextPos[2].x + m_fWndOfsX, s_avAboutTextPos[2].y } };

    for (int32 i = 1; i < COUNT_OF(m_aAboutSprite); ++i)
        m_aAboutSprite[i].SetDisplay(false);

    m_bDispAboutAntiqText = false;
    if (m_idAntiqueSel >= 0)
    {
        switch (m_aAntiqueState[m_idAntiqueSel])
        {
        case CAntiqueRecord::STATE_NONE:
            {
                m_aAboutSprite[1].SetDisplay(true);
                m_aAboutSprite[1].SetTexture(CTextureManager::GetRwTexture("shop_questAPS"));
                m_aAboutSprite[1].Move(s_vAboutItemPos.x + m_fWndOfsX, s_vAboutItemPos.y);
                m_aAboutSprite[1].Resize(256.0f, 256.0f);
            }
            break;

        case CAntiqueRecord::STATE_UNIDENTIFIED:
            {
                AntiqueIdentify(m_idAntiqueSel);
            }
            break;

        case CAntiqueRecord::STATE_IDENTIFIED:
            {
                m_aAboutSprite[1].SetDisplay(true);
                m_aAboutSprite[1].SetTexture(CTextureManager::GetRwTexture(s_apszAntiqueTex[m_idAntiqueSel]));
                m_aAboutSprite[1].Move(m_fWndOfsX + s_vAboutItemPos.x, s_vAboutItemPos.y);
                m_aAboutSprite[1].Resize(256.0f, 256.0f);

                for (int32 i = 0; i < s_aAntiquePrice[m_idAntiqueSel]; ++i)
                {                    
                    m_aAboutSprite[i + 2].SetDisplay(true);
                    m_aAboutSprite[i + 2].SetTexture(CTextureManager::GetRwTexture("shop_star"));
                    m_aAboutSprite[i + 2].Move(float(i) * 39.0f + m_fWndOfsX + s_vAboutStarPos.x,
                                               s_vAboutStarPos.y);
                    m_aAboutSprite[i + 2].Resize(32.0f, 32.0f);
                };

                m_bDispAboutAntiqText = true;
                
                m_idTextAntiqueName = s_aAntiqueText[m_idAntiqueSel];
                m_bboxTextAntiqueName = s_aAboutBox[0];
				m_bboxTextAntiqueName.x += m_fWndOfsX;

                m_idTextAntiqueComment = s_aAntiqueComment[m_idAntiqueSel];
                m_bboxTextAntiqueComment = s_aAboutBox[1];
                m_bboxTextAntiqueComment.x += m_fWndOfsX;

#ifdef TMNT2_BUILD_EU
                CGameFont::SetHeight(15.0f);
#endif /* TMNT2_BUILD_EU */

                m_iTextAntiqueCommentLineNum = CGameFont::CountFlowLine(CGameText::GetText(m_idTextAntiqueComment),
                                                                        m_bboxTextAntiqueComment.w);

                if ((m_iTextAntiqueCommentLineNum - m_iTextAntiqueCommentLineCur) > 6)
                {
                    m_aAboutSprite[8].SetDisplay(true);
                    m_aAboutSprite[8].SetTexture(CTextureManager::GetRwTexture("shop_arrow_txt_down"));
                    m_aAboutSprite[8].Move(s_vAboutArrowDPos.x + m_fWndOfsX, s_vAboutArrowDPos.y);
                    m_aAboutSprite[8].Resize(16.0f, 16.0f);

                    if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_LDOWN))
                    {
                        CGameSound::PlaySE(SDCODE_SE(0x1004));
                        m_iTextAntiqueCommentLineCur += 6;
                    };
                };
                
                if (m_iTextAntiqueCommentLineCur > 0)
                {
                    m_aAboutSprite[7].SetDisplay(true);
                    m_aAboutSprite[7].SetTexture(CTextureManager::GetRwTexture("shop_arrow_txt_up"));
                    m_aAboutSprite[7].Move(s_vAboutArrowUPos.x + m_fWndOfsX, s_vAboutArrowUPos.y);
                    m_aAboutSprite[7].Resize(16.0f, 16.0f);

                    if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_LUP))
                    {
                        CGameSound::PlaySE(SDCODE_SE(0x1004));                        
                        m_iTextAntiqueCommentLineCur = Max(m_iTextAntiqueCommentLineCur - 6, 0);
                    };
                };

                CGameData::Record().Antique().GetAntiqueTakenDate(ANTIQUEID::VALUE(m_idAntiqueSel), m_takendate);
                std::sprintf(m_szTextAntiqueTakenDate,
                             "%02d-%02d-%02d %02d:%02d",
                             m_takendate.GetYear() - 2000,
                             m_takendate.GetMonth(),
                             m_takendate.GetDay(),
                             m_takendate.GetHour(),
                             m_takendate.GetMinute());

                m_vTextAntiqueTakenDatePos = s_vGetDatePos;
                m_vTextAntiqueTakenDatePos.x += m_fWndOfsX;
                
                m_idAntiqueTakenArea = CAreaInfo::FindAntiqueArea(ANTIQUEID::VALUE(m_idAntiqueSel));
                m_bboxTextAntiqueArea = s_aAboutBox[2];
                m_bboxTextAntiqueArea.x += m_fWndOfsX;
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CAntiqueShop::AntiqueIdentify(int32 idAntique)
{
    CGameData::Record().Antique().SetAntiqueIdentified(ANTIQUEID::VALUE(idAntique));

    AntiqueCheck();
    AntiqueDispChange(m_pAnim2D);
};


void CAntiqueShop::AprilFaceChange(bool bIdentify)
{
    if (bIdentify)
    {
        int32 price = s_aAntiquePrice[m_idAntiqueSel];
        
        ASSERT(price >= 0);
        ASSERT(price < COUNT_OF(s_apszAprilFace));

        m_pAnim2D->SetTexture(m_pszAprilFace, s_apszAprilFace[price]);
        
        m_pszAprilFace = s_apszAprilFace[price];
    }
    else
    {
        m_pAnim2D->SetTexture(m_pszAprilFace, s_apszAprilFace[0]);
        m_pszAprilFace = s_apszAprilFace[0];
    };
};


static CAntiqueShop* s_pAntiqueShop;


/*static*/ CProcess* CAntiqueShopSequence::Instance(void)
{
    return new CAntiqueShopSequence;
};


CAntiqueShopSequence::CAntiqueShopSequence(void)
: CAnim2DSequence("antique")
, m_bUnlockCheckState(false)
{
    ;
};


CAntiqueShopSequence::~CAntiqueShopSequence(void)
{
    ;
};


bool CAntiqueShopSequence::OnAttach(const void* pParam)
{
    ASSERT(!s_pAntiqueShop);
    
    s_pAntiqueShop = new CAntiqueShop;
    s_pAntiqueShop->Attach();

    m_bUnlockCheckState = false;

    return CAnim2DSequence::OnAttach(FILEID::ID_ANTIQUE_PS2);
};


void CAntiqueShopSequence::OnDetach(void)
{
    if (s_pAntiqueShop)
    {
        s_pAntiqueShop->Detach();
        delete s_pAntiqueShop;
        s_pAntiqueShop = nullptr;
    };

    CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
    CAnim2DSequence::OnDetach();
};


void CAntiqueShopSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (AnimStep())
    {
    case ANIMSTEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
            {               
                if (CGameData::Record().Antique().IsNewAntiqueExisted())
                {
                    // TODO replace it with 26 voice group
                    int32 VoxCode [] = { SDCODE_VOICE(0x6000),
                                         SDCODE_VOICE(0x6001),
                                         SDCODE_VOICE(0x6002) };
                    int32 RndSel3 = Math::Rand() % 3;
                    CGameSound::PlayVoice(VoxCode[RndSel3]);
                };
            };
        }
        break;
        
    case ANIMSTEP_DRAW:
        {
            if (s_pAntiqueShop->Move())
                BeginFadeOut();
        }
        break;

    case ANIMSTEP_END:
        {
            if (!m_bUnlockCheckState)
            {
                m_bUnlockCheckState = true;
                CGameData::OnEndAntiqueShop();
                Call(PROCLABEL_SEQ_UNLOCK);
            }
            else
            {
                Ret();
            };
        }
        break;

    default:
        break;
    };

    CAnim2DSequence::OnMove(bRet, pReturnValue);
};


void CAntiqueShopSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();
    s_pAntiqueShop->Draw();
};


void CAntiqueShopSequence::BeginFadeIn(void)
{
#ifdef TMNT2_BUILD_EU    
    Animation2D().SetText("ANTIQUE_T_00", CGameText::GetText(GAMETEXT_MENU_HOME_SHOP));
    Animation2D().SetText("ANTIQUE_C_00", CGameText::GetText(GAMETEXT_EU_FOOT));
    Animation2D().SetText("ANTIQUE_C_06", CGameText::GetText(GAMETEXT_DB_OTHER));    
    Animation2D().SetText("ANTIQUE_C_05", CGameText::GetText(GAMETEXT_EU_UTROM));
    Animation2D().SetText("ANTIQUE_C_04", CGameText::GetText(GAMETEXT_STATION_DHO));
    Animation2D().SetText("ANTIQUE_C_03", CGameText::GetText(GAMETEXT_STATION_TRI));
    Animation2D().SetText("ANTIQUE_C_02", CGameText::GetText(GAMETEXT_STATION_JAP));
    Animation2D().SetText("ANTIQUE_C_01", CGameText::GetText(GAMETEXT_DB_NY));
#endif /* TMNT2_BUILD_EU */    

    s_pAntiqueShop->AntiqueDispChange(&Animation2D());

    CGameSound::PlayBGM(SDCODE_BGM(0x3023));
    
    CAnim2DSequence::BeginFadeIn();
};