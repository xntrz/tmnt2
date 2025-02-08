#include "DbgUnlockProcess.hpp"

#ifdef _DEBUG

#include "DebugUtils.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Process/ProcessMail.hpp"


/*static*/ CProcess* CDbgUnlockProcess::Instance(void)
{
    return new CDbgUnlockProcess;
};


/*static*/ void CDbgUnlockProcess::Launch(CProcess* pCaller, uint32 unlockFlag)
{
    if (pCaller->Mail().Send(PROCLABEL_DBGUNLOCK, PROCESSTYPES::MAIL::TYPE_ATTACH))
        pCaller->Mail().Send(PROCLABEL_DBGUNLOCK, PROCESSTYPES::MAIL::TYPE_MSG, reinterpret_cast<const void*>(unlockFlag));
};


bool CDbgUnlockProcess::Attach(void)
{
    m_unlockFlag = 0;

    PROCESSTYPES::MAIL mail;
    while (Mail().Recv(mail))
        m_unlockFlag = reinterpret_cast<uint32>(mail.m_param);

    return true;
};


void CDbgUnlockProcess::Move(void)
{
    //
    //  activate unlock only when saveload check seq ended
    //
    static CDebugSequenceCheckObj s_seqViewObj;

    s_seqViewObj.Update();

    if (!s_seqViewObj.Check(PROCLABEL_SEQ_SAVELOADCHECK).IsEnded())
        return;

    //
    //  unlock all path and area to SS rank
    //
    if (m_unlockFlag & UNLOCKFLAG_AREA)
    {
        OUTPUT("UNLOCKING AREA\n");

		CGameData::ClearNewGameFlag();

        CGameData::Record().Area().SetCurrentSelectedArea(AREAID::HOME);
        for (int32 i = AREAID::SELECTABLESTART; i < AREAID::SELECTABLEMAX; ++i)
        {
            CGameData::Record().Area().SetAreaOpened(AREAID::VALUE(i));
            CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_A);
            CGameData::Record().Area().SetAreaCleared(AREAID::VALUE(i), CAreaRecord::CLEAR_ROOT_B);
            CGameData::Record().Area().SetAreaRank(AREAID::VALUE(i), GAMETYPES::CLEARRANK_SS);

            CGameTime cleartime(60 * 5); // 5 min
            CGameData::Record().Area().UpdateAreaClearTime(AREAID::VALUE(i), cleartime);
        };
    };

    //
    //  unlock all secrets
    //
    if (m_unlockFlag & UNLOCKFLAG_CHARA)
    {
        OUTPUT("UNLOCKING CHARACTER\n");

        bool prev = CSecretRecord::m_bDebugNotifyUnlocked;
        CSecretRecord::m_bDebugNotifyUnlocked = false;

        for (int32 i = SECRETID::CHARACTERSTART; i < SECRETID::CHARACTERMAX; ++i)
        {
            if (!CGameData::Record().Secret().IsUnlockedSecret(SECRETID::VALUE(i)))
                CGameData::Record().Secret().UnlockSecret(SECRETID::VALUE(i));
        };

        OUTPUT("UNLOCKING HOME\n");

        for (int32 i = SECRETID::HOMESTART; i < SECRETID::HOMEMAX; ++i)
        {
            if (!CGameData::Record().Secret().IsUnlockedSecret(SECRETID::VALUE(i)))
                CGameData::Record().Secret().UnlockSecret(SECRETID::VALUE(i));
        };

        CSecretRecord::m_bDebugNotifyUnlocked = prev;
    };

    //
    //  set all crystal as taken
    //
    if (m_unlockFlag & UNLOCKFLAG_CRYSTAL)
    {
        OUTPUT("UNLOCKING CRYSTAL\n");

        CGameData::Record().Item().ForceSetCrystalNum(GAMETYPES::CRYSTAL_MAX);
    };

    //
    //  open all tourney
    //
    if (m_unlockFlag & UNLOCKFLAG_TOURNEY)
    {
        OUTPUT("UNLOCKING TOURNAMENT\n");

        CGameData::Record().Nexus().SetNexusState(CNexusRecord::NEXUSSTATE_OPEN);
        for (int32 i = 0; i < GAMETYPES::NEXUSID_NUM; ++i)
        {
            CGameData::Record().Nexus().SetTournamentState(GAMETYPES::NEXUSID(i), CNexusRecord::STATE_CLEAR);
            for (int32 j = 0; j < GAMETYPES::STAGE_MAX; ++j)
            {
                CGameData::Record().Nexus().SetStagePlayed(GAMETYPES::NEXUSID(i), j);

                CGameTime cleartime(60 * 5); // 5 min
                CGameData::Record().Nexus().UpdateStageClearTime(GAMETYPES::NEXUSID(i), j, cleartime);
            };
        };
    };

    //
    //  unlock all antiques in april shop
    //
    if (m_unlockFlag & UNLOCKFLAG_APRIL)
    {
        OUTPUT("UNLOCKING ANTIQUE\n");

        CGameData::Record().Antique().SetShopState(CAntiqueRecord::SHOPSTATE_OPEN);
        for (int32 i = ANTIQUEID::ID_FIRST; i < ANTIQUEID::ID_MAX; ++i)
        {
            if (!CGameData::Record().Antique().IsAntiqueTaken(ANTIQUEID::VALUE(i)))
                CGameData::Record().Antique().SetAntiqueTaken(ANTIQUEID::VALUE(i));
        };
    };

    //
    //  unlock all items in database
    //
    if (m_unlockFlag & UNLOCKFLAG_DB)
    {
        OUTPUT("UNLOCKING DATABASE\n");

        CGameData::Record().Database().SetDatabaseState(CDatabaseRecord::DBSTATE_OPEN);
        for (int32 i = DBITEMID::ID_FIRST; i < DBITEMID::ID_MAX; ++i)
        {
            if (!CGameData::Record().Database().IsItemUnlocked(DBITEMID::VALUE(i)))
                CGameData::Record().Database().SetItemUnlocked(DBITEMID::VALUE(i));
        };
    };

    //
    //  terminate current process
    //
    Mail().Send(PROCLABEL_DBGUNLOCK, PROCESSTYPES::MAIL::TYPE_DETACH);
};


#endif /* _DEBUG */