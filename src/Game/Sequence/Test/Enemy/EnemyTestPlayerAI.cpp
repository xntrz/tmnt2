#include "EnemyTestPlayerAI.hpp"

#ifdef _DEBUG

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Sequence/Test/DebugUtils.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/Process.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Controller.hpp"


static const int32 s_aSeqLabelToAddPlayerAI[] =
{
    PROCLABEL_SEQ_NEXUSSTAGE,
    PROCLABEL_SEQ_NORMALSTAGE,
    PROCLABEL_SEQ_ENEMYTESTSTAGE,
};


class CEnemyTestPlayerAI : public CProcess
{
private:
    struct INTERCEPTCTX
    {
        int32 label;        
    };

public:
    virtual bool Attach(void) override;
    virtual void Detach(void) override;
    virtual void Move(void) override;
    virtual void Draw(void) const override;
    void AddIntercept(int32 label);
    void OnStart(int32 label);
    void OnEnd(int32 label);

private:
    INTERCEPTCTX                    m_aInterceptCtx[32];
    int32                           m_numInterceptCtx;
    INTERCEPTCTX*                   m_pCurrentIntercept;
    bool                            m_bInSeq;
    CDebugSequenceCheckObj          m_seqViewObj;
    CGamePlayParam::PLAYERCONTEXT   m_aPlayerCtxSave[GAMETYPES::PLAYERS_MAX];
    int32                           m_numPlayerCtxSave;
};


bool CEnemyTestPlayerAI::Attach(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aInterceptCtx); ++i)
        m_aInterceptCtx[i].label = PROCESSTYPES::LABEL_EOL;
    
    m_numInterceptCtx   = 0;
    m_pCurrentIntercept = nullptr;
    m_bInSeq            = false;
    std::memset(m_aPlayerCtxSave, 0, sizeof(m_aPlayerCtxSave));
    m_numPlayerCtxSave = 0;

    for (int32 i = 0; i < COUNT_OF(s_aSeqLabelToAddPlayerAI); ++i)
        AddIntercept(s_aSeqLabelToAddPlayerAI[i]);

    return true;
};


void CEnemyTestPlayerAI::Detach(void)
{
    ;
};


void CEnemyTestPlayerAI::Move(void)
{
	m_seqViewObj.Update();

    if (m_bInSeq)
    {
        if (m_seqViewObj.Check(m_pCurrentIntercept->label).IsEnded())
        {
            OnEnd(m_pCurrentIntercept->label);

            m_pCurrentIntercept = nullptr;
            m_bInSeq            = false;
        };
    }
    else
    {
        for (int32 i = 0; i < m_numInterceptCtx; ++i)
        {
            if (m_seqViewObj.Check(m_aInterceptCtx[i].label).IsStarted())
            {
                m_pCurrentIntercept = &m_aInterceptCtx[i];
                m_bInSeq            = true;

                OnStart(m_pCurrentIntercept->label);

                break;
            };
        };
    };
};


void CEnemyTestPlayerAI::Draw(void) const
{
    ;
};


void CEnemyTestPlayerAI::AddIntercept(int32 label)
{
    ASSERT(m_numInterceptCtx < COUNT_OF(m_aInterceptCtx));

    m_aInterceptCtx[m_numInterceptCtx++] = { label };
};


void CEnemyTestPlayerAI::OnStart(int32 label)
{
    PLAYERID::VALUE aPlayerIdAlloc[] =
    {
        PLAYERID::ID_INVALID,
        PLAYERID::ID_INVALID,
        PLAYERID::ID_INVALID,
        PLAYERID::ID_INVALID,
    };

    PLAYERID::VALUE aPlayerIdFree[] =
    {
        PLAYERID::ID_LEO,
        PLAYERID::ID_RAP,
        PLAYERID::ID_MIC,
        PLAYERID::ID_DON,
    };

    static_assert(COUNT_OF(aPlayerIdAlloc) == GAMETYPES::PLAYERS_MAX, "update me");
    static_assert(COUNT_OF(aPlayerIdFree) == GAMETYPES::PLAYERS_MAX, "update me");

    int32 numPlayers = CGameData::PlayParam().GetPlayerNum();

    /* save active player ctx and init player id */
    m_numPlayerCtxSave = numPlayers;
    for (int32 i = 0; i < m_numPlayerCtxSave; ++i)
    {
        m_aPlayerCtxSave[i] = CGameData::PlayParam().PlayerContext(i);

        for (int32 j = 0; j < COUNT_OF(aPlayerIdFree); ++j)
        {
            if (CGameData::PlayParam().PlayerContext(i).m_CharacterID == aPlayerIdFree[j])
            {
                aPlayerIdAlloc[i] = aPlayerIdFree[j];
                aPlayerIdFree[j] = PLAYERID::ID_INVALID;
                break;
            };
        };
    };

    /* init players params */
    CGameData::PlayParam().ClearPlayer();

    /* init active players */
    for (int32 i = 0; i < numPlayers; ++i)
    {
        ASSERT(aPlayerIdAlloc[i] != PLAYERID::ID_INVALID);
        CGameData::PlayParam().AddPlayerCharacter(i, aPlayerIdAlloc[i]);
    };

    /* init inactive (AI) players */
    int32 numInactivePlayers = GAMETYPES::PLAYERS_MAX - CController::Max();
    for (int32 i = numPlayers; i < GAMETYPES::PLAYERS_MAX; ++i)
    {
        PLAYERID::VALUE playerId = PLAYERID::ID_INVALID;

        for (int32 j = 0; j < COUNT_OF(aPlayerIdFree); ++j)
        {
            if (aPlayerIdFree[j] != PLAYERID::ID_INVALID)
            {
                playerId = aPlayerIdFree[j];
                aPlayerIdFree[j] = PLAYERID::ID_INVALID;
				break;
            };
        };
        
        ASSERT(playerId != PLAYERID::ID_INVALID);
        CGameData::PlayParam().AddPlayerCharacter(i, playerId);
    };

    CGameData::PlayParam().InitForArea();
};


void CEnemyTestPlayerAI::OnEnd(int32 label)
{
    for (int32 i = 0; i < m_numPlayerCtxSave; ++i)
        CGameData::PlayParam().PlayerContext(i) = m_aPlayerCtxSave[i];
};


void EnemyTestPlayerAILaunch(class CProcess* pSender)
{
    if (!pSender->Info().IsProcessExist(PROCLABEL_ENEMYTESTPAI))
        pSender->Mail().Send(PROCLABEL_ENEMYTESTPAI, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


void EnemyTestPlayerAIKill(class CProcess* pSender)
{
    if (pSender->Info().IsProcessExist(PROCLABEL_ENEMYTESTPAI))
        pSender->Mail().Send(PROCLABEL_ENEMYTESTPAI, PROCESSTYPES::MAIL::TYPE_DETACH);
};


class CProcess* EnemyTestPlayerAIInstance(void)
{
    return new CEnemyTestPlayerAI;
};


#endif /* _DEBUG */