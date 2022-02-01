#include "GimmickDebug.hpp"


/*static*/ RwRGBA CGimmickDebug::COLOR_AREA_SPHERE  = { 0xFF, 0x00, 0x00, 0x80 };
/*static*/ RwRGBA CGimmickDebug::COLOR_AREA_BOX     = { 0x00, 0xFF, 0x00, 0x80 };
/*static*/ RwRGBA CGimmickDebug::COLOR_AREA_WALL    = { 0x00, 0x00, 0xFF, 0x80 };
/*static*/ bool CGimmickDebug::SHOW_MODEL           = false;
/*static*/ bool CGimmickDebug::SHOW_AREA            = true;
/*static*/ bool CGimmickDebug::SHOW_ATTACK          = false;
/*static*/ bool CGimmickDebug::SHOW_CATCH           = false;
/*static*/ bool CGimmickDebug::SHOW_ME              = true;