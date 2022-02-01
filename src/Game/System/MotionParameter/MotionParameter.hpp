#pragma once

#include "MotionParameterController.hpp"
#include "AttackParameter.hpp"
#include "ChainMotionParameter.hpp"
#include "TimingParameter.hpp"


class CMotionParameter : public CListNode<CMotionParameter>
{
public:
    struct INIT_PARAMETER
    {
        char m_szName[16];
        int32 m_playmode;
        float m_fBlendTime;
    };
    
public:
    CMotionParameter(INIT_PARAMETER* pParam);
    virtual ~CMotionParameter(void);
    void Initialize(INIT_PARAMETER* pParam);
    void CreateAttackParameter(CAttackParameter::INIT_PARAMETER* pParam);
    void AttachAttackParameter(int32 no, CAttackParameter::ATTACH_PARAMETER* pParam);
    CAttackParameter* GetAttackParameter(int32 no);
    int32 GetNumAttackParameter(void) const;
    void CreateChainMotionParameter(CChainMotionParameter::INIT_PARAMETER* pParam);
    void AttachChainMotionParameter(CChainMotionParameter::ATTACH_PARAMETER* pParam);
    float GetNextChainMotionConnectTime(const char* pszNextMotion);
    void CreateTimingParameter(CTimingParameter::INIT_PARAMETER* pParam);
    void InsertTimingByTime(CList<CTimingParameter>& rList, CTimingParameter* pTiming);
    int32 GetNumDrawAtomicData(float fPrevTime, float fNowTime);
    void GetDrawAtomicData(int32 nIndex, CMotionParameterController::DRAW_ATOMIC* pData);
    int32 GetNumDrawLocusData(float fPrevTime, float fNowTime);
    void GetDrawLocusData(int32 nIndex, CMotionParameterController::DRAW_LOCUS* pData);
    bool IsAttackTiming(float fPrevTime, float fNowTime);
    bool IsInvincibleTime(float fNowTime);
    void DestroyListTiming(CList<CTimingParameter>& rList);
    void DestroyListAttack(CList<CAttackParameter>& rList);
    const char* GetName(void) const;
    float GetBlendTime(void) const;
    int32 GetPlaymode(void) const;

private:
    char m_szName[16];
    float m_fBlendTime;
    int32 m_playmode;
    int32 m_nAttack;
    CList<CAttackParameter> m_listAttackParameter;
    CChainMotionParameter* m_pChainMotionParameter;
    int32 m_nLocusTiming;
    CList<CTimingParameter> m_listLocusTimingParameter;
    int32 m_nAtomicTiming;
    CList<CTimingParameter> m_listAtomicTimingParameter;
    int32 m_nAttackTiming;
    CList<CTimingParameter> m_listAttackTimingParameter;
    int32 m_nInvincibleTiming;
    CList<CTimingParameter> m_listInvincibleTimingParameter;
    int32 m_nAtomicOffset;
    int32 m_nLocusOffset;
};