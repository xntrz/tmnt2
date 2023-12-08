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

    GAMETEXT m_idText;
    float m_fHeight;
    TYPE m_type;
};


static const CREDITINFO s_aStaffRollText[] =
{
    { GAMETEXT(0x0E8),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    
    { GAMETEXT(0x0E9),   16.0f,  CREDITINFO::TYPE_TEAM       },
    { GAMETEXT(0x0EA),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0EB),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0EC),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0ED),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0EE),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0EF),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0F0),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0F1),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0F2),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0F3),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0F4),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0F5),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0F6),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0F7),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0F8),   18.5f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0F9),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0FA),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0FB),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0FC),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0FD),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0FE),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0FF),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x100),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x101),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x102),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x103),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x104),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x105),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x106),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x107),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x108),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x109),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x10A),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x10B),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x10C),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x10D),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x10E),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x10F),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x110),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x111),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x112),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x113),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x114),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x115),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x116),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x117),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x118),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x119),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x11A),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x11B),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x11C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x11D),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x11E),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x11F),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x120),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x121),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x122),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x123),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x124),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x125),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x126),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x127),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x128),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x129),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x12A),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x12B),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x12C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x12D),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x12E),   18.5f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x12F),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x130),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x131),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x132),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x133),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x134),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x135),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x136),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x137),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x138),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x139),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x13A),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x13B),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x13C),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x13D),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x13E),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x13F),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x140),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x141),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x142),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(143),     16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x144),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x145),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x146),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x147),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x148),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x149),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x14A),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x14B),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x14C),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x14D),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x14E),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x14F),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x150),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x151),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x152),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x153),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x154),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x155),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x156),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x157),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x158),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x159),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x15A),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x15B),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x15C),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x15D),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x15E),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x15F),   18.5f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x160),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x161),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x162),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x163),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x164),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x165),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x166),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x167),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x168),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x169),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x16A),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x16B),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x16C),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x16D),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x16E),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x16F),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x170),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x171),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x172),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x173),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x174),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x175),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x176),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x177),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x178),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x179),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x17A),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x17B),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x17C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x17D),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x17E),   18.5f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x17F),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x180),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x181),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x182),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x183),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x184),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x185),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x186),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x187),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x188),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x189),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x18A),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x18B),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x18C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x18D),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x18E),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x18F),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x190),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x191),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x192),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x193),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x194),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x195),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x196),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x197),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x198),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x199),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x19A),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x19B),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x19C),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x19D),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x19E),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x19F),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A0),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A1),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A2),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A3),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1A4),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    
    { GAMETEXT(0x1A5),   18.0f,  CREDITINFO::TYPE_TEAM       },
    { GAMETEXT(0x1A6),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A7),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A8),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1A9),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1AA),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1AB),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1AC),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1AD),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1AE),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1AF),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1B0),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1B1),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1B2),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x1B3),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1B4),   14.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1B5),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1B6),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1B7),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1B8),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1B9),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1BA),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1BB),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1BC),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1BD),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1BE),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1BF),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C0),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1C1),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C2),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C3),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1C4),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C5),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C6),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1C7),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C8),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1C9),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1CA),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1CB),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1CC),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1CD),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1CE),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1CF),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1D0),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1D1),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1D2),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1D3),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1D4),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1D5),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1D6),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1D7),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1D8),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1D9),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1DA),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1DB),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1DC),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1DD),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1DE),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1DF),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E0),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E1),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1E2),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E3),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E4),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E5),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    
    { GAMETEXT(0x1E6),   18.0f,  CREDITINFO::TYPE_TEAM       },
    { GAMETEXT(0x1E7),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E8),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1E9),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1EA),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1EB),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1EC),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1ED),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1EE),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1EF),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1F0),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1F1),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1F2),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1F3),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1F4),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1F5),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1F6),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x1F7),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x1F8),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1F9),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1FA),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1FB),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1FC),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1FD),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1FE),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x1FF),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x200),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x201),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x202),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x203),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x204),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x205),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x206),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x207),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    
    { GAMETEXT(0x20B),   18.0f,  CREDITINFO::TYPE_TEAM       },
    { GAMETEXT(0x20C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x20D),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x20E),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x20F),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x210),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x211),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    
    { GAMETEXT(0x212),   18.0f,  CREDITINFO::TYPE_TEAM       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x213),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x214),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x215),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x216),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x217),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x218),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x219),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x21A),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x21B),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x21C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x21D),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x21E),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x21F),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x220),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x221),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x222),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x223),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x224),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    
    { GAMETEXT(0x225),   18.0f,  CREDITINFO::TYPE_TEAM       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x226),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x227),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x228),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x229),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x22A),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x22B),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x22C),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x22D),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x22E),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x0),     16.0f,  CREDITINFO::TYPE_EMPTYLINE  },
    { GAMETEXT(0x22F),   16.0f,  CREDITINFO::TYPE_TITLE      },
    { GAMETEXT(0x230),   16.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(0x231),   18.0f,  CREDITINFO::TYPE_TEXT       },
    { GAMETEXT(-1),      0.0f,   CREDITINFO::TYPE_TEXT       }, 
};


static const RwRGBA COLOR_WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };
static const RwRGBA COLOR_ORANGE = { 0xFF, 0x7C, 0x00, 0xFF };
static const RwRGBA COLOR_BLUE = { 0x00, 0x98, 0xFA, 0xFF };


class CStaffRoll
{
private:
    class CCreditText
    {
    public:
        float m_fOfsX;
        float m_fOfsY;
        float m_fHeight;
        const wchar* m_pwszText;
        CREDITINFO::TYPE m_credittype;
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
    PHASE m_ePhase;
    int32 m_iStep;
    float m_fTimer;
    int32 m_iStaffRollCur;
    int32 m_iStaffRollDisp;
    int32 m_iStaffRollFinishNodeIndex;
    bool m_bStaffRollRunFlag;
    bool m_bStaffRollFinishFlag;
    CCreditText m_aCreditDrawList[32];
    CSprite m_LogoSprite;
    float m_fVSW;
    float m_fVSH;
};


CStaffRoll::CStaffRoll(void)
: m_ePhase(PHASE_LOAD)
, m_iStep(0)
, m_fTimer(0.0f)
, m_iStaffRollCur(0)
, m_iStaffRollDisp(0)
, m_iStaffRollFinishNodeIndex(0)
, m_aCreditDrawList()
, m_LogoSprite()
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
    bool bResult = false;

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
        
    case PHASE_EOL:
        {
            bResult = true;
        }
        break;
    };

    return bResult;
};


void CStaffRoll::Draw(void)
{
    switch (m_ePhase)
    {
    case PHASE_STAFF_STEP:
        {
            StaffRollDraw();
        }
        break;

    case PHASE_RW_STEP:
        {
            RwLogoDraw();
        }
        break;
    };
};


void CStaffRoll::StaffRollStart(void)
{
    CScreen::SetClearColor({ 0x00, 0x00, 0x00 , 0xFF });

    CGameFont::SetRGBA(COLOR_WHITE);
    for (int32 i = 0; i < COUNT_OF(m_aCreditDrawList); ++i)
    {
        m_aCreditDrawList[i].m_pwszText = nullptr;
        m_aCreditDrawList[i].m_fHeight = 1.0f;
        
        if (s_aStaffRollText[i].m_idText == GAMETEXT(-1))
            break;

        SetTextToDrawList(i, i);
        
        if (i)
        {
            m_aCreditDrawList[i].m_fOfsY =
                (m_aCreditDrawList[i - 1].m_fOfsY + m_aCreditDrawList[i - 1].m_fHeight) + 8.0f;
        }
        else
        {
            m_aCreditDrawList[0].m_fOfsY = (m_fVSH * 0.5f);
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
    bool bResult = false;

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
            if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_OK | CController::DIGITAL_CANCEL)
                || (m_fTimer <= 0.0f))
            {                
                ++m_iStep;
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

    case 4:
        {
            bResult = true;
        }
        break;
    };

    return bResult;
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
    if (FLAG_TEST(Digital, CController::DIGITAL_LDOWN))
        fRollSpeed = 10.0f;
#endif    

    for (int32 i = 0; i < COUNT_OF(m_aCreditDrawList); ++i)
    {
        if (!m_bStaffRollFinishFlag)
            m_aCreditDrawList[i].m_fOfsY -= fRollSpeed;

        if (m_bStaffRollRunFlag)
        {
            int32 idx = (COUNT_OF(m_aCreditDrawList) - 1);
            if (i)
            {
                idx = (i - 1);
            };
            
            if (m_aCreditDrawList[i].m_fOfsY <= -(m_fVSH * 0.5f))
            {
                ++m_iStaffRollCur;
                if (s_aStaffRollText[m_iStaffRollCur].m_idText == GAMETEXT(-1))
                {
                    m_bStaffRollRunFlag = false;
                    m_iStaffRollFinishNodeIndex = idx;
                }
                else
                {
                    SetTextToDrawList(i, m_iStaffRollCur);
                    m_aCreditDrawList[i].m_fOfsY = (m_aCreditDrawList[idx].m_fOfsY + m_aCreditDrawList[idx].m_fHeight) + 8.0f;
                };
            };
        }
        else if (m_aCreditDrawList[m_iStaffRollFinishNodeIndex].m_fOfsY <= 0.0f)
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

    float sw = float(CScreen::Width());
    float sh = float(CScreen::Height());

    m_LogoSprite.SetPositionAndSizeRealScreen(sw * 0.5f, sh * 0.5f, sw, sh);
    m_LogoSprite.SetTexture(CTextureManager::GetRwTexture("tit_rendlogo"));
    m_LogoSprite.SetUV(0.1875f, 0.0625f, 0.8125f, 0.9375f);
    m_LogoSprite.SetRGBA(255, 255, 255, 255);
};


void CStaffRoll::RwLogoEnd(void)
{
    ;
};


void CStaffRoll::RwLogoDraw(void)
{
    CSystem2D::PushRenderState();
    m_LogoSprite.Draw();
    CSystem2D::PopRenderState();
};


bool CStaffRoll::RwLogoStep(void)
{
    bool bResult = false;

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

    case 5:
        {
            bResult = true;
        }
        break;
    };

    return bResult;
};


void CStaffRoll::DrawCredit(int32 iNodeIndex)
{
    CCreditText* pCreditText = &m_aCreditDrawList[iNodeIndex];
    if (pCreditText->m_fOfsY > -(m_fVSH * 0.5f))
    {
        if ((pCreditText->m_fHeight + (m_fVSH * 0.5f)) >= pCreditText->m_fOfsY)
        {
            switch (pCreditText->m_credittype)
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
            };

			CGameFont::SetHeight(pCreditText->m_fHeight);
            CGameFont::Show(pCreditText->m_pwszText, pCreditText->m_fOfsX, pCreditText->m_fOfsY);            
        };
    };
};


void CStaffRoll::SetTextToDrawList(int32 iNodeIndex, int32 iCreditIndex)
{
    m_aCreditDrawList[iNodeIndex].m_fHeight = s_aStaffRollText[iCreditIndex].m_fHeight;
    m_aCreditDrawList[iNodeIndex].m_pwszText = CGameText::GetText(s_aStaffRollText[iCreditIndex].m_idText);
    m_aCreditDrawList[iNodeIndex].m_credittype = s_aStaffRollText[iCreditIndex].m_type;
	CGameFont::SetHeight(m_aCreditDrawList[iNodeIndex].m_fHeight);
    m_aCreditDrawList[iNodeIndex].m_fOfsX = (CGameFont::GetStringWidth(m_aCreditDrawList[iNodeIndex].m_pwszText) * -0.5f);
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
    ASSERT(s_pStaffRoll);
    
    if (s_pStaffRoll)
    {
        s_pStaffRoll->Detach();
        delete s_pStaffRoll;
        s_pStaffRoll = nullptr;
    };
};


void CStaffRollSequence::OnMove(bool bRet, const void* pReturnValue)
{
    ASSERT(s_pStaffRoll);
    
    if (s_pStaffRoll->Move())
        Ret();
};


void CStaffRollSequence::OnDraw(void) const
{
    ASSERT(s_pStaffRoll);
    
    s_pStaffRoll->Draw();
};