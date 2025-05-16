#pragma once

#include "Module.hpp"


class CModel;
class CAccumulate;
class CCharacter;


class CAccumulateUnit : public CListNode<CAccumulateUnit>
{
public:
    CAccumulateUnit(void);
    ~CAccumulateUnit(void);
    void Run(void);
    void Draw(void);
    void Initialize(CModel* pModel, int32 nBoneID, const RwV3d* pvTop, const RwV3d* pvBottom);
    void SetRadius(float fRadius);
    void SetColor(const RwRGBA& color);

private:
    CAccumulate* m_pAccumulate;
    float m_fRadius;
    float m_fTime;
    bool m_bInitialized;
};


class CAccumulateModule : public IModule
{
public:
    static CAccumulateModule* New(CCharacter* pCharacter);
    static void DrawAllAccumulate(void);
    
    CAccumulateModule(CCharacter* pCharacter);
    virtual ~CAccumulateModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void DrawAccumulate(void);
    void SetDrawOn(void);
    void SetDrawOff(void);
    void SetEffectOff(void);
    void SetStepZero(void);
    void SetStepOne(void);
    void SetStepTwo(void);
    void SetStepThree(void);

protected:
    void RegistUnit(CAccumulateUnit* pAccumulateUnit);
    void EffectOff(uint32& hEffect);
    void EffectSetPosition(uint32 hEffect, const RwV3d* pvPosition);

protected:
    static CList<CAccumulateModule> m_listDraw;
    CCharacter* m_pCharacter;
    uint32 m_hEffectSmall;
    uint32 m_hEffectMedium;
    uint32 m_hEffectLarge;
    CListNode<CAccumulateModule> m_nodeDraw;
    CList<CAccumulateUnit> m_listAccumulate;
    bool m_bDraw;
};


class CLeonardoAccumulateModule : public CAccumulateModule
{
public:
    CLeonardoAccumulateModule(CCharacter* pCharacter);
    virtual ~CLeonardoAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[2];
};


class CRaphaelAccumulateModule : public CAccumulateModule
{
public:
    CRaphaelAccumulateModule(CCharacter* pCharacter);
    virtual ~CRaphaelAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[2];
};


class CMichelangeroAccumulateModule : public CAccumulateModule
{
public:
    CMichelangeroAccumulateModule(CCharacter* pCharacter);
    virtual ~CMichelangeroAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[4];
};


class CDonatelloAccumulateModule : public CAccumulateModule
{
public:
    CDonatelloAccumulateModule(CCharacter* pCharacter);
    virtual ~CDonatelloAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[1];
};


class CSlashuurAccumulateModule : public CAccumulateModule
{
public:
    CSlashuurAccumulateModule(CCharacter* pCharacter);
    virtual ~CSlashuurAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[5];
};


class CCaseyAccumulateModule : public CAccumulateModule
{
public:
    CCaseyAccumulateModule(CCharacter* pCharacter);
    virtual ~CCaseyAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[2];
};


class CKaraiAccumulateModule : public CAccumulateModule
{
public:
    CKaraiAccumulateModule(CCharacter* pCharacter);
    virtual ~CKaraiAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[2];
};


class CSplinterAccumulateModule : public CAccumulateModule
{
public:
    CSplinterAccumulateModule(CCharacter* pCharacter);
    virtual ~CSplinterAccumulateModule(void) {};

private:
    CAccumulateUnit m_aAccumulateUnit[2];
};


class CEnemySlashuurAccumulateModule : public CAccumulateModule
{
private:
    enum STEP
    {
        STEP_IDLE = 0,
        STEP_FADE_IN,
        STEP_FADE_OUT,
    };

public:
    CEnemySlashuurAccumulateModule(CCharacter* pCharacter);
    virtual ~CEnemySlashuurAccumulateModule(void) {};
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void Fade(float fFadeTiming);
    void Fade(void);
    void SetColor(const RwRGBA& color);
    void SetColor(const RwRGBA* color);

private:
    CAccumulateUnit m_aAccumulateUnit[5];
    RwRGBA m_color;
    float m_fFadeTiming;
    float m_fNowTime;
    STEP  m_step;
};