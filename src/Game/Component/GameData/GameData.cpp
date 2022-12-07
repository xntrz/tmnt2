#include "GameData.hpp"

#include "Game/System/SaveLoad/SaveLoadTMNT2.hpp"
#include "Game/Component/GameMain/SecretInfo.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"

#ifdef _TARGET_PC
#include "System/PC/PCSetting.hpp"
#endif


static CGameRecord::RAWDATA s_RecordBackup;
static COptionData::RAWDATA s_OptionBackup;

/*static*/ CGamePlayResult CGameData::m_playresult;
/*static*/ CGamePlayParam CGameData::m_playparam;
/*static*/ CGameAttribute CGameData::m_attribute;
/*static*/ bool CGameData::m_bNewGame = false;
/*static*/ CGameRecord* CGameData::m_pRecord = nullptr;
/*static*/ COptionData* CGameData::m_pOption = nullptr;


/*static*/ void CGameData::Initialize(void)
{
    m_pOption = new COptionData;
    ASSERT(m_pOption);
    
    m_pRecord = new CGameRecord;
    ASSERT(m_pRecord);
    
    m_pOption->Initialize();
    m_pRecord->Initialize();

    m_attribute.Clear();

    m_pRecord->Snapshot(s_RecordBackup);
    m_pOption->Snapshot(s_OptionBackup);
    
    m_bNewGame = true;
};


/*static*/ void CGameData::Terminate(void)
{
    if (m_pRecord)
    {
        m_pRecord->Terminate();
        delete m_pRecord;
        m_pRecord = nullptr;
    };

    if (m_pOption)
    {
        m_pOption->Terminate();
        delete m_pOption;
        m_pOption = nullptr;
    };
};


/*static*/ void CGameData::Load(const TMNT2_SAVE_DATA& savedata)
{
    Record().Restore(savedata.m_record, true);
    Option().Restore(savedata.m_option);

    Option().Apply();
    
    m_bNewGame = Record().Area().IsNewGame();

#ifdef _TARGET_PC
    //
    //  TODO remove me when OptionSequence is done
    //
    //Option().Play().SetClassicInput(CPCSetting::m_bFlagClassicPad);
    //Option().Play().SetEnableAutosave(CPCSetting::m_bFlagAutosave);
    //Option().Play().SetEnableHelp(CPCSetting::m_bFlagHelp);
    //Option().Play().SetDifficulty(GAMETYPES::DIFFICULTY(CPCSetting::m_iDifficulty));
    //Option().Display().SetEnableFontEffect(CPCSetting::m_bFlagFontEffect);
#endif    
};


/*static*/ void CGameData::Save(TMNT2_SAVE_DATA& savedata)
{
    Record().Snapshot(savedata.m_record);
    Option().Snapshot(savedata.m_option);
};


/*static*/ bool CGameData::IsNewGame(void)
{
    return m_bNewGame;
};


/*static*/ void CGameData::ClearNewGameFlag(void)
{
    m_bNewGame = false;
};


/*static*/ void CGameData::OnNewGame(void)
{
    m_bNewGame = true;    
    Record().Area().OnNewGame();    
    PlayParam().ClearArea();
    PlayParam().SetStage(STAGEID::ID_ST01T);
};


/*static*/ void CGameData::OnBeginArea(void)
{
    PlayParam().InitForArea();
    PlayResult().InitForArea();
    CSecretInfo::CheckStartArea(PlayParam().GetArea());
};


/*static*/ void CGameData::OnBeginStage(STAGEID::VALUE idStage)
{
    PlayParam().InitForStage();
    PlayResult().InitForStage();

    if (!Attribute().IsPlayDemoMode())
    {
        CSecretInfo::CheckStartStage(idStage);
        
        if (PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_NEXUS)
        {
            GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(PlayParam().GetArea());
            Record().Nexus().SetStagePlayed(idNexus, PlayParam().GetStageIndex());
        };
    };
};


/*static*/ void CGameData::OnEndStage(STAGEID::VALUE idStage)
{
    PlayResult().TermForStage();

    if (!Attribute().IsPlayDemoMode())
    {
        CSecretInfo::CheckEndStage(idStage);

        if (PlayParam().GetStageMode() == GAMETYPES::STAGEMODE_NEXUS)
        {
            GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(PlayParam().GetArea());
            Record().Nexus().UpdateStageClearTime(
                idNexus,
                PlayParam().GetStageIndex(),
                PlayResult().GetStageCleartime(PlayParam().GetStageIndex())
            );
        };
    };
};


/*static*/ void CGameData::PushRecord(void)
{
    m_pRecord->Snapshot(s_RecordBackup);
};


/*static*/ void CGameData::PopRecord(void)
{
    m_pRecord->Restore(s_RecordBackup, false);
};


/*static*/ void CGameData::PushOption(void)
{
    m_pOption->Snapshot(s_OptionBackup);
};


/*static*/ void CGameData::PopOption(void)
{
    m_pOption->Restore(s_OptionBackup);
};


/*static*/ void CGameData::OnPreResult(void)
{
    PlayResult().Evaluate();
    
    if (PlayResult().GetAreaResult() == CGamePlayResult::AREARESULT_CLEAR)
        PlayResult().TakePrize(PlayParam().GetArea());    
};


/*static*/ void CGameData::OnPostResult(void)
{
    PlayResult().ApplyToRecord();
    Record().Area().RegistNewArea();
    CSecretInfo::CheckEndArea(PlayParam().GetArea());
};


/*static*/ void CGameData::OnEndEnding(void)
{
    CSecretInfo::CheckEnding();
};


/*static*/ void CGameData::OnEndAntiqueShop(void)
{
    CSecretInfo::CheckAntiqueSequence();
};


/*static*/ CGameAttribute& CGameData::Attribute(void)
{
    return m_attribute;
};


/*static*/ CGamePlayParam& CGameData::PlayParam(void)
{
    return m_playparam;
};


/*static*/ CGamePlayResult& CGameData::PlayResult(void)
{
    return m_playresult;
};


/*static*/ CGameRecord& CGameData::Record(void)
{
    ASSERT(m_pRecord);
    return *m_pRecord;
};


/*static*/ COptionData& CGameData::Option(void)
{
    ASSERT(m_pOption);
    return *m_pOption;
};