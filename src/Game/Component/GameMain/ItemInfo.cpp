#include "ItemInfo.hpp"


/*static*/ bool CItemInfo::IsValidID(ITEMID::VALUE idItem)
{
    return ((idItem >= ITEMID::ID_NONE && idItem <= ITEMID::ID_MISSILE) ||
            (idItem >= ITEMID::ID_CRY_RED && idItem < ITEMID::ID_MAX));
};