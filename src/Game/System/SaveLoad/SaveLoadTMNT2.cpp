#include "SaveLoadTMNT2.hpp"


bool TMNT2_SAVE_DATA::IsValid(void) const
{
    CGameRecord gamerecordTemp;
    gamerecordTemp.Restore(m_record, true);

    return gamerecordTemp.IsValid();
};