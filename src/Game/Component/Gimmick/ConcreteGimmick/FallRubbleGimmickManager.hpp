#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"


class CPlayerCharacter;


class CFallRubbleGimmickManager final : public CGimmick
{
public:
    enum TYPE
    {
        TYPE_M58OB = 0,
        TYPE_HUN,
        TYPE_HUN2,

        TYPENUM,
    };

    enum STATE
    {
        STATE_EXEC = 0,
        STATE_REST,
        STATE_END,
    };

    struct INITPARAM
    {
        float m_fTime;
        int32 m_nCreate;
        float m_fBaseHeight;
        float m_fRandom;
        float m_fBodyRadius;
        TYPE m_type;
    };
    
    struct RUBBLEINFO
    {
        float m_fTimer;
        bool m_bUse;
        uint32 m_hObj;
        RwMatrix m_matrix;
        STATE m_state;
    };

    struct RUBBLEPARAM
    {
        int32 m_nDamage;
        int32 m_nRubbleNum;
        float m_fInterval;
        float m_fBaseHeight;
        float m_fRandomHeight;
        float m_fBodySphereRadius;
    };

public:
    CFallRubbleGimmickManager(const char* pszName, void* pParam);
    virtual ~CFallRubbleGimmickManager(void);
    virtual void Run(void) override;   
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    
private:
    void rubbleGrandControl(void);
    void createRubble(void);
    void rubbleControl(RUBBLEINFO* pInfo);
    bool canRubbleCreate(CPlayerCharacter* pPlayerChara);
    void initRubbleInfo(CGameObject* pObj, RUBBLEINFO* pInfo);
    bool isExistSameHandleInWork(uint32 hObj);
    void createRubbleGimmick(RUBBLEINFO* pInfo, int32 nCount);
    void getRandomRubblePos(RwV3d* pvRet, RUBBLEINFO* pInfo);

private:
    RUBBLEINFO m_aRubbleInfo[GAMETYPES::PLAYERS_MAX];
    int32 m_nTotalCreatedNum;
    float m_fTimerUntilFall;
    int32 m_nCreateSameTime;
    TYPE m_type;
    int32 m_nRubbleDamage;
    float m_fRubbleBaseHeight;
    float m_fRubbleRandHeight;
    float m_fRubbleBodySphereRadius;
};