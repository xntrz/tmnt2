#include "GimmickDebug.hpp"

#ifdef _DEBUG
/*static*/ RwRGBA CGimmickDebug::COLOR_AREA_SPHERE  = { 0xFF, 0x00, 0x00, 0x80 };
/*static*/ RwRGBA CGimmickDebug::COLOR_AREA_BOX     = { 0x00, 0xFF, 0x00, 0x80 };
/*static*/ RwRGBA CGimmickDebug::COLOR_AREA_WALL    = { 0x00, 0x00, 0xFF, 0x80 };
/*static*/ bool CGimmickDebug::SHOW_MODEL           = false;
/*static*/ bool CGimmickDebug::SHOW_AREA            = false;
/*static*/ bool CGimmickDebug::SHOW_ME              = false;
/*static*/ bool CGimmickDebug::DISABLE_GENERERATOR  = false;
#endif