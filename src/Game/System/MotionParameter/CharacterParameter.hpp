#pragma once

#include "HitSphereParameter.hpp"
#include "LocusParameter.hpp"
#include "MotionParameter.hpp"


class CCharacterParameter
{
public:
    struct INIT_PARAMETER
    {
        char m_szName[16];
        float m_fRadius;
        float m_fHeight;
    };

public:
    CCharacterParameter(INIT_PARAMETER* pParam = nullptr);
    virtual ~CCharacterParameter(void);
    void Cleanup(void);
    void Initialize(INIT_PARAMETER* pParam);
    const CMotionParameterController::COLLISIONPARAMETER* GetCollisionParameter(void) const;
    void CreatePosition(CHitSphereParameter::INIT_PARAMETER* pParam);
    CHitSphereParameter* GetPositionParameter(int32 no);
    void CreateHitBody(CHitSphereParameter::INIT_PARAMETER* pParam);
    CHitSphereParameter* GetHitBodyParameter(int32 no);
    int32 GetHitBodyNum(void) const;
    void CreateHitCatch(CHitSphereParameter::INIT_PARAMETER* pParam);
    CHitSphereParameter* GetHitCatchParameter(int32 no);
    int32 GetHitCatchNum(void) const;
    void CreateMotion(CMotionParameter::INIT_PARAMETER* pParam);
    const char* GetMotionName(int32 no);
    CMotionParameter* GetMotion(const char* pszMotion);
    int32 GetMotionNum(void) const;
    void SetCurrentMotion(CMotionParameter* pMotion);
    void CreateAttack(CAttackParameter::INIT_PARAMETER* pParam);
    void AttachAttack(const char* pszMotionName, int32 no, CAttackParameter::ATTACH_PARAMETER* pParam);
    void CreateChainMotion(CChainMotionParameter::INIT_PARAMETER* pParam);
    void AttachChainMotion(CChainMotionParameter::ATTACH_PARAMETER* pParam);
    void CreateTiming(CTimingParameter::INIT_PARAMETER* pParam);
    void CreateLocus(CLocusParameter::INIT_PARAMETER* pParam);
    void GetLocusParamter(int32 no, CMotionParameterController::LOCUSPARAMETER* pParam);
    int32 GetLocusNum(void) const;
    void SetExistOffAtomic(bool bExist);
    void SetDefaultOffAtomicNum(int32 nAtomicNum);
    void SetDefaultOffAtomicNo(int32 nIndex, int32 nAtomicNo);
    bool IsDrawAtomic(int32 nAtomicNo) const;
    void SetReverseAtomicNo(bool bSet);
    bool IsReverseAtomicNo(void) const;
    void SetAtomicNum(int32 nAtomicNum);
    int32 GetAtomicNum(void) const;
    void DestroyHitList(CList<CHitSphereParameter>& rList);
    void DestroyLocusList(CList<CLocusParameter>& rList);
    void DestroyMotionList(CList<CMotionParameter>& rList);
    const char* GetName(void) const;

public:
    char m_szName[16];
    CMotionParameterController::COLLISIONPARAMETER m_collisionparameter;
    CList<CHitSphereParameter> m_listPos;
    int32 m_nBody;
    CList<CHitSphereParameter> m_listBody;
    int32 m_nCatch;
    CList<CHitSphereParameter> m_listCatch;
    int32 m_nMotion;
    CList<CMotionParameter> m_listMotion;
    CMotionParameter* m_pCurrentMotion;
    int32 m_nLocus;
    CList<CLocusParameter> m_listLocus;
    bool m_bOffAtomic;
    int32 m_nOffAtomic;
    int32* m_paOffAtomic;
    bool m_bReverseAtomicNo;
    int32 m_nAtomicNum;
};