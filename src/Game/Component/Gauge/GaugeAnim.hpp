#pragma once

#include "System/Common/Screen.hpp"


#define GAUGE_ANIM_FRAMERATE                  (60.0f)

#define GAUGE_ANIM_DURATION_FRAMES(frames)    ( (CScreen::Framerate()) * ((static_cast<float>(frames)) / GAUGE_ANIM_FRAMERATE) )

