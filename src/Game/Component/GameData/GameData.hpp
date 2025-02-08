#pragma once

#include "GameAttribute.hpp"
#include "GamePlayParam.hpp"
#include "GamePlayResult.hpp"
#include "GameRecord.hpp"
#include "OptionData.hpp"

#include "Game/Component/GameMain/StageID.hpp"


struct TMNT2_SAVE_DATA;


/**
 *  TODO: character result counts right when no enemies but required tests when all enemies done
 */
class CGameData
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Load(const TMNT2_SAVE_DATA& savedata);
    static void Save(TMNT2_SAVE_DATA& savedata);
    static bool IsNewGame(void);
    static void ClearNewGameFlag(void);
    static void OnNewGame(void);
    static void OnBeginArea(void);
    static void OnBeginStage(STAGEID::VALUE idStage);
    static void OnEndStage(STAGEID::VALUE idStage);
    static void PushRecord(void);
    static void PopRecord(void);
    static void PushOption(void);
    static void PopOption(void);
    static void OnPreResult(void);
    static void OnPostResult(void);
    static void OnEndEnding(void);
    static void OnEndAntiqueShop(void);
    static CGameAttribute& Attribute(void);
    static CGamePlayParam& PlayParam(void);
    static CGamePlayResult& PlayResult(void);
    static CGameRecord& Record(void);
    static COptionData& Option(void);
    
private:
    static CGamePlayResult m_playresult;
    static CGamePlayParam m_playparam;
    static CGameAttribute m_attribute;
    static CGameRecord* m_pRecord;
    static COptionData* m_pOption;
    static bool m_bNewGame;
};