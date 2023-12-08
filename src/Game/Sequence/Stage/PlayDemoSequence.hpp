#pragma once

#include "StageBaseSequence.hpp"

#include "Game/Component/GameMain/StageID.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "System/Common/Sprite.hpp"


class CPlayDemoSequence : public CStageBaseSequence
{
protected:
    struct STAGEINFO
    {
        STAGEID::VALUE m_idStage;
        int32 m_nNumPlayers;
    };
    
public:
    static CProcess* Instance(void);

    CPlayDemoSequence(void);
    virtual ~CPlayDemoSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    virtual void OnStateDetached(STATE state) override;
    void InitializeLogo(void);

protected:
    static PLAYERID::VALUE m_aPlayerTable[];
    static STAGEID::VALUE m_idStage;
    static const STAGEINFO m_aStageTable[];
    float m_fTime;
    int32 m_iStageIndex;
    CSprite m_LogoSprite;
};