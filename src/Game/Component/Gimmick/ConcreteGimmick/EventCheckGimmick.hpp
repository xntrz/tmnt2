#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CSimpleEventCheckGimmick : public CGimmick
{
protected:
    enum PRIVATESTATE
    {
        PRIVATESTATE_CHECK = 0,
        PRIVATESTATE_END,
    };
    
public:
    CSimpleEventCheckGimmick(const char* pszName, void* pParam);
    virtual ~CSimpleEventCheckGimmick(void);
    virtual void PostMove(void) override;
    virtual bool CheckEvent(void) = 0;
    void Init(const char* pszTarget);

protected:
    PRIVATESTATE m_privatestate;
    char m_szTargetName[GAMEOBJECTTYPES::NAME_MAX];
};


class CTimeCheckGimmick : public CSimpleEventCheckGimmick
{
public:
    CTimeCheckGimmick(const char* pszName, void* pParam);
    virtual ~CTimeCheckGimmick(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    virtual bool CheckEvent(void) override;
    float GetRemainTime(void) const;
    float GetTargetTime(void) const;

protected:
    float m_fTime;
    float m_fTargetTime;
    bool m_bIsCounting;
};


class CEnemyCheckGimmick : public CSimpleEventCheckGimmick
{
protected:
    enum MODE
    {
        MODE_DEFEAT_ALL = 0,
        MODE_DEFEAT_COUNT,
    };

public:
    CEnemyCheckGimmick(const char* pszName, void* pParam);
    virtual ~CEnemyCheckGimmick(void);
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual bool CheckEvent(void) override;

protected:
    MODE m_mode;
    int32 m_nTargetDefeatNum;
    char m_szGeneratorName[GAMEOBJECTTYPES::NAME_MAX];
    bool m_bCheckCompleteFlag;
};


class CAreaCheckGimmick : public CGimmick
{
protected:
    enum AREATYPE
    {
        AREATYPE_WALL = 0,
        AREATYPE_SPHERE,
        AREATYPE_BOX,
    };

    enum NUMBERTYPE
    {
        NUMBERTYPE_SOMEONE = 0,
        NUMBERTYPE_ALL,
    };

public:
    CAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CAreaCheckGimmick(void);
    virtual void Draw(void) const override;
    virtual void PostMove(void) override;
    virtual bool Query(CGimmickQuery* pQuery) const override;
    virtual bool CheckArea(void) = 0;
    bool IsInsideArea(RwV3d* pvPosition);
    void SetAreaParameter(AREATYPE areatype, void* pParam);

protected:
    bool isInsideSphere(RwV3d* pvPosition);
    bool isInsideBox(RwV3d* pvPosition);
    bool isReverseSideWall(RwV3d* pvPosition);
    static void getPlaneNormal(RwV3d* pvV0, RwV3d* pvV1, RwV3d* pvV2, RwV3d* pvNormal);
    static bool isPointReverseSidePlane(RwV3d* pvPos, RwV3d* pvPlanePos, RwV3d* pvNormal);

protected:
    char m_szTargetGimmickName[GAMEOBJECTTYPES::NAME_MAX];
    AREATYPE m_areatype;
    NUMBERTYPE m_numbertype;
    bool m_bInsideAreaSomeone;
    bool m_bInsideAreaAll;
    RwSphere m_sphere;
    RwV3d m_aWallVertex[4];
    RwV3d m_aBoxVertex[8];
};


class CPlayerAreaCheckGimmick : public CAreaCheckGimmick
{
protected:
    enum SUBID
    {
        SUBID_TRIGGER,
        SUBID_TRIGGER_ALL,
        SUBID_SWITCH,
        SUBID_SWITCH_ALL,
    };

public:
    CPlayerAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CPlayerAreaCheckGimmick(void);
    virtual bool CheckArea(void) override;

protected:
    bool m_bCheckAllFlag;
};


class CEnemyAreaCheckGimmick : public CAreaCheckGimmick
{
public:
    CEnemyAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CEnemyAreaCheckGimmick(void);
    virtual bool CheckArea(void) override;
};


class CGimmickObjAreaCheckGimmick : public CAreaCheckGimmick
{
public:
    CGimmickObjAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CGimmickObjAreaCheckGimmick(void);
    virtual bool CheckArea(void) override;

private:
    GIMMICKID::VALUE m_idTarget;
};


class CFugitoidAreaCheckGimmick : public CAreaCheckGimmick
{
public:
    CFugitoidAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CFugitoidAreaCheckGimmick(void);
    virtual bool CheckArea(void) override;
};


class CBrokenCheckGimmick : public CSimpleEventCheckGimmick
{
public:
    CBrokenCheckGimmick(const char* pszName, void* pParam);
    virtual ~CBrokenCheckGimmick(void);
    virtual bool CheckEvent(void) override;

private:
    char m_szObserveTarget[GAMEOBJECTTYPES::NAME_MAX];
};


class CUtromAreaCheckGimmick : public CGimmick
{
public:
    CUtromAreaCheckGimmick(const char* pszName, void* pParam);
    virtual ~CUtromAreaCheckGimmick(void);
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;
    void AreaCheckInit(void);
    void AreaCheckOn(void);
};


class CDonLaserGetCheckGimmick : public CSimpleEventCheckGimmick
{
public:
    CDonLaserGetCheckGimmick(const char* pszName, void* pParam);
    virtual ~CDonLaserGetCheckGimmick(void);
    virtual bool CheckEvent(void) override;
};