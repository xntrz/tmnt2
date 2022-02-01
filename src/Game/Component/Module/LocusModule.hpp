#pragma once

#include "Module.hpp"


class CModel;
class CLocus;
class CCharacter;


class CLocusModule : public IModule
{
public:
    struct LOCUSPARAMETER
    {
        int32 m_nBoneID;
        RwRGBA m_Color;
        RwV3d m_vPositionHigh;
        RwV3d m_vPositionLow;
    };

    struct LOCUSMODULEPARAMETER : public LOCUSPARAMETER
    {
        bool m_bMove;
        bool m_bTrace;
    };

    static const int32 LOCUS_NUM_MAX = 5;

public:
    CLocusModule(CModel* pModel, LOCUSPARAMETER* pLocusParameter, int32 nNumLocus);
    virtual ~CLocusModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void StartIndexedLocus(int32 nIndex);
    void StopIndexedLocus(int32 nIndex);
    void Reset(void);
    void DrawLocus(void);

protected:
    CModel* m_pModel;
    CLocus* m_apLocus[LOCUS_NUM_MAX];
    LOCUSMODULEPARAMETER m_aLocusParameter[LOCUS_NUM_MAX];
    int32 m_nNumLocus;
    RwV3d m_vPositionHigh;
    RwV3d m_vPositionLow;
};


class CLocusModuleForCharacter : public CLocusModule
{
public:
    static CLocusModuleForCharacter* New(CCharacter* pCharacter);
    
    CLocusModuleForCharacter(CCharacter* pCharacter, LOCUSPARAMETER* pLocusParameter, int32 nNumLocus);
    virtual ~CLocusModuleForCharacter(void);
    virtual void Run(void) override;

protected:
    CCharacter* m_pCharacter;
};


class CLocusModuleManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Draw(void);
    static void Run(void);
    static void Regist(CLocusModule* pLocusModule);
    static void Remove(CLocusModule* pLocusModule);
};