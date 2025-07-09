#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CAndSwitchGimmick : public CGimmick
{
protected:
    enum PRIVATESTATE
    {
        PRIVATESTATE_CHECK = 0,
        PRIVATESTATE_END,
    };
    
public:
    CAndSwitchGimmick(const char* pszName, void* pParam);
    virtual ~CAndSwitchGimmick(void);
    virtual void PostMove(void) override;
    bool checkSwitch(void);

protected:
    PRIVATESTATE m_privatestate;
    char         m_szEventGimmick[GAMEOBJECTTYPES::NAME_MAX];
    char         m_szTargetGimmick[GAMEOBJECTTYPES::NAME_MAX];
};


class CGroupGimmick : public CGimmick
{
protected:
    struct CHILDNODE
    {
        void Init(const char* pszName);
        const char* GetName(void) const;
        CGimmick* GetGimmick(void) const;

        char m_szName[GAMEOBJECTTYPES::NAME_MAX];
    };

public:
    CGroupGimmick(const char* pszName, void* pParam);
    virtual ~CGroupGimmick(void);
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    CGimmick* GetChild(int32 no) const;
    CHILDNODE* getChildNode(int32 no) const;

protected:
    CHILDNODE*  m_pChildArray;
    int32       m_nChildNum;
};


class CGameEndGimmick : public CGimmick
{
public:
    enum MODE
    {
        MODE_GAMECLEAR = 0,
        MODE_GAMEOVER,
    };

public:
    CGameEndGimmick(const char* pszName, void* pParam);
    virtual ~CGameEndGimmick(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    MODE    m_mode;
    int32   m_clearsub;
};


class CCameraChangeGimmick : public CGimmick
{
protected:
    enum STEP
    {
        STEP_IDLE = 0,
        STEP_FADEOUT,
        STEP_FADEOUT_CHECK,
        STEP_REPLACE,
        STEP_FADEIN,
        STEP_FADEIN_CHECK,
        STEP_RELEASE,
    };

public:
    CCameraChangeGimmick(const char* pszName, void* pParam);
    virtual ~CCameraChangeGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void ReplacePlayers(void);
    
protected:
    STEP    m_step;
    RwV3d   m_vPosition;
    RwV3d   m_vRotation;
    float   m_fRotationY;
    char    m_szSenderGimmick[GAMEOBJECTTYPES::NAME_MAX];
};


class CSEGimmick : public CGimmick
{
protected:
    struct SEINFO
    {
        enum FLAG
        {
            FLAG_NONE = 0x0,
            FLAG_FADE = 0x1,
            FLAG_LOOP = 0x2,
        };

        int32  m_nSE;
        uint32 m_flag;
        float  m_fDuration;
    };

    enum SUBID
    {
        SUBID_PLAY_TRIGGER = 0,
        SUBID_PLAY_INSTANT,
    };

public:
    CSEGimmick(const char* pszName, void* pParam);
	virtual ~CSEGimmick(void);
	virtual void GetPosition(RwV3d* pvPosition) const;
	virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void startSE(void);
    void stopSE(void);

protected:
    static const SEINFO m_aSeInfoList[];
    bool                m_bPlaying;
    RwV3d               m_vPosition;
    const SEINFO*       m_pNowSEInfo;
    float               m_fTimer;
};


class CInvisibleWallGimmick : public CGimmick
{
protected:
    enum TYPE
    {
        TYPE_5M_DISAPPEAR = 0,
        TYPE_5M_APPEAR,
        TYPE_20M_DISAPPEAR,
        TYPE_20M_APPEAR,

        TYPENUM,
    };

public:
    CInvisibleWallGimmick(const char* pszName, void* pParam);
    virtual ~CInvisibleWallGimmick(void);
    virtual void Draw(void) const override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void initCollision(void);
    void termCollision(void);

protected:
    TYPE                m_type;
    CNormalGimmickModel m_model;
    uint32              m_hAtari;
    bool                m_bEnable;
};


class CReplaceGimmick : public CGimmick
{
protected:
    enum SUBID
    {
        SUBID_FALL_REPLACE = 0,
        SUBID_START_REPLACE,
    };

public:
    static void ReplacePlayer(int32 nPlayerNo, const RwV3d* pvPosition, bool bBlink);
    static void SetPlayerStartPosition(int32 nPlayerNo, const RwV3d* pvPosition, bool bBlink);
    static void ReplacePlayer(const char* pszSearchGimmickName, int32 nPlayerNo, const RwV3d* pvPosition, bool bBlink);
    
    CReplaceGimmick(const char* pszName, void* pParam);
    virtual ~CReplaceGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void OnReplacePlayer(int32 nPlayerNo, bool bBlink);
    bool IsEnableReplaceGimmick(void) const;
    void setAreaVertex(void);
    bool isAreaRectVertexInScreen(void) const;

protected:
    RwV3d m_vReplacePosition;
    RwV3d m_vReplaceRotation;
    RwV3d m_aAreaRectVertex[4];
    float m_fRotationY;
    bool  m_bEnable;
    bool  m_bStartPosition;
};


class CHelpGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_RUN,
        STATE_STOP,
    };

    struct INITPARAM
    {
        int32 m_nSeGroupID;
        bool  m_bExclusive;
    };

public:
    CHelpGimmick(const char* pszName, void* pParam);
    virtual ~CHelpGimmick(void);
    virtual void Run(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void start(void);
    void forceEnd(void);
    bool isSatisfyCrystalMessageCondition(void);
    void showMessage(void);

protected:
    static const INITPARAM  m_aInitParam[];
    STATE                   m_state;
    RwV3d                   m_vPosition;
    int32                   m_nSeGroupID;
    float                   m_fDelayTime;
    float                   m_fTime;
    bool                    m_bExclusive;
};


class CTutorialGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_IDLE = 0,
        STATE_OPENED,
        STATE_CLOSED,
        STATE_EOL,
    };

public:
    CTutorialGimmick(const char* pszName, void* pParam);
    virtual ~CTutorialGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    STATE m_state;
    int32 m_nTutoNo;
    bool  m_bDonLaserTutoEnable;
};


class CEnemyKillGimmick : public CGimmick
{
public:
    CEnemyKillGimmick(const char* pszName, void* pParam);
    virtual ~CEnemyKillGimmick(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    char m_szTargetGimmick[GAMEOBJECTTYPES::NAME_MAX];
};