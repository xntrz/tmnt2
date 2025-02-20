#include "GameStageDebug.hpp"

#ifdef _DEBUG

/*static*/ bool CGameStageDebug::GODMODE = false;
/*static*/ float CGameStageDebug::CAMERA_ZOOM_SCALE = 1.0f;
/*static*/ float CGameStageDebug::CAMERA_MANUAL_SPEED = 20.0f;
/*static*/ int32 CGameStageDebug::STAGE_TICK = 0;
/*static*/ int32 CGameStageDebug::COUNTER = 0;


/*static*/ void CGameStageDebug::Reset(void)
{
    GODMODE = false;
    CAMERA_ZOOM_SCALE = 1.0f;
    CAMERA_MANUAL_SPEED = 20.0f;
    STAGE_TICK = 0;
    COUNTER = 0;
};

#endif /* _DEBUG */