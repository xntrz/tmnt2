#include "OptionsSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameData/OptionData.hpp"
#include "Game/Component/Menu/Dialog.hpp"
#include "Game/Component/Menu/UnlockMessageWindow.hpp"
#include "Game/Component/GameMain/SecretInfo.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/Gamepad.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Text/GameText.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"


#define SWITCH_ANM          (4.0f)
#define SWITCH_ANM_STEP     (1.0f)
#define SCROLL_TIME         (30)
#define SCROLL_TIME_HALF    (30 / 2)
#define SCROLL_START_S      (900.0f)
#define SCROLL_START        (SCROLL_START_S / 3.0f)
#define SCROLL_STEP_S       (SCROLL_START_S / float(SCROLL_TIME / 2))
#define SCROLL_STEP         (SCROLL_START / float(SCROLL_TIME / 2))
#define FONT_HEIGHT_SCALE   (2.0f)

#define FONT_Y_POS(basePos, itemHeight, itemNo) \
    ((basePos) + ((itemHeight) * (static_cast<float>(itemNo))))


//
//  Check values with original game
//
static_assert(SWITCH_ANM        == 4.0f, "update me or delete me");
static_assert(SWITCH_ANM_STEP   == 1.0f, "update me or delete me");
static_assert(SCROLL_START_S    == 900.0f, "update me or delete me");
static_assert(SCROLL_START      == 300.0f, "update me or delete me");
static_assert(SCROLL_STEP_S     == 60.0f, "update me or delete me");
static_assert(SCROLL_STEP       == 20.0f, "update me or delete me");


enum OPTIONMODE
{
    OPTIONMODE_OPTIONS = 0,

    OPTIONMODE_GAME,
    OPTIONMODE_DIFFICULTY,
    OPTIONMODE_SAVE,
    OPTIONMODE_GAME_OK,

    OPTIONMODE_SOUND,
    OPTIONMODE_STEREO,
    OPTIONMODE_BGM,
    OPTIONMODE_SE,
    OPTIONMODE_VOICE,
    OPTIONMODE_SOUND_OK,

    OPTIONMODE_DISPLAY,
    OPTIONMODE_EFFECT,
    OPTIONMODE_MARKER,
    OPTIONMODE_WINDOW,
    OPTIONMODE_RESOLUTION,
    OPTIONMODE_DISPLAY_OK,

    OPTIONMODE_CONTROLLER,
    OPTIONMODE_SELECTPAD,
    OPTIONMODE_GAMEPAD,
    OPTIONMODE_GAMEPAD_CONFIG,
    OPTIONMODE_GAMEPAD_RESET,
    OPTIONMODE_GAMEPAD_RUMBLE,
    OPTIONMODE_GAMEPAD_DEFAULT,
    OPTIONMODE_GAMEPAD_OK,
    OPTIONMODE_KEYBOARD,
    OPTIONMODE_KEYBOARD_CONFIG,
    OPTIONMODE_KEYBOARD_RESET,
    OPTIONMODE_KEYBOARD_DEFAULT,
    OPTIONMODE_KEYBOARD_OK,
    OPTIONMODE_CONTROLLER_OK,

    OPTIONMODE_PASSWORD,
    OPTIONMODE_INPUT,
    OPTIONMODE_PASSWORD_OK,

    OPTIONMODE_DEFAULT,
    OPTIONMODE_WARNING,
    OPTIONMODE_SCROLL_EFFECT,
    OPTIONMODE_EXIT,

    OPTIONMODE_MAX,
};


struct FontData_t
{
    bool        Flag;
    GAMETEXT    TextId;
    RwV2d       ScreenPos;
    float       Height;
    RwRGBA      Color;    
    const char* Text;
};


struct SpriteData_t
{
    bool     Flag;
    GAMETEXT TextId;
    CSprite  Sprite;
    RwV2d    ScreenPos;
    RwRGBA   Color;
};


struct ButtonName_t
{
    uint32   Digital;
    GAMETEXT TextId;
};


static const RwRGBA s_ColorRed      = { 0xFF, 0x00, 0x00, 0xFF };
static const RwRGBA s_ColorGreen    = { 0x00, 0xFF, 0x00, 0xFF };
static const RwRGBA s_ColorBlue     = { 0x00, 0x00, 0xFF, 0xFF };
static const RwRGBA s_ColorOrange   = { 0xFF, 0xB4, 0x00, 0xFF };
static const RwRGBA s_ColorPurple   = { 0xFF, 0x00, 0xFF, 0xFF };
static const RwRGBA s_ColorCyan     = { 0x80, 0x80, 0xFF, 0xFF };
static const RwRGBA s_ColorWhite    = { 0xFF, 0xFF, 0xFF, 0xFF };


#ifdef TMNT2_BUILD_EU
/* title font */
#define FONT_TITLE_X_POS                (-271.0f)
#define FONT_TITLE_Y_POS                (-185.0f)

/* option font */
#define FONT_OPTION_X                   (-108.0f)
#define FONT_OPTION_Y                   (-120.0f)
#define FONT_OPTION_HEIGHT              (29.0f)
#define FONT_OPTION_X_POS(itemNo)       (FONT_OPTION_X)
#define FONT_OPTION_Y_POS(itemNo)       (FONT_Y_POS(FONT_OPTION_Y, FONT_OPTION_HEIGHT, itemNo))
#define FONT_OPTION_EXIT_Y_POS          (110.0f)

/* game font */
#define FONT_GAME_X                     (-108.0f)
#define FONT_GAME_Y                     (-120.0f)
#define FONT_GAME_HEIGHT                (58.0f)
#define FONT_GAME_X_POS(itemNo)         (FONT_GAME_X)
#define FONT_GAME_Y_POS(itemNo)         (FONT_Y_POS(FONT_GAME_Y, FONT_GAME_HEIGHT, itemNo))

/* sound font */
#define FONT_SOUND_X                    (-108.0f)
#define FONT_SOUND_Y                    (-120.0f)
#define FONT_SOUND_HEIGHT               (58.0f)
#define FONT_SOUND_X_POS(itemNo)        (FONT_SOUND_X)
#define FONT_SOUND_Y_POS(itemNo)        (FONT_Y_POS(FONT_SOUND_Y, FONT_SOUND_HEIGHT, itemNo))

/* display font */
#define FONT_DISPLAY_X                  (-108.0f)
#define FONT_DISPLAY_Y                  (-120.0f)
#define FONT_DISPLAY_HEIGHT             (58.0f)
#define FONT_DISPLAY_X_POS(itemNo)      (FONT_DISPLAY_X)
#define FONT_DISPLAY_Y_POS(itemNo)      (FONT_Y_POS(FONT_DISPLAY_Y, FONT_DISPLAY_HEIGHT, itemNo))

/* pad config font */
#define FONT_PAD_X                      (-108.0f)
#define FONT_PAD_Y                      (-144.0f)
#define FONT_PAD_HEIGHT                 (28.0f)
#define FONT_PAD_X_POS(itemNo)          (FONT_PAD_X)
#define FONT_PAD_Y_POS(itemNo)          (FONT_Y_POS(FONT_PAD_Y, FONT_PAD_HEIGHT, itemNo))

/* pad button font */
#define FONT_PAD_BTN_X                  (202.0f)
#define FONT_PAD_BTN_Y                  (-144.0f)
#define FONT_PAD_BTN_HEIGHT             (28.0f)
#define FONT_PAD_BTN_X_POS(itemNo)      (FONT_PAD_BTN_X)
#define FONT_PAD_BTN_Y_POS(itemNo)      (FONT_Y_POS(FONT_PAD_BTN_Y, FONT_PAD_BTN_HEIGHT, itemNo))

/* keyboard config font */
#define FONT_KEYBOARD_X                 (-108.0f)
#define FONT_KEYBOARD_Y                 (-144.0f)
#define FONT_KEYBOARD_HEIGHT            (18.0f)
#define FONT_KEYBOARD_X_POS(itemNo)     (FONT_KEYBOARD_X)
#define FONT_KEYBOARD_Y_POS(itemNo)     (FONT_Y_POS(FONT_KEYBOARD_Y, FONT_KEYBOARD_HEIGHT, itemNo))

/* keyboard button font */
#define FONT_KEYBOARD_BTN_X             (192.0f)
#define FONT_KEYBOARD_BTN_Y             (-144.0f)
#define FONT_KEYBOARD_BTN_HEIGHT        (18.0f)
#define FONT_KEYBOARD_BTN_X_POS(itemNo) (FONT_KEYBOARD_BTN_X)
#define FONT_KEYBOARD_BTN_Y_POS(itemNo) (FONT_Y_POS(FONT_KEYBOARD_BTN_Y, FONT_KEYBOARD_BTN_HEIGHT, itemNo))

/* reso font */
#define FONT_RESO_X_POS                 (-61.0f)
#define FONT_RESO_Y_POS                 (102.0f)

/* help OK font */
#define FONT_HELP_YES_X_POS             (-270.0f)
#define FONT_HELP_YES_Y_POS             (196.0f)

/* help CANCEL font */
#define FONT_HELP_NO_X_POS              (0.0f)
#define FONT_HELP_NO_Y_POS              (196.0f)

/* pad decide font */
#define FONT_DECIDE_X_POS               (-294.0f)
#define FONT_DECIDE_Y_POS               (-3.0f)

/* sure font */
#define FONT_SURE_X_POS                 (-270.0f)
#define FONT_SURE_Y_POS                 (-3.0f)

/* password OK font */
#define FONT_PWOK_X_POS                 (227.0f)
#define FONT_PWOK_Y_POS                 (-14.0f)

/* select pad font */
#define FONT_SELPAD_X_POS               (-294.0f)
#define FONT_SELPAD_Y_POS               (-3.0f)

/* select key font */
#define FONT_SELKEY_X_POS               (-294.0f)
#define FONT_SELKEY_Y_POS               (-3.0f)
#else /* TMNT2_BUILD_EU */
/* title font */
#define FONT_TITLE_X_POS                (-271.0f)
#define FONT_TITLE_Y_POS                (-185.0f)

/* option font */
#define FONT_OPTION_X                   (-108.0f)
#define FONT_OPTION_Y                   (-126.0f)
#define FONT_OPTION_HEIGHT              (29.0f)
#define FONT_OPTION_X_POS(itemNo)       (FONT_OPTION_X)
#define FONT_OPTION_Y_POS(itemNo)       (FONT_Y_POS(FONT_OPTION_Y, FONT_OPTION_HEIGHT, itemNo))
#define FONT_OPTION_EXIT_Y_POS          (26.0f)

/* game font */
#define FONT_GAME_X                     (-108.0f)
#define FONT_GAME_Y                     (-126.0f)
#define FONT_GAME_HEIGHT                (29.0f)
#define FONT_GAME_X_POS(itemNo)         (FONT_GAME_X)
#define FONT_GAME_Y_POS(itemNo)         (FONT_Y_POS(FONT_GAME_Y, FONT_GAME_HEIGHT, itemNo))

/* sound font */
#define FONT_SOUND_X                    (-108.0f)
#define FONT_SOUND_Y                    (-126.0f)
#define FONT_SOUND_HEIGHT               (29.0f)
#define FONT_SOUND_X_POS(itemNo)        (FONT_SOUND_X)
#define FONT_SOUND_Y_POS(itemNo)        (FONT_Y_POS(FONT_SOUND_Y, FONT_SOUND_HEIGHT, itemNo))

/* display font */
#define FONT_DISPLAY_X                  (-108.0f)
#define FONT_DISPLAY_Y                  (-126.0f)
#define FONT_DISPLAY_HEIGHT             (29.0f)
#define FONT_DISPLAY_X_POS(itemNo)      (FONT_DISPLAY_X)
#define FONT_DISPLAY_Y_POS(itemNo)      (FONT_Y_POS(FONT_DISPLAY_Y, FONT_DISPLAY_HEIGHT, itemNo))

/* pad config font */
#define FONT_PAD_X                      (-108.0f)
#define FONT_PAD_Y                      (-144.0f)
#define FONT_PAD_HEIGHT                 (28.0f)
#define FONT_PAD_X_POS(itemNo)          (FONT_PAD_X)
#define FONT_PAD_Y_POS(itemNo)          (FONT_Y_POS(FONT_PAD_Y, FONT_PAD_HEIGHT, itemNo))

/* pad button font */
#define FONT_PAD_BTN_X                  (148.0f)
#define FONT_PAD_BTN_Y                  (-144.0f)
#define FONT_PAD_BTN_HEIGHT             (28.0f)
#define FONT_PAD_BTN_X_POS(itemNo)      (FONT_PAD_BTN_X)
#define FONT_PAD_BTN_Y_POS(itemNo)      (FONT_Y_POS(FONT_PAD_BTN_Y, FONT_PAD_BTN_HEIGHT, itemNo))

/* keyboard config font */
#define FONT_KEYBOARD_X                 (-108.0f)
#define FONT_KEYBOARD_Y                 (-144.0f)
#define FONT_KEYBOARD_HEIGHT            (18.0f)
#define FONT_KEYBOARD_X_POS(itemNo)     (FONT_KEYBOARD_X)
#define FONT_KEYBOARD_Y_POS(itemNo)     (FONT_Y_POS(FONT_KEYBOARD_Y, FONT_KEYBOARD_HEIGHT, itemNo))

/* keyboard button font */
#define FONT_KEYBOARD_BTN_X             (148.0f)
#define FONT_KEYBOARD_BTN_Y             (-144.0f)
#define FONT_KEYBOARD_BTN_HEIGHT        (18.0f)
#define FONT_KEYBOARD_BTN_X_POS(itemNo) (FONT_KEYBOARD_BTN_X)
#define FONT_KEYBOARD_BTN_Y_POS(itemNo) (FONT_Y_POS(FONT_KEYBOARD_BTN_Y, FONT_KEYBOARD_BTN_HEIGHT, itemNo))

/* reso font */
#define FONT_RESO_X_POS                 (-77.0f)
#define FONT_RESO_Y_POS                 (42.0f)

/* help OK font */
#define FONT_HELP_YES_X_POS             (-270.0f)
#define FONT_HELP_YES_Y_POS             (196.0f)

/* help CANCEL font */
#define FONT_HELP_NO_X_POS              (0.0f)
#define FONT_HELP_NO_Y_POS              (196.0f)

/* pad decide font */
#define FONT_DECIDE_X_POS               (-294.0f)
#define FONT_DECIDE_Y_POS               (-3.0f)

/* sure font */
#define FONT_SURE_X_POS                 (-270.0f)
#define FONT_SURE_Y_POS                 (-3.0f)

/* password OK font */
#define FONT_PWOK_X_POS                 (227.0f)
#define FONT_PWOK_Y_POS                 (-14.0f)

/* select pad font */
#define FONT_SELPAD_X_POS               (-294.0f)
#define FONT_SELPAD_Y_POS               (-3.0f)

/* select key font */
#define FONT_SELKEY_X_POS               (-294.0f)
#define FONT_SELKEY_Y_POS               (-3.0f)
#endif /* TMNT2_BUILD_EU */


static const FontData_t s_DummyFont = {};
static const FontData_t s_TitleFont = { true, GAMETEXT_OP_TIT, { FONT_TITLE_X_POS, FONT_TITLE_Y_POS }, (FONT_HEIGHT_SCALE * 1.5f), s_ColorGreen, nullptr };


static const FontData_t s_aOptionFont[] =
{
    { true, GAMETEXT_OP_GAME,   { FONT_OPTION_X_POS(0), FONT_OPTION_Y_POS(0)   }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_SOUND,  { FONT_OPTION_X_POS(1), FONT_OPTION_Y_POS(1)   }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_DISP,   { FONT_OPTION_X_POS(2), FONT_OPTION_Y_POS(2)   }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_CTRL,   { FONT_OPTION_X_POS(3), FONT_OPTION_Y_POS(3)   }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_PW,     { FONT_OPTION_X_POS(4), FONT_OPTION_Y_POS(4)   }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_DEF,    { FONT_OPTION_X_POS(5), FONT_OPTION_Y_POS(5)   }, FONT_HEIGHT_SCALE, s_ColorPurple, nullptr },
    { true, GAMETEXT_OP_EXIT,   { -223.0f,              FONT_OPTION_EXIT_Y_POS }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    {},
};

static const FontData_t s_aGameFont[] =
{
    { true, GAMETEXT_OP_GAME_DIFF,  { FONT_GAME_X_POS(0), FONT_GAME_Y_POS(0) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_GAME_SAVE,  { FONT_GAME_X_POS(1), FONT_GAME_Y_POS(1) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_OK,         { -223.0f,            26.0f              }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    {},
};

static const FontData_t s_aSoundFont[] =
{
    { true, GAMETEXT_OP_SOUND_MODE,     { FONT_SOUND_X_POS(0), FONT_SOUND_Y_POS(0)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_SOUND_V_BGM,    { FONT_SOUND_X_POS(1), FONT_SOUND_Y_POS(1)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_SOUND_V_SE,     { FONT_SOUND_X_POS(2), FONT_SOUND_Y_POS(2)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_SOUND_V_VOX,    { FONT_SOUND_X_POS(3), FONT_SOUND_Y_POS(3)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_OK,             { -223.0f,             26.0f                }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    {},
};

static const FontData_t s_aDisplayFont[] =
{
    { true, GAMETEXT_OP_DISP_HITFX, { FONT_DISPLAY_X_POS(0), FONT_DISPLAY_Y_POS(0) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_DISP_MARK,  { FONT_DISPLAY_X_POS(1), FONT_DISPLAY_Y_POS(1) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_DISP_HELP,  { FONT_DISPLAY_X_POS(2), FONT_DISPLAY_Y_POS(2) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_DISP_RESO,  { FONT_DISPLAY_X_POS(3), FONT_DISPLAY_Y_POS(3) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    { true, GAMETEXT_OP_OK,         { -223.0f,               26.0f                 }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
    {},
};

static const FontData_t s_aPadConfigFont[] =
{
    { true, GAMETEXT_OP_CTRL_FUNC_1_PC, { FONT_PAD_X_POS(0),  FONT_PAD_Y_POS(0)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_2,    { FONT_PAD_X_POS(1),  FONT_PAD_Y_POS(1)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_3_PC, { FONT_PAD_X_POS(2),  FONT_PAD_Y_POS(2)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_4,    { FONT_PAD_X_POS(3),  FONT_PAD_Y_POS(3)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_5,    { FONT_PAD_X_POS(4),  FONT_PAD_Y_POS(4)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_6,    { FONT_PAD_X_POS(5),  FONT_PAD_Y_POS(5)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_7,    { FONT_PAD_X_POS(6),  FONT_PAD_Y_POS(6)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_RST,       { FONT_PAD_X_POS(7),  FONT_PAD_Y_POS(7)  }, FONT_HEIGHT_SCALE, s_ColorCyan,   nullptr   },
    { true, GAMETEXT_OP_CTRL_VIB,       { FONT_PAD_X_POS(9),  FONT_PAD_Y_POS(9)  }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_DEF,       { FONT_PAD_X_POS(10), FONT_PAD_Y_POS(10) }, FONT_HEIGHT_SCALE, s_ColorPurple, nullptr	},
    { true, GAMETEXT_OP_OK,             { -223.0f,            26.0f              }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr	},
    {},
};

static const FontData_t s_aKeyboardConfigFont[] =
{
    { true, GAMETEXT_OP_CTRL_FUNC_1_PC,     { FONT_KEYBOARD_X_POS(0),  FONT_KEYBOARD_Y_POS(0)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_2,        { FONT_KEYBOARD_X_POS(1),  FONT_KEYBOARD_Y_POS(1)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_3_PC,     { FONT_KEYBOARD_X_POS(2),  FONT_KEYBOARD_Y_POS(2)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_4,        { FONT_KEYBOARD_X_POS(3),  FONT_KEYBOARD_Y_POS(3)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_5,        { FONT_KEYBOARD_X_POS(4),  FONT_KEYBOARD_Y_POS(4)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_6,        { FONT_KEYBOARD_X_POS(5),  FONT_KEYBOARD_Y_POS(5)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_7,        { FONT_KEYBOARD_X_POS(6),  FONT_KEYBOARD_Y_POS(6)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_8_PC,     { FONT_KEYBOARD_X_POS(7),  FONT_KEYBOARD_Y_POS(7)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_9_PC,     { FONT_KEYBOARD_X_POS(8),  FONT_KEYBOARD_Y_POS(8)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_10_PC,    { FONT_KEYBOARD_X_POS(9),  FONT_KEYBOARD_Y_POS(9)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_11_PC,    { FONT_KEYBOARD_X_POS(10), FONT_KEYBOARD_Y_POS(10) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_12,       { FONT_KEYBOARD_X_POS(11), FONT_KEYBOARD_Y_POS(11) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_FUNC_13,       { FONT_KEYBOARD_X_POS(12), FONT_KEYBOARD_Y_POS(12) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    { true, GAMETEXT_OP_CTRL_RST,           { FONT_KEYBOARD_X_POS(13), FONT_KEYBOARD_Y_POS(13) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorCyan,   nullptr   },
    { true, GAMETEXT_OP_CTRL_DEF,           { FONT_KEYBOARD_X_POS(16), FONT_KEYBOARD_Y_POS(16) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorPurple, nullptr	},
    { true, GAMETEXT_OP_OK,                 { -223.0f,                 26.0f                   }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr	},
    {},
};


static const FontData_t s_ResolutionFont = { true, GAMETEXT_EMPTY,              { FONT_RESO_X_POS,      FONT_RESO_Y_POS     }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr };
static const FontData_t s_HelpOkFont     = { true, GAMETEXT_HELP_FUNC_SELECT,   { FONT_HELP_YES_X_POS,  FONT_HELP_YES_Y_POS }, FONT_HEIGHT_SCALE, s_ColorWhite, nullptr };
static const FontData_t s_HelpCancelFont = { true, GAMETEXT_HELP_FUNC_BACK,     { FONT_HELP_NO_X_POS,   FONT_HELP_NO_Y_POS  }, FONT_HEIGHT_SCALE, s_ColorWhite, nullptr };
static const FontData_t s_PadDecideFont  = { true, GAMETEXT_OP_CTRL_DECIDE,     { FONT_DECIDE_X_POS,    FONT_DECIDE_Y_POS   }, FONT_HEIGHT_SCALE, s_ColorWhite, nullptr };
static const FontData_t s_SureFont       = { true, GAMETEXT_OP_DEF_MSG,         { FONT_SURE_X_POS,      FONT_SURE_Y_POS     }, FONT_HEIGHT_SCALE, s_ColorWhite, nullptr };
static const FontData_t s_PasswordOkFont = { true, GAMETEXT_OP_OK,              { FONT_PWOK_X_POS,      FONT_PWOK_Y_POS     }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr };
static const FontData_t s_SelectPadMsg   = { true, GAMETEXT_OP_CTRL_PAD,        { FONT_SELPAD_X_POS,    FONT_SELPAD_Y_POS   }, FONT_HEIGHT_SCALE, s_ColorWhite, nullptr };
static const FontData_t s_SelectKeyMsg   = { true, GAMETEXT_OP_CTRL_KEY,        { FONT_SELKEY_X_POS,    FONT_SELKEY_Y_POS   }, FONT_HEIGHT_SCALE, s_ColorWhite, nullptr };


static const char* s_apszPasswordMark[] =
{
    "pass_icon_l",
    "pass_icon_r",
    "pass_icon_m",
    "pass_icon_d",
    "pass_icon_s",
};


static const char s_achPasswordTable[] = { 'L', 'R', 'M', 'D', 'S' };


static const char* s_apszVolume[] =
{
    "op_icon_vol_01",
    "op_icon_vol_02",
    "op_icon_vol_03",
    "op_icon_vol_04",
    "op_icon_vol_05",
    "op_icon_vol_06",
    "op_icon_vol_07",
    "op_icon_vol_08",
    "op_icon_vol_09",
    "op_icon_vol_10",
};


static OPTIONTYPES::BTNFUNC s_aPadButtonFunction[] =
{
    OPTIONTYPES::BTNFUNC_ATTACK_A,
    OPTIONTYPES::BTNFUNC_ATTACK_B,
    OPTIONTYPES::BTNFUNC_JUMP,
    OPTIONTYPES::BTNFUNC_SHURIKEN,
    OPTIONTYPES::BTNFUNC_DASH,
    OPTIONTYPES::BTNFUNC_CHANGE_CHARA,
    OPTIONTYPES::BTNFUNC_GUARD,
};


static OPTIONTYPES::KEYFUNC s_aKeybordButtonFunction[] =
{
    OPTIONTYPES::KEYFUNC_ATTACK_A,
    OPTIONTYPES::KEYFUNC_ATTACK_B,
    OPTIONTYPES::KEYFUNC_JUMP,
    OPTIONTYPES::KEYFUNC_SHURIKEN,
    OPTIONTYPES::KEYFUNC_DASH,
    OPTIONTYPES::KEYFUNC_CHANGE_CHARA,
    OPTIONTYPES::KEYFUNC_GUARD,
    OPTIONTYPES::KEYFUNC_UP,
    OPTIONTYPES::KEYFUNC_DOWN,
    OPTIONTYPES::KEYFUNC_RIGHT,
    OPTIONTYPES::KEYFUNC_LEFT,    
    OPTIONTYPES::KEYFUNC_CHANGE_CAMERA,
    OPTIONTYPES::KEYFUNC_CHANGE_GAUGE,
};


static OPTIONMODE s_aNextModeOptions[] =
{
    OPTIONMODE_GAME,
    OPTIONMODE_SOUND,
    OPTIONMODE_DISPLAY,
    OPTIONMODE_CONTROLLER,
    OPTIONMODE_PASSWORD,
    OPTIONMODE_DEFAULT,
    OPTIONMODE_EXIT
};


static OPTIONMODE s_aNextModeGame[] =
{
    OPTIONMODE_DIFFICULTY,
    OPTIONMODE_SAVE,
    OPTIONMODE_GAME_OK
};


static OPTIONMODE s_aNextModeSound[] =
{
    OPTIONMODE_STEREO,
    OPTIONMODE_BGM,
    OPTIONMODE_SE,
    OPTIONMODE_VOICE,
    OPTIONMODE_SOUND_OK
};


static OPTIONMODE s_aNextModeDisplay[] =
{
    OPTIONMODE_EFFECT,
    OPTIONMODE_MARKER,
    OPTIONMODE_WINDOW,
    OPTIONMODE_RESOLUTION,
    OPTIONMODE_DISPLAY_OK
};


static const char* s_apszNextWindow[] =
{
    "op_mozi_settei",
    "op_mozi_onkyo",
    "op_mozi_hyouzi",
    nullptr,
    "op_mozi_aikotoba",
    nullptr,
    nullptr
};


static GAMETEXT s_aNextTitleId[] =
{
    GAMETEXT_OP_GAME_TIT,
    GAMETEXT_OP_SOUND_TIT,
    GAMETEXT_OP_DISP_TIT,
    GAMETEXT_EMPTY,
    GAMETEXT_OP_PW_TIT,
    GAMETEXT_OP_TIT,
    GAMETEXT_EMPTY
};


static int32 s_aNextIndexMax[] =
{
    COUNT_OF(s_aNextModeGame) - 1,
    COUNT_OF(s_aNextModeSound) - 1,
    COUNT_OF(s_aNextModeDisplay) - 1,
    0,
    0,
    0,
    0
};


static const FontData_t* s_apNextModeFont[] =
{
    s_aGameFont,
    s_aSoundFont,
    s_aDisplayFont,
    &s_DummyFont,
    &s_DummyFont,
    &s_DummyFont,
    &s_DummyFont
};


static const ButtonName_t s_aPadButtonName[] =
{
    { CController::DIGITAL_RUP,     GAMETEXT_OP_CTRL_PAD_BTN_4 },
    { CController::DIGITAL_RDOWN,   GAMETEXT_OP_CTRL_PAD_BTN_1 },
    { CController::DIGITAL_RLEFT,   GAMETEXT_OP_CTRL_PAD_BTN_2 },
    { CController::DIGITAL_RRIGHT,  GAMETEXT_OP_CTRL_PAD_BTN_3 },
    { CController::DIGITAL_L1,      GAMETEXT_OP_CTRL_PAD_BTN_5 },
    { CController::DIGITAL_L2,      GAMETEXT_OP_CTRL_PAD_BTN_6 },
    { CController::DIGITAL_R1,      GAMETEXT_OP_CTRL_PAD_BTN_7 },
    { CController::DIGITAL_R2,      GAMETEXT_OP_CTRL_PAD_BTN_8 },
    {},
};


static const uint32 s_PadConfigDigitalTrigger = CController::DIGITAL_RUP    
                                              | CController::DIGITAL_RDOWN
                                              | CController::DIGITAL_RLEFT
                                              | CController::DIGITAL_RRIGHT
                                              | CController::DIGITAL_L1
                                              | CController::DIGITAL_L2
                                              | CController::DIGITAL_R1
                                              | CController::DIGITAL_R2;


static inline GAMETEXT GetPadButtonName(uint32 Digital)
{
    for (int32 i = 0; i < COUNT_OF(s_aPadButtonName); ++i)
    {
        if (s_aPadButtonName[i].Digital & Digital)
            return s_aPadButtonName[i].TextId;
    };

    return GAMETEXT_EMPTY;
};


class COptions
{
public:
    enum CFGSTATE
    {
        CFGSTATE_NONE = 0,
        CFGSTATE_TO_OPT,
        CFGSTATE_TO_CFG,
    };
    
    enum PASSWORDTYPE
    {
        PASSWORD_L = 0,
        PASSWORD_R,
        PASSWORD_M,
        PASSWORD_D,
        PASSWORD_S,
        
        PASSWORDMAX,
    };

public:
    COptions(void);
    ~COptions(void);
    void Attach(void);
    void Detach(void);
    bool Move(void);
    void Draw(void);
    void Initialize(void);
    void SettingInit(bool bDisplayChanged);
    void BaseSet(void);
    void ArrowDisp(int32 Line, bool bWide = false);
    bool SettingProc(void);
    void CtrlProc(void);
    void DlgCreate(void);
    void DlgDestroy(void);
    bool MsgDecidePad(void);
    bool MsgSelectCfg(void);
    void CfgKeyboard(void);
    void CfgGamepad(void);
    void SwitchDisp(int32 Line, bool On);
    void SwitchVolume(int32 Line, OPTIONTYPES::VOLUMETYPE Volumetype);
    void SwitchSound(int32 Line);
    void SwitchDifficuly(int32 Line);
    bool PasswordInput(void);
    void PasswordScroll(bool In);
    void PasswordClear(void);
    void SwitchRumble(void);
    void SwitchResolution(void);
    void SelectResolution(void);
    bool SwitchOnOff(bool On);
    void SwitchClear(void);
    void OptionToCfg(void);
    void CfgToOption(void);
    void SetPadSprite(const char* TextureName);
    void SetArrowSwitch(int32 Line);
    void SetArrowSelect(int32 Line);
    void FontDataSet(const FontData_t* pFontData);
    void FontDataClear(void);
    void ScrollEffectIn(void);
    void ScrollEffectOut(void);

private:
    CGamepadOptionData::RAWDATA m_GamepadOptionData;
#ifdef TARGET_PC
    CKeyboardOptionData::RAWDATA m_KeyboardOptionData;
#endif /* TARGET_PC */
    int32 m_IndexMax;
    int32 m_IndexMaxNext;
    int32 m_VideomodeNum;
    int32 m_VideomodeNoSel;
    int32 m_VideomodeNoCur;
    int32 m_aVideomodeNo[10];
    int32 m_ConfigPad;
    int32 m_CtrlPad;
    uint32 m_aAssignButton[COUNT_OF(s_aPadButtonFunction)];
    bool m_bConfigPadMode;
    bool m_bDefaultYes;
    const char* m_pszNextWindow;
    GAMETEXT m_NextWindowTextId;
    int32 m_EffectTime;
    int32 m_EffectIn;
    int32 m_EffectOut;
    CFGSTATE m_eCfgState;
    OPTIONMODE m_eOptionMode;
    OPTIONMODE m_eOptionModeNext;
    OPTIONMODE m_eOptionModePrev;
    RwV2d m_vCursorPos;
    float m_fCursorRot;
    bool m_bSwitchMode;
    const char* m_pszWindow;
    bool m_bKeyConfig;
    bool m_bConfigWindow;
    bool m_bConfigWarning;
    float m_fSwitchMoveL;
    float m_fSwitchMoveR;
    int32 m_IndexLine;
    int32 m_IndexLinePrev;
    int32 m_IndexLineNext;
    int32 m_IndexLineExit;
    int32 m_IndexLineExitExit;
    PASSWORDTYPE m_aPassword[7];
    char m_szPassword[8];
    int32 m_PasswordCursor;
    bool m_bPasswordCheckState;
    SpriteData_t m_aOptionBase[3];
    SpriteData_t m_SelectPad;
    SpriteData_t m_Cursor;
    SpriteData_t m_Switch[10];
    SpriteData_t m_Volume[3][10];
    SpriteData_t m_InfoBar;
    SpriteData_t m_ConfigCur;
    SpriteData_t m_PasswordFrame[14];
    SpriteData_t m_PasswordObj[3];
    SpriteData_t m_Resolution[3];
    FontData_t m_WindowTitle;
    // TODO 0x1790 font data
    FontData_t m_Index[20];
    FontData_t m_OnOff[20];
    // TODO 0x1B68 font data[4]
    FontData_t m_PasswordOk;
    FontData_t m_HelpOK;
    FontData_t m_HelpCANCEL;
    const FontData_t* m_pFontData;
    const FontData_t* m_pFontDataNext;
    FontData_t m_ResolutionFont;
    FontData_t m_KeyboardFont[13];
    CMessageWindow* m_pWindow;
    CDialog* m_pDialog;
    CUnlockMessage* m_pUnlock;
#ifdef TMNT2_BUILD_EU
    bool m_bExtExitPipe;    
#endif /* TMNT2_BUILD_EU */
};


COptions::COptions(void)
: m_GamepadOptionData()
#ifdef TARGET_PC
, m_KeyboardOptionData()
#endif /* TARGET_PC */
, m_IndexMax(0)
, m_IndexMaxNext(0)
, m_VideomodeNum(0)
, m_VideomodeNoSel(0)
, m_VideomodeNoCur(0)
, m_aVideomodeNo()
, m_ConfigPad(CController::CONTROLLER_LOCKED_ON_VIRTUAL)
, m_CtrlPad(CController::CONTROLLER_LOCKED_ON_VIRTUAL)
, m_aAssignButton()
, m_bConfigPadMode(false)
, m_bDefaultYes(false)
, m_pszNextWindow(nullptr)
, m_NextWindowTextId(GAMETEXT_EMPTY)
, m_EffectTime(0)
, m_EffectIn(0)
, m_EffectOut(0)
, m_eCfgState(CFGSTATE_NONE)
, m_eOptionMode(OPTIONMODE_OPTIONS)
, m_eOptionModeNext(OPTIONMODE_OPTIONS)
, m_eOptionModePrev(OPTIONMODE_OPTIONS)
, m_vCursorPos(Math::VECTOR2_ZERO)
, m_fCursorRot(0.0f)
, m_bSwitchMode(false)
, m_pszWindow(nullptr)
, m_bKeyConfig(false)
, m_bConfigWindow(false)
, m_bConfigWarning(false)
, m_fSwitchMoveL(0.0f)
, m_fSwitchMoveR(0.0f)
, m_IndexLine(0)
, m_IndexLinePrev(0)
, m_IndexLineNext(0)
, m_IndexLineExit(0)
, m_IndexLineExitExit(0)
, m_aPassword()
, m_szPassword()
, m_PasswordCursor(0)
, m_bPasswordCheckState(false)
, m_aOptionBase()
, m_SelectPad()
, m_Cursor()
, m_Switch()
, m_Volume()
, m_InfoBar()
, m_ConfigCur()
, m_PasswordFrame()
, m_PasswordObj()
, m_Resolution()
, m_WindowTitle()
, m_Index()
, m_OnOff()
, m_PasswordOk()
, m_HelpOK()
, m_HelpCANCEL()
, m_pFontData(nullptr)
, m_pFontDataNext(nullptr)
, m_ResolutionFont()
, m_KeyboardFont()
, m_pWindow(nullptr)
, m_pDialog(nullptr)
, m_pUnlock(nullptr)
{
    ;
};


COptions::~COptions(void)
{
    ;
};


void COptions::Attach(void)
{
    Initialize();
#ifdef TARGET_PC    
    SelectResolution();
#endif /* TARGET_PC */    
    DlgCreate();    
    
    m_eOptionMode     = OPTIONMODE_SCROLL_EFFECT;
    m_eOptionModeNext = OPTIONMODE_OPTIONS;
    m_pFontDataNext   = s_aOptionFont;
    m_IndexMaxNext    = COUNT_OF(s_aOptionFont) - 2;
#ifdef TMNT2_BUILD_EU
    m_bExtExitPipe       = true;
#endif /* TMNT2_BUILD_EU */
};


void COptions::Detach(void)
{
    DlgDestroy();    
    Rt2dDeviceSetLayerDepth(1.0f);
    
    CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
};


bool COptions::Move(void)
{
    bool bShouldExit = SettingProc();
    if (!bShouldExit)
    {
        if ((m_eOptionMode != OPTIONMODE_SELECTPAD)
            && (m_eOptionMode != OPTIONMODE_CONTROLLER)
            && (m_eOptionMode != OPTIONMODE_GAMEPAD_DEFAULT)
            && (m_eOptionMode != OPTIONMODE_KEYBOARD_DEFAULT)
            && (m_eOptionMode != OPTIONMODE_INPUT)
            && (m_eOptionMode != OPTIONMODE_PASSWORD_OK)
            && (m_eOptionMode != OPTIONMODE_DEFAULT)
            && (m_eOptionMode != OPTIONMODE_SCROLL_EFFECT)
            && (m_eOptionMode != OPTIONMODE_EXIT))
        {
            CtrlProc();
        };
    };

    return bShouldExit;
};


void COptions::Draw(void)
{
    CSystem2D::PushRenderState();

#ifdef TMNT2_BUILD_EU
    if (m_bExtExitPipe)
    {
        if (m_aOptionBase[1].Flag)
            m_aOptionBase[1].Sprite.Draw();
        
        if (m_aOptionBase[2].Flag)
            m_aOptionBase[2].Sprite.Draw();
        
        if (m_aOptionBase[0].Flag)
            m_aOptionBase[0].Sprite.Draw();
    }
    else
    {
        for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
        {
            if (m_aOptionBase[i].Flag)
                m_aOptionBase[i].Sprite.Draw();
        };
    };
#else /* TMNT2_BUILD_EU */
    for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
    {
        if (m_aOptionBase[i].Flag)
            m_aOptionBase[i].Sprite.Draw();
    };
#endif /* TMNT2_BUILD_EU */
    
    if (m_SelectPad.Flag)
        m_SelectPad.Sprite.Draw();

    for (int32 i = 0; i < COUNT_OF(m_Switch); i += 2)
    {
        //
        //  Left
        //
        if (m_Switch[i + 0].Flag)
            m_Switch[i + 0].Sprite.Draw();

        //
        //  Right
        //
        if (m_Switch[i + 1].Flag)
            m_Switch[i + 1].Sprite.Draw();
    };

    for (int32 i = 0; i < COUNT_OF(m_Volume); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(m_Volume[0]); ++j)
        {
            if (m_Volume[i][j].Flag)
                m_Volume[i][j].Sprite.Draw();
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_Resolution); ++i)
    {
        if (m_Resolution[i].Flag)
            m_Resolution[i].Sprite.Draw();
    };

    if (m_PasswordObj[0].Flag)
        m_PasswordObj[0].Sprite.Draw();

    for (int32 i = 0; i < (COUNT_OF(m_PasswordFrame) / 2); ++i)
    {
        if (m_PasswordFrame[i].Flag)
            m_PasswordFrame[i].Sprite.Draw();

        if (m_PasswordFrame[i + 7].Flag)
            m_PasswordFrame[i + 7].Sprite.Draw();
    };

    if (m_PasswordObj[1].Flag)
        m_PasswordObj[1].Sprite.Draw();

    if (m_PasswordObj[2].Flag)
        m_PasswordObj[2].Sprite.Draw();

    if (m_ConfigCur.Flag)
        m_ConfigCur.Sprite.Draw();

    if (m_Cursor.Flag)
        m_Cursor.Sprite.DrawRotate();

    if (m_InfoBar.Flag)
        m_InfoBar.Sprite.Draw();

    CSystem2D::PopRenderState();

    if (m_WindowTitle.Flag)
    {
        CGameFont::SetHeightScaled(m_WindowTitle.Height);
        CGameFont::SetRGBA(m_WindowTitle.Color);
        CGameFont::Show(CGameText::GetText(m_WindowTitle.TextId),
                        m_WindowTitle.ScreenPos.x,
                        m_WindowTitle.ScreenPos.y);
    };

    for (int32 i = 0; i < COUNT_OF(m_Index); ++i)
    {
        if (m_Index[i].Flag)
        {
            CGameFont::SetHeightScaled(m_Index[i].Height);
            CGameFont::SetRGBA(m_Index[i].Color);
            
            if (m_Index[i].TextId)
            {
                CGameFont::Show(CGameText::GetText(m_Index[i].TextId),
                                m_Index[i].ScreenPos.x,
                                m_Index[i].ScreenPos.y);
            }
            else
            {
                if (m_Index[i].Text)
                    CGameFont::Show(m_Index[i].Text,
                                    m_Index[i].ScreenPos.x,
                                    m_Index[i].ScreenPos.y);
            };
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_OnOff); ++i)
    {
        if (m_OnOff[i].Flag)
        {
            CGameFont::SetHeightScaled(m_OnOff[i].Height);
            CGameFont::SetRGBA(m_OnOff[i].Color);
            CGameFont::Show(CGameText::GetText(m_OnOff[i].TextId),
                            m_OnOff[i].ScreenPos.x,
                            m_OnOff[i].ScreenPos.y);
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_KeyboardFont); ++i)
    {
        if (m_KeyboardFont[i].Flag)
        {
            CGameFont::SetHeightScaled(m_KeyboardFont[i].Height);
            CGameFont::SetRGBA(m_KeyboardFont[i].Color);
            CGameFont::Show(m_KeyboardFont[i].Text,
                            m_KeyboardFont[i].ScreenPos.x,
                            m_KeyboardFont[i].ScreenPos.y);
        };
    };

    if (m_PasswordOk.Flag)
    {
        CGameFont::SetHeightScaled(m_PasswordOk.Height);
        CGameFont::SetRGBA(m_PasswordOk.Color);
        CGameFont::Show(CGameText::GetText(m_PasswordOk.TextId),
                        m_PasswordOk.ScreenPos.x,
                        m_PasswordOk.ScreenPos.y);
    };

    if (m_ResolutionFont.Flag)
    {
        CGameFont::SetHeightScaled(m_ResolutionFont.Height);
        CGameFont::SetRGBA(m_ResolutionFont.Color);
        CGameFont::Show(m_ResolutionFont.Text,
                        m_ResolutionFont.ScreenPos.x,
                        m_ResolutionFont.ScreenPos.y);
    };

#ifdef TMNT2_BUILD_EU
    CSystem2D::PushRenderState();

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, 255);

    const wchar* pwszText = CGameText::GetText(GAMETEXT_EU_HELP_7);

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
#else /* TMNT2_BUILD_EU */
    if (m_HelpOK.Flag)
    {       
        CGameFont::SetHeightScaled(m_HelpOK.Height);
        CGameFont::SetRGBA(m_HelpOK.Color);
        CGameFont::Show(CGameText::GetText(m_HelpOK.TextId),
                        m_HelpOK.ScreenPos.x,
                        m_HelpOK.ScreenPos.y);
    };

    if (m_HelpCANCEL.Flag)
    {
        CGameFont::SetHeightScaled(m_HelpCANCEL.Height);
        CGameFont::SetRGBA(m_HelpCANCEL.Color);
        CGameFont::Show(CGameText::GetText(m_HelpCANCEL.TextId),
                        m_HelpCANCEL.ScreenPos.x,
                        m_HelpCANCEL.ScreenPos.y);
    };
#endif /* TMNT2_BUILD_EU */
};


void COptions::Initialize(void)
{
    m_GamepadOptionData = {};
#ifdef TARGET_PC    
    m_KeyboardOptionData = {};
#endif /* TARGET_PC */
    m_EffectIn = 30;
    m_EffectOut = 30;
    m_EffectTime = 15;
    m_eOptionMode = OPTIONMODE_OPTIONS;
    m_pFontData = nullptr;
    m_pFontDataNext = nullptr;
    m_bConfigWindow = false;
    m_ConfigPad = 0;
    m_CtrlPad = CController::CONTROLLER_LOCKED_ON_VIRTUAL;
    m_bConfigPadMode = false;
    m_bDefaultYes = false;
    m_bConfigWarning = false;
    m_fSwitchMoveL = 0.0f;
    m_fSwitchMoveR = 0.0f;
    m_bPasswordCheckState = false;
    m_eCfgState = CFGSTATE_NONE;

    for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
        m_aOptionBase[i].Flag = false;

    m_Cursor.Flag    = false;
    m_ConfigCur.Flag = false;
    m_SelectPad.Flag = false;

    for (int32 i = 0; i < COUNT_OF(m_Switch); ++i)
        m_Switch[i].Flag = false;

    for (int32 i = 0; i < COUNT_OF(m_Volume); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(m_Volume[0]); ++j)
            m_Volume[i][j].Flag = false;
    };

    m_InfoBar.Flag = false;

    for (int32 i = 0; i < COUNT_OF(m_Resolution); ++i)
        m_Resolution[i].Flag = false;

    for (int32 i = 0; i < (COUNT_OF(m_PasswordFrame) / 2); ++i)
    {
        m_PasswordFrame[i].Flag = false;
        m_PasswordFrame[i + 7].Flag = false;
    };

    for (int32 i = 0; i < COUNT_OF(m_PasswordObj); ++i)
        m_PasswordObj[i].Flag = false;

    m_WindowTitle.Flag = false;

    for (int32 i = 0; i < COUNT_OF(m_KeyboardFont); ++i)
        m_KeyboardFont[i].Flag = false;

    m_ResolutionFont.Flag = false;
    m_PasswordOk.Flag     = false;
    m_HelpOK.Flag         = false;
    m_HelpCANCEL.Flag     = false;
};


void COptions::SettingInit(bool bDisplayChanged)
{
    CTextureManager::SetCurrentTextureSet("Options");

    m_Cursor.Flag = true;
    m_Cursor.Sprite.SetTexture(CTextureManager::GetRwTexture("op_cursor"));
    m_Cursor.Sprite.ResizeStrict(32.0f, 32.0f);

    m_ConfigCur.Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_config_cursor"));
    m_ConfigCur.Sprite.Resize(16.0f, 32.0f);

    m_pszWindow     = "op_mozi_settei";
    m_pszNextWindow = "op_mozi_settei";

    BaseSet();

    if (!bDisplayChanged)
        m_aOptionBase[2].Sprite.SetAlpha(0);

    m_SelectPad.Flag = false;

    for (int32 i = 0; i < COUNT_OF(m_Switch); i += 2)
    {
        int32 Line = (i / 2);

#ifdef TMNT2_BUILD_EU
        float y = (float(Line) * 58.0f) - 120.0f;
#else /* TMNT2_BUILD_EU */
        float y = (float(Line) * 29.0f) - 126.0f;
#endif /* TMNT2_BUILD_EU */
        
        m_Switch[i + 0].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_left_s"));
        m_Switch[i + 0].Sprite.Resize(8.0f, 8.0f);
#ifdef TMNT2_BUILD_EU
        m_Switch[i + 0].ScreenPos = { 100.0f, y };
#else /* TMNT2_BUILD_EU */
        m_Switch[i + 0].ScreenPos = { 90.0f, y };
#endif /* TMNT2_BUILD_EU */
        m_Switch[i + 0].Sprite.Move(m_Switch[i + 0].ScreenPos.x,
                                    m_Switch[i + 0].ScreenPos.y);

        m_Switch[i + 1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_right_s"));
        m_Switch[i + 1].Sprite.Resize(8.0f, 8.0f);
#ifdef TMNT2_BUILD_EU
        m_Switch[i + 1].ScreenPos = { 230.0f, y };
#else /* TMNT2_BUILD_EU */
        m_Switch[i + 1].ScreenPos = { 230.0f, y };
#endif /* TMNT2_BUILD_EU */
        m_Switch[i + 1].Sprite.Move(m_Switch[i + 1].ScreenPos.x,
                                    m_Switch[i + 1].ScreenPos.y);
    };

    for (int32 i = 0; i < COUNT_OF(m_Volume); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(m_Volume[0]); ++j)
        {
            m_Volume[i][j].Sprite.SetTexture(CTextureManager::GetRwTexture(s_apszVolume[j]));
            m_Volume[i][j].Sprite.Resize(8.0f, 16.0f);
            m_Volume[i][j].Sprite.SetRGBA(s_ColorOrange);
#ifdef TMNT2_BUILD_EU
            m_Volume[i][j].ScreenPos.x = ((float(j) * 9.0f) - 70.0f);
            m_Volume[i][j].ScreenPos.y = ((float(i + 1) * 58.0f) - 126.0f);
#else /* TMNT2_BUILD_EU */
            m_Volume[i][j].ScreenPos.x = ((float(j) * 9.0f) + 105.0f);
            m_Volume[i][j].ScreenPos.y = ((float(i + 1) * 29.0f) - 126.0f);
#endif /* TMNT2_BUILD_EU */
            m_Volume[i][j].Sprite.Move(m_Volume[i][j].ScreenPos.x,
                                       m_Volume[i][j].ScreenPos.y);
        };
    };

    m_Resolution[0].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_ss702"));
    m_Resolution[0].Sprite.Resize(350.0f, 64.0f);
    m_Resolution[1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_left_s"));
    m_Resolution[1].Sprite.Resize(8.0f, 8.0f);
    m_Resolution[2].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_right_s"));
    m_Resolution[2].Sprite.Resize(8.0f, 8.0f);

    m_WindowTitle = s_TitleFont;
    if (!bDisplayChanged)
        m_WindowTitle.ScreenPos.x -= SCROLL_START_S;

    m_NextWindowTextId = GAMETEXT_OP_TIT;
    
    m_HelpOK     = s_HelpOkFont;
    m_HelpCANCEL = s_HelpCancelFont;

#ifdef TARGET_PC
    m_VideomodeNum   = CGameData::Option().Display().GetVideomodeNum();
    m_VideomodeNoCur = CGameData::Option().Display().GetVideomodeCur();
    m_VideomodeNoSel = m_VideomodeNoCur;
#endif /* TARGET_PC */
};


void COptions::BaseSet(void)
{
#ifdef TMNT2_BUILD_EU
    if (m_bExtExitPipe)
    {
        m_aOptionBase[0].Flag = true;
        m_aOptionBase[0].ScreenPos = { -150.0f, 105.0f };
        m_aOptionBase[0].Sprite.Move(m_aOptionBase[0].ScreenPos.x,
                                     m_aOptionBase[0].ScreenPos.y);
        m_aOptionBase[0].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_exit"));
        m_aOptionBase[0].Sprite.Resize(256.0f, 128.0f);


        m_aOptionBase[1].Flag = true;
        m_aOptionBase[1].ScreenPos = { 80.0f, 2.0f };
        m_aOptionBase[1].Sprite.Move(m_aOptionBase[1].ScreenPos.x,
                                     m_aOptionBase[1].ScreenPos.y);
        m_aOptionBase[1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_l1"));
        m_aOptionBase[1].Sprite.Resize(512.0f, 512.0f);
    }
    else
#endif /* TMNT2_BUILD_EU */
    {
        m_aOptionBase[0].Flag = true;
        m_aOptionBase[0].ScreenPos = { -203.0f, 83.0f };
        m_aOptionBase[0].Sprite.Move(m_aOptionBase[0].ScreenPos.x,
                                     m_aOptionBase[0].ScreenPos.y);
        m_aOptionBase[0].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_s1"));
        m_aOptionBase[0].Sprite.Resize(256.0f, 256.0f);


        m_aOptionBase[1].Flag = true;
        m_aOptionBase[1].ScreenPos = { 80.0f, 2.0f };
        m_aOptionBase[1].Sprite.Move(m_aOptionBase[1].ScreenPos.x,
                                     m_aOptionBase[1].ScreenPos.y);
        m_aOptionBase[1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_l1"));
        m_aOptionBase[1].Sprite.Resize(512.0f, 512.0f);
    };
    
    if (m_pszWindow)
    {
        m_aOptionBase[2].Flag = true;
        m_aOptionBase[2].ScreenPos = { 122.0f, 77.0f };
        m_aOptionBase[2].Sprite.Move(m_aOptionBase[2].ScreenPos.x,
                                     m_aOptionBase[2].ScreenPos.y);
        m_aOptionBase[2].Sprite.SetTexture(CTextureManager::GetRwTexture(m_pszWindow));
        m_aOptionBase[2].Sprite.Resize(512.0f, 128.0f);
    }
    else
    {
        m_aOptionBase[2].Flag = false;
    };
};


void COptions::ArrowDisp(int32 Line, bool bWide /*= false*/)
{
#ifdef TMNT2_BUILD_EU
    float fRightOfsX = 35.0f;
    if (!bWide)
        fRightOfsX = 230.0f;
#else /* TMNT2_BUILD_EU */
    float fLeftOfsX = 90.0f;
    if (bWide)
        fLeftOfsX = -102.0f;
#endif /* TMNT2_BUILD_EU */

    
    SpriteData_t* ArrowLeft  = &m_Switch[(Line * 2) + 0];
    ArrowLeft->Flag = true;
#ifdef TMNT2_BUILD_EU
    ArrowLeft->ScreenPos = { -92.0f - m_fSwitchMoveL,
                             (float(Line) * 58.0f) - 91.0f };
#else /* TMNT2_BUILD_EU */
    ArrowLeft->ScreenPos = { fLeftOfsX - m_fSwitchMoveL,
                             (float(Line) * 29.0f) - 126.0f };
#endif /* TMNT2_BUILD_EU */
    ArrowLeft->Sprite.Move(ArrowLeft->ScreenPos.x,
                           ArrowLeft->ScreenPos.y);

    
    SpriteData_t* ArrowRight = &m_Switch[(Line * 2) + 1];
    ArrowRight->Flag = true;
#ifdef TMNT2_BUILD_EU
    ArrowRight->ScreenPos = { fRightOfsX + m_fSwitchMoveR,
                              (float(Line) * 58.0f) - 91.0f };
#else /* TMNT2_BUILD_EU */
    ArrowRight->ScreenPos = { 230.0f + m_fSwitchMoveR,
                              (float(Line) * 29.0f) - 126.0f };
#endif /* TMNT2_BUILD_EU */
    ArrowRight->Sprite.Move(ArrowRight->ScreenPos.x,
                            ArrowRight->ScreenPos.y);

    
    if (m_bSwitchMode)
    {
        SetArrowSwitch(Line);
    }
    else
    {
        SetArrowSelect(Line);

        m_fSwitchMoveL = 0.0f;
        m_fSwitchMoveR = 0.0f;
    };
};


bool COptions::SettingProc(void)
{
    if (!m_bConfigWindow)
    {
        m_SelectPad.Flag = false;
        BaseSet();
    };

    m_Cursor.Flag = true;
    m_Cursor.Sprite.SetRotate(m_fCursorRot);    
    m_fCursorRot += 0.1f;
    if (m_fCursorRot >= 360.0f)
        m_fCursorRot -= 360.0f;

    switch (m_eOptionMode)
    {
    case OPTIONMODE_OPTIONS:
        {
            FontDataClear();
            FontDataSet(s_aOptionFont);

            m_IndexMax          = COUNT_OF(s_aOptionFont) - 2;
            m_eOptionModeNext   = s_aNextModeOptions[m_IndexLine];
            m_pFontDataNext     = s_apNextModeFont[m_IndexLine];
            m_IndexMaxNext      = s_aNextIndexMax[m_IndexLine];
            m_pszNextWindow     = s_apszNextWindow[m_IndexLine];
            m_NextWindowTextId  = s_aNextTitleId[m_IndexLine];
            m_IndexLineExit     = m_IndexLine;
            m_IndexLineNext     = 0;
            m_VideomodeNoCur    = m_VideomodeNoSel;
            m_bSwitchMode       = false;
            m_bKeyConfig        = false;
            m_bConfigWindow     = false;
            m_bConfigPadMode    = false;
            m_bConfigWarning    = false;
            m_EffectTime        = (m_IndexLine == 5 ? 0 : SCROLL_TIME);
            m_eCfgState         = CFGSTATE_NONE;
        }
        break;

    case OPTIONMODE_GAME:
        {
            FontDataClear();
            FontDataSet(s_aGameFont);

            m_IndexMax          = COUNT_OF(s_aGameFont) - 2;
            m_eOptionModeNext   = s_aNextModeGame[m_IndexLine];
            m_IndexLineNext     = m_IndexLine;
            m_pFontDataNext     = s_aOptionFont;
            m_EffectTime        = (m_IndexLine != m_IndexMax ? 0 : SCROLL_TIME);
            m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
            m_pszNextWindow     = "op_mozi_settei";
            m_NextWindowTextId  = GAMETEXT_OP_TIT;
            m_eCfgState         = CFGSTATE_NONE;
            m_bSwitchMode       = false;

            SwitchDifficuly(0);
            SwitchDisp(1, CGameData::Option().Play().IsAutosaveEnabled());
        }
        break;

    case OPTIONMODE_DIFFICULTY:
        {
            m_bSwitchMode = true;
            SwitchDifficuly(0);
        }
        break;

    case OPTIONMODE_SAVE:
        {
            m_bSwitchMode = true;            
            SwitchDisp(1, CGameData::Option().Play().IsAutosaveEnabled());
            
            bool SwitchState = SwitchOnOff(CGameData::Option().Play().IsAutosaveEnabled());
            CGameData::Option().Play().SetEnableAutosave(SwitchState);
        }
        break;

    case OPTIONMODE_GAME_OK:
        {
            CGameData::Option().Play().Apply();
            
            m_eOptionModePrev   = m_eOptionMode;
            m_eOptionMode       = OPTIONMODE_OPTIONS;
            m_IndexLine         = m_IndexLineExit;

            SwitchClear();
        }
        break;

    case OPTIONMODE_SOUND:
        {
            m_WindowTitle.TextId = GAMETEXT_OP_SOUND_TIT;

            FontDataClear();
            FontDataSet(s_aSoundFont);

            m_IndexMax          = COUNT_OF(s_aSoundFont) - 2;
            m_eOptionModeNext   = s_aNextModeSound[m_IndexLine];
            m_IndexLineNext     = m_IndexLine;
            m_pFontDataNext     = s_aOptionFont;
            m_EffectTime        = (m_IndexLine != m_IndexMax ? 0 : SCROLL_TIME);
            m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
            m_pszNextWindow     = "op_mozi_settei";
            m_NextWindowTextId  = GAMETEXT_OP_TIT;
            m_eCfgState         = CFGSTATE_NONE;
            m_bSwitchMode       = false;

            SwitchSound(0);

            SwitchVolume(1, OPTIONTYPES::VOLUMETYPE_BGM);
            SwitchVolume(2, OPTIONTYPES::VOLUMETYPE_SE);
            SwitchVolume(3, OPTIONTYPES::VOLUMETYPE_VOICE);
        }
        break;

    case OPTIONMODE_STEREO:
        {
            m_bSwitchMode = true;
            SwitchSound(0);
        }
        break;

    case OPTIONMODE_BGM:
        {
            m_bSwitchMode = true;
            SwitchVolume(1, OPTIONTYPES::VOLUMETYPE_BGM);
        }
        break;

    case OPTIONMODE_SE:
        {
            m_bSwitchMode = true;
            SwitchVolume(2, OPTIONTYPES::VOLUMETYPE_SE);
        }
        break;

    case OPTIONMODE_VOICE:
        {
            m_bSwitchMode = true;
            SwitchVolume(3, OPTIONTYPES::VOLUMETYPE_VOICE);
        }
        break;

    case OPTIONMODE_SOUND_OK:
        {
            CGameData::Option().Sound().Apply();
            
            m_eOptionModePrev = m_eOptionMode;
            m_eOptionMode     = OPTIONMODE_OPTIONS;
            m_IndexLine       = m_IndexLineExit;

            SwitchClear();
        }
        break;

    case OPTIONMODE_DISPLAY:
        {
            m_WindowTitle.TextId = GAMETEXT_OP_DISP_TIT;

            FontDataClear();
            FontDataSet(s_aDisplayFont);
            
            m_IndexMax          = COUNT_OF(s_aDisplayFont) - 2;
            m_eOptionModeNext   = s_aNextModeDisplay[m_IndexLine];
            m_IndexLineNext     = m_IndexLine;
            m_pFontDataNext     = s_aOptionFont;
            m_EffectTime        = (m_IndexLine != m_IndexMax ? 0 : SCROLL_TIME);
            m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
            m_pszNextWindow     = "op_mozi_settei";
            m_NextWindowTextId  = GAMETEXT_OP_TIT;
            m_eCfgState         = CFGSTATE_NONE;
            m_bSwitchMode       = false;

            SwitchDisp(0, CGameData::Option().Display().IsEnabledFontEffect());
            SwitchDisp(1, CGameData::Option().Display().IsEnabledPlayerMarker());
            SwitchDisp(2, CGameData::Option().Display().IsHelpEnabled());
#ifdef TARGET_PC            
            SwitchResolution();
#endif /* TARGET_PC */          
        }
        break;

    case OPTIONMODE_EFFECT:
        {
            m_bSwitchMode = true;
            SwitchDisp(0, CGameData::Option().Display().IsEnabledFontEffect());

            bool SwitchState = SwitchOnOff(CGameData::Option().Display().IsEnabledFontEffect());
            CGameData::Option().Display().SetEnableFontEffect(SwitchState);
        }
        break;

    case OPTIONMODE_MARKER:
        {
            m_bSwitchMode = true;
            SwitchDisp(1, CGameData::Option().Display().IsEnabledPlayerMarker());

            bool SwitchState = SwitchOnOff(CGameData::Option().Display().IsEnabledPlayerMarker());
            CGameData::Option().Display().SetEnablePlayerMarker(SwitchState);
        }
        break;

    case OPTIONMODE_WINDOW:
        {
            m_bSwitchMode = true;
            SwitchDisp(2, CGameData::Option().Display().IsHelpEnabled());

            bool SwitchState = SwitchOnOff(CGameData::Option().Display().IsHelpEnabled());
            CGameData::Option().Display().SetEnableHelp(SwitchState);            
        }
        break;

    case OPTIONMODE_RESOLUTION:
        {
            m_bSwitchMode = true;            
            SwitchResolution();         
        }
        break;

    case OPTIONMODE_DISPLAY_OK:
        {
            m_eOptionModePrev = OPTIONMODE_DISPLAY_OK;
            m_eOptionMode     = OPTIONMODE_OPTIONS;
            m_IndexLine       = m_IndexLineExit;

#ifdef TARGET_PC            
            if (m_VideomodeNoCur != m_VideomodeNoSel)
            {
                CGameData::Option().Display().SetVideomode(m_VideomodeNoSel);
                if (CGameData::Option().Display().ApplyVideomode())
                    SettingInit(true);
            };
#endif /* TARGET_PC */
            
            SwitchClear();
        }
        break;

    case OPTIONMODE_CONTROLLER:
        {
            m_Cursor.Flag = false;
            
            if (MsgDecidePad())
            {
                int32 configPad = m_ConfigPad;
                if (configPad == CGameData::Attribute().GetVirtualPad())
                {
                    m_EffectTime        = SCROLL_TIME;
                    m_pFontData         = s_aOptionFont;
                    m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
                    m_eOptionMode       = OPTIONMODE_SCROLL_EFFECT;
                    m_eOptionModeNext   = OPTIONMODE_CONTROLLER_OK;
                    m_NextWindowTextId  = GAMETEXT_OP_TIT;
                    m_eCfgState         = CFGSTATE_NONE;

                    if (m_pWindow->IsOpen())
                        m_pWindow->InputAction();
                }
                else
                {   
                    m_eOptionMode = OPTIONMODE_SELECTPAD;
#ifdef TARGET_PC                    
                    if (ControllerIsKeyboard(configPad))
                        CGameData::Option().Keyboard().Snapshot(m_KeyboardOptionData);
                    else
                        CGameData::Option().Gamepad(m_ConfigPad).Snapshot(m_GamepadOptionData);
#else /* TARGET_PC */
                    CGameData::Option().Gamepad(m_ConfigPad).Snapshot(m_GamepadOptionData);
#endif /* TARGET_PC */              
                };
            };
        }
        break;

    case OPTIONMODE_SELECTPAD:
        {
            m_NextWindowTextId = GAMETEXT_OP_CTRL;

#ifdef TARGET_PC            
            if (ControllerIsKeyboard(m_ConfigPad))
            {
                m_eOptionModeNext = OPTIONMODE_KEYBOARD;
                m_pFontDataNext   = s_aKeyboardConfigFont;
                m_IndexMaxNext    = COUNT_OF(s_aKeyboardConfigFont) - 2;

                SetPadSprite("op_pipe_config_key_pc");
            }
            else
            {
                m_eOptionModeNext = OPTIONMODE_GAMEPAD;
                m_pFontDataNext   = s_aPadConfigFont;
                m_IndexMaxNext    = COUNT_OF(s_aPadConfigFont) - 2;

                SetPadSprite("op_pipe_config_pad");
            };
#else /* TARGET_PC */
            m_eOptionModeNext = OPTIONMODE_GAMEPAD;
            m_pFontDataNext   = s_aPadConfigFont;
            m_IndexMaxNext    = COUNT_OF(s_aPadConfigFont) - 2;
            
            SetPadSprite("op_pipe_config_pad");
#endif /* TARGET_PC */
            
            m_SelectPad.Sprite.SetAlpha(0);

            if (MsgSelectCfg())
            {
                if (m_pWindow->IsOpen())
                    m_pWindow->InputAction();

                m_EffectTime    = SCROLL_TIME;
                m_eOptionMode   = OPTIONMODE_SCROLL_EFFECT;
                m_eCfgState     = CFGSTATE_TO_CFG;
                m_bConfigWindow = true;
            };
        }
        break;

    case OPTIONMODE_GAMEPAD:
        {
            m_WindowTitle.TextId = GAMETEXT_OP_CTRL;

            FontDataClear();
            FontDataSet(s_aPadConfigFont);

            m_bKeyConfig     = (m_IndexLine < 7);
            m_IndexMax       = COUNT_OF(s_aPadConfigFont) - 2;
            m_bSwitchMode    = false;
            m_bConfigPadMode = true;
            m_EffectTime     = 0;
            m_eCfgState      = CFGSTATE_NONE;
            
            CfgGamepad();
            SwitchRumble();
            
            switch (m_IndexLine)
            {
            case 7:
                m_eOptionModeNext = OPTIONMODE_GAMEPAD_RESET;
                break;

            case 8:
                m_eOptionModeNext = OPTIONMODE_GAMEPAD_RUMBLE;
                break;

            case 9:
                m_eOptionModeNext = OPTIONMODE_GAMEPAD_DEFAULT;
                break;

            default:
                if (m_IndexLine == m_IndexMax)
                {
                    m_eOptionModeNext   = OPTIONMODE_GAMEPAD_OK;
                    m_EffectTime        = SCROLL_TIME;
                    m_pFontDataNext     = s_aOptionFont;
                    m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
                    m_eCfgState         = CFGSTATE_TO_OPT;
                }
                break;
            };

            m_IndexLineNext     = m_IndexLine;
            m_pszNextWindow     = "op_mozi_settei";
            m_NextWindowTextId  = GAMETEXT_OP_TIT;
        }
        break;

    case OPTIONMODE_GAMEPAD_CONFIG:
        {
            m_eOptionMode = OPTIONMODE_GAMEPAD;
        }
        break;

    case OPTIONMODE_GAMEPAD_RESET:
        {
            CGameData::Option().Gamepad(m_ConfigPad).Restore(m_GamepadOptionData);
            m_eOptionMode = OPTIONMODE_GAMEPAD;
        }
        break;

    case OPTIONMODE_GAMEPAD_RUMBLE:
        {
            m_eOptionModeNext = m_eOptionModePrev;
            m_bSwitchMode     = true;
            
            SwitchRumble();
        }
        break;

    case OPTIONMODE_GAMEPAD_DEFAULT:
        {
            if (!m_pDialog->IsOpen())
            {
                if (m_pDialog->GetStatus() == CDialog::STATUS_YES)
                    CGameData::Option().Gamepad(m_ConfigPad).SetDefault();

                m_bDefaultYes = false;
                m_eOptionMode = OPTIONMODE_GAMEPAD;
            };
        }
        break;

    case OPTIONMODE_GAMEPAD_OK:
        {
            CGameData::Option().Gamepad(m_ConfigPad).Apply();
            
            m_eOptionModePrev   = m_eOptionMode;
            m_eOptionMode       = OPTIONMODE_CONTROLLER_OK;
            m_IndexLine         = m_IndexLineExit;
            m_bConfigWindow     = false;
            m_bConfigPadMode    = false;

            if (CGameData::Attribute().GetVirtualPad() == CController::CONTROLLER_UNLOCKED_ON_VIRTUAL)
                UnlockAllControllers();

            SwitchClear();
        }
        break;

#ifdef TARGET_PC        
    case OPTIONMODE_KEYBOARD:
        {
            m_WindowTitle.TextId = GAMETEXT_OP_CTRL;

            FontDataClear();
            FontDataSet(s_aKeyboardConfigFont);
            
            m_bKeyConfig     = (m_IndexLine < 13);
            m_IndexMax       = COUNT_OF(s_aKeyboardConfigFont) - 2;
            m_bSwitchMode    = false;
            m_bConfigPadMode = true;
            m_EffectTime     = 0;
            m_eCfgState      = CFGSTATE_NONE;

            CfgKeyboard();

            switch (m_IndexLine)
            {
            case 13:
                m_eOptionModeNext = OPTIONMODE_KEYBOARD_RESET;
                break;

            case 14:
                m_eOptionModeNext = OPTIONMODE_KEYBOARD_DEFAULT;
                break;

            default:
                if (m_IndexLine == m_IndexMax)
                {
                    m_eOptionModeNext   = OPTIONMODE_KEYBOARD_OK;
                    m_EffectTime        = SCROLL_TIME;
                    m_pFontDataNext     = s_aOptionFont;
                    m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
                    m_eCfgState         = CFGSTATE_TO_OPT;
                }
                break;
            };

            m_IndexLineNext    = m_IndexLine;
            m_pszNextWindow    = "op_mozi_settei";
            m_NextWindowTextId = GAMETEXT_OP_TIT;
        }
        break;

    case OPTIONMODE_KEYBOARD_CONFIG:
        {
            m_eOptionMode = OPTIONMODE_KEYBOARD;
        }
        break;

    case OPTIONMODE_KEYBOARD_RESET:
        {
            CGameData::Option().Keyboard().Restore(m_KeyboardOptionData);
            m_eOptionMode = OPTIONMODE_KEYBOARD;
        }
        break;

    case OPTIONMODE_KEYBOARD_DEFAULT:
        {
            if (!m_pDialog->IsOpen())
            {
                if (m_pDialog->GetStatus() == CDialog::STATUS_YES)
                    CGameData::Option().Keyboard().SetDefault();

                m_bDefaultYes = false;
                m_eOptionMode = OPTIONMODE_KEYBOARD;
            };
        }
        break;

    case OPTIONMODE_KEYBOARD_OK:
        {
            CGameData::Option().Keyboard().Apply();

            m_eOptionModePrev = m_eOptionMode;
            m_eOptionMode     = OPTIONMODE_CONTROLLER_OK;
            m_IndexLine       = m_IndexLineExit;
            m_bConfigWindow   = false;
            m_bConfigPadMode  = false;

            if (CGameData::Attribute().GetVirtualPad() == CController::CONTROLLER_UNLOCKED_ON_VIRTUAL)
                UnlockAllControllers();

            SwitchClear();
        }
        break;
#endif /* TARGET_PC */
        
    case OPTIONMODE_CONTROLLER_OK:
        {
            m_eOptionModePrev = OPTIONMODE_CONTROLLER_OK;
            m_eOptionMode     = OPTIONMODE_OPTIONS;
            m_IndexLine       = m_IndexLineExit;
        }
        break;

    case OPTIONMODE_PASSWORD:
        {
            m_WindowTitle.TextId = GAMETEXT_OP_PW_TIT;

            FontDataClear();

            m_Cursor.Flag = false;
            if (m_EffectTime > 0)
            {
                PasswordScroll(true);
                --m_EffectTime;
            }
            else
            {
                m_eOptionMode    = OPTIONMODE_INPUT;
                m_EffectTime     = SCROLL_TIME;
                m_PasswordCursor = 0;

                for (int32 i = 0; i < COUNT_OF(m_aPassword); ++i)
                    m_aPassword[i] = PASSWORD_L;

                for (int32 i = 0; i < COUNT_OF(m_szPassword); ++i)
                    m_szPassword[0] = '\0';
                
                m_bPasswordCheckState = false;
            };
        }
        break;

    case OPTIONMODE_INPUT:
        {
            m_Cursor.Flag = false;
            if (PasswordInput())
                m_eOptionMode = OPTIONMODE_PASSWORD_OK;
        }
        break;

    case OPTIONMODE_PASSWORD_OK:
        {
            m_Cursor.Flag = false;
            if (!m_bPasswordCheckState)
            {
                if (m_EffectTime > 0)
                {
                    PasswordScroll(false);
                    --m_EffectTime;                    
                }
                else
                {
                    m_EffectTime        = SCROLL_TIME;
                    m_pFontDataNext     = s_aOptionFont;
                    m_IndexMaxNext      = COUNT_OF(s_aOptionFont) - 2;
                    m_eOptionMode       = OPTIONMODE_SCROLL_EFFECT;
                    m_eOptionModeNext   = OPTIONMODE_OPTIONS;
                    m_IndexLineNext     = m_IndexLineExit;
                    m_pszNextWindow     = "op_mozi_settei";
                    m_NextWindowTextId  = GAMETEXT_OP_TIT;
                };
            }
            else
            {
                int32 virtualPad = CGameData::Attribute().GetVirtualPad();
                if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_OK))
                {
                    CGameSound::PlaySE(SDCODE_SE(0x1002));

                    m_pUnlock->InputAction();
                    m_bPasswordCheckState = false;
                };
            };
        }
        break;

    case OPTIONMODE_DEFAULT:
        {
            if (!m_pDialog->IsOpen())
            {
                if (m_pDialog->GetStatus() == CDialog::STATUS_YES)
                {
                    CGameData::Option().Play().SetDefault();
                    CGameData::Option().Play().Apply();

                    CGameData::Option().Sound().SetDefault();
                    CGameData::Option().Sound().Apply();

                    CGameData::Option().Display().SetDefault();
                    CGameData::Option().Display().Apply();
                    //CGameData::Option().Display().ApplyVideomode();

                    for (int32 i = 0; i < CController::Max(); ++i)
                    {
                        CGameData::Option().Gamepad(i).SetDefault();
                        CGameData::Option().Gamepad(i).Apply();
                    };

#ifdef TARGET_PC
                    CGameData::Option().Keyboard().SetDefault();
                    CGameData::Option().Keyboard().Apply();
#endif /* TARGET_PC */
                };

                m_bDefaultYes = false;
                m_eOptionMode = OPTIONMODE_OPTIONS;
            };
        }
        break;

    case OPTIONMODE_WARNING:
        {
            ;
        }
        break;

    case OPTIONMODE_SCROLL_EFFECT:
        {
            m_Cursor.Flag = false;
            if (m_EffectTime <= 0)
            {
                m_eCfgState     = CFGSTATE_NONE;
                m_IndexMax      = m_IndexMaxNext;
                m_eOptionMode   = m_eOptionModeNext;
                m_IndexLine     = m_IndexLineNext;
                m_EffectTime    = SCROLL_TIME;
            }
            else
            {
                if (m_EffectTime > SCROLL_TIME_HALF)
                {
                    ScrollEffectOut();
                }
                else
                {
                    FontDataClear();
                    FontDataSet(m_pFontDataNext);

                    m_WindowTitle.TextId = m_NextWindowTextId;
                    m_pszWindow          = m_pszNextWindow;
                    
                    ScrollEffectIn();
                };

                switch (m_eCfgState)
                {
                case CFGSTATE_TO_CFG:
                    OptionToCfg();
                    break;

                case CFGSTATE_TO_OPT:
                    CfgToOption();
                    break;

                default:
                    break;						
                };

                --m_EffectTime;
            };
        }
        break;

    case OPTIONMODE_EXIT:
        {
            m_Cursor.Flag = false;
            FontDataClear();
            return true;
        }
        break;

    default:
        break;
    };

    return false;
};


void COptions::CtrlProc(void)
{
    if (m_Cursor.Flag)
        m_Index[m_IndexLine].Color = s_ColorGreen;

    m_Cursor.ScreenPos.x = (m_Index[m_IndexLine].ScreenPos.x - 18.0f);
    m_Cursor.ScreenPos.y = (m_Index[m_IndexLine].ScreenPos.y);
    m_Cursor.Sprite.Move(m_Cursor.ScreenPos.x, m_Cursor.ScreenPos.y);

    if (m_bConfigPadMode)
        m_CtrlPad = m_ConfigPad;
    else
        m_CtrlPad = CGameData::Attribute().GetVirtualPad();

    if (!m_bSwitchMode)
    {
        if (CController::GetDigitalRepeat(m_CtrlPad, CController::DIGITAL_LUP))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            m_IndexLine = InvClamp(m_IndexLine - 1, 0, m_IndexMax - 1);
        }
        else if (CController::GetDigitalRepeat(m_CtrlPad, CController::DIGITAL_LDOWN))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            m_IndexLine = InvClamp(m_IndexLine + 1, 0, m_IndexMax - 1);
        }
        else if (CController::GetDigitalTrigger(m_CtrlPad, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            m_IndexLinePrev = m_IndexLine;
            m_IndexLine     = m_IndexMax;
        }
        else if (CController::GetDigitalTrigger(m_CtrlPad, CController::DIGITAL_LRIGHT))
        {
            if (m_IndexLine == m_IndexMax)
            {
                CGameSound::PlaySE(SDCODE_SE(0x1004));
                m_IndexLine = m_IndexLinePrev;
            };
        };

        if (!m_bKeyConfig)
        {
            if (CController::GetDigitalTrigger(m_CtrlPad, CController::DIGITAL_OK))
            {
                CGameSound::PlaySE(SDCODE_SE(0x1002));
                
                if ((m_eOptionModeNext == OPTIONMODE_GAMEPAD_DEFAULT) ||
                    (m_eOptionModeNext == OPTIONMODE_KEYBOARD_DEFAULT) ||
                    (m_eOptionModeNext == OPTIONMODE_DEFAULT))
                {
                    if (!m_pDialog->IsOpen())
                    {
                        m_pDialog->Open();
                        m_pDialog->SetStatus(CDialog::STATUS_NO);
                        m_pDialog->SetController(m_CtrlPad);
                    };

                    m_IndexLineNext     = m_IndexLine;
                    m_pszNextWindow     = "op_mozi_settei";
                    m_NextWindowTextId  = GAMETEXT_OP_TIT;
                };

                if (m_EffectTime <= 0)
                {
                    m_eOptionModePrev   = m_eOptionMode;
                    m_eOptionMode       = m_eOptionModeNext;
                    m_IndexLine         = m_IndexLineNext;
                }
                else
                {
                    m_eOptionModePrev = m_eOptionMode;
                    m_eOptionMode     = OPTIONMODE_SCROLL_EFFECT;
                };
            }
            else if (CController::GetDigitalTrigger(m_CtrlPad, CController::DIGITAL_CANCEL))
            {
                CGameSound::PlaySE(SDCODE_SE(0x1001));
                if (m_IndexLine == m_IndexMax)
                {
                    m_eOptionModePrev = m_eOptionMode;
                    m_eOptionMode     = OPTIONMODE_SCROLL_EFFECT;
                }
                else
                {
                    m_IndexLinePrev = m_IndexLine;
                    m_IndexLine     = m_IndexMax;
                };
            };
        };
    }
    else if (CController::GetDigitalTrigger(m_CtrlPad, CController::DIGITAL_OK))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1002));
        m_eOptionMode = m_eOptionModePrev;
    }
    else if (CController::GetDigitalTrigger(m_CtrlPad, CController::DIGITAL_CANCEL))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1001));
        m_eOptionMode = m_eOptionModePrev;
    };
};


void COptions::DlgCreate(void)
{
    m_pWindow = new CMessageWindow(CMessageWindow::COLOR_NORMAL);    
    m_pDialog = new CDialog();
    m_pUnlock = new CUnlockMessage();

    if (m_pWindow)
    {
        m_pWindow->Set(0.0f, 33.0f, 640.0f, 192.0f);
        m_pWindow->SetOpenAction(true);
    };

    if (m_pDialog)
    {
        m_pDialog->Set(0.0f, 33.0f, 640.0f, 192.0f);
        m_pDialog->SetOpenAction(true);
        m_pDialog->SetTitle(CGameText::GetText(s_SureFont.TextId),
                            CGameFont::GetHeightScaled() * s_SureFont.Height,
                            s_SureFont.Color);
    };
};


void COptions::DlgDestroy(void)
{
    if (m_pUnlock)
    {
        delete m_pUnlock;
        m_pUnlock = nullptr;
    };

    if (m_pDialog)
    {
        delete m_pDialog;
        m_pDialog = nullptr;        
    };

    if (m_pWindow)
    {
        delete m_pWindow;
        m_pWindow = nullptr;
    };
};


bool COptions::MsgDecidePad(void)
{
    if (!m_pWindow->IsOpen())
    {
        m_pWindow->Open();
        m_pWindow->SetText(CGameText::GetText(s_PadDecideFont.TextId),
                           CGameFont::GetHeightScaled() * s_PadDecideFont.Height,
                           s_PadDecideFont.Color);
    };

    m_Cursor.Flag = false;

    int32 virtualPad = CGameData::Attribute().GetVirtualPad();
    uint32 digitalTrigger = CController::GetDigitalTrigger(virtualPad);
    if (digitalTrigger)
    {        
        if (digitalTrigger & CController::DIGITAL_OK)
        {
            if (virtualPad != CController::CONTROLLER_UNLOCKED_ON_VIRTUAL)
                m_ConfigPad = FindTriggeredController(CController::DIGITAL_OK, true);
            else
                m_ConfigPad = LockTriggeredController(CController::DIGITAL_OK);
        };

        if (m_ConfigPad == -1)
            m_ConfigPad = virtualPad;

        return true;
    };

    return false;
};


bool COptions::MsgSelectCfg(void)
{
    if (!m_pWindow->IsOpen())
        m_pWindow->Open();

    m_Cursor.Flag = false;

    const FontData_t* pFontData = (m_eOptionModeNext == OPTIONMODE_GAMEPAD ? &s_SelectPadMsg : &s_SelectKeyMsg);

    m_pWindow->SetText(CGameText::GetText(pFontData->TextId),
                       CGameFont::GetHeightScaled() * pFontData->Height,
                       pFontData->Color);

    if (CController::GetDigitalTrigger(m_ConfigPad) ||
        CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad()))
        return true;

    return false;
};


void COptions::CfgKeyboard(void)
{
    static const FontData_t s_aKeyboardButtonFont[14] =
    {
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(0),  FONT_KEYBOARD_BTN_Y_POS(0)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(1),  FONT_KEYBOARD_BTN_Y_POS(1)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(2),  FONT_KEYBOARD_BTN_Y_POS(2)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(3),  FONT_KEYBOARD_BTN_Y_POS(3)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(4),  FONT_KEYBOARD_BTN_Y_POS(4)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(5),  FONT_KEYBOARD_BTN_Y_POS(5)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(6),  FONT_KEYBOARD_BTN_Y_POS(6)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(7),  FONT_KEYBOARD_BTN_Y_POS(7)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(8),  FONT_KEYBOARD_BTN_Y_POS(8)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(9),  FONT_KEYBOARD_BTN_Y_POS(9)  }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(10), FONT_KEYBOARD_BTN_Y_POS(10) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(11), FONT_KEYBOARD_BTN_Y_POS(11) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_KEYBOARD_BTN_X_POS(12), FONT_KEYBOARD_BTN_Y_POS(12) }, (FONT_HEIGHT_SCALE * 0.75f), s_ColorOrange, nullptr },
        {},
    };

    static_assert((COUNT_OF(s_aKeyboardButtonFont) - 1) == COUNT_OF(s_aKeybordButtonFunction), "update me");

    const FontData_t* pKeyboardFontData = s_aKeyboardButtonFont;

    for (int32 i = 0; i < COUNT_OF(m_KeyboardFont); ++i)
    {
        if (!pKeyboardFontData->Flag)
            break;

        m_KeyboardFont[i] = *pKeyboardFontData++;
    };

    for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
        m_aOptionBase[i].Flag = false;

    SetPadSprite("op_pipe_config_key_pc");

    if (m_IndexLine >= 13)
    {
        m_ConfigCur.Flag = false;
    }
    else
    {
        m_KeyboardFont[m_IndexLine].Height *= 1.25f;
        m_KeyboardFont[m_IndexLine].Color   = s_ColorGreen;

        m_ConfigCur.Flag      = true;
        m_ConfigCur.ScreenPos = { m_KeyboardFont[m_IndexLine].ScreenPos.x - 8.0f,
                                  m_KeyboardFont[m_IndexLine].ScreenPos.y };
        m_ConfigCur.Sprite.Move(m_ConfigCur.ScreenPos.x,
                                m_ConfigCur.ScreenPos.y);
    };

    for (int32 i = 0; i < COUNT_OF(s_aKeybordButtonFunction); ++i)
    {
        int32 iKey = CGameData::Option().Keyboard().GetAssignedKey(s_aKeybordButtonFunction[i]);
        m_KeyboardFont[i].Text = CGameData::Option().Keyboard().GetKeyName(iKey);        
    };

    if (m_bKeyConfig)
    {
        ASSERT(m_IndexLine >= 0);
        ASSERT(m_IndexLine < COUNT_OF(s_aKeybordButtonFunction));

        if (CGameData::Option().Keyboard().AssignDownKey(s_aKeybordButtonFunction[m_IndexLine]))
            CGameSound::PlaySE(SDCODE_SE(0x1002));
    };
};


void COptions::CfgGamepad(void)
{
    static const FontData_t s_aPadButtonFont[10] =
    {
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(0), FONT_PAD_BTN_Y_POS(0) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(1), FONT_PAD_BTN_Y_POS(1) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(2), FONT_PAD_BTN_Y_POS(2) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(3), FONT_PAD_BTN_Y_POS(3) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(4), FONT_PAD_BTN_Y_POS(4) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(5), FONT_PAD_BTN_Y_POS(5) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(6), FONT_PAD_BTN_Y_POS(6) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(8), FONT_PAD_BTN_Y_POS(8) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        { true, GAMETEXT_EMPTY, { FONT_PAD_BTN_X_POS(9), FONT_PAD_BTN_Y_POS(9) }, FONT_HEIGHT_SCALE, s_ColorOrange, nullptr },
        {},
    };

    static_assert((COUNT_OF(s_aPadButtonFont) - 3) == COUNT_OF(s_aPadButtonFunction), "update me");

    const FontData_t* pPadFontData = s_aPadButtonFont;

    for (int32 i = 0; i < COUNT_OF(m_OnOff); ++i)
    {
        if (!pPadFontData->Flag)
            break;

        m_OnOff[i] = *pPadFontData++;
    };

    for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
        m_aOptionBase[i].Flag = false;

    SetPadSprite("op_pipe_config_pad");

    if (m_IndexLine >= 7)
    {
        m_ConfigCur.Flag = false;
    }
    else
    {
        m_OnOff[m_IndexLine].Height *= 1.25f;
        m_OnOff[m_IndexLine].Color = s_ColorGreen;

        m_ConfigCur.Flag      = true;
        m_ConfigCur.ScreenPos = { m_OnOff[m_IndexLine].ScreenPos.x - 8.0f,
                                  m_OnOff[m_IndexLine].ScreenPos.y };
        m_ConfigCur.Sprite.Move(m_ConfigCur.ScreenPos.x,
                                m_ConfigCur.ScreenPos.y);
    };

    
    for (int32 i = 0; i < COUNT_OF(s_aPadButtonFunction); ++i)
    {
        m_aAssignButton[i] = CGameData::Option().Gamepad(m_ConfigPad).GetAssignedButton(s_aPadButtonFunction[i]);
        m_OnOff[i].TextId = GetPadButtonName(m_aAssignButton[i]);
    };

    if (m_bKeyConfig)
    {
        uint32 digitalTrigger = CController::GetDigitalTrigger(m_ConfigPad);
        uint32 button = digitalTrigger & s_PadConfigDigitalTrigger;
        if(button)
        {
            CGameSound::PlaySE(SDCODE_SE(0x1002));
            CGameData::Option().Gamepad(m_ConfigPad).AssignButton(s_aPadButtonFunction[m_IndexLine], button);            
        };
    };
};


void COptions::SwitchDisp(int32 Line, bool On)
{
    ArrowDisp(Line);
    
    m_OnOff[Line].Flag      = true;
#ifdef TMNT2_BUILD_EU
    m_OnOff[Line].ScreenPos = { -77.0f,
                                (float(Line) * 58.0f) - 91.0f };
#else /* TMNT2_BUILD_EU */
    m_OnOff[Line].ScreenPos = { 105.0f,
                                (float(Line) * 29.0f) - 126.0f };
#endif /* TMNT2_BUILD_EU */
    m_OnOff[Line].TextId    = (On ? GAMETEXT_OP_ON : GAMETEXT_OP_OFF);
    m_OnOff[Line].Height    = (m_bSwitchMode ? 2.5f : 2.0f);
    m_OnOff[Line].Color     = (m_bSwitchMode ? s_ColorGreen : s_ColorOrange);
};


void COptions::SwitchVolume(int32 Line, OPTIONTYPES::VOLUMETYPE Volumetype)
{
    ASSERT(Volumetype >= 0);
    ASSERT(Volumetype < COUNT_OF(m_Volume));

#ifdef TMNT2_BUILD_EU
    ArrowDisp(Line, true);
#else /* TMNT2_BUILD_EU */
    ArrowDisp(Line, false);
#endif /* TMNT2_BUILD_EU */

    int32 soundVolume = CGameData::Option().Sound().GetVolume(Volumetype);
    for (int32 i = 0; i < COUNT_OF(m_Volume[0]); ++i)
    {
        m_Volume[Volumetype][i].Flag        = true;
        m_Volume[Volumetype][i].Color       = (m_bSwitchMode ? s_ColorGreen : s_ColorOrange);
        m_Volume[Volumetype][i].Color.alpha = (soundVolume <= i ? 0x7F : 0xFF);
#ifdef TMNT2_BUILD_EU
        m_Volume[Volumetype][i].ScreenPos = { (float(i) * 9.0f) - 70.0f,
                                              (float(Line) * 58.0f) - 97.0f };
#else /* TMNT2_BUILD_EU */
        m_Volume[Volumetype][i].ScreenPos   = { (float(i) * 9.0f) + 105.0f,
                                                 float(Line) * 29.0f - 126.0f };
#endif /* TMNT2_BUILD_EU */
        m_Volume[Volumetype][i].Sprite.SetRGBA(m_Volume[Volumetype][i].Color);
        m_Volume[Volumetype][i].Sprite.Move(m_Volume[Volumetype][i].ScreenPos.x,
                                            m_Volume[Volumetype][i].ScreenPos.y);
    };

    if (m_bSwitchMode)
    {
        int32 virtualPad = CGameData::Attribute().GetVirtualPad();

        if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveL = SWITCH_ANM;
            soundVolume = Clamp(soundVolume - 1, 0, soundVolume);
        }
        else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LRIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveR = SWITCH_ANM;
            soundVolume = Clamp(soundVolume + 1, 0, CGameSound::VOLUME_MAX);
        };

        CGameData::Option().Sound().SetVolume(Volumetype, soundVolume);

        if (m_fSwitchMoveL > 0.0f)
            m_fSwitchMoveL -= SWITCH_ANM_STEP;

        if (m_fSwitchMoveR > 0.0f)
            m_fSwitchMoveR -= SWITCH_ANM_STEP;
    };
};


void COptions::SwitchSound(int32 Line)
{
    GAMETEXT aTextId[] =
    {
        GAMETEXT_OP_SOUND_MONO,
        GAMETEXT_OP_SOUND_STERO,
        GAMETEXT_OP_SOUND_SUR,
        //GAMETEXT_OP_SOUND_DPL,
        //GAMETEXT_OP_SOUND_DD,
    };

    static_assert(COUNT_OF(aTextId) == CGameSound::MODE_NUM, "update me");
    
    ArrowDisp(Line);
    
    m_OnOff[Line].Flag		= true;
#ifdef TMNT2_BUILD_EU
    m_OnOff[Line].ScreenPos = { -77.0f,
                                (float(Line) * 58.0f) - 91.0f };
#else /* TMNT2_BUILD_EU */
    m_OnOff[Line].ScreenPos = { 105.0f,
                                (float(Line) * 29.0f) - 126.0f };
#endif /* TMNT2_BUILD_EU */
    m_OnOff[Line].TextId    = aTextId[CGameData::Option().Sound().GetMode()];
    m_OnOff[Line].Height	= (m_bSwitchMode ? 2.5f : 2.0f);
    m_OnOff[Line].Color		= (m_bSwitchMode ? s_ColorGreen : s_ColorOrange);

    if (m_bSwitchMode)
    {
        int32 soundMode = CGameData::Option().Sound().GetMode();
        int32 soundModeNum = (CGameSound::MODE_NUM - 1); // exclude surround

        int32 iPad = CGameData::Attribute().GetVirtualPad();
        if (CController::GetDigitalTrigger(iPad, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveL = SWITCH_ANM;
            soundMode = InvClamp(soundMode - 1, 0, soundModeNum - 1);
        }
        else if (CController::GetDigitalTrigger(iPad, CController::DIGITAL_LRIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveR = SWITCH_ANM;
            soundMode = InvClamp(soundMode + 1, 0, soundModeNum - 1);
        };

        CGameData::Option().Sound().SetMode(CGameSound::MODE(soundMode));

        if (m_fSwitchMoveL > 0.0f)
            m_fSwitchMoveL -= SWITCH_ANM_STEP;

        if (m_fSwitchMoveR > 0.0f)
            m_fSwitchMoveR -= SWITCH_ANM_STEP;
    };
};


void COptions::SwitchDifficuly(int32 Line)
{
    GAMETEXT aTextId[] =
    {
        GAMETEXT_OP_GAME_EASY,
        GAMETEXT_OP_GAME_NORM,
        GAMETEXT_OP_GAME_HARD,
    };

    static_assert(COUNT_OF(aTextId) == GAMETYPES::DIFFICULTY_NUM, "update me");

    m_OnOff[Line].Flag      = true;
    m_OnOff[Line].TextId    = aTextId[CGameData::Option().Play().GetDifficulty()];
#ifdef TMNT2_BUILD_EU
    m_OnOff[Line].ScreenPos = { -77.0f,
                                (float(Line) * 58.0f) - 91.0f };
#else /* TMNT2_BUILD_EU */
    m_OnOff[Line].ScreenPos = { 105.0f,
                                (float(Line) * 29.0f) - 126.0f };
#endif /* TMNT2_BUILD_EU */
    m_OnOff[Line].Height    = (m_bSwitchMode ? 2.5f : 2.0f);
    m_OnOff[Line].Color     = (m_bSwitchMode ? s_ColorGreen : s_ColorOrange);

    ArrowDisp(Line);

    if (m_bSwitchMode)
    {
        int32 difficulty = CGameData::Option().Play().GetDifficulty();
        
        int32 virtualPad = CGameData::Attribute().GetVirtualPad();
        if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveL = SWITCH_ANM;
            difficulty = InvClamp(difficulty - 1, 0, GAMETYPES::DIFFICULTY_NUM - 1);
        }
        else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LRIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveR = SWITCH_ANM;
            difficulty = InvClamp(difficulty + 1, 0, GAMETYPES::DIFFICULTY_NUM - 1);
        };
        
        CGameData::Option().Play().SetDifficulty(GAMETYPES::DIFFICULTY(difficulty));

        if (m_fSwitchMoveL > 0.0f)
            m_fSwitchMoveL -= SWITCH_ANM_STEP;

        if (m_fSwitchMoveR > 0.0f)
            m_fSwitchMoveR -= SWITCH_ANM_STEP;
    };
};


bool COptions::PasswordInput(void)
{
    for (int32 i = 0; i < COUNT_OF(m_PasswordFrame) / 2; ++i)
    {
        m_PasswordFrame[i].Flag = true;
        m_PasswordFrame[i].ScreenPos = { (float(i) * 69.0f) - 233.0f,
                                         -55.0f };
        m_PasswordFrame[i].Sprite.Move(m_PasswordFrame[i].ScreenPos.x,
                                      m_PasswordFrame[i].ScreenPos.y);
        m_PasswordFrame[i].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_pass"));
        m_PasswordFrame[i].Sprite.Resize(128.0f, 128.0f);
        m_PasswordFrame[i].Sprite.SetRGBA(s_ColorOrange);
    };

    m_PasswordObj[0].Flag = true;
    m_PasswordObj[0].ScreenPos = { 243.0f, -14.0f };
    m_PasswordObj[0].Sprite.Move(m_PasswordObj[0].ScreenPos.x,
                                 m_PasswordObj[0].ScreenPos.y);
    m_PasswordObj[0].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_pass_ok"));
    m_PasswordObj[0].Sprite.Resize(128.0f, 64.0f);
    m_PasswordObj[0].Sprite.SetRGBA(s_ColorOrange);

    m_PasswordObj[1].Flag = true;
    m_PasswordObj[1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_pass_up"));
    m_PasswordObj[1].Sprite.Resize(32.0f, 16.0f);

    m_PasswordObj[2].Flag = true;
    m_PasswordObj[2].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_pass"));
    m_PasswordObj[2].Sprite.Resize(32.0f, 16.0f);

    m_PasswordOk = s_PasswordOkFont;

    int32 virtualPad = CGameData::Attribute().GetVirtualPad();
    int32 passwordMax = COUNT_OF(m_aPassword);
    if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LLEFT))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_PasswordCursor = InvClamp(m_PasswordCursor - 1, 0, passwordMax);
    }
    else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LRIGHT))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_PasswordCursor = InvClamp(m_PasswordCursor + 1, 0, passwordMax);
    }
    else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_CANCEL))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1001));
        if (m_PasswordCursor == passwordMax)
        {
            PasswordClear();
            return true;
        }
        else
        {
            m_PasswordCursor = passwordMax;
        };
    };

    if (m_PasswordCursor == passwordMax)
    {
        m_PasswordObj[1].Flag = false;
        m_PasswordObj[2].Flag = false;

        m_PasswordObj[0].Sprite.SetRGBA(s_ColorGreen);

        m_PasswordOk.Color = s_ColorGreen;

        if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_OK))
        {
            for (int32 i = 0; i < COUNT_OF(m_szPassword) - 1; ++i)
                m_szPassword[i] = s_achPasswordTable[m_aPassword[i]];

            m_szPassword[COUNT_OF(m_szPassword) - 1] = '\0';

            SECRETID::VALUE secretId = CSecretInfo::CheckPassword(m_szPassword);
            if (secretId != SECRETID::ID_NONE)
            {
                CGameSound::PlaySE(SDCODE_SE(0x1002));                
                PasswordClear();
                m_pUnlock->SetMessage(secretId);
                m_pUnlock->Open();
                m_bPasswordCheckState = true;
                return true;
            }
            else
            {
                CGameSound::PlaySE(SDCODE_SE(0x1000));
                return false;
            };
        };
    }
    else
    {
        m_PasswordFrame[m_PasswordCursor].Sprite.SetRGBA(s_ColorGreen);

        
        m_PasswordObj[1].ScreenPos = { (float(m_PasswordCursor) * 69.0f) - 233.0f,
                                        -90.0f };
        m_PasswordObj[1].Sprite.Move(m_PasswordObj[1].ScreenPos.x,
                                     m_PasswordObj[1].ScreenPos.y);

        
        m_PasswordObj[2].ScreenPos = { (float(m_PasswordCursor) * 69.0f) - 233.0f,
                                        -17.0f };
        m_PasswordObj[2].Sprite.Move(m_PasswordObj[2].ScreenPos.x,
                                     m_PasswordObj[2].ScreenPos.y);

        
        int32 PasswordType = m_aPassword[m_PasswordCursor];
        if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LUP))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            PasswordType = InvClamp(PasswordType - 1, 0, int32(PASSWORDMAX) - 1);
        }
        else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LDOWN))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));            
            PasswordType = InvClamp(PasswordType + 1, 0, int32(PASSWORDMAX) - 1);
        };

        m_aPassword[m_PasswordCursor] = PASSWORDTYPE(PasswordType);
    };

    for (int32 i = 0; i < COUNT_OF(m_PasswordFrame) / 2; ++i)
    {
        RwTexture* pTexture = CTextureManager::GetRwTexture(s_apszPasswordMark[m_aPassword[i]]);

        m_PasswordFrame[i + 7].Flag = true;
        m_PasswordFrame[i + 7].ScreenPos = { (float(i) * 69.0f) - 233.0f,
                                             -55.0f };
        m_PasswordFrame[i + 7].Sprite.Move(m_PasswordFrame[i].ScreenPos.x,
                                           m_PasswordFrame[i].ScreenPos.y);
        m_PasswordFrame[i + 7].Sprite.SetTexture(pTexture);
        m_PasswordFrame[i + 7].Sprite.Resize(64.0f, 64.0f);
    };

    return false;
};


void COptions::PasswordScroll(bool In)
{
    int32 EffectTime    = (In ? (m_EffectTime) : (SCROLL_TIME - m_EffectTime));
    int32 Step          = 255 - (255 * EffectTime) / SCROLL_TIME;
    uint8 AlphaBasis    = uint8(Clamp(Step, 0, 255));

    for (int32 i = 0; i < COUNT_OF(m_PasswordFrame) / 2; ++i)
    {
        m_PasswordFrame[i].Flag = true;
        m_PasswordFrame[i].ScreenPos.x = (float(i) * (69.0f - float(EffectTime))) + (-233.0f - float(EffectTime));
        m_PasswordFrame[i].ScreenPos.y = -55.0f;
        m_PasswordFrame[i].Sprite.Move(m_PasswordFrame[i].ScreenPos.x,
                                       m_PasswordFrame[i].ScreenPos.y);
        m_PasswordFrame[i].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_pass"));
        m_PasswordFrame[i].Sprite.Resize(128.0f, 128.0f);
        m_PasswordFrame[i].Sprite.SetRGBA(s_ColorOrange);
        m_PasswordFrame[i].Sprite.SetAlpha(AlphaBasis);
    };

    m_PasswordObj[0].Flag = true;
    m_PasswordObj[0].ScreenPos = { (243.0f - (float(Step) * float(Step))),
                                   -14.0f };
    m_PasswordObj[0].Sprite.Move(m_PasswordObj[0].ScreenPos.x,
                                 m_PasswordObj[0].ScreenPos.y);
    m_PasswordObj[0].Sprite.SetTexture(CTextureManager::GetRwTexture("op_pipe_pass_ok"));
    m_PasswordObj[0].Sprite.Resize(128.0f, 64.0f);
    m_PasswordObj[0].Sprite.SetRGBA(s_ColorOrange);
    m_PasswordObj[0].Sprite.SetAlpha(AlphaBasis);
};


void COptions::PasswordClear(void)
{
    for (int32 i = 0; i < COUNT_OF(m_PasswordFrame) / 2; ++i)
    {
        m_PasswordFrame[i + 0].Flag = false;
        m_PasswordFrame[i + 7].Flag = false;
    };

    for (int32 i = 0; i < COUNT_OF(m_PasswordObj); ++i)
        m_PasswordObj[i].Flag = false;

    m_PasswordOk.Flag = false;
};


void COptions::SwitchRumble(void)
{
    /* ctrl left cursor */
    m_Switch[0].Flag      = true;
    m_Switch[0].ScreenPos = { (m_OnOff[8].ScreenPos.x - 16.0f) - m_fSwitchMoveL,
                              m_OnOff[8].ScreenPos.y };
    m_Switch[0].Sprite.Move(m_Switch[0].ScreenPos.x,
                            m_Switch[0].ScreenPos.y);

    /* ctrl right cursor */
    m_Switch[1].Flag        = true;
#ifdef TMNT2_BUILD_EU
    m_Switch[1].ScreenPos = { (m_OnOff[8].ScreenPos.x - 16.0f) + 180.0f + m_fSwitchMoveR,
                               m_OnOff[8].ScreenPos.y };
#else /* TMNT2_BUILD_EU */
    m_Switch[1].ScreenPos   = { (m_OnOff[8].ScreenPos.x - 16.0f) + 136.0f + m_fSwitchMoveR,
                                 m_OnOff[8].ScreenPos.y };
#endif /* TMNT2_BUILD_EU */
    m_Switch[1].Sprite.Move(m_Switch[1].ScreenPos.x,
                            m_Switch[1].ScreenPos.y);

    /* ctrl text */
    m_OnOff[8].TextId   = (CGameData::Option().Gamepad(m_ConfigPad).IsEnabledVibration() ? GAMETEXT_OP_ON : GAMETEXT_OP_OFF);
    m_OnOff[8].Height   = (m_bSwitchMode ? 2.5f : 2.0f);
    m_OnOff[8].Color    = (m_bSwitchMode ? s_ColorGreen : s_ColorOrange);

    
    /* if is in switch mode */
    if (m_bSwitchMode)
    {
        SetArrowSwitch(0);

        bool bState = CGameData::Option().Gamepad(m_ConfigPad).IsEnabledVibration();

        int32 virtualPad = CGameData::Attribute().GetVirtualPad();
        if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveL = SWITCH_ANM;
            bState = !bState;
        }
        else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LRIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));

            m_fSwitchMoveR = SWITCH_ANM;
            bState = !bState;
        };

        CGameData::Option().Gamepad(m_ConfigPad).SetEnableVibration(bState);

        if (m_fSwitchMoveL > 0.0f)
            m_fSwitchMoveL -= SWITCH_ANM_STEP;

        if (m_fSwitchMoveR > 0.0f)
            m_fSwitchMoveR -= SWITCH_ANM_STEP;
    }
    else
    {
        SetArrowSelect(0);
        m_fSwitchMoveL = 0.0f;
        m_fSwitchMoveR = 0.0f;        
    };
};


void COptions::SwitchResolution(void)
{
#ifdef TARGET_PC    
    m_Resolution[0].Flag      = true;
#ifdef TMNT2_BUILD_EU
    m_Resolution[0].ScreenPos = { 80.0f, 102.0f };
#else /* TMNT2_BUILD_EU */
    m_Resolution[0].ScreenPos = { 64.0f, 42.0f };
#endif /* TMNT2_BUILD_EU */
    m_Resolution[0].Sprite.Move(m_Resolution[0].ScreenPos.x,
                                m_Resolution[0].ScreenPos.y);
    m_Resolution[0].Sprite.SetRGBA(m_bSwitchMode ? s_ColorGreen : s_ColorOrange);

    
    m_Resolution[1].Flag      = true;
#ifdef TMNT2_BUILD_EU
    m_Resolution[1].ScreenPos = { -73.0f - m_fSwitchMoveL,
                                   103.0f };
#else /* TMNT2_BUILD_EU */
    m_Resolution[1].ScreenPos = { -89.0f - m_fSwitchMoveL,
                                   43.0f };
#endif /* TMNT2_BUILD_EU */
    m_Resolution[1].Sprite.Move(m_Resolution[1].ScreenPos.x,
                                m_Resolution[1].ScreenPos.y);
    m_Resolution[1].Sprite.SetAlpha(255);

    
    m_Resolution[2].Flag = true;
#ifdef TMNT2_BUILD_EU
    m_Resolution[2].ScreenPos = { 233.0f + m_fSwitchMoveR,
                                  103.0f };
#else /* TMNT2_BUILD_EU */
    m_Resolution[2].ScreenPos = { 217.0f + m_fSwitchMoveR,
                                  43.0f };
#endif /* TMNT2_BUILD_EU */
    m_Resolution[2].Sprite.Move(m_Resolution[2].ScreenPos.x,
                                m_Resolution[2].ScreenPos.y);
    m_Resolution[2].Sprite.SetAlpha(255);

    
    m_ResolutionFont        = s_ResolutionFont;
    m_ResolutionFont.Flag   = true;
    m_ResolutionFont.Height = (m_bSwitchMode ? 2.5f : 2.0f);
    m_ResolutionFont.Color  = (m_bSwitchMode ? s_ColorGreen : s_ColorOrange);
    m_ResolutionFont.Text   = CGameData::Option().Display().GetVideomodeName(m_VideomodeNoSel);

    
    if (m_bSwitchMode)
    {
        m_Resolution[1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_left"));
        m_Resolution[1].Sprite.Resize(16.0f, 16.0f);

        m_Resolution[2].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_right"));
        m_Resolution[2].Sprite.Resize(16.0f, 16.0f);

        int32 virtualPad = CGameData::Attribute().GetVirtualPad();
        if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            
            m_fSwitchMoveL   = SWITCH_ANM;
            m_VideomodeNoSel = Clamp(m_VideomodeNoSel - 1, 0, m_VideomodeNum - 1);
        }
        else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LRIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1004));
            
            m_fSwitchMoveR   = SWITCH_ANM;
            m_VideomodeNoSel = Clamp(m_VideomodeNoSel + 1, 0, m_VideomodeNum - 1);
        };
        
        if (m_fSwitchMoveL > 0.0f)
            m_fSwitchMoveL -= SWITCH_ANM_STEP;

        if (m_fSwitchMoveR > 0.0f)
            m_fSwitchMoveR -= SWITCH_ANM_STEP;
    }
    else
    {
        m_Resolution[1].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_left_s"));
        m_Resolution[1].Sprite.Resize(8.0f, 8.0f);

        m_Resolution[2].Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_right_s"));
        m_Resolution[2].Sprite.Resize(8.0f, 8.0f);

        m_fSwitchMoveL = 0.0f;
        m_fSwitchMoveR = 0.0f;
    };
#endif /* TARGET_PC */
};


void COptions::SelectResolution(void)
{
    ;
};


bool COptions::SwitchOnOff(bool On)
{
    bool bResult = On;
    
    if (!m_bSwitchMode)
        return bResult;

    int32 virtualPad = CGameData::Attribute().GetVirtualPad();
    if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LLEFT))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_fSwitchMoveL = SWITCH_ANM;
        bResult = !On;
    }
    else if (CController::GetDigitalTrigger(virtualPad, CController::DIGITAL_LRIGHT))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_fSwitchMoveR = SWITCH_ANM;
        bResult = !On;
    };

    if (m_fSwitchMoveL > 0.0f)
        m_fSwitchMoveL -= SWITCH_ANM_STEP;
    
    if (m_fSwitchMoveR > 0.0f)
        m_fSwitchMoveR -= SWITCH_ANM_STEP;

    return bResult;
};


void COptions::SwitchClear(void)
{
    for (int32 i = 0; i < COUNT_OF(m_Switch); i += 2)
    {
        m_Switch[i + 0].Flag = false;
        m_Switch[i + 1].Flag = false;
    };

    for (int32 i = 0; i < COUNT_OF(m_Volume); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(m_Volume[0]); ++j)
        {
            m_Volume[i][j].Flag = false;
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_Resolution); ++i)
        m_Resolution[i].Flag = false;
};


void COptions::OptionToCfg(void)
{
    float fStep = float(m_EffectTime) * (1.0f / float(SCROLL_TIME));
    fStep = Clamp(fStep, 0.0f, 1.0f);

    for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
    {
        m_aOptionBase[i].Flag = true;
        m_aOptionBase[i].Sprite.SetAlpha( uint8(fStep * 255.0f) );
    };

    m_SelectPad.Flag = true;
    m_SelectPad.Sprite.SetAlpha( uint8((1.0f - fStep) * 255.0f) );
};


void COptions::CfgToOption(void)
{
    float fStep = float(m_EffectTime) * (1.0f / float(SCROLL_TIME));
    fStep = Clamp(fStep, 0.0f, 1.0f);

    for (int32 i = 0; i < COUNT_OF(m_aOptionBase); ++i)
    {
        m_aOptionBase[i].Flag = true;
        m_aOptionBase[i].Sprite.SetAlpha( uint8((1.0f - fStep) * 255.0f) );
    };

    m_SelectPad.Flag = true;
    m_SelectPad.Sprite.SetAlpha( uint8(fStep * 255.0f) );
};


void COptions::SetPadSprite(const char* TextureName)
{
    m_SelectPad.Flag = true;
    m_SelectPad.ScreenPos = { 25.0f, 0.0f };
    m_SelectPad.Sprite.SetTexture(CTextureManager::GetRwTexture(TextureName));
    m_SelectPad.Sprite.Move(25.0f, 0.0f);
    m_SelectPad.Sprite.Resize(590.0f, 345.0f);
};


void COptions::SetArrowSwitch(int32 Line)
{
    SpriteData_t* ArrowLeft = &m_Switch[(Line * 2) + 0];
    SpriteData_t* ArrowRight = &m_Switch[(Line * 2) + 1];

    ArrowLeft->Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_left"));
    ArrowLeft->Sprite.SetAlpha(255);
    ArrowLeft->Sprite.Resize(16.0f, 16.0f);
    
    ArrowRight->Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_right"));
    ArrowRight->Sprite.SetAlpha(255);
    ArrowRight->Sprite.Resize(16.0f, 16.0f);
};


void COptions::SetArrowSelect(int32 Line)
{
    SpriteData_t* ArrowLeft = &m_Switch[(Line * 2) + 0];
    SpriteData_t* ArrowRight = &m_Switch[(Line * 2) + 1];

    ArrowLeft->Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_left_s"));
    ArrowLeft->Sprite.SetAlpha(255);
    ArrowLeft->Sprite.Resize(8.0f, 8.0f);

    ArrowRight->Sprite.SetTexture(CTextureManager::GetRwTexture("op_icon_right_s"));
    ArrowRight->Sprite.SetAlpha(255);
    ArrowRight->Sprite.Resize(8.0f, 8.0f);
};


void COptions::FontDataSet(const FontData_t* pFontData)
{
    const FontData_t* pData = pFontData;
    for (int32 i = 0; i < COUNT_OF(m_Index); ++i)
    {
        if (!pData->Flag)
            break;
        
        m_Index[i] = *pData++;
    };
};


void COptions::FontDataClear(void)
{
    static_assert(COUNT_OF(m_Index) == COUNT_OF(m_OnOff), "update me");
    
    for (int32 i = 0; i < COUNT_OF(m_Index); ++i)
    {
        m_Index[i].Flag = false;
        m_OnOff[i].Flag = false;
    };

    for (int32 i = 0; i < COUNT_OF(m_KeyboardFont); ++i)
        m_KeyboardFont[i].Flag = false;
};


void COptions::ScrollEffectIn(void)
{
    int32 AlphaBasis = 255 - 255 * m_EffectTime / SCROLL_TIME_HALF;
    if (AlphaBasis > 255)
        AlphaBasis = 255;

    m_WindowTitle.ScreenPos.x = s_TitleFont.ScreenPos.x - (m_EffectTime * m_EffectTime);
    m_aOptionBase[2].Sprite.SetAlpha(uint8(AlphaBasis));

    for (int32 i = 0; i < m_IndexMaxNext; ++i)
    {
        m_Index[i].ScreenPos.x -= float(m_EffectTime * i);
        m_Index[i].Color.alpha = uint8(AlphaBasis);        
    };
};


void COptions::ScrollEffectOut(void)
{
    int32 AlphaBasis = 255 * m_EffectTime / SCROLL_TIME_HALF - 255;
    if (AlphaBasis < 0)
        AlphaBasis = 0;

    m_WindowTitle.ScreenPos.x += SCROLL_STEP_S;
    m_aOptionBase[2].Sprite.SetAlpha(uint8(AlphaBasis));

    for (int32 i = 0; i < m_IndexMax; ++i)
    {
        if (i == m_IndexLine)
        {
            m_Index[i].ScreenPos.x += SCROLL_STEP_S;
            m_OnOff[i].ScreenPos.x += SCROLL_STEP_S;
        }
        else
        {
            m_Index[i].ScreenPos.x += SCROLL_STEP;
            m_OnOff[i].ScreenPos.x += SCROLL_STEP;
            m_Index[i].Color.alpha = uint8(AlphaBasis);
            m_OnOff[i].Color.alpha = uint8(AlphaBasis);
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_Switch); i += 2)
    {
        //
        //  Left
        //
        m_Switch[i + 0].ScreenPos.x += SCROLL_STEP;
        m_Switch[i + 0].Sprite.Move(m_Switch[i + 0].ScreenPos.x,
                                    m_Switch[i + 0].ScreenPos.y);
        m_Switch[i + 0].Sprite.SetAlpha(uint8(AlphaBasis));

        //
        //  Right
        //
        m_Switch[i + 1].ScreenPos.x += SCROLL_STEP;
        m_Switch[i + 1].Sprite.Move(m_Switch[i + 1].ScreenPos.x,
                                    m_Switch[i + 1].ScreenPos.y);
        m_Switch[i + 1].Sprite.SetAlpha(uint8(AlphaBasis));
    };

    for (int32 i = 0; i < COUNT_OF(m_Volume); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(m_Volume[0]); ++j)
        {
            m_Volume[i][j].ScreenPos.x += SCROLL_STEP;
            m_Volume[i][j].Sprite.Move(m_Volume[i][j].ScreenPos.x,
                                       m_Volume[i][j].ScreenPos.y);
            m_Volume[i][j].Sprite.SetAlpha(uint8(AlphaBasis));
        };
    };

    for (int32 i = 0; i < COUNT_OF(m_Resolution); ++i)
    {
        m_Resolution[i].ScreenPos.x += SCROLL_STEP;
        m_Resolution[i].Sprite.Move(m_Resolution[i].ScreenPos.x,
                                    m_Resolution[i].ScreenPos.y);
        m_Resolution[i].Sprite.SetAlpha(uint8(AlphaBasis));
    };

    m_ResolutionFont.ScreenPos.x += SCROLL_STEP;
    m_ResolutionFont.Color.alpha = uint8(AlphaBasis);

    for (int32 i = 0; i < COUNT_OF(m_KeyboardFont); ++i)
    {
        m_KeyboardFont[i].ScreenPos.x += SCROLL_STEP;
        m_KeyboardFont[i].Color.alpha = uint8(AlphaBasis);
    };

#ifdef TMNT2_BUILD_EU
    if ((m_EffectTime - 1) <= 15)
    {
        switch (m_eOptionModeNext)
        {
        case OPTIONMODE_OPTIONS:
        case OPTIONMODE_GAME_OK:
        case OPTIONMODE_SOUND_OK:
        case OPTIONMODE_DISPLAY_OK:
        case OPTIONMODE_GAMEPAD:
        case OPTIONMODE_GAMEPAD_OK:
        case OPTIONMODE_KEYBOARD_OK:
        case OPTIONMODE_CONTROLLER_OK:
        case OPTIONMODE_PASSWORD_OK:
        case OPTIONMODE_EXIT:
            m_bExtExitPipe = true;
            break;

        default:
            m_bExtExitPipe = false;
            break;
        };
    };
#endif /* TMNT2_BUILD_EU */
};


static COptions* s_pOptions = nullptr;


/*static*/ CProcess* COptionsSequence::Instance(void)
{
    return new COptionsSequence;
};


COptionsSequence::COptionsSequence(void)
: CAnim2DSequence("BG")
{
    ;
};


COptionsSequence::~COptionsSequence(void)
{
    ;
};


bool COptionsSequence::OnAttach(const void* pParam)
{
    IGamepad::SaveLockedState();

    if (!s_pOptions)
    {
        s_pOptions = new COptions();
        s_pOptions->Attach();
    };

    return CAnim2DSequence::OnAttach(FPATH("Common/Menu/Option/Options_PS2.lpac"));
};


void COptionsSequence::OnDetach(void)
{
    if (s_pOptions)
    {
        s_pOptions->Detach();
        delete s_pOptions;
        s_pOptions = nullptr;
    };

    CAnim2DSequence::OnDetach();

    IGamepad::RestoreLockedState();
};


void COptionsSequence::OnMove(bool bRet, const void* pReturnValue)
{
    CAnim2DSequence::OnMove(bRet, pReturnValue);

    switch (AnimStep())
    {
    case ANIMSTEP_DRAW:
        if (s_pOptions->Move())
            BeginFadeOut();
        break;

    case ANIMSTEP_END:
        Ret();
        break;

    default:
        break;
    };
};


void COptionsSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();
    s_pOptions->Draw();
};


bool COptionsSequence::OnAttach(const char* pszFilename)
{
    return false;
};


void COptionsSequence::BeginFadeIn(void)
{
    s_pOptions->SettingInit(false);
    CAnim2DSequence::BeginFadeIn();
};