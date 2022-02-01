#pragma once

#include "SecretID.hpp"
#include "AreaID.hpp"
#include "AntiqueID.hpp"
#include "WorldID.hpp"
#include "StageID.hpp"
#include "DatabaseItemID.hpp"


class CSecretInfo
{
private:
    struct PASSWORDINFO;
    
    static const PASSWORDINFO m_aPasswordList[];
    
public:
    static void CheckStartArea(AREAID::VALUE idArea);
    static void CheckEndArea(AREAID::VALUE idArea);
    static void CheckStartStage(STAGEID::VALUE idStage);
    static void CheckEndStage(STAGEID::VALUE idStage);
    static void CheckEnding(void);
    static void CheckAntiqueSequence(void);
    static void CheckGetBonusMaterials(AREAID::VALUE idArea);
    static void CheckStagePlay(STAGEID::VALUE idStage);
    static void CheckStageClear(STAGEID::VALUE idStage);
    static void CheckAntique(void);
    static void CheckAntiqueRoomOut(void);
    static void CheckNormalResult(AREAID::VALUE idArea);
    static void CheckAreaClearTerms(AREAID::VALUE idArea);
    static void CheckEndingResult(void);
    static void CheckTimeRelease(void);
    static void CheckGift(AREAID::VALUE idArea);
    static void CheckCrystal(void);
    static void CheckDatabase(void);
    static int32 GetGameText(SECRETID::VALUE idSecret);
    static DBITEMID::VALUE GetDatabase(SECRETID::VALUE idSecret);
    static SECRETID::VALUE CheckPassword(const char* pszPassword);
    static const char* GetPassword(SECRETID::VALUE idSecret);
    static void Unlock(SECRETID::VALUE idSecret);
    static void AnalyzeAntique(ANTIQUEID::VALUE idAntique);
    static bool IsCompleteAntique(void);
    static bool IsCompleteAntiqueAtNewYork(void);
    static bool IsCompleteAntiqueAtJapan(void);
    static bool IsCompleteAntiqueAtKuraiyama(void);

private:
    static bool isCompleteAntiqueAtWorld(WORLDID::VALUE idWorld);
};