#include "GaugeManager.hpp"
#include "GaugeResult.hpp"
#include "GaugeStatus.hpp"
#include "GaugeMeter.hpp"

#include "Game/Sequence/Ride/Ride2D.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "System/Common/RenderState.hpp"


/*static*/ void CGaugeManager::Initialize(void)
{
    CGaugeStatus::Initialize();
    
    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
		CRide2D::Initialize();        
    }
    else
    {
        CGaugeResult::Initialize();
        CGaugeMeter::Initialize();
    };
};


/*static*/ void CGaugeManager::Terminate(void)
{
    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
		CRide2D::Terminate();
    }
    else
    {
        CGaugeMeter::Termiante();
        CGaugeResult::Terminate();
    };

    CGaugeStatus::Termiante();
};


/*static*/ void CGaugeManager::Period(void)
{
    CGaugeStatus::Period();
    
    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
		CRide2D::Period();
    }
    else
    {
        CGaugeMeter::Period();
        CGaugeResult::Period();
    };
};


/*static*/ void CGaugeManager::Draw(void)
{
    CGaugeStatus::Draw();

    if (CGameData::PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_RIDE)
    {
		CRide2D::Draw();
    }
    else
    {
        CGaugeMeter::Draw();
        CGaugeResult::Draw();
    };
};


/*static*/ void CGaugeManager::SetGaugeAlphaMode(ALPHAMODE mode)
{
    switch (mode)
    {
    case ALPHAMODE_SUB:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDZERO);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;

    case ALPHAMODE_ADD:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        break;
        
    default:
    case ALPHAMODE_ALPHA:
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, true);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
        break;
    };
};