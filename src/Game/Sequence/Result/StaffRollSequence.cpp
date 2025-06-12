#include "StaffRollSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/File/FileID.hpp"


struct CREDITINFO
{
    enum TYPE
    {
        TYPE_EMPTYLINE = -1,

        TYPE_TEXT = 0,
        TYPE_TITLE,
        TYPE_TEAM,
    };

    GAMETEXT textId;
    float    height;
    TYPE     type;
};


static const CREDITINFO s_aStaffRollText[] =
{
    /*
     *  A Kengo Nakamura GAME
     */
    {   GAMETEXT_CREDIT_0,      18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },


    /*
     *  Konami Computer Entertainment Studios, Inc.
     */
    {   GAMETEXT_CREDIT_1,      16.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_CREDIT_2,      16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_3,      16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_4,      16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_5,      16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_6,      18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_7,      16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_8,      16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_9,      18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_10,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_11,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_12,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_13,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_14,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_15,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_16,     18.5f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_17,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_18,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_19,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_20,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_21,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_22,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_23,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_24,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_25,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_26,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_27,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_28,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_29,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_30,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_31,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_32,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_33,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_34,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_35,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_36,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_37,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_38,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_39,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_40,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_41,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_42,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_43,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_44,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_45,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_46,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_47,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_48,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_49,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_50,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_51,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_52,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_53,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_54,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_55,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_56,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_57,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_58,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_59,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_60,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_61,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_62,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_63,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_64,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_65,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_66,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_67,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_68,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_69,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_70,     18.5f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_71,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_72,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_73,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_74,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_75,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_76,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_77,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_78,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_79,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_80,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_81,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_82,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_83,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_84,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_85,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_86,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_87,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_88,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_89,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_90,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_91,     16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_92,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_93,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_94,     16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_95,     14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_96,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_97,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_98,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_99,     18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_100,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_101,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_102,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_103,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_104,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_105,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_106,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_107,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_108,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_109,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_110,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_111,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_112,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_113,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_114,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_115,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_116,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_117,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_118,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_119,    18.5f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_120,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_121,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_122,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_123,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_124,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_125,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_126,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_127,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_128,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_129,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_130,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_131,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_132,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_133,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_134,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_135,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_136,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_137,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_138,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_139,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_140,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_141,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_142,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_143,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_144,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_145,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_146,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_147,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_148,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_149,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_150,    18.5f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_151,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_152,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_153,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_154,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_155,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_156,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_157,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_158,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_159,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_160,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_161,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_162,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_163,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_164,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_165,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_166,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_167,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_168,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_169,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_170,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_171,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_172,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_173,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_174,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_175,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_176,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_177,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_178,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_179,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_180,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_181,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_182,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_183,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_184,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_185,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_186,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_187,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_188,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },

    
#ifdef TMNT2_BUILD_EU
    /*
     *  Konami of Europe
     */
    {   GAMETEXT_EU_CREDIT_0,   18.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_EU_CREDIT_1,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_1,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_1,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_2,   14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_3,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_4,   14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_5,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_6,   14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_7,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_8,   14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_9,   16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_10,  14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_11,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_12,  14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_13,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_14,  14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_15,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_16,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_17,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_18,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_19,  14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_20,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EU_CREDIT_21,  14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_EU_CREDIT_22,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_23,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_24,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_25,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_26,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_27,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_28,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EU_CREDIT_29,  16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
#else /* TMNT2_BUILD_EU */
    /*
     *  Konami Digital Entertainment, Inc.
     */
    {   GAMETEXT_CREDIT_189,    18.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_CREDIT_190,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_191,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_192,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_193,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_194,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_195,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_196,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_197,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_198,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_199,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_200,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_201,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_202,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_203,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_204,    14.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_205,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_206,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_207,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_208,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_209,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_210,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_211,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_212,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_213,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_214,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_215,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_216,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_217,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_218,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_219,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_220,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_221,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_222,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_223,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_224,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_225,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_226,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_227,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_228,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_229,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_230,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_231,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_232,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_233,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_234,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_235,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_236,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_237,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_238,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_239,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_240,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_241,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_242,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_243,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_244,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_245,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_246,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_247,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_248,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_249,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_250,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_251,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_252,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_253,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
#endif /* TMNT2_BUILD_EU */

    
    /*
     *  KONAMI CORPORATION
     */
    {   GAMETEXT_CREDIT_254,    18.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_CREDIT_255,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_256,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_257,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_258,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_259,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_260,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_261,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_262,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_263,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_264,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_265,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_266,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_267,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_268,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_269,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_270,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_271,    16.0f,      CREDITINFO::TYPE_TITLE      },
#if !defined(TMNT2_BUILD_EU)
    {   GAMETEXT_CREDIT_272,    16.0f,      CREDITINFO::TYPE_TEXT       },
#endif /* !defined(TMNT2_BUILD_EU) */
    {   GAMETEXT_CREDIT_273,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_274,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_275,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_276,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_277,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_278,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_279,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_280,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_281,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_282,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_283,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_284,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_285,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_286,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_287,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    
    
    /*
     *  4Kids Entertainment, Inc.
     */
    {   GAMETEXT_CREDIT_291,    18.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_CREDIT_292,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_293,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_294,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_295,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_296,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_297,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    
    
    /*
     *  4Kids Productions, Inc.
     */
    {   GAMETEXT_CREDIT_298,    18.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_299,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_300,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_301,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_302,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_303,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_304,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_305,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_306,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_307,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_308,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_309,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_310,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_311,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_312,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_313,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_314,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_315,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_316,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },

    
    /*
     *  Mirage Studious, Inc
     */
    {   GAMETEXT_CREDIT_317,    18.0f,      CREDITINFO::TYPE_TEAM       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_318,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_319,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_320,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_321,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_322,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_323,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_324,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_325,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_326,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_EMPTY,         16.0f,      CREDITINFO::TYPE_EMPTYLINE  },
    {   GAMETEXT_CREDIT_327,    16.0f,      CREDITINFO::TYPE_TITLE      },
    {   GAMETEXT_CREDIT_328,    16.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT_CREDIT_329,    18.0f,      CREDITINFO::TYPE_TEXT       },
    {   GAMETEXT(-1),           0.0f,       CREDITINFO::TYPE_TEXT       },
};



static const RwRGBA COLOR_WHITE  = { 0xFF, 0xFF, 0xFF, 0xFF };
static const RwRGBA COLOR_ORANGE = { 0xFF, 0x7C, 0x00, 0xFF };
static const RwRGBA COLOR_BLUE   = { 0x00, 0x98, 0xFA, 0xFF };


class CStaffRoll
{
private:
    class CCreditText
    {
    public:
        float            fOfsX;
        float            fOfsY;
        float            fHeight;
        const wchar*     pwszText;
        CREDITINFO::TYPE creditType;
    };

    enum PHASE
    {
        PHASE_LOAD = 0,
        PHASE_STAFF_START,
        PHASE_STAFF_STEP,
        PHASE_RW_START,
        PHASE_RW_STEP,
        PHASE_EOL,
    };

public:
    CStaffRoll(void);
    ~CStaffRoll(void);
    void Attach(void);
    void Detach(void);
    bool Move(void);
    void Draw(void);
    void StaffRollStart(void);
    void StaffRollEnd(void);
    bool StaffRollStep(void);
    void StaffRollDraw(void);
    void StaffRollUpdate(void);
    void RwLogoStart(void);
    void RwLogoEnd(void);
    void RwLogoDraw(void);
    bool RwLogoStep(void);
    void DrawCredit(int32 iNodeIndex);
    void SetTextToDrawList(int32 iNodeIndex, int32 iCreditIndex);
    
private:
    PHASE       m_ePhase;
    int32       m_iStep;
    float       m_fTimer;
    int32       m_iStaffRollCur;
    int32       m_iStaffRollDisp;
    int32       m_iStaffRollFinishNodeIndex;
    bool        m_bStaffRollRunFlag;
    bool        m_bStaffRollFinishFlag;
    CCreditText m_aCreditDrawList[32];
    CSprite     m_logoSprite;
    float       m_fVSW;
    float       m_fVSH;
};


CStaffRoll::CStaffRoll(void)
: m_ePhase(PHASE_LOAD)
, m_iStep(0)
, m_fTimer(0.0f)
, m_iStaffRollCur(0)
, m_iStaffRollDisp(0)
, m_iStaffRollFinishNodeIndex(0)
, m_aCreditDrawList()
, m_logoSprite()
, m_fVSW(0.0f)
, m_fVSH(0.0f)
{
    ;
};


CStaffRoll::~CStaffRoll(void)
{
    ;
};


void CStaffRoll::Attach(void)
{
    m_fVSW = (CSprite::m_fVirtualScreenW);
    m_fVSH = (CSprite::m_fVirtualScreenH);
    
    CGameData::Attribute().SetInteractive(false);
    CTextureManager::GenerationInc();
    CDataLoader::Regist(FILEID::ID_LOGO);
    m_ePhase = PHASE_LOAD;
};


void CStaffRoll::Detach(void)
{
    CTextureManager::GenerationDec();
    CGameSound::Stop();
};


bool CStaffRoll::Move(void)
{
    switch (m_ePhase)
    {
    case PHASE_LOAD:
        {
            CDataLoader::Period();            
            if (CDataLoader::IsLoadEnd())
                m_ePhase = PHASE_STAFF_START;            
        }
        break;
        
    case PHASE_STAFF_START:
        {
            StaffRollStart();
            m_ePhase = PHASE_STAFF_STEP;
        }
        break;
        
    case PHASE_STAFF_STEP:
        {
            if (StaffRollStep())
            {
                StaffRollEnd();
                m_ePhase = PHASE_RW_START;
            };
        }
        break;
        
    case PHASE_RW_START:
        {
            RwLogoStart();
            m_ePhase = PHASE_RW_STEP;
        }
        break;
        
    case PHASE_RW_STEP:
        {
            if (RwLogoStep())
            {
                RwLogoEnd();
                m_ePhase = PHASE_EOL;
            };
        }
        break;

    default:
        break;
    };

    return (m_ePhase == PHASE_EOL ? true : false);
};


void CStaffRoll::Draw(void)
{
    switch (m_ePhase)
    {
    case PHASE_STAFF_STEP:
        StaffRollDraw();
        break;

    case PHASE_RW_STEP:
        RwLogoDraw();
        break;

    default:
        break;
    };
};


void CStaffRoll::StaffRollStart(void)
{
    CScreen::SetClearColor({ 0x00, 0x00, 0x00 , 0xFF });

    CGameFont::SetRGBA(COLOR_WHITE);
    for (int32 i = 0; i < COUNT_OF(m_aCreditDrawList); ++i)
    {
        m_aCreditDrawList[i].pwszText = nullptr;
        m_aCreditDrawList[i].fHeight = 1.0f;
        
        if (s_aStaffRollText[i].textId == GAMETEXT(-1))
            break;

        SetTextToDrawList(i, i);
        
        if (i)
        {
            m_aCreditDrawList[i].fOfsY =
                (m_aCreditDrawList[i - 1].fOfsY + m_aCreditDrawList[i - 1].fHeight) + 8.0f;
        }
        else
        {
            m_aCreditDrawList[0].fOfsY = (m_fVSH * 0.5f);
        };
        
        m_iStaffRollCur = i;
    };

    m_bStaffRollRunFlag = true;
    m_bStaffRollFinishFlag = false;
    m_iStaffRollDisp = 0;
    m_iStaffRollFinishNodeIndex = 0;
    m_fTimer = 30.0f;
    m_iStep = 0;
    
    CGameSound::PlayBGM(SDCODE_BGM(0x3025));
};


void CStaffRoll::StaffRollEnd(void)
{
    CGameSound::Stop();
};


bool CStaffRoll::StaffRollStep(void)
{
    switch (m_iStep)
    {
    case 0:
        {
            StaffRollUpdate();
            if (m_bStaffRollFinishFlag)
                ++m_iStep;
        }
        break;

    case 1:
        {
            m_fTimer -= CScreen::TimerStride();
            if (m_fTimer <= 0.0f)
            {
                ++m_iStep;
                break;
            };

            if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(),
                                               CController::DIGITAL_OK | CController::DIGITAL_CANCEL))
            {
                ++m_iStep;
                break;
            };
        }
        break;

    case 2:
        {
            CScreenFade::BlackOut();
            ++m_iStep;
        }
        break;

    case 3:
        {
            if (!CScreenFade::IsFading())
                ++m_iStep;
        }
        break;

    default:
        break;
    };

    return (m_iStep >= 4);
};


void CStaffRoll::StaffRollDraw(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aCreditDrawList); ++i)
        DrawCredit(i);
};


void CStaffRoll::StaffRollUpdate(void)
{
    float fRollSpeed = 1.3f;

#ifdef _DEBUG
    uint32 Digital = 0;
    Digital |= CController::GetDigital(CController::CONTROLLER_LOCKED_ON_VIRTUAL);
    Digital |= CController::GetDigital(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL);
    if (Digital & CController::DIGITAL_LDOWN)
        fRollSpeed = 10.0f;
#endif /* _DEBUG */

    for (int32 i = 0; i < COUNT_OF(m_aCreditDrawList); ++i)
    {
        if (!m_bStaffRollFinishFlag)
            m_aCreditDrawList[i].fOfsY -= fRollSpeed;

        if (m_bStaffRollRunFlag)
        {
            int32 idx = (COUNT_OF(m_aCreditDrawList) - 1);
            if (i)
            {
                idx = (i - 1);
            };
            
            if (m_aCreditDrawList[i].fOfsY <= -(m_fVSH * 0.5f))
            {
                ++m_iStaffRollCur;
                if (s_aStaffRollText[m_iStaffRollCur].textId == GAMETEXT(-1))
                {
                    m_bStaffRollRunFlag = false;
                    m_iStaffRollFinishNodeIndex = idx;
                }
                else
                {
                    SetTextToDrawList(i, m_iStaffRollCur);
                    m_aCreditDrawList[i].fOfsY = (m_aCreditDrawList[idx].fOfsY + m_aCreditDrawList[idx].fHeight) + 8.0f;
                };
            };
        }
        else if (m_aCreditDrawList[m_iStaffRollFinishNodeIndex].fOfsY <= 0.0f)
        {
            m_bStaffRollFinishFlag = true;
        };
    };

    if (!m_bStaffRollFinishFlag)
        ++m_iStaffRollDisp;
};


void CStaffRoll::RwLogoStart(void)
{
    m_iStep = 0;
    m_fTimer = 2.2f;

    float sw = static_cast<float>(CScreen::Width());
    float sh = static_cast<float>(CScreen::Height());

    m_logoSprite.SetPositionAndSizeRealScreen(sw * 0.5f, sh * 0.5f, sw, sh);
    m_logoSprite.SetTexture(CTextureManager::GetRwTexture("tit_rendlogo"));
    m_logoSprite.SetUV(0.1875f, 0.0625f, 0.8125f, 0.9375f);
    m_logoSprite.SetRGBA(255, 255, 255, 255);
};


void CStaffRoll::RwLogoEnd(void)
{
    ;
};


void CStaffRoll::RwLogoDraw(void)
{
    CSystem2D::PushRenderState();
    m_logoSprite.Draw();
    CSystem2D::PopRenderState();
};


bool CStaffRoll::RwLogoStep(void)
{
    switch (m_iStep)
    {
    case 0:
        {
            CScreenFade::BlackIn(1.0f);
            ++m_iStep;
        }
        break;

    case 1:
        {
            if (!CScreenFade::IsFading())
                ++m_iStep;
        }
        break;

    case 2:
        {
            if (m_fTimer <= 0.0f)
                m_iStep = 3;

            if (m_fTimer <= 0.0f)
            {
                if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_OK | CController::DIGITAL_CANCEL))
                    m_iStep = 3;
            };

            m_fTimer -= CScreen::TimerStride();
        }
        break;

    case 3:
        {
            CScreenFade::BlackOut(1.0f);
            ++m_iStep;
        }
        break;

    case 4:
        {
            if (!CScreenFade::IsFading())
                ++m_iStep;
        }
        break;

    default:
        break;
    };

    return (m_iStep >= 5);
};


void CStaffRoll::DrawCredit(int32 iNodeIndex)
{
    CCreditText* pCreditText = &m_aCreditDrawList[iNodeIndex];
    if (pCreditText->fOfsY > -(m_fVSH * 0.5f))
    {
        if ((pCreditText->fHeight + (m_fVSH * 0.5f)) >= pCreditText->fOfsY)
        {
            switch (pCreditText->creditType)
            {
            case CREDITINFO::TYPE_TEXT:
                CGameFont::SetRGBA(COLOR_WHITE);
                break;

            case CREDITINFO::TYPE_TITLE:
                CGameFont::SetRGBA(COLOR_BLUE);
                break;

            case CREDITINFO::TYPE_TEAM:
                CGameFont::SetRGBA(COLOR_ORANGE);
                break;

            default:
                break;
            };

			CGameFont::SetHeight(pCreditText->fHeight);
            CGameFont::Show(pCreditText->pwszText, pCreditText->fOfsX, pCreditText->fOfsY);            
        };
    };
};


void CStaffRoll::SetTextToDrawList(int32 iNodeIndex, int32 iCreditIndex)
{
    m_aCreditDrawList[iNodeIndex].fHeight     = s_aStaffRollText[iCreditIndex].height;
    m_aCreditDrawList[iNodeIndex].pwszText    = CGameText::GetText(s_aStaffRollText[iCreditIndex].textId);
    m_aCreditDrawList[iNodeIndex].creditType  = s_aStaffRollText[iCreditIndex].type;
	CGameFont::SetHeight(m_aCreditDrawList[iNodeIndex].fHeight);
    m_aCreditDrawList[iNodeIndex].fOfsX       = (CGameFont::GetStringWidth(m_aCreditDrawList[iNodeIndex].pwszText) * -0.5f);
};


static CStaffRoll* s_pStaffRoll = nullptr;


/*static*/ CProcess* CStaffRollSequence::Instance(void)
{
    return new CStaffRollSequence;
};


CStaffRollSequence::CStaffRollSequence(void)
{
    ;
};


CStaffRollSequence::~CStaffRollSequence(void)
{
    ;
};


bool CStaffRollSequence::OnAttach(const void* pParam)
{
    ASSERT(!s_pStaffRoll);
    
    s_pStaffRoll = new CStaffRoll;
    s_pStaffRoll->Attach();
    
    return true;
};


void CStaffRollSequence::OnDetach(void)
{
    if (s_pStaffRoll)
    {
        s_pStaffRoll->Detach();

        delete s_pStaffRoll;
        s_pStaffRoll = nullptr;
    };
};


void CStaffRollSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (s_pStaffRoll->Move())
        Ret();
};


void CStaffRollSequence::OnDraw(void) const
{
    s_pStaffRoll->Draw();
};