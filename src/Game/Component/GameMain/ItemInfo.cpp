#include "ItemInfo.hpp"


/*static*/ bool CItemInfo::IsValidID(ITEMID::VALUE idItem)
{
    if ((idItem >= ITEMID::ID_NONE) &&
        (idItem <= ITEMID::ID_MISSILE))
        return true;

    if ((idItem >= ITEMID::ID_CRY_RED) &&
        (idItem <  ITEMID::ID_MAX))
        return true;

    return false;
};